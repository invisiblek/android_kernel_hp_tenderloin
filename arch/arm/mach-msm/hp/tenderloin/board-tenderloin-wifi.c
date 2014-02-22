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
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include <mach/sdio_al.h>
#include <mach/rpm.h>
#include <mach/rpm-regulator.h>
#include <mach/restart.h>
#include <mach/cable_detect.h>
#include <mach/board-msm8660.h>
#include <mach/iommu_domains.h>

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
//#include "board-storage-common-a.h"
#include "acpuclock.h"

#include <linux/msm_ion.h>
#include <mach/ion.h>
#include <mach/msm_rtb.h>

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

/*WLAN*/

#define WLAN_1V8_SDIO_ACT_LOAD  80000
#define WLAN_1V8_SDIO_MIN_LOAD  10000

static int tenderloin_wifi_power(int on)
{
	static struct regulator * wifi_S3A_1V8  = NULL;
	static struct regulator * wifi_L1B_3V3  = NULL;
	static struct regulator * wifi_L3B_3V3  = NULL;
	static struct regulator * wifi_L19A_1V8 = NULL;
	static int wifi_is_on = 0;
	static int rc = -EINVAL; /* remember if the gpio_requests succeeded */
	int gpios[] = {
			TENDERLOIN_GPIO_WL_HOST_WAKE,
			pin_table[TENDERLOIN_GPIO_HOST_WAKE_WL_PIN],
			pin_table[TENDERLOIN_GPIO_WLAN_RST_N_PIN]
		};

	// "8901_l3"  - 3.3V - VREG_L3B_3V3      - WLAN_PA_3V3
	// "8901_l1"  - 3.3V - VREG_L1B_3V3      - VDD_WLAN_3V3
	// "8058_l19" - 1.8V - VREG_L19A_1V8     - VDD_1.8
	// "8058_s3"  - 1.8V - VREG_S3A_1V8_WLAN - DVDD_SDIO_1V8 etc.

	if (!wifi_S3A_1V8) {
		// Always on
		wifi_S3A_1V8 = regulator_get(NULL, "8058_s3");
		if (IS_ERR(wifi_S3A_1V8)) {
			pr_err("%s: unable to get %s\n",
				__func__, "8058_s3");
		}
		if(regulator_set_voltage(wifi_S3A_1V8, 1800000, 1800000)){
			pr_err("%s: unable to set voltage for %s\n",
				__func__, "8058_s3");
		}
		if (regulator_enable(wifi_S3A_1V8)){
			pr_err("%s: Unable to enable %s\n",
				 __func__, "8058_s3");
		}
	}

	if (!wifi_L3B_3V3) {
		wifi_L3B_3V3 = regulator_get(NULL, "8901_l3");
		if (IS_ERR(wifi_L3B_3V3)) {
			pr_err("%s: unable to get %s\n",
				__func__, "8901_l3");
		}
		if (regulator_set_voltage(wifi_L3B_3V3, 3300000, 3300000)) {
			pr_err("%s: unable to set voltage for %s\n",
				__func__, "8901_l3");
		}
	}

	if (!wifi_L1B_3V3) {
		wifi_L1B_3V3 = regulator_get(NULL, "8901_l1");
		if (IS_ERR(wifi_L1B_3V3)) {
			pr_err("%s: unable to get %s\n",
				__func__, "8901_l1");
		}
		if (regulator_set_voltage(wifi_L1B_3V3, 3300000, 3300000)) {
			pr_err("%s: unable to set voltage for %s\n",
				__func__, "8901_l3");
		}
	}

	if (!wifi_L19A_1V8) {
		wifi_L19A_1V8 = regulator_get(NULL, "8058_l19");
		if (IS_ERR(wifi_L19A_1V8)) {
			pr_err("%s: unable to get %s\n",
				__func__, "8058_l19");
		}
		if (regulator_set_voltage(wifi_L19A_1V8, 1800000, 1800000)) {
			pr_err("%s: unable to set voltage for %s\n",
				__func__, "8058_l19");
		}
	}

	BUG_ON(!wifi_L3B_3V3);
	BUG_ON(!wifi_L1B_3V3);
	BUG_ON(!wifi_L19A_1V8);
	BUG_ON(!wifi_S3A_1V8);

	if (on && !wifi_is_on) {
		//
		// B. enable power
		// 3.3V -> 1.8V -> wait (Tb 5) -> CHIP_PWD

		pr_info("wifi_power(%d) 1.8V sdio: set load\n", on);
		rc = regulator_set_optimum_mode(wifi_S3A_1V8,
			WLAN_1V8_SDIO_ACT_LOAD);
		if (rc < 0) {
			pr_err("%s: Unable (%d) to set opt load for %s\n",
				 __func__, rc, "8058_s3");
		} else {
			pr_info("%s: New regulator mode for %s: %d\n",
				 __func__, "8058_s3", rc );
		}

		pr_info("wifi_power(%d) 3.3V\n", on);
		rc = regulator_enable(wifi_L3B_3V3);
		if (rc) {
			pr_err("%s: Unable (%d) to enable %s\n",
				 __func__, rc, "8901_l3");
		}

		pr_info("wifi_power(%d) 8901_l1 3.3V\n", on);
		rc = regulator_enable(wifi_L1B_3V3);
		if (rc) {
			pr_err("%s: Unable (%d) to enable %s\n",
				 __func__, rc, "8901_l1");
		}

		pr_info("wifi_power(%d) 8058_l19 1.8V\n", on);
		rc = regulator_enable(wifi_L19A_1V8);
		if (rc) {
			pr_err("%s: Unable (%d) to enable %s\n",
				 __func__, rc, "8058_l19");
		}

		printk("wifi_power(%d) CHIP_PWD\n", on);
		mdelay(5);

		/* request gpio if not yet done */
		//printk("wifi_is_on(%d)\n", wifi_is_on);
		if (!wifi_is_on) {
			rc = configure_gpios(on, gpios, ARRAY_SIZE(gpios));
			if (rc) {
				pr_err("%s: gpio request failed\n", __func__);
				return -EINVAL;
			}
		}

		gpio_direction_output(pin_table[TENDERLOIN_GPIO_WLAN_RST_N_PIN], 0);
		mdelay(5);
		gpio_direction_output(pin_table[TENDERLOIN_GPIO_WLAN_RST_N_PIN], 1);
	}
	else if (!on && wifi_is_on)
	{
		//CHIP_PWD -> wait (Tc 5)
		gpio_direction_output(pin_table[TENDERLOIN_GPIO_WLAN_RST_N_PIN], 0);
		mdelay(5);

		if (wifi_is_on) {
			rc = configure_gpios(on, gpios, ARRAY_SIZE(gpios));
			if (rc)
				pr_err("%s: failed to configure gpios on=%d rc=%d\n",
						__func__, on, rc);
		}

		rc = regulator_disable(wifi_L3B_3V3);
		if (rc) {
			pr_err("%s: Unable (%d) to disable %s\n",
				__func__, rc, "8901_l3");
		}

		rc = regulator_disable(wifi_L1B_3V3);
		if (rc) {
			pr_err("%s: Unable (%d) to disable %s\n",
				__func__, rc, "8901_l1");
		}

		rc = regulator_disable(wifi_L19A_1V8);
		if (rc) {
			pr_err("%s: Unable (%d) to disable %s\n",
				__func__, rc, "8058_l19");
		}

		rc = regulator_set_optimum_mode(wifi_S3A_1V8,
			WLAN_1V8_SDIO_MIN_LOAD);
		if (rc < 0) {
			pr_err("%s: Unable (%d) to set opt load for %s\n",
				 __func__, rc, "8058_s3");
		} else {
			pr_info("%s: New regulator mode for %s: %d\n",
				 __func__, "8058_s3", rc );
		}
	}

	wifi_is_on = on;
	return 0;
}

static struct mmc_host *wifi_mmc;
int board_sdio_wifi_enable(unsigned int param);
int board_sdio_wifi_disable(unsigned int param);

void tenderloin_probe_wifi(int id, struct mmc_host *mmc)
{
	printk("%s: id %d mmc %p\n", __PRETTY_FUNCTION__, id, mmc);
	wifi_mmc = mmc;

       //TODO: hook up to PM later
	board_sdio_wifi_enable(0);
}

void tenderloin_remove_wifi(int id, struct mmc_host *mmc)
{
	printk("%s: id %d mmc %p\n", __PRETTY_FUNCTION__, id, mmc);
	wifi_mmc = NULL;

       //TODO: hook up to PM later
	board_sdio_wifi_disable(0);
}

/*
 *  An API to enable wifi
 */
int board_sdio_wifi_enable(unsigned int param)
{
	printk(KERN_ERR "board_sdio_wifi_enable\n");

	tenderloin_wifi_power(1);
	if (wifi_mmc) {
		mmc_detect_change(wifi_mmc, msecs_to_jiffies(250));
	}

	return 0;
}
EXPORT_SYMBOL(board_sdio_wifi_enable);

/*
 *  An API to disable wifi
 */
int board_sdio_wifi_disable(unsigned int param)
{
	printk(KERN_ERR "board_sdio_wifi_disable\n");

	tenderloin_wifi_power(0);

	if (wifi_mmc) {
		mmc_detect_change(wifi_mmc, msecs_to_jiffies(100));
	}

	return 0;
}
EXPORT_SYMBOL(board_sdio_wifi_disable);

int board_get_wow_pin(void)
{
	return pin_table[TENDERLOIN_GPIO_HOST_WAKE_WL_PIN];
}
EXPORT_SYMBOL(board_get_wow_pin);

