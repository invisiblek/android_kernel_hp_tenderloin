
#include "msm_sensor.h"

#ifdef CONFIG_RAWCHIP
#include "rawchip/rawchip.h"
#endif

#define SENSOR_NAME "imx091"
#define PLATFORM_DRIVER_NAME "msm_camera_imx091"
#define imx091_obj imx091_##obj

#define IMX091_REG_READ_MODE 0x0101
#define IMX091_READ_NORMAL_MODE 0x0000	
#define IMX091_READ_MIRROR 0x0001			
#define IMX091_READ_FLIP 0x0002			
#define IMX091_READ_MIRROR_FLIP 0x0003

DEFINE_MUTEX(imx091_mut);
static struct msm_sensor_ctrl_t imx091_s_ctrl;

static struct msm_camera_i2c_reg_conf imx091_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf imx091_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf imx091_groupon_settings[] = {
	{0x104, 0x01},
};

static struct msm_camera_i2c_reg_conf imx091_groupoff_settings[] = {
	{0x104, 0x00},
};

static struct msm_camera_i2c_reg_conf imx091_mipi_settings[] = {
	{0x3087, 0x53},
	{0x309D, 0x94},
	{0x30A1, 0x08},
	{0x30C7, 0x00},
	{0x3115, 0x0E},
	{0x3118, 0x42},
	{0x311D, 0x34},
	{0x3121, 0x0D},
	{0x3212, 0xF2},
	{0x3213, 0x0F},
	{0x3215, 0x0F},
	{0x3217, 0x0B},
	{0x3219, 0x0B},
	{0x321B, 0x0D},
	{0x321D, 0x0D},
};

static struct msm_camera_i2c_reg_conf imx091_pll_settings[] = {
	{0x0305, 0x02},  
	{0x0307, 0x2F},  
	{0x303C, 0x4B},  
};


static struct msm_camera_i2c_reg_conf imx091_prev_settings[] = {
	{0x0305, 0x02},  
	{0x0307, 0x2F},  
	{0x303C, 0x4B},  

	{0x3087, 0x53},
	{0x309D, 0x94},
	{0x30A1, 0x08},
	{0x30C7, 0x00},
	{0x3115, 0x0E},
	{0x3118, 0x42},
	{0x311D, 0x34},
	{0x3121, 0x0D},
	{0x3212, 0xF2},
	{0x3213, 0x0F},
	{0x3215, 0x0F},
	{0x3217, 0x0B},
	{0x3219, 0x0B},
	{0x321B, 0x0D},
	{0x321D, 0x0D},

	{0x0340,	0x0C},
	{0x0341,	0x8C},
	{0x0342,	0x12},
	{0x0343,	0x0C},
	{0x0344,	0x00},
	{0x0345,	0x08},
	{0x0346,	0x00},
	{0x0347,	0x30},
	{0x0348,	0x10},
	{0x0349,	0x77},
	{0x034A,	0x0C},
	{0x034B,	0x5F},
	{0x034C,	0x10},
	{0x034D,	0x70},
	{0x034E,	0x0C},
	{0x034F,	0x30},
	{0x0381,	0x01},
	{0x0383,	0x01},
	{0x0385,	0x01},
	{0x0387,	0x01},
	{0x3033,	0x00},
	{0x303D,	0x10},
	{0x303E,	0xD0},
	{0x3048,	0x00},
	{0x304C,	0x7F},
	{0x304D,	0x04},
	{0x3064,	0x12},
	{0x309B,	0x20},
	{0x309E,	0x00},
	{0x30A0,	0x14},
	{0x30B2,	0x00},
	{0x30D5,	0x00},
	{0x30D6,	0x85},
	{0x30D7,	0x2A},
	{0x30D8,	0x64},
	{0x30D9,	0x89},
	{0x30DE,	0x00},
	{0x3102,	0x10},
	{0x3103,	0x44},
	{0x3104,	0x40},
	{0x3105,	0x00},
	{0x3106,	0x0D},
	{0x3107,	0x01},
	{0x310A,	0x0A},
	{0x315C,	0x99},
	{0x315D,	0x98},
	{0x316E,	0x9A},
	{0x316F,	0x99},
	{0x3301,	0x03},
	{0x3304,	0x05},
	{0x3305,	0x04},
	{0x3306,	0x12},
	{0x3307,	0x03},
	{0x3308,	0x0D},
	{0x330A,	0x09},
	{0x330C,	0x08},
	{0x330D,	0x05},
	{0x330E,	0x03},
	{0x3318,	0x64},
	{0x3322,	0x02},
	{0x3342,	0x0F},
};

static struct msm_camera_i2c_reg_conf imx091_video_settings[] = {

	
	{0x0305, 0x04}, 
	{0x0307, 0x92}, 
       {0x30A4, 0x02}, 
	{0x303C, 0x4B},

       
	{0x3031, 0x10}, 
	{0x3064, 0x12},
	{0x3087, 0x57},
	{0x308A, 0x35},
	{0x3091, 0x41},
	{0x3098, 0x03},
	{0x3099, 0xC0},
	{0x309A, 0xA3},
	{0x309D, 0x94},
	{0x30AB, 0x01},
	{0x30AD, 0x08},
	{0x30B1, 0x03},
	{0x30C4, 0x13},
	{0x30F3, 0x03},
	{0x3116, 0x31},
	{0x3117, 0x38},
	{0x3138, 0x28},
	{0x3137, 0x14},
	{0x3139, 0x2E},
	{0x314D, 0x2A},
	{0x3343, 0x04},

       {0x0340, 0x06}, 
	{0x0341, 0xDE}, 
       {0x0342, 0x0D}, 
	{0x0343, 0x48}, 
	{0x0344, 0x00}, 
	{0x0345, 0x66},  
       {0x0346, 0x01}, 
	{0x0347, 0x90}, 
       {0x0348, 0x0C},
       {0x0349, 0x71}, 
	{0x034A, 0x08}, 
	{0x034B, 0x57}, 
	{0x034C, 0x0C}, 
	{0x034D, 0x0C}, 
	{0x034E, 0x06}, 
	{0x034F, 0xC8}, 
	{0x0381, 0x01}, 
	{0x0383, 0x01}, 
	{0x0385, 0x01}, 
	{0x0387, 0x01}, 
	{0x3033, 0x00},
       {0x303D, 0x70},
       {0x303E, 0x41},
       {0x3040, 0x08},
       {0x3041, 0x89},
       {0x3048, 0x00},
	{0x304C, 0x50},
	{0x304D, 0x03},
	{0x306A, 0xD2},
	{0x309B, 0x00},
	{0x309C, 0x34},
	{0x309E, 0x00},
	{0x30AA, 0x02},
	{0x30D5, 0x00},
	{0x30D6, 0x85},
	{0x30D7, 0x2A},
       {0x30D8, 0x64},
       {0x30D9, 0x89},
       {0x30DE, 0x00},
	{0x3102, 0x10},
	{0x3103, 0x44},
	{0x3104, 0x40},
	{0x3105, 0x00},
	{0x3106, 0x10},
	{0x3107, 0x01},
	{0x315C, 0x76},
	{0x315D, 0x75},
	{0x316E, 0x77},
	{0x316F, 0x76},
       {0x3301, 0x00},
       {0x3318, 0x61},
	{0x0202, 0x04}, 
	{0x0203, 0xED}
};


static struct msm_camera_i2c_reg_conf imx091_fast_video_settings[] = {

	
	
	{0x0305, 0x04}, 
	{0x0307, 0x70}, 
       {0x30A4, 0x02}, 
	{0x303C, 0x4B},

       
	{0x3031, 0x10}, 
	{0x3064, 0x12},
	{0x3087, 0x57},
	{0x308A, 0x35},
	{0x3091, 0x41},
	{0x3098, 0x03},
	{0x3099, 0xC0},
	{0x309A, 0xA3},
	{0x309D, 0x94},
	{0x30AB, 0x01},
	{0x30AD, 0x08},
	{0x30B1, 0x03},
	{0x30C4, 0x13},
	{0x30F3, 0x03},
	{0x3116, 0x31},
	{0x3117, 0x38},
	{0x3138, 0x28},
	{0x3137, 0x14},
	{0x3139, 0x2E},
	{0x314D, 0x2A},
	{0x3343, 0x04},

	{0x0340, 0x02}, 
	{0x0341, 0x78},
	{0x0342, 0x06}, 
	{0x0343, 0xE8},
	{0x0344, 0x00},
	{0x0345, 0x04},
	{0x0346, 0x00},
	{0x0347, 0xF8},
       {0x0348, 0x0C},
       {0x0349, 0xD3},
	{0x034A, 0x08}, 
	{0x034B, 0xEF},
	{0x034C, 0x06}, 
	{0x034D, 0x68},
	{0x034E, 0x01}, 
	{0x034F, 0xFE},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x05},
	{0x0387, 0x03},
	{0x3033, 0x84},
       {0x303D, 0x70},
       {0x303E, 0x40},
       {0x3040, 0x08},
       {0x3041, 0x97},
	{0x3048, 0x01},
	{0x304C, 0xB7},
	{0x304D, 0x01},
	{0x306A, 0xD2},
	{0x309B, 0x08},
	{0x309C, 0x33},
	{0x309E, 0x04},
	{0x30AA, 0x00},
	{0x30D5, 0x04},
	{0x30D6, 0x85},
	{0x30D7, 0x2A},
	{0x30D8, 0x64},
	{0x30D9, 0x89},
	{0x30DE, 0x00},
	{0x3102, 0x05},
	{0x3103, 0x12},
	{0x3104, 0x12},
	{0x3105, 0x00},
	{0x3106, 0x46},
	{0x3107, 0x00},
	{0x315C, 0x4A},
	{0x315D, 0x49},
	{0x316E, 0x4B},
	{0x316F, 0x4A},
       {0x3301, 0x00},
	{0x3318, 0x62},
	{0x0202, 0x04}, 
	{0x0203, 0xED}
};


static struct msm_camera_i2c_reg_conf imx091_snap_settings[] = {
	{0x0305, 0x02},  
	{0x0307, 0x2F},  
	{0x303C, 0x4B},  

	{0x3087, 0x53},
	{0x309D, 0x94},
	{0x30A1, 0x08},
	{0x30C7, 0x00},
	{0x3115, 0x0E},
	{0x3118, 0x42},
	{0x311D, 0x34},
	{0x3121, 0x0D},
	{0x3212, 0xF2},
	{0x3213, 0x0F},
	{0x3215, 0x0F},
	{0x3217, 0x0B},
	{0x3219, 0x0B},
	{0x321B, 0x0D},
	{0x321D, 0x0D},

	{0x0340,	0x0C},
	{0x0341,	0x8C},
	{0x0342,	0x12},
	{0x0343,	0x0C},
	{0x0344,	0x00},
	{0x0345,	0x08},
	{0x0346,	0x00},
	{0x0347,	0x30},
	{0x0348,	0x10},
	{0x0349,	0x77},
	{0x034A,	0x0C},
	{0x034B,	0x5F},
	{0x034C,	0x10},
	{0x034D,	0x70},
	{0x034E,	0x0C},
	{0x034F,	0x30},
	{0x0381,	0x01},
	{0x0383,	0x01},
	{0x0385,	0x01},
	{0x0387,	0x01},
	{0x3033,	0x00},
	{0x303D,	0x10},
	{0x303E,	0xD0},
	{0x3048,	0x00},
	{0x304C,	0x7F},
	{0x304D,	0x04},
	{0x3064,	0x12},
	{0x309B,	0x20},
	{0x309E,	0x00},
	{0x30A0,	0x14},
	{0x30B2,	0x00},
	{0x30D5,	0x00},
	{0x30D6,	0x85},
	{0x30D7,	0x2A},
	{0x30D8,	0x64},
	{0x30D9,	0x89},
	{0x30DE,	0x00},
	{0x3102,	0x10},
	{0x3103,	0x44},
	{0x3104,	0x40},
	{0x3105,	0x00},
	{0x3106,	0x0D},
	{0x3107,	0x01},
	{0x310A,	0x0A},
	{0x315C,	0x99},
	{0x315D,	0x98},
	{0x316E,	0x9A},
	{0x316F,	0x99},
	{0x3301,	0x03},
	{0x3304,	0x05},
	{0x3305,	0x04},
	{0x3306,	0x12},
	{0x3307,	0x03},
	{0x3308,	0x0D},
	{0x330A,	0x09},
	{0x330C,	0x08},
	{0x330D,	0x05},
	{0x330E,	0x03},
	{0x3318,	0x64},
	{0x3322,	0x02},
	{0x3342,	0x0F},
};

static struct msm_camera_i2c_reg_conf imx091_recommend_settings[] = {
	{0x3087, 0x53},
	{0x309D, 0x94},
	{0x30A1, 0x08},
	{0x30C7, 0x00},
	{0x3115, 0x0E},
	{0x3118, 0x42},
	{0x311D, 0x34},
	{0x3121, 0x0D},
	{0x3212, 0xF2},
	{0x3213, 0x0F},
	{0x3215, 0x0F},
	{0x3217, 0x0B},
	{0x3219, 0x0B},
	{0x321B, 0x0D},
	{0x321D, 0x0D},

	
	{0x3032, 0x40}
};

static struct v4l2_subdev_info imx091_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	
};

static struct msm_camera_i2c_conf_array imx091_init_conf[] = {
	{&imx091_mipi_settings[0],
	ARRAY_SIZE(imx091_mipi_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx091_recommend_settings[0],
	ARRAY_SIZE(imx091_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx091_pll_settings[0],
	ARRAY_SIZE(imx091_pll_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_conf_array imx091_confs[] = {
	{&imx091_snap_settings[0],
	ARRAY_SIZE(imx091_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx091_prev_settings[0],
	ARRAY_SIZE(imx091_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx091_video_settings[0],
	ARRAY_SIZE(imx091_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx091_fast_video_settings[0],
	ARRAY_SIZE(imx091_fast_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t imx091_dimensions[] = {
	{
		.x_output = 0x1070, 
		.y_output = 0xC30, 
		.line_length_pclk =  0x120C,  
		.frame_length_lines = 0xC8C, 
		.vt_pixel_clk = 225600000,
		.op_pixel_clk = 225600000,
		.binning_factor = 1,
		.x_addr_start = 0x08, 
		.y_addr_start = 0x30, 
		.x_addr_end = 0x1077, 
		.y_addr_end = 0xC5F, 
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
		.binning_rawchip = 0x11,
	},
	{
		.x_output = 0x1070, 
		.y_output = 0xC30, 
		.line_length_pclk =  0x120C,  
		.frame_length_lines = 0xC8C, 
		.vt_pixel_clk = 225600000,
		.op_pixel_clk = 225600000,
		.binning_factor = 1,
		.x_addr_start = 0x08, 
		.y_addr_start = 0x30, 
		.x_addr_end = 0x1077, 
		.y_addr_end = 0xC5F, 
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
		.binning_rawchip = 0x11,
	},
	{
		.x_output = 0xC0C,
		.y_output = 0x6C8,
		.line_length_pclk = 0xD48,
              .frame_length_lines = 0x6DE, 
		.vt_pixel_clk = 175200000,
		.op_pixel_clk = 175200000,
		.binning_factor = 1,
	},
	{
		.x_output = 0x668,
		.y_output = 0x1FE,
		.line_length_pclk = 0x6E8,
		.frame_length_lines = 0x278,
		.vt_pixel_clk = 134400000,
		.op_pixel_clk = 134400000,
		.binning_factor = 1,
	},
};

static struct msm_camera_csid_vc_cfg imx091_cid_cfg[] = {
	{0, CSI_RAW10, CSI_DECODE_10BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params imx091_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 4,
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = imx091_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 4,
		
		.settle_cnt = 0x15,
	},
};

static struct msm_camera_csi2_params *imx091_csi_params_array[] = {
	&imx091_csi_params,
	&imx091_csi_params,
	&imx091_csi_params,
	&imx091_csi_params
};

static struct msm_sensor_output_reg_addr_t imx091_reg_addr = {
	.x_output = 0x34C,
	.y_output = 0x34E,
	.line_length_pclk = 0x342,
	.frame_length_lines = 0x340,
};

static struct msm_sensor_id_info_t imx091_id_info = {
	.sensor_id_reg_addr = 0x0,
	.sensor_id = 0x091,
};

static struct msm_sensor_exp_gain_info_t imx091_exp_gain_info = {
	.coarse_int_time_addr = 0x202,
	.global_gain_addr = 0x204,
	.vert_offset = 5,  
	.min_vert = 4,  
};

static int imx091_sensor_open_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	uint16_t value = 0;

	if (data->sensor_platform_info)
		imx091_s_ctrl.mirror_flip = data->sensor_platform_info->mirror_flip;

	
	if (imx091_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR_FLIP)
		value = IMX091_READ_MIRROR_FLIP;
	else if (imx091_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR)
		value = IMX091_READ_MIRROR;
	else if (imx091_s_ctrl.mirror_flip == CAMERA_SENSOR_FLIP)
		value = IMX091_READ_FLIP;
	else
		value = IMX091_READ_NORMAL_MODE;
	msm_camera_i2c_write(imx091_s_ctrl.sensor_i2c_client,
		IMX091_REG_READ_MODE, value, MSM_CAMERA_I2C_BYTE_DATA);

	return rc;
}

static const struct i2c_device_id imx091_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&imx091_s_ctrl},
	{ }
};

static struct i2c_driver imx091_i2c_driver = {
	.id_table = imx091_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client imx091_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


int32_t msm_camera_i2c_write_lens_position(int16_t lens_position)
{
	struct msm_sensor_ctrl_t *s_ctrl = &imx091_s_ctrl;
	int rc = 0;

	pr_info("%s lens_position %d\n", __func__, lens_position);


       s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		0x3403,
		((lens_position & 0x0300) >> 8),
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s VCM_CODE_MSB i2c write failed (%d)\n", __func__, rc);
		return rc;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		0x3402,
		(lens_position & 0x00FF),
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s VCM_CODE_LSB i2c write failed (%d)\n", __func__, rc);
		return rc;
	}

	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);

	return rc;
}

int32_t imx091_power_up(struct msm_sensor_ctrl_t *s_ctrl)
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

#if 1	
	imx091_sensor_open_init(sdata);
	pr_info("%s end\n", __func__);
#endif	

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

int32_t imx091_power_down(struct msm_sensor_ctrl_t *s_ctrl)
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

static int __init msm_sensor_init_module(void)
{
	pr_info("imx091 %s\n", __func__);
	return i2c_add_driver(&imx091_i2c_driver);
}

static struct v4l2_subdev_core_ops imx091_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops imx091_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops imx091_subdev_ops = {
	.core = &imx091_subdev_core_ops,
	.video  = &imx091_subdev_video_ops,
};

static int imx091_read_fuseid(struct sensor_cfg_data *cdata,
	struct msm_sensor_ctrl_t *s_ctrl)
{


	int32_t  rc;
	uint16_t info_value = 0;
	unsigned short info_index = 0;
	unsigned short  OTP[10] = {0};

	struct msm_camera_i2c_client *imx091_msm_camera_i2c_client = s_ctrl->sensor_i2c_client;

	pr_info("%s: sensor OTP information: E \n", __func__);


	for (info_index = 0; info_index < 10; info_index++) {
		rc = msm_camera_i2c_write_b(imx091_msm_camera_i2c_client ,0x34C9, info_index);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x34C9 (select info_index %d) fail\n", __func__, info_index);

		
		rc = msm_camera_i2c_read(imx091_msm_camera_i2c_client,0x3500, &info_value,2);
		if (rc < 0)
			pr_err("%s: i2c_read 0x3500 fail\n", __func__);

		OTP[info_index] = (unsigned short)((info_value & 0xFF00) >>8);
              info_index++;
		OTP[info_index] = (unsigned short)(info_value & 0x00FF);
		info_value = 0;
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
	pr_info("%s: sensor OTP information: X \n", __func__);


	return 0;
}


int32_t imx091_sensor_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		int mode, uint16_t gain, uint32_t line) 
{
	uint32_t fl_lines;
	uint8_t offset;
	uint32_t fps_divider = Q10;

	if (mode == SENSOR_PREVIEW_MODE)
		fps_divider = s_ctrl->fps_divider;
	fl_lines = s_ctrl->curr_frame_length_lines;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line * Q10 > (fl_lines - offset) * fps_divider)
		fl_lines = line + offset;
	else
		fl_lines = (fl_lines * fps_divider) / Q10;

if (line > fl_lines -offset)
	line = fl_lines -offset;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);

	pr_info("%s write fl_lines : %d ; write line_cnt : %d ; write gain : %d \n", __func__, fl_lines,  line, gain);

	return 0;
}

static int imx091_read_VCM_driver_IC_info(	struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t  rc;
	int page = 0, bank = 0;
	unsigned short info_value = 0;
	unsigned short  OTP = 0;
	struct msm_camera_i2c_client *msm_camera_i2c_client = s_ctrl->sensor_i2c_client;
	struct msm_camera_sensor_info *sdata;

	pr_info("%s: sensor OTP information:\n", __func__);

	s_ctrl = get_sctrl(&s_ctrl->sensor_v4l2_subdev);
	sdata = (struct msm_camera_sensor_info *) s_ctrl->sensordata;

	if ((sdata->actuator_info_table == NULL) || (sdata->num_actuator_info_table <= 1))
	{
		pr_info("%s: actuator_info_table == NULL or num_actuator_info_table <= 1 return 0\n", __func__);
		return 0;
	}

	
	page = 2;
	//read 1,3,5,7,9 bank because OTP memory may be written fail.
	for (bank = 9; bank >= 0;  bank = bank - 2) {
		
		rc = msm_camera_i2c_write_b(msm_camera_i2c_client, 0x34C9, bank);

		
		rc = msm_camera_i2c_read_b(msm_camera_i2c_client, (0x3500 + (8 * bank) + page), &info_value);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x3500 + (8 * bank) + page));

		OTP = (short)(info_value&0x0F);

		if (OTP != 0)
			break;

		info_value = 0;
	}

	if (sdata->num_actuator_info_table > 1)
	{
		if (OTP == 1) 
			sdata->actuator_info = &sdata->actuator_info_table[2][0];
		else if (OTP == 2) 
			sdata->actuator_info = &sdata->actuator_info_table[1][0];

		pr_info("%s: sdata->actuator_info->board_info->type=%s", __func__, sdata->actuator_info->board_info->type);
		pr_info("%s: sdata->actuator_info->board_info->addr=0x%x", __func__, sdata->actuator_info->board_info->addr);
	}

	
	return 0;
}

static struct msm_sensor_fn_t imx091_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain_ex = msm_sensor_write_exp_gain1_ex,
	.sensor_write_snapshot_exp_gain_ex = msm_sensor_write_exp_gain1_ex,
	.sensor_setting = msm_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = imx091_power_up,
	.sensor_power_down = imx091_power_down,
	.sensor_i2c_read_fuseid = imx091_read_fuseid,
	.sensor_i2c_read_vcm_driver_ic = imx091_read_VCM_driver_IC_info
};

static struct msm_sensor_reg_t imx091_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = imx091_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(imx091_start_settings),
	.stop_stream_conf = imx091_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(imx091_stop_settings),
	.group_hold_on_conf = imx091_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(imx091_groupon_settings),
	.group_hold_off_conf = imx091_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(imx091_groupoff_settings),
	.init_settings = &imx091_init_conf[0],
	.init_size = ARRAY_SIZE(imx091_init_conf),
	.mode_settings = &imx091_confs[0],
	.output_settings = &imx091_dimensions[0],
	.num_conf = ARRAY_SIZE(imx091_confs),
};

static struct msm_sensor_ctrl_t imx091_s_ctrl = {
	.msm_sensor_reg = &imx091_regs,
	.sensor_i2c_client = &imx091_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,  
	.sensor_output_reg_addr = &imx091_reg_addr,
	.sensor_id_info = &imx091_id_info,
	.sensor_exp_gain_info = &imx091_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csi_params = &imx091_csi_params_array[0],
	.msm_sensor_mutex = &imx091_mut,
	.sensor_i2c_driver = &imx091_i2c_driver,
	.sensor_v4l2_subdev_info = imx091_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(imx091_subdev_info),
	.sensor_v4l2_subdev_ops = &imx091_subdev_ops,
	.func_tbl = &imx091_func_tbl,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sony 8MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");

