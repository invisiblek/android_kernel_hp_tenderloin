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
#include <linux/leds-pm8058.h>
#include <linux/cm3629.h>
#include <linux/pmic8058-othc.h>
#include <linux/mfd/pmic8901.h>
#include <linux/htc_flashlight.h>
#include <linux/regulator/msm-gpio-regulator.h>
#include <linux/regulator/pmic8901-regulator.h>
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
#include <linux/atmel_qt602240.h>
#include <linux/input/cy8c_cs.h>
#include <linux/cy8c_tma_ts.h>
#include <linux/isl29028.h>
#include <linux/isl29029.h>
#include <linux/mpu.h>

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
#include <mach/htc_battery_8x60.h>
#ifdef CONFIG_TPS65200
#include <linux/tps65200.h>
#endif
#include <mach/msm_battery.h>
#include <linux/usb/msm_hsusb.h>
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
#include <mach/tpa2051d3.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include <mach/sdio_al.h>
#include <mach/rpm.h>
#include <mach/rpm-regulator.h>
#include <mach/restart.h>
#include <mach/cable_detect.h>
#include <mach/board-msm8660.h>
#include <mach/iommu_domains.h>
#include <mach/htc_headset_mgr.h>
#include <mach/htc_headset_gpio.h>
#include <mach/htc_headset_pmic.h>
#include <mach/htc_headset_8x60.h>
#include <mach/htc_headset_one_wire.h>

#ifdef CONFIG_FB_MSM_HDMI_MHL
#include <mach/mhl.h>
#endif

#include "board-pyramid.h"
#include "devices.h"
#include "devices-msm8x60.h"
#include <mach/cpuidle.h>
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
#include "board-storage-common-a.h"
#include <mach/board_htc.h>

#include <linux/msm_ion.h>
#include <mach/ion.h>
#include <mach/msm_rtb.h>

struct pm8xxx_mpp_init_info {
	unsigned			mpp;
	struct pm8xxx_mpp_config_data	config;
};

extern int ps_type;

#define AUTORESTART 50000
#ifdef AUTORESTART
#include <linux/workqueue.h>
static void my_work_handler(struct work_struct *w);

static struct workqueue_struct *wq = 0;
static DECLARE_DELAYED_WORK(my_work, my_work_handler);
static unsigned long delay;

static void
my_work_handler(struct work_struct *w)
{
  //uint32_t restart_reason = 0x6f656d99;
  //msm_proc_comm(PCOM_RESET_CHIP_IMM, &restart_reason, 0);
  msm_restart(0, "recovery");
}
#endif

#define MSM_SHARED_RAM_PHYS 0x40000000
#define MDM2AP_SYNC 129

#define GPIO_ETHERNET_RESET_N_DRAGON	30
#define LCDC_SPI_GPIO_CLK				73
#define LCDC_SPI_GPIO_CS				72
#define LCDC_SPI_GPIO_MOSI				70

#define DSPS_PIL_GENERIC_NAME		"dsps"
#define DSPS_PIL_FLUID_NAME		"dsps_fluid"

#define A01_DEV 0x80

enum {
	GPIO_EXPANDER_IRQ_BASE  = PM8901_IRQ_BASE + NR_PMIC8901_IRQS,
	GPIO_EXPANDER_GPIO_BASE = PM8901_GPIO_BASE + PM8901_MPPS,
	
	GPIO_CORE_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE,
	GPIO_CLASS_D1_EN        = GPIO_CORE_EXPANDER_BASE,
	GPIO_WLAN_DEEP_SLEEP_N,
	GPIO_LVDS_SHUTDOWN_N,
	GPIO_DISP_RESX_N        = GPIO_LVDS_SHUTDOWN_N,
	GPIO_MS_SYS_RESET_N,
	GPIO_CAP_TS_RESOUT_N,
	GPIO_CAP_GAUGE_BI_TOUT,
	GPIO_ETHERNET_PME,
	GPIO_EXT_GPS_LNA_EN,
	GPIO_MSM_WAKES_BT,
	GPIO_ETHERNET_RESET_N,
	GPIO_HEADSET_DET_N,
	GPIO_USB_UICC_EN,
	GPIO_BACKLIGHT_EN,
	GPIO_EXT_CAMIF_PWR_EN,
	GPIO_BATT_GAUGE_INT_N,
	GPIO_BATT_GAUGE_EN,
	
	GPIO_DOCKING_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + 16,
	GPIO_MIPI_DSI_RST_N        = GPIO_DOCKING_EXPANDER_BASE,
	GPIO_AUX_JTAG_DET_N,
	GPIO_DONGLE_DET_N,
	GPIO_SVIDEO_LOAD_DET,
	GPIO_SVID_AMP_SHUTDOWN1_N,
	GPIO_SVID_AMP_SHUTDOWN0_N,
	GPIO_SDC_WP,
	GPIO_IRDA_PWDN,
	GPIO_IRDA_RESET_N,
	GPIO_DONGLE_GPIO0,
	GPIO_DONGLE_GPIO1,
	GPIO_DONGLE_GPIO2,
	GPIO_DONGLE_GPIO3,
	GPIO_DONGLE_PWR_EN,
	GPIO_EMMC_RESET_N,
	GPIO_TP_EXP2_IO15,
	
	GPIO_SURF_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + (16 * 2),
	GPIO_SD_CARD_DET_1      = GPIO_SURF_EXPANDER_BASE,
	GPIO_SD_CARD_DET_2,
	GPIO_SD_CARD_DET_4,
	GPIO_SD_CARD_DET_5,
	GPIO_UIM3_RST,
	GPIO_SURF_EXPANDER_IO5,
	GPIO_SURF_EXPANDER_IO6,
	GPIO_ADC_I2C_EN,
	GPIO_SURF_EXPANDER_IO8,
	GPIO_SURF_EXPANDER_IO9,
	GPIO_SURF_EXPANDER_IO10,
	GPIO_SURF_EXPANDER_IO11,
	GPIO_SURF_EXPANDER_IO12,
	GPIO_SURF_EXPANDER_IO13,
	GPIO_SURF_EXPANDER_IO14,
	GPIO_SURF_EXPANDER_IO15,
	
	GPIO_LEFT_KB_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + (16 * 3),
	GPIO_LEFT_LED_1            = GPIO_LEFT_KB_EXPANDER_BASE,
	GPIO_LEFT_LED_2,
	GPIO_LEFT_LED_3,
	GPIO_LEFT_LED_WLAN,
	GPIO_JOYSTICK_EN,
	GPIO_CAP_TS_SLEEP,
	GPIO_LEFT_KB_IO6,
	GPIO_LEFT_LED_5,
	
	GPIO_RIGHT_KB_EXPANDER_BASE = GPIO_EXPANDER_GPIO_BASE + (16 * 3) + 8,
	GPIO_RIGHT_LED_1            = GPIO_RIGHT_KB_EXPANDER_BASE,
	GPIO_RIGHT_LED_2,
	GPIO_RIGHT_LED_3,
	GPIO_RIGHT_LED_BT,
	GPIO_WEB_CAMIF_STANDBY,
	GPIO_COMPASS_RST_N,
	GPIO_WEB_CAMIF_RESET_N,
	GPIO_RIGHT_LED_5,
	GPIO_R_ALTIMETER_RESET_N,
	
	GPIO_SOUTH_EXPANDER_BASE,
	GPIO_MIC2_ANCR_SEL = GPIO_SOUTH_EXPANDER_BASE,
	GPIO_MIC1_ANCL_SEL,
	GPIO_HS_MIC4_SEL,
	GPIO_FML_MIC3_SEL,
	GPIO_FMR_MIC5_SEL,
	GPIO_TS_SLEEP,
	GPIO_HAP_SHIFT_LVL_OE,
	GPIO_HS_SW_DIR,
	
	GPIO_NORTH_EXPANDER_BASE,
	GPIO_EPM_3_3V_EN = GPIO_NORTH_EXPANDER_BASE,
	GPIO_EPM_5V_BOOST_EN,
	GPIO_AUX_CAM_2P7_EN,
	GPIO_LED_FLASH_EN,
	GPIO_LED1_GREEN_N,
	GPIO_LED2_RED_N,
	GPIO_FRONT_CAM_RESET_N,
	GPIO_EPM_LVLSFT_EN,
	GPIO_N_ALTIMETER_RESET_N,
	
	GPIO_EPM_EXPANDER_BASE,
	GPIO_PWR_MON_START = GPIO_EPM_EXPANDER_BASE,
	GPIO_PWR_MON_RESET_N,
	GPIO_ADC1_PWDN_N,
	GPIO_ADC2_PWDN_N,
	GPIO_EPM_EXPANDER_IO4,
	GPIO_ADC1_MUX_SPI_INT_N_3_3V,
	GPIO_ADC2_MUX_SPI_INT_N,
	GPIO_EPM_EXPANDER_IO7,
	GPIO_PWR_MON_ENABLE,
	GPIO_EPM_SPI_ADC1_CS_N,
	GPIO_EPM_SPI_ADC2_CS_N,
	GPIO_EPM_EXPANDER_IO11,
	GPIO_EPM_EXPANDER_IO12,
	GPIO_EPM_EXPANDER_IO13,
	GPIO_EPM_EXPANDER_IO14,
	GPIO_EPM_EXPANDER_IO15,
};


static unsigned int engineerid, mem_size_mb;



#define UI_INT1_N 25
#define UI_INT2_N 34
#define UI_INT3_N 14
#define FM_GPIO 17

/* Speed bin register. */
#define QFPROM_SPEED_BIN_ADDR		(MSM_QFPROM_BASE + 0x00C0)

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

#ifdef CONFIG_PERFLOCK
static unsigned pyramid_perf_acpu_table[] = {
	594000000, 
	810000000, 
	1026000000,
	1134000000,
	1674000000, 
};

static struct perflock_data pyramid_perflock_data = {
	.perf_acpu_table = pyramid_perf_acpu_table,
	.table_size = ARRAY_SIZE(pyramid_perf_acpu_table),
};

static struct perflock_data pyramid_cpufreq_ceiling_data = {
	.perf_acpu_table = pyramid_perf_acpu_table,
	.table_size = ARRAY_SIZE(pyramid_perf_acpu_table),
};

static struct perflock_pdata perflock_pdata = {
       .perf_floor = &pyramid_perflock_data,
       .perf_ceiling = &pyramid_cpufreq_ceiling_data,
};

struct platform_device msm8x60_device_perf_lock = {
       .name = "perf_lock",
       .id = -1,
       .dev = {
               .platform_data = &perflock_pdata,
       },
};
#endif

static struct regulator_consumer_supply vreg_consumers_8901_S0[] = {
	REGULATOR_SUPPLY("8901_s0",		NULL),
};
static struct regulator_consumer_supply vreg_consumers_8901_S1[] = {
	REGULATOR_SUPPLY("8901_s1",		NULL),
};

static struct regulator_init_data saw_s0_init_data = {
		.constraints = {
			.name = "8901_s0",
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
			.min_uV = 800000,
			.max_uV = 1325000,
		},
		.consumer_supplies = vreg_consumers_8901_S0,
		.num_consumer_supplies = ARRAY_SIZE(vreg_consumers_8901_S0),
};

static struct regulator_init_data saw_s1_init_data = {
		.constraints = {
			.name = "8901_s1",
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
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

#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)

#define ISP1763_INT_GPIO		117
#define ISP1763_RST_GPIO		152
static struct resource isp1763_resources[] = {
	[0] = {
		.flags	= IORESOURCE_MEM,
		.start	= 0x1D000000,
		.end	= 0x1D005FFF,		
	},
	[1] = {
		.flags	= IORESOURCE_IRQ,
	},
};
static void __init msm8x60_cfg_isp1763(void)
{
	isp1763_resources[1].start = gpio_to_irq(ISP1763_INT_GPIO);
	isp1763_resources[1].end = gpio_to_irq(ISP1763_INT_GPIO);
}

static int isp1763_setup_gpio(int enable)
{
	int status = 0;

	if (enable) {
		status = gpio_request(ISP1763_INT_GPIO, "isp1763_usb");
		if (status) {
			pr_err("%s:Failed to request GPIO %d\n",
						__func__, ISP1763_INT_GPIO);
			return status;
		}
		status = gpio_direction_input(ISP1763_INT_GPIO);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_INT_GPIO);
			goto gpio_free_int;
		}
		status = gpio_request(ISP1763_RST_GPIO, "isp1763_usb");
		if (status) {
			pr_err("%s:Failed to request GPIO %d\n",
						__func__, ISP1763_RST_GPIO);
			goto gpio_free_int;
		}
		status = gpio_direction_output(ISP1763_RST_GPIO, 1);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_RST_GPIO);
			goto gpio_free_rst;
		}
		pr_debug("\nISP GPIO configuration done\n");
		return status;
	}

gpio_free_rst:
	gpio_free(ISP1763_RST_GPIO);
gpio_free_int:
	gpio_free(ISP1763_INT_GPIO);

	return status;
}
static struct isp1763_platform_data isp1763_pdata = {
	.reset_gpio	= ISP1763_RST_GPIO,
	.setup_gpio	= isp1763_setup_gpio
};

static struct platform_device isp1763_device = {
	.name          = "isp1763_usb",
	.num_resources = ARRAY_SIZE(isp1763_resources),
	.resource      = isp1763_resources,
	.dev           = {
		.platform_data = &isp1763_pdata
	}
};
#endif



static int pyramid_phy_init_seq[] = { 0x06, 0x36, 0x0C, 0x31, 0x31, 0x32, 0x1, 0x0E, 0x1, 0x11, -1 };
static struct msm_otg_platform_data msm_otg_pdata = {
	.phy_init_seq	= pyramid_phy_init_seq,
        .mode		= USB_OTG,
	.otg_control	= OTG_PMIC_CONTROL,
	.phy_type	= CI_45NM_INTEGRATED_PHY,
	.power_budget	= 750,

        //        .pmic_id_irq		= PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 36),
        //        .pmic_id_irq = MSM_GPIO_TO_INT(PYRAMID_GPIO_USB_ID),
        //        .pmic_id_irq		= PM8058_USB_ID_IN_IRQ(PM8058_IRQ_BASE),
};

#ifdef CONFIG_USB_GADGET_MSM_72K
static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.is_phy_status_timer_on = 1,
};
#endif

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

#ifdef CONFIG_HTC_BATT_8x60
static struct htc_battery_platform_data htc_battery_pdev_data = {
	.guage_driver = GUAGE_NONE,
	.gpio_mbat_in = MSM_GPIO_TO_INT(PYRAMID_GPIO_MBAT_IN),
	.gpio_mbat_in_trigger_level = MBAT_IN_LOW_TRIGGER,
	.charger = SWITCH_CHARGER_TPS65200,
	.mpp_data = {
		{PM8058_MPP_PM_TO_SYS(XOADC_MPP_3), PM_MPP_AIN_AMUX_CH6},
		{PM8058_MPP_PM_TO_SYS(XOADC_MPP_5), PM_MPP_AIN_AMUX_CH6},
		{PM8058_MPP_PM_TO_SYS(XOADC_MPP_7), PM_MPP_AIN_AMUX_CH6},
		{PM8058_MPP_PM_TO_SYS(XOADC_MPP_8), PM_MPP_AIN_AMUX_CH6},
       },
};

static struct platform_device htc_battery_pdev = {
	.name = "htc_battery",
	.id = -1,
	.dev    = {
	.platform_data = &htc_battery_pdev_data,
	},
};
#endif


#ifdef CONFIG_FLASHLIGHT_AAT1271
static void config_flashlight_gpios(void)
{
	static uint32_t flashlight_gpio_table[] = {
		GPIO_CFG(PYRAMID_TORCH_EN, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(PYRAMID_FLASH_EN, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	};

	gpio_tlmm_config(flashlight_gpio_table[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(flashlight_gpio_table[1], GPIO_CFG_ENABLE);
}

static struct flashlight_platform_data flashlight_data = {
	.gpio_init 		= config_flashlight_gpios,
	.torch 			= PYRAMID_TORCH_EN,
	.flash 			= PYRAMID_FLASH_EN,
	.flash_duration_ms 	= 600,
	.led_count 		= 2,
};

static struct platform_device flashlight_device = {
	.name = "FLASHLIGHT_AAT1271",
	.dev = {
		.platform_data	= &flashlight_data,
	},
};
#endif

static void config_gpio_table_c2(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("[GPIO] %s: gpio_tlmm_config(%#x)=%d\n",
					__func__, table[n], rc);
			break;
		}
	}
}
#ifdef CONFIG_FB_MSM_HDMI_MHL
static void mhl_sii9234_1v2_power(bool enable);
#endif

static uint32_t usb_ID_PIN_input_table[] = {
	GPIO_CFG(PYRAMID_GPIO_USB_ID, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t usb_ID_PIN_ouput_table[] = {
	GPIO_CFG(PYRAMID_GPIO_USB_ID, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};


static uint32_t mhl_reset_pin_ouput_table[] = {
	GPIO_CFG(PYRAMID_GPIO_MHL_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
};

static uint32_t mhl_usb_switch_ouput_table[] = {
	GPIO_CFG(PYRAMID_GPIO_MHL_USB_SWITCH, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
};

void config_pyramid_mhl_gpios(void)
{
	config_gpio_table_c2(mhl_usb_switch_ouput_table,
			ARRAY_SIZE(mhl_usb_switch_ouput_table));
	config_gpio_table_c2(mhl_reset_pin_ouput_table,
			ARRAY_SIZE(mhl_reset_pin_ouput_table));
}

static void pm8058_usb_config(void)
{

}

void config_pyramid_usb_id_gpios(bool output)
{
	if (output) {
		gpio_tlmm_config(usb_ID_PIN_ouput_table[0], 0);
		gpio_set_value(PYRAMID_GPIO_USB_ID, 1);
		printk(KERN_INFO "%s %d output high\n",  __func__, PYRAMID_GPIO_USB_ID);
	} else {
		gpio_tlmm_config(usb_ID_PIN_input_table[0], 0);
		printk(KERN_INFO "%s %d input none pull\n",  __func__, PYRAMID_GPIO_USB_ID);
	}
}

static void pyramid_usb_dpdn_switch(int path)
{
	switch (path) {
	case PATH_USB:
	case PATH_MHL:
	{
		int polarity = 1; 
		int mhl = (path == PATH_MHL);

		config_gpio_table_c2(mhl_usb_switch_ouput_table,
				ARRAY_SIZE(mhl_usb_switch_ouput_table));

		pr_info("[CABLE] %s: Set %s path\n", __func__, mhl ? "MHL" : "USB");
		gpio_set_value(PYRAMID_GPIO_MHL_USB_SWITCH, (mhl ^ !polarity) ? 1 : 0);
		break;
	}
	}

	#ifdef CONFIG_FB_MSM_HDMI_MHL
	sii9234_change_usb_owner((path == PATH_MHL)?1:0);
	#endif
}

static struct cable_detect_platform_data cable_detect_pdata = {
	.vbus_mpp_gpio	= PM8058_MPP_PM_TO_SYS(10),
	.vbus_mpp_config = pm8058_usb_config,
	.vbus_mpp_irq	= (PM8058_CBLPWR_IRQ + PM8058_IRQ_BASE),
	.detect_type	= CABLE_TYPE_PMIC_ADC,
	.usb_id_pin_gpio= PYRAMID_GPIO_USB_ID,
	.usb_dpdn_switch= pyramid_usb_dpdn_switch,
	.mhl_reset_gpio = PYRAMID_GPIO_MHL_RESET,
	.mpp_data = {
		.usbid_mpp	= PM8058_MPP_PM_TO_SYS(XOADC_MPP_4),
		.usbid_amux	= PM_MPP_AIN_AMUX_CH5,
	},
        .config_usb_id_gpios = config_pyramid_usb_id_gpios,
#ifdef CONFIG_FB_MSM_HDMI_MHL
	.mhl_1v2_power	= mhl_sii9234_1v2_power,
#endif
};

static struct platform_device cable_detect_device = {
	.name	= "cable_detect",
	.id	= -1,
	.dev	= {
		.platform_data = &cable_detect_pdata,
	},
};

#ifdef CONFIG_FB_MSM_HDMI_MHL
static int pm8901_mpp_hdmi_init(void);

static uint32_t msm_hdmi_off_gpio[] = {
	GPIO_CFG(170,  0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
	GPIO_CFG(171,  0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
	GPIO_CFG(172,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static uint32_t msm_hdmi_on_gpio[] = {
	GPIO_CFG(170,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
	GPIO_CFG(171,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
	GPIO_CFG(172,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

void hdmi_hpd_feature(int enable);

static void mhl_sii9234_1v2_power(bool enable)
{
	static bool prev_on;

	if (enable == prev_on)
		return;

	if (enable) {
		config_gpio_table_c2(msm_hdmi_on_gpio, ARRAY_SIZE(msm_hdmi_on_gpio));
		hdmi_hpd_feature(1);
		pr_info("%s(on): success\n", __func__);
	} else {
		config_gpio_table_c2(msm_hdmi_off_gpio, ARRAY_SIZE(msm_hdmi_off_gpio));
		hdmi_hpd_feature(0);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = enable;
}



#ifdef CONFIG_FB_MSM_HDMI_MHL_SII9234
static uint32_t mhl_gpio_table[] = {
	GPIO_CFG(PYRAMID_GPIO_MHL_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(PYRAMID_GPIO_MHL_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};
static int mhl_sii9234_all_power(bool enable);
static int mhl_sii9234_power(int on)
{
	int rc = 0;

	switch (on) {
	case 0:
		mhl_sii9234_1v2_power(false);
		break;
	case 1:
		mhl_sii9234_all_power(true);
		pm8901_mpp_hdmi_init();
		config_gpio_table_c2(mhl_gpio_table, ARRAY_SIZE(mhl_gpio_table));
		break;
	default:
		pr_warning("%s(%d) got unsupport parameter!!!\n", __func__, on);
		break;
	}
	return rc;
}

static T_MHL_PLATFORM_DATA mhl_sii9234_device_data = {
	.gpio_intr = PYRAMID_GPIO_MHL_INT,
	.gpio_reset = PYRAMID_GPIO_MHL_RESET,
	.ci2ca = 0,
	#ifdef CONFIG_FB_MSM_HDMI_MHL
	.mhl_usb_switch		= pyramid_usb_dpdn_switch,
	.mhl_1v2_power = mhl_sii9234_1v2_power,
	#endif
	.power = mhl_sii9234_power,
};

static struct i2c_board_info msm_i2c_gsbi7_mhl_sii9234_info[] =
{
	{
		I2C_BOARD_INFO(MHL_SII9234_I2C_NAME, 0x72 >> 1),
		.platform_data = &mhl_sii9234_device_data,
		.irq = MSM_GPIO_TO_INT(PYRAMID_GPIO_MHL_INT)
	},
};
#endif
#endif

#ifdef CONFIG_LEDS_PM8058
static struct pm8058_led_config pm_led_config[] = {
	{
		.name = "green",
		.type = PM8058_LED_RGB,
		.bank = 0,
		.pwm_size = 9,
		.clk = PM_PWM_CLK_32KHZ,
		.pre_div = PM_PWM_PREDIVIDE_2,
		.pre_div_exp = 1,
		.pwm_value = 511,
	},
	{
		.name = "amber",
		.type = PM8058_LED_RGB,
		.bank = 1,
		.pwm_size = 9,
		.clk = PM_PWM_CLK_32KHZ,
		.pre_div = PM_PWM_PREDIVIDE_2,
		.pre_div_exp = 1,
		.pwm_value = 511,
	},
	{
		.name = "button-backlight",
		.type = PM8058_LED_DRVX,
		.bank = 6,
		.flags = PM8058_LED_LTU_EN,
		.period_us = USEC_PER_SEC / 1000,
		.start_index = 0,
		.duites_size = 8,
		.duty_time_ms = 32,
		.lut_flag = PM_PWM_LUT_RAMP_UP | PM_PWM_LUT_PAUSE_HI_EN,
		.out_current = 10,
	},

};
static struct pm8058_led_platform_data pm8058_leds_data = {
	.led_config = pm_led_config,
	.num_leds = ARRAY_SIZE(pm_led_config),
	.duties = {0, 15, 30, 45, 60, 75, 90, 100,
		   100, 90, 75, 60, 45, 30, 15, 0,
		   0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0},
};

static struct platform_device pm8058_leds = {
	.name	= "leds-pm8058",
	.id	= -1,
	.dev	= {
		.platform_data	= &pm8058_leds_data,
	},
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
	.gpio_chg_stat = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, PYRAMID_CHG_STAT),
	.gpio_chg_int  = MSM_GPIO_TO_INT(PYRAMID_GPIO_CHG_INT),
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

static uint32_t gsbi4_gpio_table[] = {
	GPIO_CFG(PYRAMID_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi4_gpio_table_gpio[] = {
	GPIO_CFG(PYRAMID_CAM_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_CAM_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi5_gpio_table[] = {
	GPIO_CFG(PYRAMID_TP_I2C_SDA, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_TP_I2C_SCL, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi5_gpio_table_gpio[] = {
	GPIO_CFG(PYRAMID_TP_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_TP_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi7_gpio_table[] = {
	GPIO_CFG(PYRAMID_GENERAL_I2C_SDA, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_GENERAL_I2C_SCL, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi7_gpio_table_gpio[] = {
	GPIO_CFG(PYRAMID_GENERAL_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_GENERAL_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi10_gpio_table[] = {
	GPIO_CFG(PYRAMID_SENSOR_I2C_SDA, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIOMUX_DRV_8MA),
	GPIO_CFG(PYRAMID_SENSOR_I2C_SCL, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIOMUX_DRV_8MA),
};

static uint32_t gsbi10_gpio_table_gpio[] = {
	GPIO_CFG(PYRAMID_SENSOR_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(PYRAMID_SENSOR_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
	printk(KERN_INFO "%s(): adap_id = %d, config_type = %d \n", __func__, adap_id, config_type);

	if ((adap_id == MSM_GSBI4_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi4_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi4_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI4_QUP_I2C_BUS_ID) && (config_type == 0)) {
		gpio_tlmm_config(gsbi4_gpio_table_gpio[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi4_gpio_table_gpio[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI5_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi5_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi5_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI5_QUP_I2C_BUS_ID) && (config_type == 0)) {
		gpio_tlmm_config(gsbi5_gpio_table_gpio[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi5_gpio_table_gpio[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI7_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi7_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi7_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI7_QUP_I2C_BUS_ID) && (config_type == 0)) {
		gpio_tlmm_config(gsbi7_gpio_table_gpio[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi7_gpio_table_gpio[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI10_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi10_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi10_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI10_QUP_I2C_BUS_ID) && (config_type == 0)) {
		gpio_tlmm_config(gsbi10_gpio_table_gpio[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi10_gpio_table_gpio[1], GPIO_CFG_ENABLE);
	}
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

static struct msm_i2c_platform_data msm_gsbi5_qup_i2c_pdata = {
	.clk_freq = 100000,
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

static struct msm_i2c_platform_data msm_gsbi10_qup_i2c_pdata = {
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi12_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
        //	.use_gsbi_shared_mode = 1,
        //	.share_uart_flag = 1,   
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

#endif

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
static struct msm_spi_platform_data msm_gsbi1_qup_spi_pdata = {
	.max_clock_speed = 24000000,
};
#endif

#ifdef CONFIG_I2C_SSBI
/* PMIC SSBI */
static struct msm_i2c_ssbi_platform_data msm_ssbi2_pdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
};

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

static struct dsps_gpio_info dsps_pyramid_gpios[] = {
#ifndef CONFIG_MACH_PYRAMID
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
	msm_pil_dsps.dev.platform_data = DSPS_PIL_GENERIC_NAME;
	pdata->gpios = dsps_pyramid_gpios;
	pdata->gpios_num = ARRAY_SIZE(dsps_pyramid_gpios);

        platform_device_register(&msm_dsps_device);
}
#endif 

#define MSM_PMEM_SF_SIZE 0x4000000 /* 64 Mbytes */

#define MSM_PMEM_KERNEL_EBI1_SIZE  0x3BC000
#define MSM_PMEM_ADSP_SIZE         0x4200000
#define MSM_PMEM_AUDIO_SIZE        0x4CF000

#define MSM_SMI_BASE          0x38000000
#define MSM_SMI_SIZE          0x4000000

#define KERNEL_SMI_BASE       (MSM_SMI_BASE)
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
#define KERNEL_SMI_SIZE       0x000000
#else
#define KERNEL_SMI_SIZE       0x600000
#endif

#define USER_SMI_BASE         (KERNEL_SMI_BASE + KERNEL_SMI_SIZE)
#define USER_SMI_SIZE         (MSM_SMI_SIZE - KERNEL_SMI_SIZE)
#define MSM_PMEM_SMIPOOL_SIZE USER_SMI_SIZE

#ifdef CONFIG_MSM_CP
#define MSM_ION_HOLE_SIZE	SZ_128K /* (128KB) */
#else
#define MSM_ION_HOLE_SIZE	0
#endif

#define MSM_MM_FW_SIZE		(0x200000 - MSM_ION_HOLE_SIZE) /*(2MB-128KB)*/
#define MSM_ION_MM_SIZE		0x3800000  /* (56MB) */
#define MSM_ION_MFC_SIZE	SZ_8K

#define MSM_MM_FW_BASE		MSM_SMI_BASE
#define MSM_ION_HOLE_BASE	(MSM_MM_FW_BASE + MSM_MM_FW_SIZE)
#define MSM_ION_MM_BASE		(MSM_ION_HOLE_BASE + MSM_ION_HOLE_SIZE)
#define MSM_ION_MFC_BASE	(MSM_ION_MM_BASE + MSM_ION_MM_SIZE)

#ifdef CONFIG_MSM_CP
#define SECURE_BASE	(MSM_ION_HOLE_BASE)
#define SECURE_SIZE	(MSM_ION_MM_SIZE + MSM_ION_HOLE_SIZE)
#else
#define SECURE_BASE	(MSM_MM_FW_BASE)
#define SECURE_SIZE	(MSM_ION_MM_SIZE + MSM_MM_FW_SIZE)
#endif

#define MSM_ION_SF_SIZE		0x4000000 /* 64MB */
#define MSM_ION_CAMERA_SIZE     MSM_PMEM_ADSP_SIZE

#ifdef CONFIG_FB_MSM_OVERLAY1_WRITEBACK
#define MSM_ION_WB_SIZE		0xC00000 /* 12MB */
#else
#define MSM_ION_WB_SIZE		0x600000 /* 6MB */
#endif

#define MSM_ION_QSECOM_SIZE	0x600000 /* (6MB) */

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define MSM_ION_AUDIO_SIZE	MSM_PMEM_AUDIO_SIZE
#define MSM_ION_HEAP_NUM	9
#define MSM_HDMI_PRIM_ION_SF_SIZE MSM_HDMI_PRIM_PMEM_SF_SIZE
static unsigned msm_ion_sf_size = MSM_ION_SF_SIZE;
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
static unsigned pmem_sf_size = MSM_PMEM_SF_SIZE;
static int __init pmem_sf_size_setup(char *p)
{
	pmem_sf_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_sf_size", pmem_sf_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;

static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;

static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_ALLORNOTHING,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = {.platform_data = &android_pmem_pdata},
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};
#endif 

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
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct android_pmem_platform_data android_pmem_smipool_pdata = {
	.name = "pmem_smipool",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_SMI,
	.request_region = request_smi_region,
	.release_region = release_smi_region,
	.setup_region = setup_smi_region,
	.map_on_demand = 1,
};
static struct platform_device android_pmem_smipool_device = {
	.name = "android_pmem",
	.id = 7,
	.dev = { .platform_data = &android_pmem_smipool_pdata },
};
#endif 
#endif 

static void __init msm8x60_allocate_memory_regions(void)
{
        msm8x60_allocate_fb_region();
}

static int pyramid_ts_cy8c_set_rst(int on)
{
	struct pm8058_gpio_cfg {
		int                gpio;
		struct pm_gpio cfg;
	};

	struct pm8058_gpio_cfg tp_rst[] = {
		{ /* TW RST Set LOW */
			PM8058_GPIO_PM_TO_SYS(PYRAMID_TP_RST),
			{
				.direction	= PM_GPIO_DIR_OUT,
				.output_value	= 0,
				.output_buffer	= PM_GPIO_OUT_BUF_CMOS,
				.pull		= PM_GPIO_PULL_NO,
				.out_strength	= PM_GPIO_STRENGTH_HIGH,
				.function	= PM_GPIO_FUNC_NORMAL,
				.vin_sel	= PM8058_GPIO_VIN_S3,
				.inv_int_pol	= 0,
			}
		},
		{ /* TW RST Set HIGH */
			PM8058_GPIO_PM_TO_SYS(PYRAMID_TP_RST),
			{
				.direction	= PM_GPIO_DIR_OUT,
				.output_value	= 1,
				.output_buffer	= PM_GPIO_OUT_BUF_CMOS,
				.pull		= PM_GPIO_PULL_NO,
				.out_strength	= PM_GPIO_STRENGTH_HIGH,
				.function	= PM_GPIO_FUNC_NORMAL,
				.vin_sel	= PM8058_GPIO_VIN_S3,
				.inv_int_pol	= 0,
			}
		},
	};
	return pm8xxx_gpio_config(tp_rst[on].gpio,	&tp_rst[on].cfg);
}

static int pyramid_ts_cy8c_power(int on)
{
	printk(KERN_INFO "%s():\n", __func__);
	if (on)
		pyramid_ts_cy8c_set_rst(1);

	return 0;
}

static int pyramid_ts_cy8c_reset(void)
{
	printk(KERN_INFO "[TP] HW reset touch.\n");
#if 0
	printk(KERN_INFO "[TP] PYRAMID_TP_RST: %d, PM8058_GPIO_PM_TO_SYS(PYRAMID_TP_RST): %d\n",
		PYRAMID_TP_RST, PM8058_GPIO_PM_TO_SYS(PYRAMID_TP_RST));
#endif
	pyramid_ts_cy8c_set_rst(0);
	msleep(10);
	pyramid_ts_cy8c_set_rst(1);
	msleep(200);

	return 0;
}

struct cy8c_i2c_platform_data pyramid_ts_cy8c_data[] = {
	{
		.version = 0x0C,
		.timeout = 1,
		.unlock_attr = 1,
		.abs_x_min = 11,
		.abs_x_max = 1012,
		.abs_y_min = 6,
		.abs_y_max = 940,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 512,
		.power = pyramid_ts_cy8c_power,
		.gpio_irq = PYRAMID_TP_ATT_N_XB,
		/*.filter_level = {40, 80, 942, 982},*/
		.reset = pyramid_ts_cy8c_reset,
	},

	{
		.version = 0x08,
		.timeout = 1,
		.abs_x_min = 11,
		.abs_x_max = 1012,
		.abs_y_min = 6,
		.abs_y_max = 940,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 512,
		.power = pyramid_ts_cy8c_power,
		.gpio_irq = PYRAMID_TP_ATT_N_XB,
		/*.filter_level = {40, 80, 942, 982},*/
	},

	{
		.version = 0x00,
		.timeout = 1,
		.abs_x_min = 11,
		.abs_x_max = 1012,
		.abs_y_min = 6,
		.abs_y_max = 940,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 512,
		.power = pyramid_ts_cy8c_power,
		.gpio_irq = PYRAMID_TP_ATT_N_XB,
		.reset = pyramid_ts_cy8c_reset,
	},
};

#define PVT_VERSION	0x80

static void pyramid_ts_cy8c_set_system_rev(uint8_t rev)
{
	ssize_t i = 0;
#if 0
	printk(KERN_INFO "[TP] sizeof pyramid_ts_cy8c_data:%d, system_ver:%d\n",
	 sizeof(pyramid_ts_cy8c_data)/sizeof(struct cy8c_i2c_platform_data), rev);
#endif
	if (rev >= PVT_VERSION)
		for (i = 0; i < sizeof(pyramid_ts_cy8c_data)/sizeof(struct cy8c_i2c_platform_data); i++)
			pyramid_ts_cy8c_data[i].auto_reset = 1;
}

static struct i2c_board_info msm_i2c_gsbi5_info[] = {
	{
		I2C_BOARD_INFO(CYPRESS_TMA_NAME, 0x67),
		.platform_data = &pyramid_ts_cy8c_data,
		.irq = MSM_GPIO_TO_INT(PYRAMID_TP_ATT_N_XB)
	},
};

static ssize_t pyramid_virtual_keys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf,
		__stringify(EV_KEY) ":" __stringify(KEY_HOME)	":62:1015:110:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU)   ":200:1015:106:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_BACK)   ":340:1015:120:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":482:1015:110:100"
		"\n");
}

static struct kobj_attribute pyramid_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.cy8c-touchscreen",
		.mode = S_IRUGO,
	},
	.show = &pyramid_virtual_keys_show,
};

static struct attribute *pyramid_properties_attrs[] = {
	&pyramid_virtual_keys_attr.attr,
	NULL
};

static struct attribute_group pyramid_properties_attr_group = {
	.attrs = pyramid_properties_attrs,
};

#ifdef CONFIG_BT
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.inject_rx_on_wakeup = 0,

	
        //	.bt_wakeup_pin = PYRAMID_GPIO_BT_CHIP_WAKE,
        //	.host_wakeup_pin = PYRAMID_GPIO_BT_HOST_WAKE,
};

static struct platform_device pyramid_rfkill = {
	.name = "pyramid_rfkill",
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

#define RPM_VREG_INIT(_id, _min_uV, _max_uV, _modes, _ops, _apply_uV, \
		      _default_uV, _peak_uA, _avg_uA, _pull_down, _pin_ctrl, \
		      _freq, _pin_fn, _force_mode, _sleep_set_force_mode, \
		      _state, _sleep_selectable, _always_on) \
	{ \
		.init_data = { \
			.constraints = { \
				.valid_modes_mask	= _modes, \
				.valid_ops_mask		= _ops, \
				.min_uV			= _min_uV, \
				.max_uV			= _max_uV, \
				.input_uV		= _min_uV, \
				.apply_uV		= _apply_uV, \
				.always_on		= _always_on, \
			}, \
			.consumer_supplies	= vreg_consumers_##_id, \
			.num_consumer_supplies	= \
				ARRAY_SIZE(vreg_consumers_##_id), \
		}, \
		.id			= RPM_VREG_ID_##_id, \
		.default_uV		= _default_uV, \
		.peak_uA		= _peak_uA, \
		.avg_uA			= _avg_uA, \
		.pull_down_enable	= _pull_down, \
		.pin_ctrl		= _pin_ctrl, \
		.freq			= RPM_VREG_FREQ_##_freq, \
		.pin_fn			= _pin_fn, \
		.force_mode		= _force_mode, \
		.sleep_set_force_mode	= _sleep_set_force_mode, \
		.state			= _state, \
		.sleep_selectable	= _sleep_selectable, \
	}

#define RPM_PC(_id, _always_on, _pin_fn, _pin_ctrl) \
	{ \
		.init_data = { \
			.constraints = { \
				.valid_ops_mask	= REGULATOR_CHANGE_STATUS, \
				.always_on	= _always_on, \
			}, \
			.num_consumer_supplies	= \
					ARRAY_SIZE(vreg_consumers_##_id##_PC), \
			.consumer_supplies	= vreg_consumers_##_id##_PC, \
		}, \
		.id	  = RPM_VREG_ID_##_id##_PC, \
		.pin_fn	  = RPM_VREG_PIN_FN_8660_##_pin_fn, \
		.pin_ctrl = _pin_ctrl, \
	}


#define RPM_LDO(_id, _always_on, _pd, _sleep_selectable, _min_uV, _max_uV, \
		_init_peak_uA) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, RPM_VREG_PIN_CTRL_NONE, NONE, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_SMPS(_id, _always_on, _pd, _sleep_selectable, _min_uV, _max_uV, \
		 _init_peak_uA, _freq) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, RPM_VREG_PIN_CTRL_NONE, _freq, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_VS(_id, _always_on, _pd, _sleep_selectable) \
	RPM_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE, \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE, 0, 0, \
		      1000, 1000, _pd, RPM_VREG_PIN_CTRL_NONE, NONE, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_NCP(_id, _always_on, _pd, _sleep_selectable, _min_uV, _max_uV) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_NORMAL, \
		      REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS, 0, \
		      _min_uV, 1000, 1000, _pd, RPM_VREG_PIN_CTRL_NONE, NONE, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define LDO50HMIN	RPM_VREG_8660_LDO_50_HPM_MIN_LOAD
#define LDO150HMIN	RPM_VREG_8660_LDO_150_HPM_MIN_LOAD
#define LDO300HMIN	RPM_VREG_8660_LDO_300_HPM_MIN_LOAD
#define SMPS_HMIN	RPM_VREG_8660_SMPS_HPM_MIN_LOAD
#define FTS_HMIN	RPM_VREG_8660_FTSMPS_HPM_MIN_LOAD

static struct platform_device rpm_regulator_device __devinitdata = {
	.name	= "rpm-regulator",
	.id	= -1,
	.dev	= {
		.platform_data = &pyramid_rpm_regulator_pdata,
	},
};

static struct platform_device *early_devices[] __initdata = {
	&msm_device_saw_s0,
	&msm_device_saw_s1,
#ifdef CONFIG_MSM_BUS_SCALING
	&msm_bus_apps_fabric,
	&msm_bus_sys_fabric,
	&msm_bus_mm_fabric,
	&msm_bus_sys_fpb,
	&msm_bus_cpss_fpb,
#endif
	&msm_device_dmov_adm0,
	&msm_device_dmov_adm1,
#ifdef CONFIG_FLASHLIGHT_AAT1271
	&flashlight_device,
#endif
};

static struct platform_device msm_tsens_device = {
	.name   = "tsens-tm",
	.id = -1,
};

static struct htc_headset_pmic_platform_data htc_headset_pmic_data = {
	.driver_flag		= 0,
	.hpin_gpio		= PYRAMID_GPIO_AUD_HP_DET,
	.hpin_irq		= 0,
	.key_gpio		= PM8058_GPIO_PM_TO_SYS(PYRAMID_AUD_REMO_PRES),
	.key_irq		= 0,
	.key_enable_gpio	= 0,
	.adc_mic_bias		= {0, 0},
};

static struct platform_device htc_headset_pmic = {
	.name	= "HTC_HEADSET_PMIC",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_pmic_data,
	},
};

static struct htc_headset_8x60_platform_data htc_headset_8x60_data = {
	.adc_mpp	= PM8058_MPP_PM_TO_SYS(XOADC_MPP_10),
	.adc_amux	= PM_MPP_AIN_AMUX_CH5,
	.adc_mic_bias	= {HS_DEF_MIC_ADC_15_BIT_MIN,
			   HS_DEF_MIC_ADC_15_BIT_MAX},
	.adc_remote	= {0, 1721, 1722, 2927, 2928, 6300},
};

static struct platform_device htc_headset_8x60 = {
	.name	= "HTC_HEADSET_8X60",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_8x60_data,
	},
};

static struct platform_device *headset_devices[] = {
	&htc_headset_pmic,
	&htc_headset_8x60,
	
};

static struct headset_adc_config htc_headset_mgr_config[] = {
	{
		.type = HEADSET_MIC,
		.adc_max = 29158,
		.adc_min = 22766,
	},
	{
		.type = HEADSET_BEATS,
		.adc_max = 22765,
		.adc_min = 16698,
	},
	{
		.type = HEADSET_BEATS_SOLO,
		.adc_max = 16697,
		.adc_min = 7801,
	},
	{
		.type = HEADSET_NO_MIC,
		.adc_max = 7800,
		.adc_min = 0,
	},
};

static struct htc_headset_mgr_platform_data htc_headset_mgr_data = {
	.driver_flag		= 0,
	.headset_devices_num	= ARRAY_SIZE(headset_devices),
	.headset_devices	= headset_devices,
	.headset_config_num	= 0,
	.headset_config		= 0,
};

static struct platform_device htc_headset_mgr = {
	.name	= "HTC_HEADSET_MGR",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_mgr_data,
	},
};

static void headset_device_register(void)
{
	pr_info("[HS_BOARD] (%s) Headset device register\n", __func__);
	platform_device_register(&htc_headset_mgr);
}

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

static struct platform_device scm_log_device = {
    .name   = "scm-log2",
    .id = -1,
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
	.align = SZ_64K,
	.request_region = request_smi_region,
	.release_region = release_smi_region,
	.setup_region = setup_smi_region,
	.secure_base = SECURE_BASE,
	.secure_size = SECURE_SIZE,
	.iommu_map_all = 1,
	.iommu_2x_map_domain = VIDEO_DOMAIN,
};

static struct ion_cp_heap_pdata cp_mfc_ion_pdata = {
	.permission_type = IPT_TYPE_MFC_SHAREDMEM,
	.align = PAGE_SIZE,
	.request_region = request_smi_region,
	.release_region = release_smi_region,
	.setup_region = setup_smi_region,
};

static struct ion_cp_heap_pdata cp_wb_ion_pdata = {
	.permission_type = IPT_TYPE_MDP_WRITEBACK,
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
			.base	= MSM_MM_FW_BASE,
			.size	= MSM_MM_FW_SIZE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = (void *) &mm_fw_co_ion_pdata,
		},
		{
			.id	= ION_CP_MFC_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MFC_HEAP_NAME,
			.base	= MSM_ION_MFC_BASE,
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
			.id	= ION_CP_WB_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_WB_HEAP_NAME,
			.size	= MSM_ION_WB_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_wb_ion_pdata,
		},
		{
			.id	= ION_QSECOM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_QSECOM_HEAP_NAME,
			.size	= MSM_ION_QSECOM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_ion_pdata,
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

static struct platform_device *pyramid_devices[] __initdata = {
	&msm8x60_device_acpuclk,
	&msm_device_smd,
        &msm_device_uart_dm12,
        &msm_device_uart_dm3,
	&msm_device_otg,
	&msm_device_gadget_peripheral,
        &msm_device_hsusb_host,
	&android_usb_device,
	&msm_pil_q6v3,
	&msm_pil_modem,
	&msm_pil_tzapps,
	&msm_pil_dsps,
	&msm_pil_vidc,
#ifdef CONFIG_QSEECOM
	&qseecom_device,
#endif
#ifdef CONFIG_I2C_QUP
	&msm_gsbi3_qup_i2c_device,
	&msm_gsbi4_qup_i2c_device,
	&msm_gsbi5_qup_i2c_device,
	&msm_gsbi7_qup_i2c_device,
        &msm_gsbi8_qup_i2c_device,
	&msm_gsbi10_qup_i2c_device,
	&msm_gsbi12_qup_i2c_device,
#endif
#ifdef CONFIG_SERIAL_MSM_HS 
        &msm_device_uart_dm1,
#endif
#ifdef CONFIG_BT
	&pyramid_rfkill,
#endif

#ifdef CONFIG_MSM_SSBI
	&msm_device_ssbi_pmic1,
	&msm_device_ssbi_pmic2,
#endif
#ifdef CONFIG_I2C_SSBI
        &msm_device_ssbi2,
	&msm_device_ssbi3,
#endif
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
	&isp1763_device,
#endif

#if defined (CONFIG_MSM_8x60_VOIP)
	&asoc_msm_mvs,
	&asoc_mvs_dai0,
	&asoc_mvs_dai1,
#endif

#ifdef CONFIG_BATTERY_MSM
	&msm_batt_device,
#endif
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_smipool_device,
	&android_pmem_audio_device,
#endif 
#endif 
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
#ifdef CONFIG_MSM_GEMINI
	&msm_gemini_device,
#endif

#if defined(CONFIG_MSM_RPM_LOG) || defined(CONFIG_MSM_RPM_LOG_MODULE)
	&msm8660_rpm_log_device,
#endif
#if defined(CONFIG_MSM_RPM_STATS_LOG)
	&msm8660_rpm_stat_device,
#endif
	&msm_device_vidc,
#ifdef CONFIG_SENSORS_MSM_ADC
	&msm_adc_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#ifdef CONFIG_HTC_BATT_8x60
	&htc_battery_pdev,
#endif
#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif


#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
#ifdef CONFIG_MSM_USE_TSIF1
	&msm_device_tsif[1],
#else
	&msm_device_tsif[0],
#endif 
#endif 

#ifdef CONFIG_HW_RANDOM_MSM
	&msm_device_rng,
#endif
#ifdef CONFIG_LEDS_PM8058
	&pm8058_leds,
#endif
	&msm_tsens_device,
        &cable_detect_device,
	&msm8660_rpm_device,
#ifdef CONFIG_ION_MSM
	&ion_dev,
#endif
	&msm8660_device_watchdog,
	&msm_device_tz_log,
#ifdef CONFIG_MSM_RTB
	&msm_rtb_device,
#endif
	&msm8660_iommu_domain_device,
	&scm_log_device,
#ifdef CONFIG_PERFLOCK
	&msm8x60_device_perf_lock,
#endif
};

static struct memtype_reserve msm8x60_reserve_table[] __initdata = {
#if defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	[MEMTYPE_SMI] = {
		.start	=	MSM_SMI_BASE,
		.limit	=	MSM_SMI_SIZE,
		.flags	=	MEMTYPE_FLAGS_FIXED,
	},
#else
	
	
	
	
	
	[MEMTYPE_SMI_KERNEL] = {
		.start	=	KERNEL_SMI_BASE,
		.limit	=	KERNEL_SMI_SIZE,
		.size	=	KERNEL_SMI_SIZE,
		.flags	=	MEMTYPE_FLAGS_FIXED,
	},
	
	
	[MEMTYPE_SMI] = {
		.start	=	USER_SMI_BASE,
		.limit	=	USER_SMI_SIZE,
		.flags	=	MEMTYPE_FLAGS_FIXED,
	},
#endif
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

static void __init reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	unsigned int i;

	/* Verify size of heap is a multiple of 64K */
	for (i = 0; i < ion_pdata.nr; i++) {
		struct ion_platform_heap *heap = &(ion_pdata.heaps[i]);

		if (heap->extra_data &&
			heap->type == (enum ion_heap_type) ION_HEAP_TYPE_CP) {
			int map_all = ((struct ion_cp_heap_pdata *)
				heap->extra_data)->iommu_map_all;

			if (map_all && (heap->size & (SZ_64K-1))) {
				heap->size = ALIGN(heap->size, SZ_64K);
				pr_err("Heap %s size is not a multiple of 64K. Adjusting size to %x\n",
					heap->name, heap->size);

			}
		}
	}

	msm8x60_reserve_table[MEMTYPE_EBI1].size += msm_ion_sf_size;
	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_CAMERA_SIZE;
	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_WB_SIZE;
	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_AUDIO_SIZE;
	msm8x60_reserve_table[MEMTYPE_EBI1].size += MSM_ION_QSECOM_SIZE;
#endif
}

static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	android_pmem_adsp_pdata.size = pmem_adsp_size;
	android_pmem_smipool_pdata.size = MSM_PMEM_SMIPOOL_SIZE;
	android_pmem_pdata.size = pmem_sf_size;
	android_pmem_audio_pdata.size = MSM_PMEM_AUDIO_SIZE;
#endif /*CONFIG_MSM_MULTIMEDIA_USE_ION*/
#endif /*CONFIG_ANDROID_PMEM*/
}

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
        msm8x60_reserve_table[p->memory_type].size += p->size;
}
#endif 
#endif 

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
	reserve_memory_for(&android_pmem_audio_pdata);
#endif /*CONFIG_MSM_MULTIMEDIA_USE_ION*/
	msm8x60_reserve_table[MEMTYPE_EBI1].size += pmem_kernel_ebi1_size;
#endif /*CONFIG_ANDROID_PMEM*/
}

static void __init reserve_mdp_memory(void);

static void __init msm8x60_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
	reserve_ion_memory();
	reserve_mdp_memory();
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

static void __init pyramid_early_memory(void)
{
	reserve_info = &msm8x60_reserve_info;
}

static void __init pyramid_reserve(void)
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

#ifdef CONFIG_MSM8X60_AUDIO_1X
static uint32_t msm_spi_gpio[] = {
	GPIO_CFG(PYRAMID_SPI_DO,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(PYRAMID_SPI_DI,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(PYRAMID_SPI_CS,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(PYRAMID_SPI_CLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t auxpcm_gpio_table[] = {
	GPIO_CFG(111, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(112, 1, GPIO_CFG_INPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(113, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(114, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static void msm_auxpcm_init(void)
{
	gpio_tlmm_config(auxpcm_gpio_table[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(auxpcm_gpio_table[1], GPIO_CFG_ENABLE);
	gpio_tlmm_config(auxpcm_gpio_table[2], GPIO_CFG_ENABLE);
	gpio_tlmm_config(auxpcm_gpio_table[3], GPIO_CFG_ENABLE);
}

static struct tpa2051d3_platform_data tpa2051d3_pdata = {
	.gpio_tpa2051_spk_en = PYRAMID_AUD_HP_EN,
	.spkr_cmd = {0x00, 0x82, 0x00, 0x07, 0xCD, 0x4F, 0x0D},
	.hsed_cmd = {0x00, 0x8C, 0x20, 0x57, 0xCD, 0x4F, 0x0D},
};
#define TPA2051D3_I2C_SLAVE_ADDR	(0xE0 >> 1)

static struct i2c_board_info msm_i2c_gsbi7_tpa2051d3_info[] = {
	{
		I2C_BOARD_INFO(TPA2051D3_I2C_NAME, TPA2051D3_I2C_SLAVE_ADDR),
		.platform_data = &tpa2051d3_pdata,
	},
};

void msm_snddev_voltage_on(void)
{
}

void __init pyramid_audio_init(void);

void msm_snddev_voltage_off(void)
{
}

static struct spi_board_info msm_spi_board_info[] __initdata = {
	{
		.modalias	= "spi_aic3254",
		.mode           = SPI_MODE_1,
		.bus_num        = 0,
		.chip_select    = 0,
		.max_speed_hz   = 10800000,
	}
};
#endif 

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

static int isl29028_power(int pwr_device, uint8_t enable)
{
	return 0;
}
/*
thh_value = b_value + (((c_value - b_value) * a_value) / x_value)!

thl_value = b_value + ((c_value - b_value) / x_value)!

A: 3

X: 14
*/


static int isl29028_threoshold(int b, int c, int a, int x, int *thl_value, int *thh_value)
{
	int a_defult = 3, x_defult = 14;

	if (a == 0)
		a = a_defult;

	if (x == 0)
		x = x_defult;

	*thh_value = b + (((c - b) * a) / x);
	*thl_value = b + ((c - b) / x);
	return 0;
}


static struct isl29028_platform_data isl29028_pdata = {
	.intr = PM8058_GPIO_PM_TO_SYS(PYRAMID_PLS_INT),
	.levels = { 1, 3, 5, 15, 29, 339,
			588, 728, 869, 4095},
	.golden_adc = 450,
	.power = isl29028_power,
	.calibrate_func = isl29028_threoshold,
	.lt = 0x15,
	.ht = 0x16,
};

static struct i2c_board_info i2c_isl29028_devices[] = {
	{
		I2C_BOARD_INFO(ISL29028_I2C_NAME, 0x8A >> 1),
		.platform_data = &isl29028_pdata,
		.irq = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, PYRAMID_PLS_INT),
	},
};

static int isl29029_power(int pwr_device, uint8_t enable)
{
	return 0;
}

static struct isl29029_platform_data isl29029_pdata = {
	.intr = PM8058_GPIO_PM_TO_SYS(PYRAMID_PLS_INT),
	.levels = { 1, 3, 5, 15, 29, 339,
			588, 728, 869, 4095},
	.golden_adc = 450,
	.power = isl29029_power,
	.calibrate_func = isl29028_threoshold,
	.lt = 0x15,
	.ht = 0x16,
};

static struct i2c_board_info i2c_isl29029_devices[] = {
	{
		I2C_BOARD_INFO(ISL29029_I2C_NAME, 0x8A >> 1),
		.platform_data = &isl29029_pdata,
		.irq = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, PYRAMID_PLS_INT),
	},
};


static struct mpu3050_platform_data mpu3050_data = {
	.int_config = 0x10,
	.orientation = { -1, 0, 0,
					0, 1, 0,
					0, 0, -1 },
	.level_shifter = 0,

	.accel = {
		.get_slave_descr = get_accel_slave_descr,
		.adapt_num = MSM_GSBI10_QUP_I2C_BUS_ID, /* The i2c bus to which the mpu device is connected */
		.bus = EXT_SLAVE_BUS_SECONDARY,
		.address = 0x70 >> 1,
			.orientation = { -1, 0, 0,
							0, -1, 0,
							0, 0, 1 },

	},

	.compass = {
		.get_slave_descr = get_compass_slave_descr,
		.adapt_num = MSM_GSBI10_QUP_I2C_BUS_ID, /* The i2c bus to which the mpu device is connected */
		.bus = EXT_SLAVE_BUS_PRIMARY,
		.address = 0x18 >> 1,
			.orientation = { -1, 0, 0,
							0, 1, 0,
							0, 0, -1 },
	},
};

static struct mpu3050_platform_data mpu3050_data_XB = {
	.int_config = 0x10,
	.orientation = { -1, 0, 0,
					0, 1, 0,
					0, 0, -1 },
	.level_shifter = 0,

	.accel = {
		.get_slave_descr = get_accel_slave_descr,
		.adapt_num = MSM_GSBI10_QUP_I2C_BUS_ID, /* The i2c bus to which the mpu device is connected */
		.bus = EXT_SLAVE_BUS_SECONDARY,
		.address = 0x70 >> 1,
			.orientation = { -1, 0, 0,
							0, -1, 0,
							0, 0, 1 },

	},

	.compass = {
		.get_slave_descr = get_compass_slave_descr,
		.adapt_num = MSM_GSBI10_QUP_I2C_BUS_ID, /* The i2c bus to which the mpu device is connected */
		.bus = EXT_SLAVE_BUS_PRIMARY,
		.address = 0x1A >> 1,
			.orientation = { -1, 0, 0,
							0, 1, 0,
							0, 0, -1 },
	},
};


static struct i2c_board_info __initdata mpu3050_GSBI10_boardinfo[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0xD0 >> 1),
		.irq = MSM_GPIO_TO_INT(PYRAMID_GYRO_INT),
		.platform_data = &mpu3050_data,
	},
};

static struct i2c_board_info __initdata mpu3050_GSBI10_boardinfo_XB[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0xD0 >> 1),
		.irq = MSM_GPIO_TO_INT(PYRAMID_GYRO_INT),
		.platform_data = &mpu3050_data_XB,
	},
};

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
#ifdef CONFIG_TPS65200
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_tps_65200_boardinfo,
		ARRAY_SIZE(msm_tps_65200_boardinfo),
	},
#endif
#if defined(CONFIG_MSM8X60_AUDIO_1X)
	{
		I2C_SURF | I2C_FFA | I2C_FLUID | I2C_DRAGON,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_tpa2051d3_info,
		ARRAY_SIZE(msm_i2c_gsbi7_tpa2051d3_info),
	},
#endif
#ifdef CONFIG_FB_MSM_HDMI_MHL
#ifdef CONFIG_FB_MSM_HDMI_MHL_SII9234

	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_mhl_sii9234_info,
		ARRAY_SIZE(msm_i2c_gsbi7_mhl_sii9234_info),
	},

#endif
#endif
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI5_QUP_I2C_BUS_ID,
		msm_i2c_gsbi5_info,
		ARRAY_SIZE(msm_i2c_gsbi5_info),
	},
};
#endif 

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

	if (system_rev >= 1) {
		if (ps_type == 1) {
			i2c_register_board_info(MSM_GSBI10_QUP_I2C_BUS_ID,
				i2c_isl29028_devices,
				ARRAY_SIZE(i2c_isl29028_devices));
		} else if (ps_type == 2) {
			i2c_register_board_info(MSM_GSBI10_QUP_I2C_BUS_ID,
				i2c_isl29029_devices,
				ARRAY_SIZE(i2c_isl29029_devices));
		} else
			printk(KERN_DEBUG "No Intersil chips\n");

		i2c_register_board_info(MSM_GSBI10_QUP_I2C_BUS_ID,
				mpu3050_GSBI10_boardinfo_XB, ARRAY_SIZE(mpu3050_GSBI10_boardinfo_XB));
	} else {
		i2c_register_board_info(MSM_GSBI10_QUP_I2C_BUS_ID,
				mpu3050_GSBI10_boardinfo, ARRAY_SIZE(mpu3050_GSBI10_boardinfo));
	}

#endif
}

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
	msm_gsbi5_qup_i2c_device.dev.platform_data = &msm_gsbi5_qup_i2c_pdata;
	msm_gsbi7_qup_i2c_device.dev.platform_data = &msm_gsbi7_qup_i2c_pdata;
	msm_gsbi8_qup_i2c_device.dev.platform_data = &msm_gsbi8_qup_i2c_pdata;
	msm_gsbi10_qup_i2c_device.dev.platform_data = &msm_gsbi10_qup_i2c_pdata;
	msm_gsbi12_qup_i2c_device.dev.platform_data = &msm_gsbi12_qup_i2c_pdata;
#endif
#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
	msm_gsbi1_qup_spi_device.dev.platform_data = &msm_gsbi1_qup_spi_pdata;
#endif
#ifdef CONFIG_I2C_SSBI
	msm_device_ssbi2.dev.platform_data = &msm_ssbi2_pdata;
	msm_device_ssbi3.dev.platform_data = &msm_ssbi3_pdata;
#endif

	android_usb_pdata.swfi_latency = msm_rpmrs_levels[0].latency_us;
	msm_device_otg.dev.platform_data = &msm_otg_pdata;

#ifdef CONFIG_USB_GADGET_MSM_72K
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
#endif

#ifdef CONFIG_BT
	bt_export_bd_address();
#endif
#ifdef CONFIG_SERIAL_MSM_HS 
	msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(PYRAMID_GPIO_BT_HOST_WAKE);
	msm_device_uart_dm1.name = "msm_serial_hs_brcm";
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

static void __init pyramid_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_msm8x60_io();

	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");
}

#if (defined(CONFIG_MMC_MSM_SDC1_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC2_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC3_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC4_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC5_SUPPORT))

#define MAX_SDCC_CONTROLLER	5

struct msm_sdcc_gpio {
	
	s16 no;
	
	const char *name;
	bool always_on;
	bool is_enabled;
};

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static struct msm_sdcc_gpio sdc1_gpio_cfg[] = {
	{159, "sdc1_dat_0"},
	{160, "sdc1_dat_1"},
	{161, "sdc1_dat_2"},
	{162, "sdc1_dat_3"},
#ifdef CONFIG_MMC_MSM_SDC1_8_BIT_SUPPORT
	{163, "sdc1_dat_4"},
	{164, "sdc1_dat_5"},
	{165, "sdc1_dat_6"},
	{166, "sdc1_dat_7"},
#endif
	{167, "sdc1_clk"},
	{168, "sdc1_cmd"}
};

static uint32_t sdc1_on_gpio_table[] = {
	GPIO_CFG(159, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
	GPIO_CFG(160, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
	GPIO_CFG(161, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
	GPIO_CFG(162, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
#ifdef CONFIG_MMC_MSM_SDC1_8_BIT_SUPPORT
	GPIO_CFG(163, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
	GPIO_CFG(164, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
	GPIO_CFG(165, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
	GPIO_CFG(166, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_10MA), 
#endif
	GPIO_CFG(167, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_10MA), 
	GPIO_CFG(168, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_10MA), 
};
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static struct msm_sdcc_gpio sdc2_gpio_cfg[] = {
	{143, "sdc2_dat_0"},
	{144, "sdc2_dat_1", 1},
	{145, "sdc2_dat_2"},
	{146, "sdc2_dat_3"},
#ifdef CONFIG_MMC_MSM_SDC2_8_BIT_SUPPORT
	{147, "sdc2_dat_4"},
	{148, "sdc2_dat_5"},
	{149, "sdc2_dat_6"},
	{150, "sdc2_dat_7"},
#endif
	{151, "sdc2_cmd"},
	{152, "sdc2_clk", 1}
};
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
static struct msm_sdcc_gpio sdc5_gpio_cfg[] = {
	{95, "sdc5_cmd"},
	{96, "sdc5_dat_3"},
	{97, "sdc5_clk", 1},
	{98, "sdc5_dat_2"},
	{99, "sdc5_dat_1", 1},
	{100, "sdc5_dat_0"}
};
#endif

struct msm_sdcc_pad_pull_cfg {
	enum msm_tlmm_pull_tgt pull;
	u32 pull_val;
};

struct msm_sdcc_pad_drv_cfg {
	enum msm_tlmm_hdrive_tgt drv;
	u32 drv_val;
};

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct msm_sdcc_pad_drv_cfg sdc3_pad_on_drv_cfg[] = {
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC3_CMD, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC3_DATA, GPIO_CFG_8MA}
};

static struct msm_sdcc_pad_pull_cfg sdc3_pad_on_pull_cfg[] = {
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_sdcc_pad_drv_cfg sdc3_pad_off_drv_cfg[] = {
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC3_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC3_DATA, GPIO_CFG_2MA}
};

static struct msm_sdcc_pad_pull_cfg sdc3_pad_off_pull_cfg[] = {
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_DOWN},
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_DOWN}
};
#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct msm_sdcc_pad_drv_cfg sdc4_pad_on_drv_cfg[] = {
	{TLMM_HDRV_SDC4_CLK, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC4_CMD, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC4_DATA, GPIO_CFG_8MA}
};

static struct msm_sdcc_pad_pull_cfg sdc4_pad_on_pull_cfg[] = {
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_sdcc_pad_drv_cfg sdc4_pad_off_drv_cfg[] = {
	{TLMM_HDRV_SDC4_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC4_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC4_DATA, GPIO_CFG_2MA}
};

static struct msm_sdcc_pad_pull_cfg sdc4_pad_off_pull_cfg[] = {
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_DOWN},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_DOWN}
};
#endif

struct msm_sdcc_pin_cfg {
	u8 is_gpio;
	u8 cfg_sts;
	u8 gpio_data_size;
	struct msm_sdcc_gpio *gpio_data;
	struct msm_sdcc_pad_drv_cfg *pad_drv_on_data;
	struct msm_sdcc_pad_drv_cfg *pad_drv_off_data;
	struct msm_sdcc_pad_pull_cfg *pad_pull_on_data;
	struct msm_sdcc_pad_pull_cfg *pad_pull_off_data;
	u8 pad_drv_data_size;
	u8 pad_pull_data_size;
	u8 sdio_lpm_gpio_cfg;
};


static struct msm_sdcc_pin_cfg sdcc_pin_cfg_data[MAX_SDCC_CONTROLLER] = {
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	[0] = {
		.is_gpio = 1,
		.gpio_data_size = ARRAY_SIZE(sdc1_gpio_cfg),
		.gpio_data = sdc1_gpio_cfg
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	[1] = {
		.is_gpio = 1,
		.gpio_data_size = ARRAY_SIZE(sdc2_gpio_cfg),
		.gpio_data = sdc2_gpio_cfg
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	[2] = {
		.is_gpio = 0,
		.pad_drv_on_data = sdc3_pad_on_drv_cfg,
		.pad_drv_off_data = sdc3_pad_off_drv_cfg,
		.pad_pull_on_data = sdc3_pad_on_pull_cfg,
		.pad_pull_off_data = sdc3_pad_off_pull_cfg,
		.pad_drv_data_size = ARRAY_SIZE(sdc3_pad_on_drv_cfg),
		.pad_pull_data_size = ARRAY_SIZE(sdc3_pad_on_pull_cfg)
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	[3] = {
		.is_gpio = 0,
		.pad_drv_on_data = sdc4_pad_on_drv_cfg,
		.pad_drv_off_data = sdc4_pad_off_drv_cfg,
		.pad_pull_on_data = sdc4_pad_on_pull_cfg,
		.pad_pull_off_data = sdc4_pad_off_pull_cfg,
		.pad_drv_data_size = ARRAY_SIZE(sdc4_pad_on_drv_cfg),
		.pad_pull_data_size = ARRAY_SIZE(sdc4_pad_on_pull_cfg)
	},
#endif
#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
	[4] = {
		.is_gpio = 1,
		.gpio_data_size = ARRAY_SIZE(sdc5_gpio_cfg),
		.gpio_data = sdc5_gpio_cfg
	}
#endif
};

static int msm_sdcc_setup_gpio(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct msm_sdcc_pin_cfg *curr;
	int n;

	curr = &sdcc_pin_cfg_data[dev_id - 1];
	if (!curr->gpio_data)
		goto out;

	for (n = 0; n < curr->gpio_data_size; n++) {
		if (enable) {

			if (curr->gpio_data[n].always_on &&
				curr->gpio_data[n].is_enabled)
				continue;
			pr_debug("%s: enable: %s\n", __func__,
					curr->gpio_data[n].name);
			rc = gpio_request(curr->gpio_data[n].no,
				curr->gpio_data[n].name);
			if (rc) {
				pr_err("%s: gpio_request(%d, %s)"
					"failed", __func__,
					curr->gpio_data[n].no,
					curr->gpio_data[n].name);
				goto free_gpios;
			}
			
			rc = gpio_direction_output(
				curr->gpio_data[n].no, 1);
			if (rc) {
				pr_err("%s: gpio_direction_output"
					"(%d, 1) failed\n", __func__,
					curr->gpio_data[n].no);
				goto free_gpios;
			}
			curr->gpio_data[n].is_enabled = 1;
		} else {
			if (curr->gpio_data[n].always_on)
				continue;
			pr_debug("%s: disable: %s\n", __func__,
					curr->gpio_data[n].name);
			gpio_free(curr->gpio_data[n].no);
			curr->gpio_data[n].is_enabled = 0;
		}
	}
	curr->cfg_sts = enable;
	goto out;

free_gpios:
	for (; n >= 0; n--)
		gpio_free(curr->gpio_data[n].no);
out:
	return rc;
}

static int msm_sdcc_setup_pad(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct msm_sdcc_pin_cfg *curr;
	int n;

	curr = &sdcc_pin_cfg_data[dev_id - 1];
	if (!curr->pad_drv_on_data || !curr->pad_pull_on_data)
		goto out;

	if (enable) {
		for (n = 0; n < curr->pad_drv_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_drv_on_data[n].drv ==
						TLMM_HDRV_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_hdrive(curr->pad_drv_on_data[n].drv,
				curr->pad_drv_on_data[n].drv_val);
		}
		for (n = 0; n < curr->pad_pull_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_pull_on_data[n].pull ==
						TLMM_PULL_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_pull(curr->pad_pull_on_data[n].pull,
				curr->pad_pull_on_data[n].pull_val);
		}
	} else {
		
		for (n = 0; n < curr->pad_drv_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_drv_off_data[n].drv ==
						TLMM_HDRV_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_hdrive(
				curr->pad_drv_off_data[n].drv,
				curr->pad_drv_off_data[n].drv_val);
		}
		for (n = 0; n < curr->pad_pull_data_size; n++) {
			if (curr->sdio_lpm_gpio_cfg) {
				if (curr->pad_pull_off_data[n].pull ==
						TLMM_PULL_SDC4_DATA)
					continue;
			}
			msm_tlmm_set_pull(
				curr->pad_pull_off_data[n].pull,
				curr->pad_pull_off_data[n].pull_val);
		}
	}
	curr->cfg_sts = enable;
out:
	return rc;
}

struct sdcc_reg {
	
	const char *reg_name;
	unsigned char set_voltage_sup;
	
	unsigned int level;
	
	struct regulator *reg;
	
	bool enabled;
	
	bool always_on;
	
	bool op_pwr_mode_sup;
	
	unsigned int lpm_uA;
	unsigned int hpm_uA;
};
static struct sdcc_reg sdcc_vdd_reg_data[MAX_SDCC_CONTROLLER];
static struct sdcc_reg sdcc_vccq_reg_data[1];
static struct sdcc_reg sdcc_vddp_reg_data[MAX_SDCC_CONTROLLER];

struct sdcc_reg_data {
	struct sdcc_reg *vdd_data; 
	struct sdcc_reg *vccq_data; 
	struct sdcc_reg *vddp_data; 
	unsigned char sts; 
};
static struct sdcc_reg_data sdcc_vreg_data[MAX_SDCC_CONTROLLER];

static int msm_sdcc_vreg_init_reg(struct sdcc_reg *vreg)
{
	int rc = 0;

	
	vreg->reg = regulator_get(NULL, vreg->reg_name);
	if (IS_ERR(vreg->reg)) {
		rc = PTR_ERR(vreg->reg);
		pr_err("%s: regulator_get(%s) failed. rc=%d\n",
			__func__, vreg->reg_name, rc);
		goto out;
	}

	
	if (vreg->set_voltage_sup) {
		rc = regulator_set_voltage(vreg->reg, vreg->level,
					vreg->level);
		if (rc) {
			pr_err("%s: regulator_set_voltage(%s) failed rc=%d\n",
				__func__, vreg->reg_name, rc);
			goto vreg_put;
		}
	}
	goto out;

vreg_put:
	regulator_put(vreg->reg);
out:
	return rc;
}

static inline void msm_sdcc_vreg_deinit_reg(struct sdcc_reg *vreg)
{
	regulator_put(vreg->reg);
}

static int msm_sdcc_vreg_init(int dev_id, unsigned char init)
{
	int rc = 0;
	struct sdcc_reg *curr_vdd_reg, *curr_vccq_reg, *curr_vddp_reg;
	struct sdcc_reg_data *curr;

	curr = &sdcc_vreg_data[dev_id - 1];
	curr_vdd_reg = curr->vdd_data;
	curr_vccq_reg = curr->vccq_data;
	curr_vddp_reg = curr->vddp_data;

	if (init) {
		if (curr_vdd_reg) {
			rc = msm_sdcc_vreg_init_reg(curr_vdd_reg);
			if (rc)
				goto out;
		}
		if (curr_vccq_reg) {
			rc = msm_sdcc_vreg_init_reg(curr_vccq_reg);
			if (rc)
				goto vdd_reg_deinit;
		}
		if (curr_vddp_reg) {
			rc = msm_sdcc_vreg_init_reg(curr_vddp_reg);
			if (rc)
				goto vccq_reg_deinit;
		}
		goto out;
	} else
		
		goto vddp_reg_deinit;

vddp_reg_deinit:
	if (curr_vddp_reg)
		msm_sdcc_vreg_deinit_reg(curr_vddp_reg);
vccq_reg_deinit:
	if (curr_vccq_reg)
		msm_sdcc_vreg_deinit_reg(curr_vccq_reg);
vdd_reg_deinit:
	if (curr_vdd_reg)
		msm_sdcc_vreg_deinit_reg(curr_vdd_reg);
out:
	return rc;
}

static int msm_sdcc_vreg_enable(struct sdcc_reg *vreg)
{
	int rc;

	if (!vreg->enabled) {
		rc = regulator_enable(vreg->reg);
		if (rc) {
			pr_err("%s: regulator_enable(%s) failed. rc=%d\n",
				__func__, vreg->reg_name, rc);
			goto out;
		}
		vreg->enabled = 1;
	}

	
	if (vreg->always_on && vreg->op_pwr_mode_sup) {
		rc = regulator_set_optimum_mode(vreg->reg, vreg->hpm_uA);
		if (rc < 0) {
			pr_err("%s: reg=%s: HPM setting failed"
				" hpm_uA=%d, rc=%d\n",
				__func__, vreg->reg_name,
				vreg->hpm_uA, rc);
			goto vreg_disable;
		}
		rc = 0;
	}
	goto out;

vreg_disable:
	regulator_disable(vreg->reg);
	vreg->enabled = 0;
out:
	return rc;
}

static int msm_sdcc_vreg_disable(struct sdcc_reg *vreg)
{
	int rc;

	
	if (!vreg->always_on) {
		rc = regulator_disable(vreg->reg);
		if (rc) {
			pr_err("%s: regulator_disable(%s) failed. rc=%d\n",
				__func__, vreg->reg_name, rc);
			goto out;
		}
		vreg->enabled = 0;
	}

	
	if (vreg->always_on && vreg->op_pwr_mode_sup) {
		rc = regulator_set_optimum_mode(vreg->reg, vreg->lpm_uA);
		if (rc < 0) {
			pr_err("%s: reg=%s: LPM setting failed"
				" lpm_uA=%d, rc=%d\n",
				__func__,
				vreg->reg_name,
				vreg->lpm_uA, rc);
			goto out;
		}
		rc = 0;
	}

out:
	return rc;
}

static int msm_sdcc_setup_vreg(int dev_id, unsigned char enable)
{
	int rc = 0;
	struct sdcc_reg *curr_vdd_reg, *curr_vccq_reg, *curr_vddp_reg;
	struct sdcc_reg_data *curr;

	curr = &sdcc_vreg_data[dev_id - 1];
	curr_vdd_reg = curr->vdd_data;
	curr_vccq_reg = curr->vccq_data;
	curr_vddp_reg = curr->vddp_data;

	
	if ((curr_vdd_reg && !curr_vdd_reg->reg) ||
		(curr_vccq_reg && !curr_vccq_reg->reg) ||
		(curr_vddp_reg && !curr_vddp_reg->reg)) {
		
		rc = msm_sdcc_vreg_init(dev_id, 1);
		if (rc) {
			pr_err("%s: regulator init failed = %d\n",
				__func__, rc);
			goto out;
		}
	}

	if (curr->sts == enable)
		goto out;

	if (curr_vdd_reg) {
		if (enable)
			rc = msm_sdcc_vreg_enable(curr_vdd_reg);
		else
			rc = msm_sdcc_vreg_disable(curr_vdd_reg);
		if (rc)
			goto out;
	}

	if (curr_vccq_reg) {
		if (enable)
			rc = msm_sdcc_vreg_enable(curr_vccq_reg);
		else
			rc = msm_sdcc_vreg_disable(curr_vccq_reg);
		if (rc)
			goto out;
	}

	if (curr_vddp_reg) {
		if (enable)
			rc = msm_sdcc_vreg_enable(curr_vddp_reg);
		else
			rc = msm_sdcc_vreg_disable(curr_vddp_reg);
		if (rc)
			goto out;
	}
	curr->sts = enable;

out:
	return rc;
}

static u32 msm_sdcc_setup_power(struct device *dv, unsigned int vdd)
{
	u32 rc_pin_cfg = 0;
	u32 rc_vreg_cfg = 0;
	u32 rc = 0;
	struct platform_device *pdev;
	struct msm_sdcc_pin_cfg *curr_pin_cfg;

	pdev = container_of(dv, struct platform_device, dev);
	
	curr_pin_cfg = &sdcc_pin_cfg_data[pdev->id - 1];
	if (curr_pin_cfg->cfg_sts == !!vdd)
		goto setup_vreg;

	if (curr_pin_cfg->is_gpio)
		rc_pin_cfg = msm_sdcc_setup_gpio(pdev->id, !!vdd);
	else
		rc_pin_cfg = msm_sdcc_setup_pad(pdev->id, !!vdd);

setup_vreg:
	
	rc_vreg_cfg = msm_sdcc_setup_vreg(pdev->id, !!vdd);

	if (rc_pin_cfg || rc_vreg_cfg)
		rc = rc_pin_cfg ? rc_pin_cfg : rc_vreg_cfg;

	return rc;
}
#if 0
static void msm_sdcc_sdio_lpm_gpio(struct device *dv, unsigned int active)
{
	struct msm_sdcc_pin_cfg *curr_pin_cfg;
	struct platform_device *pdev;

	pdev = container_of(dv, struct platform_device, dev);
	
	curr_pin_cfg = &sdcc_pin_cfg_data[pdev->id - 1];

	if (curr_pin_cfg->cfg_sts == active)
		return;

	curr_pin_cfg->sdio_lpm_gpio_cfg = 1;
	if (curr_pin_cfg->is_gpio)
		msm_sdcc_setup_gpio(pdev->id, active);
	else
		msm_sdcc_setup_pad(pdev->id, active);
	curr_pin_cfg->sdio_lpm_gpio_cfg = 0;
}

static int msm_sdc3_get_wpswitch(struct device *dev)
{
	struct platform_device *pdev;
	int status;
	pdev = container_of(dev, struct platform_device, dev);

	status = gpio_request(GPIO_SDC_WP, "SD_WP_Switch");
	if (status) {
		pr_err("%s:Failed to request GPIO %d\n",
					__func__, GPIO_SDC_WP);
	} else {
		status = gpio_direction_input(GPIO_SDC_WP);
		if (!status) {
			status = gpio_get_value_cansleep(GPIO_SDC_WP);
			pr_info("%s: WP Status for Slot %d = %d\n",
				 __func__, pdev->id, status);
		}
		gpio_free(GPIO_SDC_WP);
	}
	return status;
}
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
int sdc5_register_status_notify(void (*callback)(int, void *),
	void *dev_id)
{
	sdc5_status_notify_cb = callback;
	sdc5_status_notify_cb_devid = dev_id;
	return 0;
}
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
int sdc2_register_status_notify(void (*callback)(int, void *),
	void *dev_id)
{
	sdc2_status_notify_cb = callback;
	sdc2_status_notify_cb_devid = dev_id;
	return 0;
}
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static unsigned int msm8x60_sdcc_slot_status(struct device *dev)
{
	int status;

	status = gpio_request(PM8058_GPIO_PM_TO_SYS(PYRAMID_SDC3_DET)
				, "SD_HW_Detect");
	if (status) {
		pr_err("%s:Failed to request GPIO %d\n", __func__,
				PM8058_GPIO_PM_TO_SYS(PYRAMID_SDC3_DET));
	} else {
		status = gpio_direction_input(
				PM8058_GPIO_PM_TO_SYS(PYRAMID_SDC3_DET));
		if (!status)
			status = !(gpio_get_value_cansleep(
				PM8058_GPIO_PM_TO_SYS(PYRAMID_SDC3_DET)));
		gpio_free(PM8058_GPIO_PM_TO_SYS(PYRAMID_SDC3_DET));
	}
	return (unsigned int) status;
}
#endif
#endif

#define MSM_MPM_PIN_SDC3_DAT1	21
#define MSM_MPM_PIN_SDC4_DAT1	23

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static struct mmc_platform_data msm8x60_sdc1_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
        .translate_vdd  = msm_sdcc_setup_power,
#ifdef CONFIG_MMC_MSM_SDC1_8_BIT_SUPPORT
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,
#else
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#endif
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 1,
        //        .hc_erase_group_def =1,
        //        .msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
        //        .bkops_support = 1,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static struct mmc_platform_data msm8x60_sdc2_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_165_195,
	.translate_vdd  = msm_sdcc_setup_power,
	.sdio_lpm_gpio_setup = msm_sdcc_sdio_lpm_gpio,
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.register_status_notify = sdc2_register_status_notify,
#ifdef CONFIG_MSM_SDIO_AL
	.is_sdio_al_client = 1,
#endif
	.msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct mmc_platform_data msm8x60_sdc3_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd  = msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.status      = msm8x60_sdcc_slot_status,
	.status_irq  = PM8058_GPIO_IRQ(PM8058_IRQ_BASE,
				       PYRAMID_SDC3_DET),
	.irq_flags   = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
        //	.mpm_sdiowakeup_int = MSM_MPM_PIN_SDC3_DAT1,
        .msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
};
#endif

#if 0
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct mmc_platform_data msm8x60_sdc4_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd  = msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.mpm_sdiowakeup_int = MSM_MPM_PIN_SDC4_DAT1,
	.msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
};
#endif
#endif

#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
static struct mmc_platform_data msm8x60_sdc5_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_165_195,
	.translate_vdd  = msm_sdcc_setup_power,
	.sdio_lpm_gpio_setup = msm_sdcc_sdio_lpm_gpio,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
	.nonremovable	= 0,
	.register_status_notify = sdc5_register_status_notify,
#ifdef CONFIG_MSM_SDIO_AL
	.is_sdio_al_client = 1,
#endif
	.msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
};
#endif

static void __init msm8x60_init_mmc(void)
{
	int ret;

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	config_gpio_table_c2(sdc1_on_gpio_table, ARRAY_SIZE(sdc1_on_gpio_table));
	
	sdcc_vreg_data[0].vdd_data = &sdcc_vdd_reg_data[0];
	sdcc_vreg_data[0].vdd_data->reg_name = "8901_l5";
	sdcc_vreg_data[0].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[0].vdd_data->level = 2850000;
	sdcc_vreg_data[0].vdd_data->always_on = 1;
	sdcc_vreg_data[0].vdd_data->op_pwr_mode_sup = 1;
	sdcc_vreg_data[0].vdd_data->lpm_uA = 9000;
	sdcc_vreg_data[0].vdd_data->hpm_uA = 200000;

	sdcc_vreg_data[0].vccq_data = &sdcc_vccq_reg_data[0];
	sdcc_vreg_data[0].vccq_data->reg_name = "8901_lvs0";
	sdcc_vreg_data[0].vccq_data->set_voltage_sup = 0;
	sdcc_vreg_data[0].vccq_data->always_on = 1;

	msm_add_sdcc(1, &msm8x60_sdc1_data);
#endif
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	sdcc_vreg_data[1].vdd_data = &sdcc_vdd_reg_data[1];
	sdcc_vreg_data[1].vdd_data->reg_name = "8058_s3";
	sdcc_vreg_data[1].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[1].vdd_data->level = 1800000;

	sdcc_vreg_data[1].vccq_data = NULL;

	if (machine_is_msm8x60_fusion())
		msm8x60_sdc2_data.msmsdcc_fmax = 24000000;
	if (machine_is_msm8x60_fusion() || machine_is_msm8x60_fusn_ffa()) {
		msm8x60_sdc2_data.sdiowakeup_irq = gpio_to_irq(144);
		msm_sdcc_setup_gpio(2, 1);
		msm_add_sdcc(2, &msm8x60_sdc2_data);
	}
#endif
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	
	sdcc_vreg_data[2].vdd_data = &sdcc_vdd_reg_data[2];
	sdcc_vreg_data[2].vdd_data->reg_name = "8058_l14";
	sdcc_vreg_data[2].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[2].vdd_data->level = 2850000;
	sdcc_vreg_data[2].vdd_data->always_on = 1;
	sdcc_vreg_data[2].vdd_data->op_pwr_mode_sup = 1;
	sdcc_vreg_data[2].vdd_data->lpm_uA = 9000;
	sdcc_vreg_data[2].vdd_data->hpm_uA = 200000;

	sdcc_vreg_data[2].vccq_data = NULL;

	sdcc_vreg_data[2].vddp_data = &sdcc_vddp_reg_data[2];
	sdcc_vreg_data[2].vddp_data->reg_name = "8058_l5";
	sdcc_vreg_data[2].vddp_data->set_voltage_sup = 1;
	sdcc_vreg_data[2].vddp_data->level = 2850000;
	sdcc_vreg_data[2].vddp_data->always_on = 1;
	sdcc_vreg_data[2].vddp_data->op_pwr_mode_sup = 1;
	sdcc_vreg_data[2].vddp_data->lpm_uA = 2000;
	
	sdcc_vreg_data[2].vddp_data->hpm_uA = 16000;
        msm_add_sdcc(3, &msm8x60_sdc3_data);
#endif
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	
	ret = pyramid_init_mmc();
	if (ret != 0)
		printk(KERN_ERR "%s: Unable to initialize MMC (SDCC4)\n", __func__);
#if 0 
	sdcc_vreg_data[3].vdd_data = &sdcc_vdd_reg_data[3];
	sdcc_vreg_data[3].vdd_data->reg_name = "8058_s3";
	sdcc_vreg_data[3].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[3].vdd_data->level = 1800000;

	sdcc_vreg_data[3].vccq_data = NULL;

	msm_add_sdcc(4, &msm8x60_sdc4_data);
#endif
#endif
#ifdef CONFIG_MMC_MSM_SDC5_SUPPORT
	sdcc_vreg_data[4].vdd_data = &sdcc_vdd_reg_data[4];
	sdcc_vreg_data[4].vdd_data->reg_name = "8058_s3";
	sdcc_vreg_data[4].vdd_data->set_voltage_sup = 1;
	sdcc_vreg_data[4].vdd_data->level = 1800000;

	sdcc_vreg_data[4].vccq_data = NULL;

	if (machine_is_msm8x60_fusion())
		msm8x60_sdc5_data.msmsdcc_fmax = 24000000;
	if (machine_is_msm8x60_fusion() || machine_is_msm8x60_fusn_ffa()) {
		msm8x60_sdc5_data.sdiowakeup_irq = gpio_to_irq(99);
		msm_sdcc_setup_gpio(5, 1);
		msm_add_sdcc(5, &msm8x60_sdc5_data);
	}
#endif
}

#define _GET_REGULATOR(var, name) do {					\
	if (var == NULL) {						\
		var = regulator_get(NULL, name);			\
		if (IS_ERR(var)) {					\
			pr_err("'%s' regulator not found, rc=%ld\n",	\
				name, PTR_ERR(var));			\
			var = NULL;					\
		}							\
	}								\
} while (0)

static void __init reserve_mdp_memory(void)
{
        msm8x60_mdp_writeback(msm8x60_reserve_table);
}

static void __init msm8x60_cfg_smsc911x(void)
{
	smsc911x_resources[1].start =
		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 6);
	smsc911x_resources[1].end =
		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 6);
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

struct msm_board_data {
	struct msm_gpiomux_configs *gpiomux_cfgs;
};

static struct msm_board_data pyramid_board_data __initdata = {
	.gpiomux_cfgs = msm8x60_pyramid_gpiomux_cfgs,
};

void pyramid_add_usb_devices(void)
{
	printk(KERN_INFO "%s rev: %d\n", __func__, system_rev);
}

#if 0 // LPM
#define PM8058_LPM_SET(id)	(1 << RPM_VREG_ID_##id)
#define PM8901_LPM_SET(id)	(1 << (RPM_VREG_ID_##id - RPM_VREG_ID_PM8901_L0))

static uint32_t __initdata pyramid_regulator_lpm_set[] =
{
	PM8058_LPM_SET(PM8058_L0) | PM8058_LPM_SET(PM8058_L1) | PM8058_LPM_SET(PM8058_L2) |
	PM8058_LPM_SET(PM8058_L5) | PM8058_LPM_SET(PM8058_L6) | PM8058_LPM_SET(PM8058_L7) |
	PM8058_LPM_SET(PM8058_L8) | PM8058_LPM_SET(PM8058_L9) | PM8058_LPM_SET(PM8058_L10) |
	PM8058_LPM_SET(PM8058_L11) | PM8058_LPM_SET(PM8058_L13) |
	PM8058_LPM_SET(PM8058_L15) | PM8058_LPM_SET(PM8058_L16) | PM8058_LPM_SET(PM8058_L17) |
	PM8058_LPM_SET(PM8058_L18) | PM8058_LPM_SET(PM8058_L19) | PM8058_LPM_SET(PM8058_L20) |
	PM8058_LPM_SET(PM8058_L21) | PM8058_LPM_SET(PM8058_L22) | PM8058_LPM_SET(PM8058_L23) |
	PM8058_LPM_SET(PM8058_L24) | PM8058_LPM_SET(PM8058_L25),
	PM8901_LPM_SET(PM8901_L0) | PM8901_LPM_SET(PM8901_L1) | PM8901_LPM_SET(PM8901_L2) |
	PM8901_LPM_SET(PM8901_L3) | PM8901_LPM_SET(PM8901_L4),
};
#endif

static void __init msm8x60_gfx_init(void)
{
	msm8x60_check_2d_hardware();
	platform_device_register(&msm_kgsl_3d0);
	platform_device_register(&msm_kgsl_2d0);
	platform_device_register(&msm_kgsl_2d1);
}

static void __init pyramid_init(void)
{
        int rc;
	uint32_t soc_platform_version;
	uint32_t raw_speed_bin, speed_bin;
	struct kobject *properties_kobj;

#ifdef AUTORESTART
        delay = msecs_to_jiffies(AUTORESTART);
        wq = create_singlethread_workqueue("my");
        queue_delayed_work(wq, &my_work, delay);
#endif

	if (meminfo_init(SYS_MEMORY, SZ_256M) < 0)
		pr_err("meminfo_init() failed!\n");

	htc_add_ramconsole_devices();
	platform_device_register(&msm_gpio_device);

	raw_speed_bin = readl(QFPROM_SPEED_BIN_ADDR);
	speed_bin = raw_speed_bin & 0xF;

	BUG_ON(msm_rpm_init(&msm8660_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(&msm_rpmrs_data));

	regulator_suppress_info_printing();

	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");
	
	platform_device_register(&rpm_regulator_device);

	msm_clock_init(&msm8x60_clock_init_data);

        //        msm8x60_init_gpiomux(pyramid_board_data.gpiomux_cfgs);

        pyramid_init_gpiomux();
	pyramid_init_pmic();

        msm8x60_i2c_init();
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

	platform_add_devices(early_devices, ARRAY_SIZE(early_devices));

	msm8x60_init_uart12dm();
	msm8x60_init_mmc();

#ifdef CONFIG_MSM_CAMERA
        msm8x60_init_cam();
#endif

	/* Accessory */
	printk(KERN_INFO "[HS_BOARD] (%s) system_rev = %d, LE = %d\n", __func__,
	       system_rev, (speed_bin == 0x1) ? 1 : 0);
	if (system_rev > 2 || speed_bin == 0x1) {
		htc_headset_pmic_data.key_gpio =
			PM8058_GPIO_PM_TO_SYS(PYRAMID_AUD_REMO_PRES);
		htc_headset_mgr_data.headset_config_num =
			ARRAY_SIZE(htc_headset_mgr_config);
		htc_headset_mgr_data.headset_config = htc_headset_mgr_config;
		printk(KERN_INFO "[HS_BOARD] (%s) Set MEMS config\n", __func__);
	}

#ifdef CONFIG_BATTERY_MSM8X60
        platform_device_register(&msm_charger_device);
#endif

	msm8x60_cfg_smsc911x();

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 1)
		platform_add_devices(msm8660_footswitch,
				     msm8660_num_footswitch);

	platform_add_devices(pyramid_devices,
			     ARRAY_SIZE(pyramid_devices));

        pyramid_gpio_mpp_init();

        pyramid_init_fb();

#ifdef CONFIG_MSM_DSPS
		msm8x60_init_dsps();
#endif

#ifdef CONFIG_USB_EHCI_MSM_72K
	msm_add_host(0, &msm_usb_host_pdata);
#endif

	
	if (board_mfg_mode() != 6)
		pyramid_add_usb_devices();

	properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (properties_kobj) {
          rc = sysfs_create_group(properties_kobj, &pyramid_properties_attr_group);
	}

	platform_add_devices(asoc_devices,
			ARRAY_SIZE(asoc_devices));

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
        platform_device_register(&msm_gsbi1_qup_spi_device);
#endif
	pyramid_ts_cy8c_set_system_rev(system_rev);

	register_i2c_devices();

        platform_device_register(&smsc911x_device);

	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));

#ifdef CONFIG_SENSORS_MSM_ADC

#endif
#ifdef CONFIG_MSM8X60_AUDIO_1X
	spi_register_board_info(msm_spi_board_info, ARRAY_SIZE(msm_spi_board_info));
	gpio_tlmm_config(msm_spi_gpio[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(msm_spi_gpio[1], GPIO_CFG_ENABLE);
	gpio_tlmm_config(msm_spi_gpio[2], GPIO_CFG_ENABLE);
	gpio_tlmm_config(msm_spi_gpio[3], GPIO_CFG_ENABLE);
	msm_auxpcm_init(); 
	msm_snddev_init();
	pyramid_audio_init(); 
#endif

        pyramid_init_keypad();
        pyramid_wifi_init();
        headset_device_register();

        printk(KERN_ERR "%s: --\n", __func__);
}

int __init parse_tag_memsize(const struct tag *tags);

static void __init pyramid_fixup(struct tag *tags,
		char **cmdline, struct meminfo *mi)
{
	mem_size_mb = parse_tag_memsize((const struct tag *)tags);
	printk(KERN_DEBUG "%s: mem_size_mb=%u\n", __func__, mem_size_mb);

	engineerid = parse_tag_engineerid(tags);
	mi->nr_banks = 1;
	mi->bank[0].start = PHY_BASE_ADDR1;
	mi->bank[0].size = SIZE_ADDR1;
}

MACHINE_START(PYRAMID, "pyramid")	
        .fixup = pyramid_fixup,
	.map_io = pyramid_map_io,
	.reserve = pyramid_reserve,
	.init_irq = msm8x60_init_irq,
	.handle_irq = gic_handle_irq,
	.init_machine = pyramid_init,
	.timer = &msm_timer,
	.init_early = msm8x60_allocate_memory_regions,
	.init_very_early = pyramid_early_memory,
	.restart = msm_restart,
MACHINE_END

