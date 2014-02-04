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
#include <linux/smsc911x.h>
#include <linux/spi/spi.h>
#include <linux/input/tdisc_shinetsu.h>
#include <linux/i2c/isa1200.h>
#include <linux/dma-mapping.h>
#include <linux/i2c/bq27520.h>
#include <linux/atmel_qt602240.h>
#include <linux/cm3628.h>
#include <linux/pn544.h>
#include <linux/akm8975.h>
#include <linux/bma250.h>
#include <linux/ewtzmu2.h>
#include <linux/mpu.h>

#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
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

#include <mach/msm_serial_hs.h>
#include <mach/htc_bdaddress.h>

#include <mach/msm_serial_hs_lite.h>
#include <mach/msm_iomap.h>
#include <mach/msm_memtypes.h>
#include <asm/mach/mmc.h>
#include <mach/htc_battery_8x60.h>
#ifdef CONFIG_MFD_TPS65200
#include <linux/mfd/tps65200.h>
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
#include <linux/tpa2051d3.h>
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
#include <mach/htc_headset_misc.h>
#include <mach/htc_headset_pmic.h>
#include <mach/htc_headset_8x60.h>

#ifdef CONFIG_FB_MSM_HDMI_MHL
#include <mach/mhl.h>
#endif

#include "board-ruby.h"
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
#include <mach/board_htc.h>
#include <linux/memblock.h>
#include <linux/msm_ion.h>
#include <mach/ion.h>
#include <mach/msm_rtb.h>

extern int ps_type;

static unsigned int engineerid, mem_size_mb;

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

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x11,
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

		.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x11,
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

static struct platform_device msm_device_saw_s0 = {
	.name          = "saw-regulator",
	.id            = 0,
	.dev           = {
		.platform_data = &msm_saw_regulator_pdata_s0,
	},
};

static struct platform_device msm_device_saw_s1 = {
	.name          = "saw-regulator",
	.id            = 1,
	.dev           = {
		.platform_data = &msm_saw_regulator_pdata_s1,
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

#define GPIO_ETHERNET_RESET_N ((PM8901_MPP_BASE + PM8901_MPPS) + 9)
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
		[MSM_RPMRS_VDD_MEM_MAX]         = 1250,
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

static int ruby_phy_init_seq[] = { 0x06, 0x36, 0x0C, 0x31, 0x31, 0x32, 0x1, 0x0E, 0x1, 0x11, -1 };
static struct msm_otg_platform_data msm_otg_pdata = {
	.phy_init_seq	= ruby_phy_init_seq,
        .mode		= USB_OTG,
	.otg_control	= OTG_PMIC_CONTROL,
	.phy_type	= CI_45NM_INTEGRATED_PHY,
	.power_budget	= 750,
};

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
	.gpio_mbat_in = MSM_GPIO_TO_INT(RUBY_GPIO_MBAT_IN),
	.gpio_mbat_in_trigger_level = MBAT_IN_HIGH_TRIGGER,
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
		GPIO_CFG(RUBY_TORCH_EN, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(RUBY_FLASH_EN, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	};

	gpio_tlmm_config(flashlight_gpio_table[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(flashlight_gpio_table[1], GPIO_CFG_ENABLE);
}

static struct flashlight_platform_data flashlight_data = {
	.gpio_init 		= config_flashlight_gpios,
	.torch 			= RUBY_TORCH_EN,
	.flash 			= RUBY_FLASH_EN,
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
#ifdef CONFIG_FB_MSM_HDMI_MHL
static void mhl_sii9234_1v2_power(bool enable);
#endif

static uint32_t usb_ID_PIN_input_table[] = {
	GPIO_CFG(RUBY_GPIO_USB_ID, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t usb_ID_PIN_ouput_table[] = {
	GPIO_CFG(RUBY_GPIO_USB_ID, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};


static uint32_t mhl_reset_pin_ouput_table[] = {
	GPIO_CFG(RUBY_GPIO_MHL_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
};

static uint32_t mhl_usb_switch_ouput_table[] = {
	GPIO_CFG(RUBY_GPIO_MHL_USB_SWITCH, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
};

void config_ruby_mhl_gpios(void)
{
	config_gpio_table(mhl_usb_switch_ouput_table,
			ARRAY_SIZE(mhl_usb_switch_ouput_table));
	config_gpio_table(mhl_reset_pin_ouput_table,
			ARRAY_SIZE(mhl_reset_pin_ouput_table));
}

static void pm8058_usb_config(void)
{

}

void config_ruby_usb_id_gpios(bool output)
{
	if (output) {
		gpio_tlmm_config(usb_ID_PIN_ouput_table[0], 0);
		gpio_set_value(RUBY_GPIO_USB_ID, 1);
		printk(KERN_INFO "%s %d output high\n",  __func__, RUBY_GPIO_USB_ID);
	} else {
		gpio_tlmm_config(usb_ID_PIN_input_table[0], 0);
		printk(KERN_INFO "%s %d input none pull\n",  __func__, RUBY_GPIO_USB_ID);
	}
}

static void ruby_usb_dpdn_switch(int path)
{
	switch (path) {
	case PATH_USB:
	case PATH_MHL:
	{
		int polarity = 1; 
		int mhl = (path == PATH_MHL);

		config_gpio_table(mhl_usb_switch_ouput_table,
				ARRAY_SIZE(mhl_usb_switch_ouput_table));

		pr_info("[CABLE] %s: Set %s path\n", __func__, mhl ? "MHL" : "USB");
		gpio_set_value(RUBY_GPIO_MHL_USB_SWITCH, (mhl ^ !polarity) ? 1 : 0);
		break;
	}
	}

	#ifdef CONFIG_FB_MSM_HDMI_MHL
	sii9234_change_usb_owner((path == PATH_MHL)?1:0);
	#endif
}

static int64_t ruby_get_usbid_adc(void)
{
	int mean_adc;
	int adc[5];
	int i, ret = 0;
	int result; /*adc = 1250 * mean_adc / mean_vref*/

	result = 0;
	mean_adc = 0;

	ret = pm8058_htc_config_mpp_and_adc_read(
				adc, 5,
				CHANNEL_ADC_BATT_THERM,
				PM8058_MPP_PM_TO_SYS(XOADC_MPP_4),
				PM_MPP_AIN_AMUX_CH5);
	if (ret)
		pr_info("[CABLE] Get usb id adc data failed.");
	for (i = 0; i < 5; i++)
		mean_adc += adc[i];

	mean_adc /= 5;
	pr_info("[CABLE] %s: MPP ADC %d\n", __func__, mean_adc);

	result = PM8058ADC_15BIT(mean_adc);
	pr_info("[CABLE] %s: result = %d\n", __func__, result);
	return result;
}

static struct cable_detect_platform_data cable_detect_pdata = {
	.vbus_mpp_gpio	= PM8058_MPP_PM_TO_SYS(10),
	.vbus_mpp_config = pm8058_usb_config,
	.vbus_mpp_irq	= (PM8058_CBLPWR_IRQ + PM8058_IRQ_BASE),
	.detect_type	= CABLE_TYPE_PMIC_ADC,
	.get_adc_cb	= ruby_get_usbid_adc,
	.usb_id_pin_gpio= RUBY_GPIO_USB_ID,
	.usb_dpdn_switch= ruby_usb_dpdn_switch,
	.mhl_reset_gpio = RUBY_GPIO_MHL_RESET,
	.mpp_data = {
		.usbid_mpp	= PM8058_MPP_PM_TO_SYS(XOADC_MPP_4),
		.usbid_amux	= PM_MPP_AIN_AMUX_CH5,
	},
        .config_usb_id_gpios = config_ruby_usb_id_gpios,
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
static struct regulator *reg_8901_l0;
static struct regulator *reg_8901_l4;
static struct regulator *reg_8901_l3;

static uint32_t msm_hdmi_off_gpio[] = {
	GPIO_CFG(170,  0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(171,  0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
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
		config_gpio_table(msm_hdmi_on_gpio, ARRAY_SIZE(msm_hdmi_on_gpio));
		hdmi_hpd_feature(1);
		pr_info("%s(on): success\n", __func__);
	} else {
		config_gpio_table(msm_hdmi_off_gpio, ARRAY_SIZE(msm_hdmi_off_gpio));
		hdmi_hpd_feature(0);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = enable;
}

static int mhl_sii9234_all_power(bool enable)
{
	static bool prev_on;
	int rc;

	if (enable == prev_on)
		return 0;

	if (!reg_8901_l4)
		_GET_REGULATOR(reg_8901_l4, "8901_l4");
	if (!reg_8901_l3)
		_GET_REGULATOR(reg_8901_l3, "8901_l3");
	if (!reg_8901_l0)
		_GET_REGULATOR(reg_8901_l0, "8901_l0");

	if (enable) {
		rc = regulator_set_voltage(reg_8901_l4, 1800000, 1800000);
		if (rc) {
			pr_err("%s: regulator_set_voltage reg_8901_l4 failed rc=%d\n",
				__func__, rc);
			return rc;
		}
		rc = regulator_set_voltage(reg_8901_l3, 3300000, 3300000);
		if (rc) {
			pr_err("%s: regulator_set_voltage reg_8901_l3 failed rc=%d\n",
				__func__, rc);
			return rc;
		}

		rc = regulator_set_voltage(reg_8901_l0, 1200000, 1200000);
		if (rc) {
			pr_err("%s: regulator_set_voltage reg_8901_l0 failed rc=%d\n",
				__func__, rc);
			return rc;
		}	rc = regulator_enable(reg_8901_l4);

		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8901_l4", rc);
			return rc;
		}
		rc = regulator_enable(reg_8901_l3);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8901_l3", rc);
			return rc;
		}

		rc = regulator_enable(reg_8901_l0);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8901_l0", rc);
			return rc;
		}
		pr_info("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_8901_l4);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8901_l4", rc);
		rc = regulator_disable(reg_8901_l3);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8901_l3", rc);
		rc = regulator_disable(reg_8901_l0);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8901_l0", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = enable;

	return 0;
}

#ifdef CONFIG_FB_MSM_HDMI_MHL_SII9234
static uint32_t mhl_gpio_table[] = {
	GPIO_CFG(RUBY_GPIO_MHL_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(RUBY_GPIO_MHL_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	GPIO_CFG(RUBY_GPIO_MHL_WAKE_UP, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};
static int mhl_sii9234_power(int on)
{
	int rc = 0;

	switch (on) {
	case 0:
		mhl_sii9234_1v2_power(false);
		break;
	case 1:
		mhl_sii9234_all_power(true);
		config_gpio_table(mhl_gpio_table, ARRAY_SIZE(mhl_gpio_table));
		break;
	default:
		pr_warning("%s(%d) got unsupport parameter!!!\n", __func__, on);
		break;
	}
	return rc;
}

static T_MHL_PLATFORM_DATA mhl_sii9234_device_data = {
	.gpio_intr = RUBY_GPIO_MHL_INT,
	.gpio_reset = RUBY_GPIO_MHL_RESET,
	.ci2ca = 0,
	.mhl_usb_switch		= ruby_usb_dpdn_switch,
	.mhl_1v2_power = mhl_sii9234_1v2_power,
	.enable_5v = hdmi_enable_5v,
	.power = mhl_sii9234_power,
};

static struct i2c_board_info msm_i2c_gsbi7_mhl_sii9234_info[] =
{
	{
		I2C_BOARD_INFO(MHL_SII9234_I2C_NAME, 0x72 >> 1),
		.platform_data = &mhl_sii9234_device_data,
		.irq = RUBY_GPIO_MHL_INT
	},
};
#endif
#endif

#ifdef CONFIG_LEDS_PM8058
static struct pm8058_led_config pm_led_config[] = {
	{
		.name = "button-backlight",
		.type = PM8058_LED_CURRENT,
		.bank = 3,
		.flags = PM8058_LED_LTU_EN,
		.period_us = USEC_PER_SEC / 1000,
		.start_index = 0,
		.duites_size = 8,
		.duty_time_ms = 32,
		.lut_flag = PM_PWM_LUT_RAMP_UP | PM_PWM_LUT_PAUSE_HI_EN,
		.out_current = 40,
	},
	{
		.name = "amber",
		.type = PM8058_LED_DRVX,
		.bank = 4,
		.flags = PM8058_LED_BLINK_EN,
		.out_current = 6,
	},
	{
		.name = "green",
		.type = PM8058_LED_DRVX,
		.bank = 5,
		.flags = PM8058_LED_BLINK_EN,
		.out_current = 6,
	},
	{
		.name = "blue",
		.type = PM8058_LED_DRVX,
		.bank = 6,
		.flags = PM8058_LED_BLINK_EN,
		.out_current = 3,
	},

};
static struct pm8058_led_platform_data pm8058_leds_data = {
	.led_config = pm_led_config,
	.num_leds = ARRAY_SIZE(pm_led_config),
	.duties = {0, 15, 30, 45, 60, 75, 90, 100,
		   100, 90, 75, 60, 45, 30, 15, 0,
		   0, 0, 4, 12, 20, 28, 36, 44,
		   52, 60, 68, 76, 84, 92, 100, 100,
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

#ifdef CONFIG_SENSORS_PN544
static void config_nfc_gpios(void)
{
	static uint32_t nfc_gpio_table[] = {
		GPIO_CFG(RUBY_GPIO_NFC_VEN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(RUBY_GPIO_NFC_DL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(RUBY_GPIO_NFC_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	};

	config_gpio_table(nfc_gpio_table,
		ARRAY_SIZE(nfc_gpio_table));

	gpio_set_value(RUBY_GPIO_NFC_VEN, 1);
	pr_info("%s\n", __func__);
}

static struct pn544_i2c_platform_data nfc_platform_data = {
	.gpio_init	= config_nfc_gpios,
	.irq_gpio = RUBY_GPIO_NFC_INT,
	.ven_gpio = RUBY_GPIO_NFC_VEN,
	.firm_gpio = RUBY_GPIO_NFC_DL,
	.ven_isinvert = 1,
};

static struct i2c_board_info pn544_i2c_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO(PN544_I2C_NAME, 0x50 >> 1),
		.platform_data = &nfc_platform_data,
		.irq = MSM_GPIO_TO_INT(RUBY_GPIO_NFC_INT),
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

#ifdef CONFIG_MFD_TPS65200
static struct tps65200_platform_data tps65200_data = {
	.gpio_chg_stat = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, RUBY_CHG_STAT),
	.gpio_chg_int  = MSM_GPIO_TO_INT(RUBY_GPIO_CHG_INT),
};

#ifdef CONFIG_SUPPORT_DQ_BATTERY
static int __init check_dq_setup(char *str)
{
	if (!strcmp(str, "PASS"))
		tps65200_data.dq_result = 1;
	else
		tps65200_data.dq_result = 0;

	if (system_rev == 4) /* for XE board*/
		tps65200_data.dq_result = 1;

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
static uint32_t gsbi3_gpio_table[] = {
	GPIO_CFG(RUBY_NFC_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
	GPIO_CFG(RUBY_NFC_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi3_gpio_table_gpio[] = {
	GPIO_CFG(RUBY_NFC_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_NFC_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi4_gpio_table[] = {
	GPIO_CFG(RUBY_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
	GPIO_CFG(RUBY_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi4_gpio_table_gpio[] = {
	GPIO_CFG(RUBY_CAM_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_CAM_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi5_gpio_table[] = {
	GPIO_CFG(RUBY_TP_I2C_SDA, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
	GPIO_CFG(RUBY_TP_I2C_SCL, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi5_gpio_table_gpio[] = {
	GPIO_CFG(RUBY_TP_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_TP_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi7_gpio_table[] = {
	GPIO_CFG(RUBY_GENERAL_I2C_SDA, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
	GPIO_CFG(RUBY_GENERAL_I2C_SCL, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi7_gpio_table_gpio[] = {
	GPIO_CFG(RUBY_GENERAL_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_GENERAL_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi12_gpio_table[] = {
	GPIO_CFG(RUBY_GPIO_SENSOR_I2C_SDA, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
	GPIO_CFG(RUBY_GPIO_SENSOR_I2C_SCL, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t gsbi12_gpio_table_gpio[] = {
	GPIO_CFG(RUBY_GPIO_SENSOR_I2C_SDA, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_GPIO_SENSOR_I2C_SCL, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
	printk(KERN_INFO "%s(): adap_id = %d, config_type = %d \n", __func__, adap_id, config_type);

	if ((adap_id == MSM_GSBI3_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi3_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi3_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI3_QUP_I2C_BUS_ID) && (config_type == 0)) {
		gpio_tlmm_config(gsbi3_gpio_table_gpio[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi3_gpio_table_gpio[1], GPIO_CFG_ENABLE);
	}

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

	if ((adap_id == MSM_GSBI12_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi12_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi12_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM_GSBI12_QUP_I2C_BUS_ID) && (config_type == 0)) {
		gpio_tlmm_config(gsbi12_gpio_table_gpio[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi12_gpio_table_gpio[1], GPIO_CFG_ENABLE);
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
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi7_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi9_qup_i2c_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi12_qup_i2c_pdata = {
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_gpio_config,
};
#endif

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
static int spi_dma_config(void)
{
	return -ENOSYS;
}

static struct msm_spi_platform_data msm_gsbi1_qup_spi_pdata = {
	.max_clock_speed = 24000000,
	.dma_config = spi_dma_config,
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

static unsigned fb_size;
static int __init fb_size_setup(char *p)
{
	fb_size = memparse(p, NULL);
	return 0;
}
early_param("fb_size", fb_size_setup);

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

static void __init msm8x60_allocate_memory_regions(void)
{
        msm8x60_allocate_fb_region();
}

static int ruby_ts_atmel_power(int on)
{
	pr_info("[TP] %s: power %d\n", __func__, on);

	gpio_set_value(PM8058_GPIO_PM_TO_SYS(RUBY_TP_RST), 0);
	msleep(5);
	gpio_set_value(PM8058_GPIO_PM_TO_SYS(RUBY_TP_RST), 1);
	msleep(40);

	return 0;
}

struct atmel_i2c_platform_data ruby_ts_atmel_data[] = {
	{
		.version = 0x0020,
		.abs_x_min = 0,
		.abs_x_max = 1023,
		.abs_y_min = 0,
		.abs_y_max = 1858,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = RUBY_TP_ATT_N,
		.gpio_rst = PM8058_GPIO_PM_TO_SYS(RUBY_TP_RST),
		.power = ruby_ts_atmel_power,
		.unlock_attr = 0,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {9, 0, 5, 5, 0, 0, 5, 25, 5, 192},
		.config_T9 = {139, 0, 0, 19, 11, 0, 16, 40, 3, 5, 10, 5, 5, 0, 4, 10, 10, 0, 255, 7, 0, 0, 2, 2, 18, 24, 184, 35, 150, 70, 20, 12},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T18 = {0, 0},
		.config_T19 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T22 = {15, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 7, 18, 255, 255, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 3, 4, 8, 60},
		.object_crc = {0x57, 0x2D, 0x92},
		.cable_config = {40, 35, 8, 16},
		.GCAF_level = {20, 24, 28, 40, 63},
	},
	{
		.version = 0x0016,
		.abs_x_min = 0,
		.abs_x_max = 1023,
		.abs_y_min = 0,
		.abs_y_max = 1858,
		.abs_pressure_min = 0,
		.abs_pressure_max = 255,
		.abs_width_min = 0,
		.abs_width_max = 20,
		.gpio_irq = RUBY_TP_ATT_N,
		.gpio_rst = PM8058_GPIO_PM_TO_SYS(RUBY_TP_RST),
		.power = ruby_ts_atmel_power,
		.unlock_attr = 0,
		.config_T6 = {0, 0, 0, 0, 0, 0},
		.config_T7 = {50, 15, 25},
		.config_T8 = {9, 0, 5, 5, 0, 0, 5, 25},
		.config_T9 = {139, 0, 0, 19, 11, 0, 16, 40, 3, 5, 10, 5, 5, 0, 4, 10, 10, 0, 255, 7, 0, 0, 2, 2, 18, 24, 184, 35, 150, 70, 20},
		.config_T15 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T18 = {0, 0},
		.config_T19 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T20 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T22 = {15, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 7, 18, 255, 255, 0},
		.config_T23 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T24 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T25 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		.config_T28 = {0, 0, 3, 4, 8, 60},
		.cable_config = {40, 35, 8, 16},
		.GCAF_level = {20, 24, 28, 40, 63},
	},
};

static struct i2c_board_info msm_i2c_gsbi5_info[] = {
	{
		I2C_BOARD_INFO(ATMEL_QT602240_NAME, 0x94 >> 1),
		.platform_data = &ruby_ts_atmel_data,
		.irq = MSM_GPIO_TO_INT(RUBY_TP_ATT_N)
	},
};

static ssize_t ruby_virtual_keys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf,
		__stringify(EV_KEY) ":" __stringify(KEY_HOME)	":62:1015:110:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU)   ":200:1015:106:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_BACK)   ":340:1015:120:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":482:1015:110:100"
		"\n");
}

static struct kobj_attribute ruby_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.atmel-touchscreen",
		.mode = S_IRUGO,
	},
	.show = &ruby_virtual_keys_show,
};

static struct attribute *ruby_properties_attrs[] = {
	&ruby_virtual_keys_attr.attr,
	NULL
};

static struct attribute_group ruby_properties_attr_group = {
	.attrs = ruby_properties_attrs,
};

#ifdef CONFIG_SERIAL_MSM_HS
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.wakeup_irq = MSM_GPIO_TO_INT(RUBY_GPIO_BT_UART1_RX),
	.inject_rx_on_wakeup = 1,
	.rx_to_inject = 0x32,
};
#endif

static struct platform_device ruby_rfkill = {
	.name = "ruby_rfkill",
	.id = -1,
};

static struct platform_device rpm_regulator_early_device __devinitdata = {
	.name	= "rpm-regulator",
	.id	= 0,
	.dev	= {
		.platform_data = &ruby_rpm_regulator_early_pdata,
	},
};

static struct platform_device rpm_regulator_device __devinitdata = {
	.name	= "rpm-regulator",
	.id	= 1,
	.dev	= {
		.platform_data = &ruby_rpm_regulator_pdata,
	},
};

static struct platform_device *early_regulators[] __initdata = {
	&msm_device_saw_s0,
	&msm_device_saw_s1,
	&rpm_regulator_early_device,
};

static struct platform_device *early_devices[] __initdata = {
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

static void ruby_headset_init(void)
{
	ruby_headset_pmic_init();
}

/* HTC_HEADSET_GPIO Driver */
static struct htc_headset_gpio_platform_data htc_headset_gpio_data = {
	.hpin_gpio		= RUBY_GPIO_AUD_HP_DET,
	.key_enable_gpio	= 0,
	.mic_select_gpio	= 0,
};

static struct platform_device htc_headset_gpio = {
	.name	= "HTC_HEADSET_GPIO",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_gpio_data,
	},
};

/* HTC_HEADSET_PMIC Driver */
static struct htc_headset_pmic_platform_data htc_headset_pmic_data = {
	.driver_flag		= 0,
	.hpin_gpio		= 0,
	.hpin_irq		= 0,
	.key_gpio		= 0,
	.key_irq		= 0,
	.key_enable_gpio	= 0,
	.adc_mic_bias		= {0, 0},
	.adc_remote		= {0, 0, 0, 0, 0, 0},
};

static struct platform_device htc_headset_pmic = {
	.name	= "HTC_HEADSET_PMIC",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_pmic_data,
	},
};

/* HTC_HEADSET_MISC Driver */
static struct htc_headset_misc_platform_data htc_headset_misc_data = {
	.driver_flag		= 0,
	.ext_hpin_gpio		= 0,
	.ext_accessory_type	= 0,
};

static struct platform_device htc_headset_misc = {
	.name	= "HTC_HEADSET_MISC",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_misc_data,
	},
};

/* HTC_HEADSET_8X60 Driver */
static struct htc_headset_8x60_platform_data htc_headset_8x60_data = {
	.adc_mpp	= PM8058_MPP_PM_TO_SYS(XOADC_MPP_10),
	.adc_amux	= PM_MPP_AIN_AMUX_CH5,
	.adc_mic_bias	= {HS_DEF_MIC_ADC_15_BIT_MIN,
			   HS_DEF_MIC_ADC_15_BIT_MAX},
	.adc_remote	= {0, 722, 723, 2746, 2747, 6603},
};

static struct platform_device htc_headset_8x60 = {
	.name	= "HTC_HEADSET_8X60",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_8x60_data,
	},
};

/* HTC_HEADSET_MGR Driver */
static struct platform_device *headset_devices[] = {
	&htc_headset_pmic,
	&htc_headset_misc,
	&htc_headset_8x60,
	&htc_headset_gpio,
	/* Please put the headset detection driver on the last */
};

static struct headset_adc_config htc_headset_mgr_config[] = {
	{
		.type = HEADSET_MIC,
		.adc_max = 26837,
		.adc_min = 6073,
	},
	{
		.type = HEADSET_NO_MIC,
		.adc_max = 6072,
		.adc_min = 0,
	},
};

static struct htc_headset_mgr_platform_data htc_headset_mgr_data = {
	.driver_flag		= 0,
	.headset_devices_num	= ARRAY_SIZE(headset_devices),
	.headset_devices	= headset_devices,
	.headset_config_num	= ARRAY_SIZE(htc_headset_mgr_config),
	.headset_config		= htc_headset_mgr_config,
	.headset_init		= 0,
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

static struct platform_device *charm_devices[] __initdata = {
	&msm_charm_modem,
#ifdef CONFIG_MSM_SDIO_AL
	&msm_device_sdio_al,
#endif
};

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
static struct ion_co_heap_pdata co_sf_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
};

static struct ion_cp_heap_pdata cp_mm_ion_pdata = {
	.permission_type = IPT_TYPE_MM_CARVEOUT,
	.align = SZ_64K,
	.request_region = request_smi_region,
	.release_region = release_smi_region,
	.setup_region = setup_smi_region,
};

static struct ion_cp_heap_pdata cp_mfc_ion_pdata = {
	.permission_type = IPT_TYPE_MFC_SHAREDMEM,
	.align = PAGE_SIZE,
};

static struct ion_cp_heap_pdata cp_wb_ion_pdata = {
	.permission_type = IPT_TYPE_MDP_WRITEBACK,
	.align = PAGE_SIZE,
};

static struct ion_co_heap_pdata co_mm_fw_ion_pdata = {
	.adjacent_mem_id = ION_CP_MM_HEAP_ID,
	.align = SZ_128K,
};

static struct ion_co_heap_pdata co_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
};

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
		{
			.id	= ION_CP_MM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MM_HEAP_NAME,
			.size	= MSM_ION_MM_SIZE,
			.base	= MSM_ION_MM_BASE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = &cp_mm_ion_pdata,
		},
		{
			.id	= ION_MM_FIRMWARE_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_MM_FIRMWARE_HEAP_NAME,
			.size	= MSM_ION_MM_FW_SIZE,
			.base	= MSM_ION_MM_FW_BASE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = &co_mm_fw_ion_pdata,
		},
		{
			.id	= ION_CP_MFC_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MFC_HEAP_NAME,
			.size	= MSM_ION_MFC_SIZE,
			.base	= MSM_ION_MFC_BASE,
			.memory_type = ION_SMI_TYPE,
			.extra_data = &cp_mfc_ion_pdata,
		},
		{
			.id	= ION_SF_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_SF_HEAP_NAME,
			.size	= MSM_ION_SF_SIZE,
			.base	= MSM_ION_SF_BASE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = &co_sf_ion_pdata,
		},
		{
			.id	= ION_CAMERA_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_CAMERA_HEAP_NAME,
			.size	= MSM_ION_CAMERA_SIZE,
			.base	= MSM_ION_CAMERA_BASE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = &co_ion_pdata,
		},
		{
			.id	= ION_CP_WB_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_WB_HEAP_NAME,
			.size	= MSM_ION_WB_SIZE,
			.base	= MSM_ION_WB_BASE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = &cp_wb_ion_pdata,
		},
		{
			.id	= ION_AUDIO_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_AUDIO_HEAP_NAME,
			.size	= MSM_ION_AUDIO_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = &co_ion_pdata,
		},
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

static struct platform_device *ruby_devices[] __initdata = {
	&msm8x60_device_acpuclk,
	&msm_device_smd,
        &msm_device_uart_dm11,
	&msm_device_otg,
	&msm_device_gadget_peripheral,
        &msm_device_hsusb_host,
	&android_usb_device,
	&msm_pil_q6v3,
	&msm_pil_modem,
	&msm_pil_tzapps,
	&msm_pil_vidc,
#ifdef CONFIG_QSEECOM
	&qseecom_device,
#endif
#ifdef CONFIG_I2C_QUP
	&msm_gsbi3_qup_i2c_device,
	&msm_gsbi4_qup_i2c_device,
	&msm_gsbi5_qup_i2c_device,
	&msm_gsbi7_qup_i2c_device,
	&msm_gsbi12_qup_i2c_device,
#endif
#ifdef CONFIG_SERIAL_MSM_HS
        &msm_device_uart_dm1,
#endif
	&ruby_rfkill,

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

#ifdef CONFIG_HTC_BATT_8x60
	&htc_battery_pdev,
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
	&msm8660_pm_8x60,
};

static struct memtype_reserve msm8x60_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
		.start	=	MSM_SMI_BASE,
		.limit	=	MSM_SMI_SIZE,
		.flags	=	MEMTYPE_FLAGS_FIXED,
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

static void __init reserve_ion_memory(void)
{
#ifdef CONFIG_ION_MSM
	unsigned int i;
	int ret;

	ret = memblock_remove(MSM_ION_SF_BASE, MSM_ION_SF_SIZE);
	BUG_ON(ret);

	for (i = 0; i < ion_pdata.nr; ++i) {
		struct ion_platform_heap *heap = &(ion_pdata.heaps[i]);
		if(heap->base == 0) {
			switch(heap->memory_type) {
			case ION_SMI_TYPE:
				msm8x60_reserve_table[MEMTYPE_SMI].size += heap->size;
				break;
			case ION_EBI_TYPE:
				msm8x60_reserve_table[MEMTYPE_EBI1].size += heap->size;
				break;
			}
		}
	}
#endif
}

static void __init reserve_mdp_memory(void);

static void __init msm8x60_calculate_reserve_sizes(void)
{
	reserve_ion_memory();
	reserve_mdp_memory();
}

static int msm8x60_paddr_to_memtype(unsigned int paddr)
{
	if (paddr >= 0x40000000 && paddr < 0x70000000)
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

static void __init ruby_early_memory(void)
{
	reserve_info = &msm8x60_reserve_info;
}

static void __init ruby_reserve(void)
{
	msm_reserve();
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
		/*
		 * Vote for the A1 clock to be in pin control mode before
		* the external images are loaded.
		*/
		a1 = msm_xo_get(MSM_XO_TCXO_A1, "mdm");
		BUG_ON(!a1);
		msm_xo_mode_vote(a1, MSM_XO_MODE_PIN_CTRL);
	}
}

#define PM_GPIO_CDC_RST_N 20
#define GPIO_CDC_RST_N PM8058_GPIO_PM_TO_SYS(PM_GPIO_CDC_RST_N)

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

#ifdef CONFIG_MSM8X60_AUDIO
static uint32_t msm_spi_gpio[] = {
	GPIO_CFG(RUBY_SPI_DO,  1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_SPI_DI,  1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_SPI_CS,  1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(RUBY_SPI_CLK, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t auxpcm_gpio_table[] = {
	GPIO_CFG(111, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(112, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
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
	.gpio_tpa2051_spk_en = RUBY_AUD_HANDSET_ENO,
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

void __init ruby_audio_init(void);

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

static struct regulator *pm8058_l10_2v85;
static struct regulator *pm8058_l8_2v85;
static struct regulator *pm8058_l11_1v8;

static int g_sensors_reset(void)
{
	int rc = 0;

	if (system_rev >= 3) {
		pm8058_l10_2v85 = regulator_get(NULL, "8058_l10");
		if (IS_ERR(pm8058_l10_2v85)) {
			printk(KERN_ERR "Get pm8058_l10_2v85 fail\n");
			return PTR_ERR(pm8058_l10_2v85);
		}
	} else {
		pm8058_l8_2v85 = regulator_get(NULL, "8058_l8");
		if (IS_ERR(pm8058_l8_2v85)) {
			printk(KERN_ERR "Get pm8058_l8_2v85 fail\n");
			return PTR_ERR(pm8058_l8_2v85);
		}
	}

	pm8058_l11_1v8 = regulator_get(NULL, "8058_l11");
	if (IS_ERR(pm8058_l11_1v8)) {
		printk(KERN_ERR "Get pm8058_l11_1v8 fail\n");
		return PTR_ERR(pm8058_l11_1v8);
	}

	if (system_rev >= 3) {
		rc = regulator_set_voltage(pm8058_l10_2v85, 2850000, 2850000);
		if (rc) {
			pr_err("[GSNR]%s: unable to set %s voltage to %d"
				" rc:%d\n", __func__, "pm8058_l10",
				2850000, rc);
		}
	} else {
		rc = regulator_set_voltage(pm8058_l8_2v85, 2850000, 2850000);
		if (rc) {
			pr_err("[GSNR]%s: unable to set %s voltage to %d"
				" rc:%d\n", __func__, "pm8058_l8",
				2850000, rc);
		}
	}

	rc = regulator_set_voltage(pm8058_l11_1v8, 1800000, 1800000);
	if (rc) {
		pr_err("[GSNR]%s: unable to set %s voltage to %d"
			" rc:%d\n", __func__, "pm8058_l11",
			1800000, rc);
	}

	if (system_rev >= 3) {
		rc = regulator_enable(pm8058_l10_2v85);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"pm8058_l10", rc);
		}
	} else {
		rc = regulator_enable(pm8058_l8_2v85);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"pm8058_l8", rc);
		}
	}
	udelay(100);
	rc = regulator_enable(pm8058_l11_1v8);
	if (rc) {
		pr_err("'%s' regulator enable failed, rc=%d\n",
			"pm8058_l11", rc);
	}

	mdelay(5);

	rc = regulator_disable(pm8058_l11_1v8);
	if (rc) {
		pr_err("'%s' regulator disable failed, rc=%d\n",
			"pm8058_l11", rc);
	}
	udelay(100);
	if (system_rev >= 3) {
		rc = regulator_disable(pm8058_l10_2v85);
		if (rc) {
			pr_err("'%s' regulator disable failed, rc=%d\n",
				"pm8058_l10", rc);
		}
	} else {
		rc = regulator_disable(pm8058_l8_2v85);
		if (rc) {
			pr_err("'%s' regulator disable failed, rc=%d\n",
				"pm8058_l8", rc);
		}
	}

	mdelay(10);

	if (system_rev >= 3) {
		rc = regulator_enable(pm8058_l10_2v85);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"pm8058_l10", rc);
		}
	} else {
		rc = regulator_enable(pm8058_l8_2v85);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"pm8058_l8", rc);
		}
	}
	udelay(100);
	rc = regulator_enable(pm8058_l11_1v8);
	if (rc) {
		pr_err("'%s' regulator enable failed, rc=%d\n",
			"pm8058_l11", rc);
	}

	mdelay(1);

	printk(KERN_DEBUG "Sensors reset success!!\n");

	return 0;
}

static int capella_cm3628_repower(uint8_t enable)
{
	int rc = 0;
	rc  = g_sensors_reset();
	return rc;
}

static struct cm3628_platform_data cm3628_PVT_pdata = {
	.intr = PM8058_GPIO_PM_TO_SYS(RUBY_PSENSOR_PVT_INTz),
	.levels = {1, 3, 27, 57, 77, 1012, 1752, 1835, 1920, 65535},
	.golden_adc = 0x535,
	.power = NULL,
	.re_power = capella_cm3628_repower,
	.ALS_slave_address = 0xC0>>1,
	.PS_slave_address = 0xC2>>1,
	.check_interrupt_add = 0x2C>>1,
	.is_cmd = CM3628_ALS_IT_400ms | CM3628_ALS_PERS_2,
	.ps_thd_set = 0x4,
	.ps_conf2_val = 0,
	.ps_calibration_rule = 1,
	.ps_conf1_val = CM3628_PS_DR_1_320 | CM3628_PS_IT_1_3T,
	.ps_thd_no_cal = 0x16,
	.ps_thd_with_cal = 0x4,
	.ps_adc_offset = 0x3,
};


static struct i2c_board_info i2c_CM3628_PVT_devices[] = {
	{
		I2C_BOARD_INFO(CM3628_I2C_NAME, 0xC0 >> 1),
		.platform_data = &cm3628_PVT_pdata,
		.irq = PM8058_GPIO_IRQ(PM8058_IRQ_BASE, RUBY_PSENSOR_PVT_INTz),
	},
};

static struct cm3628_platform_data cm3628_pdata = {
	.intr = RUBY_PSENSOR_INTz,
	.levels = {1, 3, 27, 57, 77, 1012, 1752, 1835, 1920, 65535},
	.golden_adc = 0x535,
	.power = NULL,
	.re_power = capella_cm3628_repower,
	.ALS_slave_address = 0xC0>>1,
	.PS_slave_address = 0xC2>>1,
	.check_interrupt_add = 0x2C>>1,
	.is_cmd = CM3628_ALS_IT_400ms | CM3628_ALS_PERS_2,
	.ps_thd_set = 0x4,
	.ps_conf2_val = 0,
	.ps_calibration_rule = 1,
	.ps_conf1_val = CM3628_PS_DR_1_320 | CM3628_PS_IT_1_3T,
	.ps_thd_no_cal = 0x16,
	.ps_thd_with_cal = 0x4,
	.ps_adc_offset = 0x3,
};


static struct i2c_board_info i2c_CM3628_devices[] = {
	{
		I2C_BOARD_INFO(CM3628_I2C_NAME, 0xC0 >> 1),
		.platform_data = &cm3628_pdata,
		.irq = MSM_GPIO_TO_INT(RUBY_PSENSOR_INTz),
	},
};

static uint32_t gyro_ID_PIN_input_table[] = {
	GPIO_CFG(RUBY_GPIO_GYRO_ID, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static uint32_t gyro_DIAG_PIN_pull_down[] = {
	GPIO_CFG(RUBY_GPIO_GYRO_DIAG, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static uint32_t gyro_DIAG_PIN_no_pull[] = {
	GPIO_CFG(RUBY_GPIO_GYRO_DIAG, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

void config_ruby_gyro_diag_gpios(bool pulldown)
{
	if (pulldown) {
		config_gpio_table(gyro_DIAG_PIN_pull_down, ARRAY_SIZE(gyro_DIAG_PIN_pull_down));
		printk(KERN_INFO "%s %d pull down\n",  __func__, RUBY_GPIO_GYRO_DIAG);
	} else {
		config_gpio_table(gyro_DIAG_PIN_no_pull, ARRAY_SIZE(gyro_DIAG_PIN_no_pull));
		printk(KERN_INFO "%s %d input none pull\n",  __func__, RUBY_GPIO_GYRO_DIAG);
	}
}

static struct pana_gyro_platform_data pana_gyro_pdata = {
	.acc_dir = 0x06,
	.acc_polarity = 0x07,
	.gyro_dir = 0x06,
	.gyro_polarity = 0x07,
	.mag_dir = 0x06,
	.mag_polarity = 0x07,
	.sleep_pin = RUBY_GPIO_PANA_GYRO_SLEEP,
	.config_gyro_diag_gpios = config_ruby_gyro_diag_gpios,
};
static struct i2c_board_info __initdata pana_gyro_GSBI12_boardinfo[] = {
	{
		I2C_BOARD_INFO("ewtzmu2", 0xD2 >> 1),
		.irq = MSM_GPIO_TO_INT(RUBY_GPIO_GYRO_INT),
		.platform_data = &pana_gyro_pdata,
	},
};

static struct bma250_platform_data gsensor_bma250_platform_data = {
	.intr = RUBY_GPIO_GSENSOR_INT_N,
	.chip_layout = 1,
};

static struct i2c_board_info i2c_bma250_devices[] = {
	{
		I2C_BOARD_INFO(BMA250_I2C_NAME, 0x30 >> 1),
		.platform_data = &gsensor_bma250_platform_data,
		.irq = MSM_GPIO_TO_INT(RUBY_GPIO_GSENSOR_INT_N),
	},
};

static struct akm8975_platform_data compass_platform_data = {
	.layouts = RUBY_LAYOUTS,
	.use_pana_gyro = 1,
};

static struct i2c_board_info i2c_akm8975_devices[] = {
	{
		I2C_BOARD_INFO(AKM8975_I2C_NAME, 0x1A >> 1),
		.platform_data = &compass_platform_data,
		.irq = MSM_GPIO_TO_INT(RUBY_GPIO_COMPASS_INT),
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
		.adapt_num = MSM_GSBI12_QUP_I2C_BUS_ID, /* The i2c bus to which the mpu device is connected */
		.bus = EXT_SLAVE_BUS_SECONDARY,
		.address = 0x30 >> 1,
			.orientation = { 1, 0, 0,
					0, 1, 0,
					0, 0, 1 },
	},

	.compass = {
		.get_slave_descr = get_compass_slave_descr,
		.adapt_num = MSM_GSBI12_QUP_I2C_BUS_ID, /* The i2c bus to which the mpu device is connected */
		.bus = EXT_SLAVE_BUS_PRIMARY,
		.address = 0x1A >> 1,
			.orientation = { 1, 0, 0,
					0, 1, 0,
					0, 0, 1 },
	},
};

static struct i2c_board_info __initdata mpu3050_GSBI12_boardinfo[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0xD0 >> 1),
		.irq = MSM_GPIO_TO_INT(RUBY_GPIO_GYRO_INT),
		.platform_data = &mpu3050_data,
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
#ifdef CONFIG_PMIC8058
	{
		I2C_SURF | I2C_FFA,
		MSM_SSBI1_I2C_BUS_ID,
		pm8058_boardinfo,
		ARRAY_SIZE(pm8058_boardinfo),
	},
#endif
#ifdef CONFIG_PMIC8901
	{
		I2C_SURF | I2C_FFA,
		MSM_SSBI2_I2C_BUS_ID,
		pm8901_boardinfo,
		ARRAY_SIZE(pm8901_boardinfo),
	},
#endif
#endif /* !CONFIG_MSM_SSBI */
#ifdef CONFIG_MFD_TPS65200
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_tps_65200_boardinfo,
		ARRAY_SIZE(msm_tps_65200_boardinfo),
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
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_timpani_info,
		ARRAY_SIZE(msm_i2c_gsbi7_timpani_info),
	},
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_tpa2051d3_info,
		ARRAY_SIZE(msm_i2c_gsbi7_tpa2051d3_info),
	},
#ifdef CONFIG_SENSORS_PN544
	{
		I2C_SURF | I2C_FFA,
		MSM_GSBI3_QUP_I2C_BUS_ID,
		pn544_i2c_boardinfo,
		ARRAY_SIZE(pn544_i2c_boardinfo),
	},
#endif
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
		if (gpio_get_value(RUBY_GPIO_GYRO_ID) == 1) {
			pr_info("use invensens GYRO\n");
			i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				mpu3050_GSBI12_boardinfo, ARRAY_SIZE(mpu3050_GSBI12_boardinfo));
			config_ruby_gyro_diag_gpios(1);
			printk(KERN_INFO "%s , set gyro DIAG %d input  pull down \n",  __func__, RUBY_GPIO_GYRO_DIAG);
		} else {
			pr_info("use Pana GYRO, RUBY_GPIO_GYRO_DIAG %d\n", gpio_get_value(RUBY_GPIO_GYRO_DIAG));
			i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				pana_gyro_GSBI12_boardinfo, ARRAY_SIZE(pana_gyro_GSBI12_boardinfo));
			i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				i2c_bma250_devices, ARRAY_SIZE(i2c_bma250_devices));
			i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				i2c_akm8975_devices, ARRAY_SIZE(i2c_akm8975_devices));
		}
	} else {
		pr_info("use invensens GYRO in XA\n");
		i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				mpu3050_GSBI12_boardinfo, ARRAY_SIZE(mpu3050_GSBI12_boardinfo));
		config_ruby_gyro_diag_gpios(1);
		printk(KERN_INFO "%s , set gyro DIAG %d input  pull down \n",  __func__, RUBY_GPIO_GYRO_DIAG);
	}
	if (system_rev > 4) {
		pr_info("PVT for cm3628 \n");
		i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				i2c_CM3628_PVT_devices, ARRAY_SIZE(i2c_CM3628_PVT_devices));
	} else {
		pr_info("not PVT for cm3628 \n");
		i2c_register_board_info(MSM_GSBI12_QUP_I2C_BUS_ID,
				i2c_CM3628_devices, ARRAY_SIZE(i2c_CM3628_devices));
	}

	config_gpio_table(gyro_ID_PIN_input_table, ARRAY_SIZE(gyro_ID_PIN_input_table));
	printk(KERN_INFO "%s , set gyro ID %d input  pull down after detct gyro compeletly\n",  __func__, RUBY_GPIO_GYRO_ID);
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
	msm_gsbi12_qup_i2c_device.dev.platform_data = &msm_gsbi12_qup_i2c_pdata;
#endif
#ifdef CONFIG_MSM_GSBI9_UART
	/* Setting protocol code to 0x60 for dual UART/I2C in GSBI9 */
	gsbi_mem = ioremap_nocache(MSM_GSBI9_PHYS, 4);
	writel_relaxed(GSBI_DUAL_MODE_CODE, gsbi_mem);
	iounmap(gsbi_mem);
	msm_gsbi9_qup_i2c_pdata.use_gsbi_shared_mode = 1;
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

#ifdef CONFIG_SERIAL_MSM_HS
	msm_device_uart_dm1.name = "msm_serial_hs_ti_dc"; /* for ti dc*/
	msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif
#ifdef CONFIG_MSM_GSBI9_UART
	msm_device_uart_gsbi9 = msm_add_gsbi9_uart();
	if (IS_ERR(msm_device_uart_gsbi9))
		pr_err("%s(): Failed to create uart gsbi9 device\n", __func__);
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

static void __init ruby_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_msm8x60_io();

	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");
}

static void __init reserve_mdp_memory(void)
{
        msm8x60_mdp_writeback(msm8x60_reserve_table);
}

static void __init msm8x60_gfx_init(void)
{
	msm8x60_check_2d_hardware();
	platform_device_register(&msm_kgsl_3d0);
	platform_device_register(&msm_kgsl_2d0);
	platform_device_register(&msm_kgsl_2d1);
}

int __initdata irq_ignore_tbl[] =
{
	MSM_GPIO_TO_INT(133),
	MSM_GPIO_TO_INT(134),
};
unsigned __initdata irq_num_ignore_tbl = ARRAY_SIZE(irq_ignore_tbl);

extern int msm8x60_multi_sdio_init(void);

static void __init ruby_init(void)
{
        int rc;
	uint32_t soc_platform_version;
	struct kobject *properties_kobj;
	struct regulator *margin_power;

	if (meminfo_init(SYS_MEMORY, SZ_256M) < 0)
		pr_err("meminfo_init() failed!\n");

	htc_add_ramconsole_devices();
	platform_device_register(&msm_gpio_device);

	BUG_ON(msm_rpm_init(&msm8660_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(&msm_rpmrs_data));

	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");

	regulator_suppress_info_printing();

	platform_add_devices(early_regulators, ARRAY_SIZE(early_regulators));

	clk_ignor_list_add("msm_serial_hsl.3", "core_clk", &msm8x60_clock_init_data);
	
	platform_device_register(&rpm_regulator_device);

	msm_clock_init(&msm8x60_clock_init_data);

        ruby_init_gpiomux();
	ruby_init_pmic();

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
	ruby_init_mmc();

	msm8x60_cfg_smsc911x();
#ifdef CONFIG_MSM_CAMERA
        ruby_init_cam();
#endif

	/* Accessory */
	pr_info("[HS_BOARD] (%s) MFG_BUILD=%d, FLAG=%lu\n", __func__,
		board_build_flag(), get_kernel_flag());
	if (board_build_flag() != MFG_BUILD && !(get_kernel_flag() &
	    KERNEL_FLAG_SERIAL_HSL_ENABLE)) {
		pr_info("[HS_BOARD] (%s) Enable MHL audio jack\n", __func__);
		htc_headset_misc_data.driver_flag = DRIVER_HS_MISC_EXT_HP_DET;
		htc_headset_misc_data.ext_hpin_gpio =
			PM8058_GPIO_PM_TO_SYS(RUBY_H2W_CABLE_IN1);
		htc_headset_misc_data.ext_accessory_type = USB_AUDIO_OUT;
		htc_headset_mgr_data.headset_init = ruby_headset_init;
	}

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 1)
		platform_add_devices(msm8660_footswitch,
				     msm8660_num_footswitch);

	platform_add_devices(ruby_devices,
			     ARRAY_SIZE(ruby_devices));

        ruby_gpio_mpp_init();

	platform_device_register(&msm8x60_8901_mpp_vreg);

        ruby_init_fb();

#ifdef CONFIG_USB_EHCI_MSM_72K
	msm_add_host(0, &msm_usb_host_pdata);
#endif
	
	properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (properties_kobj) {
          rc = sysfs_create_group(properties_kobj, &ruby_properties_attr_group);
	}

	platform_add_devices(asoc_devices,
			ARRAY_SIZE(asoc_devices));

	platform_add_devices(charm_devices, ARRAY_SIZE(charm_devices));

#if defined(CONFIG_SPI_QUP) || defined(CONFIG_SPI_QUP_MODULE)
        platform_device_register(&msm_gsbi1_qup_spi_device);
#endif

	register_i2c_devices();

	platform_device_register(&smsc911x_device);

	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));

#ifdef CONFIG_MSM8X60_AUDIO
	spi_register_board_info(msm_spi_board_info, ARRAY_SIZE(msm_spi_board_info));
	gpio_tlmm_config(msm_spi_gpio[0], GPIO_CFG_DISABLE);
	gpio_tlmm_config(msm_spi_gpio[1], GPIO_CFG_DISABLE);
	gpio_tlmm_config(msm_spi_gpio[2], GPIO_CFG_DISABLE);
	gpio_tlmm_config(msm_spi_gpio[3], GPIO_CFG_DISABLE);
	msm_auxpcm_init(); 
	msm_snddev_init();
	ruby_audio_init(); 
#endif

        ruby_init_keypad();
        ruby_wifi_init();
        headset_device_register();

	msm8x60_multi_sdio_init();
	msm_fusion_setup_pinctrl();
        msm_mpm_set_irq_ignore_list(irq_ignore_tbl, irq_num_ignore_tbl);

	/* change S4B to 1.25v, L22A to 1.2v for DDR stability issue */
	margin_power = regulator_get(NULL, "8901_s4");
	regulator_set_voltage(margin_power, 1250000, 1250000);
	regulator_enable(margin_power);
	regulator_put(margin_power);
	margin_power = regulator_get(NULL, "8058_l22");
	regulator_set_voltage(margin_power, 1200000, 1200000);
	regulator_enable(margin_power);
	regulator_put(margin_power);

        printk(KERN_ERR "%s: --\n", __func__);
}

int __init parse_tag_memsize(const struct tag *tags);

static void __init ruby_fixup(struct tag *tags,
		char **cmdline, struct meminfo *mi)
{
	mem_size_mb = parse_tag_memsize((const struct tag *)tags);
	printk(KERN_DEBUG "%s: mem_size_mb=%u\n", __func__, mem_size_mb);

	engineerid = parse_tag_engineerid(tags);
	mi->nr_banks = 1;
	mi->bank[0].start = PHY_BASE_ADDR1;
	mi->bank[0].size = SIZE_ADDR1;
}

MACHINE_START(RUBY, "ruby")	
        .fixup = ruby_fixup,
	.map_io = ruby_map_io,
	.reserve = ruby_reserve,
	.init_irq = msm8x60_init_irq,
	.handle_irq = gic_handle_irq,
	.init_machine = ruby_init,
	.timer = &msm_timer,
	.init_early = msm8x60_allocate_memory_regions,
	.init_very_early = ruby_early_memory,
	.restart = msm_restart,
MACHINE_END

