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
#include "tenderloin.h"

#include <sound/jack.h>
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

static int wm8994_en_pwr_amp(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
	struct snd_soc_codec *codec = w->codec;

	if( SND_SOC_DAPM_EVENT_ON(event) ){
		snd_soc_write(codec, WM8994_GPIO_1, 0x41);
	}
	else{
		snd_soc_write(codec, WM8994_GPIO_1, 0x1);
	}
	return 0;
}

static const struct snd_soc_dapm_widget tenderloin_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_MIC("Internal Mic", NULL),
	SND_SOC_DAPM_SPK("Speaker",wm8994_en_pwr_amp),
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

int headphone_plugged = 0;
struct switch_dev *headphone_switch;
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

int tenderloin_soc_dai_init(struct snd_soc_pcm_runtime *rtd)
{
 	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret = 0;

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

	snd_soc_dapm_new_controls(dapm, tenderloin_dapm_widgets,
				ARRAY_SIZE(tenderloin_dapm_widgets));

	snd_soc_dapm_add_routes(dapm, tenderloin_dapm_routes,
				ARRAY_SIZE(tenderloin_dapm_routes));

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
	return ret;
}

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

	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_CBS_CFS |
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF);

		if (ret != 0) {
			pr_err("Failed to set DAI format: %d\n", ret);
			return ret;
		}

	bclk_rate = params_rate(params) * WM_CHANNELS * WM_BITS;
	fll_rate = bclk_rate * WM_FLL_MULT;
	if (fll_rate < WM_FLL_MIN_RATE)
		fll_rate = WM_FLL_MIN_RATE;

	pr_debug("Enter %s rate = %d\n", __func__, rate);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		rtd = snd_soc_get_pcm_runtime(codec_dai->card, "Media Playback");
		codec_dai = rtd->codec_dai;

		if (rx_hw_param_status)
			return 0;
		/* wm8903 run @ LRC*256 */
		rc = snd_soc_dai_set_pll(codec_dai, WM8994_FLL1, WM8994_FLL_SRC_BCLK, bclk_rate, fll_rate);
		if (rc < 0) {
		  pr_err("Failed to set DAI FLL to rate %d: ret %d\n", WM_FLL_MULT * bclk_rate, rc);
		  return rc;
		}
		ret = snd_soc_dai_set_sysclk(codec_dai, WM8994_SYSCLK_FLL1, fll_rate,
						SND_SOC_CLOCK_IN);
		if (ret < 0) {
			pr_err("can't set rx codec clk configuration\n");
			return ret;
		}
		rx_hw_param_status++;
	} else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		rtd = snd_soc_get_pcm_runtime(codec_dai->card, "Media Capture");
		codec_dai = rtd->codec_dai;
		if (tx_hw_param_status)
			return 0;
		rc = snd_soc_dai_set_pll(codec_dai, WM8994_FLL2, WM8994_FLL_SRC_BCLK, bclk_rate, fll_rate);
		if (rc < 0) {
		  pr_err("Failed to set DAI FLL to rate %d: ret %d\n", WM_FLL_MULT * bclk_rate, rc);
		  return rc;
		}
		ret = snd_soc_dai_set_sysclk(codec_dai, WM8994_SYSCLK_FLL2, fll_rate,
						SND_SOC_CLOCK_IN);
		if (ret < 0) {
			pr_err("can't set tx codec clk configuration\n");
			return ret;
		}
		tx_hw_param_status++;
}

        snd_mask_set(&params->masks[SNDRV_PCM_HW_PARAM_FORMAT - SNDRV_PCM_HW_PARAM_FIRST_MASK], SNDRV_PCM_FORMAT_S16_LE);
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

static struct snd_soc_dai_link tenderloin_msm_dai[] = {
	{
		.name = "Media Playback",
		.stream_name = "Media Playback",
		.cpu_dai_name = "msm-cpu-dai.0",
		.codec_dai_name = "wm8994-aif1",
		.platform_name = "msm-dsp-audio.0",
		.codec_name = "wm8994-codec",
		.ops = &machine_ops,
	},
	{
		.name = "Media Capture",
		.stream_name = "Media Capture",
		.cpu_dai_name = "msm-cpu-dai.0",
		.codec_dai_name = "wm8994-aif2",
		.platform_name = "msm-dsp-audio.0",
		.codec_name = "wm8994-codec",
		.ops = &machine_ops,
	},
};

void tenderloin_soc_card_fixup(struct snd_soc_card *card, dai_init_cb dai_init)
{
  tenderloin_msm_dai[0].init = dai_init;
  card->dai_link = tenderloin_msm_dai;
  card->num_links = ARRAY_SIZE(tenderloin_msm_dai);
  card->codec_conf = wm8994_codec_conf;
  card->num_configs = ARRAY_SIZE(wm8994_codec_conf);
}
