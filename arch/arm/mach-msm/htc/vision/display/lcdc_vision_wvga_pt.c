#include "../../../../drivers/video/msm/msm_fb.h"
#include "lcdc_vision.h"

int lcdc_vision_device_register(struct msm_panel_info *pinfo);

static struct msm_panel_info pinfo;

static int __init lcdc_vision_panel_init(void)
{
	int ret;

        printk(KERN_ERR "%s: +++++++++++++++++++++++\n", __func__);
	pinfo.xres = 480;
	pinfo.yres = 800;
	MSM_FB_SINGLE_MODE_PANEL(&pinfo);
	pinfo.type = LCDC_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 18;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 24576000;
	pinfo.bl_max = 255;
	pinfo.bl_min = 0;

	pinfo.lcdc.h_back_porch = 18;
	pinfo.lcdc.h_front_porch = 20;
	pinfo.lcdc.h_pulse_width = 2;
	pinfo.lcdc.v_back_porch = 5;
	pinfo.lcdc.v_front_porch = 4;
	pinfo.lcdc.v_pulse_width = 2;
	pinfo.lcdc.border_clr = 0;
	pinfo.lcdc.underflow_clr = 0xff;
	pinfo.lcdc.hsync_skew = 0;

        ret = lcdc_vision_device_register(&pinfo);
        if (ret)
          printk(KERN_ERR "%s: failed to register device!\n", __func__);

        return ret;
}

device_initcall_sync(lcdc_vision_panel_init);
//late_initcall(lcdc_vision_panel_init);
