#include <mach/panel_id.h>
#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_m7ul.h"

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	
	
	{0x03, 0x08, 0x05, 0x00, 0x20},
	
	{0xDD, 0x51, 0x27, 0x00, 0x6E, 0x74, 0x2C,
	0x55, 0x3E, 0x3, 0x4, 0xA0},
	
	{0x5F, 0x00, 0x00, 0x10},
	
	{0xFF, 0x00, 0x06, 0x00},
	
	{0x00, 0x38, 0x32, 0xDA, 0x00, 0x10, 0x0F, 0x61,
	0x41, 0x0F, 0x01,
	0x00, 0x1A, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02 },
};

static struct mipi_dsi_phy_ctrl dsi_jdi_cmd_mode_phy_db = {
	
        {0x03, 0x08, 0x05, 0x00, 0x20},
        
	{0xD7, 0x34, 0x23, 0x00, 0x63, 0x6A, 0x28, 0x37, 0x3C, 0x03, 0x04},
        
        {0x5F, 0x00, 0x00, 0x10},
        
        {0xFF, 0x00, 0x06, 0x00},
        
	{0x00, 0xA8, 0x30, 0xCA, 0x00, 0x20, 0x0F, 0x62, 0x70, 0x88, 0x99, 0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static struct msm_panel_info pinfo;

static int __init mipi_cmd_jdi_renesas_init(void)
{
	int ret;

	pinfo.xres = 1080;
	pinfo.yres = 1920;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 27;
	pinfo.lcdc.h_front_porch = 38;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 4;
	pinfo.lcdc.v_front_porch = 4;
	pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcd.v_back_porch = pinfo.lcdc.v_back_porch;
	pinfo.lcd.v_front_porch = pinfo.lcdc.v_front_porch;
	pinfo.lcd.v_pulse_width = pinfo.lcdc.v_pulse_width;

	pinfo.lcd.primary_vsync_init = pinfo.yres;
	pinfo.lcd.primary_rdptr_irq = 0;
	pinfo.lcd.primary_start_pos = pinfo.yres +
			pinfo.lcd.v_back_porch + pinfo.lcd.v_front_porch - 1;

	pinfo.lcdc.border_clr = 0;      
	pinfo.lcdc.underflow_clr = 0xff;        
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 830000000;

	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6000; 

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;

	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.t_clk_post = 0x3;
	pinfo.mipi.t_clk_pre = 0x2B;
	pinfo.mipi.stream = 0;  
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; 
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;

	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_jdi_cmd_mode_phy_db;
	pinfo.mipi.esc_byte_ratio = 5;

	ret = mipi_m7_device_register(&pinfo, MIPI_DSI_PRIM,
		MIPI_DSI_PANEL_FWVGA_PT);

	pr_info("%s\n", __func__);

	return ret;
}

static int __init mipi_cmd_sharp_init(void)
{
	int ret;

	pinfo.xres = 1080;
	pinfo.yres = 1920;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 27;
	pinfo.lcdc.h_front_porch = 38;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 4;
	pinfo.lcdc.v_front_porch = 4;
	pinfo.lcdc.v_pulse_width = 2;
 
	pinfo.lcd.v_back_porch = pinfo.lcdc.v_back_porch;
	pinfo.lcd.v_front_porch = pinfo.lcdc.v_front_porch;
	pinfo.lcd.v_pulse_width = pinfo.lcdc.v_pulse_width;

	pinfo.lcd.primary_vsync_init = pinfo.yres;
	pinfo.lcd.primary_rdptr_irq = 0;
	pinfo.lcd.primary_start_pos = pinfo.yres +
			pinfo.lcd.v_back_porch + pinfo.lcd.v_front_porch - 1;

	pinfo.lcdc.border_clr = 0;	
	pinfo.lcdc.underflow_clr = 0xff;	
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 830000000;

	
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6000; 

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;

	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.t_clk_post = 0x3;
	pinfo.mipi.t_clk_pre = 0x2B;
	pinfo.mipi.stream = 0;	
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; 
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;

	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_jdi_cmd_mode_phy_db;
	pinfo.mipi.esc_byte_ratio = 5;

	ret = mipi_m7_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_FWVGA_PT);

	pr_info("%s\n", __func__);
	return ret;

}

static int __init mipi_video_sharp_init(void)
{
	int ret;

	pinfo.xres = 1080;
	pinfo.yres = 1920;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 58;
	pinfo.lcdc.h_front_porch = 100;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 4;
	pinfo.lcdc.v_front_porch = 4;
	pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcd.v_back_porch = 4;
	pinfo.lcd.v_front_porch = 4;
	pinfo.lcd.v_pulse_width = 2;

	pinfo.lcd.primary_vsync_init = pinfo.yres;
	pinfo.lcd.primary_rdptr_irq = 0;
	pinfo.lcd.primary_start_pos = pinfo.yres +
			pinfo.lcd.v_back_porch + pinfo.lcd.v_front_porch - 1;

	pinfo.lcdc.border_clr = 0;	
	pinfo.lcdc.underflow_clr = 0xff;	
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 860000000;

	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = FALSE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.t_clk_post = 0x05;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; 
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.esc_byte_ratio = 5;

	ret = mipi_m7_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_FWVGA_PT);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	pr_info("%s\n", __func__);
	return ret;
}

static int __init mipi_video_sony_init(void)
{
	int ret;

	pinfo.xres = 1080;
	pinfo.yres = 1920;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 58;
	pinfo.lcdc.h_front_porch = 100;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 3;
	pinfo.lcdc.v_front_porch = 3;
	pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcd.v_back_porch = 3;
	pinfo.lcd.v_front_porch = 3;
	pinfo.lcd.v_pulse_width = 2;

	pinfo.lcd.primary_vsync_init = pinfo.yres;
	pinfo.lcd.primary_rdptr_irq = 0;
	pinfo.lcd.primary_start_pos = pinfo.yres +
		pinfo.lcd.v_back_porch + pinfo.lcd.v_front_porch - 1;

	pinfo.lcdc.border_clr = 0;	
	pinfo.lcdc.underflow_clr = 0xff;	
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 860000000;

	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = FALSE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.t_clk_post = 0x05;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; 
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.esc_byte_ratio = 5;

	ret = mipi_m7_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_FWVGA_PT);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

static int __init mipi_command_samsung_init(void)
{
	int ret;

	pinfo.xres = 1080;
	pinfo.yres = 1920;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 27;
	pinfo.lcdc.h_front_porch = 38;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 4;
	pinfo.lcdc.v_front_porch = 4;
	pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcd.v_back_porch = pinfo.lcdc.v_back_porch;
	pinfo.lcd.v_front_porch = pinfo.lcdc.v_front_porch;
	pinfo.lcd.v_pulse_width = pinfo.lcdc.v_pulse_width;

	pinfo.lcd.primary_vsync_init = pinfo.yres;
	pinfo.lcd.primary_rdptr_irq = 0;
	pinfo.lcd.primary_start_pos = pinfo.yres +
			pinfo.lcd.v_back_porch + pinfo.lcd.v_front_porch - 1;

	pinfo.lcdc.border_clr = 0;	
	pinfo.lcdc.underflow_clr = 0xff;	
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 830000000;

	
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6000; 

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;

	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.t_clk_post = 0x3;
	pinfo.mipi.t_clk_pre = 0x2B;
	pinfo.mipi.stream = 0;	
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; 
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.esc_byte_ratio = 5;

	pinfo.mipi.frame_rate = 60;

        //	pinfo.lcdc.no_set_tear = 1;
	
	pinfo.mipi.dsi_phy_db = &dsi_jdi_cmd_mode_phy_db;

#ifdef CONFIG_FB_MSM_ESD_WORKAROUND
	m7_panel_data.esd_workaround = true;
#endif

	ret = mipi_m7_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_FWVGA_PT);

	pr_info("%s\n", __func__);
	return ret;
}
static int __init mipi_m7ul_hd_pt_init(void)
{
	if (panel_type == PANEL_ID_DLXJ_SHARP_RENESAS) {
		mipi_video_sharp_init();
		pr_info("%s panel ID = PANEL_ID_DLXJ_SHARP_RENESAS\n", __func__);
	} else if (panel_type == PANEL_ID_DLXJ_SONY_RENESAS) {
		mipi_video_sony_init();
		pr_info("%s panel ID = PANEL_ID_DLXJ_SONY_RENESAS\n", __func__);
	} else if (panel_type == PANEL_ID_M7_JDI_SAMSUNG) {
		mipi_command_samsung_init();
		pr_info("%s panel ID = PANEL_ID_M7_JDI_SAMSUNG\n", __func__);
	} else if (panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2) {
		mipi_command_samsung_init();
		pr_info("%s panel ID = PANEL_ID_M7_JDI_SAMSUNG_C2\n", __func__);
	} else if (panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2_1) {
		mipi_command_samsung_init();
		pr_info("%s panel ID = PANEL_ID_M7_JDI_SAMSUNG_C2_1\n", __func__);
	} else if (panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2_2) {
		mipi_command_samsung_init();
		pr_info("%s panel ID = PANEL_ID_M7_JDI_SAMSUNG_C2_2\n", __func__);
	} else if (panel_type == PANEL_ID_M7_SHARP_RENESAS) {
		mipi_video_sharp_init();
		pr_info("%s panel ID = PANEL_ID_M7_SHARP_RENESAS\n", __func__);
	} else if (panel_type == PANEL_ID_M7_SHARP_RENESAS_C1) {
		mipi_cmd_sharp_init();
		pr_info("%s panel ID = PANEL_ID_M7_SHARP_RENESAS_CMD\n", __func__);
	} else if (panel_type == PANEL_ID_M7_JDI_RENESAS) {
		mipi_cmd_jdi_renesas_init();
		pr_info("%s panel ID = PANEL_ID_M7_JDI_RENESAS_CMD\n", __func__);
	} else {
		pr_err("%s: panel not supported!!\n", __func__);
		return -ENODEV;
	}
        return 0;
}

late_initcall(mipi_m7ul_hd_pt_init);
