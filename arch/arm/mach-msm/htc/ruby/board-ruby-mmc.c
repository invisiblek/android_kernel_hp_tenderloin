/* linux/arch/arm/mach-msm/board-pyramid-mmc.c
 *
 * Copyright (C) 2008 HTC Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/err.h>
#include <linux/debugfs.h>
#include <linux/gpio.h>
#include <linux/module.h>

#include <asm/gpio.h>
#include <asm/io.h>

#include <mach/vreg.h>
#include <mach/htc_pwrsink.h>

#include <asm/mach/mmc.h>
#include <mach/msm_iomap.h>
#include <linux/mfd/pmic8058.h>

#include "devices.h"
#include "board-ruby.h"
#include <mach/proc_comm.h>
#include "board-common-wimax.h"
#include <mach/mpm.h>

#ifdef CONFIG_WIMAX_SERIAL_MSM
#include <mach/msm_serial_wimax.h>
#include <linux/irq.h>

#define MSM_GSBI3_PHYS		0x16200000
#define MSM_UART3_PHYS 		(MSM_GSBI3_PHYS + 0x40000)
#define INT_UART3_IRQ		GSBI3_UARTDM_IRQ
#endif

#include <linux/irq.h>

#include <mach/rpm.h>
#include <mach/rpm-regulator.h>

#include "rpm_resources.h"

int msm_proc_comm(unsigned cmd, unsigned *data1, unsigned *data2);

#define PM8058_GPIO_BASE			NR_MSM_GPIOS
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_GPIO_BASE)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)		(sys_gpio - PM8058_GPIO_BASE)

static uint32_t wifi_on_gpio_table[] = {
	GPIO_CFG(RUBY_GPIO_WIFI_IRQ, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA), /* WLAN IRQ */
};

static uint32_t wifi_off_gpio_table[] = {
	GPIO_CFG(RUBY_GPIO_WIFI_IRQ, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_4MA), /* WLAN IRQ */
};

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static struct embedded_sdio_data ruby_wifi_emb_data = {
	.cccr   = {
		.multi_block	= 1,
		.low_speed	= 0,
		.wide_bus	= 1,
		.high_power	= 0,
		.high_speed	= 0,
		.disable_cd	= 1,
	},
};

static void (*wifi_status_cb)(int card_present, void *dev_id);
static void *wifi_status_cb_devid;

static int
ruby_wifi_status_register(void (*callback)(int card_present, void *dev_id),
				void *dev_id)
{
	if (wifi_status_cb)
		return -EAGAIN;

	wifi_status_cb = callback;
	wifi_status_cb_devid = dev_id;
	return 0;
}

static int ruby_wifi_cd;	/* WiFi virtual 'card detect' status */

static unsigned int ruby_wifi_status(struct device *dev)
{
	return ruby_wifi_cd;
}

void ruby_remove_wifi(int id, struct mmc_host *mmc);
void ruby_probe_wifi(int id, struct mmc_host *mmc);

static struct mmc_platform_data ruby_wifi_data = {
	.ocr_mask	= MMC_VDD_165_195,
	.status		= ruby_wifi_status,
	.register_status_notify	= ruby_wifi_status_register,
	.embedded_sdio	= &ruby_wifi_emb_data,
	.mmc_bus_width	= MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin   = 400000,
	.msmsdcc_fmid   = 24000000,
	.msmsdcc_fmax   = 48000000,
	.board_probe	= ruby_probe_wifi,
	.board_remove	= ruby_remove_wifi,
	.nonremovable	= 1,
	.is_ti_wifi	= 1,
};

int ruby_wifi_set_carddetect(int val)
{
	printk(KERN_INFO "%s: %d\n", __func__, val);
	ruby_wifi_cd = val;
	if (wifi_status_cb)
		wifi_status_cb(val, wifi_status_cb_devid);
	else
		printk(KERN_WARNING "%s: Nobody to notify\n", __func__);
	return 0;
}
EXPORT_SYMBOL(ruby_wifi_set_carddetect);

int ti_wifi_power(int on)
{
	const unsigned SDC4_HDRV_PULL_CTL_ADDR = (unsigned) MSM_TLMM_BASE + 0x20A0;

	printk(KERN_INFO "%s: %d\n", __func__, on);

	if (on) {
		writel(0x1FDB, SDC4_HDRV_PULL_CTL_ADDR);
		config_gpio_table(wifi_on_gpio_table,
				  ARRAY_SIZE(wifi_on_gpio_table));
	} else {
		writel(0x0BDB, SDC4_HDRV_PULL_CTL_ADDR);
		config_gpio_table(wifi_off_gpio_table,
				  ARRAY_SIZE(wifi_off_gpio_table));
	}
	
	mdelay(1);
	gpio_set_value(RUBY_GPIO_WIFI_SHUTDOWN_N, on);

	mdelay(120);
	return 0;
}
EXPORT_SYMBOL(ti_wifi_power);

int ruby_wifi_reset(int on)
{
	printk(KERN_INFO "%s: do nothing\n", __func__);
	return 0;
}

static struct mmc_host *wifi_mmc;
int board_sdio_wifi_enable(unsigned int param);
int board_sdio_wifi_disable(unsigned int param);

void ruby_probe_wifi(int id, struct mmc_host *mmc)
{
	printk("%s: id %d mmc %p\n", __PRETTY_FUNCTION__, id, mmc);
	wifi_mmc = mmc;
}

void ruby_remove_wifi(int id, struct mmc_host *mmc)
{
	printk("%s: id %d mmc %p\n", __PRETTY_FUNCTION__, id, mmc);
	wifi_mmc = NULL;
}

/*
 *  An API to enable wifi
 */
int board_sdio_wifi_enable(unsigned int param)
{
	printk(KERN_ERR "board_sdio_wifi_enable\n");

	ti_wifi_power(1);
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

	ti_wifi_power(0);

	if (wifi_mmc) {
		mmc_detect_change(wifi_mmc, msecs_to_jiffies(100));
	}

	return 0;
}
EXPORT_SYMBOL(board_sdio_wifi_disable);

int __init ruby_init_wifi_mmc()
{
	uint32_t id;
	const unsigned SDC4_HDRV_PULL_CTL_ADDR = (unsigned)(MSM_TLMM_BASE + 0x20A0);

	printk(KERN_INFO "ruby: %s\n", __func__);

	/* initial WIFI_SHUTDOWN# */
	id = GPIO_CFG(RUBY_GPIO_WIFI_SHUTDOWN_N, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_NO_PULL, GPIO_CFG_2MA);

	/*WIFI:Per HW request,we need pull up wlan SDCC4 DATA/CMD pin when initial to prevent power leakage issue*/
	writel(0x1FDB, SDC4_HDRV_PULL_CTL_ADDR);

	gpio_tlmm_config(id, 0);
	gpio_set_value(RUBY_GPIO_WIFI_SHUTDOWN_N, 0);
	msm_add_sdcc(4, &ruby_wifi_data);

	return 0;
}
