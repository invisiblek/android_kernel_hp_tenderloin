#include "msm_sensor.h"

#ifdef CONFIG_RAWCHIP
#include "rawchip/rawchip.h"
#endif

#define SENSOR_NAME "ov8838"
#define PLATFORM_DRIVER_NAME "msm_camera_ov8838"
#define ov8838_obj ov8838_##obj

#define OV8838_REG_READ_MODE 0x0101
#define OV8838_READ_NORMAL_MODE 0x0000	
#define OV8838_READ_MIRROR 0x0001			
#define OV8838_READ_FLIP 0x0002			
#define OV8838_READ_MIRROR_FLIP 0x0003	

#define REG_DIGITAL_GAIN_GREEN_R 0x020E
#define REG_DIGITAL_GAIN_RED 0x0210
#define REG_DIGITAL_GAIN_BLUE 0x0212
#define REG_DIGITAL_GAIN_GREEN_B 0x0214
DEFINE_MUTEX(ov8838_sensor_init_mut);

#if 0
#define DEFAULT_VCM_MAX 73
#define DEFAULT_VCM_MED 35
#define DEFAULT_VCM_MIN 8
#endif

DEFINE_MUTEX(ov8838_mut);
static struct msm_sensor_ctrl_t ov8838_s_ctrl;

static struct msm_camera_i2c_reg_conf ov8838_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf ov8838_stop_settings[] = {
	{0x0100, 0x00}, 
};

static struct msm_camera_i2c_reg_conf ov8838_groupon_settings[] = {
	{0x104, 0x01},
};

static struct msm_camera_i2c_reg_conf ov8838_groupoff_settings[] = {
	{0x104, 0x00},
};

static struct msm_camera_i2c_reg_conf ov8838_mipi_settings[] = {
};

static struct msm_camera_i2c_reg_conf ov8838_pll_settings[] = {
};

static struct msm_camera_i2c_reg_conf ov8838_recommend_settings[] = {
	{ 0x0100, 0x00 },
	{ 0x0103, 0x01 },

	{ 0x0100, 0x00 },
	{ 0x0100, 0x00 },
	{ 0x0100, 0x00 },

	{ 0x0100, 0x00 },
	{ 0x0102, 0x01 },
	{ 0x3001, 0x2a },
	{ 0x3002, 0x88 },
	{ 0x3005, 0x00 },
	{ 0x3011, 0x41 },
	{ 0x3015, 0x08 },
	{ 0x301b, 0xb4 },
	{ 0x301d, 0x02 },
	{ 0x3021, 0x00 },
	{ 0x3022, 0x00 },
	{ 0x3081, 0x02 },
	{ 0x3083, 0x01 },

	
	{ 0x3090, 0x03 },
	{ 0x3091, 0x22 },
	{ 0x3092, 0x00 },
	{ 0x3093, 0x00 },
	{ 0x3094, 0x00 },
	

	
	{ 0x3098, 0x04 },
	{ 0x3099, 0x10 },
	{ 0x309a, 0x00 },
	{ 0x309b, 0x00 },
	

	{ 0x30a2, 0x01 }, 

	
	{ 0x30b0, 0x05 },
	{ 0x30b2, 0x00 },
	{ 0x30b3, 0x51 },
	{ 0x30b4, 0x03 },
	{ 0x30b5, 0x04 },
	

	{ 0x30b6, 0x01 },
	{ 0x3104, 0xa1 },
	{ 0x3106, 0x01 },
	{ 0x3400, 0x04 },
	{ 0x3401, 0x00 },
	{ 0x3402, 0x04 },
	{ 0x3403, 0x00 },
	{ 0x3404, 0x04 },
	{ 0x3405, 0x00 },
	{ 0x3406, 0x01 },
	{ 0x3500, 0x00 },
	{ 0x3501, 0x20 },
	{ 0x3502, 0xc0 },
	{ 0x3503, 0x07 },
	{ 0x3504, 0x00 },
	{ 0x3505, 0x30 },
	{ 0x3506, 0x00 },
	{ 0x3507, 0x10 },
	{ 0x3508, 0x80 },
	{ 0x3509, 0x10 },
	{ 0x350a, 0x00 },
	{ 0x350b, 0x38 },

	
	{ 0x3600, 0xb8 },
	{ 0x3601, 0x0a },
	{ 0x3602, 0x9c },
	{ 0x3604, 0x38 },
	{ 0x3612, 0x80 },
	{ 0x3620, 0x41 },
	{ 0x3621, 0xa4 },
	{ 0x3622, 0x0b },
	{ 0x3625, 0x44 },
	{ 0x3630, 0x55 },
	{ 0x3631, 0xd2 },
	{ 0x3632, 0x00 },
	{ 0x3633, 0x34 },
	{ 0x3634, 0x03 },
	{ 0x364d, 0x0d },
	{ 0x364f, 0x60 },
	{ 0x3660, 0x80 },
	{ 0x3662, 0x10 },
	{ 0x3665, 0x00 },
	{ 0x3666, 0x00 },
	{ 0x3667, 0x00 },
	{ 0x366a, 0x80 },
	{ 0x366c, 0x00 },
	{ 0x366d, 0x00 },
	{ 0x366e, 0x00 },
	{ 0x366f, 0x20 },
	{ 0x3680, 0xb5 },
	{ 0x3681, 0x00 },
	{ 0x3701, 0x14 },
	{ 0x3702, 0x50 },
	{ 0x3703, 0x8c },
	{ 0x3704, 0x68 },
	{ 0x3705, 0x02 },
	{ 0x3708, 0xe3 },
	{ 0x3709, 0x43 },
	{ 0x370a, 0x00 },
	{ 0x370b, 0x20 },
	{ 0x370c, 0x0c },
	{ 0x370d, 0x11 },
	{ 0x370e, 0x00 },
	{ 0x370f, 0x00 },
	{ 0x3710, 0x00 },
	{ 0x371c, 0x01 },
	{ 0x371f, 0x0c },
	{ 0x3721, 0x00 },
	{ 0x3724, 0x10 },
	{ 0x3726, 0x00 },
	{ 0x372a, 0x01 },
	{ 0x3730, 0x18 },
	{ 0x3738, 0x22 },
	{ 0x3739, 0xd0 },
	{ 0x373a, 0x50 },
	{ 0x373b, 0x02 },
	{ 0x373c, 0x20 },
	{ 0x373f, 0x02 },
	{ 0x3740, 0x42 },
	{ 0x3741, 0x02 },
	{ 0x3742, 0x18 },
	{ 0x3743, 0x01 },
	{ 0x3744, 0x02 },
	{ 0x3747, 0x10 },
	{ 0x374c, 0x04 },
	{ 0x3751, 0xf0 },
	{ 0x3752, 0x00 },
	{ 0x3753, 0x00 },
	{ 0x3754, 0xc0 },
	{ 0x3755, 0x00 },
	{ 0x3756, 0x1a },
	{ 0x3758, 0x00 },
	{ 0x3759, 0x0f },
	{ 0x375c, 0x04 },
	{ 0x3767, 0x01 },
	{ 0x376b, 0x44 },
	{ 0x3774, 0x10 },
	{ 0x3776, 0x00 },
	{ 0x377f, 0x08 },
	{ 0x3780, 0x22 },
	{ 0x3781, 0xcc },
	{ 0x3784, 0x2c },
	{ 0x3785, 0x08 },
	{ 0x3786, 0x16 },
	{ 0x378f, 0xf5 },
	{ 0x3791, 0xb0 },
	{ 0x3795, 0x00 },
	{ 0x3796, 0x94 },
	{ 0x3797, 0x11 },
	{ 0x3798, 0x30 },
	{ 0x3799, 0x41 },
	{ 0x379a, 0x07 },
	{ 0x379b, 0xb0 },
	{ 0x379c, 0x0c },
	{ 0x37c5, 0x00 },
	{ 0x37c6, 0xa0 },
	{ 0x37c7, 0x00 },
	{ 0x37c9, 0x00 },
	{ 0x37ca, 0x00 },
	{ 0x37cb, 0x00 },
	{ 0x37cc, 0x00 },
	{ 0x37cd, 0x00 },
	{ 0x37ce, 0x01 },
	{ 0x37cf, 0x00 },
	{ 0x37d1, 0x01 },
	{ 0x37de, 0x00 },
	{ 0x37df, 0x00 },

	
	{ 0x3800, 0x00 },
	{ 0x3801, 0x04 },
	{ 0x3802, 0x01 },
	{ 0x3803, 0x38 },
	{ 0x3804, 0x0c },
	{ 0x3805, 0xdb },
	{ 0x3806, 0x08 },
	{ 0x3807, 0x75 },
	{ 0x3808, 0x0c },
	{ 0x3809, 0xd0 },
	{ 0x380a, 0x07 },
	{ 0x380b, 0x36 },
	{ 0x380c, 0x0e },
	{ 0x380d, 0x18 },
	{ 0x380e, 0x09 },
	{ 0x380f, 0xce },    
	{ 0x3810, 0x00 },
	{ 0x3811, 0x04 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x04 },
	{ 0x3814, 0x11 },
	{ 0x3815, 0x11 },
	{ 0x3820, 0x10 },
	{ 0x3821, 0x0e },
	{ 0x3823, 0x00 },
	{ 0x3824, 0x00 },
	{ 0x3825, 0x00 },
	{ 0x3826, 0x00 },
	{ 0x3827, 0x00 },
	{ 0x382a, 0x04 },
	{ 0x3a04, 0x09 },
	{ 0x3a05, 0xa9 },
	{ 0x3a06, 0x00 },
	{ 0x3a07, 0xf8 },
	{ 0x3b00, 0x00 },
	{ 0x3b02, 0x00 },
	{ 0x3b03, 0x00 },
	{ 0x3b04, 0x00 },
	{ 0x3b05, 0x00 },
	{ 0x3d00, 0x00 },
	{ 0x3d01, 0x00 },
	{ 0x3d02, 0x00 },
	{ 0x3d03, 0x00 },
	{ 0x3d04, 0x00 },
	{ 0x3d05, 0x00 },
	{ 0x3d06, 0x00 },
	{ 0x3d07, 0x00 },
	{ 0x3d08, 0x00 },
	{ 0x3d09, 0x00 },
	{ 0x3d0a, 0x00 },
	{ 0x3d0b, 0x00 },
	{ 0x3d0c, 0x00 },
	{ 0x3d0d, 0x00 },
	{ 0x3d0e, 0x00 },
	{ 0x3d0f, 0x00 },
	{ 0x3d80, 0x00 },
	{ 0x3d81, 0x00 },
	{ 0x3d84, 0x00 },
	{ 0x4000, 0x18 },
	{ 0x4001, 0x04 },
	{ 0x4002, 0x45 },
	{ 0x4004, 0x08 },
	{ 0x4005, 0x18 },
	{ 0x4006, 0x20 },
	{ 0x4008, 0x24 },
	{ 0x4009, 0x40 },  
	{ 0x404f, 0x90 },
	{ 0x4100, 0x20 },
	{ 0x4101, 0x03 },
	{ 0x4102, 0x04 },
	{ 0x4103, 0x03 },
	{ 0x4104, 0x5a },
	{ 0x4307, 0x30 },
	{ 0x4315, 0x00 },
	{ 0x4511, 0x05 },
	{ 0x4512, 0x01 },
	{ 0x4805, 0x21 },
	{ 0x4806, 0x00 },
	{ 0x481f, 0x36 },
	{ 0x4831, 0x6c },
	{ 0x4837, 0x0c },
	{ 0x4a00, 0xaa },
	{ 0x4a03, 0x01 },
	{ 0x4a05, 0x08 },
	{ 0x4a0a, 0x88 },
	{ 0x4d03, 0xbb },
	{ 0x5000, 0x06 },
	{ 0x5001, 0x01 },
	{ 0x5002, 0x80 },
	{ 0x5003, 0x20 },
	{ 0x5013, 0x00 },
	{ 0x5046, 0x4a },
	{ 0x5780, 0x1c },
	{ 0x5786, 0x20 },
	{ 0x5787, 0x10 },
	{ 0x5788, 0x18 },
	{ 0x578a, 0x04 },
	{ 0x578b, 0x02 },
	{ 0x578c, 0x02 },
	{ 0x578e, 0x06 },
	{ 0x578f, 0x02 },
	{ 0x5790, 0x02 },
	{ 0x5791, 0xff },
	{ 0x5a08, 0x02 },
	{ 0x5e00, 0x00 },
	{ 0x5e10, 0x0c },
};

static struct msm_camera_i2c_reg_conf ov8838_prev_settings[] = {
	 
	{0x3208, 0x03},

	
	{0x3800, 0x03},	
	{0x3801, 0x38},
	{0x3802, 0x01}, 
	{0x3803, 0x34},
	{0x3804, 0x09},	
	{0x3805, 0xa7},
	{0x3806, 0x08},	
	{0x3807, 0x7B},
	{0x3808, 0x06},	
	{0x3809, 0x68},
	{0x380a, 0x04},	
	{0x380b, 0xD0},
	{0x380c, 0x0e},	
	{0x380d, 0x28},
	{0x380e, 0x09},	
	{0x380f, 0xce},    
	{0x3810, 0x00},
	{0x3811, 0x04},
	{0x3812, 0x00},
	{0x3813, 0x04},  
	{0x3814, 0x11},
	{0x3815, 0x11},

	{0x3a04, 0x07},
	{0x3a05, 0x49},

	
	{0x3016, 0xf9},
	{0x3016, 0xf0},
	{0x3208, 0x13},
	{0x3208, 0xa3},
};

static struct msm_camera_i2c_reg_conf ov8838_video_settings[] = {
	
	{0x3208, 0x01},

	
	{0x3800, 0x00},	
	{0x3801, 0x64},
	{0x3802, 0x01},
	{0x3803, 0x70},
	{0x3804, 0x0c},	
	{0x3805, 0x7B},
	{0x3806, 0x08},
	{0x3807, 0x3F},
	{0x3808, 0x0c},
	{0x3809, 0x0C},
	{0x380a, 0x06},
	{0x380b, 0xC8},
	{0x380c, 0x0e},	
	{0x380d, 0x28},
	{0x380e, 0x06},	
	{0x380f, 0xFA},
	{0x3810, 0x00},
	{0x3811, 0x06},
	{0x3812, 0x00},
	{0x3813, 0x04},
	{0x3814, 0x11},
	{0x3815, 0x11},

	{0x3820, 0x10},
	{0x3821, 0x0e},

	{0x3a04, 0x07},
	{0x3a05, 0x49},
	{0x3708, 0xe3},
	{0x3709, 0x43},
	{0x4512, 0x01},

	
	{0x3016, 0xf9},
	{0x3016, 0xf0},
	{0x3208, 0x11},
	{0x3208, 0xa1},
};

static struct msm_camera_i2c_reg_conf ov8838_fast_video_settings[] = {
	
	{ 0x3208, 0x03 },

	
	{ 0x3800, 0x00 },
	{ 0x3801, 0x00 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0xc0 },
	{ 0x3804, 0x0c },
	{ 0x3805, 0xdf },
	{ 0x3806, 0x08 },
	{ 0x3807, 0xed },
	{ 0x3808, 0x06 },
	{ 0x3809, 0x68 },
	{ 0x380a, 0x01 },
	{ 0x380b, 0xfe },
	{ 0x380c, 0x0e },
	{ 0x380d, 0x28 },
	{ 0x380e, 0x02 },
	{ 0x380f, 0x53 },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x04 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x04 },
	{ 0x3814, 0x31 },
	{ 0x3815, 0x71 },

	{ 0x3820, 0x11 },
	{ 0x3821, 0x0f },

	{ 0x3a04, 0x04 },
	{ 0x3a05, 0xc9 },
	{ 0x3708, 0xe6 },
	{ 0x3709, 0x03 },
	{ 0x4512, 0x00 },

	
	{ 0x3016, 0xf9 },
	{ 0x3016, 0xf0 },
	{ 0x3208, 0x13 }, 
	{ 0x3208, 0xa3 },
};

static struct msm_camera_i2c_reg_conf ov8838_snap_settings[] = {
	
	{ 0x3208, 0x00 },

	
	{ 0x3800, 0x00 },
	{ 0x3801, 0x04 },
	{ 0x3802, 0x00 },
	{ 0x3803, 0x04 },
	{ 0x3804, 0x0c },
	{ 0x3805, 0xdb },
	{ 0x3806, 0x09 },
	{ 0x3807, 0xab },
	{ 0x3808, 0x0c },
	{ 0x3809, 0xd0 },
	{ 0x380a, 0x09 },
	{ 0x380b, 0xa0 },
	{ 0x380c, 0x0e },
	{ 0x380d, 0x28 },
	{ 0x380e, 0x09 },
	{ 0x380f, 0xe6 },
	{ 0x3810, 0x00 },
	{ 0x3811, 0x04 },
	{ 0x3812, 0x00 },
	{ 0x3813, 0x04 },
	{ 0x3814, 0x11 },
	{ 0x3815, 0x11 },

	{ 0x3820, 0x10},
	{ 0x3821, 0x0e},

	{ 0x3a04, 0x09 },
	{ 0x3a05, 0xa9 },

	{ 0x3708, 0xe3},
	{ 0x3709, 0x43},
	{ 0x4512, 0x01},

	
	{ 0x3016, 0xf9 },
	{ 0x3016, 0xf0 },
	{ 0x3208, 0x10 },
	{ 0x3208, 0xa0 },
};

static struct msm_camera_i2c_reg_conf ov8838_snap_wide_settings[] = {
	
	{0x3208, 0x01},

	
	{0x3800, 0x00},	
	{0x3801, 0x04},
	{0x3802, 0x01},	
	{0x3803, 0x34},
	{0x3804, 0x0c},	
	{0x3805, 0xdb},
	{0x3806, 0x08},	
	{0x3807, 0x7B},
	{0x3808, 0x0c},	
	{0x3809, 0xd0},
	{0x380a, 0x07},	
	{0x380b, 0x40},
	{0x380c, 0x0e},	
	{0x380d, 0x28},
	{0x380e, 0x09},	
	{0x380f, 0xce},    
	{0x3810, 0x00},
	{0x3811, 0x04},
	{0x3812, 0x00},
	{0x3813, 0x04},
	{0x3814, 0x11},
	{0x3815, 0x11},
	{0x3820, 0x10},
	{0x3821, 0x0e},

	{0x3a04, 0x07},
	{0x3a05, 0x49},
	{0x3708, 0xe3},
	{0x3709, 0x43},
	{0x4512, 0x01},

	
	{0x3016, 0xf9},
	{0x3016, 0xf0},
	{0x3208, 0x11},
	{0x3208, 0xa1},
};

 
 static struct msm_camera_i2c_reg_conf ov8838_night_settings[] = {
	
	{0x3208, 0x00},
	
	{0x3800, 0x00},
	{0x3801, 0x04},
	{0x3802, 0x00},
	{0x3803, 0x04},
	{0x3804, 0x0c},
	{0x3805, 0xdb},
	{0x3806, 0x09},
	{0x3807, 0xab},
	{0x3808, 0x0c},
	{0x3809, 0xd0},
	{0x380a, 0x09},
	{0x380b, 0xa0},
	{0x380c, 0x0e},
	{0x380d, 0x28},
	{0x380e, 0x09},
	{0x380f, 0xe6},	
	{0x3810, 0x00},
	{0x3811, 0x04},
	{0x3812, 0x00},
	{0x3813, 0x04},
	{0x3814, 0x11},
	{0x3815, 0x11},

	{0x3a04, 0x09},
	{0x3a05, 0xa9},

	{0x3016, 0xf9},
	{0x3016, 0xf0},
	{0x3208, 0x10},
	

	{0x3208, 0xa0},
};

static struct v4l2_subdev_info ov8838_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	
};

static struct msm_camera_i2c_conf_array ov8838_init_conf[] = {
	{&ov8838_recommend_settings[0],
	ARRAY_SIZE(ov8838_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_mipi_settings[0],
	ARRAY_SIZE(ov8838_mipi_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_pll_settings[0],
	ARRAY_SIZE(ov8838_pll_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_conf_array ov8838_confs[] = {
	{&ov8838_snap_settings[0],
	ARRAY_SIZE(ov8838_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_prev_settings[0],
	ARRAY_SIZE(ov8838_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_video_settings[0],
	ARRAY_SIZE(ov8838_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_fast_video_settings[0],
	ARRAY_SIZE(ov8838_fast_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_night_settings[0],
	ARRAY_SIZE(ov8838_night_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8838_snap_wide_settings[0],
	ARRAY_SIZE(ov8838_snap_wide_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t ov8838_dimensions[] = {
	{
		.x_output = 0xCD0,	
		.y_output = 0x9A0, 
		.line_length_pclk = 0xe28, 
		.frame_length_lines = 0x9ce, 
		.vt_pixel_clk = 272000000, 
		.op_pixel_clk = 259200000, 
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0,
		.x_addr_end = 0xCCF, 
		.y_addr_end = 0x99F, 
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
		.binning_rawchip = 0x11,
	},
	{
		.x_output = 0x668, 
		.y_output = 0x4D0, 
		.line_length_pclk = 0xe28, 
		.frame_length_lines = 0x9d0,  
		.vt_pixel_clk = 259200000, 
		.op_pixel_clk = 259200000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0,
		.x_addr_end = 0xCCF, 
		.y_addr_end = 0x99F, 
		.x_even_inc = 1,
		.x_odd_inc = 3,
		.y_even_inc = 1,
		.y_odd_inc = 3,
		.binning_rawchip = 0x22,
	},
	{
		.x_output = 0xC0C, 
		.y_output = 0x6C8, 
		.line_length_pclk = 0xe28, 
		.frame_length_lines = 0x6FA,  
		.vt_pixel_clk = 259200000, 
		.op_pixel_clk = 259200000,
		.binning_factor = 1,
		.x_addr_start = 0x62, 
		.y_addr_start = 0x16C, 
		.x_addr_end = 0xC6D, 
		.y_addr_end = 0x833, 
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
		.binning_rawchip = 0x11,
	},
	{
		.x_output = 0x668, 
		.y_output = 0x1FE, 
		.line_length_pclk = 0xe28, 
		.frame_length_lines = 0x253,  
		.vt_pixel_clk = 259200000, 
		.op_pixel_clk = 259200000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0xD4, 
		.x_addr_end = 0xCCF, 
		.y_addr_end = 0x8CB, 
		.x_even_inc = 1,
		.x_odd_inc = 3,
		.y_even_inc = 1,
		.y_odd_inc = 7,
		.binning_rawchip = 0x22,
	},
	{ 
		.x_output = 0xCD0,
		.y_output = 0x9A0,
		.line_length_pclk = 0xe28,
		.frame_length_lines = 0x9e6,  
		.vt_pixel_clk = 259200000, 
		.op_pixel_clk = 259200000,
		.binning_factor = 1,
		.x_addr_start = 0x4,
		.y_addr_start = 0x4,
		.x_addr_end = 0xCDB,
		.y_addr_end = 0x9AB,
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
		.binning_rawchip = 0x11,
	},
	{
		.x_output = 0xCD0,	
		.y_output = 0x740,	
		.line_length_pclk = 0xe28,	
		.frame_length_lines = 0x9d0,	
		.vt_pixel_clk = 259200000, 
		.op_pixel_clk = 259200000,
		.binning_factor = 1,
		.x_addr_start = 0,
		.y_addr_start = 0x130,	
		.x_addr_end = 0xCCF,	
		.y_addr_end = 0x86F,	
		.x_even_inc = 1,
		.x_odd_inc = 1,
		.y_even_inc = 1,
		.y_odd_inc = 1,
		.binning_rawchip = 0x11,
	},
};

static struct msm_camera_csid_vc_cfg ov8838_cid_cfg[] = {
	{0, CSI_RAW10, CSI_DECODE_10BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params ov8838_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 4,
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = ov8838_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 4,
		
		.settle_cnt = 0x15,
	},
};

static struct msm_camera_csi2_params *ov8838_csi_params_array[] = {
	&ov8838_csi_params,
	&ov8838_csi_params,
	&ov8838_csi_params,
	&ov8838_csi_params,
	&ov8838_csi_params,
	&ov8838_csi_params
};

static struct msm_sensor_output_reg_addr_t ov8838_reg_addr = {
	.x_output = 0x3808,
	.y_output = 0x380a,
	.line_length_pclk = 0x380c,
	.frame_length_lines = 0x380e,
};

static struct msm_sensor_id_info_t ov8838_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x8830,
};

static struct msm_sensor_exp_gain_info_t ov8838_exp_gain_info = {
	.coarse_int_time_addr = 0x3500, 
	.global_gain_addr = 0x350B, 
	.vert_offset = 12,	
	.min_vert = 4,  
	.sensor_max_linecount = 65519,  
};

#if 0
int32_t ov8838_set_dig_gain(struct msm_sensor_ctrl_t *s_ctrl, uint16_t dig_gain)
{
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		REG_DIGITAL_GAIN_GREEN_R, dig_gain,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		REG_DIGITAL_GAIN_RED, dig_gain,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		REG_DIGITAL_GAIN_BLUE, dig_gain,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		REG_DIGITAL_GAIN_GREEN_B, dig_gain,
		MSM_CAMERA_I2C_WORD_DATA);

	return 0;
}
#endif

#if 0
static uint32_t vcm_clib;
static uint16_t vcm_clib_min,vcm_clib_med,vcm_clib_max;

static int ov8838_read_vcm_clib(struct sensor_cfg_data *cdata, struct msm_sensor_ctrl_t *s_ctrl)
{
	uint8_t rc=0;
	unsigned short info_value = 0;

	struct msm_camera_i2c_client *ov8838_msm_camera_i2c_client = s_ctrl->sensor_i2c_client;

	vcm_clib =0;
	vcm_clib_min = 0;
	vcm_clib_med = 0;
	vcm_clib_max = 0;

	pr_info("%s: sensor OTP information:\n", __func__);

	
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x3A1C, 0x00);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3A1C fail\n", __func__);

	
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x04);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x0A00 (Start) fail\n", __func__);

	mdelay(4);

	vcm_clib =0;
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A02, 5);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x0A02 (select page %d) fail\n", __func__, 5);
			
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x01);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x0A00: Set read mode fail\n", __func__);

	rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A04), &info_value);
	if (rc < 0)
		pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A04));
	else
		{
		pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
		vcm_clib = (vcm_clib << 8) | info_value;
		}
	rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A05), &info_value);
	if (rc < 0)
		pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A05));
	else
		{
		pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
		vcm_clib = (vcm_clib << 8) | info_value;
		}

	
	if(vcm_clib >> 8 == 0x03)
		{
		  uint32_t p;

		  rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A0C), &info_value);
		  if (rc < 0)
			  pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A0C));
		  else
			  {
			  pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
			  vcm_clib = (vcm_clib << 8) | info_value;
			  }
		  rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A0D), &info_value);
		  if (rc < 0)
			  pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A0D));
		  else
			  {
			  pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
			  vcm_clib = (vcm_clib << 8) | info_value;
			  }

		  p=((vcm_clib & 0x0000FFFF) ) >> 3 ;
		  vcm_clib_min= p - 20;
		  vcm_clib_max= p + 26;
		  vcm_clib_med= (vcm_clib_max + vcm_clib_min)/2 -26/4;
		  pr_info("%s: VCM clib=0x%x, [Sharp] (p=%d) min/med/max=%d %d %d\n"
			  , __func__, vcm_clib, p, vcm_clib_min, vcm_clib_med, vcm_clib_max);
		}
	else
		{
			vcm_clib =0;
			rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A02, 16);
			if (rc < 0)
				pr_err("%s: i2c_write_b 0x0A02 (select page %d) fail\n", __func__, 16);
			
			rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x01);
			if (rc < 0)
				pr_err("%s: i2c_write_b 0x0A00: Set read mode fail\n", __func__);

			rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A04), &info_value);
			if (rc < 0)
				pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A04));
			else
			{
				pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
				vcm_clib = (vcm_clib << 8) | info_value;
			}
			rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A05), &info_value);
			if (rc < 0)
				pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A05));
			else
			{
				pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
				vcm_clib = (vcm_clib << 8) | info_value;
			}

			if(vcm_clib >> 8 == 0x04)
			{
				uint32_t p;

				rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A0E), &info_value);
				if (rc < 0)
				pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A0E));
				else
				{
				pr_info("%s: i2c_read_b 0x%x\n", __func__, info_value);
				vcm_clib = (vcm_clib << 16) | info_value;
				}

				p=((vcm_clib & 0x000000FF) + 0x00000080) >> 3 ;
				vcm_clib_min= p - 20;
				vcm_clib_max= p + 26;
				vcm_clib_med= (vcm_clib_max + vcm_clib_min)/2 -26/4;
				pr_info("%s: VCM clib=0x%x, [Lite-On] (p=%d) min/med/max=%d %d %d\n"
				, __func__, vcm_clib, p, vcm_clib_min, vcm_clib_med, vcm_clib_max);
			}
		}
	if(((vcm_clib & 0x0000FFFF) == 0x0000) || (vcm_clib_min==0 && vcm_clib_med==0 && vcm_clib_max==0)
		||(
		     (DEFAULT_VCM_MAX < vcm_clib_max) || (DEFAULT_VCM_MAX < vcm_clib_med) || (DEFAULT_VCM_MAX < vcm_clib_min)
		  || (DEFAULT_VCM_MIN > vcm_clib_max) || (DEFAULT_VCM_MIN > vcm_clib_med) || (DEFAULT_VCM_MIN > vcm_clib_min)
		  || ((vcm_clib_med < vcm_clib_min) || (vcm_clib_med > vcm_clib_max))
		))
		{
		  vcm_clib_min=DEFAULT_VCM_MIN;
		  vcm_clib_med=DEFAULT_VCM_MED;
		  vcm_clib_max=DEFAULT_VCM_MAX;
		}

    cdata->cfg.calib_vcm_pos.min=vcm_clib_min;
    cdata->cfg.calib_vcm_pos.med=vcm_clib_med;
    cdata->cfg.calib_vcm_pos.max=vcm_clib_max;

	pr_info("%s: VCM clib=0x%x, min/med/max=%d %d %d\n"
		, __func__, vcm_clib, cdata->cfg.calib_vcm_pos.min, cdata->cfg.calib_vcm_pos.med, cdata->cfg.calib_vcm_pos.max);

	return 0;

}
#endif

static int lens_info;	

static void ov8838_read_lens_info(struct msm_sensor_ctrl_t *s_ctrl)
{
	lens_info = 6;	

#if 0
	int32_t  rc;
	int page = 0;
	unsigned short info_value = 0, info_index = 0;
	unsigned short  OTP[10] = {0};
	struct msm_camera_i2c_client *ov8838_msm_camera_i2c_client = s_ctrl->sensor_i2c_client;

	lens_info = 6;	

	pr_info("%s\n", __func__);
	pr_info("%s: sensor OTP information:\n", __func__);

	
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x3A1C, 0x00);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3A1C fail\n", __func__);

	
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x04);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x0A00 (Start) fail\n", __func__);

	mdelay(4);

	
	info_index = 1;
	info_value = 0;
	memset(OTP, 0, sizeof(OTP));

	for (page = 20; page >= 16; page--) {
		rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A02, page);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x0A02 (select page %d) fail\n", __func__, page);

		
		rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x01);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x0A00: Set read mode fail\n", __func__);

		
		rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A04 + info_index), &info_value);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A04 + info_index));

		 
		if (((info_value&0x0F) != 0) || page == 0)
			break;
	}
	OTP[info_index] = (short)(info_value&0x0F);

	if (OTP[1] != 0) {
		pr_info("Get Fuseid from Page20 to Page16\n");
		goto get_done;
	}

	
	info_index = 1;
	info_value = 0;
	memset(OTP, 0, sizeof(OTP));

	for (page = 4; page >= 0; page--) {
		rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A02, page);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x0A02 (select page %d) fail\n", __func__, page);

		
		rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x01);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x0A00: Set read mode fail\n", __func__);

		
		rc = msm_camera_i2c_read_b(ov8838_msm_camera_i2c_client, (0x0A04 + info_index), &info_value);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x0A04 + info_index));

		 
		if (((info_value & 0x0F) != 0) || page == 0)
			break;
	}
	OTP[info_index] = (short)(info_value&0x0F);

get_done:
	
	rc = msm_camera_i2c_write_b(ov8838_msm_camera_i2c_client, 0x0A00, 0x00);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x0A00 (Stop) fail\n", __func__);

	pr_info("%s: LensID=%x\n", __func__, OTP[1]);

	if (OTP[1] == 5)	
		lens_info = OTP[1];
#endif

	return;
}


static int ov8838_sensor_open_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	uint16_t value = 0;

	if (data->sensor_platform_info)
		ov8838_s_ctrl.mirror_flip = data->sensor_platform_info->mirror_flip;

	
	if (ov8838_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR_FLIP)
		value = OV8838_READ_MIRROR_FLIP;
	else if (ov8838_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR)
		value = OV8838_READ_MIRROR;
	else if (ov8838_s_ctrl.mirror_flip == CAMERA_SENSOR_FLIP)
		value = OV8838_READ_FLIP;
	else
		value = OV8838_READ_NORMAL_MODE;
	msm_camera_i2c_write(ov8838_s_ctrl.sensor_i2c_client,
		OV8838_REG_READ_MODE, value, MSM_CAMERA_I2C_BYTE_DATA);

	ov8838_read_lens_info(&ov8838_s_ctrl);

	return rc;
}

static const char *ov8838Vendor = "sony";
static const char *ov8838NAME = "ov8838";
static const char *ov8838Size = "8M";

static ssize_t sensor_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	sprintf(buf, "%s %s %s\n", ov8838Vendor, ov8838NAME, ov8838Size);
	ret = strlen(buf) + 1;

	return ret;
}

static DEVICE_ATTR(sensor, 0444, sensor_vendor_show, NULL);

static struct kobject *android_ov8838;

static int ov8838_sysfs_init(void)
{
	int ret ;
	pr_info("ov8838:kobject creat and add\n");
	android_ov8838 = kobject_create_and_add("android_camera", NULL);
	if (android_ov8838 == NULL) {
		pr_info("ov8838_sysfs_init: subsystem_register " \
		"failed\n");
		ret = -ENOMEM;
		return ret ;
	}
	pr_info("ov8838:sysfs_create_file\n");
	ret = sysfs_create_file(android_ov8838, &dev_attr_sensor.attr);
	if (ret) {
		pr_info("ov8838_sysfs_init: sysfs_create_file " \
		"failed\n");
		kobject_del(android_ov8838);
	}

	return 0 ;
}

int32_t ov8838_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int	rc = 0;
	pr_info("%s\n", __func__);
	rc = msm_sensor_i2c_probe(client, id);
	if(rc >= 0)
		ov8838_sysfs_init();
	pr_info("%s: rc(%d)\n", __func__, rc);
	return rc;
}

static const struct i2c_device_id ov8838_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov8838_s_ctrl},
	{ }
};

static struct i2c_driver ov8838_i2c_driver = {
	.id_table = ov8838_i2c_id,
	.probe  = ov8838_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov8838_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

int32_t ov8838_power_up(struct msm_sensor_ctrl_t *s_ctrl)
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
	ov8838_sensor_open_init(sdata);
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

int32_t ov8838_power_down(struct msm_sensor_ctrl_t *s_ctrl)
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
	pr_info("ov8838 %s\n", __func__);
	return i2c_add_driver(&ov8838_i2c_driver);
}

static struct v4l2_subdev_core_ops ov8838_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ov8838_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov8838_subdev_ops = {
	.core = &ov8838_subdev_core_ops,
	.video  = &ov8838_subdev_video_ops,
};

static int ov8838_read_fuseid(struct sensor_cfg_data *cdata,
	struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	int i;
	uint16_t read_data = 0;
	static uint8_t OTP[10] = {0,0,0,0,0,0,0,0,0,0};
	static int first=true;

	if (first) {
		first = false;
		
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3400, 0x01, MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			pr_err("%s: msm_camera_i2c_write 0x3400 failed\n", __func__);

		
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3402, 0x01, MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			pr_err("%s: msm_camera_i2c_write 0x3402 failed\n", __func__);

		for (i = 0; i < 10; i++) {
			rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client, (0x3404 + i), &read_data, MSM_CAMERA_I2C_BYTE_DATA);
			if (rc < 0)
				pr_err("%s: msm_camera_i2c_read 0x%x failed\n", __func__, (0x3404 + i));

			OTP[i] = (uint8_t)(read_data&0x00FF);
			read_data = 0;
		}
	}

	pr_info("%s: VenderID=%x,LensID=%x,SensorID=%02x%02x\n", __func__,
		OTP[0], OTP[1], OTP[2], OTP[3]);
	pr_info("%s: ModuleFuseID= %02x%02x%02x%02x%02x%02x\n", __func__,
		OTP[4], OTP[5], OTP[6], OTP[7], OTP[8], OTP[9]);

	cdata->cfg.fuse.fuse_id_word1 = 0;
	cdata->cfg.fuse.fuse_id_word2 = (OTP[0]);
	cdata->cfg.fuse.fuse_id_word3 =
		(OTP[4]<<24) |
		(OTP[5]<<16) |
		(OTP[6]<<8) |
		(OTP[7]);
	cdata->cfg.fuse.fuse_id_word4 =
		(OTP[8]<<8) |
		(OTP[9]);

	pr_info("ov8838: fuse->fuse_id_word1:%d\n",
		cdata->cfg.fuse.fuse_id_word1);
	pr_info("ov8838: fuse->fuse_id_word2:%d\n",
		cdata->cfg.fuse.fuse_id_word2);
	pr_info("ov8838: fuse->fuse_id_word3:0x%08x\n",
		cdata->cfg.fuse.fuse_id_word3);
	pr_info("ov8838: fuse->fuse_id_word4:0x%08x\n",
		cdata->cfg.fuse.fuse_id_word4);
	return 0;

}

static int ov8838_read_VCM_driver_IC_info(	struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t  rc;
	int page = 0;
	unsigned short info_value = 0, info_index = 0;
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

	
	rc = msm_camera_i2c_write_b(msm_camera_i2c_client, 0x0100, 0x00);	
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x0100 fail\n", __func__);

	
	rc = msm_camera_i2c_write_b(msm_camera_i2c_client, 0x3368, 0x18);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3368 fail\n", __func__);

	rc = msm_camera_i2c_write_b(msm_camera_i2c_client, 0x3369, 0x00);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3369 fail\n", __func__);

	
	rc = msm_camera_i2c_write_b(msm_camera_i2c_client, 0x3400, 0x01);
	if (rc < 0)
		pr_err("%s: i2c_write_b 0x3400 fail\n", __func__);

	mdelay(4);

	
	info_index = 10;
	
	for (page = 3; page >= 0; page--) {
		
		rc = msm_camera_i2c_write_b(msm_camera_i2c_client, 0x3402, page);
		if (rc < 0)
			pr_err("%s: i2c_write_b 0x3402 (select page %d) fail\n", __func__, page);

		
		rc = msm_camera_i2c_read_b(msm_camera_i2c_client, (0x3404 + info_index), &info_value);
		if (rc < 0)
			pr_err("%s: i2c_read_b 0x%x fail\n", __func__, (0x3404 + info_index));

		
		if (((info_value & 0x0F) != 0) || page < 0)
			break;
	}
	OTP = (short)(info_value&0x0F);

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

static struct msm_sensor_fn_t ov8838_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain_ex = msm_sensor_write_exp_gain_ov, 
	.sensor_write_snapshot_exp_gain_ex = msm_sensor_write_exp_gain_ov, 

	.sensor_write_snapshot_exp_gain = msm_sensor_write_exp_gain1,
	
	.sensor_setting = msm_sensor_setting_parallel_ov,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = ov8838_power_up,
	.sensor_power_down = ov8838_power_down,
	.sensor_i2c_read_fuseid = ov8838_read_fuseid,
	.sensor_i2c_read_vcm_driver_ic = ov8838_read_VCM_driver_IC_info,
#if 0
	
	.sensor_i2c_read_vcm_clib = ov8838_read_vcm_clib,
	
#endif
};

static struct msm_sensor_reg_t ov8838_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov8838_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov8838_start_settings),
	.stop_stream_conf = ov8838_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ov8838_stop_settings),
	.group_hold_on_conf = ov8838_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(ov8838_groupon_settings),
	.group_hold_off_conf = ov8838_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(ov8838_groupoff_settings),
	.init_settings = &ov8838_init_conf[0],
	.init_size = ARRAY_SIZE(ov8838_init_conf),
	.mode_settings = &ov8838_confs[0],
	.output_settings = &ov8838_dimensions[0],
	.num_conf = ARRAY_SIZE(ov8838_confs),
};

static struct msm_sensor_ctrl_t ov8838_s_ctrl = {
	.msm_sensor_reg = &ov8838_regs,
	.sensor_i2c_client = &ov8838_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &ov8838_reg_addr,
	.sensor_id_info = &ov8838_id_info,
	.sensor_exp_gain_info = &ov8838_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csi_params = &ov8838_csi_params_array[0],
	.msm_sensor_mutex = &ov8838_mut,
	.sensor_i2c_driver = &ov8838_i2c_driver,
	.sensor_v4l2_subdev_info = ov8838_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov8838_subdev_info),
	.sensor_v4l2_subdev_ops = &ov8838_subdev_ops,
	.func_tbl = &ov8838_func_tbl,
	.sensor_first_mutex = &ov8838_sensor_init_mut, 
	
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sony 8 MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");


