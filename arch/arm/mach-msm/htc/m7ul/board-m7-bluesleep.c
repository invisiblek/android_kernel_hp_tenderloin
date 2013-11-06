/*
 * Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
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


#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <mach/irqs.h>

#include "board-m7.h"
#include "devices.h"

extern void bluesleep_setup_uart_port(struct platform_device *uart_dev);

static struct resource bluesleep_resources[] = {
	{
		.name = "gpio_host_wake",
		.start = PM8921_GPIO_PM_TO_SYS(BT_HOST_WAKE),
		.end = PM8921_GPIO_PM_TO_SYS(BT_HOST_WAKE),
		.flags = IORESOURCE_IO,
	},
	{
		.name = "gpio_ext_wake",
		.start = PM8921_GPIO_PM_TO_SYS(BT_WAKE),
		.end = PM8921_GPIO_PM_TO_SYS(BT_WAKE),
		.flags = IORESOURCE_IO,
	},
	{
		.name = "host_wake",
		.start = MSM_GPIO_TO_INT(PM8921_GPIO_PM_TO_SYS(BT_HOST_WAKE)),
		.end = MSM_GPIO_TO_INT(PM8921_GPIO_PM_TO_SYS(BT_HOST_WAKE)),
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device msm_bluesleep_device = {
	.name = "bluesleep",
	.id = -1,
	.num_resources = ARRAY_SIZE(bluesleep_resources),
	.resource = bluesleep_resources,
};

static int __init m7_bluesleep_init(void)
{
	int ret;

	ret = platform_device_register(&msm_bluesleep_device);
	if (ret >= 0) {
		bluesleep_setup_uart_port(&msm_device_uart_dm6);
	} else {
		printk(KERN_ERR "m7-bluesleep: error registering");
	}

	return ret;
}

static void __exit m7_bluesleep_exit(void)
{
	platform_device_unregister(&msm_bluesleep_device);
}

module_init(m7_bluesleep_init);
module_exit(m7_bluesleep_exit);
MODULE_DESCRIPTION("m7 Bluesleep driver");
MODULE_AUTHOR("CyanogenMod Project");
MODULE_LICENSE("GPL");
