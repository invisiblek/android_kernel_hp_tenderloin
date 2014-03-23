/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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
 *
 */

#include "../../../../drivers/video/msm/msm_fb.h"
#include "../../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_shooter.h"

static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
/* DSI_BIT_CLK at 482MHz, 2 lane, RGB888 */
		{0x03, 0x01, 0x01, 0x00},	/* regulator */
		/* timing   */
		{0xB4, 0x8D, 0x1D, 0x00, 0x20, 0x94, 0x20,
		0x8F, 0x20, 0x03, 0x04},
		{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
		{0xee, 0x02, 0x86, 0x00},	/* strength */
		/* pll control */
		{0x40, 0xf9, 0xb0, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x30, 0x07, 0x03,
		0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};

static int __init mipi_cmd_shooter_qhd_pt_init(void)
{
	int ret;

	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 64;
	pinfo.lcdc.h_front_porch = 96;
	pinfo.lcdc.h_pulse_width = 32;
	pinfo.lcdc.v_back_porch = 16;
	pinfo.lcdc.v_front_porch = 16;
	pinfo.lcdc.v_pulse_width = 4;

	pinfo.lcd.v_back_porch = 16;
	pinfo.lcd.v_front_porch = 16;
	pinfo.lcd.v_pulse_width = 4;

	pinfo.lcd.primary_vsync_init = pinfo.yres;
	pinfo.lcd.primary_rdptr_irq = 0;
	pinfo.lcd.primary_start_pos = pinfo.yres +
		pinfo.lcd.v_back_porch + pinfo.lcd.v_front_porch - 1;

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

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
        pinfo.mipi.rgb_swap = DSI_RGB_SWAP_BGR;
        pinfo.mipi.esc_byte_ratio = 4;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x22;
	pinfo.mipi.t_clk_pre = 0x3f;
	pinfo.mipi.stream = 0;	/* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;

	ret = mipi_shooter_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_QHD_PT);
	if (ret)
          printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}

//late_initcall(mipi_cmd_shooter_qhd_pt_init);
device_initcall_sync(mipi_cmd_shooter_qhd_pt_init);
