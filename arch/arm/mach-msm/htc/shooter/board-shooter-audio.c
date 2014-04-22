/* linux/arch/arm/mach-msm/board-shooter-audio.c
 *
 * Copyright (C) 2010-2011 HTC Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/android_pmem.h>
#include <linux/mfd/pmic8058.h>
#include <linux/mfd/marimba.h>
#include <linux/delay.h>
#include <linux/pmic8058-othc.h>
#include <linux/spi_aic3254.h>

#include <linux/gpio.h>
#include <mach/gpio.h>
#include <mach/dal.h>
#include <linux/tpa2051d3.h>
#include "qdsp6v2/snddev_icodec.h"
#include "qdsp6v2/snddev_ecodec.h"
#include "qdsp6v2/snddev_hdmi.h"
#include <mach/qdsp6v2/audio_dev_ctl.h>
#include <sound/apr_audio.h>
#include <sound/q6asm.h>
#include <mach/htc_acoustic_8x60.h>

#include "board-shooter-audio-data.h"

extern unsigned int system_rev;

static struct mutex bt_sco_lock;
static struct mutex mic_lock;
static int curr_rx_mode;
static atomic_t aic3254_ctl = ATOMIC_INIT(0);
static atomic_t q6_effect_mode = ATOMIC_INIT(-1);

static uint32_t msm_snddev_gpio[] = {
	GPIO_CFG(108, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(109, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(110, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

#define BIT_SPEAKER	(1 << 0)
#define BIT_HEADSET	(1 << 1)
#define BIT_RECEIVER	(1 << 2)
#define BIT_FM_SPK	(1 << 3)
#define BIT_FM_HS	(1 << 4)
#define SHOOTER_AUD_CODEC_RST        (67)
#define SHOOTER_AUD_HP_EN          PMGPIO(18)
#define SHOOTER_HAP_ENABLE        PMGPIO(19)
#define SHOOTER_AUD_MIC_SEL        PMGPIO(26)
#define PM8058_GPIO_BASE			NR_MSM_GPIOS
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_GPIO_BASE)
#define PMGPIO(x) (x-1)
void shooter_snddev_bmic_pamp_on(int en);
static uint32_t msm_aic3254_reset_gpio[] = {
	GPIO_CFG(SHOOTER_AUD_CODEC_RST, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

void shooter_snddev_poweramp_on(int en)
{
	pr_info("%s %d\n", __func__, en);
	if (en) {
		msleep(30);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_HAP_ENABLE), 1);
		set_speaker_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_SPEAKER;
		msleep(5);
	} else {
		set_speaker_amp(0);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_HAP_ENABLE), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_SPEAKER;
	}
}

void shooter_snddev_hsed_pamp_on(int en)
{
	pr_info("%s %d\n", __func__, en);
	if (en) {
		msleep(30);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 1);
		set_headset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_HEADSET;
		msleep(5);
	} else {
		set_headset_amp(0);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_HEADSET;
	}
}

void shooter_snddev_hs_spk_pamp_on(int en)
{
	pr_info("%s %d\n", __func__, en);
	if (en) {
		msleep(30);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 1);
		set_speaker_headset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_HEADSET;
		msleep(5);
	} else {
		set_speaker_headset_amp(0);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_HEADSET;
	}
}

void shooter_snddev_receiver_pamp_on(int en)
{
	pr_info("%s %d\n", __func__, en);
	if (en) {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 1);
		set_handset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_RECEIVER;
	} else {
		set_handset_amp(0);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_RECEIVER;
	}
}

/* power on/off externnal mic bias */
void shooter_mic_enable(int en, int shift)
{
	pr_info("%s: %d, shift %d\n", __func__, en, shift);

	mutex_lock(&mic_lock);

	if (en)
		pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_ALWAYS_ON);
	else
		pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_OFF);

	mutex_unlock(&mic_lock);
}

void shooter_snddev_imic_pamp_on(int en)
{
	int ret;

	pr_info("%s %d\n", __func__, en);
	shooter_snddev_bmic_pamp_on(en);
	if (en) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_err("%s: Enabling int mic power failed\n", __func__);

	} else {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
		if (ret)
			pr_err("%s: Enabling int mic power failed\n", __func__);
	}
}

void shooter_snddev_bmic_pamp_on(int en)
{
	int ret;

	pr_info("%s %d\n", __func__, en);

	if (en) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_err("%s: Enabling back mic power failed\n", __func__);

		/* select internal mic path */
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_MIC_SEL), 0);
	} else {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_OFF);
		if (ret)
			pr_err("%s: Enabling back mic power failed\n", __func__);
	}
}

void shooter_snddev_emic_pamp_on(int en)
{
	pr_info("%s %d\n", __func__, en);

	if (en) {
		/* select external mic path */
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_MIC_SEL), 1);
	}
}

void shooter_snddev_stereo_mic_pamp_on(int en)
{
	int ret;

	pr_info("%s %d\n", __func__, en);

	if (en) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_err("%s: Enabling int mic power failed\n", __func__);

		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_err("%s: Enabling back mic power failed\n", __func__);

		/* select external mic path */
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_MIC_SEL), 0);
	} else {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
		if (ret)
			pr_err("%s: Disabling int mic power failed\n", __func__);


		ret = pm8058_micbias_enable(OTHC_MICBIAS_1, OTHC_SIGNAL_OFF);
		if (ret)
			pr_err("%s: Disabling back mic power failed\n", __func__);
	}
}

void shooter_snddev_fmspk_pamp_on(int en)
{
	pr_info("%s %d\n", __func__, en);
	if (en) {
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_HAP_ENABLE), 1);
		set_speaker_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_FM_SPK;
	} else {
		set_speaker_amp(0);
		gpio_direction_output(PM8058_GPIO_PM_TO_SYS(SHOOTER_HAP_ENABLE), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_FM_SPK;
	}
}

void shooter_snddev_fmhs_pamp_on(int en)
{
	pr_info("%s %d\n", __func__, en);
	if (en) {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 1);
		set_headset_amp(1);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode |= BIT_FM_HS;
	} else {
		set_headset_amp(0);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), 0);
		if (!atomic_read(&aic3254_ctl))
			curr_rx_mode &= ~BIT_FM_HS;
	}
}

void shooter_voltage_on(int en)
{
}

int shooter_get_rx_vol(uint8_t hw, int network, int level)
{
	int vol = 0;

	/* to be implemented */

	pr_info("%s(%d, %d, %d) => %d\n", __func__, hw, network, level, vol);

	return vol;
}

void shooter_rx_amp_enable(int en)
{
	if (curr_rx_mode != 0) {
		atomic_set(&aic3254_ctl, 1);
		pr_info("%s: curr_rx_mode 0x%x, en %d\n",
			__func__, curr_rx_mode, en);
		if (curr_rx_mode & BIT_SPEAKER)
			shooter_snddev_poweramp_on(en);
		if (curr_rx_mode & BIT_HEADSET)
			shooter_snddev_hsed_pamp_on(en);
		if (curr_rx_mode & BIT_RECEIVER)
			shooter_snddev_receiver_pamp_on(en);
		if (curr_rx_mode & BIT_FM_SPK)
			shooter_snddev_fmspk_pamp_on(en);
		if (curr_rx_mode & BIT_FM_HS)
			shooter_snddev_fmhs_pamp_on(en);
		atomic_set(&aic3254_ctl, 0);;
	}
}

int shooter_support_aic3254(void)
{
	return 1;
}

int shooter_support_adie(void)
{
	return 0;
}

int shooter_support_back_mic(void)
{
	return 1;
}

void shooter_get_acoustic_tables(struct acoustic_tables *tb)
{
	switch (system_rev) {
	case 0:
	case 1:
		strcpy(tb->tpa2051, "TPA2051_CFG.csv");
		break;
	case 2:
		strcpy(tb->tpa2051, "TPA2051_CFG_XC.csv");
		break;
	default:
		strcpy(tb->tpa2051, "TPA2051_CFG_XC.csv");
		break;
	}
}

int shooter_support_beats(void)
{
	/* this means HW support 1V for beats */
	/* PCB ID is defined by HW revision
	 * [0] raised means support 1V headset
	 * [7-4] set as "1000" stands for PVT device */
	if (((system_rev&0x1) == 0x1) && ((system_rev>>4&0xF) == 0x8))
		return 1;
	else
		return 0;
}

void shooter_enable_beats(int en)
{
	pr_info("%s: %d\n", __func__, en);
	set_beats_on(en);
}

int shooter_is_msm_i2s_slave(void)
{
	/* 1 - CPU slave, 0 - CPU master */
	return 1;
}

void shooter_reset_3254(void)
{
	gpio_tlmm_config(msm_aic3254_reset_gpio[0], GPIO_CFG_ENABLE);
	gpio_set_value(SHOOTER_AUD_CODEC_RST, 0);
	mdelay(1);
	gpio_set_value(SHOOTER_AUD_CODEC_RST, 1);
}

void shooter_set_q6_effect_mode(int mode)
{
	pr_info("%s: mode %d\n", __func__, mode);
	atomic_set(&q6_effect_mode, mode);
}

int shooter_get_q6_effect_mode(void)
{
	int mode = atomic_read(&q6_effect_mode);
	pr_info("%s: mode %d\n", __func__, mode);
	return mode;
}

static struct q6v2audio_analog_ops ops = {
	.speaker_enable	        = shooter_snddev_poweramp_on,
	.headset_enable	        = shooter_snddev_hsed_pamp_on,
	.handset_enable	        = shooter_snddev_receiver_pamp_on,
	.headset_speaker_enable	= shooter_snddev_hs_spk_pamp_on,
	.int_mic_enable         = shooter_snddev_imic_pamp_on,
	.back_mic_enable        = shooter_snddev_bmic_pamp_on,
	.ext_mic_enable         = shooter_snddev_emic_pamp_on,
	.stereo_mic_enable      = shooter_snddev_stereo_mic_pamp_on,
	.fm_headset_enable      = shooter_snddev_fmhs_pamp_on,
	.fm_speaker_enable      = shooter_snddev_fmspk_pamp_on,
	.voltage_on		= shooter_voltage_on,
};

static struct q6v2audio_icodec_ops iops = {
	.support_aic3254 = shooter_support_aic3254,
	.support_adie = shooter_support_adie,
	.is_msm_i2s_slave = shooter_is_msm_i2s_slave,
};

static struct aic3254_ctl_ops cops = {
	.rx_amp_enable        = shooter_rx_amp_enable,
	.reset_3254           = shooter_reset_3254,
	.lb_dsp_init          = &LOOPBACK_DSP_INIT_PARAM,
	.lb_receiver_imic     = &LOOPBACK_Receiver_IMIC_PARAM,
	.lb_speaker_imic      = &LOOPBACK_Speaker_IMIC_PARAM,
	.lb_headset_emic      = &LOOPBACK_Headset_EMIC_PARAM,
	.lb_receiver_bmic     = &LOOPBACK_Receiver_BMIC_PARAM,
	.lb_speaker_bmic      = &LOOPBACK_Speaker_BMIC_PARAM,
	.lb_headset_bmic      = &LOOPBACK_Headset_BMIC_PARAM,
};

static struct acoustic_ops acoustic = {
	.enable_mic_bias = shooter_mic_enable,
	.support_aic3254 = shooter_support_aic3254,
	.support_adie = shooter_support_adie,
	.support_back_mic = shooter_support_back_mic,
	.get_acoustic_tables = shooter_get_acoustic_tables,
	.support_beats = shooter_support_beats,
	.enable_beats = shooter_enable_beats,
	.set_q6_effect = shooter_set_q6_effect_mode,
};

void shooter_aic3254_set_mode(int config, int mode)
{
	aic3254_set_mode(config, mode);
}


static struct q6v2audio_aic3254_ops aops = {
       .aic3254_set_mode = shooter_aic3254_set_mode,
};

void shooter_audio_gpios_init(void)
{
	pr_info("%s\n", __func__);
	gpio_request(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_HP_EN), "AUD_SPK_ENO");
	gpio_request(PM8058_GPIO_PM_TO_SYS(SHOOTER_AUD_MIC_SEL), "AUD_HANDSET_ENO");
}

void __init shooter_audio_init(void)
{
        int i = 0;

	mutex_init(&bt_sco_lock);
	mutex_init(&mic_lock);

#ifdef CONFIG_MSM8X60_AUDIO
	pr_info("%s\n", __func__);
	htc_8x60_register_analog_ops(&ops);
	htc_8x60_register_icodec_ops(&iops);
	acoustic_register_ops(&acoustic);
	htc_8x60_register_aic3254_ops(&aops);
	msm_set_voc_freq(8000, 8000);
#endif

	aic3254_register_ctl_ops(&cops);

	for (i = 0 ; i < ARRAY_SIZE(msm_snddev_gpio); i++)
		gpio_tlmm_config(msm_snddev_gpio[i], GPIO_CFG_DISABLE);

	/* PMIC GPIO Init (See board-shooter.c) */
        shooter_audio_gpios_init();

	/* Reset AIC3254 */
	shooter_reset_3254();
}
