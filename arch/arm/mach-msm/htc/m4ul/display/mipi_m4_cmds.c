static char enter_sleep[2] = {0x10, 0x00}; 
static char exit_sleep[2] = {0x11, 0x00}; 
static char display_off[2] = {0x28, 0x00}; 
static char display_on[2] = {0x29, 0x00}; 

static char led_pwm1[] = {0x51, 0xff}; 
static char led_pwm2[] = {0x53, 0x24}; 
static char pwm_off[]  = {0x51, 0x00};

static char himax_b9[] = {0xB9, 0xFF, 0x83, 0x92}; 
static char himax_d4[] = {0xD4, 0x00}; 
static char himax_ba[] = {0xBA, 0x12, 0x83, 0x00, 0xD6, 0xC6, 0x00, 0x0A}; 
static char himax_c0[] = {0xC0, 0x01, 0x94}; 
static char himax_c6[] = {0xC6, 0x35, 0x00, 0x00, 0x04}; 
static char himax_d5[] = {0xD5, 0x00, 0x00, 0x02}; 
static char himax_bf[] = {0xBF, 0x05, 0x60, 0x02}; 
static char himax_35[] = {0x35, 0x00}; 
static char himax_c2[] = {0xC2, 0x08}; 
static char himax_55[] = {0x55, 0x03};
static char cabc_UI[] = {
	0xCA, 0x2D, 0x27, 0x26,
	0x25, 0x25, 0x25, 0x21,
	0x20, 0x20};
#ifdef CONFIG_MSM_CABC_VIDEO_ENHANCE
static char cabc_moving[] = {
	0xCA, 0x40, 0x3C, 0x38,
	0x34, 0x33, 0x32, 0x2D,
	0x24, 0x20};
#endif
static char himax_e3[] = {0xE3, 0x01};
static char himax_e5[] = {
	0xE5, 0x00, 0x04, 0x0B,
	0x05, 0x05, 0x00, 0x80,
	0x20, 0x80, 0x10, 0x00,
	0x07, 0x07, 0x07, 0x07,
	0x07, 0x80, 0x0A};
static char himax_c9[] = {0xC9, 0x1F, 0x00, 0x1E, 0x3F, 0x00, 0x80}; 


static char cmd2_page0_0[] = {0xFF, 0x01};
static char cmd2_page0_1[] = {0xFB, 0x01};
static char cmd2_page0_data00[] = {0x00, 0x4A};
static char cmd2_page0_data01[] = {0x01, 0x33};
static char cmd2_page0_data02[] = {0x02, 0x53};
static char cmd2_page0_data03[] = {0x03, 0x55};
static char cmd2_page0_data04[] = {0x04, 0x55};
static char cmd2_page0_data05[] = {0x05, 0x33};
static char cmd2_page0_data06[] = {0x06, 0x22};
static char cmd2_page0_data07[] = {0x08, 0x56};
static char cmd2_page0_data08[] = {0x09, 0x8F};
static char cmd2_page0_data09[] = {0x0B, 0x97};
static char cmd2_page0_data10[] = {0x0C, 0x97};
static char cmd2_page0_data11[] = {0x0D, 0x2F};
static char cmd2_page0_data12[] = {0x0E, 0x24};
static char cmd2_page0_data15[] = {0x36, 0x73};
static char cmd2_page0_data16[] = {0x0F, 0x04};

static char cmd3_0[] = {0xFF, 0xEE};
static char cmd3_1[] = {0xFB, 0x01};
static char cmd3_data00[] = {0x04, 0xAD};
static char cmd3_data01[] = {0xFF, 0x00};

static char cmd2_page4_0[] = {0xFF, 0x05};
static char cmd2_page4_1[] = {0xFB, 0x01};
static char cmd2_page4_data00[] = {0x01, 0x00};
static char cmd2_page4_data01[] = {0x02, 0x82};
static char cmd2_page4_data02[] = {0x03, 0x82};
static char cmd2_page4_data03[] = {0x04, 0x82};
static char cmd2_page4_data04[] = {0x05, 0x30};
static char cmd2_page4_data05[] = {0x06, 0x33};
static char cmd2_page4_data06[] = {0x07, 0x01};
static char cmd2_page4_data07[] = {0x08, 0x00};
static char cmd2_page4_data08[] = {0x09, 0x46};
static char cmd2_page4_data09[] = {0x0A, 0x46};
static char cmd2_page4_data10[] = {0x0D, 0x0B};
static char cmd2_page4_data11[] = {0x0E, 0x1D};
static char cmd2_page4_data12[] = {0x0F, 0x08};
static char cmd2_page4_data13[] = {0x10, 0x53};
static char cmd2_page4_data14[] = {0x11, 0x00};
static char cmd2_page4_data15[] = {0x12, 0x00};
static char cmd2_page4_data16[] = {0x14, 0x01};
static char cmd2_page4_data17[] = {0x15, 0x00};
static char cmd2_page4_data18[] = {0x16, 0x05};
static char cmd2_page4_data19[] = {0x17, 0x00};
static char cmd2_page4_data20[] = {0x19, 0x7F};
static char cmd2_page4_data21[] = {0x1A, 0xFF};
static char cmd2_page4_data22[] = {0x1B, 0x0F};
static char cmd2_page4_data23[] = {0x1C, 0x00};
static char cmd2_page4_data24[] = {0x1D, 0x00};
static char cmd2_page4_data25[] = {0x1E, 0x00};
static char cmd2_page4_data26[] = {0x1F, 0x07};
static char cmd2_page4_data27[] = {0x20, 0x00};
static char cmd2_page4_data28[] = {0x21, 0x00};
static char cmd2_page4_data29[] = {0x22, 0x55};
static char cmd2_page4_data30[] = {0x23, 0x4D};
static char cmd2_page4_data31[] = {0x6C, 0x00};
static char cmd2_page4_data32[] = {0x6D, 0x00};
static char cmd2_page4_data33[] = {0x2D, 0x02};
static char cmd2_page4_data34[] = {0x83, 0x02};
static char cmd2_page4_data35[] = {0x9E, 0x58};
static char cmd2_page4_data36[] = {0x9F, 0x6A};
static char cmd2_page4_data37[] = {0xA0, 0x41};
static char cmd2_page4_data38[] = {0xA2, 0x10};
static char cmd2_page4_data39[] = {0xBB, 0x0A};
static char cmd2_page4_data40[] = {0xBC, 0x0A};
static char cmd2_page4_data41[] = {0x28, 0x01};
static char cmd2_page4_data42[] = {0x2F, 0x02};
static char cmd2_page4_data43[] = {0x32, 0x08};
static char cmd2_page4_data44[] = {0x33, 0xB8};
static char cmd2_page4_data45[] = {0x36, 0x02};
static char cmd2_page4_data46[] = {0x37, 0x00};
static char cmd2_page4_data47[] = {0x43, 0x00};
static char cmd2_page4_data48[] = {0x4B, 0x21};
static char cmd2_page4_data49[] = {0x4C, 0x03};
static char cmd2_page4_data50[] = {0x50, 0x21};
static char cmd2_page4_data51[] = {0x51, 0x03};
static char cmd2_page4_data52[] = {0x58, 0x21};
static char cmd2_page4_data53[] = {0x59, 0x03};
static char cmd2_page4_data54[] = {0x5D, 0x21};
static char cmd2_page4_data55[] = {0x5E, 0x03};

static char cmd2_page2_0[] = {0xFF, 0x03};
static char cmd2_page2_1[] = {0xFE, 0x08};
static char cmd2_page2_data00[] = {0x25, 0x26};
static char cmd2_page2_data01[] = {0x00, 0x00};
static char cmd2_page2_data02[] = {0x01, 0x05};
static char cmd2_page2_data03[] = {0x02, 0x10};
static char cmd2_page2_data04[] = {0x03, 0x14};
static char cmd2_page2_data05[] = {0x04, 0x16};
static char cmd2_page2_data06[] = {0x05, 0x18};
static char cmd2_page2_data07[] = {0x06, 0x20};
static char cmd2_page2_data08[] = {0x07, 0x20};
static char cmd2_page2_data09[] = {0x08, 0x18};
static char cmd2_page2_data10[] = {0x09, 0x16};
static char cmd2_page2_data11[] = {0x0A, 0x14};
static char cmd2_page2_data12[] = {0x0B, 0x12};
static char cmd2_page2_data13[] = {0x0C, 0x06};
static char cmd2_page2_data14[] = {0x0D, 0x02};
static char cmd2_page2_data15[] = {0x0E, 0x01};
static char cmd2_page2_data16[] = {0x0F, 0x00};
static char cmd2_page2_data17[] = {0xFB, 0x01};
static char cmd2_page2_data18[] = {0xFF, 0x00};
static char cmd2_page2_data19[] = {0xFE, 0x01};

static char cmd2_page3_0[] = {0xFF, 0x04};
static char cmd2_page3_1[] = {0xFB, 0x01};
static char cmd2_page3_data00[] = {0x0A, 0x03};
static char cmd2_page3_data01[] = {0x05, 0x2D};
static char cmd2_page3_data02[] = {0x21, 0xFF};
static char cmd2_page3_data03[] = {0x22, 0xF7};
static char cmd2_page3_data04[] = {0x23, 0xEF};
static char cmd2_page3_data05[] = {0x24, 0xE7};
static char cmd2_page3_data06[] = {0x25, 0xDF};
static char cmd2_page3_data07[] = {0x26, 0xD7};
static char cmd2_page3_data08[] = {0x27, 0xCF};
static char cmd2_page3_data09[] = {0x28, 0xC7};
static char cmd2_page3_data10[] = {0x29, 0xBF};
static char cmd2_page3_data11[] = {0x2A, 0xB7};

static char cmd1_0[] = {0xFF, 0x00};
static char cmd1_1[] = {0xFB, 0x01};
static char pwm_duty[] = {0x51, 0x07}; 
static char bl_ctl[4] = {0x53, 0x2C}; 
static char power_save[4] = {0x55, 0x83}; 
static char power_save2[4] = {0x5E, 0x06};
static char set_display_mode[4] = {0xC2, 0x08}; 
static char set_mipi_lane[4] = {0xBA, 0x02}; 
static char set_te_on[4] = {0x35, 0x00};

static struct dsi_cmd_desc sharp_hx_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_b9), himax_b9},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,  sizeof(himax_d4), himax_d4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_ba), himax_ba},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_c0), himax_c0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_c6), himax_c6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_d5), himax_d5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_bf), himax_bf},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_c2), himax_c2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 5,  sizeof(himax_e3), himax_e3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_e5), himax_e5},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,  sizeof(himax_35), himax_35},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,  sizeof(led_pwm2), led_pwm2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 5,  sizeof(himax_55), himax_55},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(cabc_UI), cabc_UI},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(himax_c9), himax_c9},
};

static struct dsi_cmd_desc auo_nt_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_0), cmd2_page0_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_1), cmd2_page0_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data00), cmd2_page0_data00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data01), cmd2_page0_data01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data02), cmd2_page0_data02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data03), cmd2_page0_data03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data04), cmd2_page0_data04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data05), cmd2_page0_data05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data06), cmd2_page0_data06},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data07), cmd2_page0_data07},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data08), cmd2_page0_data08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data09), cmd2_page0_data09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data10), cmd2_page0_data10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data11), cmd2_page0_data11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data12), cmd2_page0_data12},
	
	
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data15), cmd2_page0_data15},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page0_data16), cmd2_page0_data16},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd3_0), cmd3_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd3_1), cmd3_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd3_data00), cmd3_data00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd3_data01), cmd3_data01},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_0), cmd2_page4_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_1), cmd2_page4_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data00), cmd2_page4_data00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data01), cmd2_page4_data01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data02), cmd2_page4_data02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data03), cmd2_page4_data03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data04), cmd2_page4_data04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data05), cmd2_page4_data05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data06), cmd2_page4_data06},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data07), cmd2_page4_data07},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data08), cmd2_page4_data08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data09), cmd2_page4_data09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data10), cmd2_page4_data10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data11), cmd2_page4_data11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data12), cmd2_page4_data12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data13), cmd2_page4_data13},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data14), cmd2_page4_data14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data15), cmd2_page4_data15},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data16), cmd2_page4_data16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data17), cmd2_page4_data17},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data18), cmd2_page4_data18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data19), cmd2_page4_data19},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data20), cmd2_page4_data20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data21), cmd2_page4_data21},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data22), cmd2_page4_data22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data23), cmd2_page4_data23},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data24), cmd2_page4_data24},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data25), cmd2_page4_data25},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data26), cmd2_page4_data26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data27), cmd2_page4_data27},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data28), cmd2_page4_data28},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data29), cmd2_page4_data29},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data30), cmd2_page4_data30},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data31), cmd2_page4_data31},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data32), cmd2_page4_data32},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data33), cmd2_page4_data33},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data34), cmd2_page4_data34},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data35), cmd2_page4_data35},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data36), cmd2_page4_data36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data37), cmd2_page4_data37},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data38), cmd2_page4_data38},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data39), cmd2_page4_data39},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data40), cmd2_page4_data40},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data41), cmd2_page4_data41},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data42), cmd2_page4_data42},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data43), cmd2_page4_data43},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data44), cmd2_page4_data44},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data45), cmd2_page4_data45},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data46), cmd2_page4_data46},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data47), cmd2_page4_data47},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data48), cmd2_page4_data48},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data49), cmd2_page4_data49},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data50), cmd2_page4_data50},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data51), cmd2_page4_data51},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data52), cmd2_page4_data52},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data53), cmd2_page4_data53},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data54), cmd2_page4_data54},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page4_data55), cmd2_page4_data55},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_0), cmd2_page2_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_1), cmd2_page2_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data00), cmd2_page2_data00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data01), cmd2_page2_data01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data02), cmd2_page2_data02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data03), cmd2_page2_data03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data04), cmd2_page2_data04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data05), cmd2_page2_data05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data06), cmd2_page2_data06},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data07), cmd2_page2_data07},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data08), cmd2_page2_data08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data09), cmd2_page2_data09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data10), cmd2_page2_data10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data11), cmd2_page2_data11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data12), cmd2_page2_data12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data13), cmd2_page2_data13},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data14), cmd2_page2_data14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data15), cmd2_page2_data15},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data16), cmd2_page2_data16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data17), cmd2_page2_data17},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data18), cmd2_page2_data18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page2_data19), cmd2_page2_data19},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_0), cmd2_page3_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_1), cmd2_page3_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data00), cmd2_page3_data00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data01), cmd2_page3_data01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data02), cmd2_page3_data02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data03), cmd2_page3_data03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data04), cmd2_page3_data04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data05), cmd2_page3_data05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data06), cmd2_page3_data06},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data07), cmd2_page3_data07},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data08), cmd2_page3_data08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data09), cmd2_page3_data09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data10), cmd2_page3_data10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd2_page3_data11), cmd2_page3_data11},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd1_0), cmd1_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(cmd1_1), cmd1_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(pwm_duty), pwm_duty},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(bl_ctl), bl_ctl},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(power_save), power_save},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(power_save2), power_save2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(set_display_mode), set_display_mode},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 10, sizeof(set_mipi_lane), set_mipi_lane},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(set_te_on), set_te_on},
	{DTYPE_DCS_WRITE, 1, 0, 0, 100, sizeof(exit_sleep), exit_sleep},
};

static struct dsi_cmd_desc sharp_display_off_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(pwm_off), pwm_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 1,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 130,
		sizeof(enter_sleep), enter_sleep},
};

static struct dsi_cmd_desc sharp_display_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 40, sizeof(display_on), display_on},
};

static struct dsi_cmd_desc sharp_hx_cmd_backlight_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(led_pwm1), led_pwm1},
};

#ifdef CONFIG_MSM_CABC_VIDEO_ENHANCE
static struct dsi_cmd_desc cabc_UI_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(cabc_UI), cabc_UI},
};
static struct dsi_cmd_desc cabc_moving_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,  sizeof(cabc_moving), cabc_moving},
};
#endif

