#include "../../../../drivers/video/msm/msm_fb.h"
#include "lcdc_tenderloin.h"

int lcdc_tenderloin_device_register(struct msm_panel_info *pinfo);

static struct msm_panel_info pinfo;

static int __init lcdc_tenderloin_panel_init(void)
{
	int ret;

        printk(KERN_ERR "%s: +++++++++++++++++++++++\n", __func__);

	pinfo.xres = 1024;
	pinfo.yres = 768;
	MSM_FB_SINGLE_MODE_PANEL(&pinfo);
	pinfo.type = LCDC_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
        pinfo.bpp = 18;
	pinfo.fb_num = 2;
        pinfo.clk_rate = 96000000;
	pinfo.bl_max = PWM_LEVEL_MAX;
	pinfo.bl_min = PWM_LEVEL_MIN;

	pinfo.lcdc.h_back_porch = 400;
	pinfo.lcdc.h_front_porch = 272;
	pinfo.lcdc.h_pulse_width = 328;
	pinfo.lcdc.v_back_porch = 6;
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 7;
	pinfo.lcdc.border_clr = 0;
	pinfo.lcdc.underflow_clr = 0xff;
	pinfo.lcdc.hsync_skew = 0;

	ret = lcdc_tenderloin_device_register(&pinfo);
	if (ret)
		printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}

//late_initcall(lcdc_tenderloin_panel_init);
device_initcall_sync(lcdc_tenderloin_panel_init);
