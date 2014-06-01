/* Copyright (c) ?
 * Copyright (c) 2013 Sebastian Sobczyk <sebastiansobczyk@wp.pl>
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

#include "msm_sensor.h"

#ifdef CONFIG_RAWCHIP
#include "rawchip/rawchip.h"
#endif

#define SENSOR_NAME "s5k3h1gx"
#define PLATFORM_DRIVER_NAME "msm_camera_s5k3h1gx"

#define S5K3H1GX_REG_READ_MODE 0x0101
#define S5K3H1GX_READ_NORMAL_MODE 0x0000	
#define S5K3H1GX_READ_MIRROR 0x0001			
#define S5K3H1GX_READ_FLIP 0x0002			
#define S5K3H1GX_READ_MIRROR_FLIP 0x0003	

DEFINE_MUTEX(s5k3h1gx_mut);
DEFINE_MUTEX(s5k3h1gx_sensor_init_mut); 
static struct msm_sensor_ctrl_t s5k3h1gx_s_ctrl;

static struct msm_camera_i2c_reg_conf s5k3h1gx_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_groupon_settings[] = {
	{0x104, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_groupoff_settings[] = {
	{0x104, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_mipi_settings[] = {
	{0x3065, 0x35},
	{0x310E, 0x00},
	{0x3098, 0xAB},
	{0x30C7, 0x0A},
	{0x309A, 0x01},
	{0x310D, 0xC6},
	{0x30C3, 0x40},
	{0x30BB, 0x02},
	{0x30BC, 0x38},
	{0x30BD, 0x40},
	{0x3110, 0x70},
	{0x3111, 0x80},
	{0x3112, 0x7B},
	{0x3113, 0xC0},
	{0x30C7, 0x1A},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_pll_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_prev_settings[] = {
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x6C},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xB0},
	{0x31A1, 0x56},
	{0x0200, 0x02},
	{0x0201, 0x50},
	{0x0202, 0x04},
	{0x0203, 0xDB},
	{0x0204, 0x00},
	{0x0205, 0x20},
	{0x0342, 0x0D},
	{0x0343, 0x8E},
	{0x0340, 0x04},
	{0x0341, 0xE0},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCD},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x0381, 0x01},
	{0x0383, 0x03},
	{0x0385, 0x01},
	{0x0387, 0x03},
	{0x0401, 0x00},
	{0x0405, 0x10},
	{0x0700, 0x05},
	{0x0701, 0x30},
	{0x034C, 0x06},
	{0x034D, 0x68},
	{0x034E, 0x04},
	{0x034F, 0xD0},
	{0x300E, 0xED},
	{0x301D, 0x80},
	{0x301A, 0x77},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_video_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
	{0x0344, 0x00}, 
	{0x0345, 0x62},
	{0x0346, 0x01}, 
	{0x0347, 0x6C},
	{0x0348, 0x0C}, 
	{0x0349, 0x6D},
	{0x034A, 0x08}, 
	{0x034B, 0x33},
	{0x0381, 0x01}, 
	{0x0383, 0x01}, 
	{0x0385, 0x01}, 
	{0x0387, 0x01}, 
	{0x0105, 0x01}, 
	{0x0401, 0x00}, 
	{0x0405, 0x10},
	{0x0700, 0x05}, 
	{0x0701, 0x30},
	{0x034C, 0x0C}, 
	{0x034D, 0x0C},
	{0x034E, 0x06}, 
	{0x034F, 0xC8},
	{0x0200, 0x02}, 
	{0x0201, 0x50},
	{0x0202, 0x04}, 
	{0x0203, 0xDB},
	{0x0204, 0x00}, 
	{0x0205, 0x20},
	{0x0342, 0x0D}, 
	{0x0343, 0x8E},
	{0x0340, 0x06}, 
	{0x0341, 0xD8},
	{0x300E, 0x29}, 
	{0x31A3, 0x00}, 
	{0x301A, 0x77}, 
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_fast_video_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0xD4},
	{0x0348, 0x0C},
	{0x0349, 0xCD},
	{0x034A, 0x08},
	{0x034B, 0xCB},
	{0x0381, 0x01},
	{0x0383, 0x03},
	{0x0385, 0x01},
	{0x0387, 0x07},
	{0x0401, 0x00},
	{0x0405, 0x10},
	{0x0700, 0x05},
	{0x0701, 0x30},
	{0x034C, 0x06},
	{0x034D, 0x68},
	{0x034E, 0x01},
	{0x034F, 0xFE},
	{0x0200, 0x02},
	{0x0201, 0x50},
	{0x0202, 0x01},
	{0x0203, 0x39},
	{0x0204, 0x00},
	{0x0205, 0x20},
	{0x0342, 0x0D},
	{0x0343, 0x8E},
	{0x0340, 0x02},
	{0x0341, 0x0E},
	{0x300E, 0x2D},
	{0x31A3, 0x40},
	{0x301A, 0xA7},
	{0x3053, 0xCB}, 
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_snap_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
	{0x0200, 0x02},
	{0x0201, 0x50},
	{0x0202, 0x04},
	{0x0203, 0xE7},
	{0x0204, 0x00},
	{0x0205, 0x20},
	{0x0342, 0x0D},
	{0x0343, 0x8E},
	{0x0340, 0x09},
	{0x0341, 0xC0},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCF},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0105, 0x01}, 
	{0x0401, 0x00},
	{0x0405, 0x10},
	{0x0700, 0x05},
	{0x0701, 0x30},
	{0x034C, 0x0C},
	{0x034D, 0xD0},
	{0x034E, 0x09},
	{0x034F, 0xA0},
	{0x300E, 0x29}, 
	{0x31A3, 0x00}, 
	{0x301A, 0x77}, 
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_4_3_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
	{0x0200, 0x02},
	{0x0201, 0x50},
	{0x0202, 0x04},
	{0x0203, 0xE7},
	{0x0204, 0x00},
	{0x0205, 0x20},
	{0x0342, 0x0D},
	{0x0343, 0x8E},
	{0x0340, 0x09},
	{0x0341, 0xC0},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCF},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0105, 0x01}, 
	{0x0401, 0x00},
	{0x0405, 0x10},
	{0x0700, 0x05},
	{0x0701, 0x30},
	{0x034C, 0x0C},
	{0x034D, 0xD0},
	{0x034E, 0x09},
	{0x034F, 0xA0},
	{0x300E, 0x29}, 
	{0x31A3, 0x00}, 
	{0x301A, 0x77}, 
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_snap_wide_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x01},
	{0x0347, 0x30},
	{0x0348, 0x0C},
	{0x0349, 0xCF},
	{0x034A, 0x08},
	{0x034B, 0x6F},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0105, 0x01}, 
	{0x0401, 0x00},
	{0x0405, 0x10},
	{0x0700, 0x05},
	{0x0701, 0x30},
	{0x034C, 0x0C},
	{0x034D, 0xD0},
	{0x034E, 0x07},
	{0x034F, 0x40},
	{0x0200, 0x02},
	{0x0201, 0x50},
	{0x0202, 0x04},
	{0x0203, 0xDB},
	{0x0204, 0x00},
	{0x0205, 0x20},
	{0x0342, 0x0D},
	{0x0343, 0x8E},
	{0x0340, 0x09},
	{0x0341, 0x60},
	{0x300E, 0x29},
	{0x31A3, 0x00},
	{0x301A, 0xA7},
	{0x3053, 0xCB},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_night_settings[] = {
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x98},
	{0x0303, 0x01},
	{0x0301, 0x05},
	{0x030B, 0x01},
	{0x0309, 0x05},
	{0x30CC, 0xE0},
	{0x31A1, 0x5A},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCD},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x0381, 0x01},
	{0x0383, 0x03},
	{0x0385, 0x01},
	{0x0387, 0x03},
	{0x0401, 0x00},
	{0x0405, 0x10},
	{0x0700, 0x05},
	{0x0701, 0x30},
	{0x034C, 0x06},
	{0x034D, 0x68},
	{0x034E, 0x04},
	{0x034F, 0xD0},
	{0x0200, 0x02},
	{0x0201, 0x50},
	{0x0202, 0x04},
	{0x0203, 0xDB},
	{0x0204, 0x00},
	{0x0205, 0x20},
	{0x0342, 0x0D},
	{0x0343, 0x8E},
	{0x0340, 0x04},
	{0x0341, 0xE0},
	{0x0340, 0x04},
	{0x0341, 0xE0},
	{0x300E, 0x2D},
	{0x31A3, 0x40},
	{0x301A, 0xA7},
	{0x3053, 0xCF},
};

static struct msm_camera_i2c_reg_conf s5k3h1gx_recommend_settings[] = {
	{0x3000, 0x08},
	{0x3001, 0x05},
	{0x3002, 0x0D},
	{0x3003, 0x21},
	{0x3004, 0x62},
	{0x3005, 0x0B},
	{0x3006, 0x6D},
	{0x3007, 0x02},
	{0x3008, 0x62},
	{0x3009, 0x62},
	{0x300A, 0x41},
	{0x300B, 0x10},
	{0x300C, 0x21},
	{0x300D, 0x04},
	{0x307E, 0x03},
	{0x307F, 0xA5},
	{0x3080, 0x04},
	{0x3081, 0x29},
	{0x3082, 0x03},
	{0x3083, 0x21},
	{0x3011, 0x5F},
	{0x3156, 0xE2},
	{0x3027, 0x0E},
	{0x300f, 0x02},
	{0x3010, 0x10},
	{0x3017, 0x74},
	{0x3018, 0x00},
	{0x3020, 0x02},
	{0x3021, 0x24},
	{0x3023, 0x80},
	{0x3024, 0x04},
	{0x3025, 0x08},
	{0x301C, 0xD4},
	{0x315D, 0x00},
	{0x300E, 0x29},
	{0x31A3, 0x00},
	{0x301A, 0xA7},
	{0x3053, 0xCF},
	{0x3054, 0x00},
	{0x3055, 0x35},
	{0x3062, 0x04},
	{0x3063, 0x38},
	{0x31A4, 0x04},
	{0x3016, 0x54},
	{0x3157, 0x02},
	{0x3158, 0x00},
	{0x315B, 0x02},
	{0x315C, 0x00},
	{0x301B, 0x05},
	{0x3028, 0x41},
	{0x302A, 0x00},
	{0x3060, 0x01},
	{0x302D, 0x19},
	{0x302B, 0x04},
	{0x3072, 0x13},
	{0x3073, 0x21},
	{0x3074, 0x82},
	{0x3075, 0x20},
	{0x3076, 0xA2},
	{0x3077, 0x02},
	{0x3078, 0x91},
	{0x3079, 0x91},
	{0x307A, 0x61},
	{0x307B, 0x28},
	{0x307C, 0x31},
};

static struct v4l2_subdev_info s5k3h1gx_subdev_info[] = {
	{
	.code  = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt   = 1,
	.order = 0,
	},
};

static struct msm_camera_i2c_conf_array s5k3h1gx_init_conf[] = {
	{&s5k3h1gx_mipi_settings[0],
	ARRAY_SIZE(s5k3h1gx_mipi_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_recommend_settings[0],
	ARRAY_SIZE(s5k3h1gx_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_pll_settings[0],
	ARRAY_SIZE(s5k3h1gx_pll_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_conf_array s5k3h1gx_confs[] = {
	{&s5k3h1gx_snap_settings[0],
	ARRAY_SIZE(s5k3h1gx_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_prev_settings[0],
	ARRAY_SIZE(s5k3h1gx_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_video_settings[0],
	ARRAY_SIZE(s5k3h1gx_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_fast_video_settings[0],
	ARRAY_SIZE(s5k3h1gx_fast_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_snap_wide_settings[0],
	ARRAY_SIZE(s5k3h1gx_snap_wide_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_4_3_settings[0],
	ARRAY_SIZE(s5k3h1gx_4_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k3h1gx_night_settings[0],
	ARRAY_SIZE(s5k3h1gx_night_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t s5k3h1gx_dimensions[] = {
	{
		.x_output = 0xCD0,
		.y_output = 0x9A0,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x9C0,
		.vt_pixel_clk = 182400000,
		.op_pixel_clk = 182400000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0,
		.x_addr_end = 0xCCF,
		.y_addr_end = 0x99F,
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
	},
	{
		.x_output = 0x668,
		.y_output = 0x4D0,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x4E0,
		.vt_pixel_clk = 129600000,
		.op_pixel_clk = 129600000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0,
		.x_addr_end = 0xCCD,
		.y_addr_end = 0x99F,
		.x_even_inc = 1,
		.x_odd_inc = 3,
		.y_even_inc = 1,
		.y_odd_inc = 3,
	},
	{
		.x_output = 0xC0C,
		.y_output = 0x6C8,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x6D8,
		.vt_pixel_clk = 182400000,
		.op_pixel_clk = 182400000,
		.binning_factor = 1,
		.x_addr_start = 0x062,
		.y_addr_start = 0x16C,
		.x_addr_end = 0xC6D,
		.y_addr_end = 0x833,
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
	},
	{
		.x_output = 0x668,
		.y_output = 0x1FE,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x20E,
		.vt_pixel_clk = 182400000,
		.op_pixel_clk = 182400000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0x0D4,
		.x_addr_end = 0xCCD,
		.y_addr_end = 0x8CB,
		.x_even_inc = 1,
		.x_odd_inc = 3,
		.y_even_inc = 1,
		.y_odd_inc = 7,
	},
	{
		.x_output = 0xCD0,
		.y_output = 0x740,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x960,  
		.vt_pixel_clk = 182400000,
		.op_pixel_clk = 182400000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0x0130,
		.x_addr_end = 0xCCF,
		.y_addr_end = 0x86F,
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
	},
	{
		.x_output = 0xCD0,
		.y_output = 0x9A0,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x9C0,
		.vt_pixel_clk = 182400000,
		.op_pixel_clk = 182400000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0,
		.x_addr_end = 0xCCF,
		.y_addr_end = 0x99F,
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
	},
	{
		.x_output = 0x668,
		.y_output = 0x4D0,
		.line_length_pclk = 0xD8E,
		.frame_length_lines = 0x4E0,
		.vt_pixel_clk = 182400000,
		.op_pixel_clk = 182400000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0,
		.x_addr_end = 0xCCD,
		.y_addr_end = 0x99F,
		.x_even_inc = 1,
		.x_odd_inc = 3,
		.y_even_inc = 1,
		.y_odd_inc = 3,
	},
};

static struct msm_camera_csi_params s5k3h1gx_csi_params = {
	.data_format = CSI_RAW10,
	.lane_cnt    = 2,
	.lane_assign = 0xe4,
	.dpcm_scheme = 0,
	.settle_cnt  = 0x2a,
};

static struct msm_camera_csi_params *s5k3h1gx_csi_params_array[] = {
	&s5k3h1gx_csi_params,
	&s5k3h1gx_csi_params,
	&s5k3h1gx_csi_params,
	&s5k3h1gx_csi_params,
	&s5k3h1gx_csi_params,
	&s5k3h1gx_csi_params,
};

static struct msm_sensor_output_reg_addr_t s5k3h1gx_reg_addr = {
	.x_output = 0x34C,
	.y_output = 0x34E,
	.line_length_pclk = 0x342,
	.frame_length_lines = 0x340,
};

static struct msm_sensor_id_info_t s5k3h1gx_id_info = {
	.sensor_id_reg_addr = 0x0,
	.sensor_id = 0x3810,
};

static struct msm_sensor_exp_gain_info_t s5k3h1gx_exp_gain_info = {
	.coarse_int_time_addr = 0x202,
	.global_gain_addr = 0x204,
	.vert_offset = 16,
	.min_vert = 4,  
	.sensor_max_linecount = 65519,  
};

static int s5k3h1gx_sensor_open_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	uint16_t value = 0;

	if (data->sensor_platform_info)
		s5k3h1gx_s_ctrl.mirror_flip = data->sensor_platform_info->mirror_flip;

	if (s5k3h1gx_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR_FLIP)
		value = S5K3H1GX_READ_MIRROR_FLIP;
	else if (s5k3h1gx_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR)
		value = S5K3H1GX_READ_MIRROR;
	else if (s5k3h1gx_s_ctrl.mirror_flip == CAMERA_SENSOR_FLIP)
		value = S5K3H1GX_READ_FLIP;
	else
		value = S5K3H1GX_READ_NORMAL_MODE;
	msm_camera_i2c_write(s5k3h1gx_s_ctrl.sensor_i2c_client,
		S5K3H1GX_REG_READ_MODE, value, MSM_CAMERA_I2C_BYTE_DATA);

	return rc;
}

static const char *s5k3h1gxVendor = "samsung";
static const char *s5k3h1gxNAME = "s5k3h1gx";
static const char *s5k3h1gxSize = "8M";

static ssize_t sensor_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	sprintf(buf, "%s %s %s\n", s5k3h1gxVendor, s5k3h1gxNAME, s5k3h1gxSize);
	ret = strlen(buf) + 1;

	return ret;
}

static ssize_t lens_info_show(struct device *dev,
  struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	int lens_info = 6;

	sprintf(buf, "%d\n", lens_info);
	ret = strlen(buf) + 1;

	return ret;
}

static DEVICE_ATTR(sensor, 0444, sensor_vendor_show, NULL);
static DEVICE_ATTR(lensinfo, 0444, lens_info_show, NULL);

static struct kobject *android_s5k3h1gx;

static int s5k3h1gx_sysfs_init(void)
{
	int ret ;
	pr_info("s5k3h1gx:kobject creat and add\n");
	android_s5k3h1gx = kobject_create_and_add("android_camera", NULL);
	if (android_s5k3h1gx == NULL) {
		pr_info("s5k3h1gx_sysfs_init: subsystem_register " \
		"failed\n");
		ret = -ENOMEM;
		return ret ;
	}
	pr_info("s5k3h1gx:sysfs_create_file\n");
	ret = sysfs_create_file(android_s5k3h1gx, &dev_attr_sensor.attr);
	if (ret) {
		pr_info("s5k3h1gx_sysfs_init: sysfs_create_file " \
		"failed\n");
		kobject_del(android_s5k3h1gx);
	}
	pr_info("s5k3h1gx:sysfs_create_file lensinfo\n");
	ret = sysfs_create_file(android_s5k3h1gx, &dev_attr_lensinfo.attr);
	if (ret) {
		pr_info("s5k3h1gx_sysfs_init: dev_attr_lensinfo failed\n");
		kobject_del(android_s5k3h1gx);
	}

	return 0 ;
}

static struct msm_camera_i2c_client s5k3h1gx_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

int32_t s5k3h1gx_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc;
	struct msm_camera_sensor_info *sdata = NULL;

	pr_info("%s\n", __func__);
	if (s_ctrl && s_ctrl->sensordata)
		sdata = s_ctrl->sensordata;
	else {
		pr_err("%s: s_ctrl sensordata NULL\n", __func__);
		return (-1);
	}

	if (sdata->camera_power_on == NULL) {
		pr_err("sensor platform_data didnt register\n");
		return -EIO;
	}

	if (!sdata->use_rawchip) {
		rc = msm_camio_clk_enable(CAMIO_CAM_MCLK_CLK);
		if (rc < 0) {
			pr_err("%s: msm_camio_sensor_clk_on failed:%d\n",
			 __func__, rc);
			goto enable_mclk_failed;
		}
	}

	rc = sdata->camera_power_on();
	if (rc < 0) {
		pr_err("%s failed to enable power\n", __func__);
		goto enable_power_on_failed;
	}

	rc = msm_sensor_set_power_up(s_ctrl);
	if (rc < 0) {
		pr_err("%s msm_sensor_power_up failed\n", __func__);
		goto enable_sensor_power_up_failed;
	}

	s5k3h1gx_sensor_open_init(sdata);
	pr_info("%s end\n", __func__);

	return rc;

enable_sensor_power_up_failed:
	if (sdata->camera_power_off == NULL)
		pr_err("sensor platform_data didnt register\n");
	else
		sdata->camera_power_off();
enable_power_on_failed:
	msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);
enable_mclk_failed:
	return rc;
}

int32_t s5k3h1gx_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc;
	struct msm_camera_sensor_info *sdata = NULL;
	pr_info("%s\n", __func__);

	if (s_ctrl && s_ctrl->sensordata)
		sdata = s_ctrl->sensordata;
	else {
		pr_err("%s: s_ctrl sensordata NULL\n", __func__);
		return (-1);
	}

	if (sdata->camera_power_off == NULL) {
		pr_err("sensor platform_data didnt register\n");
		return -EIO;
	}

	rc = sdata->camera_power_off();
	if (rc < 0)
		pr_err("%s failed to disable power\n", __func__);

	rc = msm_sensor_set_power_down(s_ctrl);
	if (rc < 0)
		pr_err("%s msm_sensor_power_down failed\n", __func__);

	if (!sdata->use_rawchip) {
		msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);
		if (rc < 0)
			pr_err("%s: msm_camio_sensor_clk_off failed:%d\n",
				 __func__, rc);
	}

	return rc;
}

int32_t s5k3h1gx_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int	rc = 0;
	pr_info("%s\n", __func__);
	rc = msm_sensor_i2c_probe(client, id);
	if(rc >= 0)
		s5k3h1gx_sysfs_init();
	pr_info("%s: rc(%d)\n", __func__, rc);
	return rc;
}

static const struct i2c_device_id s5k3h1gx_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&s5k3h1gx_s_ctrl},
	{ }
};

static struct i2c_driver s5k3h1gx_i2c_driver = {
	.id_table = s5k3h1gx_i2c_id,
	.probe  = s5k3h1gx_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static int __init msm_sensor_init_module(void)
{
	pr_info("%s\n", __func__);
	return i2c_add_driver(&s5k3h1gx_i2c_driver);
}

static struct v4l2_subdev_core_ops s5k3h1gx_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k3h1gx_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops s5k3h1gx_subdev_ops = {
	.core = &s5k3h1gx_subdev_core_ops,
	.video  = &s5k3h1gx_subdev_video_ops,
};

static int s5k3h1gx_read_fuseid(struct sensor_cfg_data *cdata,
	struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t  rc;
	unsigned short i, R1, R2, R3;
	unsigned short  OTP[10] = {0};

	struct msm_camera_i2c_client *s5k3h1gx_msm_camera_i2c_client = s_ctrl->sensor_i2c_client;

	pr_info("%s: sensor OTP information:\n", __func__);

	rc = msm_camera_i2c_write_b(s5k3h1gx_msm_camera_i2c_client, 0x3124, 0x10);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3124 fail\n", __func__);

	
	rc = msm_camera_i2c_write_b(s5k3h1gx_msm_camera_i2c_client, 0x3127, 0xF1);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3127 (Start) fail\n", __func__);

	mdelay(4);

	for (i = 0; i < 10; i++) {
		rc = msm_camera_i2c_write_b(s5k3h1gx_msm_camera_i2c_client, 0x312B, i);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x312B fail\n", __func__);

		rc = msm_camera_i2c_read_b(s5k3h1gx_msm_camera_i2c_client, 0x312C, &R1);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x310C fail\n", __func__);

		rc = msm_camera_i2c_read_b(s5k3h1gx_msm_camera_i2c_client, 0x312D, &R2);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x312D fail\n", __func__);

		rc = msm_camera_i2c_read_b(s5k3h1gx_msm_camera_i2c_client, 0x312E, &R3);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x312E fail\n", __func__);

		if ((R3&0x0F) != 0)
			OTP[i] = (short)(R3&0x0F);
		else if ((R3>>4) != 0)
			OTP[i] = (short)(R3>>4);
		else if ((R2&0x0F) != 0)
			OTP[i] = (short)(R2&0x0F);
		else if ((R2>>4) != 0)
			OTP[i] = (short)(R2>>4);
		else if ((R1&0x0F) != 0)
			OTP[i] = (short)(R1&0x0F);
	}

	pr_info("%s: VenderID=%x,LensID=%x,SensorID=%x%x\n", __func__,
		OTP[0], OTP[1], OTP[2], OTP[3]);
	pr_info("%s: ModuleFuseID= %x%x%x%x%x%x\n", __func__,
		OTP[4], OTP[5], OTP[6], OTP[7], OTP[8], OTP[9]);

    cdata->cfg.fuse.fuse_id_word1 = 0;
    cdata->cfg.fuse.fuse_id_word2 = 0;
	cdata->cfg.fuse.fuse_id_word3 = (OTP[0]);
	cdata->cfg.fuse.fuse_id_word4 =
		(OTP[4]<<20) |
		(OTP[5]<<16) |
		(OTP[6]<<12) |
		(OTP[7]<<8) |
		(OTP[8]<<4) |
		(OTP[9]);

	pr_info("s5k3h1gx: fuse->fuse_id_word1:%d\n",
		cdata->cfg.fuse.fuse_id_word1);
	pr_info("s5k3h1gx: fuse->fuse_id_word2:%d\n",
		cdata->cfg.fuse.fuse_id_word2);
	pr_info("s5k3h1gx: fuse->fuse_id_word3:0x%08x\n",
		cdata->cfg.fuse.fuse_id_word3);
	pr_info("s5k3h1gx: fuse->fuse_id_word4:0x%08x\n",
		cdata->cfg.fuse.fuse_id_word4);
	return 0;
}

static struct msm_sensor_fn_t s5k3h1gx_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain_ex = msm_sensor_write_exp_gain2_ex,
	.sensor_write_snapshot_exp_gain_ex = msm_sensor_write_exp_gain2_ex,
	.sensor_setting = msm_sensor_setting1,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = s5k3h1gx_power_up,
	.sensor_power_down = s5k3h1gx_power_down,
	.sensor_i2c_read_fuseid = s5k3h1gx_read_fuseid,
};

static struct msm_sensor_reg_t s5k3h1gx_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = s5k3h1gx_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(s5k3h1gx_start_settings),
	.stop_stream_conf = s5k3h1gx_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(s5k3h1gx_stop_settings),
	.group_hold_on_conf = s5k3h1gx_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(s5k3h1gx_groupon_settings),
	.group_hold_off_conf = s5k3h1gx_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(s5k3h1gx_groupoff_settings),
	.init_settings = &s5k3h1gx_init_conf[0],
	.init_size = ARRAY_SIZE(s5k3h1gx_init_conf),
	.mode_settings = &s5k3h1gx_confs[0],
	.output_settings = &s5k3h1gx_dimensions[0],
	.num_conf = ARRAY_SIZE(s5k3h1gx_confs),
};

static struct msm_sensor_ctrl_t s5k3h1gx_s_ctrl = {
	.msm_sensor_reg = &s5k3h1gx_regs,
	.sensor_i2c_client = &s5k3h1gx_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &s5k3h1gx_reg_addr,
	.sensor_id_info = &s5k3h1gx_id_info,
	.sensor_exp_gain_info = &s5k3h1gx_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csic_params = &s5k3h1gx_csi_params_array[0],
	.msm_sensor_mutex = &s5k3h1gx_mut,
	.sensor_i2c_driver = &s5k3h1gx_i2c_driver,
	.sensor_v4l2_subdev_info = s5k3h1gx_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k3h1gx_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k3h1gx_subdev_ops,
	.func_tbl = &s5k3h1gx_func_tbl,
	.sensor_first_mutex = &s5k3h1gx_sensor_init_mut, 
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Samsung 8 MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
