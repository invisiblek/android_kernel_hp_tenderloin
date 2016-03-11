/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/delay.h>
#include <linux/pwm.h>
#ifdef CONFIG_PMIC8058_PWM
#include <linux/mfd/pmic8058.h>
#include <linux/pmic8058-pwm.h>
#endif
#include <mach/gpio.h>
#include "../../../../drivers/video/msm/msm_fb.h"
#include "lcdc_tenderloin.h"

static struct msm_panel_common_pdata *lcdc_tenderloin_pdata;

#ifdef CONFIG_PMIC8058_PWM
extern int pwm_config2(struct pwm_device *pwm, unsigned duty_numerator,
				unsigned duty_denominator, unsigned period_us);

static struct pwm_device *bl_pwm0;
#endif

static int lcdc_tenderloin_panel_on(struct platform_device *pdev)
{
	return 0;
}

static int lcdc_tenderloin_panel_off(struct platform_device *pdev)
{
	return 0;
}

static void lcdc_tenderloin_panel_set_backlight(struct msm_fb_data_type *mfd)
{
	unsigned int bl_level;
	int ret;

	bl_level = mfd->bl_level;
	printk(KERN_ERR "[DISP] %s: %d\n", __func__, bl_level);

#ifdef CONFIG_PMIC8058_PWM
	if (bl_pwm0) {
		ret = pwm_config2(bl_pwm0, bl_level, PWM_LEVEL_MAX, PWM_PERIOD_USEC);
		if (ret)
			printk(KERN_ERR "pwm_config on pwm 0 failed %d\n", ret);

		ret = pwm_enable(bl_pwm0);
		if (ret)
			printk(KERN_ERR "pwm_enable on pwm 0 failed %d\n", ret);
	}
#endif
}

static int pmic_backlight_gpio[2] = { GPIO_BACKLIGHT_PWM0, GPIO_BACKLIGHT_PWM1 };

static int __devinit lcdc_tenderloin_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
		lcdc_tenderloin_pdata = pdev->dev.platform_data;
		return 0;
	}

#ifdef CONFIG_PMIC8058_PWM
	bl_pwm0 = pwm_request(pmic_backlight_gpio[0], "backlight");
	if (bl_pwm0 == NULL || IS_ERR(bl_pwm0)) {
		pr_err("%s pwm_request() failed\n", __func__);
		bl_pwm0 = NULL;
	}

	printk(KERN_INFO "%s: bl_pwm0=%p LPG_chan0=%d\n", __func__,
					bl_pwm0, (int)pmic_backlight_gpio[0]);
#endif

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe = lcdc_tenderloin_probe,
	.driver = {
		.name = "lcdc_tenderloin",
	},
};

static struct msm_fb_panel_data tenderloin_panel_data = {
 	.on = lcdc_tenderloin_panel_on,
 	.off = lcdc_tenderloin_panel_off,
	.set_backlight = lcdc_tenderloin_panel_set_backlight,
};

static int lcdc_tenderloin_lcd_init(void);

int lcdc_tenderloin_device_register(struct msm_panel_info *pinfo)
{
	struct platform_device *pdev = NULL;
	int ret;

	ret = lcdc_tenderloin_lcd_init();
	if (ret) {
		pr_err("lcdc_tenderloin_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("lcdc_tenderloin", 1);
	if (!pdev)
		return -ENOMEM;

	tenderloin_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &tenderloin_panel_data,
		sizeof(tenderloin_panel_data));
	if (ret) {
		printk(KERN_ERR "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int lcdc_tenderloin_lcd_init(void)
{
	return platform_driver_register(&this_driver);
}
