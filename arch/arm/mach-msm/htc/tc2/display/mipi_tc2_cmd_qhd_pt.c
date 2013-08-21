#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_tc2.h"

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* DSI_BIT_CLK at 482MHz, 2 lane, RGB888 */
	/* regulator *//* off=0x0500 */
	{0x09, 0x08, 0x05, 0x00, 0x20},
	/* timing *//* off=0x0440 */
	//clk_rate:600MHz
	{0xb9, 0x2A, 0x20, 0x00, 0x24, 0x50, 0x1D, 0x2A, 0x24,
	0x03, 0x04, 0xa0},
	/* phy ctrl *//* off=0x0470 */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength *//* off=0x0480 */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control *//* off=0x0200 */
	{0x0, 0xe, 0x30, 0xda, 0x00, 0x10, 0x0f, 0x61,
	0x40, 0x07, 0x03,
	0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02},
};

static struct msm_panel_info pinfo;

static int __init mipi_cmd_tc2_qhd_pt_init(void)
{
	int ret;

	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 22;
	pinfo.lcdc.h_front_porch = 22;
	pinfo.lcdc.h_pulse_width = 1;
	pinfo.lcdc.v_back_porch = 3;
	pinfo.lcdc.v_front_porch = 3;
	pinfo.lcdc.v_pulse_width = 1;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;

	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 482000000;

	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6096; /* adjust refx100 to prevent tearing */

	pinfo.lcd.v_back_porch = 2;
	pinfo.lcd.v_front_porch = 2;
	pinfo.lcd.v_pulse_width = 2;

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.esc_byte_ratio = 4;

	pinfo.mipi.t_clk_post = 0x0a;
	pinfo.mipi.t_clk_pre = 0x21;
	pinfo.mipi.stream = 0;	/* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.tx_eot_append = 1; /* to prevent flicker and color shift */
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;

	ret = mipi_tc2_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_QHD_PT);

	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

late_initcall(mipi_cmd_tc2_qhd_pt_init);
