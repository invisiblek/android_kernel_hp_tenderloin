/* arch/arm/mach-msm/board-tc2-keypad.c
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
#include <linux/keyreset.h>
#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include "board-tc2.h"

static char *keycaps = "--qwerty";
#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "board_tc2."
module_param_named(keycaps, keycaps, charp, 0);

static struct gpio_event_direct_entry tc2_keypad_input_map[] = {
	{
		.gpio = MSM_PWR_KEYz,
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

static struct gpio_event_input_info tc2_keypad_input_info = {
	.info.func             = gpio_event_input_func,
	.flags                 = GPIOEDF_PRINT_KEYS,
	.type                  = EV_KEY,
#if BITS_PER_LONG != 64 && !defined(CONFIG_KTIME_SCALAR)
	.debounce_time.tv.nsec = 15 * NSEC_PER_MSEC,
# else
	.debounce_time.tv64    = 15 * NSEC_PER_MSEC,
# endif
	.keymap                = tc2_keypad_input_map,
	.keymap_size           = ARRAY_SIZE(tc2_keypad_input_map),
};

static struct gpio_event_info *tc2_keypad_info[] = {
	&tc2_keypad_input_info.info,
};

static struct gpio_event_platform_data tc2_keypad_data = {
	.names = {
		"tc2-keypad",
		NULL,
	},
	.info = tc2_keypad_info,
	.info_count = ARRAY_SIZE(tc2_keypad_info),
};

static struct platform_device tc2_keypad_input_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev		= {
		.platform_data	= &tc2_keypad_data,
	},
};
static struct keyreset_platform_data tc2_reset_keys_pdata = {
	/*.keys_up = tc2_reset_keys_up,*/
	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEDOWN,
		KEY_VOLUMEUP,
		0
	},
};

struct platform_device tc2_reset_keys_device = {
	.name = KEYRESET_NAME,
	.dev.platform_data = &tc2_reset_keys_pdata,
};

int __init tc2_init_keypad(void)
{
	printk(KERN_DEBUG "[KEY]%s\n", __func__);

	if (platform_device_register(&tc2_reset_keys_device))
		printk(KERN_WARNING "%s: register reset key fail\n", __func__);

	return platform_device_register(&tc2_keypad_input_device);
}
