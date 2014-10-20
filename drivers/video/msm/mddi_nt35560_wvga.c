/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

#include <linux/delay.h>
#include "msm_fb.h"
#include "mddihost.h"
#include "mddihosti.h"

#define ENTER_SLEEP_MODE			0x1000
#define EXIT_SLEEP_MODE				0x1100
#define WRDISBV					0x5100

static int mddi_nt35560_panel_on(struct platform_device *pdev)
{
	mddi_host_client_cnt_reset();

	mddi_queue_register_write(EXIT_SLEEP_MODE, 0, TRUE, 0);
	mddi_wait(125);

	return 0;
}

static int mddi_nt35560_panel_off(struct platform_device *pdev)
{
	mddi_queue_register_write(ENTER_SLEEP_MODE, 0, TRUE, 0);
	mddi_wait(10);

	return 0;
}

static void mddi_nt35560_panel_set_backlight(struct msm_fb_data_type *mfd)
{
	mddi_queue_register_write(WRDISBV, mfd->bl_level, TRUE, 0);
}

static int __devinit nt35560_probe(struct platform_device *pdev)
{
	if (pdev->id == 0)
		return 0;

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe = nt35560_probe,
	.driver = {
		.name = "mddi_nt35560_wvga",
	},
};

static struct msm_fb_panel_data nt35560_panel_data = {
	.on = mddi_nt35560_panel_on,
	.off = mddi_nt35560_panel_off,
	.set_backlight = mddi_nt35560_panel_set_backlight,
};

static struct platform_device this_device = {
	.name = "mddi_nt35560_wvga",
	.id = 1,
	.dev = {
		.platform_data = &nt35560_panel_data,
	}
};

static int __init mddi_nt35560_wvga_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

#ifdef CONFIG_FB_MSM_MDDI_AUTO_DETECT
	if (msm_fb_detect_client("mddi_nt35560_wvga"))
		return -ENODEV;

	if ((0xb9f6 << 16 | 0x5560) != mddi_get_client_id())
		return -ENODEV;
#endif

	ret = platform_driver_register(&this_driver);
	if (ret)
		return ret;

	pinfo = &nt35560_panel_data.panel_info;
	pinfo->xres = 480;
	pinfo->yres = 800;
	MSM_FB_SINGLE_MODE_PANEL(pinfo);
	pinfo->type = MDDI_PANEL;
	pinfo->pdest = DISPLAY_1;
	pinfo->mddi.vdopkt = MDDI_DEFAULT_PRIM_PIX_ATTR;
	pinfo->mddi.is_type1 = FALSE;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;
	pinfo->lcd.refx100 = 6000;
	pinfo->lcd.v_back_porch = 0;
	pinfo->lcd.v_front_porch = 0;
	pinfo->lcd.v_pulse_width = 22;
	pinfo->lcd.rev = 2;
	pinfo->lcd.vsync_enable = TRUE;
	pinfo->lcd.hw_vsync_mode = TRUE;
	pinfo->lcd.vsync_notifier_period = (1 * HZ);
	pinfo->bl_max = 255;
	pinfo->bl_min = 12;
	pinfo->clk_rate = 192000000;
	pinfo->clk_min = 192000000;
	pinfo->clk_max = 192000000;
	pinfo->fb_num = 2;

	ret = platform_device_register(&this_device);
	if (ret) {
		printk(KERN_ERR "%s not able to register the device\n",
			__func__);
		platform_driver_unregister(&this_driver);
	}

	return ret;
}

module_init(mddi_nt35560_wvga_init);
