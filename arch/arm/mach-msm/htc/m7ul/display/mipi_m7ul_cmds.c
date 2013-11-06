static char enter_sleep[2] = {0x10, 0x00}; 
static char exit_sleep[2] = {0x11, 0x00}; 
static char display_off[2] = {0x28, 0x00}; 
static char display_on[2] = {0x29, 0x00}; 
static char nop[2] = {0x00, 0x00};
static char CABC[2] = {0x55, 0x00};
static char jdi_samsung_CABC[2] = {0x55, 0x03};

static char samsung_password_l2[3] = { 0xF0, 0x5A, 0x5A}; 
static char samsung_MIE_ctrl1[4] = {0xC0, 0x40, 0x10, 0x80};
static char samsung_MIE_ctrl2[2] = {0xC2, 0x0F}; 

static char samsung_ctrl_source[17] = { 
		0xF2, 0x0C, 0x03, 0x73,
		0x06, 0x04, 0x00, 0x25,
		0x00, 0x26, 0x00, 0xD1,
		0x00, 0xD1, 0x00, 0x00,
		0x00};
static char samsung_ctrl_bl[4] = {0xC3, 0x63, 0x40, 0x01}; 
static char samsung_ctrl_positive_gamma[70] = { 
		0xFA, 0x00, 0x3F, 0x20,
		0x19, 0x25, 0x24, 0x27,
		0x19, 0x19, 0x13, 0x00,
		0x08, 0x0E, 0x0F, 0x14,
		0x15, 0x1F, 0x25, 0x2A,
		0x2B, 0x2A, 0x20, 0x3C,
		0x00, 0x3F, 0x20, 0x19,
		0x25, 0x24, 0x27, 0x19,
		0x19, 0x13, 0x00, 0x08,
		0x0E, 0x0F, 0x14, 0x15,
		0x1F, 0x25, 0x2A, 0x2B,
		0x2A, 0x20, 0x3C, 0x00,
		0x3F, 0x20, 0x19, 0x25,
		0x24, 0x27, 0x19, 0x19,
		0x13, 0x00, 0x08, 0x0E,
		0x0F, 0x14, 0x15, 0x1F,
		0x25, 0x2A, 0x2B, 0x2A,
		0x20, 0x3C};

static char samsung_ctrl_negative_gamma[70] = { 
		0xFB, 0x00, 0x3F, 0x20,
		0x19, 0x25, 0x24, 0x27,
		0x19, 0x19, 0x13, 0x00,
		0x08, 0x0E, 0x0F, 0x14,
		0x15, 0x1F, 0x25, 0x2A,
		0x2B, 0x2A, 0x20, 0x3C,
		0x00, 0x3F, 0x20, 0x19,
		0x25, 0x24, 0x27, 0x19,
		0x19, 0x13, 0x00, 0x08,
		0x0E, 0x0F, 0x14, 0x15,
		0x1F, 0x25, 0x2A, 0x2B,
		0x2A, 0x20, 0x3C, 0x00,
		0x3F, 0x20, 0x19, 0x25,
		0x24, 0x27, 0x19, 0x19,
		0x13, 0x00, 0x08, 0x0E,
		0x0F, 0x14, 0x15, 0x1F,
		0x25, 0x2A, 0x2B, 0x2A,
		0x20, 0x3C};
static char samsung_password_l3[3] = { 0xFC, 0x5A, 0x5A }; 

static char samsung_cmd_test[5] = { 0xFF, 0x00, 0x00, 0x00, 0x20}; 

static char samsung_panel_exit_sleep[2] = {0x11, 0x00}; 
static char samsung_bl_ctrl[2] = {0x53, 0x2C};
static char samsung_ctrl_brightness[2] = {0x51, 0xFF};
static char samsung_enable_te[2] = {0x35, 0x00};

static char samsung_set_column_address[5] = { 0x2A, 0x00, 0x00, 0x04, 0x37 }; 

static char samsung_set_page_address[5] = { 0x2B, 0x00, 0x00, 0x07, 0x7F }; 
static char samsung_panel_display_on[2] = {0x29, 0x00}; 
static char samsung_display_off[2] = {0x28, 0x00}; 
static char samsung_enter_sleep[2] = {0x10, 0x00}; 

static char samsung_deep_standby_off[2] = {0xB0, 0x01}; 
static char SAE[2] = {0xE9, 0x12};
static char samsung_swwr_mipi_speed[4] = {0xE4, 0x00, 0x04, 0x00};
static char samsung_swwr_kinky_gamma[17] = {0xF2, 0x0C, 0x03, 0x03, 0x06, 0x04, 0x00, 0x25, 0x00, 0x26, 0x00, 0xD1, 0x00, 0xD1, 0x00, 0x0A, 0x00};
static char samsung_password_l2_close[3] = { 0xF0, 0xA5, 0xA5}; 
static char samsung_password_l3_close[3] = { 0xFC, 0xA5, 0xA5}; 
static char  Oscillator_Bias_Current[4] = { 0xFD, 0x56, 0x08, 0x00}; 
static char samsung_ctrl_positive_gamma_c2_1[70] = { 
		0xFA, 0x1E, 0x38, 0x0C,
		0x0C, 0x12, 0x14, 0x16,
		0x17, 0x1A, 0x1A, 0x19,
		0x14, 0x10, 0x0D, 0x10,
		0x13, 0x1D, 0x20, 0x20,
		0x21, 0x26, 0x27, 0x36,
		0x0F, 0x3C, 0x12, 0x15,
		0x1E, 0x21, 0x24, 0x24,
		0x26, 0x24, 0x23, 0x1C,
		0x15, 0x11, 0x13, 0x16,
		0x1E, 0x21, 0x21, 0x21,
		0x26, 0x27, 0x36, 0x00,
		0x3F, 0x13, 0x18, 0x22,
		0x27, 0x29, 0x2A, 0x2B,
		0x2A, 0x29, 0x23, 0x1B,
		0x16, 0x18, 0x19, 0x1F,
		0x22, 0x23, 0x24, 0x2A,
		0x2D, 0x37};
static char samsung_ctrl_negative_gamma_c2_1[70] = { 
		0xFB, 0x1E, 0x38, 0x0C,
		0x0C, 0x12, 0x14, 0x16,
		0x17, 0x1A, 0x1A, 0x19,
		0x14, 0x10, 0x0D, 0x10,
		0x13, 0x1D, 0x20, 0x20,
		0x21, 0x26, 0x27, 0x36,
		0x0F, 0x3C, 0x12, 0x15,
		0x1E, 0x21, 0x24, 0x24,
		0x26, 0x24, 0x23, 0x1C,
		0x15, 0x11, 0x13, 0x16,
		0x1E, 0x21, 0x21, 0x21,
		0x26, 0x27, 0x36, 0x00,
		0x3F, 0x13, 0x18, 0x22,
		0x27, 0x29, 0x2A, 0x2B,
		0x2A, 0x29, 0x23, 0x1B,
		0x16, 0x18, 0x19, 0x1F,
		0x22, 0x23, 0x24, 0x2A,
		0x2D, 0x37};

static char BCSAVE[] = { 
		0xCD, 0x80, 0xB3, 0x67,
		0x1C, 0x78, 0x37, 0x00,
		0x10, 0x73, 0x41, 0x99,
		0x10, 0x00, 0x00};

static char TMF[] = { 
		0xCE, 0x33, 0x1C, 0x0D,
		0x20, 0x14, 0x00, 0x16,
		0x23, 0x18, 0x2C, 0x16,
		0x00, 0x00};

static struct dsi_cmd_desc samsung_cmd_backlight_cmds_nop[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(samsung_ctrl_brightness), samsung_ctrl_brightness},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
};

static struct dsi_cmd_desc samsung_cmd_backlight_cmds[] = {
        {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(samsung_ctrl_brightness), samsung_ctrl_brightness},
};

static struct dsi_cmd_desc samsung_display_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(samsung_panel_display_on), samsung_panel_display_on},
};

static struct dsi_cmd_desc samsung_jdi_panel_cmd_mode_cmds[] = {
#if 0
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2), samsung_password_l2},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_display_ctrl), samsung_display_ctrl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_display_ctrl_interface), samsung_display_ctrl_interface},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_ctrl_BRR), samsung_ctrl_BRR},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_crtl_Hsync), samsung_crtl_Hsync},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_GoutL), samsung_ctrl_GoutL},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_GoutR), samsung_ctrl_GoutR},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl1), samsung_MIE_ctrl1},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_ctrl_bl_mode),samsung_ctrl_bl_mode},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl2), samsung_MIE_ctrl2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_bl), samsung_ctrl_bl},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_ctrl_SHE), samsung_ctrl_SHE},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(samsung_ctrl_SAE), samsung_ctrl_SAE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_source), samsung_ctrl_source},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_power), samsung_ctrl_power},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_panel), samsung_ctrl_panel},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_positive_gamma), samsung_ctrl_positive_gamma},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_negative_gamma), samsung_ctrl_negative_gamma},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3), samsung_password_l3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_cmd_test), samsung_cmd_test},
#endif
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(samsung_panel_exit_sleep), samsung_panel_exit_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2), samsung_password_l2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl1), samsung_MIE_ctrl1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_bl), samsung_ctrl_bl},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 1, sizeof(SAE),SAE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_positive_gamma), samsung_ctrl_positive_gamma},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_negative_gamma), samsung_ctrl_negative_gamma},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3), samsung_password_l3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_cmd_test), samsung_cmd_test},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_swwr_mipi_speed), samsung_swwr_mipi_speed},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_swwr_kinky_gamma), samsung_swwr_kinky_gamma},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2_close), samsung_password_l2_close},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3_close), samsung_password_l3_close},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(samsung_set_column_address), samsung_set_column_address},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(samsung_set_page_address), samsung_set_page_address},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_bl_ctrl), samsung_bl_ctrl},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(CABC), CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_enable_te), samsung_enable_te},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
};

static struct dsi_cmd_desc samsung_jdi_panel_cmd_mode_cmds_c2_nvm[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3), samsung_password_l3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_cmd_test), samsung_cmd_test},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(samsung_panel_exit_sleep), samsung_panel_exit_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2), samsung_password_l2},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl1), samsung_MIE_ctrl1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_bl), samsung_ctrl_bl},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 1, sizeof(SAE), SAE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2_close), samsung_password_l2_close},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3_close), samsung_password_l3_close},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_bl_ctrl), samsung_bl_ctrl},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(CABC), CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_enable_te), samsung_enable_te},
	
};

static struct dsi_cmd_desc samsung_jdi_panel_cmd_mode_cmds_c2_1[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3), samsung_password_l3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_cmd_test), samsung_cmd_test},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Oscillator_Bias_Current), Oscillator_Bias_Current},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(samsung_panel_exit_sleep), samsung_panel_exit_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2), samsung_password_l2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl1), samsung_MIE_ctrl1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl2), samsung_MIE_ctrl2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_bl), samsung_ctrl_bl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(BCSAVE), BCSAVE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(TMF), TMF},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 1, sizeof(SAE), SAE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_source), samsung_ctrl_source},
        {DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_positive_gamma_c2_1), samsung_ctrl_positive_gamma_c2_1},
        {DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_negative_gamma_c2_1), samsung_ctrl_negative_gamma_c2_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2_close), samsung_password_l2_close},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3_close), samsung_password_l3_close},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_bl_ctrl), samsung_bl_ctrl},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(jdi_samsung_CABC), jdi_samsung_CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_enable_te), samsung_enable_te},
	
};
static struct dsi_cmd_desc samsung_jdi_panel_cmd_mode_cmds_c2_2[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3), samsung_password_l3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_cmd_test), samsung_cmd_test},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Oscillator_Bias_Current), Oscillator_Bias_Current},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(samsung_panel_exit_sleep), samsung_panel_exit_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2), samsung_password_l2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl1), samsung_MIE_ctrl1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_MIE_ctrl2), samsung_MIE_ctrl2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_bl), samsung_ctrl_bl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(BCSAVE), BCSAVE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(TMF), TMF},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 1, sizeof(SAE), SAE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_ctrl_source), samsung_ctrl_source},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l2_close), samsung_password_l2_close},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(samsung_password_l3_close), samsung_password_l3_close},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_bl_ctrl), samsung_bl_ctrl},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(jdi_samsung_CABC), jdi_samsung_CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(samsung_enable_te), samsung_enable_te},
	
};

static struct dsi_cmd_desc samsung_display_off_cmds[] = {
	{DTYPE_DCS_WRITE,  1, 0, 0, 0, sizeof(samsung_display_off), samsung_display_off},
	{DTYPE_DCS_WRITE,  1, 0, 0, 48, sizeof(samsung_enter_sleep), samsung_enter_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(samsung_password_l2), samsung_password_l2},
	{DTYPE_GEN_WRITE,  1, 0, 0, 0, sizeof(samsung_deep_standby_off), samsung_deep_standby_off},
};

static char write_display_brightness[3]= {0x51, 0x0F, 0xFF};
static char write_control_display[2] = {0x53, 0x24}; 
static struct dsi_cmd_desc renesas_cmd_backlight_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(write_display_brightness), write_display_brightness},
};
static struct dsi_cmd_desc renesas_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(display_on), display_on},
};
static char interface_setting_0[2] = {0xB0, 0x04};

static char Color_enhancement[33]= {
	0xCA, 0x01, 0x02, 0xA4,
	0xA4, 0xB8, 0xB4, 0xB0,
	0xA4, 0x3F, 0x28, 0x05,
	0xB9, 0x90, 0x70, 0x01,
	0xFF, 0x05, 0xF8, 0x0C,
	0x0C, 0x0C, 0x0C, 0x13,
	0x13, 0xF0, 0x20, 0x10,
	0x10, 0x10, 0x10, 0x10,
	0x10};
static char m7_Color_enhancement[33]= {
        0xCA, 0x01, 0x02, 0x9A,
        0xA4, 0xB8, 0xB4, 0xB0,
        0xA4, 0x08, 0x28, 0x05,
        0x87, 0xB0, 0x50, 0x01,
        0xFF, 0x05, 0xF8, 0x0C,
        0x0C, 0x50, 0x40, 0x13,
        0x13, 0xF0, 0x08, 0x10,
        0x10, 0x3F, 0x3F, 0x3F,
        0x3F};
static char Outline_Sharpening_Control[3]= {
	0xDD, 0x11, 0xA1};

static char BackLight_Control_6[8]= {
	0xCE, 0x00, 0x07, 0x00,
	0xC1, 0x24, 0xB2, 0x02};
static char BackLight_Control_6_28kHz[8]= {
       0xCE, 0x00, 0x01, 0x00,
       0xC1, 0xF4, 0xB2, 0x02}; 
static char Manufacture_Command_setting[4] = {0xD6, 0x01};
static char hsync_output[4] = {0xC3, 0x01, 0x00, 0x10};
static char protect_on[4] = {0xB0, 0x03};
static char TE_OUT[4] = {0x35, 0x00};
static char deep_standby_off[2] = {0xB1, 0x01};

static char unlock[] = {0xB0, 0x00};
static char display_brightness[] = {0x51, 0xFF};
static char led_pwm_en[] = {0x53, 0x0C};
static char enable_te[] = {0x35, 0x00};
static char Source_Timing_Setting[23]= {
	0xC4, 0x70, 0x0C, 0x0C,
	0x55, 0x55, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x05,
	0x00, 0x0C, 0x0C, 0x55,
	0x55, 0x00, 0x00, 0x00,
	0x00, 0x05, 0x05};
static char lock[] = {0xB0, 0x03};
static char Write_Content_Adaptive_Brightness_Control[2] = {0x55, 0x42};
static char common_setting[] = {
       0xCE, 0x6C, 0x40, 0x43,
       0x49, 0x55, 0x62, 0x71,
       0x82, 0x94, 0xA8, 0xB9,
       0xCB, 0xDB, 0xE9, 0xF5,
       0xFC, 0xFF, 0x04, 0xD3, 
       0x00, 0x00, 0x54, 0x24};

static char cabc_still[] = {0xB9, 0x03, 0x82, 0x3C, 0x10, 0x3C, 0x87};
static char cabc_movie[] = {0xBA, 0x03, 0x78, 0x64, 0x10, 0x64, 0xB4};
static char SRE_Manual_0[] = {0xBB, 0x01, 0x00, 0x00};

static char blue_shift_adjust_1[] = {
	0xC7, 0x01, 0x0B, 0x12,
	0x1B, 0x2A, 0x3A, 0x45,
	0x56, 0x3A, 0x42, 0x4E,
	0x5B, 0x64, 0x6C, 0x75,
	0x01, 0x0B, 0x12, 0x1A,
	0x29, 0x37, 0x41, 0x52,
	0x36, 0x3F, 0x4C, 0x59,
	0x62, 0x6A, 0x74};

static char blue_shift_adjust_2[] = {
	0xC8, 0x01, 0x00, 0xF4,
	0x00, 0x00, 0xFC, 0x00,
	0x00, 0xF7, 0x00, 0x00,
	0xFC, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0xFC, 0x0F};

static struct dsi_cmd_desc sharp_cmd_backlight_cmds[] = {
        {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(display_brightness), display_brightness},
};

static struct dsi_cmd_desc sharp_renesas_cmd_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(unlock), unlock},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Manufacture_Command_setting), Manufacture_Command_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(common_setting), common_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(cabc_still), cabc_still},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(cabc_movie), cabc_movie},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(SRE_Manual_0), SRE_Manual_0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(m7_Color_enhancement), m7_Color_enhancement},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(Write_Content_Adaptive_Brightness_Control), Write_Content_Adaptive_Brightness_Control},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(led_pwm_en), led_pwm_en},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(blue_shift_adjust_1), blue_shift_adjust_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(blue_shift_adjust_2), blue_shift_adjust_2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Source_Timing_Setting), Source_Timing_Setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(lock), lock},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nop), nop},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nop), nop},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(enable_te), enable_te},
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
};
static struct dsi_cmd_desc m7_sharp_video_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(interface_setting_0), interface_setting_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(nop), nop},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(nop), nop},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Manufacture_Command_setting), Manufacture_Command_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Color_enhancement), Color_enhancement},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Outline_Sharpening_Control), Outline_Sharpening_Control},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(BackLight_Control_6_28kHz), BackLight_Control_6_28kHz},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(write_control_display), write_control_display},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(CABC), CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(TE_OUT), TE_OUT},
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 1, sizeof(exit_sleep), exit_sleep},

};

static struct dsi_cmd_desc sharp_video_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(interface_setting_0), interface_setting_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Manufacture_Command_setting), Manufacture_Command_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Color_enhancement), Color_enhancement},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Outline_Sharpening_Control), Outline_Sharpening_Control},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(BackLight_Control_6), BackLight_Control_6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(write_control_display), write_control_display},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(CABC), CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(TE_OUT), TE_OUT},
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(exit_sleep), exit_sleep},

};

static struct dsi_cmd_desc sony_video_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(interface_setting_0), interface_setting_0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(hsync_output), hsync_output},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Color_enhancement), Color_enhancement},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Outline_Sharpening_Control), Outline_Sharpening_Control},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(BackLight_Control_6), BackLight_Control_6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Manufacture_Command_setting), Manufacture_Command_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(protect_on), protect_on},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(CABC), CABC},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(write_control_display), write_control_display},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(TE_OUT), TE_OUT},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(exit_sleep), exit_sleep},
};

static struct dsi_cmd_desc sharp_display_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 20,
		sizeof(display_off), display_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50,
		sizeof(enter_sleep), enter_sleep}
};

static struct dsi_cmd_desc sony_display_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(display_off), display_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 48, sizeof(enter_sleep), enter_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(interface_setting_0), interface_setting_0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nop), nop},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(deep_standby_off), deep_standby_off},
};

static char unlock_command[2] = {0xB0, 0x04}; 
static char lock_command[2] = {0xB0, 0x03}; 
static struct dsi_cmd_desc jdi_renesas_cmd_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(unlock_command), unlock_command},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(common_setting), common_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(cabc_still), cabc_still},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(cabc_movie), cabc_movie},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(SRE_Manual_0), SRE_Manual_0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(m7_Color_enhancement), m7_Color_enhancement},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(Manufacture_Command_setting), Manufacture_Command_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(lock_command), lock_command},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(write_control_display), write_control_display},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(Write_Content_Adaptive_Brightness_Control), Write_Content_Adaptive_Brightness_Control},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(enable_te), enable_te},
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 1, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 48, sizeof(enter_sleep), enter_sleep},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(unlock_command), unlock_command},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(deep_standby_off), deep_standby_off}
};

unsigned char idx[5] = {0x50, 0x01, 0x02, 0x05, 0x00};
unsigned char val[5] = {0x02, 0x09, 0x78, 0x14, 0x04};
unsigned char idx0[1] = {0x03};
unsigned char val0[1] = {0xFF};
unsigned char idx1[5] = {0x00, 0x01, 0x02, 0x03, 0x05};
unsigned char val1[5] = {0x04, 0x09, 0x78, 0xff, 0x14};
unsigned char val2[5] = {0x14, 0x08, 0x78, 0xff, 0x14};
unsigned char idx2[6] = {0x00, 0x01, 0x03, 0x03, 0x03, 0x03};
unsigned char idx3[6] = {0x00, 0x03, 0x03, 0x03, 0x03, 0x01};
unsigned char val3[6] = {0x14, 0x09, 0x50, 0xA0, 0xE0, 0xFF};
unsigned char val4[6] = {0x14, 0xF0, 0xA0, 0x50, 0x11, 0x08};

