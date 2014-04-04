/* arch/arm/mach-msm/board-tenderloin-keypad.c
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
#include <linux/moduleparam.h>

#include "board-tenderloin.h"

#include <linux/mfd/pmic8058.h>

static char *keycaps = "--qwerty";
#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "board_tenderloin."
module_param_named(keycaps, keycaps, charp, 0);

static struct gpio_event_direct_entry tenderloin_keypad_input_map[] = {
	{
		.gpio = VOL_UP_GPIO,
		.code = KEY_VOLUMEUP,
	},
	{
		.gpio = VOL_DN_GPIO,
		.code = KEY_VOLUMEDOWN,
	},
        {
                .gpio = CORE_NAVI_GPIO,
                .code = KEY_HOMEPAGE,
        },
};

static struct gpio_event_input_info tenderloin_keypad_input_info = {
	.info.func = gpio_event_input_func,
	.info.no_suspend = true,
	.flags = GPIOEDF_PRINT_KEYS,
	.type = EV_KEY,
#if BITS_PER_LONG != 64 && !defined(CONFIG_KTIME_SCALAR)
	.debounce_time.tv.nsec = 5 * NSEC_PER_MSEC,
# else
	.debounce_time.tv64 = 5 * NSEC_PER_MSEC,
# endif
	.keymap = tenderloin_keypad_input_map,
	.keymap_size = ARRAY_SIZE(tenderloin_keypad_input_map),
};

static struct gpio_event_info *tenderloin_keypad_info[] = {
	&tenderloin_keypad_input_info.info,
};

static struct gpio_event_platform_data tenderloin_keypad_data = {
	.name = "tenderloin-keypad",
	.info = tenderloin_keypad_info,
	.info_count = ARRAY_SIZE(tenderloin_keypad_info),
};

static struct platform_device tenderloin_keypad_input_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev		= {
		.platform_data	= &tenderloin_keypad_data,
	},
};

static struct keyreset_platform_data tenderloin_reset_keys_pdata = {
	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEDOWN,
		KEY_VOLUMEUP,
		0
	},
};

struct platform_device tenderloin_reset_keys_device = {
	.name = KEYRESET_NAME,
	.dev.platform_data = &tenderloin_reset_keys_pdata,
};

int __init tenderloin_init_keypad(void)
{
	printk(KERN_DEBUG "%s\n", __func__);

        tenderloin_keypad_input_map[0].gpio = pin_table[VOL_UP_GPIO_PIN];
        tenderloin_keypad_input_map[1].gpio = pin_table[VOL_DN_GPIO_PIN];

	if (platform_device_register(&tenderloin_reset_keys_device))
		printk(KERN_WARNING "%s: register reset key fail\n", __func__);

	return platform_device_register(&tenderloin_keypad_input_device);
}
