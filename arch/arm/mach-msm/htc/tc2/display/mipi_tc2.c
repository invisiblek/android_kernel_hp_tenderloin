/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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

#include <linux/leds.h>
#include <mach/panel_id.h>
#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_tc2.h"

static struct mipi_dsi_panel_platform_data *mipi_tc2_pdata;

static int mipi_tc2_lcd_init(void);

static int wled_trigger_initialized;
static atomic_t lcd_power_state;

static char sony_orise_001[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 : address shift*/
static char sony_orise_002[] = {
        0xFF, 0x96, 0x01, 0x01}; /* DTYPE_DCS_LWRITE : 0x9600:0x96, 0x9601:0x01, 0x9602:0x01*/
static char sony_orise_003[] ={0x00, 0x80}; /* DTYPE_DCS_WRITE1 : address shift*/
static char sony_orise_004[] = {
        0xFF, 0x96, 0x01};
static char sony_inv_01[] = {0x00, 0xB3};
static char sony_inv_02[] = {0xC0, 0x50};
static char sony_timing1_01[] = {0x00, 0x80};
static char sony_timing1_02[] = {0xF3, 0x04};
static char sony_timing2_01[] = {0x00, 0xC0};
static char sony_timing2_02[] = {0xC2, 0xB0};
static char sony_pwrctl2_01[] = {0x00, 0xA0};
static char sony_pwrctl2_02[] = {
	0xC5, 0x04, 0x3A, 0x56,
	0x44, 0x44, 0x44, 0x44};
static char sony_pwrctl3_01[] = {0x00, 0xB0};
static char sony_pwrctl3_02[] = {
	0xC5, 0x04, 0x3A, 0x56,
	0x44, 0x44, 0x44, 0x44};

static char sony_gamma28_00[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 :address shift*/
static char sony_gamma28_01[] = {
	0xe1, 0x07, 0x10, 0x16,
	0x0F, 0x08, 0x0F, 0x0D,
	0x0C, 0x02, 0x06, 0x0F,
	0x0B, 0x11, 0x0D, 0x07,
	0x00
}; /* DTYPE_DCS_LWRITE :0xE100:0x11, 0xE101:0x19, 0xE102: 0x1e, ..., 0xff are padding for 4 bytes*/

static char sony_gamma28_02[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 :address shift*/
static char sony_gamma28_03[] = {
	0xe2, 0x07, 0x10, 0x16,
	0x0F, 0x08, 0x0F, 0x0D,
	0x0C, 0x02, 0x06, 0x0F,
	0x0B, 0x11, 0x0D, 0x07,
	0x00
}; /* DTYPE_DCS_LWRITE :0xE200:0x11, 0xE201:0x19, 0xE202: 0x1e, ..., 0xff are padding for 4 bytes*/

static char sony_gamma28_04[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 :address shift*/
static unsigned char sony_gamma28_05[] = {
	0xe3, 0x19, 0x1D, 0x20,
	0x0C, 0x04, 0x0B, 0x0B,
	0x0A, 0x03, 0x07, 0x12,
	0x0B, 0x11, 0x0D, 0x07,
	0x00
}; /* DTYPE_DCS_LWRITE :0xE200:0x11, 0xE201:0x19, 0xE202: 0x1e, ..., 0xff are padding for 4 bytes*/

static char sony_gamma28_06[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 :address shift*/
static char sony_gamma28_07[] = {
	0xe4, 0x19, 0x1D, 0x20,
	0x0C, 0x04, 0x0B, 0x0B,
	0x0A, 0x03, 0x07, 0x12,
	0x0B, 0x11, 0x0D, 0x07,
	0x00
}; /* DTYPE_DCS_LWRITE :0xE200:0x11, 0xE201:0x19, 0xE202: 0x1e, ..., 0xff are padding for 4 bytes*/

static char sony_gamma28_08[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 :address shift*/
static char sony_gamma28_09[] = {
	0xe5, 0x07, 0x0F, 0x15,
	0x0D, 0x06, 0x0E, 0x0D,
	0x0C, 0x02, 0x06, 0x0F,
	0x09, 0x0D, 0x0D, 0x06,
	0x00
}; /* DTYPE_DCS_LWRITE :0xE200:0x11, 0xE201:0x19, 0xE202: 0x1e, ..., 0xff are padding for 4 bytes*/

static char sony_gamma28_10[] ={0x00, 0x00}; /* DTYPE_DCS_WRITE1 :address shift*/
static char sony_gamma28_11[] = {
	0xe6, 0x07, 0x0F, 0x15,
	0x0D, 0x06, 0x0E, 0x0D,
	0x0C, 0x02, 0x06, 0x0F,
	0x09, 0x0D, 0x0D, 0x06,
	0x00
}; /* DTYPE_DCS_LWRITE :0xE200:0x11, 0xE201:0x19, 0xE202: 0x1e, ..., 0xff are padding for 4 bytes*/

static char pwm_freq_sel_cmds1[] = {0x00, 0xB4}; /* address shift to pwm_freq_sel */
static char pwm_freq_sel_cmds2[] = {0xC6, 0x00}; /* CABC command with parameter 0 */

static char pwm_dbf_cmds1[] = {0x00, 0xB1}; /* address shift to PWM DBF */
static char pwm_dbf_cmds2[] = {0xC6, 0x04}; /* CABC command-- DBF: [2:1], force duty: [0] */

static char sony_ce_table1[] = {
	0xD4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xfe, 0xfe, 0xfe, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40};

static char sony_ce_table2[] = {
	0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xfd, 0xfd, 0xfd, 0x4f, 0x4f, 0x4e,
	0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4d,
	0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4e, 0x4e,
	0x4e, 0x4f, 0x4f, 0x4f, 0x50, 0x50, 0x50, 0x51,
	0x51, 0x51, 0x52, 0x52, 0x52, 0x53, 0x53, 0x53,
	0x54, 0x54, 0x54, 0x54, 0x55, 0x55, 0x55, 0x56,
	0x56, 0x56, 0x56, 0x56, 0x56, 0x56, 0x56, 0x55,
	0x55, 0x55, 0x55, 0x54, 0x54, 0x54, 0x54, 0x54,
	0x53, 0x53, 0x54, 0x54, 0x54, 0x55, 0x55, 0x55,
	0x55, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x58,
	0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x59, 0x59,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x59,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58,
	0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58,
	0x58, 0x59, 0x59, 0x59, 0x59, 0x59, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b,
	0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b,
	0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b,
	0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b, 0x5b,
	0x5b, 0x5b, 0x5b, 0x5a, 0x59, 0x58, 0x58, 0x57,
	0x56, 0x55, 0x54, 0x54, 0x53, 0x52, 0x51, 0x50,
	0x50};

static char sony_ce_01[] = {0x00, 0x00};
static char sony_ce_02[] = {0xD6, 0x08};

static char dsi_tc2_pwm3[] = {0x55, 0x00};/* DTYPE_DCS_WRITE1 *///CABC off
static char enter_sleep[2] = {0x10, 0x00}; /* DTYPE_DCS_WRITE */
static char exit_sleep[2] = {0x11, 0x00}; /* DTYPE_DCS_WRITE */
static char display_off[2] = {0x28, 0x00}; /* DTYPE_DCS_WRITE */
static char display_on[2] = {0x29, 0x00}; /* DTYPE_DCS_WRITE */

static char led_pwm1[] = {0x51, 0x00}; /* DTYPE_DCS_WRITE1 */
static char dsi_tc2_pwm2[] = {0x53, 0x24};/* DTYPE_DCS_WRITE1 *///bkl on and no dim

static struct dsi_cmd_desc tc2_video_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc tc2_color_enhance[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_01), sony_ce_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_ce_table1), sony_ce_table1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_01), sony_ce_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_ce_table2), sony_ce_table2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_01), sony_ce_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_02), sony_ce_02},
};

static struct dsi_cmd_desc tc2_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_orise_001), sony_orise_001},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_orise_002), sony_orise_002},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_orise_003), sony_orise_003},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_orise_004), sony_orise_004},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_inv_01), sony_inv_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_inv_02), sony_inv_02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_timing1_01), sony_timing1_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_timing1_02), sony_timing1_02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_timing2_01), sony_timing2_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_timing2_02), sony_timing2_02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_pwrctl2_01), sony_pwrctl2_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_pwrctl2_02), sony_pwrctl2_02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_pwrctl3_01), sony_pwrctl3_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_pwrctl3_02), sony_pwrctl3_02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_orise_001), sony_orise_001},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_gamma28_00), sony_gamma28_00},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_gamma28_01), sony_gamma28_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_gamma28_02), sony_gamma28_02},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_gamma28_03), sony_gamma28_03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_gamma28_04), sony_gamma28_04},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_gamma28_05), sony_gamma28_05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_gamma28_06), sony_gamma28_06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_gamma28_07), sony_gamma28_07},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_gamma28_08), sony_gamma28_08},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_gamma28_09), sony_gamma28_09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_gamma28_10), sony_gamma28_10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_gamma28_11), sony_gamma28_11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_orise_001), sony_orise_001},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(pwm_freq_sel_cmds1), pwm_freq_sel_cmds1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(pwm_freq_sel_cmds2), pwm_freq_sel_cmds2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(pwm_dbf_cmds1), pwm_dbf_cmds1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(pwm_dbf_cmds2), pwm_dbf_cmds2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_01), sony_ce_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_ce_table1), sony_ce_table1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_01), sony_ce_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(sony_ce_table2), sony_ce_table2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_01), sony_ce_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(sony_ce_02), sony_ce_02},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(dsi_tc2_pwm2), dsi_tc2_pwm2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(dsi_tc2_pwm3), dsi_tc2_pwm3},
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
};

static struct dsi_cmd_desc tc2_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(enter_sleep), enter_sleep}
};
static struct dsi_cmd_desc tc2_display_on_cmds[] = {  //brandon, check the value
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(display_on), display_on},
};


static struct dsi_cmd_desc tc2_cmd_backlight_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(led_pwm1), led_pwm1},
};

static int tc2_send_display_cmds(struct dsi_cmd_desc *cmd, int cnt)
{
	int ret = 0;
	struct dcs_cmd_req cmdreq;

	cmdreq.cmds = cmd;
	cmdreq.cmds_cnt = cnt;
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	ret = mipi_dsi_cmdlist_put(&cmdreq);
	if (ret < 0)
		printk(KERN_ERR "[DISP] %s failed (%d)\n", __func__, ret);
	return ret;
}


void mipi_exit_ulps(void)
{
       uint32 status;

       status=MIPI_INP(MIPI_DSI_BASE + 0x00a8);
       status&=0x10000000; // in order to keep bit28
       status |= (BIT(8) | BIT(9) | BIT(10) | BIT(12));
       MIPI_OUTP(MIPI_DSI_BASE + 0x00a8, status);
       mb();
       msleep(5);
       status=MIPI_INP(MIPI_DSI_BASE + 0x00a4);
       if ((status&0x1700)!=0) 
         {
           status=MIPI_INP(MIPI_DSI_BASE + 0x00a8);
           status&=0x10000000; // in order to keep bit28
           MIPI_OUTP(MIPI_DSI_BASE + 0x00a8, status);
         } 
       else 
         {
           pr_debug("%s: cannot exit ulps\n", __func__);
         }
}

int mipi_lcd_on = 1;
bool first_init = true;

static int mipi_tc2_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct msm_panel_info *pinfo;

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

        if (mipi_lcd_on && first_init)
          {
            tc2_send_display_cmds(tc2_color_enhance,
			ARRAY_SIZE(tc2_color_enhance));
            
            mipi_exit_ulps();
            
            first_init = false;
            return 0;
          }
        else if (mipi_lcd_on)
          return 0;

	pinfo = &mfd->panel_info;
	mipi  = &mfd->panel_info.mipi;

        tc2_send_display_cmds(tc2_display_on_cmds, ARRAY_SIZE(tc2_display_on_cmds));
	if (mipi->mode == DSI_VIDEO_MODE)
          {
            tc2_send_display_cmds(tc2_video_on_cmds,
                                  ARRAY_SIZE(tc2_video_on_cmds));
          } 
        else 
          {
            tc2_send_display_cmds(tc2_cmd_on_cmds,
                                  ARRAY_SIZE(tc2_cmd_on_cmds));
          }
	atomic_set(&lcd_power_state, 1);
        mipi_lcd_on = 1;

	pr_debug("Init done\n");

	return 0;
}

static int mipi_tc2_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

        if (!mipi_lcd_on)
          return 0;

        if (panel_type != PANEL_ID_NONE)
          tc2_send_display_cmds(tc2_display_off_cmds,
                                ARRAY_SIZE(tc2_display_off_cmds));


	atomic_set(&lcd_power_state, 0);

        mipi_lcd_on = 0;

	return 0;
}

DEFINE_LED_TRIGGER(bkl_led_trigger);

static unsigned char tc2_shrink_pwm(int val)
{
	unsigned int pwm_min, pwm_default, pwm_max;
	unsigned char shrink_br = BRI_SETTING_MAX;

	pwm_min = 7;
	pwm_default = 86;
	pwm_max = 255;


	if (val <= 0) {
		shrink_br = 0;
	} else if (val > 0 && (val < BRI_SETTING_MIN)) {
			shrink_br = pwm_min;
	} else if ((val >= BRI_SETTING_MIN) && (val <= BRI_SETTING_DEF)) {
			shrink_br = (val - BRI_SETTING_MIN) * (pwm_default - pwm_min) /
		(BRI_SETTING_DEF - BRI_SETTING_MIN) + pwm_min;
	} else if (val > BRI_SETTING_DEF && val <= BRI_SETTING_MAX) {
			shrink_br = (val - BRI_SETTING_DEF) * (pwm_max - pwm_default) /
		(BRI_SETTING_MAX - BRI_SETTING_DEF) + pwm_default;
	} else if (val > BRI_SETTING_MAX)
			shrink_br = pwm_max;

	pr_info("brightness orig=%d, transformed=%d\n", val, shrink_br);

	return shrink_br;
}

static void mipi_tc2_set_backlight(struct msm_fb_data_type *mfd)
{
	struct mipi_panel_info *mipi;
///HTC:
        led_pwm1[1] = tc2_shrink_pwm(mfd->bl_level);
///:HTC

	if (mipi_tc2_pdata && (mipi_tc2_pdata->enable_wled_bl_ctrl)
	    && (wled_trigger_initialized)) {
		led_trigger_event(bkl_led_trigger, led_pwm1[1]);
		return;
	}
	mipi  = &mfd->panel_info.mipi;
	pr_debug("%s+:bl=%d \n", __func__, mfd->bl_level);


	tc2_send_display_cmds(tc2_cmd_backlight_cmds,
                              ARRAY_SIZE(tc2_cmd_backlight_cmds));

///HTC:
#ifdef CONFIG_BACKLIGHT_WLED_CABC
	/* For WLED CABC, To switch on/off WLED module */
	if (wled_trigger_initialized) {
		led_trigger_event(bkl_led_trigger, mfd->bl_level);
	}
#endif
///:HTC
	return;
}

static int __devinit mipi_tc2_lcd_probe(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct platform_device *current_pdev;
	static struct mipi_dsi_phy_ctrl *phy_settings;
	static char dlane_swap;

	if (pdev->id == 0) {
		mipi_tc2_pdata = pdev->dev.platform_data;

		if (mipi_tc2_pdata
			&& mipi_tc2_pdata->phy_ctrl_settings) {
			phy_settings = (mipi_tc2_pdata->phy_ctrl_settings);
		}

		if (mipi_tc2_pdata
			&& mipi_tc2_pdata->dlane_swap) {
			dlane_swap = (mipi_tc2_pdata->dlane_swap);
		}

		return 0;
	}

	current_pdev = msm_fb_add_device(pdev);

	if (current_pdev) {
		mfd = platform_get_drvdata(current_pdev);
		if (!mfd)
			return -ENODEV;
		if (mfd->key != MFD_KEY)
			return -EINVAL;

		mipi  = &mfd->panel_info.mipi;

		if (phy_settings != NULL)
			mipi->dsi_phy_db = phy_settings;

		if (dlane_swap)
			mipi->dlane_swap = dlane_swap;
	}
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_tc2_lcd_probe,
	.driver = {
		.name   = "mipi_tc2",
	},
};

static struct msm_fb_panel_data tc2_panel_data = {
	.on		= mipi_tc2_lcd_on,
	.off		= mipi_tc2_lcd_off,
	.set_backlight  = mipi_tc2_set_backlight,
};

static int ch_used[3];

int mipi_tc2_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_tc2_lcd_init();
	if (ret) {
		pr_err("mipi_novatek_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_tc2", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	tc2_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &tc2_panel_data,
		sizeof(tc2_panel_data));
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

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int mipi_tc2_lcd_init(void)
{
	if(panel_type == PANEL_ID_NONE)	{
		pr_info("%s panel ID = PANEL_ID_NONE\n", __func__);
		return 0;
	}

	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
	wled_trigger_initialized = 1;
	atomic_set(&lcd_power_state, 1);

	return platform_driver_register(&this_driver);
}

//module_init(mipi_tc2_lcd_init);
