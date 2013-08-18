/* arch/arm/mach-msm/board-operaul-keypad.c
 *
 * Copyright (C) 2008 Google, Inc.
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

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/keyreset.h>
#include <linux/delay.h>
#include <asm/mach-types.h>
#include <mach/gpio.h>


#include "board-operaul.h"

#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "board_operaul."

static struct gpio_event_direct_entry operaul_keypad_input_map[] = {
	{
		.gpio = MSM_AP_KPDPWRz,
		.code = KEY_POWER,
	},
	{
		.gpio = MSM_VOL_UPz,
		.code = KEY_VOLUMEUP,
	},
	{
		.gpio = MSM_VOL_DOWNz,
		.code = KEY_VOLUMEDOWN,
	 },
};

static struct gpio_event_input_info operaul_keypad_input_info = {
	.info.func             = gpio_event_input_func,
	.flags                 = GPIOEDF_PRINT_KEYS,
	.type                  = EV_KEY,
#if BITS_PER_LONG != 64 && !defined(CONFIG_KTIME_SCALAR)
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
# else
	.debounce_time.tv64    = 20 * NSEC_PER_MSEC,
# endif
	.keymap                = operaul_keypad_input_map,
	.keymap_size           = ARRAY_SIZE(operaul_keypad_input_map),
};

static struct gpio_event_info *operaul_keypad_info[] = {
	&operaul_keypad_input_info.info,
};

static struct gpio_event_platform_data operaul_keypad_data = {
	.names = {
		"operaul-keypad",
		NULL,
	},
	.info = operaul_keypad_info,
	.info_count = ARRAY_SIZE(operaul_keypad_info),
};

static struct platform_device operaul_keypad_input_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev		= {
		.platform_data	= &operaul_keypad_data,
	},
};
static struct keyreset_platform_data operaul_reset_keys_pdata = {
	
	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEDOWN,
		KEY_VOLUMEUP,
		0
	},
};

struct platform_device operaul_reset_keys_device = {
	.name = KEYRESET_NAME,
	.dev.platform_data = &operaul_reset_keys_pdata,
};

int __init operaul_init_keypad(void)
{
	printk(KERN_DEBUG "[KEY]%s\n", __func__);

	if (platform_device_register(&operaul_reset_keys_device))
		printk(KERN_WARNING "%s: register reset key fail\n", __func__);

	return platform_device_register(&operaul_keypad_input_device);
}
