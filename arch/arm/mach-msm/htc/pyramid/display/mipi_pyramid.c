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
#include <mach/panel_id.h>
/* #include <mach/htc_battery_common.h> */
#include "../../../../drivers/video/msm/msm_fb.h"
#include "../../../../drivers/video/msm/mipi_dsi.h"
#include "../../../../drivers/video/msm/mdp4.h"
#include "mipi_pyramid.h"
#include <mach/debug_display.h>

#define DEBUG 0
/* -----------------------------------------------------------------------------
 *                             Constant value define
 * -----------------------------------------------------------------------------
 */

char max_pktsize[2] = {MIPI_DSI_MRPS, 0x00}; /* LSB tx first, 16 bytes */

int bl_level_prevset = 1;
struct dsi_cmd_desc *mipi_power_on_cmd = NULL;
struct dsi_cmd_desc *mipi_power_off_cmd = NULL;
int mipi_power_on_cmd_size = 0;
int mipi_power_off_cmd_size = 0;

static int mipi_pyramid_send_cmd(struct dsi_cmd_desc *cmd, unsigned int len, bool clk_ctrl)
{
	int ret = 0;
	struct dcs_cmd_req cmdreq;

        if (cmd == NULL
            || (len <= 0))
          return -1;

	cmdreq.cmds = cmd;
	cmdreq.cmds_cnt = len;
	cmdreq.flags = CMD_REQ_COMMIT;
        if (clk_ctrl)
          cmdreq.flags |= CMD_CLK_CTRL;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	ret = mipi_dsi_cmdlist_put(&cmdreq);
	if (ret < 0)
		pr_err("%s failed (%d)\n", __func__, ret);
        return ret;
}

const char NOVATEK_SONY_C3_MIPI_INIT[] = {
	3,
	DTYPE_DCS_WRITE, 0x11, 0x00,

	1, 120, /* Wait time... */

	3,
	DTYPE_DCS_WRITE, 0x29, 0x00,

	1, 40, /* Wait time... */

	3,
	DTYPE_DCS_WRITE1, 0x51, 0x00,
	3,
	DTYPE_DCS_WRITE1, 0x53, 0x24,
	3,
	DTYPE_DCS_WRITE1, 0x55, 0x00,
	3,
	DTYPE_DCS_WRITE1, 0x5E, 0x00,
	0,
};

static struct dsi_cmd_desc NOVATEK_SONY_MIPI_INIT[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 100, 5, (char[]){0xFF, 0xAA, 0x55, 0x25, 0x01} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 39, (char[]){0xF8, 0x02, 0x02, 0x00, 0x20, 0x33, 0x00, 0x00, 0x40, 0x00, 0x00, 0x23, 0x02, 0x99, 0xC8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x23, 0x0F, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x05, 0x05, 0x00, 0x34, 0x00, 0x04, 0x55, 0x00, 0x04, 0x11, 0x38} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 9, (char[]){0xF3, 0x00, 0x32, 0x00, 0x38, 0x31, 0x08, 0x11, 0x00} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 6, (char[]){0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 3, (char[]){0xB1, 0xEC, 0x0A} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xB6, 0x07} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 3, (char[]){0xB7, 0x00, 0x73} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 5, (char[]){0xB8, 0x01, 0x04, 0x06, 0x05} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 3, (char[]){0xB9, 0x00, 0x00} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xBA, 0x02} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xBB, 0x88, 0x08, 0x88} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xBC, 0x01, 0x01, 0x01} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 6, (char[]){0xBD, 0x01, 0x84, 0x1D, 0x1C, 0x00} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 6, (char[]){0xBE, 0x01, 0x84, 0x1D, 0x1C, 0x00} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 6, (char[]){0xBF, 0x01, 0x84, 0x1D, 0x1C, 0x00} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 13, (char[]){0xCA, 0x01, 0xE4, 0xE4, 0xE4, 0xE4, 0xE4, 0xE4, 0xE4, 0x08, 0x08, 0x00, 0x01} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 3, (char[]){0xE0, 0x01, 0x03} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 6, (char[]){0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB0, 0x0A, 0x0A, 0x0A} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB1, 0x0A, 0x0A, 0x0A} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB2, 0x02, 0x02, 0x02} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB3, 0x08, 0x08, 0x08} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB5, 0x05, 0x05, 0x05} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB6, 0x45, 0x45, 0x45} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB7, 0x25, 0x25, 0x25} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB8, 0x36, 0x36, 0x36} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xB9, 0x36, 0x36, 0x36} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 4, (char[]){0xBA, 0x15, 0x15, 0x15} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xBF, 0x01} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xC2, 0x01} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 8, (char[]){0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, 2, (char[]){0x11, 0x00} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0x2C, 0x00} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0x35, 0x00} },
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, 3, (char[]){0x44, 0x02, 0xCF} },
};


/* -----------------------------------------------------------------------------
 *                         External routine declaration
 * -----------------------------------------------------------------------------
 */

static struct msm_panel_common_pdata *mipi_pyramid_pdata;

static int mipi_pyramid_lcd_init(void);

/* novatek blue panel */
static char led_pwm1[] =
{
	0x51, 0x0,
};

static char bkl_enable_cmds[] = {0x53, 0x24};/* DTYPE_DCS_WRITE1 */ /* bkl on and no dim */

static char sw_reset[2] = {0x01, 0x00}; /* DTYPE_DCS_WRITE */
static char enter_sleep[2] = {0x10, 0x00}; /* DTYPE_DCS_WRITE */
static char exit_sleep[2] = {0x11, 0x00}; /* DTYPE_DCS_WRITE */
static char display_off[2] = {0x28, 0x00}; /* DTYPE_DCS_WRITE */
static char display_on[2] = {0x29, 0x00}; /* DTYPE_DCS_WRITE */
static char enable_te[2] = {0x35, 0x00};/* DTYPE_DCS_WRITE1 */
static char test_reg[3] = {0x44, 0x02, 0xCF};/* DTYPE_DCS_LWRITE */
static char test_reg_qhd[3] = {0x44, 0x01, 0x3f};/* DTYPE_DCS_LWRITE */ /* 479:1b7; 319:13f; 479:1df */
static char test_reg_ruy_shp[3] = {0x44, 0x01, 0x68};/* DTYPE_DCS_LWRITE */
static char test_reg_ruy_auo[3] = {0x44, 0x01, 0x68};/* DTYPE_DCS_LWRITE */

static char set_twolane[2] = {0xae, 0x03}; /* DTYPE_DCS_WRITE1 */
static char rgb_888[2] = {0x3A, 0x77}; /* DTYPE_DCS_WRITE1 */
/* commands by Novatke */
static char novatek_f4[2] = {0xf4, 0x55}; /* DTYPE_DCS_WRITE1 */
static char novatek_8c[16] = { /* DTYPE_DCS_LWRITE */
	0x8C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x08, 0x00, 0x30, 0xC0, 0xB7, 0x37};
static char novatek_ff[2] = {0xff, 0x55 }; /* DTYPE_DCS_WRITE1 */

static char set_width[5] = { /* DTYPE_DCS_LWRITE */
	0x2A, 0x00, 0x00, 0x02, 0x1B}; /* 540 - 1 */
static char set_height[5] = { /* DTYPE_DCS_LWRITE */
	0x2B, 0x00, 0x00, 0x03, 0xBF}; /* 960 - 1 */

static char novatek_pwm_f3[2] = {0xF3, 0xAA }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_00[2] = {0x00, 0x01 }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_21[2] = {0x21, 0x20 }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_22[2] = {0x22, 0x03 }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_7d[2] = {0x7D, 0x01 }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_7f[2] = {0x7F, 0xAA }; /* DTYPE_DCS_WRITE1 */

static char novatek_pwm_cp[2] = {0x09, 0x34 }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_cp2[2] = {0xc9, 0x01 }; /* DTYPE_DCS_WRITE1 */
static char novatek_pwm_cp3[2] = {0xff, 0xaa }; /* DTYPE_DCS_WRITE1 */

static char maucctr_0[6] = {0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00};/* DTYPE_DCS_LWRITE */
static char maucctr_1[6] = {0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01}; /* DTYPE_DCS_LWRITE */
static char novatek_b5x[4] = {0xB5, 0x05, 0x05, 0x05}; /* DTYPE_DCS_LWRITE */
static char novatek_ce[8] = {0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /* DTYPE_DCS_LWRITE */
static char novatek_e0[3] = {0xE0, 0x01, 0x03}; /* DTYPE_DCS_LWRITE */

static struct dsi_cmd_desc novatek_wvga_c3_cmd_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(maucctr_0), maucctr_0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(novatek_e0), novatek_e0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(maucctr_1), maucctr_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(novatek_b5x), novatek_b5x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(novatek_ce), novatek_ce},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg), test_reg},
};

#ifdef HTC_USED_0_3_MIPI_INIT
static struct dsi_cmd_desc novatek_wvga_cmd_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 100,
		sizeof(lv3), lv3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_f8), novatek_f8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_f3), novatek_f3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(maucctr_0), maucctr_0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b1), novatek_b1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(novatek_b6), novatek_b6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b7), novatek_b7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b8), novatek_b8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b9), novatek_b9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_ba), novatek_ba},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_bb), novatek_bb},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_bc), novatek_bc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_bd), novatek_bd},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_be), novatek_be},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_bf), novatek_bf},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_ca), novatek_ca},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_e0), novatek_e0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(maucctr_1), maucctr_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b0x), novatek_b0x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b1x), novatek_b1x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b2x), novatek_b2x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b3x), novatek_b3x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b5x), novatek_b5x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b6x), novatek_b6x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b7x), novatek_b7x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b8x), novatek_b8x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_b9x), novatek_b9x},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_bax), novatek_bax},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(novatek_bfx), novatek_bfx},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(btenctr_1), btenctr_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(novatek_ce), novatek_ce},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(memory_start), memory_start},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(enable_te), enable_te},
	{DTYPE_DCS_WRITE, 1, 0, 0, 40,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm1), led_pwm1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm3), led_pwm3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(novatek_5e), novatek_5e},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm2), led_pwm2},
};
#endif /* HTC_USED_0_3_MIPI_INIT */

/* Gamma for cut 1 */
/* R+ */
static char ruy_shp_gamma1_d1[] = {
	0xD1, 0x00, 0x6D, 0x00,	0x76, 0x00, 0x88, 0x00,
	0x97, 0x00, 0xA5, 0x00,	0xBD, 0x00, 0xD0, 0x00,
	0xEE
};
static char ruy_shp_gamma1_d2[] = {
	0xD2, 0x01, 0x06, 0x01,	0x2B, 0x01, 0x46, 0x01,
	0x6B, 0x01, 0x83, 0x01,	0x84, 0x01, 0xA1, 0x01,
	0xC4
};
static char ruy_shp_gamma1_d3[] = {
	0xD3, 0x01, 0xD3, 0x01,	0xE4, 0x01, 0xF3, 0x02,
	0x0F, 0x02, 0x28, 0x02,	0x65, 0x02, 0x87, 0x02,
	0x95
};
static char ruy_shp_gamma1_d4[] = {
	0xD4, 0x02, 0x99, 0x02,	0x99
};
/* G+ */
static char ruy_shp_gamma1_d5[] = {
	0xD5, 0x00, 0xBE, 0x00,	0xC5, 0x00, 0xD2, 0x00,
	0xDD, 0x00, 0xE7, 0x00,	0xF9, 0x01, 0x09, 0x01,
	0x23
};
static char ruy_shp_gamma1_d6[] = {
	0xD6, 0x01, 0x35, 0x01,	0x4D, 0x01, 0x5F, 0x01,
	0x7A, 0x01, 0x93, 0x01,	0x93, 0x01, 0xA8, 0x01,
	0xC9
};
static char ruy_shp_gamma1_d7[] = {
	0xD7, 0x01, 0xD8, 0x01,	0xE8, 0x01, 0xF6, 0x02,
	0x11, 0x02, 0x29, 0x02,	0x49, 0x02, 0x71, 0x02,
	0x91
};
static char ruy_shp_gamma1_d8[] = {
	0xD8, 0x02, 0x99, 0x02,	0x99
};
/* B+ */
static char ruy_shp_gamma1_d9[] = {
	0xD9, 0x00, 0x84, 0x00,	0x93, 0x00, 0xAE, 0x00,
	0xC5, 0x00, 0xD6, 0x00,	0xF1, 0x01, 0x08, 0x01,
	0x29
};
static char ruy_shp_gamma1_dd[] = {
	0xDD, 0x01, 0x3D, 0x01,	0x5A, 0x01, 0x6D, 0x01,
	0x81, 0x01, 0x97, 0x01,	0x97, 0x01, 0xAC, 0x01,
	0xCD
};
static char ruy_shp_gamma1_de[] = {
	0xDE, 0x01, 0xDD, 0x01,	0xE7, 0x01, 0xF6, 0x02,
	0x0E, 0x02, 0x34, 0x02,	0x6A, 0x02, 0x75, 0x02,
	0x90
};
static char ruy_shp_gamma1_df[] = {
	0xDF, 0x02, 0x99, 0x02,	0x99
};
/* R- */
static char ruy_shp_gamma1_e0[] = {
	0xE0, 0x00, 0x8D, 0x00,	0x99, 0x00, 0xB0, 0x00,
	0xC5, 0x00, 0xD8, 0x00,	0xF9, 0x01, 0x14, 0x01,
	0x3E
};
static char ruy_shp_gamma1_e1[] = {
	0xE1, 0x01, 0x60, 0x01,	0x96, 0x01, 0xBF, 0x01,
	0xF8, 0x02, 0x20, 0x02,	0x22, 0x02, 0x57, 0x02,
	0x9C
};
static char ruy_shp_gamma1_e2[] = {
	0xE2, 0x02, 0xBE, 0x02,	0xE7, 0x03, 0x0C, 0x03,
	0x40, 0x03, 0x64, 0x03,	0xAB, 0x03, 0xD4, 0x03,
	0xE8
};
static char ruy_shp_gamma1_e3[] = {
	0xE3, 0x03, 0xED, 0x03,	0xEE
};
/* G- */
static char ruy_shp_gamma1_e4[] = {
	0xE4, 0x00, 0xFB, 0x01,	0x04, 0x01, 0x16, 0x01,
	0x26, 0x01, 0x34, 0x01,	0x4E, 0x01, 0x65, 0x01,
	0x8A
};
static char ruy_shp_gamma1_e5[] = {
	0xE5, 0x01, 0xA4, 0x01,	0xC9, 0x01, 0xE5, 0x02,
	0x11, 0x02, 0x3C, 0x02,	0x3D, 0x02, 0x63, 0x02,
	0xA9
};
static char ruy_shp_gamma1_e6[] = {
	0xE6, 0x02, 0xCB, 0x02,	0xF0, 0x03, 0x14, 0x03,
	0x44, 0x03, 0x64, 0x03,	0x8B, 0x03, 0xB9, 0x03,
	0xE2
};
static char ruy_shp_gamma1_e7[] = {
	0xE7, 0x03, 0xED, 0x03, 0xEE
};
/* B- */
static char ruy_shp_gamma1_e8[] = {
	0xE8, 0x00, 0xAB, 0x00,	0xC0, 0x00, 0xE5, 0x01,
	0x04, 0x01, 0x1C, 0x01,	0x43, 0x01, 0x62, 0x01,
	0x92
};
static char ruy_shp_gamma1_e9[] = {
	0xE9, 0x01, 0xB1, 0x01,	0xDD, 0x01, 0xFB, 0x02,
	0x1D, 0x02, 0x43, 0x02,	0x44, 0x02, 0x6C, 0x02,
	0xB0
};
static char ruy_shp_gamma1_ea[] = {
	0xEA, 0x02, 0xD5, 0x02,	0xED, 0x03, 0x12, 0x03,
	0x3F, 0x03, 0x73, 0x03,	0xB0, 0x03, 0xBD, 0x03,
	0xE0
};
static char ruy_shp_gamma1_eb[] = {
	0xEB, 0x03, 0xED, 0x03,	0xEE
};

/* Gamma for cut 2 */
/* R+ */
static char ruy_shp_gamma2_d1[] = {
	0xD1, 0x00, 0x6D, 0x00,	0x76, 0x00, 0x88, 0x00,
	0x97, 0x00, 0xA5, 0x00,	0xBD, 0x00, 0xD0, 0x00,
	0xEE
};
static char ruy_shp_gamma2_d2[] = {
	0xD2, 0x01, 0x06, 0x01,	0x2B, 0x01, 0x46, 0x01,
	0x6B, 0x01, 0x83, 0x01,	0x84, 0x01, 0xA1, 0x01,
	0xC4
};
static char ruy_shp_gamma2_d3[] = {
	0xD3, 0x01, 0xD3, 0x01,	0xE4, 0x01, 0xF3, 0x02,
	0x0F, 0x02, 0x28, 0x02,	0x65, 0x02, 0x87, 0x02,
	0x95
};
static char ruy_shp_gamma2_d4[] = {
	0xD4, 0x02, 0x99, 0x02,	0x99
};
/* G+ */
static char ruy_shp_gamma2_d5[] = {
	0xD5, 0x00, 0xBE, 0x00,	0xC5, 0x00, 0xD2, 0x00,
	0xDD, 0x00, 0xE7, 0x00,	0xF9, 0x01, 0x09, 0x01,
	0x23
};
static char ruy_shp_gamma2_d6[] = {
	0xD6, 0x01, 0x35, 0x01,	0x4D, 0x01, 0x5F, 0x01,
	0x7A, 0x01, 0x93, 0x01,	0x93, 0x01, 0xA8, 0x01,
	0xC9
};
static char ruy_shp_gamma2_d7[] = {
	0xD7, 0x01, 0xD8, 0x01,	0xE8, 0x01, 0xF6, 0x02,
	0x11, 0x02, 0x29, 0x02,	0x49, 0x02, 0x71, 0x02,
	0x91
};
static char ruy_shp_gamma2_d8[] = {
	0xD8, 0x02, 0x99, 0x02,	0x99
};
/* B+ */
static char ruy_shp_gamma2_d9[] = {
	0xD9, 0x00, 0x84, 0x00,	0x93, 0x00, 0xAE, 0x00,
	0xC5, 0x00, 0xD6, 0x00,	0xF1, 0x01, 0x08, 0x01,
	0x29
};
static char ruy_shp_gamma2_dd[] = {
	0xDD, 0x01, 0x3D, 0x01,	0x5A, 0x01, 0x6D, 0x01,
	0x81, 0x01, 0x97, 0x01,	0x97, 0x01, 0xAC, 0x01,
	0xCD
};
static char ruy_shp_gamma2_de[] = {
	0xDE, 0x01, 0xDD, 0x01,	0xE7, 0x01, 0xF6, 0x02,
	0x0E, 0x02, 0x34, 0x02,	0x6A, 0x02, 0x75, 0x02,
	0x90
};
static char ruy_shp_gamma2_df[] = {
	0xDF, 0x02, 0x99, 0x02,	0x99
};
/* R- */
static char ruy_shp_gamma2_e0[] = {
	0xE0, 0x00, 0x8D, 0x00,	0x99, 0x00, 0xB0, 0x00,
	0xC5, 0x00, 0xD8, 0x00,	0xF9, 0x01, 0x14, 0x01,
	0x3E
};
static char ruy_shp_gamma2_e1[] = {
	0xE1, 0x01, 0x60, 0x01,	0x96, 0x01, 0xBF, 0x01,
	0xF8, 0x02, 0x20, 0x02,	0x22, 0x02, 0x57, 0x02,
	0x9C
};
static char ruy_shp_gamma2_e2[] = {
	0xE2, 0x02, 0xBE, 0x02,	0xE7, 0x03, 0x0C, 0x03,
	0x40, 0x03, 0x64, 0x03,	0xAB, 0x03, 0xD4, 0x03,
	0xE8
};
static char ruy_shp_gamma2_e3[] = {
	0xE3, 0x03, 0xED, 0x03,	0xEE
};
/* G- */
static char ruy_shp_gamma2_e4[] = {
	0xE4, 0x00, 0xFB, 0x01,	0x04, 0x01, 0x16, 0x01,
	0x26, 0x01, 0x34, 0x01,	0x4E, 0x01, 0x65, 0x01,
	0x8A
};
static char ruy_shp_gamma2_e5[] = {
	0xE5, 0x01, 0xA4, 0x01,	0xC9, 0x01, 0xE5, 0x02,
	0x11, 0x02, 0x3C, 0x02,	0x3D, 0x02, 0x63, 0x02,
	0xA9
};
static char ruy_shp_gamma2_e6[] = {
	0xE6, 0x02, 0xCB, 0x02,	0xF0, 0x03, 0x14, 0x03,
	0x44, 0x03, 0x64, 0x03,	0x8B, 0x03, 0xB9, 0x03,
	0xE2
};
static char ruy_shp_gamma2_e7[] = {
	0xE7, 0x03, 0xED, 0x03, 0xEE
};
/* B- */
static char ruy_shp_gamma2_e8[] = {
	0xE8, 0x00, 0xAB, 0x00,	0xC0, 0x00, 0xE5, 0x01,
	0x04, 0x01, 0x1C, 0x01,	0x43, 0x01, 0x62, 0x01,
	0x92
};
static char ruy_shp_gamma2_e9[] = {
	0xE9, 0x01, 0xB1, 0x01,	0xDD, 0x01, 0xFB, 0x02,
	0x1D, 0x02, 0x43, 0x02,	0x44, 0x02, 0x6C, 0x02,
	0xB0
};
static char ruy_shp_gamma2_ea[] = {
	0xEA, 0x02, 0xD5, 0x02,	0xED, 0x03, 0x12, 0x03,
	0x3F, 0x03, 0x73, 0x03,	0xB0, 0x03, 0xBD, 0x03,
	0xE0
};
static char ruy_shp_gamma2_eb[] = {
	0xEB, 0x03, 0xED, 0x03,	0xEE
};

static struct dsi_cmd_desc ruy_shp_cmd_on_cmds[] = {
	/* added by our own */
	{ DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		5, (char[]){ 0xFF, 0xAA, 0x55, 0x25, 0x01 } },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		17, (char[]){ 0xFA, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00,
					  0x00, 0x03, 0x20, 0x12,
					  0x20, 0xFF, 0xFF, 0xFF } },/* 90Hz -> 60Hz */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		5, (char[]){ 0xF3, 0x03, 0x03, 0x07, 0x14 } },/* vertical noise*/

	{ DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep },

	/* page 0 */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00 } },/* select page 0 */

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		2, (char[]){ 0xB1, 0xFC } },/* display option */
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		2, (char[]){ 0xB6, 0x07 } },/* output data hold time */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		3, (char[]){ 0xB7, 0x00, 0x00 } },/* EQ gate signal */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		5, (char[]){ 0xB8, 0x00, 0x07, 0x07, 0x07 } },/* EQ source driver */

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		2, (char[]){ 0xBA, 0x02 } },/* vertical porch */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		4, (char[]){ 0xBB, 0x83, 0x03, 0x83 } },/* source driver (vertical noise) */

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		2, (char[]){ 0xBC, 0x02 } },/* inversion driving */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		5, (char[]){ 0xBD, 0x01, 0x4B, 0x08, 0x26 } },/* timing control (normal mode) */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		12, (char[]){ 0xC7, 0x00, 0x0F, 0x0F, 0x06, 0x07, 0x09, 0x0A, 0x0A, 0x0A, 0xF0, 0xF0 } },/* timing control */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		ARRAY_SIZE(novatek_e0), novatek_e0 },/* PWM frequency = 13kHz */

	/* page 1 */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01 } },/* select page 1 */

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB0, 0x1F} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB1, 0x1F} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB3, 0x0D} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB4, 0x0F} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB6, 0x44} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB7, 0x24} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xB9, 0x27} },
	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){ 0xBA, 0x24} },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		4, (char[]){ 0xBC, 0x00, 0xC8, 0x00 } },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		4, (char[]){ 0xBD, 0x00, 0x78, 0x00 } },

	/* enter gamma table */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d1), ruy_shp_gamma1_d1 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d2), ruy_shp_gamma1_d2 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d3), ruy_shp_gamma1_d3 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d4), ruy_shp_gamma1_d4 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d5), ruy_shp_gamma1_d5 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d6), ruy_shp_gamma1_d6 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d7), ruy_shp_gamma1_d7 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d8), ruy_shp_gamma1_d8 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_d9), ruy_shp_gamma1_d9 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_dd), ruy_shp_gamma1_dd },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_de), ruy_shp_gamma1_de },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_df), ruy_shp_gamma1_df },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e0), ruy_shp_gamma1_e0 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e1), ruy_shp_gamma1_e1 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e2), ruy_shp_gamma1_e2 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e3), ruy_shp_gamma1_e3 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e4), ruy_shp_gamma1_e4 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e5), ruy_shp_gamma1_e5 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e6), ruy_shp_gamma1_e6 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e7), ruy_shp_gamma1_e7 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e8), ruy_shp_gamma1_e8 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_e9), ruy_shp_gamma1_e9 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_ea), ruy_shp_gamma1_ea },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma1_eb), ruy_shp_gamma1_eb },
	/* leave gamma table */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x00, 0x00 } },/* select page 0 */
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_ruy_shp), test_reg_ruy_shp },

	{ DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height },
};

static struct dsi_cmd_desc ruy_shp_c2_cmd_on_cmds[] = {
	/* added by our own */
	{ DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		5, (char[]){ 0xFF, 0xAA, 0x55, 0x25, 0x01 } },

	{ DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep },

	/* page 0 */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00 } },/* select page 0 */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		ARRAY_SIZE(novatek_e0), novatek_e0 },/* PWM frequency = 13kHz */

	/* page 1 */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01 } },/* select page 1 */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		4, (char[]){ 0xBC, 0x00, 0xC0, 0x00 } },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		4, (char[]){ 0xBD, 0x00, 0x70, 0x00 } },

	/* enter gamma table */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d1), ruy_shp_gamma2_d1 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d2), ruy_shp_gamma2_d2 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d3), ruy_shp_gamma2_d3 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d4), ruy_shp_gamma2_d4 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d5), ruy_shp_gamma2_d5 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d6), ruy_shp_gamma2_d6 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d7), ruy_shp_gamma2_d7 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d8), ruy_shp_gamma2_d8 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_d9), ruy_shp_gamma2_d9 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_dd), ruy_shp_gamma2_dd },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_de), ruy_shp_gamma2_de },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_df), ruy_shp_gamma2_df },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e0), ruy_shp_gamma2_e0 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e1), ruy_shp_gamma2_e1 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e2), ruy_shp_gamma2_e2 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e3), ruy_shp_gamma2_e3 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e4), ruy_shp_gamma2_e4 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e5), ruy_shp_gamma2_e5 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e6), ruy_shp_gamma2_e6 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e7), ruy_shp_gamma2_e7 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e8), ruy_shp_gamma2_e8 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_e9), ruy_shp_gamma2_e9 },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_ea), ruy_shp_gamma2_ea },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0, ARRAY_SIZE(ruy_shp_gamma2_eb), ruy_shp_gamma2_eb },
	/* leave gamma table */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x00, 0x00 } },/* select page 0 */
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_ruy_shp), test_reg_ruy_shp },

	{ DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height },
};

static struct dsi_cmd_desc ruy_shp_c2o_cmd_on_cmds[] = {
	/* added by our own */
	{ DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		5, (char[]){ 0xFF, 0xAA, 0x55, 0x25, 0x01 } },

	{ DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep },

	/* page 0 */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00 } },/* select page 0 */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		ARRAY_SIZE(novatek_e0), novatek_e0 },/* PWM frequency = 13kHz */

	/* page 1 */
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01 } },/* select page 1 */

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		6, (char[]){ 0xF0, 0x55, 0xAA, 0x52, 0x00, 0x00 } },/* select page 0 */
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},

	{ DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_ruy_shp), test_reg_ruy_shp },

	{ DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize },

	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width },
	{ DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height },
};

/* Gamma table of PYD, sharp panel */
static char pyd_sharp_gm[] = {
	/* into gamma page */
	0xf3, 0xaa,
	/* R positive */
	0x24, 0x00, 0x25, 0x04, 0x26, 0x11, 0x27, 0x1c, 0x28, 0x1a, 0x29, 0x2e,
	0x2a, 0x5e, 0x2b, 0x21, 0x2d, 0x1f, 0x2f, 0x27, 0x30, 0x60, 0x31, 0x15,
	0x32, 0x3e, 0x33, 0x5f, 0x34, 0x7c, 0x35, 0x86, 0x36, 0x87, 0x37, 0x08,
	/* R negative */
	0x38, 0x01, 0x39, 0x06, 0x3a, 0x14, 0x3b, 0x21, 0x3d, 0x1a, 0x3f, 0x2d,
	0x40, 0x5f, 0x41, 0x33, 0x42, 0x20, 0x43, 0x27, 0x44, 0x7b, 0x45, 0x15,
	0x46, 0x3e, 0x47, 0x5f, 0x48, 0xa7, 0x49, 0xb3, 0x4a, 0xb4, 0x4b, 0x35,
	/* G positive */
	0x4c, 0x2a, 0x4d, 0x2d, 0x4e, 0x36, 0x4f, 0x3e, 0x50, 0x18, 0x51, 0x2a,
	0x52, 0x5c, 0x53, 0x2c, 0x54, 0x1d, 0x55, 0x25, 0x56, 0x65, 0x57, 0x12,
	0x58, 0x3a, 0x59, 0x57, 0x5a, 0x93, 0x5b, 0xb2, 0x5c, 0xb6, 0x5d, 0x37,
	/* G negative */
	0x5e, 0x30, 0x5f, 0x34, 0x60, 0x3e, 0x61, 0x46, 0x62, 0x19, 0x63, 0x2b,
	0x64, 0x5c, 0x65, 0x3f, 0x66, 0x1f, 0x67, 0x26, 0x68, 0x80, 0x69, 0x13,
	0x6a, 0x3c, 0x6b, 0x57, 0x6c, 0xc0, 0x6d, 0xe2, 0x6e, 0xe7, 0x6f, 0x68,
	/* B positive */
	0x70, 0x00, 0x71, 0x0a, 0x72, 0x26, 0x73, 0x37, 0x74, 0x1e, 0x75, 0x32,
	0x76, 0x60, 0x77, 0x32, 0x78, 0x1f, 0x79, 0x26, 0x7a, 0x68, 0x7b, 0x14,
	0x7c, 0x39, 0x7d, 0x59, 0x7e, 0x85, 0x7f, 0x86, 0x80, 0x87, 0x81, 0x08,
	/* B negative */
	0x82, 0x01, 0x83, 0x0c, 0x84, 0x2b, 0x85, 0x3e, 0x86, 0x1f, 0x87, 0x33,
	0x88, 0x61, 0x89, 0x45, 0x8a, 0x1f, 0x8b, 0x26, 0x8c, 0x84, 0x8d, 0x14,
	0x8e, 0x3a, 0x8f, 0x59, 0x90, 0xb1, 0x91, 0xb3, 0x92, 0xb4, 0x93, 0x35,
	0xc9, 0x01,
	/* out of gamma page */
	0xff, 0xaa,
};

static struct dsi_cmd_desc pyd_sharp_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 8},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 24},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 28},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 30},/* don't change this line */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 32},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 34},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 38},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 40},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 42},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 44},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 46},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 48},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 50},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 52},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 54},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 56},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 58},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 60},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 62},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 64},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 66},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 68},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 70},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 72},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 74},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 76},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 78},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 80},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 82},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 84},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 86},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 88},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 90},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 92},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 94},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 96},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 98},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 130},/* don't change this line */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 220},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_00), novatek_pwm_00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_21), novatek_pwm_21},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_22), novatek_pwm_22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7d), novatek_pwm_7d},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7f), novatek_pwm_7f},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp), novatek_pwm_cp},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg), test_reg},
	{DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_f4), novatek_f4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(novatek_8c), novatek_8c},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_ff), novatek_ff},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(set_twolane), set_twolane},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(rgb_888), rgb_888},
};

/* Gamma table of PYD, sharp panel */
static char pyd_auo_gm[] = {
	/* into gamma page */
	0xf3, 0xaa,
	/* R positive */
	0x24, 0X63, 0X25, 0X6B, 0X26, 0X78, 0X27, 0X7E, 0X28, 0X19, 0X29, 0X2E,
	0x2A, 0X61, 0X2B, 0X61, 0X2D, 0X1b, 0X2F, 0X22, 0X30, 0X84, 0X31, 0X1B,
	0x32, 0X4F, 0X33, 0X63, 0X34, 0X28, 0X35, 0XDF, 0X36, 0XC9, 0X37, 0X69,
	/* R negative */
	0x38, 0X63, 0X39, 0X6B, 0X3A, 0X78, 0X3B, 0X7E, 0X3D, 0X19, 0X3F, 0X2E,
	0x40, 0X61, 0X41, 0X61, 0X42, 0X1b, 0X43, 0X22, 0X44, 0X84, 0X45, 0X1B,
	0x46, 0X4F, 0X47, 0X63, 0X48, 0XC7, 0X49, 0XDF, 0X4A, 0XC9, 0X4B, 0X69,
	/* G positive */
	0x4C, 0X45, 0X4D, 0X54, 0X4E, 0X64, 0X4F, 0X75, 0X50, 0X18, 0X51, 0X2E,
	0x52, 0X62, 0X53, 0X61, 0X54, 0X1D, 0X55, 0X26, 0X56, 0X9D, 0X57, 0X10,
	0x58, 0X39, 0X59, 0X55, 0X5A, 0XC3, 0X5B, 0XD7, 0X5C, 0XFF, 0X5D, 0X6B,
	/* G negative */
	0x5E, 0X45, 0X5F, 0X54, 0X60, 0X64, 0X61, 0X75, 0X62, 0X18, 0X63, 0X2E,
	0x64, 0X62, 0X65, 0X61, 0X66, 0X1D, 0X67, 0X26, 0X68, 0X65, 0X69, 0X10,
	0x6A, 0X39, 0X6B, 0X55, 0X6C, 0XC3, 0X6D, 0XD7, 0X6E, 0XFF, 0X6F, 0X6B,
	/* B positive */
	0x70, 0X7D, 0X71, 0X82, 0X72, 0X89, 0X73, 0X97, 0X74, 0X19, 0X75, 0X2E,
	0x76, 0X61, 0X77, 0X6E, 0X78, 0X1A, 0X79, 0X1E, 0X7A, 0X8E, 0X7B, 0X0C,
	0x7C, 0X27, 0X7D, 0X58, 0X7E, 0XCF, 0X7F, 0XD9, 0X80, 0XFc, 0X81, 0X68,
	/* B negative */
	0x82, 0X7D, 0X83, 0X82, 0X84, 0X89, 0X85, 0X97, 0X86, 0X19, 0X87, 0X2E,
	0x88, 0X61, 0X89, 0X6E, 0X8A, 0X1A, 0X8B, 0X1E, 0X8C, 0X8E, 0X8D, 0X0C,
	0x8E, 0X27, 0X8F, 0X58, 0X90, 0XCF, 0X91, 0XD9, 0X92, 0XFc, 0X93, 0X68,
	/* out of gamma page */
	0xC9, 0x01,
	0xff, 0xaa,
};

/* Gamma table of RIR, sharp panel */
static char rir_shp_gm[] = {
       0xf3, 0xaa,
       /* 2.5 gamma */
       /* R + */
       0x24, 0x00, 0x25, 0x03, 0x26, 0x0e, 0x27, 0x19, 0x28, 0x18, 0x29, 0x2c,
       0x2A, 0x5d, 0x2B, 0x18, 0x2D, 0x1f, 0x2F, 0x26, 0x30, 0x58, 0x31, 0x16,
       0x32, 0x3a, 0x33, 0x53, 0x34, 0x69, 0x35, 0x6f, 0x36, 0x71, 0x37, 0x08,
       /* R - */
       0x38, 0x01, 0x39, 0x05, 0x3A, 0x11, 0x3B, 0x1d, 0x3D, 0x18, 0x3F, 0x2c,
       0x40, 0x5d, 0x41, 0x29, 0x42, 0x1f, 0x43, 0x26, 0x44, 0x72, 0x45, 0x15,
       0x46, 0x3a, 0x47, 0x53, 0x48, 0x93, 0x49, 0x9a, 0x4A, 0x9c, 0x4B, 0x35,
       /* G + */
       0x4C, 0x17, 0x4D, 0x1A, 0x4E, 0x24, 0x4F, 0x2d, 0x50, 0x19, 0x51, 0x2c,
       0x52, 0x5d, 0x53, 0x21, 0x54, 0x1E, 0x55, 0x25, 0x56, 0x5d, 0x57, 0x13,
       0x58, 0x37, 0x59, 0x4f, 0x5A, 0x78, 0x5B, 0xaa, 0x5C, 0xb6, 0x5D, 0x37,
       /* G - */
       0x5E, 0x1A, 0x5F, 0x1e, 0x60, 0x29, 0x61, 0x33, 0x62, 0x19, 0x63, 0x2c,
       0x64, 0x5d, 0x65, 0x33, 0x66, 0x1f, 0x67, 0x26, 0x68, 0x77, 0x69, 0x14,
       0x6A, 0x38, 0x6B, 0x4f, 0x6C, 0xa3, 0x6D, 0xdb, 0x6E, 0xe8, 0x6F, 0x68,
       /* B + */
       0x70, 0x4B, 0x71, 0x4d, 0x72, 0x55, 0x73, 0x5a, 0x74, 0x17, 0x75, 0x2a,
       0x76, 0x5b, 0x77, 0x32, 0x78, 0x1E, 0x79, 0x25, 0x7A, 0x64, 0x7B, 0x19,
       0x7C, 0x4c, 0x7D, 0x62, 0x7E, 0x53, 0x7F, 0x67, 0x80, 0x71, 0x81, 0x08,
       /* B - */
       0x82, 0x54, 0x83, 0x57, 0x84, 0x5f, 0x85, 0x65, 0x86, 0x17, 0x87, 0x29,
       0x88, 0x5a, 0x89, 0x47, 0x8A, 0x1d, 0x8B, 0x25, 0x8C, 0x7f, 0x8D, 0x1a,
       0x8E, 0x4c, 0x8F, 0x62, 0x90, 0x7a, 0x91, 0x90, 0x92, 0x9B, 0x93, 0x35,

       /* 2.x gamma */

       0xff, 0xaa,
};

static char rir_auo_gm[] = {
       0xf3, 0xaa,
       /* 2.5 gamma */
       /* R + */
       0x24, 0x00, 0x25, 0x0C, 0x26, 0x27, 0x27, 0x39, 0x28, 0x1B, 0x29, 0x2E,
       0x2A, 0x5D, 0x2B, 0x43, 0x2D, 0x20, 0x2F, 0x28, 0x30, 0x80, 0x31, 0x0F,
       0x32, 0x24, 0x33, 0x46, 0x34, 0xB8, 0x35, 0xDE, 0x36, 0xEF, 0x37, 0x6B,
       /* R - */
       0x38, 0x00, 0x39, 0x0C, 0x3A, 0x27, 0x3B, 0x39, 0x3D, 0x1B, 0x3F, 0x2E,
       0x40, 0x5D, 0x41, 0x43, 0x42, 0x20, 0x43, 0x28, 0x44, 0x80, 0x45, 0x0F,
       0x46, 0x24, 0x47, 0x46, 0x48, 0xB8, 0x49, 0xDE, 0x4A, 0xEF, 0x4B, 0x6B,
       /* G + */
       0x4C, 0x00, 0x4D, 0x0C, 0x4E, 0x20, 0x4F, 0x35, 0x50, 0x21, 0x51, 0x35,
       0x52, 0x64, 0x53, 0x53, 0x54, 0x20, 0x55, 0x28, 0x56, 0x88, 0x57, 0x0F,
       0x58, 0x27, 0x59, 0x46, 0x5A, 0xB8, 0x5B, 0xDE, 0x5C, 0xEF, 0x5D, 0x6B,
       /* G - */
       0x5E, 0x00, 0x5F, 0x0C, 0x60, 0x20, 0x61, 0x35, 0x62, 0x21, 0x63, 0x35,
       0x64, 0x64, 0x65, 0x53, 0x66, 0x20, 0x67, 0x28, 0x68, 0x88, 0x69, 0x0F,
       0x6A, 0x27, 0x6B, 0x46, 0x6C, 0xB8, 0x6D, 0xDE, 0x6E, 0xEF, 0x6F, 0x6B,
       /* B + */
       0x70, 0x00, 0x71, 0x0C, 0x72, 0x95, 0x73, 0x99, 0x74, 0x1B, 0x75, 0x2F,
       0x76, 0x5F, 0x77, 0x71, 0x78, 0x20, 0x79, 0x28, 0x7A, 0x96, 0x7B, 0x0F,
       0x7C, 0x29, 0x7D, 0x46, 0x7E, 0xB8, 0x7F, 0xDE, 0x80, 0xEF, 0x81, 0x6B,
       /* B - */
       0x82, 0x00, 0x83, 0x0C, 0x84, 0x95, 0x85, 0x99, 0x86, 0x1B, 0x87, 0x2F,
       0x88, 0x5F, 0x89, 0x71, 0x8A, 0x20, 0x8B, 0x28, 0x8C, 0x96, 0x8D, 0x0F,
       0x8E, 0x29, 0x8F, 0x46, 0x90, 0xB8, 0x91, 0xDE, 0x92, 0xEF, 0x93, 0x6B,

       /* 2.x gamma */
       0xC9, 0x01,
       0xff, 0xaa,
};

static struct dsi_cmd_desc pyd_auo_cmd_on_cmds[] = {
    {DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xf3, 0xaa} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA3, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA4, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA5, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA6, 0x01} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xFF, 0xAA} },

	/* Gamma table start */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 8},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 24},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 28},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 30},/* don't change this line! */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 32},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 34},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 38},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 40},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 42},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 44},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 46},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 48},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 50},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 52},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 54},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 56},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 58},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 60},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 62},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 64},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 66},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 68},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 70},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 72},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 74},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 76},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 78},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 80},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 82},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 84},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 86},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 88},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 90},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 92},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 94},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 96},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 98},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 130},/* don't change this line! */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 220},
	/* Gamma table end */
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
			sizeof(exit_sleep), exit_sleep},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_00), novatek_pwm_00},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_21), novatek_pwm_21},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_22), novatek_pwm_22},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7d), novatek_pwm_7d},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7f), novatek_pwm_7f},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp), novatek_pwm_cp},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_ruy_auo), test_reg_ruy_auo},
    {DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height},
};

static struct dsi_cmd_desc shp_novatek_cmd_on_cmds[] = {
       {DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset},
       {DTYPE_DCS_WRITE, 1, 0, 0, 150,
		sizeof(exit_sleep), exit_sleep},

       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm },
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 2},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 4},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 6},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 8},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 10},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 12},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 14},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 16},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 18},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 20},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 22},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 24},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 26},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 28},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 30},/* don't change this line! */
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 32},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 34},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 36},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 38},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 40},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 42},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 44},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 46},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 48},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 50},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 52},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 54},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 56},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 58},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 60},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 62},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 64},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 66},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 68},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 70},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 72},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 74},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 76},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 78},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 80},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 82},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 84},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 86},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 88},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 90},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 92},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 94},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 96},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 98},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 100},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 102},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 104},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 106},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 108},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 110},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 112},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 114},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 116},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 118},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 120},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 122},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 124},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 126},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 128},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 130},/* don't change this line! */
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 132},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 134},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 136},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 138},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 140},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 142},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 144},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 146},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 148},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 150},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 152},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 154},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 156},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 158},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 160},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 162},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 164},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 166},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 168},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 170},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 172},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 174},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 176},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 178},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 180},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 182},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 184},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 186},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 188},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 190},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 192},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 194},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 196},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 198},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 200},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 202},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 204},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 206},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 208},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 210},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 212},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 214},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_shp_gm + 216},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 50, 2, rir_shp_gm + 218},

       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_00), novatek_pwm_00},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_21), novatek_pwm_21},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_22), novatek_pwm_22},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7d), novatek_pwm_7d},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7f), novatek_pwm_7f},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp), novatek_pwm_cp},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
       {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_qhd), test_reg_qhd},
       {DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_f4), novatek_f4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(novatek_8c), novatek_8c},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_ff), novatek_ff},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(set_twolane), set_twolane},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(rgb_888), rgb_888},
};

static struct dsi_cmd_desc auo_nov_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xf3, 0xaa} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA3, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA4, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA5, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA6, 0x01} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xFF, 0xAA} },

	/* Gamma table start */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  8},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  24},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  28},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  30},/* don't change this line! */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  32},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  34},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  38},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  40},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  42},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  44},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  46},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  48},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  50},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  52},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  54},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  56},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  58},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  60},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  62},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  64},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  66},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  68},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  70},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  72},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  74},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  76},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  78},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  80},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  82},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  84},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  86},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  88},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  90},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm +  92},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 94},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 96},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 98},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 130},/* don't change this line! */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, rir_auo_gm + 220},
	/* Gamma table end */
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
			sizeof(exit_sleep), exit_sleep},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_f3), novatek_pwm_f3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_00), novatek_pwm_00},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_21), novatek_pwm_21},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_22), novatek_pwm_22},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_7d), novatek_pwm_7d},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_7f), novatek_pwm_7f},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_f3), novatek_pwm_f3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_cp), novatek_pwm_cp},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
	sizeof(enable_te), enable_te},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(test_reg), test_reg},
    {DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
	sizeof(max_pktsize), max_pktsize},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(set_width), set_width},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(set_height), set_height},
};

static struct dsi_cmd_desc shr_sharp_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_00), novatek_pwm_00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_21), novatek_pwm_21},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_22), novatek_pwm_22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7d), novatek_pwm_7d},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7f), novatek_pwm_7f},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp), novatek_pwm_cp},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
       {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(bkl_enable_cmds), bkl_enable_cmds},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_qhd), test_reg_qhd},
	{DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(set_twolane), set_twolane},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(rgb_888), rgb_888},
};

static struct dsi_cmd_desc novatek_display_off_cmds[] = {
		{DTYPE_DCS_WRITE, 1, 0, 0, 0,
			sizeof(display_off), display_off},
		{DTYPE_DCS_WRITE, 1, 0, 0, 110,
			sizeof(enter_sleep), enter_sleep}
};

static struct dsi_cmd_desc novatek_cmd_backlight_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(led_pwm1), led_pwm1},
};

static struct dsi_cmd_desc novatek_display_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

/* -----------------------------------------------------------------------------
 *                         Common Routine Implementation
 * -----------------------------------------------------------------------------
 */

void mipi_pyramid_panel_type_detect(void)
{
	if (panel_type == PANEL_ID_PYD_SHARP) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_PYD_SHARP\n", __func__);
		mipi_power_on_cmd = pyd_sharp_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(pyd_sharp_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_PYD_AUO_NT) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_PYD_AUO_NT\n", __func__);
		mipi_power_on_cmd = pyd_auo_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(pyd_auo_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_RUY_SHARP_NT) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_RUY_SHARP_NT\n", __func__);
		mipi_power_on_cmd = ruy_shp_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(ruy_shp_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_RUY_SHARP_NT_C2) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_RUY_SHARP_NT_C2\n", __func__);
		mipi_power_on_cmd = ruy_shp_c2_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(ruy_shp_c2_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_RUY_SHARP_NT_C2O) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_RUY_SHARP_NT_C2O\n", __func__);
		mipi_power_on_cmd = ruy_shp_c2o_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(ruy_shp_c2o_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_DOT_SONY) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_DOT_SONY\n", __func__);
		mipi_power_on_cmd = NOVATEK_SONY_MIPI_INIT;
		mipi_power_on_cmd_size = ARRAY_SIZE(NOVATEK_SONY_MIPI_INIT);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_DOT_SONY_C3) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_DOT_SONY_C3\n", __func__);
		mipi_power_on_cmd = novatek_wvga_c3_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(novatek_wvga_c3_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_RIR_SHARP_NT) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_RIR_SHARP_NT\n", __func__);
		mipi_power_on_cmd = shp_novatek_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(shp_novatek_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_RIR_AUO_NT) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_RIR_AUO_NT\n", __func__);
		mipi_power_on_cmd = auo_nov_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(auo_nov_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else if (panel_type == PANEL_ID_SHR_SHARP_NT) {
		PR_DISP_INFO("%s: panel_type=PANEL_ID_SHR_SHARP_NT\n", __func__);
		mipi_power_on_cmd = shr_sharp_cmd_on_cmds;
		mipi_power_on_cmd_size = ARRAY_SIZE(shr_sharp_cmd_on_cmds);
		mipi_power_off_cmd = novatek_display_off_cmds;
		mipi_power_off_cmd_size = ARRAY_SIZE(novatek_display_off_cmds);
	} else {
		printk(KERN_ERR "%s: panel_type=0x%x not support\n", __func__, panel_type);
	}
	return;
}

int mipi_lcd_on = 1;

static int mipi_pyramid_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

        printk(KERN_ERR "%s: ++\n", __func__);

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi  = &mfd->panel_info.mipi;

        if (mipi_lcd_on)
          return 0;

        mipi_pyramid_send_cmd(mipi_power_on_cmd, mipi_power_on_cmd_size, false);

	mipi_lcd_on = 1;

        printk(KERN_ERR "%s: --\n", __func__);
	return 0;
}

static int mipi_pyramid_display_on(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;
  unsigned int val = 0;

  mfd = platform_get_drvdata(pdev);

  mipi_pyramid_send_cmd(novatek_display_on_cmds, ARRAY_SIZE(novatek_display_on_cmds), true);

  val = mfd->bl_level;
  if (val == 0) {
    if (bl_level_prevset != 1) {
      val = bl_level_prevset;
      mfd->bl_level = val;
    } else {
      val = DEFAULT_BRIGHTNESS;
      mfd->bl_level = val;
    }
  }

  return 0;
}

static int mipi_pyramid_lcd_off(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;

  mfd = platform_get_drvdata(pdev);

  if (!mfd)
    return -ENODEV;
  if (mfd->key != MFD_KEY)
    return -EINVAL;

  if (!mipi_lcd_on)
    return 0;

  mfd->bl_level = 0;
  mipi_dsi_set_backlight(mfd, mfd->bl_level);
  mipi_lcd_on = 0;

  return 0;
}

static int mipi_pyramid_display_off(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;

  mfd = platform_get_drvdata(pdev);

  mipi_pyramid_send_cmd(mipi_power_off_cmd, mipi_power_off_cmd_size, true);

  return 0;
}

static unsigned char pyd_shp_shrink_pwm(int br)
{
	unsigned char shrink_br = BRI_SETTING_MAX;

	if (br <= 0) {
		shrink_br = 0;
	} else if (br > 0 && br <= BRI_SETTING_MIN) {
		shrink_br = SHARP_PWM_MIN;
	} else if (br > BRI_SETTING_MIN && br <= BRI_SETTING_DEF) {
		shrink_br = (SHARP_PWM_MIN + (br - BRI_SETTING_MIN) *
				(SHARP_PWM_DEFAULT - SHARP_PWM_MIN) /
				(BRI_SETTING_DEF - BRI_SETTING_MIN));
	} else if (br > BRI_SETTING_DEF && br <= BRI_SETTING_MAX) {
		shrink_br = (SHARP_PWM_DEFAULT + (br - BRI_SETTING_DEF) *
				(SHARP_PWM_MAX - SHARP_PWM_DEFAULT) /
				(BRI_SETTING_MAX - BRI_SETTING_DEF));
	} else if (br > BRI_SETTING_MAX)
		shrink_br = SHARP_PWM_MAX;
#if DEBUG
	PR_DISP_INFO("SHP: brightness orig=%d, transformed=%d\n", br, shrink_br);
#endif
	return shrink_br;
}

static unsigned char pyd_auo_shrink_pwm(int br)
{
	unsigned char shrink_br = 0;

	if (br <= 0) {
		shrink_br = 0;
	} else if (br > 0 && br <= BRI_SETTING_MIN) {
		shrink_br = AUO_PWM_MIN;
	} else if (br > BRI_SETTING_MIN && br <= BRI_SETTING_DEF) {
		shrink_br = (AUO_PWM_MIN + (br - BRI_SETTING_MIN) *
				(AUO_PWM_DEFAULT - AUO_PWM_MIN) /
				(BRI_SETTING_DEF - BRI_SETTING_MIN));
	} else if (br > BRI_SETTING_DEF && br <= BRI_SETTING_MAX) {
		shrink_br = (AUO_PWM_DEFAULT + (br - BRI_SETTING_DEF) *
				(AUO_PWM_MAX - AUO_PWM_DEFAULT) /
				(BRI_SETTING_MAX - BRI_SETTING_DEF));
	} else if (br > BRI_SETTING_MAX)
		shrink_br = AUO_PWM_MAX;
#if DEBUG
	PR_DISP_INFO("AUO: brightness orig=%d, transformed=%d\n", br, shrink_br);
#endif
	return shrink_br;
}

inline void mipi_dsi_set_backlight(struct msm_fb_data_type *mfd, int level)
{
	struct mipi_panel_info *mipi;

        mfd->bl_level = level;
        printk(KERN_ERR "%s: %d\n", __func__, level);
	mipi  = &mfd->panel_info.mipi;

	if (panel_type == PANEL_ID_PYD_SHARP)
          led_pwm1[1] = pyd_shp_shrink_pwm(mfd->bl_level);
	else
          led_pwm1[1] = pyd_auo_shrink_pwm(mfd->bl_level);
	if (mfd->bl_level == 0 ||
			board_mfg_mode() == 4/* ||
			(board_mfg_mode() == 5 && !(htc_battery_get_zcharge_mode()%2))*/) {
		led_pwm1[1] = 0;
	}
        mipi_pyramid_send_cmd(novatek_cmd_backlight_cmds, ARRAY_SIZE(novatek_cmd_backlight_cmds), true);

	bl_level_prevset = mfd->bl_level;
}

static void mipi_pyramid_set_backlight(struct msm_fb_data_type *mfd)
{
  
        mipi_dsi_set_backlight(mfd, mfd->bl_level);
}

static int mipi_pyramid_lcd_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
		mipi_pyramid_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_pyramid_lcd_probe,
	.driver = {
		.name   = "mipi_pyramid",
	},
};

static struct msm_fb_panel_data pyramid_panel_data = {
	.on		= mipi_pyramid_lcd_on,
        .off		= mipi_pyramid_lcd_off,
        .set_backlight  = mipi_pyramid_set_backlight,
	.late_init = mipi_pyramid_display_on,
	.early_off = mipi_pyramid_display_off,
};

static int ch_used[3];

int mipi_pyramid_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_pyramid_lcd_init();
	if (ret) {
		pr_err("mipi_pyramid_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_pyramid", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	pyramid_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &pyramid_panel_data,
		sizeof(pyramid_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

        printk(KERN_ERR "%s --\n", __func__);
	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int mipi_pyramid_lcd_init(void)
{
        mipi_pyramid_panel_type_detect();

	return platform_driver_register(&this_driver);
}

