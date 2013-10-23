/* Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
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

#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/bootmem.h>
#include <linux/io.h>
#ifdef CONFIG_SPI_QSD
#include <linux/spi/spi.h>
#endif
#include <linux/msm_ssbi.h>
#include <linux/mfd/pmic8058.h>
#include <linux/leds.h>
#include <linux/mfd/marimba.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/smsc911x.h>
#include <linux/ofn_atlab.h>
#include <linux/power_supply.h>
#include <linux/i2c/isa1200.h>
#include <linux/i2c/tsc2007.h>
#include <linux/leds-pmic8058.h>
#include <linux/input/cy8c_ts.h>
#include <linux/msm_adc.h>
#include <linux/dma-mapping.h>
#include <linux/regulator/consumer.h>
#include <mach/atmega_microp.h>
#include <linux/akm8975.h>
#include <linux/bma150.h>
#include <linux/capella_cm3602.h>
#include <linux/atmel_qt602240.h>
#include <mach/htc_battery.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>

#include <mach/mpp.h>
#include <mach/board.h>
#include <mach/camera.h>
#include <mach/memory.h>
#include <mach/msm_iomap.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#include <mach/msm_spi.h>
#include <mach/qdsp5v2/msm_lpa.h>
#include <mach/dma.h>
#include <linux/android_pmem.h>
#include <linux/input/msm_ts.h>
#include <mach/pmic.h>
#include <mach/rpc_pmapp.h>
#include <mach/qdsp5v2/aux_pcm.h>
#include <mach/qdsp5v2/mi2s.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>
#include <mach/rpc_server_handset.h>
#include <mach/msm_tsif.h>
#include <mach/socinfo.h>
#include <mach/msm_memtypes.h>

#include <asm/mach/mmc.h>
#include <asm/mach/flash.h>
#include <mach/vreg.h>
#include <linux/platform_data/qcom_crypto_device.h>
#include <mach/system.h>
#ifdef CONFIG_BT
#include <mach/htc_bdaddress.h>
#endif

#include "devices.h"
#include "timer.h"
#ifdef CONFIG_USB_G_ANDROID
#include <linux/usb/android.h>
#include <mach/usbdiag.h>
#endif
#include "pm.h"
#include "pm-boot.h"
#include "spm.h"
#include "acpuclock.h"
#include "clock.h"
#include <mach/dal_axi.h>
#include <mach/msm_serial_hs.h>
#include <mach/qdsp5v2/mi2s.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>
#include <mach/sdio_al.h>
#include "smd_private.h"
#include <linux/bma150.h>

#include "board-msm7x30-regulator.h"
#include "pm.h"
#include <mach/board_htc.h>
#include "board-vision.h"
#include <mach/htc_restart_handler.h>
#define AUTORESTART 15000
#ifdef AUTORESTART
#include <linux/workqueue.h>
#include <mach/proc_comm.h>
static void my_work_handler(struct work_struct *w);

static struct workqueue_struct *wq = 0;
static DECLARE_DELAYED_WORK(my_work, my_work_handler);
static unsigned long delay;

static void
my_work_handler(struct work_struct *w)
{
  uint32_t restart_reason = 0x6f656d99;//RESTART_REASON_RECOVERY;
  msm_proc_comm(PCOM_RESET_CHIP_IMM, &restart_reason, 0);
  //msm_restart(0, "");
}
#endif

static struct platform_device ion_dev;

#define MSM_PMEM_SF_SIZE	0x01E00000

#define MSM_PMEM_ADSP_SIZE         0x02000000
#define MSM_PMEM_AUDIO_SIZE        0x00000000

#define MSM_ION_SF_SIZE		MSM_PMEM_SF_SIZE
#define MSM_ION_CAMERA_SIZE     MSM_PMEM_ADSP_SIZE
#define MSM_ION_MM_FW_SIZE	0x200000 /* (2MB) */
#define MSM_ION_MM_SIZE		0x3600000 /* (54MB) Must be a multiple of 64K */
#define MSM_ION_MFC_SIZE	SZ_8K
#define MSM_ION_WB_SIZE		0x600000 /* 6MB */
#define MSM_ION_QSECOM_SIZE	0x600000 /* (6MB) */
#define PMEM_KERNEL_EBI0_SIZE   0x600000
#define MSM_ION_AUDIO_SIZE	(MSM_PMEM_AUDIO_SIZE + PMEM_KERNEL_EBI0_SIZE)

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define MSM_ION_HEAP_NUM	5
#else
#define MSM_ION_HEAP_NUM	1
#endif

#define PMIC_GPIO_INT		27
#define PMIC_VREG_WLAN_LEVEL	2900
#define PMIC_GPIO_SD_DET	36
#define PMIC_GPIO_SDC4_EN_N	17  /* PMIC GPIO Number 18 */
#define PMIC_GPIO_HDMI_5V_EN_V3 32  /* PMIC GPIO for V3 H/W */
#define PMIC_GPIO_HDMI_5V_EN_V2 39 /* PMIC GPIO for V2 H/W */

#define ADV7520_I2C_ADDR	0x39

#define FPGA_SDCC_STATUS       0x8E0001A8

#define PMIC_GPIO_SDC4_PWR_EN_N 24  /* PMIC GPIO Number 25 */

/* Macros assume PMIC GPIOs start at 0 */
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)     (pm_gpio + NR_GPIO_IRQS)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)    (sys_gpio - NR_GPIO_IRQS)
#define PM8058_MPP_BASE			   PM8058_GPIO_PM_TO_SYS(PM8058_GPIOS)
#define PM8058_MPP_PM_TO_SYS(pm_gpio)	   (pm_gpio + PM8058_MPP_BASE)

#define PMIC_GPIO_FLASH_BOOST_ENABLE	15	/* PMIC GPIO Number 16 */
#define PMIC_GPIO_HAP_ENABLE   16  /* PMIC GPIO Number 17 */

#define PMIC_GPIO_WLAN_EXT_POR  22 /* PMIC GPIO NUMBER 23 */

#define BMA150_GPIO_INT 1

#define HAP_LVL_SHFT_MSM_GPIO 24

#define DDR1_BANK_BASE 0X20000000
#define DDR2_BANK_BASE 0X40000000

static unsigned int phys_add = DDR2_BANK_BASE;
unsigned long ebi1_phys_offset = DDR2_BANK_BASE;
EXPORT_SYMBOL(ebi1_phys_offset);

static unsigned pmem_sf_size = MSM_PMEM_SF_SIZE;
static int __init pmem_sf_size_setup(char *p)
{
	pmem_sf_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_sf_size", pmem_sf_size_setup);

static unsigned fb_size;
static int __init fb_size_setup(char *p)
{
	fb_size = memparse(p, NULL);
	return 0;
}
early_param("fb_size", fb_size_setup);

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

static unsigned pmem_kernel_ebi0_size = PMEM_KERNEL_EBI0_SIZE;
static int __init pmem_kernel_ebi0_size_setup(char *p)
{
	pmem_kernel_ebi0_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi0_size", pmem_kernel_ebi0_size_setup);

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
#endif

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

static struct htc_battery_platform_data htc_battery_pdev_data = {
	.guage_driver = GUAGE_MODEM,
	.charger = SWITCH_CHARGER_TPS65200,
	.m2a_cable_detect = 1,
};

static struct platform_device htc_battery_pdev = {
	.name = "htc_battery",
	.id   = -1,
	.dev  = {
		.platform_data = &htc_battery_pdev_data,
	},
};

struct pm8xxx_gpio_init_info {
	unsigned			gpio;
	struct pm_gpio			config;
};

static int vision_ts_atmel_power(int on)
{
	pr_info("%s: power %d\n", __func__, on);

	if (on == 1) {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(VISION_TP_RSTz), 1);
	} else if (on == 2) {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(VISION_TP_RSTz), 0);
		msleep(5);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(VISION_TP_RSTz), 1);
		msleep(40);
	}

	return 0;
}

struct atmel_i2c_platform_data vision_ts_atmel_data[] = {
	{
		.version = 0x0020,
		.source = 1,
		.abs_x_min = 1,
		.abs_x_max = 1023,
		.abs_y_min = 2,
		.abs_y_max = 966,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = VISION_GPIO_TP_ATT_N,
		.power = vision_ts_atmel_power,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {8, 0, 5, 5, 0, 0, 5, 40, 4, 170},
		.config_T9 = {139, 0, 0, 18, 12, 0, 16, 45, 3, 1, 10, 10, 5, 15, 4, 10, 20, 0, 0, 0, 0, 0, 6, 0, 20, 52, 150, 48, 146, 82, 20, 12},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T19 = {0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T22 = {15, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 7, 18, 255, 255, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {3, 0, 200, 50, 64, 31, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T27 = {0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 2, 8, 28, 60},
		.object_crc = {0x6D, 0xDB, 0x95},
                .cable_config = {45, 16, 8, 16},
		.GCAF_level = {20, 24, 28, 40, 63},
	},
	{
		.version = 0x0020,
		.source = 0,
		.abs_x_min = 1,
		.abs_x_max = 1023,
		.abs_y_min = 2,
		.abs_y_max = 966,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = VISION_GPIO_TP_ATT_N,
		.power = vision_ts_atmel_power,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {8, 0, 5, 5, 0, 0, 5, 40, 4, 170},
		.config_T9 = {139, 0, 0, 18, 12, 0, 16, 45, 3, 5, 10, 10, 5, 15, 4, 10, 20, 0, 0, 0, 0, 0, 0, 0, 0, 30, 152, 48, 146, 82, 20, 12},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T19 = {0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T22 = {15, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 7, 18, 255, 255, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {3, 0, 200, 50, 64, 31, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T27 = {0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 2, 8, 28, 60},
		.object_crc = {0xA8, 0x85, 0x00},
		.cable_config = {45, 16, 8, 16},
		.GCAF_level = {20, 24, 28, 40, 63},
	},
	{
		.version = 0x0016,
		.source = 1,
		.abs_x_min = 1,
		.abs_x_max = 1023,
		.abs_y_min = 2,
		.abs_y_max = 966,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = VISION_GPIO_TP_ATT_N,
		.power = vision_ts_atmel_power,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {8, 0, 10, 10, 0, 0, 10, 15},
		.config_T9 = {139, 0, 0, 18, 12, 0, 16, 30, 3, 1, 10, 10, 5, 15, 4, 10, 20, 0, 0, 0, 0, 0, 6, 0, 20, 52, 150, 48, 146, 82, 40},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T19 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {7, 0, 0, 0, 0, 0, 0, 40, 20, 4, 15, 0},
		.config_T22 = {15, 0, 0, 0, 0, 0, 0, 0, 16, 0, 1, 0, 7, 18, 255, 255, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {3, 0, 200, 50, 64, 31, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T27 = {0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 2, 4, 8, 60},
		.object_crc = {0x75, 0x0F, 0xF7},
		.cable_config = {30, 30, 8, 16},
		.GCAF_level = {20, 24, 28, 40, 63},
	},
	{
		.version = 0x016,
		.source = 0,
		.abs_x_min = 1,
		.abs_x_max = 1023,
		.abs_y_min = 2,
		.abs_y_max = 966,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = VISION_GPIO_TP_ATT_N,
		.power = vision_ts_atmel_power,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {8, 0, 10, 10, 0, 0, 10, 15},
		.config_T9 = {139, 0, 0, 18, 12, 0, 16, 30, 3, 5, 10, 10, 5, 15, 4, 10, 20, 0, 0, 0, 0, 0, 6, 0, 20, 52, 150, 48, 146, 82, 40},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T19 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {7, 0, 0, 0, 0, 0, 0, 40, 20, 4, 15, 0},
		.config_T22 = {15, 0, 0, 0, 0, 0, 0, 0, 16, 0, 1, 0, 7, 18, 255, 255, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {3, 0, 200, 50, 64, 31, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T27 = {0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 2, 4, 8, 60},
		.cable_config = {30, 30, 8, 16},
		.GCAF_level = {20, 24, 28, 40, 63},
	},
	{
		.version = 0x0015,
		.source = 1,
		.abs_x_min = 1,
		.abs_x_max = 1023,
		.abs_y_min = 2,
		.abs_y_max = 966,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = VISION_GPIO_TP_ATT_N,
		.power = vision_ts_atmel_power,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {10, 0, 20, 10, 0, 0, 5, 0},
		.config_T9 = {139, 0, 0, 18, 12, 0, 16, 45, 2, 1, 0, 5, 2, 15, 2, 10, 25, 5, 0, 0, 0, 0, 0, 0, 0, 0, 159, 47, 149, 81},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T19 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {7, 0, 0, 0, 0, 0, 0, 40, 20, 4, 15, 0},
		.config_T22 = {7, 0, 0, 0, 0, 0, 0, 0, 20, 0, 1, 10, 15, 20, 25, 30, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {3, 0, 200, 50, 64, 31, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T27 = {0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 2, 4, 8, 60},
	},
	{
		.version = 0x015,
		.source = 0,
		.abs_x_min = 1,
		.abs_x_max = 1023,
		.abs_y_min = 2,
		.abs_y_max = 966,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = VISION_GPIO_TP_ATT_N,
		.power = vision_ts_atmel_power,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {10, 0, 20, 10, 0, 0, 5, 0},
		.config_T9 = {139, 0, 0, 18, 12, 0, 16, 45, 2, 5, 0, 5, 2, 15, 2, 10, 25, 5, 0, 0, 0, 0, 0, 0, 0, 0, 159, 47, 149, 81},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T19 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {7, 0, 0, 0, 0, 0, 0, 40, 20, 4, 15, 0},
		.config_T22 = {7, 0, 0, 0, 0, 0, 0, 0, 20, 0, 1, 10, 15, 20, 25, 30, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {3, 0, 200, 50, 64, 31, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T27 = {0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 2, 4, 8, 60},
	},
};

static ssize_t vision_atmel_virtual_keys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf,
               __stringify(EV_KEY) ":" __stringify(KEY_HOME)       ":47:830:74:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU)   ":155:830:80:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_BACK)   ":337:830:90:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":434:830:60:50"
		"\n");
}

static struct kobj_attribute vision_atmel_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.atmel-touchscreen",
		.mode = S_IRUGO,
	},
	.show = &vision_atmel_virtual_keys_show,
};

static struct attribute *vision_properties_attrs[] = {
	&vision_atmel_virtual_keys_attr.attr,
	NULL
};

static struct attribute_group vision_properties_attr_group = {
	.attrs = vision_properties_attrs,
};


static int __capella_cm3602_power(int on)
{
	int rc;
	struct vreg *vreg = vreg_get(0, "gp7");
	if (!vreg) {
		printk(KERN_ERR "%s: vreg error\n", __func__);
		return -EIO;
	}
	rc = vreg_set_level(vreg, 2850);

	printk(KERN_DEBUG "%s: Turn the capella_cm3602 power %s\n",
		__func__, (on) ? "on" : "off");

	if (on) {
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(
				VISION_GPIO_PROXIMITY_EN), 1);
		rc = vreg_enable(vreg);
		if (rc < 0)
			printk(KERN_ERR "%s: vreg enable failed\n", __func__);
	} else {
		rc = vreg_disable(vreg);
		if (rc < 0)
			printk(KERN_ERR "%s: vreg disable failed\n", __func__);
		gpio_set_value(PM8058_GPIO_PM_TO_SYS(
				VISION_GPIO_PROXIMITY_EN), 0);
	}

	return rc;
	return 0;
}

static DEFINE_MUTEX(capella_cm3602_lock);
static int als_power_control;


static int capella_cm3602_power(int pwr_device, uint8_t enable)
{
	unsigned int old_status = 0;
	int ret = 0, on = 0;
	mutex_lock(&capella_cm3602_lock);

	old_status = als_power_control;
	if (enable)
		als_power_control |= pwr_device;
	else
		als_power_control &= ~pwr_device;

	on = als_power_control ? 1 : 0;
	if (old_status == 0 && on)
		ret = __capella_cm3602_power(1);
	else if (!on)
		ret = __capella_cm3602_power(0);

	mutex_unlock(&capella_cm3602_lock);
	return ret;
}

static struct capella_cm3602_platform_data capella_cm3602_pdata = {
	.power = capella_cm3602_power,
	.p_en = PM8058_GPIO_PM_TO_SYS(VISION_GPIO_PROXIMITY_EN),
	.p_out = VISION_GPIO_PROXIMITY_INT_N,
	.irq = MSM_GPIO_TO_INT(VISION_GPIO_PROXIMITY_INT_N),
};

static struct platform_device capella_cm3602 = {
	.name = CAPELLA_CM3602,
	.id = -1,
	.dev = {
		.platform_data = &capella_cm3602_pdata
	}
};

static struct microp_function_config microp_functions[] = {
	{
		.name   = "microp_intrrupt",
		.category = MICROP_FUNCTION_INTR,
	},
	{
		.name   = "reset-int",
		.category = MICROP_FUNCTION_RESET_INT,
		.int_pin = 1 << 8,
	},
};

static struct microp_led_config up_led_config[] = {
	{
		.name = "amber",
		.type = LED_RGB,
	},
	{
		.name = "green",
		.type = LED_RGB,
	},
	{
		.name = "button-backlight",
		.type = LED_PWM,
		.led_pin = 1 << 0,
		.init_value = 0,
		.fade_time = 5,
	},
	{
		.name = "jogball-backlight",
		.type = LED_JOGBALL,
	},
};

static struct microp_function_config microp_lightsensor_function = {
	.name = "light_sensor",
	.category = MICROP_FUNCTION_LSENSOR,
	.levels = { 0x1, 0x3, 0x5, 0x13, 0x1A, 0x45, 0xDB, 0x135, 0x1F2, 0x3FF },
	.channel = 3,
	.int_pin = 1 << 9,
	.golden_adc = 0xC0,
        .ls_power = capella_cm3602_power,
};

static struct lightsensor_platform_data lightsensor_data = {
	.config = &microp_lightsensor_function,
	.irq = MSM_uP_TO_INT(9),
};

static struct microp_led_platform_data microp_leds_data = {
	.num_leds	= ARRAY_SIZE(up_led_config),
	.led_config	= up_led_config,
};

//static struct bma150_platform_data bravo_g_sensor_pdata = {
//	.microp_new_cmd = 1,
//	.chip_layout = 1,
//};

static struct platform_device microp_devices[] = {
	{
		.name = "lightsensor_microp",
		.dev = {
			.platform_data = &lightsensor_data,
		},
	},
	{
		.name = "leds-microp",
		.id = -1,
		.dev = {
			.platform_data = &microp_leds_data,
		},
	},
        //        {
        //                .name = BMA150_G_SENSOR_NAME,
        //                .dev = {
        //                        .platform_data = &bravo_g_sensor_pdata,
        //                },
        //        },
        //	{
        //		.name	= "HTC_HEADSET_MGR",
        //		.id	= -1,
        //		.dev	= {
        //			.platform_data	= &htc_headset_mgr_data,
        //		},
        //	},
};

static struct microp_i2c_platform_data microp_data = {
	.num_functions   = ARRAY_SIZE(microp_functions),
	.microp_function = microp_functions,
	.num_devices = ARRAY_SIZE(microp_devices),
	.microp_devices = microp_devices,
	.gpio_reset = VISION_GPIO_UP_RESET_N,
	.spi_devices = SPI_GSENSOR,
};

static struct akm8975_platform_data compass_platform_data = {
	.layouts = VISION_LAYOUTS,
};

static struct i2c_board_info i2c_devices[] = {
	{
		I2C_BOARD_INFO(ATMEL_QT602240_NAME, 0x94 >> 1),
		.platform_data = &vision_ts_atmel_data,
		.irq = MSM_GPIO_TO_INT(VISION_GPIO_TP_ATT_N)
	},
	{
		I2C_BOARD_INFO(MICROP_I2C_NAME, 0xCC >> 1),
		.platform_data = &microp_data,
		.irq = MSM_GPIO_TO_INT(VISION_GPIO_UP_INT_N)
	},
        //	{
        //		I2C_BOARD_INFO("audience_a1026", 0x3E),
        //		.platform_data = &a1026_data,
        //	},
};

static struct i2c_board_info i2c_compass_devices1[] = {
	{
		I2C_BOARD_INFO(AKM8975_I2C_NAME, 0x18 >> 1),
		.platform_data = &compass_platform_data,
		.irq = MSM_GPIO_TO_INT(VISION_GPIO_COMPASS_INT),
	},
};

static struct i2c_board_info i2c_compass_devices2[] = {
	{
		I2C_BOARD_INFO(AKM8975_I2C_NAME, 0x1A >> 1),
		.platform_data = &compass_platform_data,
		.irq = MSM_GPIO_TO_INT(VISION_GPIO_COMPASS_INT),
	},
};

static int pm8058_gpios_init(void)
{
	int rc;
	static struct pm_gpio tp_rstz = {
		.direction      = PM_GPIO_DIR_OUT,
		.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
		.output_value   = 1,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM8058_GPIO_VIN_L5,
		.out_strength   = PM_GPIO_STRENGTH_HIGH,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio vol_up = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_UP_31P5,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio vol_dn = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_UP_31P5,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio sliding_intz = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_UP_31P5,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio oj_act = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_UP_31P5,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio cam_step1 = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_UP_31P5,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio cam_step2 = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_UP_31P5,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio headset = {
		.direction      = PM_GPIO_DIR_IN,
		.output_buffer  = 0,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = 0,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	static struct pm_gpio ps_en = {
		.direction      = PM_GPIO_DIR_OUT,
		.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM8058_GPIO_VIN_L5,
		.out_strength   = PM_GPIO_STRENGTH_HIGH,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_TP_RSTz), &tp_rstz);
	if (rc) {
		printk(KERN_ERR "%s TP_RSTz config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s TP_RSTz config ok\n", __func__);

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_VOL_UP), &vol_up);
	if (rc) {
		printk(KERN_ERR "%s VOL_UP config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s VOL_UP config ok\n", __func__);
	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_VOL_DN), &vol_dn);
	if (rc) {
		printk(KERN_ERR "%s VOL_DN config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s VOL_DN config ok\n", __func__);

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_SLIDING_INTz), &sliding_intz);
	if (rc) {
		printk(KERN_ERR "%s SLIDING_INTz config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s SLIDING_INTz config ok\n", __func__);

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_OJ_ACTION), &oj_act);
	if (rc) {
		printk(KERN_ERR "%s OJ_ACTION config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s OJ_ACTION config ok\n", __func__);

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_CAM_STEP1), &cam_step1);
	if (rc) {
		printk(KERN_ERR "%s CAM_STEP1 config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s CAM_STEP1 config ok\n", __func__);

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_CAM_STEP2), &cam_step2);
	if (rc) {
		printk(KERN_ERR "%s CAM_STEP2 config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s CAM_STEP2 config ok\n", __func__);

	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_AUD_HP_DETz), &headset);
	if (rc) {
		printk(KERN_ERR "%s AUD_HP_DETz config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s AUD_HP_DETz config ok\n", __func__);


	rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(VISION_GPIO_PROXIMITY_EN), &ps_en);
	if (rc) {
		printk(KERN_ERR "%s PROXIMITY_EN config failed\n", __func__);
		return rc;
	} else
	  printk(KERN_ERR "%s PROXIMITY_EN config ok\n", __func__);

	return 0;
#if 0
	int rc;

	struct pm8xxx_gpio_init_info sdc4_en = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC4_EN_N),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.pull           = PM_GPIO_PULL_NO,
			.vin_sel        = PM8058_GPIO_VIN_L5,
			.function       = PM_GPIO_FUNC_NORMAL,
			.inv_int_pol    = 0,
			.out_strength   = PM_GPIO_STRENGTH_LOW,
			.output_value   = 0,
		},
	};

	struct pm8xxx_gpio_init_info sdc4_pwr_en = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC4_PWR_EN_N),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.pull           = PM_GPIO_PULL_NO,
			.vin_sel        = PM8058_GPIO_VIN_L5,
			.function       = PM_GPIO_FUNC_NORMAL,
			.inv_int_pol    = 0,
			.out_strength   = PM_GPIO_STRENGTH_LOW,
			.output_value   = 0,
		},
	};

	struct pm8xxx_gpio_init_info haptics_enable = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_HAP_ENABLE),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.pull           = PM_GPIO_PULL_NO,
			.out_strength   = PM_GPIO_STRENGTH_HIGH,
			.function       = PM_GPIO_FUNC_NORMAL,
			.inv_int_pol    = 0,
			.vin_sel        = 2,
			.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
			.output_value   = 0,
		},
	};

	struct pm8xxx_gpio_init_info hdmi_5V_en = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_HDMI_5V_EN_V3),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.pull           = PM_GPIO_PULL_NO,
			.vin_sel        = PM8058_GPIO_VIN_VPH,
			.function       = PM_GPIO_FUNC_NORMAL,
			.out_strength   = PM_GPIO_STRENGTH_LOW,
			.output_value   = 0,
		},
	};

	struct pm8xxx_gpio_init_info flash_boost_enable = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_FLASH_BOOST_ENABLE),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
			.output_value   = 0,
			.pull           = PM_GPIO_PULL_NO,
			.vin_sel        = PM8058_GPIO_VIN_S3,
			.out_strength   = PM_GPIO_STRENGTH_HIGH,
			.function        = PM_GPIO_FUNC_2,
		},
	};

	struct pm8xxx_gpio_init_info gpio23 = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_WLAN_EXT_POR),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
			.output_value   = 0,
			.pull           = PM_GPIO_PULL_NO,
			.vin_sel        = 2,
			.out_strength   = PM_GPIO_STRENGTH_LOW,
			.function       = PM_GPIO_FUNC_NORMAL,
		}
	};

	struct pm8xxx_gpio_init_info sdcc_det = {
		PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SD_DET - 1),
		{
			.direction      = PM_GPIO_DIR_IN,
			.pull           = PM_GPIO_PULL_UP_1P5,
			.vin_sel        = 2,
			.function       = PM_GPIO_FUNC_NORMAL,
			.inv_int_pol    = 0,
		},
	};

	if (machine_is_msm7x30_fluid())
		sdcc_det.config.inv_int_pol = 1;

	rc = pm8xxx_gpio_config(sdcc_det.gpio, &sdcc_det.config);
	if (rc) {
		pr_err("%s PMIC_GPIO_SD_DET config failed\n", __func__);
		return rc;
	}

	if (machine_is_msm8x55_svlte_surf() || machine_is_msm8x55_svlte_ffa() ||
						machine_is_msm7x30_fluid())
		hdmi_5V_en.gpio = PMIC_GPIO_HDMI_5V_EN_V2;
	else
		hdmi_5V_en.gpio = PMIC_GPIO_HDMI_5V_EN_V3;

	hdmi_5V_en.gpio = PM8058_GPIO_PM_TO_SYS(hdmi_5V_en.gpio);

	rc = pm8xxx_gpio_config(hdmi_5V_en.gpio, &hdmi_5V_en.config);
	if (rc) {
		pr_err("%s PMIC_GPIO_HDMI_5V_EN config failed\n", __func__);
		return rc;
	}

	/* Deassert GPIO#23 (source for Ext_POR on WLAN-Volans) */
	rc = pm8xxx_gpio_config(gpio23.gpio, &gpio23.config);
	if (rc) {
		pr_err("%s PMIC_GPIO_WLAN_EXT_POR config failed\n", __func__);
		return rc;
	}

	if (machine_is_msm7x30_fluid()) {
		/* Haptics gpio */
		rc = pm8xxx_gpio_config(haptics_enable.gpio,
						&haptics_enable.config);
		if (rc) {
			pr_err("%s: PMIC GPIO %d write failed\n", __func__,
							haptics_enable.gpio);
			return rc;
		}
		/* Flash boost gpio */
		rc = pm8xxx_gpio_config(flash_boost_enable.gpio,
						&flash_boost_enable.config);
		if (rc) {
			pr_err("%s: PMIC GPIO %d write failed\n", __func__,
						flash_boost_enable.gpio);
			return rc;
		}
		/* SCD4 gpio */
		rc = pm8xxx_gpio_config(sdc4_en.gpio, &sdc4_en.config);
		if (rc) {
			pr_err("%s PMIC_GPIO_SDC4_EN_N config failed\n",
								 __func__);
			return rc;
		}
		rc = gpio_request(sdc4_en.gpio, "sdc4_en");
		if (rc) {
			pr_err("%s PMIC_GPIO_SDC4_EN_N gpio_request failed\n",
				__func__);
			return rc;
		}
		gpio_set_value_cansleep(sdc4_en.gpio, 0);
	}
	/* FFA -> gpio_25 controls vdd of sdcc4 */
	else {
		/* SCD4 gpio_25 */
		rc = pm8xxx_gpio_config(sdc4_pwr_en.gpio, &sdc4_pwr_en.config);
		if (rc) {
			pr_err("%s PMIC_GPIO_SDC4_PWR_EN_N config failed: %d\n",
			       __func__, rc);
			return rc;
		}

		rc = gpio_request(sdc4_pwr_en.gpio, "sdc4_pwr_en");
		if (rc) {
			pr_err("PMIC_GPIO_SDC4_PWR_EN_N gpio_req failed: %d\n",
			       rc);
			return rc;
		}
	}

	return 0;
#endif
}

/* Regulator API support */

#ifdef CONFIG_MSM_PROC_COMM_REGULATOR
static struct platform_device msm_proccomm_regulator_dev = {
	.name = PROCCOMM_REGULATOR_DEV_NAME,
	.id   = -1,
	.dev  = {
		.platform_data = &msm7x30_proccomm_regulator_data
	}
};
#endif

static int pm8058_pwm_config(struct pwm_device *pwm, int ch, int on)
{
	struct pm_gpio pwm_gpio_config = {
		.direction      = PM_GPIO_DIR_OUT,
		.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.out_strength   = PM_GPIO_STRENGTH_HIGH,
		.function       = PM_GPIO_FUNC_2,
	};
	int	rc = -EINVAL;
	int	id, mode, max_mA;

	id = mode = max_mA = 0;
	switch (ch) {
	case 0:
	case 1:
	case 2:
		if (on) {
			id = 24 + ch;
			rc = pm8xxx_gpio_config(PM8058_GPIO_PM_TO_SYS(id - 1),
							&pwm_gpio_config);
			if (rc)
				pr_err("%s: pm8xxx_gpio_config(%d): rc=%d\n",
				       __func__, id, rc);
		}
		break;

	case 3:
		id = PM_PWM_LED_KPD;
		mode = PM_PWM_CONF_DTEST3;
		max_mA = 200;
		break;

	case 4:
		id = PM_PWM_LED_0;
		mode = PM_PWM_CONF_PWM1;
		max_mA = 40;
		break;

	case 5:
		id = PM_PWM_LED_2;
		mode = PM_PWM_CONF_PWM2;
		max_mA = 40;
		break;

	case 6:
		id = PM_PWM_LED_FLASH;
		mode = PM_PWM_CONF_DTEST3;
		max_mA = 200;
		break;

	default:
		break;
	}

	if (ch >= 3 && ch <= 6) {
		if (!on) {
			mode = PM_PWM_CONF_NONE;
			max_mA = 0;
		}
		rc = pm8058_pwm_config_led(pwm, id, mode, max_mA);
		if (rc)
			pr_err("%s: pm8058_pwm_config_led(ch=%d): rc=%d\n",
			       __func__, ch, rc);
	}

	return rc;
}

static int pm8058_pwm_enable(struct pwm_device *pwm, int ch, int on)
{
	int	rc;

	switch (ch) {
	case 7:
		rc = pm8058_pwm_set_dtest(pwm, on);
		if (rc)
			pr_err("%s: pwm_set_dtest(%d): rc=%d\n",
			       __func__, on, rc);
		break;
	default:
		rc = -EINVAL;
		break;
	}
	return rc;
}

static struct pm8058_pwm_pdata pm8xxx_pwm_data = {
	.config         = pm8058_pwm_config,
	.enable         = pm8058_pwm_enable,
};

static struct pm8xxx_irq_platform_data pm8xxx_irq_pdata = {
	.irq_base		= PMIC8058_IRQ_BASE,
	.devirq			= MSM_GPIO_TO_INT(PMIC_GPIO_INT),
	.irq_trigger_flag       = IRQF_TRIGGER_LOW,
};

static struct pm8xxx_gpio_platform_data pm8xxx_gpio_pdata = {
	.gpio_base		= PM8058_GPIO_PM_TO_SYS(0),
};

static struct pm8xxx_mpp_platform_data pm8xxx_mpp_pdata = {
	.mpp_base	= PM8058_MPP_PM_TO_SYS(0),
};

static struct pm8058_platform_data pm8058_7x30_data = {
	.irq_pdata		= &pm8xxx_irq_pdata,
	.gpio_pdata		= &pm8xxx_gpio_pdata,
	.mpp_pdata		= &pm8xxx_mpp_pdata,
        .pwm_pdata		= &pm8xxx_pwm_data,
};

#ifdef CONFIG_MSM_SSBI
static struct msm_ssbi_platform_data msm7x30_ssbi_pm8058_pdata = {
	.rsl_id = "D:PMIC_SSBI",
	.controller_type = MSM_SBI_CTRL_SSBI2,
	.slave	= {
		.name			= "pm8058-core",
		.platform_data		= &pm8058_7x30_data,
	},
};
#endif

#ifdef CONFIG_MSM_GEMINI
static struct resource msm_gemini_resources[] = {
	{
		.start  = 0xA3A00000,
		.end    = 0xA3A00000 + 0x0150 - 1,
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

static int __init buses_init(void)
{
	if (gpio_tlmm_config(GPIO_CFG(PMIC_GPIO_INT, 1, GPIO_CFG_INPUT,
				  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE))
		pr_err("%s: gpio_tlmm_config (gpio=%d) failed\n",
		       __func__, PMIC_GPIO_INT);

	return 0;
}

#ifdef CONFIG_USB_G_ANDROID
static struct android_usb_platform_data android_usb_pdata = {
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};
#endif

static struct msm_pm_platform_data msm_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
		.latency = 8594,
		.residency = 23740,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
		.latency = 4594,
		.residency = 23740,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
#ifdef CONFIG_MSM_STANDALONE_POWER_COLLAPSE
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 0,
#else /*CONFIG_MSM_STANDALONE_POWER_COLLAPSE*/
		.idle_supported = 0,
		.suspend_supported = 0,
		.idle_enabled = 0,
		.suspend_enabled = 0,
#endif /*CONFIG_MSM_STANDALONE_POWER_COLLAPSE*/
		.latency = 500,
		.residency = 6000,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 1,
		.latency = 443,
		.residency = 1098,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
		.latency = 2,
		.residency = 0,
	},
};

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_RESET_VECTOR_VIRT,
	.v_addr = (uint32_t *)PAGE_OFFSET,
};

#ifdef CONFIG_SPI_QSD
static struct spi_board_info msm_spi_board_info[] __initdata = {
	{
		.modalias	= "spi_qsd",
		.mode		= SPI_MODE_3,
		.bus_num	= 0,
		.chip_select	= 2,
		.max_speed_hz	= 10000000,
	},
	{
		.modalias	= "spi_oj",
		.mode		= SPI_MODE_3,
		.bus_num	= 0,
		.chip_select	= 3,
		.max_speed_hz	= 512000,
	}
};
#endif

static void config_gpio_table(uint32_t *table, int len)
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
static uint32_t qsd_spi_gpio_on_table[] = {
  GPIO_CFG(45, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(47, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(48, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(87, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(89, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA)
};

static uint32_t qsd_spi_gpio_off_table[] = {
  GPIO_CFG(45, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(47, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(48, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(87, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
  GPIO_CFG(89, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA)
};

static int msm_qsd_spi_gpio_config(void)
{
	config_gpio_table(qsd_spi_gpio_on_table,
		ARRAY_SIZE(qsd_spi_gpio_on_table));
	gpio_set_value(VISION_OJ_RSTz, 1);
	return 0;
}

static void msm_qsd_spi_gpio_release(void)
{
	config_gpio_table(qsd_spi_gpio_off_table,
		ARRAY_SIZE(qsd_spi_gpio_off_table));
	gpio_set_value(VISION_OJ_RSTz, 0);
}

static struct msm_spi_platform_data qsd_spi_pdata = {
	.max_clock_speed = 26331429,
	.gpio_config  = msm_qsd_spi_gpio_config,
	.gpio_release = msm_qsd_spi_gpio_release,
};

static struct resource qsd_spi_resources[] = {
	{
		.name   = "spi_irq_in",
		.start	= INT_SPI_INPUT,
		.end	= INT_SPI_INPUT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name   = "spi_irq_out",
		.start	= INT_SPI_OUTPUT,
		.end	= INT_SPI_OUTPUT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name   = "spi_irq_err",
		.start	= INT_SPI_ERROR,
		.end	= INT_SPI_ERROR,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name   = "spi_base",
		.start	= 0xA8000000,
		.end	= 0xA8000000 + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name   = "spidm_channels",
		.flags  = IORESOURCE_DMA,
	},
	{
		.name   = "spidm_crci",
		.flags  = IORESOURCE_DMA,
	},
};

struct platform_device qsd_device_spi = {
	.name		= "spi_qsd",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qsd_spi_resources),
	.resource	= qsd_spi_resources,
};

static void __init msm_qsd_spi_init(void)
{
	qsd_device_spi.dev.platform_data = &qsd_spi_pdata;
}

#ifdef CONFIG_USB_EHCI_MSM_72K
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
        int rc;
        static int vbus_is_on;
	struct pm8xxx_gpio_init_info usb_vbus = {
		PM8058_GPIO_PM_TO_SYS(36),
		{
			.direction      = PM_GPIO_DIR_OUT,
			.pull           = PM_GPIO_PULL_NO,
			.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
			.output_value   = 1,
			.vin_sel        = 2,
			.out_strength   = PM_GPIO_STRENGTH_MED,
			.function       = PM_GPIO_FUNC_NORMAL,
			.inv_int_pol    = 0,
		},
	};

        /* If VBUS is already on (or off), do nothing. */
        if (unlikely(on == vbus_is_on))
                return;

        if (on) {
		rc = pm8xxx_gpio_config(usb_vbus.gpio, &usb_vbus.config);
		if (rc) {
                        pr_err("%s PMIC GPIO 36 write failed\n", __func__);
                        return;
                }
	} else {
		gpio_set_value_cansleep(PM8058_GPIO_PM_TO_SYS(36), 0);
	}

        vbus_is_on = on;
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
        .phy_info   = (USB_PHY_INTEGRATED | USB_PHY_MODEL_45NM),
        .vbus_power = msm_hsusb_vbus_power,
        .power_budget   = 180,
};
#endif

#ifndef CONFIG_USB_EHCI_MSM_72K
static int msm_hsusb_pmic_notif_init(void (*callback)(int online), int init);
#endif
static struct msm_otg_platform_data msm_otg_pdata = {
#ifdef CONFIG_USB_EHCI_MSM_72K
	.vbus_power = msm_hsusb_vbus_power,
#endif
	.pemp_level		= PRE_EMPHASIS_WITH_20_PERCENT,
	.cdr_autoreset		= CDR_AUTO_RESET_DISABLE,
	.drv_ampl		= HS_DRV_AMPLITUDE_DEFAULT,
	.se1_gating		= SE1_GATING_DISABLE,
};

#ifdef CONFIG_USB_GADGET
static int phy_init_seq[] = { 0x06, 0x36, 0x0C, 0x31, 0x31, 0x32, 0x1, 0x0D, 0x1, 0x10, -1 };
static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.phy_init_seq		= phy_init_seq,
	.is_phy_status_timer_on = 1,
};
#endif
#ifndef CONFIG_USB_EHCI_MSM_72K
typedef void (*notify_vbus_state) (int);
notify_vbus_state notify_vbus_state_func_ptr;
int vbus_on_irq;
static irqreturn_t pmic_vbus_on_irq(int irq, void *data)
{
	pr_info("%s: vbus notification from pmic\n", __func__);

	(*notify_vbus_state_func_ptr) (1);

	return IRQ_HANDLED;
}
static int msm_hsusb_pmic_notif_init(void (*callback)(int online), int init)
{
	int ret;

	if (init) {
		if (!callback)
			return -ENODEV;

		notify_vbus_state_func_ptr = callback;
		vbus_on_irq = platform_get_irq_byname(&msm_device_otg,
			"vbus_on");
		if (vbus_on_irq <= 0) {
			pr_err("%s: unable to get vbus on irq\n", __func__);
			return -ENODEV;
		}

		ret = request_any_context_irq(vbus_on_irq, pmic_vbus_on_irq,
			IRQF_TRIGGER_RISING, "msm_otg_vbus_on", NULL);
		if (ret < 0) {
			pr_info("%s: request_irq for vbus_on"
				"interrupt failed\n", __func__);
			return ret;
		}
		msm_otg_pdata.pmic_vbus_irq = vbus_on_irq;
		return 0;
	} else {
		free_irq(vbus_on_irq, 0);
		notify_vbus_state_func_ptr = NULL;
		return 0;
	}
}
#endif

static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
       .inject_rx_on_wakeup = 1,
       .rx_to_inject = 0xFD,
};

static struct platform_device msm_migrate_pages_device = {
	.name   = "msm_migrate_pages",
	.id     = -1,
};

#ifdef CONFIG_MSM_SDIO_AL
static struct msm_gpio mdm2ap_status = {
	GPIO_CFG(77, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	"mdm2ap_status"
};


static int configure_mdm2ap_status(int on)
{
	if (on)
		return msm_gpios_request_enable(&mdm2ap_status, 1);
	else {
		msm_gpios_disable_free(&mdm2ap_status, 1);
		return 0;
	}
}

static int get_mdm2ap_status(void)
{
	return gpio_get_value(GPIO_PIN(mdm2ap_status.gpio_cfg));
}

static struct sdio_al_platform_data sdio_al_pdata = {
	.config_mdm2ap_status = configure_mdm2ap_status,
	.get_mdm2ap_status = get_mdm2ap_status,
	.allow_sdioc_version_major_2 = 1,
	.peer_sdioc_version_minor = 0x0001,
	.peer_sdioc_version_major = 0x0003,
	.peer_sdioc_boot_version_minor = 0x0001,
	.peer_sdioc_boot_version_major = 0x0003,
};

struct platform_device msm_device_sdio_al = {
	.name = "msm_sdio_al",
	.id = -1,
	.dev		= {
		.platform_data	= &sdio_al_pdata,
	},
};

#endif /* CONFIG_MSM_SDIO_AL */

static struct msm_gpio msm_i2c_gpios_hw[] = {
	{ GPIO_CFG(70, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_scl" },
	{ GPIO_CFG(71, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_sda" },
};

static struct msm_gpio msm_i2c_gpios_io[] = {
	{ GPIO_CFG(70, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_scl" },
	{ GPIO_CFG(71, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_sda" },
};

static struct msm_gpio qup_i2c_gpios_io[] = {
	{ GPIO_CFG(16, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_scl" },
	{ GPIO_CFG(17, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_sda" },
};
static struct msm_gpio qup_i2c_gpios_hw[] = {
	{ GPIO_CFG(16, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_scl" },
	{ GPIO_CFG(17, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_sda" },
};

static void
msm_i2c_gpio_config(int adap_id, int config_type)
{
	struct msm_gpio *msm_i2c_table;

	/* Each adapter gets 2 lines from the table */
	if (adap_id > 0)
		return;
	if (config_type)
		msm_i2c_table = &msm_i2c_gpios_hw[adap_id*2];
	else
		msm_i2c_table = &msm_i2c_gpios_io[adap_id*2];
	msm_gpios_enable(msm_i2c_table, 2);
}
/*This needs to be enabled only for OEMS*/
#ifndef CONFIG_QUP_EXCLUSIVE_TO_CAMERA
static struct regulator *qup_vreg;
#endif
static void
qup_i2c_gpio_config(int adap_id, int config_type)
{
	int rc = 0;
	struct msm_gpio *qup_i2c_table;
	/* Each adapter gets 2 lines from the table */
	if (adap_id != 4)
		return;
	if (config_type)
		qup_i2c_table = qup_i2c_gpios_hw;
	else
		qup_i2c_table = qup_i2c_gpios_io;
	rc = msm_gpios_enable(qup_i2c_table, 2);
	if (rc < 0)
		printk(KERN_ERR "QUP GPIO enable failed: %d\n", rc);
	/*This needs to be enabled only for OEMS*/
#ifndef CONFIG_QUP_EXCLUSIVE_TO_CAMERA
	if (!IS_ERR_OR_NULL(qup_vreg)) {
		rc = regulator_enable(qup_vreg);
		if (rc) {
			pr_err("%s: regulator_enable failed: %d\n",
			__func__, rc);
		}
	}
#endif
}

static struct msm_i2c_platform_data msm_i2c_pdata = {
	.clk_freq = 100000,
	.pri_clk = 70,
	.pri_dat = 71,
	.rmutex  = 1,
        .rsl_id = "S:6",
	.msm_i2c_config_gpio = msm_i2c_gpio_config,
};

static void __init msm_device_i2c_init(void)
{
	if (msm_gpios_request(msm_i2c_gpios_hw, ARRAY_SIZE(msm_i2c_gpios_hw)))
		pr_err("failed to request I2C gpios\n");

	msm_device_i2c.dev.platform_data = &msm_i2c_pdata;
}

static struct msm_i2c_platform_data msm_i2c_2_pdata = {
	.clk_freq = 100000,
	.rmutex  = 1,
        .rsl_id = "S:7",
	.msm_i2c_config_gpio = msm_i2c_gpio_config,
};

static void __init msm_device_i2c_2_init(void)
{
	msm_device_i2c_2.dev.platform_data = &msm_i2c_2_pdata;
}

static struct msm_i2c_platform_data qup_i2c_pdata = {
	.clk_freq = 384000,
	.msm_i2c_config_gpio = qup_i2c_gpio_config,
};

static void __init qup_device_i2c_init(void)
{
	if (msm_gpios_request(qup_i2c_gpios_hw, ARRAY_SIZE(qup_i2c_gpios_hw)))
		pr_err("failed to request I2C gpios\n");

	qup_device_i2c.dev.platform_data = &qup_i2c_pdata;
	/*This needs to be enabled only for OEMS*/
#ifndef CONFIG_QUP_EXCLUSIVE_TO_CAMERA
	qup_vreg = regulator_get(&qup_device_i2c.dev, "lvsw1");
	if (IS_ERR(qup_vreg)) {
		dev_err(&qup_device_i2c.dev,
			"%s: regulator_get failed: %ld\n",
			__func__, PTR_ERR(qup_vreg));
	}
#endif
}

#ifdef CONFIG_I2C_SSBI
static struct msm_i2c_ssbi_platform_data msm_i2c_ssbi7_pdata = {
	.rsl_id = "D:CODEC_SSBI",
	.controller_type = MSM_SBI_CTRL_SSBI,
};
#endif

static void __init msm7x30_init_irq(void)
{
	msm_init_irq();
}

#if (defined(CONFIG_MMC_MSM_SDC1_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC2_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC3_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC4_SUPPORT))

struct sdcc_gpio {
	struct msm_gpio *cfg_data;
	uint32_t size;
	struct msm_gpio *sleep_cfg_data;
};
#if defined(CONFIG_MMC_MSM_SDC1_SUPPORT)
static struct msm_gpio sdc1_lvlshft_cfg_data[] = {
	{GPIO_CFG(35, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA), "sdc1_lvlshft"},
};
#endif
static struct msm_gpio sdc1_cfg_data[] = {
	{GPIO_CFG(38, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "sdc1_clk"},
	{GPIO_CFG(39, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_cmd"},
	{GPIO_CFG(40, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_3"},
	{GPIO_CFG(41, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_2"},
	{GPIO_CFG(42, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_1"},
	{GPIO_CFG(43, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_0"},
};

static struct msm_gpio sdc2_cfg_data[] = {
	{GPIO_CFG(64, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "sdc2_clk"},
	{GPIO_CFG(65, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_cmd"},
	{GPIO_CFG(66, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_3"},
	{GPIO_CFG(67, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_2"},
	{GPIO_CFG(68, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_1"},
	{GPIO_CFG(69, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_0"},

#ifdef CONFIG_MMC_MSM_SDC2_8_BIT_SUPPORT
	{GPIO_CFG(115, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_4"},
	{GPIO_CFG(114, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_5"},
	{GPIO_CFG(113, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_6"},
	{GPIO_CFG(112, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_7"},
#endif
};

static struct msm_gpio sdc3_cfg_data[] = {
	{GPIO_CFG(110, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "sdc3_clk"},
	{GPIO_CFG(111, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_cmd"},
	{GPIO_CFG(116, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_3"},
	{GPIO_CFG(117, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_2"},
	{GPIO_CFG(118, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_1"},
	{GPIO_CFG(119, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_0"},
};

static struct msm_gpio sdc3_sleep_cfg_data[] = {
	{GPIO_CFG(110, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			"sdc3_clk"},
	{GPIO_CFG(111, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			"sdc3_cmd"},
	{GPIO_CFG(116, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			"sdc3_dat_3"},
	{GPIO_CFG(117, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			"sdc3_dat_2"},
	{GPIO_CFG(118, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			"sdc3_dat_1"},
	{GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			"sdc3_dat_0"},
};

static struct msm_gpio sdc4_cfg_data[] = {
	{GPIO_CFG(58, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "sdc4_clk"},
	{GPIO_CFG(59, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_cmd"},
	{GPIO_CFG(60, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_3"},
	{GPIO_CFG(61, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_2"},
	{GPIO_CFG(62, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_1"},
	{GPIO_CFG(63, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_0"},
};

static struct sdcc_gpio sdcc_cfg_data[] = {
	{
		.cfg_data = sdc1_cfg_data,
		.size = ARRAY_SIZE(sdc1_cfg_data),
		.sleep_cfg_data = NULL,
	},
	{
		.cfg_data = sdc2_cfg_data,
		.size = ARRAY_SIZE(sdc2_cfg_data),
		.sleep_cfg_data = NULL,
	},
	{
		.cfg_data = sdc3_cfg_data,
		.size = ARRAY_SIZE(sdc3_cfg_data),
		.sleep_cfg_data = sdc3_sleep_cfg_data,
	},
	{
		.cfg_data = sdc4_cfg_data,
		.size = ARRAY_SIZE(sdc4_cfg_data),
		.sleep_cfg_data = NULL,
	},
};

static struct regulator *sdcc_vreg_data[ARRAY_SIZE(sdcc_cfg_data)];

static unsigned long vreg_sts, gpio_sts;

static uint32_t msm_sdcc_setup_gpio(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct sdcc_gpio *curr;

	curr = &sdcc_cfg_data[dev_id - 1];

	if (!(test_bit(dev_id, &gpio_sts)^enable))
		return rc;

	if (enable) {
		set_bit(dev_id, &gpio_sts);
		rc = msm_gpios_request_enable(curr->cfg_data, curr->size);
		if (rc)
			printk(KERN_ERR "%s: Failed to turn on GPIOs for slot %d\n",
				__func__,  dev_id);
	} else {
		clear_bit(dev_id, &gpio_sts);
		if (curr->sleep_cfg_data) {
			msm_gpios_enable(curr->sleep_cfg_data, curr->size);
			msm_gpios_free(curr->sleep_cfg_data, curr->size);
		} else {
			msm_gpios_disable_free(curr->cfg_data, curr->size);
		}
	}

	return rc;
}

static uint32_t msm_sdcc_setup_vreg(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct regulator *curr = sdcc_vreg_data[dev_id - 1];
	static int enabled_once[] = {0, 0, 0, 0};

	if (test_bit(dev_id, &vreg_sts) == enable)
		return rc;

	if (dev_id == 4) {
		if (enable) {
			pr_debug("Enable Vdd dev_%d\n", dev_id);
			gpio_set_value_cansleep(
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC4_PWR_EN_N),
						0);
			set_bit(dev_id, &vreg_sts);
		} else {
			pr_debug("Disable Vdd dev_%d\n", dev_id);
			gpio_set_value_cansleep(
				PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC4_PWR_EN_N),
				1);
			clear_bit(dev_id, &vreg_sts);
		}
	}

	if (!enable || enabled_once[dev_id - 1])
			return 0;
	if (!curr)
		return -ENODEV;

	if (IS_ERR(curr))
		return PTR_ERR(curr);

	if (enable) {
		set_bit(dev_id, &vreg_sts);

		rc = regulator_enable(curr);
		if (rc)
			pr_err("%s: could not enable regulator: %d\n",
					__func__, rc);
		enabled_once[dev_id - 1] = 1;
	} else {
		clear_bit(dev_id, &vreg_sts);

		rc = regulator_disable(curr);
		if (rc)
			pr_err("%s: could not disable regulator: %d\n",
					__func__, rc);
	}
	return rc;
}

static uint32_t msm_sdcc_setup_power(struct device *dv, unsigned int vdd)
{
	int rc = 0;
	struct platform_device *pdev;

	pdev = container_of(dv, struct platform_device, dev);
	rc = msm_sdcc_setup_gpio(pdev->id, (vdd ? 1 : 0));
	if (rc)
		goto out;

	if (pdev->id == 4) /* S3 is always ON and cannot be disabled */
		rc = msm_sdcc_setup_vreg(pdev->id, (vdd ? 1 : 0));
out:
	return rc;
}

#if defined(CONFIG_MMC_MSM_SDC1_SUPPORT) && \
	defined(CONFIG_CSDIO_VENDOR_ID) && \
	defined(CONFIG_CSDIO_DEVICE_ID) && \
	(CONFIG_CSDIO_VENDOR_ID == 0x70 && CONFIG_CSDIO_DEVICE_ID == 0x1117)

#define MBP_ON  1
#define MBP_OFF 0

#define MBP_RESET_N \
	GPIO_CFG(44, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA)
#define MBP_INT0 \
	GPIO_CFG(46, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA)

#define MBP_MODE_CTRL_0 \
	GPIO_CFG(35, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA)
#define MBP_MODE_CTRL_1 \
	GPIO_CFG(36, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA)
#define MBP_MODE_CTRL_2 \
	GPIO_CFG(34, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA)
#define TSIF_EN \
	GPIO_CFG(35, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN,	GPIO_CFG_2MA)
#define TSIF_DATA \
	GPIO_CFG(36, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN,	GPIO_CFG_2MA)
#define TSIF_CLK \
	GPIO_CFG(34, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)

static struct msm_gpio mbp_cfg_data[] = {
	{GPIO_CFG(44, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
		"mbp_reset"},
	{GPIO_CFG(85, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),
		"mbp_io_voltage"},
};

static int mbp_config_gpios_pre_init(int enable)
{
	int rc = 0;

	if (enable) {
		rc = msm_gpios_request_enable(mbp_cfg_data,
			ARRAY_SIZE(mbp_cfg_data));
		if (rc) {
			printk(KERN_ERR
				"%s: Failed to turnon GPIOs for mbp chip(%d)\n",
				__func__, rc);
		}
	} else
		msm_gpios_disable_free(mbp_cfg_data, ARRAY_SIZE(mbp_cfg_data));
	return rc;
}

static struct regulator_bulk_data mbp_regs_io[2];
static struct regulator_bulk_data mbp_regs_rf[2];
static struct regulator_bulk_data mbp_regs_adc[1];
static struct regulator_bulk_data mbp_regs_core[1];

static int mbp_init_regs(struct device *dev)
{
	struct regulator_bulk_data regs[] = {
		/* Analog and I/O regs */
		{ .supply = "gp4",  .min_uV = 2600000, .max_uV = 2600000 },
		{ .supply = "s3",   .min_uV = 1800000, .max_uV = 1800000 },
		/* RF regs */
		{ .supply = "s2",   .min_uV = 1300000, .max_uV = 1300000 },
		{ .supply = "rf",   .min_uV = 2600000, .max_uV = 2600000 },
		/* ADC regs */
		{ .supply = "s4",   .min_uV = 2200000, .max_uV = 2200000 },
		/* Core regs */
		{ .supply = "gp16", .min_uV = 1200000, .max_uV = 1200000 },
	};

	struct regulator_bulk_data *regptr = regs;
	int rc;

	rc = regulator_bulk_get(dev, ARRAY_SIZE(regs), regs);

	if (rc) {
		dev_err(dev, "%s: could not get regulators: %d\n",
				__func__, rc);
		goto out;
	}

	rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs), regs);

	if (rc) {
		dev_err(dev, "%s: could not set voltages: %d\n",
				__func__, rc);
		goto reg_free;
	}

	memcpy(mbp_regs_io, regptr, sizeof(mbp_regs_io));
	regptr += ARRAY_SIZE(mbp_regs_io);

	memcpy(mbp_regs_rf, regptr, sizeof(mbp_regs_rf));
	regptr += ARRAY_SIZE(mbp_regs_rf);

	memcpy(mbp_regs_adc, regptr, sizeof(mbp_regs_adc));
	regptr += ARRAY_SIZE(mbp_regs_adc);

	memcpy(mbp_regs_core, regptr, sizeof(mbp_regs_core));

	return 0;

reg_free:
	regulator_bulk_free(ARRAY_SIZE(regs), regs);
out:
	return rc;
}

static int mbp_setup_rf_vregs(int state)
{
	return state ?
		regulator_bulk_enable(ARRAY_SIZE(mbp_regs_rf), mbp_regs_rf) :
		regulator_bulk_disable(ARRAY_SIZE(mbp_regs_rf), mbp_regs_rf);
}

static int mbp_setup_vregs(int state)
{
	return state ?
		regulator_bulk_enable(ARRAY_SIZE(mbp_regs_io), mbp_regs_io) :
		regulator_bulk_disable(ARRAY_SIZE(mbp_regs_io), mbp_regs_io);
}

static int mbp_set_tcxo_en(int enable)
{
	int rc;
	const char *id = "UBMC";
	struct vreg *vreg_analog = NULL;

	rc = pmapp_clock_vote(id, PMAPP_CLOCK_ID_A1,
		enable ? PMAPP_CLOCK_VOTE_ON : PMAPP_CLOCK_VOTE_OFF);
	if (rc < 0) {
		printk(KERN_ERR "%s: unable to %svote for a1 clk\n",
			__func__, enable ? "" : "de-");
		return -EIO;
	}
	return rc;
}

static void mbp_set_freeze_io(int state)
{
	if (state)
		gpio_set_value(85, 0);
	else
		gpio_set_value(85, 1);
}

static int mbp_set_core_voltage_en(int enable)
{
	static bool is_enabled;
	int rc = 0;

	if (enable && !is_enabled) {
		rc = regulator_bulk_enable(ARRAY_SIZE(mbp_regs_core),
				mbp_regs_core);
		if (rc) {
			pr_err("%s: could not enable regulators: %d\n",
					__func__, rc);
		} else {
			is_enabled = true;
		}
	}

	return rc;
}

static void mbp_set_reset(int state)
{
	if (state)
		gpio_set_value(GPIO_PIN(MBP_RESET_N), 0);
	else
		gpio_set_value(GPIO_PIN(MBP_RESET_N), 1);
}

static int mbp_config_interface_mode(int state)
{
	if (state) {
		gpio_tlmm_config(MBP_MODE_CTRL_0, GPIO_CFG_ENABLE);
		gpio_tlmm_config(MBP_MODE_CTRL_1, GPIO_CFG_ENABLE);
		gpio_tlmm_config(MBP_MODE_CTRL_2, GPIO_CFG_ENABLE);
		gpio_set_value(GPIO_PIN(MBP_MODE_CTRL_0), 0);
		gpio_set_value(GPIO_PIN(MBP_MODE_CTRL_1), 1);
		gpio_set_value(GPIO_PIN(MBP_MODE_CTRL_2), 0);
	} else {
		gpio_tlmm_config(MBP_MODE_CTRL_0, GPIO_CFG_DISABLE);
		gpio_tlmm_config(MBP_MODE_CTRL_1, GPIO_CFG_DISABLE);
		gpio_tlmm_config(MBP_MODE_CTRL_2, GPIO_CFG_DISABLE);
	}
	return 0;
}

static int mbp_setup_adc_vregs(int state)
{
	return state ?
		regulator_bulk_enable(ARRAY_SIZE(mbp_regs_adc), mbp_regs_adc) :
		regulator_bulk_disable(ARRAY_SIZE(mbp_regs_adc), mbp_regs_adc);
}

static int mbp_power_up(void)
{
	int rc;

	rc = mbp_config_gpios_pre_init(MBP_ON);
	if (rc)
		goto exit;
	pr_debug("%s: mbp_config_gpios_pre_init() done\n", __func__);

	rc = mbp_setup_vregs(MBP_ON);
	if (rc)
		goto exit;
	pr_debug("%s: gp4 (2.6) and s3 (1.8) done\n", __func__);

	rc = mbp_set_tcxo_en(MBP_ON);
	if (rc)
		goto exit;
	pr_debug("%s: tcxo clock done\n", __func__);

	mbp_set_freeze_io(MBP_OFF);
	pr_debug("%s: set gpio 85 to 1 done\n", __func__);

	udelay(100);
	mbp_set_reset(MBP_ON);

	udelay(300);
	rc = mbp_config_interface_mode(MBP_ON);
	if (rc)
		goto exit;
	pr_debug("%s: mbp_config_interface_mode() done\n", __func__);

	udelay(100 + mbp_set_core_voltage_en(MBP_ON));
	pr_debug("%s: power gp16 1.2V done\n", __func__);

	mbp_set_freeze_io(MBP_ON);
	pr_debug("%s: set gpio 85 to 0 done\n", __func__);

	udelay(100);

	rc = mbp_setup_rf_vregs(MBP_ON);
	if (rc)
		goto exit;
	pr_debug("%s: s2 1.3V and rf 2.6V done\n", __func__);

	rc = mbp_setup_adc_vregs(MBP_ON);
	if (rc)
		goto exit;
	pr_debug("%s: s4 2.2V  done\n", __func__);

	udelay(200);

	mbp_set_reset(MBP_OFF);
	pr_debug("%s: close gpio 44 done\n", __func__);

	msleep(20);
exit:
	return rc;
}

static int mbp_power_down(void)
{
	int rc;

	mbp_set_reset(MBP_ON);
	pr_debug("%s: mbp_set_reset(MBP_ON) done\n", __func__);

	udelay(100);

	rc = mbp_setup_adc_vregs(MBP_OFF);
	if (rc)
		goto exit;
	pr_debug("%s: vreg_disable(vreg_adc) done\n", __func__);

	udelay(5);

	rc = mbp_setup_rf_vregs(MBP_OFF);
	if (rc)
		goto exit;
	pr_debug("%s: mbp_setup_rf_vregs(MBP_OFF) done\n", __func__);

	udelay(5);

	mbp_set_freeze_io(MBP_OFF);
	pr_debug("%s: mbp_set_freeze_io(MBP_OFF) done\n", __func__);

	udelay(100);
	rc = mbp_set_core_voltage_en(MBP_OFF);
	if (rc)
		goto exit;
	pr_debug("%s: mbp_set_core_voltage_en(MBP_OFF) done\n", __func__);

	rc = mbp_set_tcxo_en(MBP_OFF);
	if (rc)
		goto exit;
	pr_debug("%s: mbp_set_tcxo_en(MBP_OFF) done\n", __func__);

	rc = mbp_setup_vregs(MBP_OFF);
	if (rc)
		goto exit;
	pr_debug("%s: mbp_setup_vregs(MBP_OFF) done\n", __func__);

	rc = mbp_config_gpios_pre_init(MBP_OFF);
	if (rc)
		goto exit;
exit:
	return rc;
}

static void (*mbp_status_notify_cb)(int card_present, void *dev_id);
static void *mbp_status_notify_cb_devid;
static int mbp_power_status;
static int mbp_power_init_done;

static uint32_t mbp_setup_power(struct device *dv,
	unsigned int power_status)
{
	int rc = 0;
	struct platform_device *pdev;

	pdev = container_of(dv, struct platform_device, dev);

	if (power_status == mbp_power_status)
		goto exit;
	if (power_status) {
		pr_debug("turn on power of mbp slot");
		rc = mbp_power_up();
		mbp_power_status = 1;
	} else {
		pr_debug("turn off power of mbp slot");
		rc = mbp_power_down();
		mbp_power_status = 0;
	}
exit:
	return rc;
};

int mbp_register_status_notify(void (*callback)(int, void *),
	void *dev_id)
{
	mbp_status_notify_cb = callback;
	mbp_status_notify_cb_devid = dev_id;
	return 0;
}

static unsigned int mbp_status(struct device *dev)
{
	return mbp_power_status;
}

static uint32_t msm_sdcc_setup_power_mbp(struct device *dv, unsigned int vdd)
{
	struct platform_device *pdev;
	uint32_t rc = 0;

	pdev = container_of(dv, struct platform_device, dev);
	rc = msm_sdcc_setup_power(dv, vdd);
	if (rc) {
		pr_err("%s: Failed to setup power (%d)\n",
			__func__, rc);
		goto out;
	}
	if (!mbp_power_init_done) {
		rc = mbp_init_regs(dv);
		if (rc) {
			dev_err(dv, "%s: regulator init failed: %d\n",
					__func__, rc);
			goto out;
		}
		mbp_setup_power(dv, 1);
		mbp_setup_power(dv, 0);
		mbp_power_init_done = 1;
	}
	if (vdd >= 0x8000) {
		rc = mbp_setup_power(dv, (0x8000 == vdd) ? 0 : 1);
		if (rc) {
			pr_err("%s: Failed to config mbp chip power (%d)\n",
				__func__, rc);
			goto out;
		}
		if (mbp_status_notify_cb) {
			mbp_status_notify_cb(mbp_power_status,
				mbp_status_notify_cb_devid);
		}
	}
out:
	/* should return 0 only */
	return 0;
}

#endif

#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static unsigned int msm7x30_sdcc_slot_status(struct device *dev)
{
	return (unsigned int)
		gpio_get_value_cansleep(
			PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SD_DET - 1));
}

static int msm_sdcc_get_wpswitch(struct device *dv)
{
	void __iomem *wp_addr = 0;
	uint32_t ret = 0;
	struct platform_device *pdev;

	if (!(machine_is_msm7x30_surf()))
		return -1;
	pdev = container_of(dv, struct platform_device, dev);

	wp_addr = ioremap(FPGA_SDCC_STATUS, 4);
	if (!wp_addr) {
		pr_err("%s: Could not remap %x\n", __func__, FPGA_SDCC_STATUS);
		return -ENOMEM;
	}

	ret = (((readl(wp_addr) >> 4) >> (pdev->id-1)) & 0x01);
	pr_info("%s: WP Status for Slot %d = 0x%x \n", __func__,
							pdev->id, ret);
	iounmap(wp_addr);

	return ret;
}
#endif

#if defined(CONFIG_MMC_MSM_SDC1_SUPPORT)
#if defined(CONFIG_CSDIO_VENDOR_ID) && \
	defined(CONFIG_CSDIO_DEVICE_ID) && \
	(CONFIG_CSDIO_VENDOR_ID == 0x70 && CONFIG_CSDIO_DEVICE_ID == 0x1117)
static struct mmc_platform_data msm7x30_sdc1_data = {
	.ocr_mask	= MMC_VDD_165_195 | MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power_mbp,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.status	        = mbp_status,
	.register_status_notify = mbp_register_status_notify,
        //	.msmsdcc_fmin	= 400000,
        //	.msmsdcc_fmid	= 24000000,
        //	.msmsdcc_fmax	= 48000000,
        .msmsdcc_fmin	= 144000,
        .msmsdcc_fmid	= 24576000,
        .msmsdcc_fmax	= 24576000,
	.nonremovable	= 0,
};
#else
static struct mmc_platform_data msm7x30_sdc1_data = {
	.ocr_mask	= MMC_VDD_165_195,
	.translate_vdd	= msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
        //	.msmsdcc_fmin	= 144000,
        //	.msmsdcc_fmid	= 24576000,
        //	.msmsdcc_fmax	= 49152000,
	.nonremovable	= 0,
};
#endif
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static struct mmc_platform_data msm7x30_sdc2_data = {
	.ocr_mask	= MMC_VDD_165_195 | MMC_VDD_27_28,
	.translate_vdd	= msm_sdcc_setup_power,
#ifdef CONFIG_MMC_MSM_SDC2_8_BIT_SUPPORT
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,
#else
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#endif
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
        //	.msmsdcc_fmax	= 48000000,
	.msmsdcc_fmax	= 50000000,
        //	.msmsdcc_fmin	= 144000,
        //	.msmsdcc_fmid	= 24576000,
        //	.msmsdcc_fmax	= 49152000,
	.nonremovable	= 1,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct mmc_platform_data msm7x30_sdc3_data = {
	.ocr_mask	= MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.sdiowakeup_irq = MSM_GPIO_TO_INT(118),
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
        //	.msmsdcc_fmin	= 144000,
        //	.msmsdcc_fmid	= 24576000,
        //	.msmsdcc_fmax	= 49152000,
	.nonremovable	= 0,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct mmc_platform_data msm7x30_sdc4_data = {
	.ocr_mask	= MMC_VDD_27_28 | MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.status      = msm7x30_sdcc_slot_status,
	.status_irq  = PM8058_GPIO_IRQ(PMIC8058_IRQ_BASE, PMIC_GPIO_SD_DET - 1),
	.irq_flags   = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.wpswitch    = msm_sdcc_get_wpswitch,
	.msmsdcc_fmin	= 400000,
	.msmsdcc_fmid	= 24000000,
	.msmsdcc_fmax	= 48000000,
        //	.msmsdcc_fmin	= 144000,
        //	.msmsdcc_fmid	= 24576000,
        //	.msmsdcc_fmax	= 49152000,
	.nonremovable	= 0,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static int msm_sdc1_lvlshft_enable(void)
{
	static struct regulator *ldo5;
	int rc;

	/* Enable LDO5, an input to the FET that powers slot 1 */

	ldo5 = regulator_get(NULL, "ldo5");

	if (IS_ERR(ldo5)) {
		rc = PTR_ERR(ldo5);
		pr_err("%s: could not get ldo5: %d\n", __func__, rc);
		goto out;
	}

	rc = regulator_set_voltage(ldo5, 2850000, 2850000);
	if (rc) {
		pr_err("%s: could not set ldo5 voltage: %d\n", __func__, rc);
		goto ldo5_free;
	}

	rc = regulator_enable(ldo5);
	if (rc) {
		pr_err("%s: could not enable ldo5: %d\n", __func__, rc);
		goto ldo5_free;
	}

	/* Enable GPIO 35, to turn on the FET that powers slot 1 */
	rc = msm_gpios_request_enable(sdc1_lvlshft_cfg_data,
				ARRAY_SIZE(sdc1_lvlshft_cfg_data));
	if (rc)
		printk(KERN_ERR "%s: Failed to enable GPIO 35\n", __func__);

	rc = gpio_direction_output(GPIO_PIN(sdc1_lvlshft_cfg_data[0].gpio_cfg),
				1);
	if (rc)
		printk(KERN_ERR "%s: Failed to turn on GPIO 35\n", __func__);

	return 0;

ldo5_free:
	regulator_put(ldo5);
out:
	ldo5 = NULL;
	return rc;
}
#endif

static int mmc_regulator_init(int sdcc_no, const char *supply, int uV)
{
	int rc;

	BUG_ON(sdcc_no < 1 || sdcc_no > 4);

	sdcc_no--;

	sdcc_vreg_data[sdcc_no] = regulator_get(NULL, supply);

	if (IS_ERR(sdcc_vreg_data[sdcc_no])) {
		rc = PTR_ERR(sdcc_vreg_data[sdcc_no]);
		pr_err("%s: could not get regulator \"%s\": %d\n",
				__func__, supply, rc);
		goto out;
	}

	rc = regulator_set_voltage(sdcc_vreg_data[sdcc_no], uV, uV);

	if (rc) {
		pr_err("%s: could not set voltage for \"%s\" to %d uV: %d\n",
				__func__, supply, uV, rc);
		goto reg_free;
	}

	return rc;

reg_free:
	regulator_put(sdcc_vreg_data[sdcc_no]);
out:
	sdcc_vreg_data[sdcc_no] = NULL;
	return rc;
}

static void __init msm7x30_init_mmc(void)
{
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	if (mmc_regulator_init(1, "s3", 1800000))
		goto out1;

	if (machine_is_msm7x30_fluid()) {
		msm7x30_sdc1_data.ocr_mask =  MMC_VDD_27_28 | MMC_VDD_28_29;
		if (msm_sdc1_lvlshft_enable()) {
                        pr_err("%s: could not enable level shift\n", __func__);
			goto out1;
		}
	}

	msm_add_sdcc(1, &msm7x30_sdc1_data);
out1:
#endif
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	if (mmc_regulator_init(2, "s3", 1800000))
		goto out2;

	if (machine_is_msm8x55_svlte_surf())
		msm7x30_sdc2_data.msmsdcc_fmax =  24576000;
	if (machine_is_msm8x55_svlte_surf() ||
			machine_is_msm8x55_svlte_ffa()) {
		msm7x30_sdc2_data.sdiowakeup_irq = MSM_GPIO_TO_INT(68);
		msm7x30_sdc2_data.is_sdio_al_client = 1;
	}

	msm_add_sdcc(2, &msm7x30_sdc2_data);
 out2:
#endif
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	if (mmc_regulator_init(3, "s3", 1800000))
		goto out3;

	msm_sdcc_setup_gpio(3, 1);
	msm_add_sdcc(3, &msm7x30_sdc3_data);
out3:
#endif
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	if (mmc_regulator_init(4, "mmc", 2850000))
		return;

	msm_add_sdcc(4, &msm7x30_sdc4_data);
#endif
        return ;
}

#ifdef CONFIG_SERIAL_MSM_CONSOLE
static struct msm_gpio uart2_config_data[] = {
	{ GPIO_CFG(49, 2, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "UART2_RFR"},
	{ GPIO_CFG(50, 2, GPIO_CFG_INPUT,   GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "UART2_CTS"},
	{ GPIO_CFG(51, 2, GPIO_CFG_INPUT,   GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "UART2_Rx"},
	{ GPIO_CFG(52, 2, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "UART2_Tx"},
};

static void msm7x30_init_uart2(void)
{
	msm_gpios_request_enable(uart2_config_data,
			ARRAY_SIZE(uart2_config_data));

}
#endif

/* TSIF begin */
#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)

#define TSIF_B_SYNC      GPIO_CFG(37, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
#define TSIF_B_DATA      GPIO_CFG(36, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
#define TSIF_B_EN        GPIO_CFG(35, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
#define TSIF_B_CLK       GPIO_CFG(34, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)

static const struct msm_gpio tsif_gpios[] = {
	{ .gpio_cfg = TSIF_B_CLK,  .label =  "tsif_clk", },
	{ .gpio_cfg = TSIF_B_EN,   .label =  "tsif_en", },
	{ .gpio_cfg = TSIF_B_DATA, .label =  "tsif_data", },
	{ .gpio_cfg = TSIF_B_SYNC, .label =  "tsif_sync", },
};

static struct msm_tsif_platform_data tsif_platform_data = {
	.num_gpios = ARRAY_SIZE(tsif_gpios),
	.gpios = tsif_gpios,
	.tsif_pclk = "iface_clk",
	.tsif_ref_clk = "ref_clk",
};
#endif /* defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE) */
/* TSIF end   */

static struct msm_spm_platform_data msm_spm_data __initdata = {
	.reg_base_addr = MSM_SAW0_BASE,

	.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x05,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x18,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0x00006666,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0xFF000666,

	.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x01,
	.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x03,
	.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

	.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
	.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

	.awake_vlevel = 0xF2,
	.retention_vlevel = 0xE0,
	.collapse_vlevel = 0x72,
	.retention_mid_vlevel = 0xE0,
	.collapse_mid_vlevel = 0xE0,

	.vctl_timeout_us = 50,
};

#if defined(CONFIG_TOUCHSCREEN_TSC2007) || \
	defined(CONFIG_TOUCHSCREEN_TSC2007_MODULE)

#define TSC2007_TS_PEN_INT	20

static struct msm_gpio tsc2007_config_data[] = {
	{ GPIO_CFG(TSC2007_TS_PEN_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	"tsc2007_irq" },
};

static struct regulator_bulk_data tsc2007_regs[] = {
	{ .supply = "s3", .min_uV = 1800000, .max_uV = 1800000 },
	{ .supply = "s2", .min_uV = 1300000, .max_uV = 1300000 },
};

static int tsc2007_init(void)
{
	int rc;

	rc = regulator_bulk_get(NULL, ARRAY_SIZE(tsc2007_regs), tsc2007_regs);

	if (rc) {
		pr_err("%s: could not get regulators: %d\n", __func__, rc);
		goto out;
	}

	rc = regulator_bulk_set_voltage(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);

	if (rc) {
		pr_err("%s: could not set voltages: %d\n", __func__, rc);
		goto reg_free;
	}

	rc = regulator_bulk_enable(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);

	if (rc) {
		pr_err("%s: could not enable regulators: %d\n", __func__, rc);
		goto reg_free;
	}

	rc = msm_gpios_request_enable(tsc2007_config_data,
			ARRAY_SIZE(tsc2007_config_data));
	if (rc) {
		pr_err("%s: Unable to request gpios\n", __func__);
		goto reg_disable;
	}

	return 0;

reg_disable:
	regulator_bulk_disable(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);
reg_free:
	regulator_bulk_free(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);
out:
	return rc;
}

static int tsc2007_get_pendown_state(void)
{
	int rc;

	rc = gpio_get_value(TSC2007_TS_PEN_INT);
	if (rc < 0) {
		pr_err("%s: MSM GPIO %d read failed\n", __func__,
						TSC2007_TS_PEN_INT);
		return rc;
	}

	return (rc == 0 ? 1 : 0);
}

static void tsc2007_exit(void)
{

	regulator_bulk_disable(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);
	regulator_bulk_free(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);

	msm_gpios_disable_free(tsc2007_config_data,
		ARRAY_SIZE(tsc2007_config_data));
}

static int tsc2007_power_shutdown(bool enable)
{
	int rc;

	rc = (enable == false) ?
		regulator_bulk_enable(ARRAY_SIZE(tsc2007_regs), tsc2007_regs) :
		regulator_bulk_disable(ARRAY_SIZE(tsc2007_regs), tsc2007_regs);

	if (rc) {
		pr_err("%s: could not %sable regulators: %d\n",
				__func__, enable ? "dis" : "en", rc);
		return rc;
	}

	if (enable == false)
		msleep(20);

	return 0;
}

static struct tsc2007_platform_data tsc2007_ts_data = {
	.model = 2007,
	.x_plate_ohms = 300,
	.min_x		= 210,
	.max_x		= 3832,
	.min_y		= 150,
	.max_y		= 3936,
	.irq_flags    = IRQF_TRIGGER_LOW,
	.init_platform_hw = tsc2007_init,
	.exit_platform_hw = tsc2007_exit,
	.power_shutdown	  = tsc2007_power_shutdown,
	.invert_x	  = true,
	.invert_y	  = true,
	/* REVISIT: Temporary fix for reversed pressure */
	.invert_z1	  = true,
	.invert_z2	  = true,
	.get_pendown_state = tsc2007_get_pendown_state,
};

static struct i2c_board_info tsc_i2c_board_info[] = {
	{
		I2C_BOARD_INFO("tsc2007", 0x48),
		.irq		= MSM_GPIO_TO_INT(TSC2007_TS_PEN_INT),
		.platform_data = &tsc2007_ts_data,
	},
};
#endif

static void vision_reset(void)
{
	gpio_set_value(VISION_GPIO_PS_HOLD, 0);
}

static struct platform_device *devices[] __initdata = {
#if defined(CONFIG_SERIAL_MSM) || defined(CONFIG_MSM_SERIAL_DEBUGGER)
	&msm_device_uart2,
#endif
#ifdef CONFIG_MSM_PROC_COMM_REGULATOR
	&msm_proccomm_regulator_dev,
#endif
	&asoc_msm_pcm,
	&asoc_msm_dai0,
	&asoc_msm_dai1,
#if defined (CONFIG_SND_MSM_MVS_DAI_SOC)
	&asoc_msm_mvs,
	&asoc_mvs_dai0,
	&asoc_mvs_dai1,
#endif
	&msm_device_smd,
	&msm_device_dmov,
	&qsd_device_spi,
	&msm_device_nand,
        &msm_device_otg,
        &msm_device_gadget_peripheral,
        &android_usb_device,

#ifdef CONFIG_MSM_SSBI
	&msm_device_ssbi_pmic1,
#endif
#ifdef CONFIG_I2C_SSBI
	&msm_device_ssbi7,
#endif
#ifdef CONFIG_MSM_V4L2_VIDEO_OVERLAY_DEVICE
	&msm_v4l2_video_overlay_device,
#endif
	&msm_migrate_pages_device,
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	&android_pmem_device,
	&android_pmem_adsp_device,
#endif
	&android_pmem_audio_device,
#endif 
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
	&msm_device_i2c,
	&msm_device_i2c_2,
#ifdef CONFIG_INPUT_CAPELLA_CM3602
        &capella_cm3602,
#endif
	&msm_device_uart_dm1,
#ifdef CONFIG_MSM7KV2_AUDIO
	&msm_aictl_device,
	&msm_mi2s_device,
	&msm_lpa_device,
	&msm_aux_pcm_device,
#endif
	&qup_device_i2c,
	&msm_kgsl_3d0,
	&msm_kgsl_2d0,
	&msm_device_vidc_720p,
#ifdef CONFIG_MSM_GEMINI
	&msm_gemini_device,
#endif
#ifndef CONFIG_MSM_CAMERA_V4L2
#ifdef CONFIG_MSM_VPE
	&msm_vpe_device,
#endif
#endif
#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
	&msm_device_tsif,
#endif
#ifdef CONFIG_MSM_SDIO_AL
	&msm_device_sdio_al,
#endif

        &htc_battery_pdev,
	&msm_ebi0_thermal,
	&msm_ebi1_thermal,
	&msm_adsp_device,
#ifdef CONFIG_ION_MSM
	&ion_dev,
#endif
};

static void __init msm7x30_init(void)
{
        int rc = 0;
	struct kobject *properties_kobj;
	unsigned smem_size;
	uint32_t soc_version = 0;

	/* Must set msm_hw_reset_hook before first proc comm */
	msm_hw_reset_hook = vision_reset;

        htc_add_ramconsole_devices();
	soc_version = socinfo_get_version();
#ifdef AUTORESTART
        delay = msecs_to_jiffies(AUTORESTART);
        wq = create_singlethread_workqueue("my");
        queue_delayed_work(wq, &my_work, delay);
#endif

	msm_clock_init(&msm7x30_clock_init_data);
#ifdef CONFIG_SERIAL_MSM_CONSOLE
	msm7x30_init_uart2();
#endif
	msm_spm_init(&msm_spm_data, 1);
	platform_device_register(&msm7x30_device_acpuclk);

#ifdef CONFIG_BT
	bt_export_bd_address();
#endif
#ifdef CONFIG_SERIAL_MSM_HS
#ifdef CONFIG_SERIAL_MSM_HS_PURE_ANDROID
        //	msm_uart_dm1_pdata.rx_wakeup_irq = -1;
#else
        //	msm_uart_dm1_pdata.rx_wakeup_irq = gpio_to_irq(VISION_GPIO_BT_HOST_WAKE);
	msm_device_uart_dm1.name = "msm_serial_hs_brcm";
#endif
	msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
	if (SOCINFO_VERSION_MAJOR(soc_version) >= 2 &&
			SOCINFO_VERSION_MINOR(soc_version) >= 1) {
		pr_debug("%s: SOC Version:2.(1 or more)\n", __func__);
		msm_otg_pdata.ldo_set_voltage = 0;
	}

	msm_device_otg.dev.platform_data = &msm_otg_pdata;
#ifdef CONFIG_USB_GADGET
	msm_otg_pdata.swfi_latency =
 	msm_pm_data
 	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
#endif
#endif

#ifdef CONFIG_MSM_SSBI
	msm_device_ssbi_pmic1.dev.platform_data =
				&msm7x30_ssbi_pm8058_pdata;
#endif
	buses_init();

	platform_add_devices(devices, ARRAY_SIZE(devices));

        vision_fb_init();

#ifdef CONFIG_USB_EHCI_MSM_72K
	msm_add_host(0, &msm_usb_host_pdata);
#endif

	msm7x30_init_mmc();
	msm_qsd_spi_init();

	spi_register_board_info(msm_spi_board_info, ARRAY_SIZE(msm_spi_board_info));

	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	msm_pm_register_irqs();
	msm_device_i2c_init();
	msm_device_i2c_2_init();
#ifdef CONFIG_MICROP_COMMON
	vision_microp_init();
#endif
	qup_device_i2c_init();

	if (system_rev >= 1)
		i2c_register_board_info(0, i2c_compass_devices2,
			ARRAY_SIZE(i2c_compass_devices2));
	else
		i2c_register_board_info(0, i2c_compass_devices1,
			ARRAY_SIZE(i2c_compass_devices1));

#ifdef CONFIG_I2C_SSBI
	msm_device_ssbi7.dev.platform_data = &msm_i2c_ssbi7_pdata;
#endif
	pm8058_gpios_init();

#ifdef CONFIG_MSM_RMT_STORAGE_SERVER
        rmt_storage_add_ramfs();
#endif

	boot_reason = *(unsigned int *)
		(smem_get_entry(SMEM_POWER_ON_STATUS_INFO, &smem_size));
	printk(KERN_NOTICE "Boot Reason = 0x%02x\n", boot_reason);

	properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (properties_kobj)
		rc = sysfs_create_group(properties_kobj,
				&vision_properties_attr_group);
	if (!properties_kobj || rc)
		pr_err("failed to create board_properties\n");

	i2c_register_board_info(0, i2c_devices,	ARRAY_SIZE(i2c_devices));

        vision_init_keypad();
}

#ifdef CONFIG_ION_MSM
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct ion_cp_heap_pdata cp_wb_ion_pdata = {
	.permission_type = IPT_TYPE_MDP_WRITEBACK,
	.align = PAGE_SIZE,
};

static struct ion_co_heap_pdata co_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
};
#endif

/**
 * These heaps are listed in the order they will be allocated.
 * Don't swap the order unless you know what you are doing!
 */
struct ion_platform_heap msm7x30_heaps[] = {
		{
			.id	= ION_SYSTEM_HEAP_ID,
			.type	= ION_HEAP_TYPE_SYSTEM,
			.name	= ION_VMALLOC_HEAP_NAME,
		},
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
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
	.heaps = msm7x30_heaps,
};

static struct platform_device ion_dev = {
	.name = "ion-msm",
	.id = 1,
	.dev = { .platform_data = &ion_pdata },
};
#endif

static struct memtype_reserve msm7x30_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

unsigned long size;
unsigned long msm_ion_camera_size;

static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION

	android_pmem_adsp_pdata.size = size;
	android_pmem_pdata.size = pmem_sf_size;
#endif
	android_pmem_audio_pdata.size = pmem_audio_size;
#endif
}

#ifdef CONFIG_ANDROID_PMEM
static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	msm7x30_reserve_table[p->memory_type].size += p->size;
}
#endif

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
#endif
	reserve_memory_for(&android_pmem_audio_pdata);
	msm7x30_reserve_table[MEMTYPE_EBI0].size += pmem_kernel_ebi0_size;
#endif
}

static void __init reserve_mdp_memory(void)
{
	msm7x30_mdp_writeback(msm7x30_reserve_table);
}

static void __init reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	msm7x30_reserve_table[MEMTYPE_EBI0].size += MSM_ION_CAMERA_SIZE;
	msm7x30_reserve_table[MEMTYPE_EBI0].size += MSM_ION_WB_SIZE;
	msm7x30_reserve_table[MEMTYPE_EBI0].size += MSM_ION_AUDIO_SIZE;
	msm7x30_reserve_table[MEMTYPE_EBI0].size += MSM_ION_SF_SIZE;
#endif
}

static void __init msm7x30_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
	reserve_mdp_memory();
	reserve_ion_memory();
}

static int msm7x30_paddr_to_memtype(unsigned int paddr)
{
	if (paddr < phys_add)
		return MEMTYPE_EBI0;
	if (paddr >= phys_add && paddr < 0x80000000)
		return MEMTYPE_EBI1;
	return MEMTYPE_NONE;
}

static struct reserve_info msm7x30_reserve_info __initdata = {
	.memtype_reserve_table = msm7x30_reserve_table,
	.calculate_reserve_sizes = msm7x30_calculate_reserve_sizes,
	.paddr_to_memtype = msm7x30_paddr_to_memtype,
};

static void __init msm7x30_reserve(void)
{
	reserve_info = &msm7x30_reserve_info;
	msm_reserve();
}

static void __init msm7x30_allocate_memory_regions(void)
{
        msm7x30_allocate_fb_region();
}

static void __init msm7x30_map_io(void)
{
	msm_shared_ram_phys = 0x00400000;
	msm_map_msm7x30_io();
	if (socinfo_init() < 0)
		printk(KERN_ERR "%s: socinfo_init() failed!\n",
		       __func__);
}

static void __init msm7x30_init_early(void)
{
	msm7x30_allocate_memory_regions();
}

#define MSM_LINUX_BASE1		0x04000000
#define MSM_LINUX_SIZE1		0x0C000000
#define MSM_LINUX_BASE2		0x20000000
#define MSM_LINUX_SIZE2		0x10000000

static void __init msm7x30_fixup(struct tag *tags, char **cmdline,
				 struct meminfo *mi)
{
	mi->nr_banks = 2;
	mi->bank[0].start = MSM_LINUX_BASE1;
	mi->bank[0].size = MSM_LINUX_SIZE1;
	mi->bank[1].start = MSM_LINUX_BASE2;
	mi->bank[1].size = MSM_LINUX_SIZE2;
}

MACHINE_START(VISION, "vision")
	.map_io = msm7x30_map_io,
	.reserve = msm7x30_reserve,
	.init_irq = msm7x30_init_irq,
	.init_machine = msm7x30_init,
	.timer = &msm_timer,
	.init_early = msm7x30_init_early,
	.handle_irq = vic_handle_irq,
	.fixup = msm7x30_fixup,
//        .restart = msm_restart,
MACHINE_END
