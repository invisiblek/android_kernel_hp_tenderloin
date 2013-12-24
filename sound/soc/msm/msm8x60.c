/* Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/msm_audio.h>
#include <linux/switch.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/pcm.h>
#include <sound/tlv.h>
#include <sound/initval.h>
#include <sound/control.h>
#include <sound/q6afe.h>
#include <sound/pcm.h>
#include <linux/input.h>
#include <asm/dma.h>
#include <asm/mach-types.h>
#include <mach/qdsp6v2/audio_dev_ctl.h>
#include <mach/qdsp6v2/q6voice.h>

#define LOOPBACK_ENABLE		0x1
#define LOOPBACK_DISABLE	0x0

#include "msm8x60-pcm.h"

#include <sound/jack.h>
#if defined(CONFIG_MACH_TENDERLOIN)
#include "../codecs/wm8994.h"
#include <linux/mfd/wm8994/registers.h>
#include <linux/mfd/wm8994/pdata.h>
#include <sound/pcm_params.h>

#define WM_FS 48000
#define WM_CHANNELS 2
#define WM_BITS 16
#define WM_FLL_MULT 8 /* 2*16*8 = 256, clock rates must be >= 256*fs */
#define WM_BCLK (WM_FS * WM_CHANNELS * WM_BITS) /* 1.536MHZ */
#define WM_FLL (WM_FLL_MULT * WM_BCLK) /* 12.288 MHZ */
#define WM_FLL_MIN_RATE 4096000 /* The minimum clk rate required for AIF's */
static int rx_hw_param_status;
static int tx_hw_param_status;

// name must match kernel command line argument
static uint hs_uart = 0;
static __init int set_hs_uart(char *str)
{
  get_option(&str, &hs_uart);
  return 0;
}
early_param("hs_uart", set_hs_uart);
#endif

static struct platform_device *msm_audio_snd_device;
struct audio_locks the_locks;
EXPORT_SYMBOL(the_locks);
struct msm_volume msm_vol_ctl;
EXPORT_SYMBOL(msm_vol_ctl);
struct pcm_session session_route;
EXPORT_SYMBOL(session_route);
static struct snd_kcontrol_new snd_msm_controls[];

char snddev_name[AUDIO_DEV_CTL_MAX_DEV][44];
#define MSM_MAX_VOLUME 0x2000
#define MSM_VOLUME_STEP ((MSM_MAX_VOLUME+17)/100) /* 17 added to avoid
						      more deviation */
static int device_index; /* Count of Device controls */
static int simple_control; /* Count of simple controls*/
static int src_dev;
static int dst_dev;
static int loopback_status;

static int msm_scontrol_count_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	return 0;
}

static int msm_scontrol_count_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = simple_control;
	return 0;
}

static int msm_v_call_info(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2; /* start, session_id */
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = SESSION_ID_BASE + MAX_VOC_SESSIONS;
	return 0;
}

static int msm_v_call_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	ucontrol->value.integer.value[1] = 0;
	return 0;
}

static int msm_v_call_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int start = ucontrol->value.integer.value[0];
	u32 session_id = ucontrol->value.integer.value[1];

	if ((session_id != 0) &&
	    ((session_id < SESSION_ID_BASE) ||
	     (session_id >= SESSION_ID_BASE + MAX_VOC_SESSIONS))) {
		pr_err("%s: Invalid session_id 0x%x\n", __func__, session_id);

		return -EINVAL;
	}

	if (start)
		broadcast_event(AUDDEV_EVT_START_VOICE, DEVICE_IGNORE,
							session_id);
	else
		broadcast_event(AUDDEV_EVT_END_VOICE, DEVICE_IGNORE,
							session_id);
	return 0;
}

static int msm_v_mute_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 3; /* dir, mute, session_id */
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = SESSION_ID_BASE + MAX_VOC_SESSIONS;
	return 0;
}

static int msm_v_mute_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	ucontrol->value.integer.value[1] = 0;
	ucontrol->value.integer.value[2] = 0;
	return 0;
}

static int msm_v_mute_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int dir = ucontrol->value.integer.value[0];
	int mute = ucontrol->value.integer.value[1];
	u32 session_id = ucontrol->value.integer.value[2];

	if ((session_id != 0) &&
	    ((session_id < SESSION_ID_BASE) ||
	     (session_id >= SESSION_ID_BASE + MAX_VOC_SESSIONS))) {
		pr_err("%s: Invalid session_id 0x%x\n", __func__, session_id);

		return -EINVAL;
	}

	return msm_set_voice_mute(dir, mute, session_id);
}

static int msm_v_volume_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 3; /* dir, volume, session_id */
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = SESSION_ID_BASE + MAX_VOC_SESSIONS;
	return 0;
}

static int msm_v_volume_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	ucontrol->value.integer.value[1] = 0;
	ucontrol->value.integer.value[2] = 0;
	return 0;
}

static int msm_v_volume_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int dir = ucontrol->value.integer.value[0];
	int volume = ucontrol->value.integer.value[1];
	u32 session_id = ucontrol->value.integer.value[2];

	if ((session_id != 0) &&
	    ((session_id < SESSION_ID_BASE) ||
	     (session_id >= SESSION_ID_BASE + MAX_VOC_SESSIONS))) {
		pr_err("%s: Invalid session_id 0x%x\n", __func__, session_id);

		return -EINVAL;
	}

	return msm_set_voice_vol(dir, volume, session_id);
}

static int msm_volume_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2; /* Volume */
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 16383;
	return 0;
}
static int msm_volume_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int msm_volume_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int ret = 0;
	int session_id = ucontrol->value.integer.value[0];
	int volume = ucontrol->value.integer.value[1];
	int factor = ucontrol->value.integer.value[2];
	u64 session_mask = 0;

	if (factor > 10000)
		return -EINVAL;

	if ((volume < 0) || (volume/factor > 100))
		return -EINVAL;

	volume = (MSM_VOLUME_STEP * volume);

	/* Convert back to original decimal point by removing the 10-base factor
	* and discard the fractional portion
	*/

	volume = volume/factor;

	if (volume > MSM_MAX_VOLUME)
		volume = MSM_MAX_VOLUME;

	/* Only Decoder volume control supported */
	session_mask = (((u64)0x1) << session_id) << (MAX_BIT_PER_CLIENT * \
				((int)AUDDEV_CLNT_DEC-1));
	msm_vol_ctl.volume = volume;
	pr_debug("%s:session_id %d, volume %d", __func__, session_id, volume);
	broadcast_event(AUDDEV_EVT_STREAM_VOL_CHG, DEVICE_IGNORE,
							session_mask);

	return ret;
}

static int msm_voice_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 3; /* Device */

	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = msm_snddev_devcount();
	return 0;
}

static int msm_voice_put(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	int rc = 0;
	uint32_t rx_dev_id;
	uint32_t tx_dev_id;
	struct msm_snddev_info *rx_dev_info;
	struct msm_snddev_info *tx_dev_info;
	int set = ucontrol->value.integer.value[2];
	u64 session_mask;

	if (!set)
		return -EPERM;
	/* Rx Device Routing */
	rx_dev_id = ucontrol->value.integer.value[0];
	rx_dev_info = audio_dev_ctrl_find_dev(rx_dev_id);

	if (IS_ERR(rx_dev_info)) {
		pr_err("%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(rx_dev_info);
		return rc;
	}

	if (!(rx_dev_info->capability & SNDDEV_CAP_RX)) {
		pr_err("%s:First Dev is supposed to be RX\n", __func__);
		return -EFAULT;
	}

	pr_debug("%s:route cfg %d STREAM_VOICE_RX type\n",
		__func__, rx_dev_id);

	msm_set_voc_route(rx_dev_info, AUDIO_ROUTE_STREAM_VOICE_RX,
				rx_dev_id);

	session_mask =	((u64)0x1) << (MAX_BIT_PER_CLIENT * \
				((int)AUDDEV_CLNT_VOC-1));

	broadcast_event(AUDDEV_EVT_DEV_CHG_VOICE, rx_dev_id, session_mask);


	/* Tx Device Routing */
	tx_dev_id = ucontrol->value.integer.value[1];
	tx_dev_info = audio_dev_ctrl_find_dev(tx_dev_id);

	if (IS_ERR(tx_dev_info)) {
		pr_err("%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(tx_dev_info);
		return rc;
	}

	if (!(tx_dev_info->capability & SNDDEV_CAP_TX)) {
		pr_err("%s:Second Dev is supposed to be Tx\n", __func__);
		return -EFAULT;
	}

	pr_debug("%s:route cfg %d %d type\n",
		__func__, tx_dev_id, AUDIO_ROUTE_STREAM_VOICE_TX);

	msm_set_voc_route(tx_dev_info, AUDIO_ROUTE_STREAM_VOICE_TX,
				tx_dev_id);

	broadcast_event(AUDDEV_EVT_DEV_CHG_VOICE, tx_dev_id, session_mask);

	if (rx_dev_info->opened)
		broadcast_event(AUDDEV_EVT_DEV_RDY, rx_dev_id,	session_mask);

	if (tx_dev_info->opened)
		broadcast_event(AUDDEV_EVT_DEV_RDY, tx_dev_id, session_mask);

	return rc;
}

static int msm_voice_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	/* TODO: query Device list */
	return 0;
}

static int msm_device_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1; /* Device */

	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = msm_snddev_devcount();
	return 0;
}

static int msm_device_put(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	int rc = 0;
	int set = 0;
	struct msm_audio_route_config route_cfg;
	struct msm_snddev_info *dev_info;
	struct msm_snddev_info *dst_dev_info;
	struct msm_snddev_info *src_dev_info;
	int tx_freq = 0;
	int rx_freq = 0;
	u32 set_freq = 0;

	set = ucontrol->value.integer.value[0];
	printk(KERN_ERR "%s: %d %d\n", __func__, ucontrol->id.numid, device_index);
	route_cfg.dev_id = ucontrol->id.numid - device_index;
	dev_info = audio_dev_ctrl_find_dev(route_cfg.dev_id);
	if (IS_ERR(dev_info)) {
		pr_err("%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(dev_info);
		return rc;
	}
	pr_info("%s:device %s set %d\n", __func__, dev_info->name, set);

	if (set) {
		if (!dev_info->opened) {
			set_freq = dev_info->sample_rate;
			if (!msm_device_is_voice(route_cfg.dev_id)) {
				msm_get_voc_freq(&tx_freq, &rx_freq);
				if (dev_info->capability & SNDDEV_CAP_TX)
					set_freq = tx_freq;

				if (set_freq == 0)
					set_freq = dev_info->sample_rate;
			} else
				set_freq = dev_info->sample_rate;


			pr_err("%s:device freq =%d\n", __func__, set_freq);
			rc = dev_info->dev_ops.set_freq(dev_info, set_freq);
			if (rc < 0) {
				pr_err("%s:device freq failed!\n", __func__);
				return rc;
			}
			dev_info->set_sample_rate = rc;
			rc = 0;
			rc = dev_info->dev_ops.open(dev_info);
			if (rc < 0) {
				pr_err("%s:Enabling %s failed\n",
					__func__, dev_info->name);
				return rc;
			}
			dev_info->opened = 1;
			broadcast_event(AUDDEV_EVT_DEV_RDY, route_cfg.dev_id,
							SESSION_IGNORE);
			if ((route_cfg.dev_id == src_dev) ||
				(route_cfg.dev_id == dst_dev)) {
				dst_dev_info = audio_dev_ctrl_find_dev(
							dst_dev);
				if (IS_ERR(dst_dev_info)) {
					pr_err("dst_dev:%s:pass invalid"
						"dev_id\n", __func__);
					rc = PTR_ERR(dst_dev_info);
					return rc;
				}
				src_dev_info = audio_dev_ctrl_find_dev(
							src_dev);
				if (IS_ERR(src_dev_info)) {
					pr_err("src_dev:%s:pass invalid"
						"dev_id\n", __func__);
					rc = PTR_ERR(src_dev_info);
					return rc;
				}
				if ((dst_dev_info->opened) &&
					(src_dev_info->opened)) {
					pr_debug("%d: Enable afe_loopback\n",
							__LINE__);
					afe_loopback(LOOPBACK_ENABLE,
					       dst_dev_info->copp_id,
					       src_dev_info->copp_id);
					loopback_status = 1;
				}
			}
		}
	} else {
		if (dev_info->opened) {
			broadcast_event(AUDDEV_EVT_REL_PENDING,
						route_cfg.dev_id,
						SESSION_IGNORE);
			rc = dev_info->dev_ops.close(dev_info);
			if (rc < 0) {
				pr_err("%s:Snd device failed close!\n",
					__func__);
				return rc;
			} else {
				dev_info->opened = 0;
				broadcast_event(AUDDEV_EVT_DEV_RLS,
					route_cfg.dev_id,
					SESSION_IGNORE);
			}
			if (loopback_status == 1) {
				if ((route_cfg.dev_id == src_dev) ||
					(route_cfg.dev_id == dst_dev)) {
					dst_dev_info = audio_dev_ctrl_find_dev(
								dst_dev);
					if (IS_ERR(dst_dev_info)) {
						pr_err("dst_dev:%s:pass invalid"
							"dev_id\n", __func__);
						rc = PTR_ERR(dst_dev_info);
						return rc;
					}
					src_dev_info = audio_dev_ctrl_find_dev(
								src_dev);
					if (IS_ERR(src_dev_info)) {
						pr_err("src_dev:%s:pass invalid"
							"dev_id\n", __func__);
						rc = PTR_ERR(src_dev_info);
						return rc;
					}
					pr_debug("%d: Disable afe_loopback\n",
						__LINE__);
					afe_loopback(LOOPBACK_DISABLE,
					       dst_dev_info->copp_id,
					       src_dev_info->copp_id);
					loopback_status = 0;
				}
			}
		}

	}
	return rc;
}

static int msm_device_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int rc = 0;
	struct msm_audio_route_config route_cfg;
	struct msm_snddev_info *dev_info;

	route_cfg.dev_id = ucontrol->id.numid - device_index;
	dev_info = audio_dev_ctrl_find_dev(route_cfg.dev_id);

	if (IS_ERR(dev_info)) {
		pr_err("%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(dev_info);
		return rc;
	}

	ucontrol->value.integer.value[0] = dev_info->copp_id;
	ucontrol->value.integer.value[1] = dev_info->capability;

	return 0;
}

static int msm_route_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 3; /* Device */

	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = msm_snddev_devcount();
	return 0;
}

static int msm_route_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	/* TODO: query Device list */
	return 0;
}

static int msm_route_put(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	int rc = 0;
	int enc_freq = 0;
	int requested_freq = 0;
	struct msm_audio_route_config route_cfg;
	struct msm_snddev_info *dev_info;
	int session_id = ucontrol->value.integer.value[0];
	int set = ucontrol->value.integer.value[2];
	u64 session_mask = 0;
	route_cfg.dev_id = ucontrol->value.integer.value[1];

	if (ucontrol->id.numid == 2)
		route_cfg.stream_type =	AUDIO_ROUTE_STREAM_PLAYBACK;
	else
		route_cfg.stream_type =	AUDIO_ROUTE_STREAM_REC;

	pr_debug("%s:route cfg %d %d type for popp %d\n",
		__func__, route_cfg.dev_id, route_cfg.stream_type, session_id);
	dev_info = audio_dev_ctrl_find_dev(route_cfg.dev_id);

	if (IS_ERR(dev_info)) {
		pr_err("%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(dev_info);
		return rc;
	}
	if (route_cfg.stream_type == AUDIO_ROUTE_STREAM_PLAYBACK) {
		rc = msm_snddev_set_dec(session_id, dev_info->copp_id, set,
				dev_info->sample_rate, dev_info->channel_mode);
		session_mask =
			(((u64)0x1) << session_id) << (MAX_BIT_PER_CLIENT * \
				((int)AUDDEV_CLNT_DEC-1));
		if (!set) {
			if (dev_info->opened)
				broadcast_event(AUDDEV_EVT_DEV_RLS,
							route_cfg.dev_id,
							session_mask);
			dev_info->sessions &= ~(session_mask);
		} else {
			dev_info->sessions = dev_info->sessions | session_mask;
			if (dev_info->opened)
				broadcast_event(AUDDEV_EVT_DEV_RDY,
							route_cfg.dev_id,
							session_mask);
		}
	} else {

		rc = msm_snddev_set_enc(session_id, dev_info->copp_id, set,
				dev_info->sample_rate, dev_info->channel_mode);
		session_mask =
			(((u64)0x1) << session_id) << (MAX_BIT_PER_CLIENT * \
			((int)AUDDEV_CLNT_ENC-1));
		if (!set) {
			if (dev_info->opened)
				broadcast_event(AUDDEV_EVT_DEV_RLS,
							route_cfg.dev_id,
							session_mask);
			dev_info->sessions &= ~(session_mask);
		} else {
			dev_info->sessions = dev_info->sessions | session_mask;
			enc_freq = msm_snddev_get_enc_freq(session_id);
			requested_freq = enc_freq;
			if (enc_freq > 0) {
				rc = msm_snddev_request_freq(&enc_freq,
						session_id,
						SNDDEV_CAP_TX,
						AUDDEV_CLNT_ENC);
				pr_debug("%s:sample rate configured %d\
					sample rate requested %d \n",
					__func__, enc_freq, requested_freq);
				if ((rc <= 0) || (enc_freq != requested_freq)) {
					pr_debug("%s:msm_snddev_withdraw_freq\n",
						__func__);
					rc = msm_snddev_withdraw_freq
						(session_id,
						SNDDEV_CAP_TX, AUDDEV_CLNT_ENC);
					broadcast_event(AUDDEV_EVT_FREQ_CHG,
							route_cfg.dev_id,
							SESSION_IGNORE);
				}
			}
			if (dev_info->opened)
				broadcast_event(AUDDEV_EVT_DEV_RDY,
							route_cfg.dev_id,
							session_mask);
		}
	}

	if (rc < 0) {
		pr_err("%s:device could not be assigned!\n", __func__);
		return -EFAULT;
	}

	return rc;
}

static int msm_device_volume_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 100;
	return 0;
}

static int msm_device_volume_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct msm_snddev_info *dev_info;

	int dev_id = ucontrol->value.integer.value[0];

	dev_info = audio_dev_ctrl_find_dev(dev_id);
	ucontrol->value.integer.value[0] = dev_info->dev_volume;

	return 0;
}

static int msm_device_volume_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int rc = -EPERM;
	struct msm_snddev_info *dev_info;

	int dev_id = ucontrol->value.integer.value[0];
	int volume = ucontrol->value.integer.value[1];

	pr_debug("%s:dev_id = %d, volume = %d\n", __func__, dev_id, volume);

	dev_info = audio_dev_ctrl_find_dev(dev_id);

	if (IS_ERR(dev_info)) {
		rc = PTR_ERR(dev_info);
		pr_err("%s: audio_dev_ctrl_find_dev failed. %ld \n",
			__func__, PTR_ERR(dev_info));
		return rc;
	}

	pr_debug("%s:dev_name = %s dev_id = %d, volume = %d\n",
			__func__, dev_info->name, dev_id, volume);

	if (dev_info->dev_ops.set_device_volume)
		rc = dev_info->dev_ops.set_device_volume(dev_info, volume);
	else {
		pr_info("%s : device %s does not support device volume "
				"control.", __func__, dev_info->name);
		return -EPERM;
	}

	return rc;
}

static int msm_reset_info(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 0;
	return 0;
}

static int msm_reset_get(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int msm_reset_put(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	pr_err("%s:Resetting all devices\n", __func__);
	return msm_reset_all_device();
}

static int msm_anc_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int msm_anc_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int msm_anc_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int rc = -EPERM;
	struct msm_snddev_info *dev_info;

	int dev_id = ucontrol->value.integer.value[0];
	int enable = ucontrol->value.integer.value[1];

	pr_debug("%s: dev_id = %d, enable = %d\n", __func__, dev_id, enable);
	dev_info = audio_dev_ctrl_find_dev(dev_id);

	if (IS_ERR(dev_info)) {
		rc = PTR_ERR(dev_info);
		pr_err("%s: audio_dev_ctrl_find_dev failed. %ld\n",
			__func__, PTR_ERR(dev_info));
		return rc;
	}

	if (dev_info->dev_ops.enable_anc) {
		rc = dev_info->dev_ops.enable_anc(dev_info, enable);
	} else {
		pr_info("%s : device %s does not support anc control.",
				 __func__, dev_info->name);
		return -EPERM;
	}

	return rc;
}

static int pcm_route_info(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	/*
	 * First parameter is session id ~ subdevice number
	 * Second parameter is device id.
	 */
	uinfo->count = 3;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = msm_snddev_devcount();
	return 0;
}

static int pcm_route_get_rx(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	return 0;
}

static int pcm_route_get_tx(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	return 0;
}

static int pcm_route_put_rx(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	int session_id = 0;
	int set = 0;
	struct msm_audio_route_config route_cfg;
	struct msm_snddev_info *dev_info;
	u64 session_mask = 0;

	/*
	 * session id is incremented by one and stored as session id 0
	 * is being used by dsp currently. whereas user space would use
	 * subdevice number as session id.
	 */
	session_id = ucontrol->value.integer.value[0];
	route_cfg.dev_id = ucontrol->value.integer.value[1];
	set = ucontrol->value.integer.value[2];

	dev_info = audio_dev_ctrl_find_dev(route_cfg.dev_id);
	if (IS_ERR(dev_info)) {
		pr_err("pass invalid dev_id %d\n", route_cfg.dev_id);
		return PTR_ERR(dev_info);
	}
	if (!(dev_info->capability & SNDDEV_CAP_RX))
			return -EINVAL;
	session_mask =
		(((u64)0x1) << session_id) << (MAX_BIT_PER_CLIENT * \
			((int)AUDDEV_CLNT_DEC-1));
	if (!set) {
		session_route.playback_session[session_id][dev_info->copp_id]
			= DEVICE_IGNORE;
		broadcast_event(AUDDEV_EVT_DEV_RLS,
				route_cfg.dev_id,
				session_mask);
		dev_info->sessions &= ~(session_mask);
		return 0;
	}
	pr_debug("%s:Routing playback session %d to %s\n",
				 __func__, (session_id),
				dev_info->name);
	session_route.playback_session[session_id][dev_info->copp_id] =
							 dev_info->copp_id;
	if (dev_info->opened) {
		dev_info->sessions = dev_info->sessions | session_mask;
		broadcast_event(AUDDEV_EVT_DEV_RDY,
				route_cfg.dev_id,
				session_mask);
	} else {
		broadcast_event(AUDDEV_EVT_DEV_RLS,
				route_cfg.dev_id,
				session_mask);
		dev_info->sessions &= ~(session_mask);
	}
	return 0;
}

static int pcm_route_put_tx(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	int session_id = 0;
	int set = 0;
	struct msm_audio_route_config route_cfg;
	struct msm_snddev_info *dev_info;
	u64 session_mask = 0;

	session_id = ucontrol->value.integer.value[0];
	route_cfg.dev_id = ucontrol->value.integer.value[1];
	set = ucontrol->value.integer.value[2];

	dev_info = audio_dev_ctrl_find_dev(route_cfg.dev_id);
	if (IS_ERR(dev_info)) {
		pr_err("pass invalid dev_id %d\n", route_cfg.dev_id);
		return PTR_ERR(dev_info);
	}
	pr_debug("%s:Routing capture session %d to %s\n", __func__,
					session_id,
					dev_info->name);
	if (!(dev_info->capability & SNDDEV_CAP_TX))
			return -EINVAL;
	session_mask =
		(((u64)0x1) << session_id) << (MAX_BIT_PER_CLIENT * \
			((int)AUDDEV_CLNT_ENC-1));
	if (!set) {
		session_route.capture_session[session_id][dev_info->copp_id]
			= DEVICE_IGNORE;
		broadcast_event(AUDDEV_EVT_DEV_RLS,
				route_cfg.dev_id,
				session_mask);
		dev_info->sessions &= ~(session_mask);
		return 0;
	}

	session_route.capture_session[session_id][dev_info->copp_id] =
							dev_info->copp_id;
	if (dev_info->opened) {
		dev_info->sessions = dev_info->sessions | session_mask;
		broadcast_event(AUDDEV_EVT_DEV_RDY,
				route_cfg.dev_id,
				session_mask);
	} else {
		broadcast_event(AUDDEV_EVT_DEV_RLS,
				route_cfg.dev_id,
				session_mask);
		dev_info->sessions &= ~(session_mask);
	}
	return 0;
}

static int msm_loopback_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 3;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max =  msm_snddev_devcount();
	return 0;
}

static int msm_loopback_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int msm_loopback_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int rc = 0;
	struct msm_snddev_info *src_dev_info = NULL; /* TX device */
	struct msm_snddev_info *dst_dev_info = NULL; /* RX device */
	int dst_dev_id = ucontrol->value.integer.value[0];
	int src_dev_id = ucontrol->value.integer.value[1];
	int set = ucontrol->value.integer.value[2];

	pr_debug("%s: dst=%d :src=%d set=%d\n", __func__,
	       dst_dev_id, src_dev_id, set);

	dst_dev_info = audio_dev_ctrl_find_dev(dst_dev_id);
	if (IS_ERR(dst_dev_info)) {
		pr_err("dst_dev:%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(dst_dev_info);
		return rc;
	}
	if (!(dst_dev_info->capability & SNDDEV_CAP_RX)) {
		pr_err("Destination device %d is not RX device\n",
			dst_dev_id);
		return -EFAULT;
	}

	src_dev_info = audio_dev_ctrl_find_dev(src_dev_id);
	if (IS_ERR(src_dev_info)) {
		pr_err("src_dev:%s:pass invalid dev_id\n", __func__);
		rc = PTR_ERR(src_dev_info);
		return rc;
	}
	if (!(src_dev_info->capability & SNDDEV_CAP_TX)) {
		pr_err("Source device %d is not TX device\n", src_dev_id);
		return -EFAULT;
	}

	if (set) {
		pr_debug("%s:%d:Enabling AFE_Loopback\n", __func__, __LINE__);
		src_dev = src_dev_id;
		dst_dev = dst_dev_id;
		loopback_status = 1;
		if ((dst_dev_info->opened) && (src_dev_info->opened))
			rc = afe_loopback(LOOPBACK_ENABLE,
				       dst_dev_info->copp_id,
				       src_dev_info->copp_id);
	} else {
		pr_debug("%s:%d:Disabling AFE_Loopback\n", __func__, __LINE__);
		src_dev = DEVICE_IGNORE;
		dst_dev = DEVICE_IGNORE;
		loopback_status = 0;
		rc = afe_loopback(LOOPBACK_DISABLE,
			       dst_dev_info->copp_id,
			       src_dev_info->copp_id);
	}
	return rc;
}
static int msm_device_mute_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = msm_snddev_devcount();
	return 0;
}

static int msm_device_mute_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	return 0;
}

static int msm_device_mute_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	int dev_id = ucontrol->value.integer.value[0];
	int mute = ucontrol->value.integer.value[1];
	struct msm_snddev_info *dev_info;
	int rc = 0;
	u16 gain = 0x2000;

	dev_info = audio_dev_ctrl_find_dev(dev_id);
	if (IS_ERR(dev_info)) {
		rc = PTR_ERR(dev_info);
		pr_err("%s: audio_dev_ctrl_find_dev failed. %ld\n",
			__func__, PTR_ERR(dev_info));
		return rc;
	}
	if (!(dev_info->capability & SNDDEV_CAP_TX)) {
		rc = -EINVAL;
		return rc;
	}
	if (mute)
		gain = 0;

	pr_debug("%s:dev_name = %s dev_id = %d, gain = %hX\n",
			__func__, dev_info->name, dev_id, gain);
	rc = afe_apply_gain(dev_info->copp_id, gain);
	if (rc < 0) {
		pr_err("%s : device %s not able to set device gain "
				"control.", __func__, dev_info->name);
		return rc;
	}
	pr_debug("Muting/Unmuting device id %d(%s)\n", dev_id, dev_info->name);

	return rc;
}

static int msm_voc_session_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = SESSION_ID_BASE + MAX_VOC_SESSIONS;
	return 0;
}

static int msm_voice_session_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] =
					voice_get_session_id("Voice session");
	return 0;
}

static int msm_voip_session_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = voice_get_session_id("VoIP session");
	return 0;
}

static struct snd_kcontrol_new snd_dev_controls[AUDIO_DEV_CTL_MAX_DEV];

static int snd_dev_ctl_index(int idx)
{
	struct msm_snddev_info *dev_info;

	dev_info = audio_dev_ctrl_find_dev(idx);
	if (IS_ERR(dev_info)) {
		pr_err("%s:pass invalid dev_id\n", __func__);
		return PTR_ERR(dev_info);
	}
	if (sizeof(dev_info->name) <= 44)
		sprintf(&snddev_name[idx][0] , "%s", dev_info->name);

	snd_dev_controls[idx].iface = SNDRV_CTL_ELEM_IFACE_MIXER;
	snd_dev_controls[idx].access = SNDRV_CTL_ELEM_ACCESS_READWRITE;
	snd_dev_controls[idx].name = &snddev_name[idx][0];
	snd_dev_controls[idx].index = idx;
	snd_dev_controls[idx].info = msm_device_info;
	snd_dev_controls[idx].get = msm_device_get;
	snd_dev_controls[idx].put = msm_device_put;
	snd_dev_controls[idx].private_value = 0;
	return 0;

}

#define MSM_EXT(xname, fp_info, fp_get, fp_put, addr) \
{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, \
  .access = SNDRV_CTL_ELEM_ACCESS_READWRITE, \
  .name = xname, \
  .info = fp_info,\
  .get = fp_get, .put = fp_put, \
  .private_value = addr, \
}

/* If new controls are to be added which would be constant across the
 * different targets, please add to the structure
 * snd_msm_controls. Please do not add any controls to the structure
 * snd_msm_secondary_controls defined below unless they are msm8x60
 * specific.
 */

static struct snd_kcontrol_new snd_msm_controls[] = {
	MSM_EXT("Count", msm_scontrol_count_info, msm_scontrol_count_get, \
						NULL, 0),
	MSM_EXT("Stream", msm_route_info, msm_route_get, \
						 msm_route_put, 0),
	MSM_EXT("Record", msm_route_info, msm_route_get, \
						 msm_route_put, 0),
	MSM_EXT("Voice", msm_voice_info, msm_voice_get, \
						 msm_voice_put, 0),
	MSM_EXT("Volume", msm_volume_info, msm_volume_get, \
						 msm_volume_put, 0),
	MSM_EXT("VoiceVolume", msm_v_volume_info, msm_v_volume_get, \
						 msm_v_volume_put, 0),
	MSM_EXT("VoiceMute", msm_v_mute_info, msm_v_mute_get, \
						 msm_v_mute_put, 0),
	MSM_EXT("Voice Call", msm_v_call_info, msm_v_call_get, \
						msm_v_call_put, 0),
	MSM_EXT("Device_Volume", msm_device_volume_info,
			msm_device_volume_get, msm_device_volume_put, 0),
	MSM_EXT("Reset", msm_reset_info,
			msm_reset_get, msm_reset_put, 0),
	MSM_EXT("ANC", msm_anc_info, msm_anc_get, msm_anc_put, 0),
	MSM_EXT("Device_Mute", msm_device_mute_info,
			msm_device_mute_get, msm_device_mute_put, 0),
};

static struct snd_kcontrol_new snd_msm_secondary_controls[] = {
	MSM_EXT("PCM Playback Sink",
			pcm_route_info, pcm_route_get_rx, pcm_route_put_rx, 0),
	MSM_EXT("PCM Capture Source",
			pcm_route_info, pcm_route_get_tx, pcm_route_put_tx, 0),
	MSM_EXT("Sound Device Loopback", msm_loopback_info,
			msm_loopback_get, msm_loopback_put, 0),
	MSM_EXT("VoiceVolume Ext",
		      msm_v_volume_info, msm_v_volume_get, msm_v_volume_put, 0),
	MSM_EXT("VoiceMute Ext",
			msm_v_mute_info, msm_v_mute_get, msm_v_mute_put, 0),
	MSM_EXT("Voice Call Ext",
			msm_v_call_info, msm_v_call_get, msm_v_call_put, 0),
	MSM_EXT("Voice session",
			msm_voc_session_info, msm_voice_session_get, NULL, 0),
	MSM_EXT("VoIP session",
			msm_voc_session_info, msm_voip_session_get, NULL, 0),
};

#if defined(CONFIG_MACH_TENDERLOIN)
static const struct snd_soc_dapm_widget tenderloin_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_MIC("Internal Mic", NULL),
        //	SND_SOC_DAPM_SPK("Speaker",wm8994_en_pwr_amp),
};

static struct snd_soc_dapm_route tenderloin_dapm_routes[] = {
	{ "Headphone", 	NULL, "HPOUT1L" },
	{ "Headphone", 	NULL, "HPOUT1R" },

	{ "Speaker", 	NULL, "LINEOUT1P" },
	{ "Speaker", 	NULL, "LINEOUT1N" },
	{ "Speaker", 	NULL, "LINEOUT2P" },
	{ "Speaker", 	NULL, "LINEOUT2N" },

	// Internal Mic
	{ "IN1LN",   NULL, "MICBIAS1" },
        { "MICBIAS1",   NULL, "Internal Mic" },
	// Headset Mic
	{ "IN2LN", 		NULL, "MICBIAS2" },
	{ "MICBIAS2", 	NULL, "Headset Mic" },

};

#endif

static int msm_new_mixer(struct snd_soc_codec *codec)
{
	unsigned int idx;
	int err;
	int dev_cnt;

	strcpy(codec->card->snd_card->mixername, "MSM Mixer");
	for (idx = 0; idx < ARRAY_SIZE(snd_msm_controls); idx++) {
		err = snd_ctl_add(codec->card->snd_card,
				snd_ctl_new1(&snd_msm_controls[idx],
					NULL));
		if (err < 0)
			pr_err("%s:ERR adding ctl\n", __func__);
	}

	for (idx = 0; idx < ARRAY_SIZE(snd_msm_secondary_controls); idx++) {
		err = snd_ctl_add(codec->card->snd_card,
			snd_ctl_new1(&snd_msm_secondary_controls[idx],
			NULL));
		if (err < 0)
			pr_err("%s:ERR adding secondary ctl\n", __func__);
	}
	dev_cnt = msm_snddev_devcount();

	for (idx = 0; idx < dev_cnt; idx++) {
		if (!snd_dev_ctl_index(idx)) {
			err = snd_ctl_add(codec->card->snd_card,
				snd_ctl_new1(&snd_dev_controls[idx],
					NULL));
			if (err < 0)
				pr_err("%s:ERR adding ctl\n", __func__);
		} else
			return 0;
	}
	simple_control = ARRAY_SIZE(snd_msm_controls)
			+ ARRAY_SIZE(snd_msm_secondary_controls);
	device_index = simple_control + 1;
#ifdef CONFIG_MACH_TENDERLOIN
	device_index += 256;
#endif
	return 0;
}

#ifdef CONFIG_MACH_TENDERLOIN
static int headphone_plugged = 0;
static struct switch_dev *headphone_switch;
static struct snd_soc_jack hp_jack;
static struct notifier_block jack_notifier;
static struct snd_soc_jack_pin hp_jack_pins[] = {
  {.pin = "Headphone", .mask = SND_JACK_HEADPHONE },
};
static struct snd_soc_jack_gpio hp_jack_gpios[] = {
  {
    .gpio = 67,
    .name = "hp-gpio",
    .report = SND_JACK_HEADPHONE,
    .debounce_time = 30,
    .wake = true,
  },
};

static int jack_notifier_event(struct notifier_block *nb, unsigned long event, void *data)
{
  struct snd_soc_codec *codec;
  struct wm8994_priv *wm8994;
  struct snd_soc_jack *jack;

  // Force enable will keep the MICBISA on, even when we stop reording
  jack = data;
  printk(KERN_ERR "%s: event!\n", __func__);

  if (jack)
    {
      codec = jack->codec;
      wm8994 = snd_soc_codec_get_drvdata(codec);

      if(1 == event){
        headphone_plugged = 1;
        // Someone inserted a jack, we need to turn on mic bias2 for headset mic detection
        snd_soc_dapm_force_enable_pin(&codec->dapm, "MICBIAS2");

        // This will enable mic detection on 8958
        wm8958_mic_detect( codec, &hp_jack, NULL, NULL);

      }else if (0 == event){
        headphone_plugged = 0;
        pr_crit("MIC DETECT: DISABLE. Jack removed\n");

        // This will disable mic detection on 8958
        wm8958_mic_detect( codec, NULL, NULL, NULL);

        if( wm8994->pdata->jack_is_mic) {
          dev_err(codec->dev, "  Reporting headset removed\n");
          wm8994->pdata->jack_is_mic = false;
          wm8994->micdet[0].jack->jack->type = SND_JACK_MICROPHONE;
          input_report_switch(wm8994->micdet[0].jack->jack->input_dev,
                              SW_MICROPHONE_INSERT,
                              0);
        } else {
          dev_err(codec->dev, "  Reporting headphone removed\n");
          input_report_switch(wm8994->micdet[0].jack->jack->input_dev,
                              SW_HEADPHONE_INSERT,
                              0);
        }

        input_sync(jack->jack->input_dev);
        snd_soc_dapm_disable_pin(&codec->dapm, "MICBIAS2");
      }
      if (headphone_switch) {
        switch_set_state(headphone_switch, headphone_plugged);
      }
    }
  return 0;
}

static ssize_t headphone_switch_print_name(struct switch_dev *sdev, char *buf)
{
  bool plugged = switch_get_state(sdev) != 0;
  return sprintf(buf, plugged ? "Headphone\n" : "None\n");
}
#endif

static int msm_soc_dai_init(
	struct snd_soc_pcm_runtime *rtd)
{

	int ret = 0;
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	init_waitqueue_head(&the_locks.enable_wait);
	init_waitqueue_head(&the_locks.eos_wait);
	init_waitqueue_head(&the_locks.write_wait);
	init_waitqueue_head(&the_locks.read_wait);
	memset(&session_route, DEVICE_IGNORE, sizeof(struct pcm_session));

	ret = msm_new_mixer(codec);
	if (ret < 0)
		pr_err("%s: ALSA MSM Mixer Fail\n", __func__);

	// permanently disable pin
        snd_soc_dapm_nc_pin(dapm, "SPKOUTRN");
        snd_soc_dapm_nc_pin(dapm, "SPKOUTRP");
        snd_soc_dapm_nc_pin(dapm, "SPKOUTLN");
        snd_soc_dapm_nc_pin(dapm, "SPKOUTLP");
        snd_soc_dapm_nc_pin(dapm, "HPOUT2P");
        snd_soc_dapm_nc_pin(dapm, "HPOUT2N");
        snd_soc_dapm_nc_pin(dapm, "IN2RP:VXRP");
        snd_soc_dapm_nc_pin(dapm, "IN2RN");
        snd_soc_dapm_nc_pin(dapm, "IN2LN");
        snd_soc_dapm_nc_pin(dapm, "IN1RN");
        snd_soc_dapm_nc_pin(dapm, "IN1RP");
        snd_soc_dapm_nc_pin(dapm, "IN1LN");

#ifdef CONFIG_MACH_TENDERLOIN
        /* Headphone jack detection */
        // If we have the HeadSet uart (hs_uart) then we DONT want to detect headphones
        if ( hs_uart == 1 ) {
          snd_soc_jack_new(codec, "headset", SND_JACK_MICROPHONE | SND_JACK_BTN_0, &hp_jack);
        } else {
          snd_soc_jack_new(codec, "headset", SND_JACK_HEADPHONE | SND_JACK_MICROPHONE | SND_JACK_BTN_0, &hp_jack);
        }
        snd_soc_jack_add_pins(&hp_jack, ARRAY_SIZE(hp_jack_pins),hp_jack_pins);
        snd_jack_set_key(hp_jack.jack, SND_JACK_BTN_0, KEY_PLAYPAUSE); // mapping button 0 to KEY_PLAYPUASE

        // Register a notifier with snd_soc_jack
        jack_notifier.notifier_call = jack_notifier_event;

        snd_soc_jack_notifier_register(&hp_jack, &jack_notifier);

        ret = snd_soc_jack_add_gpios(&hp_jack, ARRAY_SIZE(hp_jack_gpios), hp_jack_gpios);

        // add headphone switch
        headphone_switch = kzalloc(sizeof(struct switch_dev), GFP_KERNEL);
        if (headphone_switch) {
          headphone_switch->name = "h2w";
          headphone_switch->print_name = headphone_switch_print_name;

          ret = switch_dev_register(headphone_switch);
          if (ret < 0) {
            printk(KERN_ERR "Unable to register headphone switch\n");
            kfree(headphone_switch);
            headphone_switch = NULL;
          } else {
            headphone_plugged = hp_jack.status;
            switch_set_state(headphone_switch, headphone_plugged);
          }
        } else {
          printk(KERN_ERR "Unable to allocate headphone switch\n");
        }
#endif

        return ret;
}

#if defined(CONFIG_MACH_TENDERLOIN)

static struct snd_soc_codec_conf wm8994_codec_conf[] = {
	{
		.dev_name = "wm8994",
		.name_prefix = "Sub",
	},
};

static int msm8660_i2s_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int rate = params_rate(params), ret = 0;
	int fll_rate = 0;
        int bclk_rate = 0;
	int rc = 0;

        bclk_rate = params_rate(params) * WM_CHANNELS * WM_BITS;
	fll_rate = bclk_rate * WM_FLL_MULT;
        if (fll_rate < WM_FLL_MIN_RATE)
          fll_rate = WM_FLL_MIN_RATE;

	pr_debug("Enter %s rate = %d\n", __func__, rate);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		if (rx_hw_param_status)
			return 0;
		/* wm8903 run @ LRC*256 */
		rc = snd_soc_dai_set_pll(codec_dai, WM8994_FLL1, WM8994_FLL_SRC_BCLK, bclk_rate, fll_rate);
		if (rc < 0) {
		  pr_err("Failed to set DAI FLL to rate %d: ret %d\n", WM_FLL_MULT * bclk_rate, rc);
		  return rc;
		}
		ret = snd_soc_dai_set_sysclk(codec_dai, WM8994_SYSCLK_FLL1, rate * 256,
						SND_SOC_CLOCK_IN);
		snd_soc_dai_digital_mute(codec_dai, 0);
		if (ret < 0) {
			pr_err("can't set rx codec clk configuration\n");
			return ret;
		}
		rx_hw_param_status++;
	} else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		if (tx_hw_param_status)
			return 0;
		rc = snd_soc_dai_set_pll(codec_dai, WM8994_FLL2, WM8994_FLL_SRC_BCLK, bclk_rate, fll_rate);
		if (rc < 0) {
		  pr_err("Failed to set DAI FLL to rate %d: ret %d\n", WM_FLL_MULT * bclk_rate, rc);
		  return rc;
		}
		ret = snd_soc_dai_set_sysclk(codec_dai, WM8994_SYSCLK_FLL2, rate * 256,
						SND_SOC_CLOCK_IN);
		if (ret < 0) {
			pr_err("can't set tx codec clk configuration\n");
			return ret;
		}
		tx_hw_param_status++;
	}

        snd_mask_set(&params->masks[SNDRV_PCM_HW_PARAM_FORMAT - SNDRV_PCM_HW_PARAM_FIRST_MASK], SNDRV_PCM_FORMAT_S24_LE);
        return wm8994_hw_params(substream, params, codec_dai);
}

static void msm8660_i2s_shutdown(struct snd_pcm_substream *substream)
{
	pr_debug("Enter %s\n", __func__);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK ||
			 substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		tx_hw_param_status = 0;
		rx_hw_param_status = 0;
	}
}

static struct snd_soc_ops machine_ops = {
	.shutdown	= msm8660_i2s_shutdown,
	.hw_params	= msm8660_i2s_hw_params,
};

static struct snd_soc_dai_link msm_dai[] = {
	{
		.name = "Media Playback",
		.stream_name = "Media Playback",
		.cpu_dai_name = "msm-cpu-dai.0",
		.codec_dai_name = "wm8994-aif1",
		.platform_name = "msm-dsp-audio.0",
		.codec_name = "wm8994-codec",
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF
						| SND_SOC_DAIFMT_CBS_CFS,
		.init   = msm_soc_dai_init,
                .ops = &machine_ops,
	},
	{
		.name = "Media Capture",
		.stream_name = "Media Capture",
		.cpu_dai_name = "msm-cpu-dai.0",
		.codec_dai_name = "wm8994-aif2",
		.platform_name = "msm-dsp-audio.0",
		.codec_name = "wm8994-codec",
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF
								| SND_SOC_DAIFMT_CBS_CFS,
                //                .ops = &msm_aif_ops,
	},
};


#else

static struct snd_soc_dai_link msm_dai[] = {
{
	.name = "MSM Primary I2S",
	.stream_name = "DSP 1",
	.cpu_dai_name = "msm-cpu-dai.0",
	.platform_name = "msm-dsp-audio.0",
	.codec_name = "msm-codec-dai.0",
	.codec_dai_name = "msm-codec-dai",
	.init   = &msm_soc_dai_init,
},
#ifdef CONFIG_MSM_8x60_VOIP
{
	.name = "MSM Primary Voip",
	.stream_name = "MVS",
	.cpu_dai_name = "mvs-cpu-dai.0",
	.platform_name = "msm-mvs-audio.0",
	.codec_name = "mvs-codec-dai.0",
	.codec_dai_name = "mvs-codec-dai",
},
#endif
};

#endif //tenderloin

static struct snd_soc_card snd_soc_card_msm = {
	.name		= "msm-audio",
	.owner = THIS_MODULE,
	.dai_link	= msm_dai,
	.num_links = ARRAY_SIZE(msm_dai),
#if defined(CONFIG_MACH_TENDERLOIN)
	.codec_conf = wm8994_codec_conf,
	.num_configs = ARRAY_SIZE(wm8994_codec_conf),
	.dapm_widgets = tenderloin_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(tenderloin_dapm_widgets),
	.dapm_routes = tenderloin_dapm_routes,
	.num_dapm_routes = ARRAY_SIZE(tenderloin_dapm_routes),
#endif
};

static int __init msm_audio_init(void)
{
	int ret;

	msm_audio_snd_device = platform_device_alloc("soc-audio", -1);
	if (!msm_audio_snd_device)
		return -ENOMEM;

	platform_set_drvdata(msm_audio_snd_device, &snd_soc_card_msm);
	ret = platform_device_add(msm_audio_snd_device);
	if (ret) {
		platform_device_put(msm_audio_snd_device);
		return ret;
	}

	src_dev = DEVICE_IGNORE;
	dst_dev = DEVICE_IGNORE;

	return ret;
}

static void __exit msm_audio_exit(void)
{
	platform_device_unregister(msm_audio_snd_device);
}

module_init(msm_audio_init);
module_exit(msm_audio_exit);

MODULE_DESCRIPTION("PCM module");
MODULE_LICENSE("GPL v2");
