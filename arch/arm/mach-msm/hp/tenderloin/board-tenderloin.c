/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
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

#define HASTIMPANI 0
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/msm_ssbi.h>
#include <linux/mfd/pmic8058.h>

#include <linux/leds.h>
#include <linux/clk.h>
#include <linux/pmic8058-othc.h>
#include <linux/mfd/pmic8901.h>
#include <linux/htc_flashlight.h>
#include <linux/regulator/pmic8901-regulator.h>
#include <linux/regulator/fixed.h>
#include <linux/bootmem.h>
#include <linux/msm_adc.h>
#include <linux/m_adcproc.h>
#include <linux/mfd/marimba.h>
#include <linux/msm-charger.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/smsc911x.h>
#include <linux/spi/spi.h>
#include <linux/input/tdisc_shinetsu.h>
#include <linux/i2c/isa1200.h>
#include <linux/dma-mapping.h>
#include <linux/i2c/bq27520.h>
#include <linux/i2c/lsm303dlh.h>
#include <linux/isl29023.h>
#include <linux/reboot.h>

#ifdef CONFIG_LEDS_LM8502
#include <linux/i2c_lm8502_led.h>
#endif

#ifdef CONFIG_MFD_WM8994
#include <linux/mfd/wm8994/core.h>
#include <linux/mfd/wm8994/pdata.h>
#endif

#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif

#if defined(CONFIG_SMB137B_CHARGER) || defined(CONFIG_SMB137B_CHARGER_MODULE)
#include <linux/i2c/smb137b.h>
#endif
#ifdef CONFIG_SND_SOC_WM8903
#include <sound/wm8903.h>
#endif
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>
#include <asm/hardware/gic.h>

#include <mach/board_htc.h>
#include <mach/dma.h>
#include <mach/mpp.h>
#include <mach/board.h>
#include <mach/irqs.h>
#include <mach/msm_spi.h>

#ifdef CONFIG_BT
#include <mach/msm_serial_hs.h>
#include <mach/htc_bdaddress.h>
#endif

#include <mach/msm_serial_hs_lite.h>
#include <mach/msm_iomap.h>
#include <mach/msm_memtypes.h>
#include <asm/mach/mmc.h>
#ifdef CONFIG_TPS65200
#include <linux/tps65200.h>
#endif
#include <mach/msm_battery.h>
#include <mach/msm_hsusb.h>
#include <mach/gpiomux.h>
#ifdef CONFIG_MSM_DSPS
#include <mach/msm_dsps.h>
#endif
#include <mach/msm_xo.h>
#include <mach/msm_bus_board.h>
#include <mach/socinfo.h>
#include <linux/i2c/isl9519.h>
#ifdef CONFIG_USB_G_ANDROID
#include <linux/usb/android.h>
#include <mach/usbdiag.h>
#endif
#include <linux/tpa2051d3.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include <mach/sdio_al.h>
#include <mach/rpm.h>
#include <mach/rpm-regulator.h>
#include <mach/restart.h>
#include <mach/cable_detect.h>
#include <mach/board-msm8660.h>

#ifdef CONFIG_MAX8903B_CHARGER
#include <linux/max8903b_charger.h>
#endif

#ifdef CONFIG_HSUART
#include <linux/hsuart.h>
#endif

#ifdef CONFIG_USER_PINS
#include <linux/user-pins.h>
#endif

#ifdef CONFIG_NDUID
#include <linux/nduid.h>
#endif

#include "board-tenderloin.h"
#include "devices.h"
#include "devices-msm8x60.h"
#include <mach/cpuidle.h>
#include <linux/mpu.h>
#include "pm.h"
#include <mach/mpm.h>
#include "spm.h"
#include "rpm_log.h"
#include "smd_private.h"
#include "timer.h"
#include "gpiomux-8x60.h"
#include "rpm_stats.h"
#include "peripheral-loader.h"
#include <linux/platform_data/qcom_crypto_device.h>
#include "rpm_resources.h"
#include "pm-boot.h"
#include "clock.h"
//#include "board-storage-common-a.h"
#include <mach/board_htc.h>
#include "acpuclock.h"

#include <linux/msm_ion.h>
#include <mach/ion.h>
#include <mach/msm_rtb.h>
#include <linux/msm_tsens.h>

extern int ps_type;
int *pin_table = NULL;
int wm8994_reg_status = 0;

#ifdef CONFIG_MAX8903B_CHARGER
static unsigned max8903b_ps_connected = 0;
static unsigned max8903b_vbus_draw_ma = 0;
static unsigned max8903b_vbus_draw_ma_max = 0;
void max8903b_set_vbus_draw (unsigned ma);
#endif

#define MSM_SHARED_RAM_PHYS 0x40000000

#define UI_INT1_N 25
#define UI_INT2_N 34
#define UI_INT3_N 14
#define FM_GPIO 17

u32 board_type = TOPAZ_EVT1; /* In case we get a lazy bootloader */

static struct {
	enum topaz_board_types type;
	const char *str;
} boardtype_tbl[] = {
	/* WiFi */
	{TOPAZ_PROTO,    "topaz-Wifi-proto"},
	{TOPAZ_PROTO2,   "topaz-Wifi-proto2"},
	{TOPAZ_EVT1,     "topaz-Wifi-evt1"},
	{TOPAZ_EVT2,     "topaz-Wifi-evt2"},
	{TOPAZ_EVT3,     "topaz-Wifi-evt3"},
	{TOPAZ_DVT,      "topaz-Wifi-dvt"},
	{TOPAZ_PVT,      "topaz-Wifi-pvt"},

	/* 3G */
	{TOPAZ_3G_PROTO,    "topaz-3G-proto"},
	{TOPAZ_3G_PROTO2,   "topaz-3G-proto2"},
	{TOPAZ_3G_EVT1,     "topaz-3G-evt1"},
	{TOPAZ_3G_EVT2,     "topaz-3G-evt2"},
	{TOPAZ_3G_EVT3,     "topaz-3G-evt3"},
	{TOPAZ_3G_EVT4,     "topaz-3G-evt4"},
	{TOPAZ_3G_DVT,      "topaz-3G-dvt"},
	{TOPAZ_3G_PVT,      "topaz-3G-pvt"},

	/* TODO: Non-standard board strings, to be removed once all copies of
	 * bootie in the wild are updated to use the above format */

	/* WiFi */
	{TOPAZ_PROTO,    "topaz-1stbuild-Wifi"},
	{TOPAZ_PROTO2,   "topaz-2ndbuild-Wifi"},
	{TOPAZ_EVT1,     "topaz-3rdbuild-Wifi"},
	{TOPAZ_EVT2,     "topaz-4thbuild-Wifi"},
	{TOPAZ_EVT3,     "topaz-5thbuild-Wifi"},
	{TOPAZ_DVT,      "topaz-6thbuild-Wifi"},
	{TOPAZ_PVT,      "topaz-7thbuild-Wifi"},
	{TOPAZ_PVT,      "topaz-pvt-Wifi"},

	/* 3G */
	{TOPAZ_3G_PROTO,    "topaz-1stbuild-3G"},
	{TOPAZ_3G_PROTO2,   "topaz-2ndbuild-3G"},
	{TOPAZ_3G_EVT1,     "topaz-3rdbuild-3G"},
	{TOPAZ_3G_EVT2,     "topaz-4thbuild-3G"},
	{TOPAZ_3G_EVT3,     "topaz-5thbuild-3G"},
	{TOPAZ_3G_DVT,      "topaz-6thbuild-3G"},
	{TOPAZ_3G_PVT,      "topaz-7thbuild-3G"},
	{TOPAZ_3G_PVT,      "topaz-pvt-3G"}
};

static int __init boardtype_setup(char *boardtype_str)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(boardtype_tbl); i++)
		if (!strcmp(boardtype_str, boardtype_tbl[i].str))
			board_type = boardtype_tbl[i].type;

	return 0;
}
__setup("boardtype=", boardtype_setup);

/* helper function to manipulate group of gpios (msm_gpiomux)*/
static int configure_gpiomux_gpios(int on, int gpios[], int cnt)
{
	int ret = 0;
	int i;

	for (i = 0; i < cnt; i++) {
		//printk(KERN_ERR "%s:pin(%d):%s\n", __func__, gpios[i], on?"on":"off");
		if (on) {
			ret = msm_gpiomux_get(gpios[i]);
			if (unlikely(ret))
				break;
		} else {
			ret = msm_gpiomux_put(gpios[i]);
			if (unlikely(ret))
				return ret;
		}
	}
	if (ret)
		for (; i >= 0; i--)
			msm_gpiomux_put(gpios[i]);
	return ret;
}
/* helper function to manipulate group of gpios (gpio)*/
static int configure_gpios(int on, int gpios[], int cnt)
{
	int ret = 0;
	int i;

	for (i = 0; i < cnt; i++) {
		//printk(KERN_ERR "%s:pin(%d):%s\n", __func__, gpios[i], on?"request":"free");
		if (on) {
			ret = gpio_request(gpios[i], NULL);
			if (unlikely(ret))
				break;
		} else {
			gpio_free(gpios[i]);
		}
	}
	if (ret)
		for (; i >= 0; i--)
			gpio_free(gpios[i]);
	return ret;
}

#define MPU3050_GPIO_IRQ 125
#define MPU3050_GPIO_FSYNC 119

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static void (*sdc2_status_notify_cb)(int card_present, void *dev_id);
static void *sdc2_status_notify_cb_devid;
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
static void (*sdc5_status_notify_cb)(int card_present, void *dev_id);
static void *sdc5_status_notify_cb_devid;
#endif


static struct msm_spm_platform_data msm_spm_data_v1[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x0F,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0xFFFFFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0xFFFFFFFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0x94,
		.retention_vlevel = 0x81,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x94,
		.collapse_mid_vlevel = 0x8C,

		.vctl_timeout_us = 50,
	},

	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x0F,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0xFFFFFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0xFFFFFFFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x13,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0x94,
		.retention_vlevel = 0x81,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x94,
		.collapse_mid_vlevel = 0x8C,

		.vctl_timeout_us = 50,
	},
};

static struct msm_spm_platform_data msm_spm_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x1C,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0x0C0CFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0x78780FFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0xA0,
		.retention_vlevel = 0x89,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x89,
		.collapse_mid_vlevel = 0x89,

		.vctl_timeout_us = 50,
	},

	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,

#ifdef CONFIG_MSM_AVS_HW
		.reg_init_values[MSM_SPM_REG_SAW_AVS_CTL] = 0x586020FF,
#endif
		.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x1C,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x68,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0x0C0CFFFF,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0x78780FFF,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x13,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x07,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

		.awake_vlevel = 0xA0,
		.retention_vlevel = 0x89,
		.collapse_vlevel = 0x20,
		.retention_mid_vlevel = 0x89,
		.collapse_mid_vlevel = 0x89,

		.vctl_timeout_us = 50,
	},
};

static struct regulator_consumer_supply vreg_consumers_8901_S0[] = {
	REGULATOR_SUPPLY("8901_s0",		NULL),
};
static struct regulator_consumer_supply vreg_consumers_8901_S1[] = {
	REGULATOR_SUPPLY("8901_s1",		NULL),
};

static struct regulator_init_data saw_s0_init_data = {
		.constraints = {
			.name = "8901_s0",
			.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.min_uV = 800000,
			.max_uV = 1325000,
		},
		.consumer_supplies = vreg_consumers_8901_S0,
		.num_consumer_supplies = ARRAY_SIZE(vreg_consumers_8901_S0),
};

static struct regulator_init_data saw_s1_init_data = {
		.constraints = {
			.name = "8901_s1",
			.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.min_uV = 800000,
			.max_uV = 1325000,
		},
		.consumer_supplies = vreg_consumers_8901_S1,
		.num_consumer_supplies = ARRAY_SIZE(vreg_consumers_8901_S1),
};

static struct platform_device msm_device_saw_s0 = {
	.name          = "saw-regulator",
	.id            = 0,
	.dev           = {
		.platform_data = &saw_s0_init_data,
	},
};

static struct platform_device msm_device_saw_s1 = {
	.name          = "saw-regulator",
	.id            = 1,
	.dev           = {
		.platform_data = &saw_s1_init_data,
	},
};

// Fixed regulator
#define GPIO_5V_BIAS     102

static struct regulator_consumer_supply vdd50_boost_supply =
	REGULATOR_SUPPLY("vdd50_boost", NULL);

static struct regulator_init_data vdd50_boost_init_data = {
	.constraints = {
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS,
		.min_uV = 5000000,
		.max_uV = 5000000,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &vdd50_boost_supply,
};

static struct fixed_voltage_config vdd50_boost_config = {
	.supply_name = "vdd50_boost",
	.microvolts = 5000000,
	.gpio = GPIO_5V_BIAS,
	.enable_high = 1,
	.init_data = &vdd50_boost_init_data,
};

static struct platform_device tenderloin_fixed_reg_device = {
		.name          = "reg-fixed-voltage",
		.id            = 0,
		.dev           = {
				.platform_data = &vdd50_boost_config
		}
};

#define GPIO_ETHERNET_RESET_N ((PM8901_MPP_BASE + PM8901_MPPS) + 9)

static struct resource smsc911x_resources[] = {
	[0] = {
		.flags = IORESOURCE_MEM,
		.start = 0x1b800000,
		.end   = 0x1b8000ff
	},
	[1] = {
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWLEVEL,
	},
};

static struct smsc911x_platform_config smsc911x_config = {
	.irq_polarity	= SMSC911X_IRQ_POLARITY_ACTIVE_LOW,
	.irq_type	= SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags		= SMSC911X_USE_16BIT,
	.has_reset_gpio	= 1,
	.reset_gpio	= GPIO_ETHERNET_RESET_N
};

static struct platform_device smsc911x_device = {
	.name          = "smsc911x",
	.id            = 0,
	.num_resources = ARRAY_SIZE(smsc911x_resources),
	.resource      = smsc911x_resources,
	.dev           = {
		.platform_data = &smsc911x_config
	}
};

#ifdef CONFIG_NDUID
static struct nduid_config nduid_cfg[] = {
	{
		.dev_name = "nduid",
	},
};

static struct platform_device nduid_device = {
	.name = "nduid",
	.id   = -1,
	.dev  = {
		.platform_data = &nduid_cfg,
	},
};
#endif

#ifdef CONFIG_USER_PINS
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
static struct user_pin ctp_pins[] = {
	{
		.name = "wake",
  		.gpio = GPIO_CTP_WAKE,
		.act_level = 0,
		.direction = 0,
		.def_level = 1,
		.sysfs_mask = 0660,
	},
};
#endif

static struct user_pin bt_pins[] = {
	{
		.name       =  "reset",
		.gpio       =  BT_RST_N,
		.act_level  =  0, // active low
		.direction  =  0, // output
		.def_level  =  0, // low
		.pin_mode   =  (void *)-1,// undefined
		.sysfs_mask =  0777,
		.options    =  0,
		.irq_handler = NULL,
		.irq_config =  0,
	},
	{
		.name       =  "host_wake",
		.gpio       =  BT_HOST_WAKE,
		.act_level  =  1,  // active high
		.direction  =  1,  // input
		.def_level  = -1,  // undefined
		.pin_mode   =  (void *)-1, // undefined
		.sysfs_mask =  0777,
		.options    =  PIN_IRQ | PIN_WAKEUP_SOURCE,
		.irq_handler = NULL,
		.irq_config = IRQF_TRIGGER_RISING,
		.irq_handle_mode = IRQ_HANDLE_AUTO
	},
};

static struct user_pin_set  board_user_pins_sets[] = {
	{
		.set_name = "bt",
		.num_pins = ARRAY_SIZE(bt_pins),
		.pins     = bt_pins,
	},
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{
		.set_name = "ctp",
		.num_pins = ARRAY_SIZE(ctp_pins),
		.pins = ctp_pins,
	},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
};

static struct user_pins_platform_data board_user_pins_pdata = {
	.num_sets = ARRAY_SIZE(board_user_pins_sets),
	.sets     = board_user_pins_sets,
};

static struct platform_device board_user_pins_device = {
	.name = "user-pins",
	.id   = -1,
	.dev  = {
		.platform_data  = &board_user_pins_pdata,
	}
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

#define QCE_HW_KEY_SUPPORT	0
#define QCE_SHA_HMAC_SUPPORT	0
#define QCE_SHARE_CE_RESOURCE	2
#define QCE_CE_SHARED		1

static struct resource qcrypto_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.name = "crypto_crci_hash",
		.start = DMOV_CE_HASH_CRCI,
		.end = DMOV_CE_HASH_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

static struct resource qcedev_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.name = "crypto_crci_hash",
		.start = DMOV_CE_HASH_CRCI,
		.end = DMOV_CE_HASH_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)

static struct msm_ce_hw_support qcrypto_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = NULL,
};

static struct platform_device qcrypto_device = {
	.name		= "qcrypto",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcrypto_resources),
	.resource	= qcrypto_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcrypto_ce_hw_suppport,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

static struct msm_ce_hw_support qcedev_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = NULL,
};

static struct platform_device qcedev_device = {
	.name		= "qce",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcedev_resources),
	.resource	= qcedev_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcedev_ce_hw_suppport,
	},
};
#endif

static struct msm_rpmrs_level msm_rpmrs_levels[] __initdata = {
	{
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1, 8000, 100000, 1,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1500, 5000, 60100000, 3000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		false,
		1800, 5000, 60350000, 3500,
	},
	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, ACTIVE, MAX, ACTIVE),
		false,
		3800, 4500, 65350000, 5500,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, HSFS_OPEN, MAX, ACTIVE),
		false,
		2800, 2500, 66850000, 4800,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, MAX, ACTIVE),
		false,
		4800, 2000, 71850000, 6800,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		6800, 500, 75850000, 8800,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, RET_HIGH, RET_LOW),
		false,
		7800, 0, 76350000, 9800,
	},
};

static struct msm_rpmrs_platform_data msm_rpmrs_data __initdata = {
	.levels = &msm_rpmrs_levels[0],
	.num_levels = ARRAY_SIZE(msm_rpmrs_levels),
	.vdd_mem_levels  = {
		[MSM_RPMRS_VDD_MEM_RET_LOW]     = 500,
		[MSM_RPMRS_VDD_MEM_RET_HIGH]    = 750,
		[MSM_RPMRS_VDD_MEM_ACTIVE]      = 1000,
		[MSM_RPMRS_VDD_MEM_MAX]         = 1325,
	},
	.vdd_dig_levels = {
		[MSM_RPMRS_VDD_DIG_RET_LOW]     = 500,
		[MSM_RPMRS_VDD_DIG_RET_HIGH]    = 750,
		[MSM_RPMRS_VDD_DIG_ACTIVE]      = 1000,
		[MSM_RPMRS_VDD_DIG_MAX]         = 1250,
	},
	.vdd_mask = 0xFFF,
	.rpmrs_target_id = {
		[MSM_RPMRS_ID_PXO_CLK]          = MSM_RPM_ID_PXO_CLK,
		[MSM_RPMRS_ID_L2_CACHE_CTL]     = MSM_RPM_ID_APPS_L2_CACHE_CTL,
		[MSM_RPMRS_ID_VDD_DIG_0]        = MSM_RPM_ID_SMPS1_0,
		[MSM_RPMRS_ID_VDD_DIG_1]        = MSM_RPM_ID_SMPS1_1,
		[MSM_RPMRS_ID_VDD_MEM_0]        = MSM_RPM_ID_SMPS0_0,
		[MSM_RPMRS_ID_VDD_MEM_1]        = MSM_RPM_ID_SMPS0_1,
		[MSM_RPMRS_ID_RPM_CTL]          = MSM_RPM_ID_TRIGGER_SET_FROM,
	},
};

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_TZ,
};

#ifdef CONFIG_MAX8903B_CHARGER
// available current settings for MAX8903B per HW revision
static int sel_tbl_protos[] = {
	CURRENT_900MA,
	CURRENT_1000MA,
	CURRENT_1500MA,
	CURRENT_2000MA,
};

static int sel_tbl_evt1[] = {
	CURRENT_750MA,
	CURRENT_900MA,
	CURRENT_1500MA,
	CURRENT_1400MA,
};

static int sel_tbl_evt2[] = {
	CURRENT_750MA,
	CURRENT_900MA,
	CURRENT_2000MA,
	CURRENT_1400MA,
};

static int max8903b_control_index(enum max8903b_current value)
{
	int i, index;
	int *table;
	int v = value;

	switch (board_type)
	{
		case TOPAZ_PROTO:
		case TOPAZ_PROTO2:
		case TOPAZ_3G_PROTO:
		case TOPAZ_3G_PROTO2:
			table = sel_tbl_protos;
			break;

		case TOPAZ_EVT1:
			table = sel_tbl_evt1;
			if (v == CURRENT_2000MA)
				v = CURRENT_1500MA;
			break;

		default:
			table = sel_tbl_evt2;
			break;
	}

	// find index from table
	for (i = 0 ; i < 4 ; i++)
	{
		if (v == table[i]) {
			index = i;
			break;
		}

		if (i == 3) {
			index = -EINVAL;  // invalid input
		}
	}

	return index;
}

int max8903b_set_DC_CHG_Mode_current(enum max8903b_current value)
{
	int index;

	index = max8903b_control_index(value);
	switch(index)	{
		case 0: // select output port 0
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_1,0);
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_2,0);
			pr_debug("max8903 MUX : LOW / LOW\n");
			break;
		case 1: // select output port 1
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_1,0);
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_2,1);
			pr_debug("max8903 MUX : LOW / HIGH\n");
			break;
		case 2: // select output port 2
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_1,1);
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_2,0);
			pr_debug("max8903 MUX : HIGH / LOW\n");
			break;
		case 3: // select output port 3
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_1,1);
			gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_2,1);
			pr_debug("max8903 MUX : HIGH / HIGH\n");
			break;
		default:
			printk(KERN_INFO "%s: Invalid current setting, not supported in HW rev.\n", __func__);
			return -1;
			break;
	}
	return 0;
};

int max8903b_request_release_gpios(int request);
void max8903b_suspend_gpio_config(void);

static struct	max8903b_platform_data	max8903b_charger_pdata = {
	.DCM_in 	= MAX8903B_GPIO_DC_CHG_MODE,
	.DCM_in_polarity	= 1,
	.IUSB_in 	= MAX8903B_GPIO_USB_CHG_MODE,
	.IUSB_in_polarity	= 0,
	.USUS_in 	= MAX8903B_GPIO_USB_CHG_SUS,
	.USUS_in_polarity	= 0,
	.CEN_N_in 	= MAX8903B_GPIO_CHG_EN,
	.CEN_N_in_polarity	= 1,
	.DOK_N_out 	= MAX8903B_GPIO_DC_OK,
	.CHG_N_out 	= MAX8903B_GPIO_STATUS_N,
	.FLT_N_out 	= MAX8903B_GPIO_FAULT_N,
	.set_DC_CHG_Mode_current = max8903b_set_DC_CHG_Mode_current,
	.request_release_gpios = max8903b_request_release_gpios,
	.suspend_gpio_config  = max8903b_suspend_gpio_config,
};

static struct 	platform_device 	max8903b_charger_device = {
	.name               = "max8903b_chg",
	.id                 = 0,
	.dev.platform_data  = &max8903b_charger_pdata,
};

int max8903b_request_release_gpios(int request)
{
	static int gpio_array_init = 0;
	static int is_gpio_active = 0;
	static int max8903b_gpios[9];
	int rc = 0;

	if (gpio_array_init == 0) {
		max8903b_gpios[0] = 	max8903b_charger_pdata.DCM_in;
		max8903b_gpios[1] = 	max8903b_charger_pdata.IUSB_in;
		max8903b_gpios[2] = 	max8903b_charger_pdata.USUS_in;
		max8903b_gpios[3] =	max8903b_charger_pdata.CEN_N_in;
		max8903b_gpios[4] =	max8903b_charger_pdata.DOK_N_out;
		max8903b_gpios[5] =	max8903b_charger_pdata.CHG_N_out;
		max8903b_gpios[6] =	max8903b_charger_pdata.FLT_N_out;
		max8903b_gpios[7] =	MAX8903B_GPIO_CHG_D_ISET_1;
		max8903b_gpios[8] =	MAX8903B_GPIO_CHG_D_ISET_2;

		gpio_array_init = 1;
	}

	if (request) {   // reqest gpios
		if (is_gpio_active == 0) {
			rc = configure_gpios(1, max8903b_gpios, ARRAY_SIZE(max8903b_gpios));
			if (rc == 0)
				is_gpio_active = request;  // requested
		} else {
			printk("%s: GPIOs were already active(requested) !!!\n", __func__);
		}
	} else {    // free gpios
		if (is_gpio_active != 0) {
			rc = configure_gpios(0, max8903b_gpios, ARRAY_SIZE(max8903b_gpios));
			if (rc == 0)
				is_gpio_active = request;  // freed
		} else {
			printk("%s: GPIOs were already released(freed) !!!\n", __func__);
		}
	}

	return rc;
}

/* Set necessary GPIOs for suspend mode state. */
void max8903b_suspend_gpio_config(void)
{
	gpio_set_value(MAX8903B_GPIO_USB_CHG_MODE, 1);
	gpio_set_value(MAX8903B_GPIO_CHG_EN, 0);

	gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_1, 0);
	gpio_set_value(MAX8903B_GPIO_CHG_D_ISET_2, 0);
}

void max8903b_set_connected_ps (unsigned connected)
{
	max8903b_ps_connected = connected;

	if (!connected) {
		max8903b_disable_charge();
		max8903b_vbus_draw_ma_max = 0;
	} else if (connected & MAX8903B_CONNECTED_PS_DOCK) {
		max8903b_set_charge_ma(MAX8903B_DOCK_DRAW_MA);
	}
}
EXPORT_SYMBOL (max8903b_set_connected_ps);

/* Callback for msm72k_otg to notify charge supplied by phy.
 * Current draw defaults to 500mA. WebOS seems to use the sysfs to
 * set current_limit on usb plugin.
 */
void max8903b_set_vbus_draw (unsigned ma)
{
	max8903b_vbus_draw_ma = ma;

	if (ma > max8903b_vbus_draw_ma_max) {
		max8903b_vbus_draw_ma_max = ma;
	}

	if (!(max8903b_ps_connected & MAX8903B_CONNECTED_PS_DOCK)) {
		if (max8903b_ps_connected & MAX8903B_CONNECTED_PS_AC) {
			max8903b_set_charge_ma(max8903b_vbus_draw_ma_max);
		} else {
			max8903b_set_charge_ma(ma);
		}
	}
}
#endif  /* CONFIG_MAX8903B_CHARGER */

#define PID_MAGIC_ID		0x71432909
#define SERIAL_NUM_MAGIC_ID	0x61945374
#define SERIAL_NUMBER_LENGTH	127
#define DLOAD_USB_BASE_ADD	0x2A05F0C8

struct magic_num_struct {
	uint32_t pid;
	uint32_t serial_num;
};

struct dload_struct {
	uint32_t	reserved1;
	uint32_t	reserved2;
	uint32_t	reserved3;
	uint16_t	reserved4;
	uint16_t	pid;
	char		serial_number[SERIAL_NUMBER_LENGTH];
	uint16_t	reserved5;
	struct magic_num_struct
			magic_struct;
};

static int usb_diag_update_pid_and_serial_num(uint32_t pid, const char *snum)
{
	struct dload_struct __iomem *dload = 0;

	dload = ioremap(DLOAD_USB_BASE_ADD, sizeof(*dload));
	if (!dload) {
		pr_err("%s: cannot remap I/O memory region: %08x\n",
					__func__, DLOAD_USB_BASE_ADD);
		return -ENXIO;
	}

	pr_debug("%s: dload:%p pid:%x serial_num:%s\n",
				__func__, dload, pid, snum);
	
	dload->magic_struct.pid = PID_MAGIC_ID;
	dload->pid = pid;

	
	dload->magic_struct.serial_num = 0;
	if (!snum)
		return 0;

	dload->magic_struct.serial_num = SERIAL_NUM_MAGIC_ID;
	strncpy(dload->serial_number, snum, SERIAL_NUMBER_LENGTH);
	dload->serial_number[SERIAL_NUMBER_LENGTH - 1] = '\0';

	iounmap(dload);

	return 0;
}

static struct android_usb_platform_data android_usb_pdata = {
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_WEBCAM_MT9M113
static int camera_mt9m113_gpios[] = {
	TENDERLOIN_CAM_I2C_DATA,
	TENDERLOIN_CAM_I2C_CLK,
	TENDERLOIN_CAMIF_MCLK,
	TENDERLOIN_WEBCAM_RST,
	TENDERLOIN_WEBCAM_PWDN,
};

struct regulator *votg_lvs0 = NULL;
struct regulator *votg_vreg_l11 = NULL;
bool gpios_web_cam_mt9m113_on = false;

static int config_camera_on_gpios_web_cam_mt9m113(void)
{
	int rc = 0;

	printk("+++ %s\n", __func__);
	if ( !gpios_web_cam_mt9m113_on ) {

		configure_gpiomux_gpios(1, camera_mt9m113_gpios,
			ARRAY_SIZE(camera_mt9m113_gpios));

		votg_lvs0 = regulator_get(NULL, "8058_lvs0");
		if (IS_ERR_OR_NULL(votg_lvs0)) {
			printk("%s: unable to get votg_lvs0\n", __func__);
			goto err;
		}

		if (regulator_enable(votg_lvs0)) {
			printk("%s:Unable to enable the regulator votg_lvs0\n", __func__);
			goto err1;
		}
		else {
			printk("%s:enable the regulator votg_lvs0 succeed\n", __func__);
		}

		votg_vreg_l11 = regulator_get(NULL, "8058_l11");
		if (IS_ERR_OR_NULL(votg_vreg_l11)) {
			printk("%s: unable to get votg_vreg_l11\n", __func__);
			goto err1;
		}

		if(regulator_set_voltage(votg_vreg_l11, 2850000, 2850000)) {
			printk("%s: Unable to set regulator voltage:"
			" votg_l11\n", __func__);
			goto err2;
		}

		if (regulator_enable(votg_vreg_l11)) {
			printk("%s:Unable to enable the regulator votg_vreg_l11\n", __func__);
			goto err2;
		}
		else {
			printk("%s:enable the regulator votg_vreg_l11 succeed\n", __func__);
		}

		gpios_web_cam_mt9m113_on = true;
	}

	printk("--- %s\n", __func__);
	return 0;

err2:
	regulator_disable(votg_vreg_l11);
	regulator_put(votg_vreg_l11);
	votg_vreg_l11 = NULL;

err1:
	regulator_disable(votg_lvs0);
	regulator_put(votg_lvs0);
	votg_lvs0 = NULL;

err:
	configure_gpiomux_gpios(0, camera_mt9m113_gpios,
			ARRAY_SIZE(camera_mt9m113_gpios));

	//If error code is not specified return -1
	if (!rc) {
		rc = -1;
	}

	printk("--- %s\n", __func__);
	return rc;
}

static void config_camera_off_gpios_web_cam_mt9m113(void)
{
	printk("+++ %s\n", __func__);

	if (gpios_web_cam_mt9m113_on) {

		configure_gpiomux_gpios(0, camera_mt9m113_gpios,
				ARRAY_SIZE(camera_mt9m113_gpios));

		if (IS_ERR_OR_NULL(votg_lvs0)) {
			printk("%s: unable to get votg_lvs0\n", __func__);
		} else {

			if (regulator_disable(votg_lvs0)) {
				printk("%s:Unable to disable the regulator: votg_lvs0\n", __func__);
			}
			else {
				printk("%s:disable the regulator: votg_lvs0 succeed\n", __func__);
			}

			regulator_put(votg_lvs0);
			votg_lvs0 = NULL;
		}

		if (IS_ERR_OR_NULL(votg_vreg_l11)) {
			printk("%s: unable to get votg_vreg_l11\n", __func__);
		} else {

			if (regulator_disable(votg_vreg_l11)) {
				printk("%s:Unable to disable the regulator: votg_vreg_l11\n", __func__);
			}
			else {
				printk("%s:disable the regulator: votg_vreg_l11 succeed\n", __func__);
			}

			regulator_put(votg_vreg_l11);
			votg_vreg_l11 = NULL;
		}

		gpios_web_cam_mt9m113_on = false;
	}

	printk("--- %s\n", __func__);
}

struct msm_camera_device_platform_data msm_camera_device_data_web_cam_mt9m113 = {
	.camera_gpio_on  = config_camera_on_gpios_web_cam_mt9m113,
	.camera_gpio_off = config_camera_off_gpios_web_cam_mt9m113,
	.ioext.csiphy = 0x04900000,
	.ioext.csisz  = 0x00000400,
	.ioext.csiirq = CSI_1_IRQ,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 228570000,
};

struct resource msm_camera_resources[] = {
	{
		.start	= 0x04500000,
		.end	= 0x04500000 + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= VFE_IRQ,
		.end	= VFE_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct msm_camera_sensor_flash_data msm_flash_none = {
       .flash_type = MSM_CAMERA_FLASH_NONE,
       .flash_src  = NULL
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9m113_data = {
	.sensor_name	= "mt9m113",
	.sensor_reset	= 106,
	.sensor_pwd		= 107,
	.vcm_pwd		= 1,
	.vcm_enable		= 0,
	.pdata			= &msm_camera_device_data_web_cam_mt9m113,
	.resource		= msm_camera_resources,
	.num_resources	= ARRAY_SIZE(msm_camera_resources),
	.flash_data		= &msm_flash_none,
	.csi_if			= 1
};

struct platform_device msm_camera_sensor_webcam_mt9m113 = {
	.name	= "msm_camera_mt9m113",
	.dev	= {
		.platform_data = &msm_camera_sensor_mt9m113_data,
	},
};
#endif

static struct i2c_board_info msm_camera_boardinfo[] __initdata = {
	{
#ifdef CONFIG_WEBCAM_MT9M113
		I2C_BOARD_INFO("mt9m113", 0x78),
#endif
	},
};
#endif

#ifdef CONFIG_MSM_VPE
static struct resource msm_vpe_resources[] = {
	{
		.start	= 0x05300000,
		.end	= 0x05300000 + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= INT_VPE,
		.end	= INT_VPE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device msm_vpe_device = {
	.name = "msm_vpe",
	.id   = 0,
	.num_resources = ARRAY_SIZE(msm_vpe_resources),
	.resource = msm_vpe_resources,
};
#endif

#ifdef CONFIG_MSM_GEMINI
static struct resource msm_gemini_resources[] = {
	{
		.start  = 0x04600000,
		.end    = 0x04600000 + SZ_1M - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = INT_JPEG,
		.end    = INT_JPEG,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device msm_gemini_device = {
	.name           = "msm_gemini",
	.resource       = msm_gemini_resources,
	.num_resources  = ARRAY_SIZE(msm_gemini_resources),
};
#endif

#ifdef CONFIG_TPS65200
static struct tps65200_platform_data tps65200_data = {
	.charger_check = 1,
	.gpio_chg_stat = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, TENDERLOIN_CHG_STAT),
	.gpio_chg_int  = MSM_GPIO_TO_INT(TENDERLOIN_GPIO_CHG_INT),
};

#ifdef CONFIG_SUPPORT_DQ_BATTERY
static int __init check_dq_setup(char *str)
{
	if (!strcmp(str, "PASS"))
		tps65200_data.dq_result = 1;
	else
		tps65200_data.dq_result = 0;

	return 1;
}
__setup("androidboot.dq=", check_dq_setup);
#endif

static struct i2c_board_info msm_tps_65200_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO("tps65200", 0xD4 >> 1),
		.platform_data = &tps65200_data,
	},
};
#endif

#ifdef CONFIG_I2C_QUP
static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
}

static struct msm_i2c_platform_data msm_gsbi3_qup_i2c_pdata = {
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi4_qup_i2c_pdata = {
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi7_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi8_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi9_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

int board_gsbi10_init(void);

static struct msm_i2c_platform_data msm_gsbi10_qup_i2c_pdata = {
	.clk_freq = 400000,
	.src_clk_rate = 24000000,
	.use_gsbi_shared_mode = 1,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
static struct msm_spi_platform_data msm_gsbi1_qup_spi_pdata = {
	.max_clock_speed = 24000000,
};
#endif

#ifdef CONFIG_I2C_SSBI
/* CODEC/TSSC SSBI */
static struct msm_i2c_ssbi_platform_data msm_ssbi3_pdata = {
	.controller_type = MSM_SBI_CTRL_SSBI,
};
#endif

#ifdef CONFIG_BATTERY_MSM
static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.avail_chg_sources = AC_CHG,
};

static struct platform_device msm_batt_device = {
	.name              = "msm-battery",
	.id                = -1,
	.dev.platform_data = &msm_psy_batt_data,
};
#endif

#ifdef CONFIG_MSM_DSPS

static struct dsps_gpio_info dsps_tenderloin_gpios[] = {
#ifndef CONFIG_MACH_TENDERLOIN
	{
		.name = "compass_rst_n",
		.num = GPIO_COMPASS_RST_N,
		.on_val = 1,	
		.off_val = 0,	
	},
	{
		.name = "gpio_r_altimeter_reset_n",
		.num = GPIO_R_ALTIMETER_RESET_N,
		.on_val = 1,	
		.off_val = 0,	
	}
#endif
};

static void __init msm8x60_init_dsps(void)
{
	struct msm_dsps_platform_data *pdata =
		msm_dsps_device.dev.platform_data;

	pdata->pil_name = DSPS_PIL_GENERIC_NAME;
	pdata->gpios = dsps_tenderloin_gpios;
	pdata->gpios_num = ARRAY_SIZE(dsps_tenderloin_gpios);

        platform_device_register(&msm_dsps_device);
}
#endif 

#define MSM_PMEM_KERNEL_EBI1_SIZE 0x600000

#define MSM_SMI_BASE		0x38000000
#define MSM_SMI_SIZE		0x4000000

#define KERNEL_SMI_BASE		(MSM_SMI_BASE)
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
#define KERNEL_SMI_SIZE		0x000000
#else
#define KERNEL_SMI_SIZE		0x600000
#endif

#define USER_SMI_BASE		(KERNEL_SMI_BASE + KERNEL_SMI_SIZE)
#define USER_SMI_SIZE		(MSM_SMI_SIZE - KERNEL_SMI_SIZE)
#define MSM_PMEM_SMIPOOL_SIZE	USER_SMI_SIZE

#define MSM_ION_SF_SIZE		0x7800000 /* 120MB */
#define MSM_ION_CAMERA_SIZE	0x1000000 /* 16MB */
#define MSM_ION_AUDIO_SIZE	0x28B000

#ifdef CONFIG_MSM_CP
#define MSM_ION_HOLE_SIZE	SZ_128K /* (128KB) */
#else
#define MSM_ION_HOLE_SIZE	0
#endif

#define MSM_MM_FW_SIZE		(0x200000 - MSM_ION_HOLE_SIZE) /*(2MB-128KB)*/
#define MSM_ION_MM_SIZE		0x4000000  /* (64MB) */
#define MSM_ION_MFC_SIZE	SZ_8K

#define MSM_MM_FW_BASE		MSM_SMI_BASE
#define MSM_ION_HOLE_BASE	(MSM_MM_FW_BASE + MSM_MM_FW_SIZE)
#define MSM_ION_MM_BASE		(MSM_ION_HOLE_BASE + MSM_ION_HOLE_SIZE)
#define MSM_ION_MFC_BASE	(MSM_ION_MM_BASE + MSM_ION_MM_SIZE)

#ifdef CONFIG_MSM_CP
#define SECURE_BASE  (MSM_ION_HOLE_BASE)
#define SECURE_SIZE  (MSM_ION_MM_SIZE + MSM_ION_HOLE_SIZE)
#else
#define SECURE_BASE  (MSM_MM_FW_BASE)
#define SECURE_SIZE  (MSM_ION_MM_SIZE + MSM_MM_FW_SIZE)
#endif

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define MSM_ION_HEAP_NUM	7
#else
#define MSM_ION_HEAP_NUM	1
#endif

static unsigned fb_size;
static int __init fb_size_setup(char *p)
{
	fb_size = memparse(p, NULL);
	return 0;
}
early_param("fb_size", fb_size_setup);

static unsigned pmem_kernel_ebi1_size = MSM_PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);

#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
#ifndef CONFIG_ANDROID_PMEM_ION_WRAPPER
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
#else
	.ion_heap_id = ION_CP_MM_HEAP_ID,
#endif
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

#define PMEM_BUS_WIDTH(_bw) \
	{ \
		.vectors = &(struct msm_bus_vectors){ \
			.src = MSM_BUS_MASTER_AMPSS_M0, \
			.dst = MSM_BUS_SLAVE_SMI, \
			.ib = (_bw), \
			.ab = 0, \
		}, \
	.num_paths = 1, \
	}

static struct msm_bus_paths mem_smi_table[] = {
	[0] = PMEM_BUS_WIDTH(0), 
	[1] = PMEM_BUS_WIDTH(1), 
};

static struct msm_bus_scale_pdata smi_client_pdata = {
	.usecase = mem_smi_table,
	.num_usecases = ARRAY_SIZE(mem_smi_table),
	.name = "mem_smi",
};

int request_smi_region(void *data)
{
	int bus_id = (int) data;

	msm_bus_scale_client_update_request(bus_id, 1);
	return 0;
}

int release_smi_region(void *data)
{
	int bus_id = (int) data;

	msm_bus_scale_client_update_request(bus_id, 0);
	return 0;
}

void *setup_smi_region(void)
{
	return (void *)msm_bus_scale_register_client(&smi_client_pdata);
}
#endif 

static void __init msm8x60_allocate_memory_regions(void)
{
          msm8x60_allocate_fb_region();
}

#define GSBI1_PHYS			0x16000000
#define GSBI8_PHYS			0x19800000
#define GSBI_CTRL			0x0
#define PROTOCOL_CODE(code)		(((code) & 0x7) << 4)
#define UART_WITH_FLOW_CONTROL		0x4
#define I2C_ON_2_PORTS_UART		0x6

static DEFINE_MUTEX(gsbi_init_lock);

int board_gsbi_init(int gsbi, int *inited, u32 prot)
{
	int rc;
	u32 gsbi_phys;
	void *gsbi_virt;

	pr_debug("%s: gsbi=%d inited=%d\n", __func__, gsbi, *inited);

	mutex_lock(&gsbi_init_lock);

	if (*inited) {
		rc = 0;
		goto exit;
	}

	pr_debug("%s: gsbi=%d prot=%x", __func__, gsbi, prot);

	if ((gsbi >= 1) && (gsbi <= 7))
		gsbi_phys = GSBI1_PHYS + ((gsbi - 1) * 0x100000);

	else if ((gsbi >= 8) && (gsbi <= 12))
		gsbi_phys = GSBI8_PHYS + ((gsbi - 8) * 0x100000);

	else {
		rc = -EINVAL;
		goto exit;
	}

	gsbi_virt = ioremap(gsbi_phys, 4);
	if (!gsbi_virt) {
		pr_err("error remapping address 0x%08x\n", gsbi_phys);
		rc = -ENXIO;
		goto exit;
	}

	pr_debug("%s: %08x=%08x\n", __func__, gsbi_phys + GSBI_CTRL,
			PROTOCOL_CODE(prot));
	writel(PROTOCOL_CODE(prot), gsbi_virt + GSBI_CTRL);
	iounmap(gsbi_virt);
	rc = 0;
exit:
	mutex_unlock(&gsbi_init_lock);

	return (rc);
}

static int board_gsbi6_init(void)
{
	static int inited = 0;

	return (board_gsbi_init(6, &inited, UART_WITH_FLOW_CONTROL));
}

#ifdef CONFIG_BT
#ifdef CONFIG_HSUART

static int btuart_pin_mux(int on)
{
	int ret = 0;
	int gpios[] = {UART1DM_CTS_GPIO, UART1DM_RX_GPIO, UART1DM_TX_GPIO};

	printk(KERN_INFO "btuart_pin_mux: %s\n", on?"on":"off");

	ret = configure_gpiomux_gpios(on, gpios, ARRAY_SIZE(gpios));

	return ret;
}

static int btuart_deassert_rts(int deassert)
{
	int rc = 0;
	static int active = 0;
	printk(KERN_INFO "btuart_deassert_rts: %d(%s)\n", deassert, deassert?"put":"get");
	if (deassert) {
		if (active) {
			rc = msm_gpiomux_put(UART1DM_RTS_GPIO);
			if (!rc)
				active = 0;
		}
	} else {
		if (!active) {
			rc = msm_gpiomux_get(UART1DM_RTS_GPIO);
			if (!rc)
				active = 1;
		}
	}

	return rc;
}


/*
 * BT High speed UART interface
 */
static struct hsuart_platform_data btuart_data = {
	.dev_name   = "ttyHS0",
	.uart_mode  = HSUART_MODE_FLOW_CTRL_NONE | HSUART_MODE_PARITY_NONE,
	.uart_speed = HSUART_SPEED_115K,
	.options    = HSUART_OPTION_DEFERRED_LOAD | HSUART_OPTION_TX_PIO | HSUART_OPTION_RX_DM ,

	.tx_buf_size = 512,
	.tx_buf_num  = 64,
	.rx_buf_size = 512,
	.rx_buf_num  = 64,
	.max_packet_size = 450, // ~450
	.min_packet_size = 6,   // min packet size
	.rx_latency      = 10, // in bytes at current speed
	.p_board_pin_mux_cb = btuart_pin_mux,
	.p_board_config_gsbi_cb = board_gsbi6_init,
	.p_board_rts_pin_deassert_cb = btuart_deassert_rts,
//	.rts_pin         = 145,   // uart rts line pin
};

static u64 btuart_dmamask = ~(u32)0;
static struct platform_device btuart_device = {
	.name = "hsuart_tty",
	.id   =  0, // configure UART2 as hi speed uart
	.dev  = {
		.dma_mask           = &btuart_dmamask,
		.coherent_dma_mask  = 0xffffffff,
		.platform_data      = &btuart_data,
	}
};
#endif // CONFIG_HSUART

static struct platform_device tenderloin_rfkill = {
	.name = "tenderloin_rfkill",
	.id = -1,
};
#endif


#ifdef CONFIG_BATTERY_MSM8X60
static struct msm_charger_platform_data msm_charger_data = {
	.safety_time = 180,
	.update_time = 1,
	.max_voltage = 4200,
	.min_voltage = 3200,
};

static struct platform_device msm_charger_device = {
	.name = "msm-charger",
	.id = -1,
	.dev = {
		.platform_data = &msm_charger_data,
	}
};
#endif

static struct platform_device rpm_regulator_early_device __devinitdata = {
	.name	= "rpm-regulator",
	.id	= 0,
	.dev	= {
    .platform_data = &tenderloin_rpm_regulator_early_pdata,
	},
};

static struct platform_device rpm_regulator_device __devinitdata = {
	.name	= "rpm-regulator",
	.id	= 1,
	.dev	= {
		.platform_data = &tenderloin_rpm_regulator_pdata,
	},
};

static struct platform_device *early_devices[] __initdata = {
	&msm_device_saw_s0,
	&msm_device_saw_s1,
        &tenderloin_fixed_reg_device,
#ifdef CONFIG_MSM_BUS_SCALING
	&msm_bus_apps_fabric,
	&msm_bus_sys_fabric,
	&msm_bus_mm_fabric,
	&msm_bus_sys_fpb,
	&msm_bus_cpss_fpb,
#endif
	&msm_device_dmov_adm0,
	&msm_device_dmov_adm1,
};

static struct tsens_platform_data tenderloin_tsens_pdata  = {
	.tsens_factor           = 1000,
	.hw_type                = MSM_8660,
	.tsens_num_sensor       = 1,
	.slope                  = {702},
};

#if defined(CONFIG_MSM_RTB)
static struct msm_rtb_platform_data msm_rtb_pdata = {
	.size = SZ_1M,
};

static int __init msm_rtb_set_buffer_size(char *p)
{
	int s;

	s = memparse(p, NULL);
	msm_rtb_pdata.size = ALIGN(s, SZ_4K);
	return 0;
}
early_param("msm_rtb_size", msm_rtb_set_buffer_size);


static struct platform_device msm_rtb_device = {
	.name           = "msm_rtb",
	.id             = -1,
	.dev            = {
		.platform_data = &msm_rtb_pdata,
	},
};
#endif

#ifdef CONFIG_MSM_SDIO_AL

static unsigned mdm2ap_status = 140;

static int configure_mdm2ap_status(int on)
{
	int ret = 0;
	if (on)
		ret = msm_gpiomux_get(mdm2ap_status);
	else
		ret = msm_gpiomux_put(mdm2ap_status);

	if (ret)
		pr_err("%s: mdm2ap_status config failed, on = %d\n", __func__,
		       on);

	return ret;
}


static int get_mdm2ap_status(void)
{
	return gpio_get_value(mdm2ap_status);
}

static struct sdio_al_platform_data sdio_al_pdata = {
	.config_mdm2ap_status = configure_mdm2ap_status,
	.get_mdm2ap_status = get_mdm2ap_status,
	.allow_sdioc_version_major_2 = 0,
	.peer_sdioc_version_minor = 0x0202,
	.peer_sdioc_version_major = 0x0004,
	.peer_sdioc_boot_version_minor = 0x0001,
	.peer_sdioc_boot_version_major = 0x0003
};

struct platform_device msm_device_sdio_al = {
	.name = "msm_sdio_al",
	.id = -1,
	.dev		= {
		.parent = &msm_charm_modem.dev,
		.platform_data	= &sdio_al_pdata,
	},
};

#endif 


#ifdef CONFIG_SND_SOC_MSM8660_APQ
static struct platform_device *dragon_alsa_devices[] __initdata = {
	&msm_pcm,
	&msm_pcm_routing,
	&msm_cpudai0,
	&msm_cpudai1,
	&msm_cpudai_hdmi_rx,
	&msm_cpudai_bt_rx,
	&msm_cpudai_bt_tx,
	&msm_cpudai_fm_rx,
	&msm_cpudai_fm_tx,
	&msm_cpu_fe,
	&msm_stub_codec,
	&msm_lpa_pcm,
};
#endif

static struct platform_device *asoc_devices[] __initdata = {
	&asoc_msm_pcm,
	&asoc_msm_dai0,
	&asoc_msm_dai1,
};

#ifdef CONFIG_QSEECOM
static struct msm_bus_vectors qseecom_clks_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = 0,
		.ab = 0,
	},
};

static struct msm_bus_vectors qseecom_enable_dfab_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = (492 * 8) * 1000000UL,
		.ab = (492 * 8) *  100000UL,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = 0,
		.ab = 0,
	},
};

static struct msm_bus_vectors qseecom_enable_sfpb_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = (64 * 8) * 1000000UL,
		.ab = (64 * 8) *  100000UL,
	},
};

static struct msm_bus_paths qseecom_hw_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(qseecom_clks_init_vectors),
		qseecom_clks_init_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_dfab_vectors),
		qseecom_enable_sfpb_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_sfpb_vectors),
		qseecom_enable_sfpb_vectors,
	},
};

static struct msm_bus_scale_pdata qseecom_bus_pdata = {
	.usecase = qseecom_hw_bus_scale_usecases,
	.num_usecases = ARRAY_SIZE(qseecom_hw_bus_scale_usecases),
	.name = "qsee",
};

static struct platform_device qseecom_device = {
	.name		= "qseecom",
	.id		= -1,
	.dev		= {
		.platform_data = &qseecom_bus_pdata,
	},
};
#endif

#ifdef CONFIG_ION_MSM
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct ion_cp_heap_pdata cp_mm_ion_pdata = {
	.permission_type = IPT_TYPE_MM_CARVEOUT,
	.align = PAGE_SIZE,
	.request_region = request_smi_region,
	.release_region = release_smi_region,
	.setup_region = setup_smi_region,
	.secure_base = SECURE_BASE,
	.secure_size = SECURE_SIZE,
};

static struct ion_cp_heap_pdata cp_mfc_ion_pdata = {
	.permission_type = IPT_TYPE_MFC_SHAREDMEM,
	.align = PAGE_SIZE,
};

static struct ion_co_heap_pdata mm_fw_co_ion_pdata = {
	.adjacent_mem_id = ION_CP_MM_HEAP_ID,
};

static struct ion_co_heap_pdata co_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
};
#endif

/**
 * These heaps are listed in the order they will be allocated. Due to
 * video hardware restrictions and content protection the FW heap has to
 * be allocated adjacent (below) the MM heap and the MFC heap has to be
 * allocated after the MM heap to ensure MFC heap is not more than 256MB
 * away from the base address of the FW heap.
 * However, the order of FW heap and MM heap doesn't matter since these
 * two heaps are taken care of by separate code to ensure they are adjacent
 * to each other.
 * Don't swap the order unless you know what you are doing!
 */
struct ion_platform_heap msm8x60_heaps [] = {
		{
			.id	= ION_SYSTEM_HEAP_ID,
			.type	= ION_HEAP_TYPE_SYSTEM,
			.name	= ION_VMALLOC_HEAP_NAME,
		},
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
		{
			.id	= ION_CP_MM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MM_HEAP_NAME,
			.base   = MSM_ION_MM_BASE,
			.size	= MSM_ION_MM_SIZE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = (void *) &cp_mm_ion_pdata,
		},
		{
			.id	= ION_MM_FIRMWARE_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_MM_FIRMWARE_HEAP_NAME,
			.base  = MSM_MM_FW_BASE,
			.size  = MSM_MM_FW_SIZE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = (void *) &mm_fw_co_ion_pdata,
		},
		{
			.id	= ION_CP_MFC_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MFC_HEAP_NAME,
			.base  = MSM_ION_MFC_BASE,
			.size	= MSM_ION_MFC_SIZE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = (void *) &cp_mfc_ion_pdata,
		},
		{
			.id	= ION_SF_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_SF_HEAP_NAME,
			.size	= MSM_ION_SF_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *)&co_ion_pdata,
		},
		{
			.id	= ION_CAMERA_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_CAMERA_HEAP_NAME,
			.size	= MSM_ION_CAMERA_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = &co_ion_pdata,
		},
		{
			.id	= ION_AUDIO_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_AUDIO_HEAP_NAME,
			.size	= MSM_ION_AUDIO_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *)&co_ion_pdata,
		},
#endif
};

static struct ion_platform_data ion_pdata = {
	.nr = MSM_ION_HEAP_NUM,
	.heaps = msm8x60_heaps,
};

static struct platform_device ion_dev = {
	.name = "ion-msm",
	.id = 1,
	.dev = { .platform_data = &ion_pdata },
};
#endif

static struct platform_device *tenderloin_devices[] __initdata = {

	&msm8x60_device_acpuclk,
	&msm_device_smd,
        &msm_device_uart_dm12,
	&msm_device_otg,
	&msm_device_gadget_peripheral,
	&android_usb_device,
	&msm_pil_q6v3,
	&msm_pil_modem,
	&msm_pil_tzapps,
#ifdef CONFIG_I2C_QUP
        &msm_gsbi3_qup_i2c_device,
        &msm_gsbi4_qup_i2c_device,
        &msm_gsbi7_qup_i2c_device,
        &msm_gsbi8_qup_i2c_device,
        &msm_gsbi9_qup_i2c_device,
	&msm_gsbi10_qup_i2c_device,
#endif
#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
	&msm_gsbi1_qup_spi_device,
#endif
#ifdef CONFIG_SERIAL_MSM_HS 
        &msm_device_uart_dm1,
#endif
#ifdef CONFIG_BT
#ifdef CONFIG_HSUART
	&btuart_device,
#endif
	&tenderloin_rfkill,
#endif

#ifdef CONFIG_MSM_SSBI
	&msm_device_ssbi_pmic1,
	&msm_device_ssbi_pmic2,
#endif
#ifdef CONFIG_I2C_SSBI
	&msm_device_ssbi3,
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	&max8903b_charger_device,
#endif
#ifdef CONFIG_BATTERY_MSM
	&msm_batt_device,
#endif
#ifdef CONFIG_ANDROID_PMEM
	&android_pmem_adsp_device,
#endif
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_WEBCAM_MT9M113
	&msm_camera_sensor_webcam_mt9m113,
#endif
#endif //CONFIG_MSM_CAMERA
#ifdef CONFIG_MSM_GEMINI
	&msm_gemini_device,
#endif
#ifdef CONFIG_MSM_VPE
	&msm_vpe_device,
#endif
	&msm_device_vidc,
#ifdef CONFIG_USER_PINS
	&board_user_pins_device,
#endif
#ifdef CONFIG_NDUID
	&nduid_device,
#endif
#ifdef CONFIG_SENSORS_MSM_ADC
	&msm_adc_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif

#ifdef CONFIG_HW_RANDOM_MSM
	&msm_device_rng,
#endif
	&msm8660_rpm_device,
#ifdef CONFIG_ION_MSM
	&ion_dev,
#endif
	&msm8660_device_watchdog,
#ifdef CONFIG_MSM_RTB
	&msm_rtb_device,
#endif
};

static struct memtype_reserve msm8x60_reserve_table[] __initdata = {
	/* Kernel SMI memory pool for video core, used for firmware */
	/* and encoder, decoder scratch buffers */
	/* Kernel SMI memory pool should always precede the user space */
	/* SMI memory pool, as the video core will use offset address */
	/* from the Firmware base */
	[MEMTYPE_SMI_KERNEL] = {
		.start	=	KERNEL_SMI_BASE,
		.limit	=	KERNEL_SMI_SIZE,
		.size	=	KERNEL_SMI_SIZE,
		.flags	=	MEMTYPE_FLAGS_FIXED,
	},
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

static void reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	unsigned int i;

	/* Verify size of heap is a multiple of 64K */
	for (i = 0; i < ion_pdata.nr; i++) {
		struct ion_platform_heap *heap = &(ion_pdata.heaps[i]);

		if (heap->extra_data && heap->type == (enum ion_heap_type)ION_HEAP_TYPE_CP) {
			int map_all = ((struct ion_cp_heap_pdata *)
				heap->extra_data)->iommu_map_all;

			if (map_all && (heap->size & (SZ_64K-1))) {
				heap->size = ALIGN(heap->size, SZ_64K);
				pr_err("Heap %s size is not a multiple of 64K. Adjusting size to %x\n",
					heap->name, heap->size);

			}
		}
	}

	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_SF_SIZE;
	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_CAMERA_SIZE;
	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_AUDIO_SIZE;
#endif
}

static void __init msm8x60_calculate_reserve_sizes(void)
{
	reserve_ion_memory();
}

static int msm8x60_paddr_to_memtype(unsigned int paddr)
{
	if (paddr >= 0x40000000 && paddr < 0x80000000)
		return MEMTYPE_EBI1;
	if (paddr >= 0x38000000 && paddr < 0x40000000)
		return MEMTYPE_SMI;
	return MEMTYPE_NONE;
}

static struct reserve_info msm8x60_reserve_info __initdata = {
	.memtype_reserve_table = msm8x60_reserve_table,
	.calculate_reserve_sizes = msm8x60_calculate_reserve_sizes,
	.paddr_to_memtype = msm8x60_paddr_to_memtype,
};

static void __init tenderloin_early_memory(void)
{
	reserve_info = &msm8x60_reserve_info;
}

static void __init tenderloin_reserve(void)
{
	msm_reserve();
}

#define EXT_CHG_VALID_MPP 10
#define EXT_CHG_VALID_MPP_2 11

#define PM_GPIO_CDC_RST_N 20
#define GPIO_CDC_RST_N PM8058_GPIO_PM_TO_SYS(PM_GPIO_CDC_RST_N)

#if HASTIMPANI
static struct regulator *vreg_timpani_1;
static struct regulator *vreg_timpani_2;

static unsigned int msm_timpani_setup_power(void)
{
	int rc;

	vreg_timpani_1 = regulator_get(NULL, "8058_l0");
	if (IS_ERR(vreg_timpani_1)) {
		pr_err("%s: Unable to get 8058_l0\n", __func__);
		return -ENODEV;
	}

	vreg_timpani_2 = regulator_get(NULL, "8058_s3");
	if (IS_ERR(vreg_timpani_2)) {
		pr_err("%s: Unable to get 8058_s3\n", __func__);
		regulator_put(vreg_timpani_1);
		return -ENODEV;
	}

	rc = regulator_set_voltage(vreg_timpani_1, 1200000, 1200000);
	if (rc) {
		pr_err("%s: unable to set L0 voltage to 1.2V\n", __func__);
		goto fail;
	}

	rc = regulator_set_voltage(vreg_timpani_2, 1800000, 1800000);
	if (rc) {
		pr_err("%s: unable to set S3 voltage to 1.8V\n", __func__);
		goto fail;
	}

	rc = regulator_enable(vreg_timpani_1);
	if (rc) {
		pr_err("%s: Enable regulator 8058_l0 failed\n", __func__);
		goto fail;
	}

	/* The settings for LDO0 should be set such that
	*  it doesn't require to reset the timpani. */
	rc = regulator_set_optimum_mode(vreg_timpani_1, 5000);
	if (rc < 0) {
		pr_err("Timpani regulator optimum mode setting failed\n");
		goto fail;
	}

	rc = regulator_enable(vreg_timpani_2);
	if (rc) {
		pr_err("%s: Enable regulator 8058_s3 failed\n", __func__);
		regulator_disable(vreg_timpani_1);
		goto fail;
	}

	rc = gpio_request(GPIO_CDC_RST_N, "CDC_RST_N");
	if (rc) {
		pr_err("%s: GPIO Request %d failed\n", __func__,
			GPIO_CDC_RST_N);
		regulator_disable(vreg_timpani_1);
		regulator_disable(vreg_timpani_2);
		goto fail;
	} else {
		gpio_direction_output(GPIO_CDC_RST_N, 1);
		usleep_range(1000, 1050);
		gpio_direction_output(GPIO_CDC_RST_N, 0);
		usleep_range(1000, 1050);
		gpio_direction_output(GPIO_CDC_RST_N, 1);
		gpio_free(GPIO_CDC_RST_N);
	}
	return rc;

fail:
	regulator_put(vreg_timpani_1);
	regulator_put(vreg_timpani_2);
	return rc;
}

static void msm_timpani_shutdown_power(void)
{
	int rc;

	rc = regulator_disable(vreg_timpani_1);
	if (rc)
		pr_err("%s: Disable regulator 8058_l0 failed\n", __func__);

	regulator_put(vreg_timpani_1);

	rc = regulator_disable(vreg_timpani_2);
	if (rc)
		pr_err("%s: Disable regulator 8058_s3 failed\n", __func__);

	regulator_put(vreg_timpani_2);
}

/* Power analog function of codec */
static struct regulator *vreg_timpani_cdc_apwr;
static int msm_timpani_codec_power(int vreg_on)
{
	int rc = 0;

	if (!vreg_timpani_cdc_apwr) {

		vreg_timpani_cdc_apwr = regulator_get(NULL, "8058_s4");

		if (IS_ERR(vreg_timpani_cdc_apwr)) {
			pr_err("%s: vreg_get failed (%ld)\n",
			__func__, PTR_ERR(vreg_timpani_cdc_apwr));
			rc = PTR_ERR(vreg_timpani_cdc_apwr);
			return rc;
		}
	}

	if (vreg_on) {

		rc = regulator_set_voltage(vreg_timpani_cdc_apwr,
				2200000, 2200000);
		if (rc) {
			pr_err("%s: unable to set 8058_s4 voltage to 2.2 V\n",
					__func__);
			goto vreg_fail;
		}

		rc = regulator_enable(vreg_timpani_cdc_apwr);
		if (rc) {
			pr_err("%s: vreg_enable failed %d\n", __func__, rc);
			goto vreg_fail;
		}
	} else {
		rc = regulator_disable(vreg_timpani_cdc_apwr);
		if (rc) {
			pr_err("%s: vreg_disable failed %d\n",
			__func__, rc);
			goto vreg_fail;
		}
	}

	return 0;

vreg_fail:
	regulator_put(vreg_timpani_cdc_apwr);
	vreg_timpani_cdc_apwr = NULL;
	return rc;
}

static struct marimba_codec_platform_data timpani_codec_pdata = {
	.marimba_codec_power =  msm_timpani_codec_power,
};

#define TIMPANI_SLAVE_ID_CDC_ADDR		0X77
#define TIMPANI_SLAVE_ID_QMEMBIST_ADDR		0X66

static struct marimba_platform_data timpani_pdata = {
	.slave_id[MARIMBA_SLAVE_ID_CDC] = TIMPANI_SLAVE_ID_CDC_ADDR,
	.slave_id[MARIMBA_SLAVE_ID_QMEMBIST] = TIMPANI_SLAVE_ID_QMEMBIST_ADDR,
	.marimba_setup = msm_timpani_setup_power,
	.marimba_shutdown = msm_timpani_shutdown_power,
	.codec = &timpani_codec_pdata,
	.tsadc_ssbi_adap = MARIMBA_SSBI_ADAP,
};

#define TIMPANI_I2C_SLAVE_ADDR	0xD

static struct i2c_board_info msm_i2c_gsbi7_timpani_info[] = {
	{
		I2C_BOARD_INFO("timpani", TIMPANI_I2C_SLAVE_ADDR),
		.platform_data = &timpani_pdata,
	},
};
#endif

#ifdef CONFIG_SND_SOC_WM8903
static struct wm8903_platform_data wm8903_pdata = {
	.gpio_cfg[2] = 0x3A8,
};

#define WM8903_I2C_SLAVE_ADDR 0x34
static struct i2c_board_info wm8903_codec_i2c_info[] = {
	{
		I2C_BOARD_INFO("wm8903", WM8903_I2C_SLAVE_ADDR >> 1),
		.platform_data = &wm8903_pdata,
	},
};
#endif

static struct wm8958_enh_eq_cfg enh_eq_beats_cfg = { "Beats",
											   		 { 	0x0020,//0x2200
  												 	   	0x0000,//0x2201
													   	0x0040,//0x2202
														0x0000,//0x2203
														0x0000,//0x2204
														0xCC75,//0x2205
														0x007F,//0x2206
														0x65A7,//0x2207
														0x0030,//0x2208
														0x0057,//0x2209
														0x0009,//0x220a
														0x5BB9,//0x220b
														0x0064,//0x220c
														0x90CD,//0x220d
														0x001E,//0x220e
														0x7BA4,//0x220f
														0x000D,//0x2210
														0x473B,//0x2211
														0x00FE,//0x2212
														0x4290,//0x2213
														0x0014,//0x2214
														0xCD7F,//0x2215
														0x0000,//0x2216
														0x0000,//0x2217
														0x00EB,//0x2218
														0x3281,//0x2219
														0x00AA,//0x221a
														0x9205,//0x221b
														0x00E9,//0x221c
														0x9AFE,//0x221d
														0x000C,//0x221e
														0x73D5,//0x221f
													},
												   };

#ifdef CONFIG_PMIC8901

#define PM8901_GPIO_INT           91
/*
 * Consumer specific regulator names:
 *			 regulator name		consumer dev_name
 */
static struct regulator_consumer_supply vreg_consumers_8901_USB_OTG[] = {
	REGULATOR_SUPPLY("8901_usb_otg",	NULL),
};
static struct regulator_consumer_supply vreg_consumers_8901_HDMI_MVS[] = {
	REGULATOR_SUPPLY("8901_hdmi_mvs",	NULL),
};


#define PM8901_VREG_INIT(_id, _min_uV, _max_uV, _modes, _ops, _apply_uV, \
			 _always_on) \
	{ \
		.init_data = { \
			.constraints = { \
				.valid_modes_mask = _modes, \
				.valid_ops_mask = _ops, \
				.min_uV = _min_uV, \
				.max_uV = _max_uV, \
				.input_uV = _min_uV, \
				.apply_uV = _apply_uV, \
				.always_on = _always_on, \
			}, \
			.consumer_supplies = vreg_consumers_8901_##_id, \
			.num_consumer_supplies = \
				ARRAY_SIZE(vreg_consumers_8901_##_id), \
		}, \
		.id = PM8901_VREG_ID_##_id, \
	}

#define PM8901_VREG_INIT_VS(_id) \
	PM8901_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL, \
			REGULATOR_CHANGE_STATUS, 0, 0)

struct pm8901_vreg_pdata pm8901_regulator_pdata[] = {
	PM8901_VREG_INIT_VS(USB_OTG),
	PM8901_VREG_INIT_VS(HDMI_MVS),
};

int pm8901_regulator_pdata_len __devinitdata =
	ARRAY_SIZE(pm8901_regulator_pdata);

#endif /* CONFIG_PMIC8901 */

#if defined(CONFIG_MARIMBA_CORE) && (defined(CONFIG_GPIO_SX150X) \
	|| defined(CONFIG_GPIO_SX150X_MODULE))

struct bahama_config_register{
	u8 reg;
	u8 value;
	u8 mask;
};

enum version{
	VER_1_0,
	VER_2_0,
	VER_UNSUPPORTED = 0xFF
};
#endif 

static struct isl29023_platform_data isl29023_pdata = {
    .rext = 10,
    .polled = 1,
    .poll_interval = 500,
};

static struct lsm303dlh_acc_platform_data lsm303dlh_acc_pdata = {
	.poll_interval = 100,
	.min_interval = LSM303DLH_ACC_MIN_POLL_PERIOD_MS,
	.g_range = LSM303DLH_ACC_G_2G,
#ifdef CONFIG_MACH_TENDERLOIN
    .axis_map_x = 1,
    .axis_map_y = 0,
    .axis_map_z = 2,
    .negate_x = 1,
    .negate_y = 0,
    .negate_z = 0,
#else
    .axis_map_x = 0,
    .axis_map_y = 1,
    .axis_map_z = 2,
    .negate_x = 0,
    .negate_y = 0,
    .negate_z = 0,
#endif
	.gpio_int1 = LSM303DLH_ACC_DEFAULT_INT1_GPIO,
	.gpio_int2 = LSM303DLH_ACC_DEFAULT_INT2_GPIO,
};

static struct lsm303dlh_mag_platform_data lsm303dlh_mag_pdata = {
	.poll_interval = 100,
	.min_interval = LSM303DLH_MAG_MIN_POLL_PERIOD_MS,
	.h_range = LSM303DLH_MAG_H_8_1G,
#ifdef CONFIG_MACH_TENDERLOIN
    .axis_map_x = 1,
    .axis_map_y = 0,
    .axis_map_z = 2,
    .negate_x = 1,
    .negate_y = 0,
    .negate_z = 0,
#else
    .axis_map_x = 0,
    .axis_map_y = 1,
    .axis_map_z = 2,
    .negate_x = 0,
    .negate_y = 0,
    .negate_z = 0,
#endif
};

static struct mpu3050_platform_data mpu3050_data = {
	.int_config = 0x10,
	.orientation = {   1,  0,  0,
			   0,  1,  0,
			   0,  0,  1 },
	.accel = {
		.get_slave_descr = get_accel_slave_descr,
		.adapt_num   = 0,
		.bus         = EXT_SLAVE_BUS_SECONDARY,
		.address     = 0x18,
		.orientation = {   1,  0,  0,
				   0,  1,  0,
				   0,  0,  1 },
	},
	.compass = {
		.get_slave_descr = get_compass_slave_descr,
		.adapt_num   = 0,
		.bus         = EXT_SLAVE_BUS_PRIMARY,
		.address     = 0x1E,
		.orientation = {  1,  0,  0,
				  0,  1,  0,
				  0,  0,  1 },
	},
};

static struct i2c_board_info lsm303dlh_acc_i2c_board_info[] = {
    {
        I2C_BOARD_INFO ( "lsm303dlh_acc_sysfs", 0x18),
        .platform_data = &lsm303dlh_acc_pdata,
    },
};

static struct i2c_board_info lsm303dlh_mag_i2c_board_info[] = {
    {
        I2C_BOARD_INFO ( "lsm303dlh_mag_sysfs", 0x1e),
        .platform_data = &lsm303dlh_mag_pdata,
    },
};

static struct i2c_board_info mpu3050_i2c_board_info[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0x68),
		.irq = MSM_GPIO_TO_INT(TENDERLOIN_GYRO_INT),
		.platform_data = &mpu3050_data,
	},
};

static struct i2c_board_info isl29023_i2c_board_info[] = {
    {
        I2C_BOARD_INFO ( "isl29023", 0x44),
        .irq = MSM_GPIO_TO_INT(TENDERLOIN_LS_INT),
        .platform_data = &isl29023_pdata,
    },
};

// MPU3050
static uint32_t tenderloin_mpu3050_cfgs[] = {
	GPIO_CFG(MPU3050_GPIO_IRQ,   0, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(MPU3050_GPIO_FSYNC, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA)
};

static void __init tenderloin_init_mpu3050(void)
{
	unsigned n;
	for (n = 0; n < ARRAY_SIZE(tenderloin_mpu3050_cfgs); ++n)
		gpio_tlmm_config(tenderloin_mpu3050_cfgs[n], 0);

	if (gpio_request(MPU3050_GPIO_FSYNC, "MPU3050_FSYNC")) {
		pr_err("%s: MPU3050_GPIO_FSYNC request failed\n", __func__);
		return;
	}
	gpio_direction_output(MPU3050_GPIO_FSYNC, 0);
}

#ifdef CONFIG_MFD_WM8994

#define WM8958_I2C_SLAVE_ADDR 0x1a

static struct regulator *vreg_wm8958;

static int wm8994_ldo_power(int enable)
{
	gpio_tlmm_config(GPIO_CFG(66, 0, GPIO_CFG_OUTPUT,GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(108, 0, GPIO_CFG_OUTPUT,GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	if (enable)
	{
		pr_err("%s: Power up the WM8994 LDOs\n", __func__);
		gpio_set_value_cansleep(66,1);
		gpio_set_value_cansleep(108,1);
	}else{
		pr_err("%s: Power down the WM8994 LDOs\n", __func__);
		gpio_set_value_cansleep(66,0);
		gpio_set_value_cansleep(108,0);
	}

	return 0;

}

static struct regulator *vreg_wm8958;
static unsigned int msm_wm8958_setup_power(void)
{
		static struct regulator *tp_5v0 = NULL;
		int rc=0;

		pr_err("%s: codec power setup\n", __func__);

		if (!tp_5v0) {
			tp_5v0 = regulator_get(NULL, "vdd50_boost");
			if (IS_ERR(tp_5v0)) {
				pr_err("failed to get regulator 'vdd50_boost' with %ld\n",
					PTR_ERR(tp_5v0));
				tp_5v0 = NULL;
			}
		}

		if (tp_5v0) {
			rc = regulator_enable(tp_5v0);
			if (rc < 0) {
				pr_err("failed to enable regulator 'vdd50_boost' with %d\n", rc);
			}
		}

		vreg_wm8958 = regulator_get(NULL, "8058_s3");
		if (IS_ERR(vreg_wm8958)) {
				pr_err("%s: Unable to get 8058_s3\n", __func__);
				return -ENODEV;
		}

		if(regulator_set_voltage(vreg_wm8958, 1800000, 1800000))
		{
				pr_err("%s: Unable to set regulator voltage:"
								" votg_8058_s3\n", __func__);
		}
		rc = regulator_enable(vreg_wm8958);

		if (rc) {
				pr_err("%s: Enable regulator 8058_s3 failed\n", __func__);

		}
		wm8994_ldo_power(1);
		wm8994_reg_status = 1;
		mdelay(30);
		return rc;
}

static void msm_wm8958_shutdown_power(void)
{
	 if (wm8994_reg_status){
		static struct regulator *tp_5v0 = NULL;
		int rc;

		pr_err("%s: codec power shutdown\n", __func__);

		if (!tp_5v0) {
			tp_5v0 = regulator_get(NULL, "vdd50_boost");
			if (IS_ERR(tp_5v0)) {
				pr_err("failed to get regulator 'vdd50_boost' with %ld\n",
					PTR_ERR(tp_5v0));
				tp_5v0 = NULL;
			}
		}

		if (tp_5v0) {
			rc = regulator_disable(tp_5v0);
			if (rc < 0) {
				pr_err("failed to disable regulator 'vdd50_boost' with %d\n", rc);
			}
		}

		wm8994_ldo_power(0);
		rc = regulator_disable(vreg_wm8958);
		if (rc)
				pr_err("%s: Disable regulator 8058_s3 failed\n", __func__);

		regulator_put(vreg_wm8958);
		wm8994_reg_status = 0;
	 }else{
		pr_err("%s: Already disabled -- wm8994_reg_status = %d\n", __func__, wm8994_reg_status);
		return;
	 }
}

static struct wm8994_pdata wm8958_pdata = {
	.gpio_defaults = { 	0x0001, //GPIO1 AUDIO_AMP_EN write 0x41 to enable AMP
						0x8001, //GPIO2 MCLK2 Pull Control
						0x8001, //GPIO3 BCLK2 Pull Control
						0x8001, //GPIO4 DACLRCLK2 Pull Control
						0x8001, //GPIO5 DACDAT2 Pull Control
						0x2005, //GPIO6 HEAD_MIC_DET to GPIO57 8660
						0xa101, //GPIO7 N/A on wm8958, only wm8994
						0xa101, //GPIO8 0xa101 not use
						0xa101, //GPIO9 0xa101 not use
						0xa101, //GPIO10 0xa101 not use
						0xa101, //GPIO11 0xa101 not use
	},

	/* Put the line outputs into differential mode so that the driver
	 * knows it can power the chip down to cold without pop/click issues.
	 * Line outputs are not actually connected on the board.
	 */
	.num_enh_eq_cfgs = 1,
	.enh_eq_cfgs = &enh_eq_beats_cfg,
	.lineout1_diff = 1,
	.lineout2_diff = 1,
	.wm8994_setup = msm_wm8958_setup_power,
	.wm8994_shutdown = msm_wm8958_shutdown_power,
	.micdet_irq = TENDERLOIN_AUD_HEAD_MIC_DET_IRQ_GPIO,
	.micbias = { 0x0029, 0x002d, },
};

static struct i2c_board_info audio_boardinfo[] __initdata = {
    {
        I2C_BOARD_INFO("wm8958", WM8958_I2C_SLAVE_ADDR),
        .platform_data = &wm8958_pdata,
    },
};
#endif /* CONFIG_MFD_WM8994 */
#ifdef CONFIG_LEDS_LM8502
static struct lm8502_memory_config lm8502_memcfg = {
	.eng1_startpage = 0,
	.eng1_endpage = 0,
	.eng2_startpage = 1,
	.eng2_endpage = 1,
};

static struct led_cfg lm8502_ledlist_0[] = {
	[0] = { // LED1
		.type = LED_WHITE,
		.current_addr = 0x26,
		.control_addr = 0x06,
	},
};

static struct led_cfg lm8502_ledlist_1[] = {
	[0] = { // LED2
		.type = LED_WHITE,
		.current_addr = 0x27,
		.control_addr = 0x07,
	},
};

static struct lm8502_led_config lm8502_leds[] = {
	[0] = {
		//led class dev
		.cdev.name  = "core_navi_left",
		.cdev.max_brightness = 100, // 100 percent brightness
		.cdev.flags = LED_CORE_SUSPENDRESUME,
		//led list
		.led_list = lm8502_ledlist_0,
		.nleds = ARRAY_SIZE(lm8502_ledlist_0),
		//hw group
		.hw_group = LED_HW_GRP_NONE,
		//defualt value
		.default_max_current = 0x02, // Current full-scale setting, max 3mA
		.default_state = LED_OFF,
		.default_brightness = 100, // 100 percent brightness
	},
	[1] = {
		//led class dev
		.cdev.name  = "core_navi_right",
		.cdev.max_brightness = 100, // 100 percent brightness
		.cdev.flags = LED_CORE_SUSPENDRESUME,
		//led list
		.led_list = lm8502_ledlist_1,
		.nleds = ARRAY_SIZE(lm8502_ledlist_1),
		//hw group
		.hw_group = LED_HW_GRP_NONE,
		//defualt value
		.default_max_current = 0x02, // Current full-scale setting, max 12.5mA
		.default_state = LED_OFF,
		.default_brightness = 100, // 100 percent brightness
	},
};

static struct lm8502_platform_data lm8502_platform_data = {
	.enable_gpio = LM8502_LIGHTING_EN_GPIO,
	.interrupt_gpio = LM8502_LIGHTING_INT_IRQ_GPIO,
	 //vib
	.vib_default_duty_cycle = 100, //percent
	.vib_default_direction = 0,
	.vib_invert_direction = 1,
	//flash or torch
	.flash_default_duration = 3,    // 3*32ms
	.flash_default_current = 500,   // 500mA
	.torch_default_current = 100,   // 100mA
	//leds
	.leds = lm8502_leds,
	.nleds = ARRAY_SIZE(lm8502_leds),
	//leds firmware memory configuration
	.memcfg = &lm8502_memcfg,
	//power save mode
	.power_mode = MISC_POWER_SAVE_ON,
	//others
	.dev_name = "lm8502",
};

static struct i2c_board_info lm8502_board_info[] = {
	{
		I2C_BOARD_INFO(LM8502_I2C_DEVICE, LM8502_I2C_ADDR),
		.platform_data = &lm8502_platform_data,
		.irq = MSM_GPIO_TO_INT(LM8502_LIGHTING_INT_IRQ_GPIO),
	}
};
#endif

#ifdef CONFIG_I2C
#define I2C_SURF 1
#define I2C_FFA  (1 << 1)
#define I2C_RUMI (1 << 2)
#define I2C_SIM  (1 << 3)
#define I2C_FLUID (1 << 4)
#define I2C_DRAGON (1 << 5)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};

static struct i2c_registry msm8x60_i2c_devices[] __initdata = {
#ifdef CONFIG_INPUT_LSM303DLH
    {
        I2C_SURF | I2C_FFA,
        MSM_GSBI3_QUP_I2C_BUS_ID,
        lsm303dlh_acc_i2c_board_info,
        ARRAY_SIZE(lsm303dlh_acc_i2c_board_info),
    },
    {
        I2C_SURF | I2C_FFA,
        MSM_GSBI3_QUP_I2C_BUS_ID,
        lsm303dlh_mag_i2c_board_info,
        ARRAY_SIZE(lsm303dlh_mag_i2c_board_info),
    },
#endif
    {
        I2C_SURF | I2C_FFA,
        MSM_GSBI3_QUP_I2C_BUS_ID,
        mpu3050_i2c_board_info,
        ARRAY_SIZE(mpu3050_i2c_board_info),
    },
    {
        I2C_SURF | I2C_FFA,
        MSM_GSBI3_QUP_I2C_BUS_ID,
        isl29023_i2c_board_info,
        ARRAY_SIZE(isl29023_i2c_board_info),
    },
#ifdef CONFIG_LEDS_LM8502
    {
	I2C_SURF,
	MSM_GSBI8_QUP_I2C_BUS_ID,   // use GSBI8 as LM8502 i2c
	lm8502_board_info,
	ARRAY_SIZE(lm8502_board_info),
    },
#endif
#ifndef CONFIG_MSM_SSBI
#ifdef CONFIG_PMIC8901
	{
		I2C_SURF | I2C_FFA,
		MSM_SSBI2_I2C_BUS_ID,
		pm8901_boardinfo,
		ARRAY_SIZE(pm8901_boardinfo),
	},
#endif
#endif /*CONFIG_MSM_SSBI */
#ifdef CONFIG_MFD_WM8994
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		audio_boardinfo,
		ARRAY_SIZE(audio_boardinfo),
	},
#endif
#ifdef CONFIG_MSM_CAMERA
    {
		I2C_SURF | I2C_FFA,
		MSM_GSBI4_QUP_I2C_BUS_ID,
		msm_camera_boardinfo,
		ARRAY_SIZE(msm_camera_boardinfo),
	},
#endif
#if HASTIMPANI
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_timpani_info,
		ARRAY_SIZE(msm_i2c_gsbi7_timpani_info),
	},
#endif
};
#endif 

/*
 *   S3A_1V8
 */
static struct regulator *board_S3A_1V8 = NULL;
static void __init board_setup_S3A_1V8 (void)
{
	int rc = 0;

#if 1
	/*
	 * Enable S3A_1V8 regulator and force it to HPM mode.
	 *
	 * Ideally, this regulator should be in HPM mode when WiFi is ON
	 * (even in suspend) or system is not in suspend.
	 *
	 * The code below is not optimal. It forces S3A_1V8 regulator to HPM
	 * mode all the time, even in suspend, which add extra 300uA to
	 * suspend power draw.
	 *
	 */
	board_S3A_1V8 = regulator_get(NULL, "8058_s3");
	if (IS_ERR(board_S3A_1V8)) {
		pr_err("%s: unable to get %s\n",
			__func__, "8058_s3");
	}
	rc = regulator_set_voltage(board_S3A_1V8, 1800000, 1800000);
	if (rc) {
		pr_err("%s: unable (%d) to set voltage for %s\n",
			__func__, rc, "8058_s3");
	}
	if (regulator_enable(board_S3A_1V8)) {
		pr_err("%s: Unable to enable %s\n",
			 __func__, "8058_s3");
	}
	rc = regulator_set_optimum_mode(board_S3A_1V8, 100000);
	if (rc < 0) {
		pr_err("%s: unable (%d) to set opt mode for %s\n",
			__func__, rc, "8058_s3");

	}
	pr_info("%s: %s: forcing HPM mode (%d)\n",
			__func__, "8058_s3", rc );
#else
	/*
	 *   The Alternative approach would be to call rpm_vreg_set_voltage
	 *   API with sleep parameter set to zero. Regulator must be configured
	 *   as sleep_selectable.
	 */
	rc = rpm_vreg_set_voltage(RPM_VREG_ID_PM8058_S3,
				  enum rpm_vreg_voter voter, ??
				  0, ??
				  0);

#endif

	return;
}

static void __init msm8x60_cfg_smsc911x(void)
{
	smsc911x_resources[1].start =
		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 6);
	smsc911x_resources[1].end =
		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 6);
}

static void fixup_i2c_configs(void)
{
#ifdef CONFIG_I2C
	/*
	 * Set PMIC 8901 MPP0 active_high to 0 for surf and charm_surf. This
	 * implies that the regulator connected to MPP0 is enabled when
	 * MPP0 is low.
	 */
  //	if (machine_is_tenderloin())
  //		pm8901_vreg_init_pdata[PM8901_VREG_ID_MPP0].active_high = 0;
  //	else
  //		pm8901_vreg_init_pdata[PM8901_VREG_ID_MPP0].active_high = 1;

	if (machine_is_tenderloin() && boardtype_is_3g()) {
#ifdef CONFIG_INPUT_LSM303DLH
		lsm303dlh_acc_pdata.negate_y = 1;
		lsm303dlh_acc_pdata.negate_z = 1;
		lsm303dlh_mag_pdata.negate_y = 1;
		lsm303dlh_mag_pdata.negate_z = 1;
#endif
		mpu3050_data.orientation[0] = -mpu3050_data.orientation[0];
		mpu3050_data.orientation[8] = -mpu3050_data.orientation[8];
		mpu3050_data.accel.orientation[0] = -mpu3050_data.accel.orientation[0];
		mpu3050_data.accel.orientation[8] = -mpu3050_data.accel.orientation[8];
		mpu3050_data.compass.orientation[0] = -mpu3050_data.compass.orientation[0];
		mpu3050_data.compass.orientation[8] = -mpu3050_data.compass.orientation[8];
	}
#endif
}

/*
 * Most segments of the EBI2 bus are disabled by default.
 */
static void __init msm8x60_init_ebi2(void)
{
	uint32_t ebi2_cfg;
	void *ebi2_cfg_ptr;
	struct clk *mem_clk = clk_get_sys("msm_ebi2", "mem_clk");

	if (IS_ERR(mem_clk)) {
		pr_err("%s: clk_get_sys(%s,%s), failed", __func__,
					"msm_ebi2", "mem_clk");
		return;
	}
	clk_prepare_enable(mem_clk);
	clk_put(mem_clk);

	ebi2_cfg_ptr = ioremap_nocache(0x1a100000, sizeof(uint32_t));
	if (ebi2_cfg_ptr != 0) {
		ebi2_cfg = readl_relaxed(ebi2_cfg_ptr);

		if (machine_is_msm8x60_surf() || machine_is_msm8x60_ffa() || machine_is_tenderloin())
			ebi2_cfg |= (1 << 4) | (1 << 5); /* CS2, CS3 */

		writel_relaxed(ebi2_cfg, ebi2_cfg_ptr);
		iounmap(ebi2_cfg_ptr);
	}

	if (machine_is_tenderloin()) {
		ebi2_cfg_ptr = ioremap_nocache(0x1a110000, SZ_4K);
		if (ebi2_cfg_ptr != 0) {
			/* EBI2_XMEM_CFG:PWRSAVE_MODE off */
			writel(0UL, ebi2_cfg_ptr);

			/* CS2: Delay 9 cycles (140ns@64MHz) between SMSC
			 * LAN9221 Ethernet controller reads and writes.
			 * The lowest 4 bits are the read delay, the next
			 * 4 are the write delay. */
			writel(0x031F1C99, ebi2_cfg_ptr + 0x10);
#ifdef CONFIG_USB_PEHCI_HCD
			if(boardtype_is_3g()) {
				/*
				* RECOVERY=5, HOLD_WR=1
				* INIT_LATENCY_WR=1, INIT_LATENCY_RD=1
				* WAIT_WR=1, WAIT_RD=2
				*/
				writel(0x51010112, ebi2_cfg_ptr + 0x14);
				/*
				* HOLD_RD=1
				* ADV_OE_RECOVERY=0, ADDR_HOLD_ENA=1
				*/
				writel(0x01000020, ebi2_cfg_ptr + 0x34);
			}
			else {
				/* EBI2 CS3 muxed address/data,
				* two cyc addr enable */
				writel(0xA3030020, ebi2_cfg_ptr + 0x34);
			}
#else
			/* EBI2 CS3 muxed address/data,
			* two cyc addr enable */
			writel(0xA3030020, ebi2_cfg_ptr + 0x34);

#endif
			iounmap(ebi2_cfg_ptr);
		}
	}
}

static void __init register_i2c_devices(void)
{
#ifdef CONFIG_I2C
	u8 mach_mask = 0;
	int i;

	
	mach_mask = I2C_SURF;

	
	for (i = 0; i < ARRAY_SIZE(msm8x60_i2c_devices); ++i) {
		if (msm8x60_i2c_devices[i].machs & mach_mask)
			i2c_register_board_info(msm8x60_i2c_devices[i].bus,
						msm8x60_i2c_devices[i].info,
						msm8x60_i2c_devices[i].len);
	}

#endif
}

#ifdef CONFIG_SERIAL_MSM_HS

static int configure_uart_gpios(int on)
{
	int ret = 0;
	int uart_gpios[] = {53, 54, 55, 56};

	ret = configure_gpiomux_gpios(on, uart_gpios, ARRAY_SIZE(uart_gpios));

	return ret;
}

static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.inject_rx_on_wakeup = 1,
	.rx_to_inject = 0xFD,
    .gpio_config = configure_uart_gpios,
};
#endif

static void __init msm8x60_init_uart12dm(void)
{
#if !defined(CONFIG_USB_PEHCI_HCD) && !defined(CONFIG_USB_PEHCI_HCD_MODULE)
	
	void *fpga_mem = ioremap_nocache(0x1D000000, SZ_4K);

	if (!fpga_mem)
		pr_err("%s(): Error getting memory\n", __func__);

	
	writew(0xFFFF, fpga_mem + 0x15C);
	
	writew(0, fpga_mem + 0x172);
	
	writew(1, fpga_mem + 0xEA);
	
	writew(1, fpga_mem + 0xEC);
	mb();
	iounmap(fpga_mem);
#endif
}

#define MSM_GSBI9_PHYS		0x19900000
#define GSBI_DUAL_MODE_CODE	0x60

static void __init msm8x60_i2c_init(void)
{
#ifdef CONFIG_I2C_QUP
	void *gsbi_mem = ioremap_nocache(0x19C00000, 4);
	
	writel_relaxed(0x6 << 4, gsbi_mem);
	/* Ensure protocol code is written before proceeding further */
	mb();
	iounmap(gsbi_mem);

	msm_gsbi3_qup_i2c_device.dev.platform_data = &msm_gsbi3_qup_i2c_pdata;
	msm_gsbi4_qup_i2c_device.dev.platform_data = &msm_gsbi4_qup_i2c_pdata;
	msm_gsbi7_qup_i2c_device.dev.platform_data = &msm_gsbi7_qup_i2c_pdata;
	msm_gsbi8_qup_i2c_device.dev.platform_data = &msm_gsbi8_qup_i2c_pdata;
	msm_gsbi9_qup_i2c_device.dev.platform_data = &msm_gsbi9_qup_i2c_pdata;
	msm_gsbi10_qup_i2c_device.dev.platform_data = &msm_gsbi10_qup_i2c_pdata;
#endif
#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
	msm_gsbi1_qup_spi_device.dev.platform_data = &msm_gsbi1_qup_spi_pdata;
#endif
#ifdef CONFIG_I2C_SSBI
	msm_device_ssbi3.dev.platform_data = &msm_ssbi3_pdata;
#endif

#ifdef CONFIG_SERIAL_MSM_HS 
	msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(UART1DM_RX_GPIO);
	msm_device_uart_dm1.name = "msm_uartdm";
	msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif
}

static void __init msm8x60_init_buses(void)
{
#ifdef CONFIG_MSM_BUS_SCALING

	
	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 2) {
		msm_bus_apps_fabric_pdata.rpm_enabled = 1;
		msm_bus_sys_fabric_pdata.rpm_enabled = 1;
		msm_bus_mm_fabric_pdata.rpm_enabled = 1;
		msm_bus_sys_fpb_pdata.rpm_enabled = 1;
		msm_bus_cpss_fpb_pdata.rpm_enabled = 1;
	}

	msm_bus_apps_fabric.dev.platform_data = &msm_bus_apps_fabric_pdata;
	msm_bus_sys_fabric.dev.platform_data = &msm_bus_sys_fabric_pdata;
	msm_bus_mm_fabric.dev.platform_data = &msm_bus_mm_fabric_pdata;
	msm_bus_sys_fpb.dev.platform_data = &msm_bus_sys_fpb_pdata;
	msm_bus_cpss_fpb.dev.platform_data = &msm_bus_cpss_fpb_pdata;
#endif
}

static void __init tenderloin_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_msm8x60_io();

	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");
}

void msm_fusion_setup_pinctrl(void)
{
	struct msm_xo_voter *a1;

	if (socinfo_get_platform_subtype() == 0x3) {
		a1 = msm_xo_get(MSM_XO_TCXO_A1, "mdm");
		BUG_ON(!a1);
		msm_xo_mode_vote(a1, MSM_XO_MODE_PIN_CTRL);
	}
}

void tenderloin_add_usb_devices(void)
{
	printk(KERN_INFO "%s\n", __func__);
}

static void __init msm8x60_gfx_init(void)
{
	msm8x60_check_2d_hardware();
	platform_device_register(&msm_kgsl_3d0);
	platform_device_register(&msm_kgsl_2d0);
	platform_device_register(&msm_kgsl_2d1);
}

static int board_reboot_call(struct notifier_block *this, unsigned long code, void *_cmd)
{
	if(code == SYS_RESTART) {
		//pull down the boot config2 before reset the device
		gpio_set_value(82, 0);
		//wait enough time to discharge the gpio82
		mdelay(50);
	}
	return NOTIFY_DONE;
}

static struct notifier_block tenderloin_reboot_notifier = {
	.notifier_call = board_reboot_call,
};

void board_register_reboot_notifier(void)
{
	if(boardtype_is_3g() &&(board_type == TOPAZ_3G_EVT4))
	{
		register_reboot_notifier(&tenderloin_reboot_notifier);
	}
}

void __init tenderloin_fixup_pins(void)
{
	// touchpanel
	ctp_pins[0].gpio = pin_table[GPIO_CTP_WAKE_PIN];

	// BT
	bt_pins[0].gpio = pin_table[BT_RST_N_PIN];
	bt_pins[1].gpio = pin_table[BT_HOST_WAKE_PIN];

	// Lighting
	lm8502_platform_data.interrupt_gpio = pin_table[LM8502_LIGHTING_INT_IRQ_PIN];
	lm8502_board_info[0].irq = MSM_GPIO_TO_INT(pin_table[LM8502_LIGHTING_INT_IRQ_PIN]);

	// Charging
	max8903b_charger_pdata.IUSB_in = pin_table[MAX8903B_GPIO_USB_CHG_MODE_PIN];
	max8903b_charger_pdata.DOK_N_out = pin_table[MAX8903B_GPIO_DC_OK_PIN];

        tenderloin_a6_fixup_pins();
}

static void __init tenderloin_init(void)
{
	uint32_t soc_platform_version;

        if (meminfo_init(SYS_MEMORY, SZ_256M) < 0)
          pr_err("meminfo_init() failed!\n");

	platform_device_register(&msm_gpio_device);
	msm_tsens_early_init(&tenderloin_tsens_pdata);
	BUG_ON(msm_rpm_init(&msm8660_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(&msm_rpmrs_data));

	regulator_suppress_info_printing();

        tenderloin_init_pins();

	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");

	platform_device_register(&rpm_regulator_early_device);
	platform_device_register(&rpm_regulator_device);

	msm_clock_init(&msm8x60_clock_init_data);

        if (boardtype_is_3g())
		mpu3050_i2c_board_info[0].irq = MSM_GPIO_TO_INT(TENDERLOIN_GYRO_INT_3G);

        tenderloin_init_gpiomux();
        tenderloin_init_pmic();

        msm8x60_i2c_init();
        tenderloin_usb_i2c_init();
        msm8x60_gfx_init();

	soc_platform_version = socinfo_get_platform_version();
	if (SOCINFO_VERSION_MAJOR(soc_platform_version) == 1 &&
			SOCINFO_VERSION_MINOR(soc_platform_version) >= 2) {
		struct msm_spm_platform_data *spm_data;

		spm_data = &msm_spm_data_v1[1];
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] &= ~0x0F00UL;
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] |= 0x0100UL;

		spm_data = &msm_spm_data[1];
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] &= ~0x0F00UL;
		spm_data->reg_init_values[MSM_SPM_REG_SAW_CFG] |= 0x0100UL;
	}

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 1)
		msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	else
		msm_spm_init(msm_spm_data_v1, ARRAY_SIZE(msm_spm_data_v1));

        msm8x60_init_buses();

/*
 * Enable EBI2 only for boards which make use of it. Leave
 * it disabled for all others for additional power savings.
 */
    if (boardtype_is_3g())
       msm8x60_init_ebi2();

	platform_add_devices(early_devices, ARRAY_SIZE(early_devices));

	msm8x60_init_uart12dm();
	msm8x60_init_mmc();

	tenderloin_init_mpu3050();

        tenderloin_init_ts();

#ifdef CONFIG_BATTERY_MSM8X60
        platform_device_register(&msm_charger_device);
#endif

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 1)
		platform_add_devices(msm8660_footswitch,
				     msm8660_num_footswitch);

	platform_add_devices(tenderloin_devices,
			     ARRAY_SIZE(tenderloin_devices));

        tenderloin_init_fb();

	tenderloin_gpio_mpp_init();
        tenderloin_usb_init();
        platform_device_register(&tenderloin_8901_mpp_vreg);
#ifdef CONFIG_MSM_DSPS
	msm8x60_init_dsps();
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	//reverse the polarity of the max8903b USUS_pin on TopazWifi from DVT1 hwbuild
	if ((board_type > TOPAZ_EVT1) && (board_type != TOPAZ_3G_PROTO)) {
		max8903b_charger_pdata.USUS_in_polarity = 1;
	}
#endif
	msm8x60_cfg_smsc911x();
        platform_add_devices(asoc_devices,
			ARRAY_SIZE(asoc_devices));

	fixup_i2c_configs();

	register_i2c_devices();
	platform_device_register(&smsc911x_device);

	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));

#ifdef CONFIG_SENSORS_MSM_ADC

#endif
#ifdef CONFIG_A6
	tenderloin_init_a6();
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	msm_snddev_init();
#endif
#ifdef CONFIG_MACH_TENDERLOIN
    board_setup_S3A_1V8();
#endif
    tenderloin_init_keypad();
    printk(KERN_ERR "%s: --\n", __func__);
}

MACHINE_START(TENDERLOIN, "tenderloin")	
	.map_io = tenderloin_map_io,
	.reserve = tenderloin_reserve,
	.init_irq = msm8x60_init_irq,
	.handle_irq = gic_handle_irq,
	.init_machine = tenderloin_init,
	.timer = &msm_timer,
	.init_early = msm8x60_allocate_memory_regions,
	.init_very_early = tenderloin_early_memory,
	.restart = msm_restart,
MACHINE_END

