/* linux/arch/arm/mach-msm/board-operaul-mmc.c
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
#include <linux/irq.h>
#include <asm/gpio.h>
#include <asm/io.h>

#include <mach/vreg.h>
#include <mach/htc_pwrsink.h>

#include <asm/mach/mmc.h>
#include <mach/msm_iomap.h>
#include <linux/mfd/pm8xxx/pm8038.h>
#include "board-8930.h"
#include "../Board-storage-common-htc.h"

#include "devices.h"
#include "board-operaul.h"
#include <mach/proc_comm.h>
#include <linux/export.h>

#include "board-operaul-mmc.h"

#include <mach/rpm.h>
#include <mach/rpm-regulator.h>
#include "rpm_resources.h"

int msm_proc_comm(unsigned cmd, unsigned *data1, unsigned *data2);


#define PM8058_GPIO_BASE			NR_MSM_GPIOS
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_GPIO_BASE)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)		(sys_gpio - PM8058_GPIO_BASE)


static uint32_t wifi_on_gpio_table[] = {
	GPIO_CFG(MSM_WIFI_SD_D3, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_D2, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_D1, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_D0, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_CMD, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WCN_CMD_CLK, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WW_IRQ, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 
};

static uint32_t wifi_off_gpio_table[] = {
	GPIO_CFG(MSM_WIFI_SD_D3, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_D2, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_D1, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_D0, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WIFI_SD_CMD, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WCN_CMD_CLK, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(MSM_WW_IRQ, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), 
};

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config(%#x)=%d\n", __func__, table[n], rc);
			break;
		}
	}
}

static struct embedded_sdio_data operaul_wifi_emb_data = {
	.cccr	= {
		.sdio_vsn	= 2,
		.multi_block	= 1,
		.low_speed	= 0,
		.wide_bus	= 0,
		.high_power	= 1,
		.high_speed	= 1,
	}
};

static void (*wifi_status_cb)(int card_present, void *dev_id);
static void *wifi_status_cb_devid;

static int
operaul_wifi_status_register(void (*callback)(int card_present, void *dev_id),
				void *dev_id)
{
	if (wifi_status_cb)
		return -EAGAIN;

	wifi_status_cb = callback;
	wifi_status_cb_devid = dev_id;
	return 0;
}

static int operaul_wifi_cd;	

static unsigned int operaul_wifi_status(struct device *dev)
{
	return operaul_wifi_cd;
}

static unsigned int wifi_sup_clk_rates[] = {
	400000, 24000000, 48000000,
};
static struct mmc_platform_data operaul_wifi_data = {
	.ocr_mask               = MMC_VDD_28_29,
	.status                 = operaul_wifi_status,
	.register_status_notify = operaul_wifi_status_register,
	.embedded_sdio          = &operaul_wifi_emb_data,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.sup_clk_table	= wifi_sup_clk_rates,
	.sup_clk_cnt	= ARRAY_SIZE(wifi_sup_clk_rates),
	.uhs_caps	= (MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
			MMC_CAP_UHS_SDR50),
	.msm_bus_voting_data = &wifi_sps_to_ddr_bus_voting_data,
	.nonremovable   = 0,
};


int operaul_wifi_set_carddetect(int val)
{
	printk(KERN_INFO "%s: %d\n", __func__, val);
	operaul_wifi_cd = val;
	if (wifi_status_cb)
		wifi_status_cb(val, wifi_status_cb_devid);
	else
		printk(KERN_WARNING "%s: Nobody to notify\n", __func__);
	return 0;
}
EXPORT_SYMBOL(operaul_wifi_set_carddetect);

int operaul_wifi_power(int on)
{

	printk(KERN_INFO "%s: %d\n", __func__, on);

	if (on) {

		config_gpio_table(wifi_on_gpio_table,
				  ARRAY_SIZE(wifi_on_gpio_table));
	} else {

		config_gpio_table(wifi_off_gpio_table,
				  ARRAY_SIZE(wifi_off_gpio_table));
	}

	gpio_set_value(MSM_WIFI_SHUTDOWNz, on); 

#if 0
	mdelay(1);
	gpio_set_value(MSM_WL_DEV_WAKE, on);
#endif
	mdelay(120);
	return 0;
}
EXPORT_SYMBOL(operaul_wifi_power);

int operaul_wifi_reset(int on)
{
	printk(KERN_INFO "%s: do nothing\n", __func__);
	return 0;
}

static struct msm_rpmrs_level msm_rpmrs_levels[] __initdata = {
	{
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1, 784, 180000, 100,
	},
#if 0
	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1300, 228, 1200000, 2000,
	},
#endif
	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, GDHS, MAX, ACTIVE),
		false,
		2000, 138, 1208400, 3200,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		6000, 119, 1850300, 9000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, GDHS, MAX, ACTIVE),
		false,
		9200, 68, 2839200, 16400,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, MAX, ACTIVE),
		false,
		10300, 63, 3128000, 18200,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		18000, 10, 4602600, 27000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, RET_HIGH, RET_LOW),
		false,
		20000, 2, 5752000, 32000,
	},
};

int __init operaul_init_mmc()
{
	uint32_t id;
	struct pm_gpio pm_config;
	wifi_status_cb = NULL;

	printk(KERN_INFO "operaul: %s\n", __func__);

	

#if 1 

	pm_config.direction = PM_GPIO_DIR_OUT;
	pm_config.output_buffer = PM_GPIO_OUT_BUF_CMOS;
	pm_config.output_value = 0;
	pm_config.pull = PM_GPIO_PULL_NO;
	pm_config.vin_sel = PM8038_GPIO_VIN_L11;
	pm_config.out_strength = PM_GPIO_STRENGTH_HIGH;
	pm_config.function = PM_GPIO_FUNC_1;
	pm_config.inv_int_pol = 0;
	pm_config.disable_pin = 0;
	pm8xxx_gpio_config(PM8038_GPIO_PM_TO_SYS(8), &pm_config);
	mdelay(5);
#endif

	
#if 0
	id = GPIO_CFG(MSM_WL_DEV_WAKE, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
	gpio_tlmm_config(id, 0);
	gpio_set_value(MSM_WL_DEV_WAKE, 1);
#endif
	
	id = GPIO_CFG(MSM_WIFI_SHUTDOWNz, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
	gpio_tlmm_config(id, 0);
	gpio_set_value(MSM_WIFI_SHUTDOWNz, 0);

	
	operaul_wifi_data.cpu_dma_latency = msm_rpmrs_levels[0].latency_us;

	msm_add_sdcc(4, &operaul_wifi_data);

	return 0;
}
