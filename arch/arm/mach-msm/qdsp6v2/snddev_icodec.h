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
 *
 */
#ifndef __MACH_QDSP6V2_SNDDEV_ICODEC_H
#define __MACH_QDSP6V2_SNDDEV_ICODEC_H
#include <linux/mfd/msm-adie-codec.h>
#include <mach/qdsp6v2/audio_def.h>
#include <linux/spi_aic3254.h>
#include <mach/pmic.h>

struct snddev_icodec_data {
	u32 capability; /* RX or TX */
	const char *name;
	u32 copp_id; /* audpp routing */
	/* Adie profile */
	struct adie_codec_dev_profile *profile;
	/* Afe setting */
	u8 channel_mode;
	u32 default_sample_rate;
	int (*pamp_on) (void);
	void (*pamp_off) (void);
	int (*voltage_on) (void);
	void (*voltage_off) (void);
	u32 dev_vol_type;
	u32 aic3254_id;
	u32 aic3254_voc_id;
	u32 default_aic3254_id;
};

/* Context for each internal codec sound device */
struct snddev_icodec_state {
	struct snddev_icodec_data *data;
	struct adie_codec_path *adie_path;
	u32 sample_rate;
	u32 enabled;
};

struct q6v2audio_analog_ops {
	void (*speaker_enable)(int en);
	void (*headset_enable)(int en);
	void (*handset_enable)(int en);
	void (*headset_speaker_enable)(int en);
	void (*int_mic_enable)(int en);
	void (*back_mic_enable)(int en);
	void (*ext_mic_enable)(int en);
	void (*stereo_mic_enable)(int en);
	void (*usb_headset_enable)(int en);
	void (*fm_headset_enable)(int en);
	void (*fm_speaker_enable)(int en);
	void (*voltage_on) (int on);
};

struct q6v2audio_icodec_ops {
	int (*support_aic3254) (void);
	int (*support_adie) (void);
	int (*is_msm_i2s_slave) (void);
	int (*support_aic3254_use_mclk) (void);
};

struct q6v2audio_aic3254_ops {
	void (*aic3254_set_mode)(int config, int mode);
};

struct aic3254_info {
    u32 dev_id;
    u32 path_id;
};

int update_aic3254_info(struct aic3254_info *info);
void htc_8x60_register_analog_ops(struct q6v2audio_analog_ops *ops);
void htc_8x60_register_icodec_ops(struct q6v2audio_icodec_ops *ops);
void htc_8x60_register_aic3254_ops(struct q6v2audio_aic3254_ops *ops);

#endif
