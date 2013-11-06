#include <mach/panel_id.h>
#include <linux/gpio.h>
#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_m7ul.h"
#include "mipi_m7ul_cmds.c"

typedef struct dsi_cmd_t {
  struct dsi_cmd_desc *cmds;
  int count;
} dsi_cmd;

static dsi_cmd video_on_c;
static dsi_cmd cmd_on_c;
static dsi_cmd display_on_c;
static dsi_cmd display_off_c;
static dsi_cmd backlight_c;

static struct mipi_dsi_panel_platform_data *mipi_m7_pdata;
static struct i2c_client *blk_pwm_client;
static int resume_blk = 0;
static int pwmic_ver;
static unsigned int pwm_min = 6;
static unsigned int pwm_default = 81;
static unsigned int pwm_max = 255;

static int m7_send_display_cmds(dsi_cmd *cmd, bool clk_ctrl)
{
	int ret = 0;
	struct dcs_cmd_req cmdreq;

        if (cmd == NULL
            || (cmd->cmds == NULL) 
            || (cmd->count <= 0))
          return 0;

	cmdreq.cmds = cmd->cmds;
	cmdreq.cmds_cnt = cmd->count;
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

static int mipi_m7_lcd_init(void);

int mipi_lcd_on = 1;
static int mipi_m7_lcd_on(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;
  struct mipi_panel_info *mipi;
  
  mfd = platform_get_drvdata(pdev);
  if (!mfd)
    return -ENODEV;
  if (mfd->key != MFD_KEY)
    return -EINVAL;
  
  if (mipi_lcd_on)
    return 0;

  mipi  = &mfd->panel_info.mipi;
  if (mipi->mode == DSI_VIDEO_MODE) {
    m7_send_display_cmds(&video_on_c, false);
  } else {
    m7_send_display_cmds(&cmd_on_c, false);
  }

  mipi_lcd_on = 1;
  
  return 0;
}

static int mipi_m7_lcd_off(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;
  
  mfd = platform_get_drvdata(pdev);
  
  if (!mfd)
    return -ENODEV;
  if (mfd->key != MFD_KEY)
    return -EINVAL;
  
  if (!mipi_lcd_on)
    return 0;
  
  mipi_lcd_on = 0;

  resume_blk = 1;
  
  return 0;
}

static int mipi_m7_display_on(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;
  
  mfd = platform_get_drvdata(pdev);
  
  if (panel_type == PANEL_ID_DLXJ_SHARP_RENESAS ||
      panel_type == PANEL_ID_DLXJ_SONY_RENESAS ||
      panel_type == PANEL_ID_M7_SHARP_RENESAS)
    msleep(120);
  
  m7_send_display_cmds(&display_on_c, (mfd && mfd->panel_info.type == MIPI_CMD_PANEL));
  
  return 0;
}

static int mipi_m7_display_off(struct platform_device *pdev)
{
  struct msm_fb_data_type *mfd;
  
  mfd = platform_get_drvdata(pdev);
  
  m7_send_display_cmds(&display_off_c, (mfd && mfd->panel_info.type == MIPI_CMD_PANEL));

  return 0;
}

static unsigned char pwm_value;
static int blk_low = 0;

static unsigned char m7_shrink_pwm(int val)
{
	unsigned char shrink_br = BRI_SETTING_MAX;
	if(pwmic_ver >= 2)
		pwm_min = 6;
	else
		pwm_min = 11;

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

	pwm_value = shrink_br; 

	pr_info("brightness orig=%d, transformed=%d\n", val, shrink_br);

	return shrink_br;
}

void pwmic_config(unsigned char* index, unsigned char* value, int count)
{
	int i, rc;

	for(i = 0; i < count; ++i) {
		rc = i2c_smbus_write_byte_data(blk_pwm_client, index[i], value[i]);
		if (rc)
			pr_err("i2c write fail\n");
	}
}

static void mipi_m7_set_backlight(struct msm_fb_data_type *mfd)
{
	int rc;

	if ((panel_type == PANEL_ID_M7_JDI_SAMSUNG) ||
		(panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2) ||
		(panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2_1) ||
		(panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2_2) ||
		(panel_type == PANEL_ID_M7_JDI_RENESAS))
		samsung_ctrl_brightness[1] = m7_shrink_pwm((unsigned char)(mfd->bl_level));
	else if (panel_type == PANEL_ID_M7_SHARP_RENESAS_C1)
		display_brightness[1] = m7_shrink_pwm((unsigned char)(mfd->bl_level));
	else
		write_display_brightness[2] = m7_shrink_pwm((unsigned char)(mfd->bl_level));

	if(pwmic_ver >= 2) { 
		if (resume_blk) {
			resume_blk = 0;

			gpio_tlmm_config(GPIO_CFG(BL_HW_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
			gpio_set_value(BL_HW_EN, 1);
			msleep(1);
			pwmic_config(idx, val, sizeof(idx));
			msleep(1);
			pwmic_config(idx0, val0, sizeof(idx0));
		}
	} else {
		if (resume_blk) {
			resume_blk = 0;
			gpio_tlmm_config(GPIO_CFG(BL_HW_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
			gpio_set_value(BL_HW_EN, 1);
			msleep(1);

			if (pwm_value >= 21 ) {
				pwmic_config(idx1, val1, sizeof(idx1));
				blk_low = 0;
			} else {
				val2[3] = pwm_value-5;
				pwmic_config(idx1, val2, sizeof(idx1));
				blk_low = 1;
			}
		}

		if (pwm_value >= 21 ) {
			if ( blk_low == 1) {
				pwmic_config(idx2, val3, sizeof(idx2));
				blk_low = 0;
				pr_info("bl >= 21\n");
			}
		} else if ((pwm_value > 0)&&(pwm_value < 21)) {
			if ( blk_low == 0) {
				pwmic_config(idx3, val4, sizeof(idx3));
				blk_low = 1;
				pr_info("bl < 21\n");
			}
			rc = i2c_smbus_write_byte_data(blk_pwm_client, 0x03, (pwm_value - 5));
			if (rc)
				pr_err("i2c write fail\n");
		}
	}

        m7_send_display_cmds(&backlight_c, true);

	if ((mfd->bl_level) == 0) {
		gpio_tlmm_config(GPIO_CFG(BL_HW_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_set_value(BL_HW_EN, 0);
		resume_blk = 1;
	}

	return;
}

static int __devinit mipi_m7_lcd_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
		mipi_m7_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_add_device(pdev);

	pr_info("%s\n", __func__);
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_m7_lcd_probe,
	.driver = {
		.name   = "mipi_m7",
	},
};

static struct msm_fb_panel_data m7_panel_data = {
	.on	= mipi_m7_lcd_on,
	.off	= mipi_m7_lcd_off,
	.set_backlight = mipi_m7_set_backlight,
	.late_init = mipi_m7_display_on,
	.early_off = mipi_m7_display_off,
#if 0
	.color_enhance = m7_color_enhance,
#ifdef CABC_DIMMING_SWITCH
	.dimming_on = m7_dim_on,
#endif
#ifdef CONFIG_FB_MSM_CABC_LEVEL_CONTROL
	.set_cabc = m7_set_cabc,
#endif
	.sre_ctrl = m7_sre_ctrl,
#endif
};

static int ch_used[3];

int mipi_m7_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_m7_lcd_init();
	if (ret) {
		pr_err("mipi_m7_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_m7", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	m7_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &m7_panel_data,
		sizeof(m7_panel_data));
	if (ret) {
		pr_err("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_err("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}
	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static void mipi_video_sharp_init(void)
{
  if (panel_type == PANEL_ID_DLXJ_SHARP_RENESAS) {
    video_on_c.cmds = sharp_video_on_cmds;
    video_on_c.count = ARRAY_SIZE(sharp_video_on_cmds);
  } else {
    video_on_c.cmds = m7_sharp_video_on_cmds;
    video_on_c.count = ARRAY_SIZE(m7_sharp_video_on_cmds);  
  }
  display_on_c.cmds = renesas_display_on_cmds;
  display_on_c.count = ARRAY_SIZE(renesas_display_on_cmds);
  display_off_c.cmds = sharp_display_off_cmds;
  display_off_c.count = ARRAY_SIZE(sharp_display_off_cmds);
  backlight_c.cmds = renesas_cmd_backlight_cmds;
  backlight_c.count = ARRAY_SIZE(renesas_cmd_backlight_cmds);
}

static void mipi_cmd_sharp_init(void)
{
  cmd_on_c.cmds = sharp_renesas_cmd_on_cmds;
  cmd_on_c.count = ARRAY_SIZE(sharp_renesas_cmd_on_cmds);
  display_on_c.cmds = renesas_display_on_cmds;
  display_on_c.count = ARRAY_SIZE(renesas_display_on_cmds);
  display_off_c.cmds = sharp_display_off_cmds;
  display_off_c.count = ARRAY_SIZE(sharp_display_off_cmds);
  backlight_c.cmds = sharp_cmd_backlight_cmds;
  backlight_c.count = ARRAY_SIZE(sharp_cmd_backlight_cmds);
}

static void mipi_video_sony_init(void)
{
  video_on_c.cmds = sony_video_on_cmds;
  video_on_c.count = ARRAY_SIZE(sony_video_on_cmds);
  display_on_c.cmds = renesas_display_on_cmds;
  display_on_c.count = ARRAY_SIZE(renesas_display_on_cmds);
  display_off_c.cmds = sony_display_off_cmds;
  display_off_c.count = ARRAY_SIZE(sony_display_off_cmds);
  backlight_c.cmds = renesas_cmd_backlight_cmds;
  backlight_c.count = ARRAY_SIZE(renesas_cmd_backlight_cmds);
}

static void mipi_command_samsung_init(void)
{
  if(panel_type == PANEL_ID_M7_JDI_SAMSUNG) {
    cmd_on_c.cmds = samsung_jdi_panel_cmd_mode_cmds;
    cmd_on_c.count = ARRAY_SIZE(samsung_jdi_panel_cmd_mode_cmds);
    backlight_c.cmds = samsung_cmd_backlight_cmds_nop;
    backlight_c.count = ARRAY_SIZE(samsung_cmd_backlight_cmds_nop);
  } else if(panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2) {
    cmd_on_c.cmds = samsung_jdi_panel_cmd_mode_cmds_c2_nvm;
    cmd_on_c.count = ARRAY_SIZE(samsung_jdi_panel_cmd_mode_cmds_c2_nvm);
    backlight_c.cmds = samsung_cmd_backlight_cmds;
    backlight_c.count = ARRAY_SIZE(samsung_cmd_backlight_cmds);
  } else if(panel_type == PANEL_ID_M7_JDI_SAMSUNG_C2_1) {
    cmd_on_c.cmds = samsung_jdi_panel_cmd_mode_cmds_c2_1;
    cmd_on_c.count = ARRAY_SIZE(samsung_jdi_panel_cmd_mode_cmds_c2_1);
    backlight_c.cmds = samsung_cmd_backlight_cmds;
    backlight_c.count = ARRAY_SIZE(samsung_cmd_backlight_cmds);
  } else {
    cmd_on_c.cmds = samsung_jdi_panel_cmd_mode_cmds_c2_2;
    cmd_on_c.count = ARRAY_SIZE(samsung_jdi_panel_cmd_mode_cmds_c2_2);
    backlight_c.cmds = samsung_cmd_backlight_cmds;
    backlight_c.count = ARRAY_SIZE(samsung_cmd_backlight_cmds);
  }
  
  display_on_c.cmds = samsung_display_on_cmds;
  display_on_c.count = ARRAY_SIZE(samsung_display_on_cmds);
  display_off_c.cmds = samsung_display_off_cmds;
  display_off_c.count = ARRAY_SIZE(samsung_display_off_cmds);
}

static void mipi_cmd_jdi_renesas_init(void)
{
  cmd_on_c.cmds = jdi_renesas_cmd_on_cmds;
  cmd_on_c.count = ARRAY_SIZE(jdi_renesas_cmd_on_cmds);
  display_on_c.cmds = renesas_display_on_cmds;
  display_on_c.count = ARRAY_SIZE(renesas_display_on_cmds);
  display_off_c.cmds = jdi_display_off_cmds;
  display_off_c.count = ARRAY_SIZE(jdi_display_off_cmds);
  backlight_c.cmds = samsung_cmd_backlight_cmds;
  backlight_c.count = ARRAY_SIZE(samsung_cmd_backlight_cmds);
}

static void mipi_m7_init_cmds(int ptype)
{
  switch (panel_type)
    {
    case PANEL_ID_DLXJ_SHARP_RENESAS:
    case PANEL_ID_M7_SHARP_RENESAS:
      mipi_video_sharp_init();
      break;
    case PANEL_ID_M7_SHARP_RENESAS_C1:
      mipi_cmd_sharp_init();
      break;
    case PANEL_ID_DLXJ_SONY_RENESAS:
      mipi_video_sony_init();
      break;
    case PANEL_ID_M7_JDI_SAMSUNG:
    case PANEL_ID_M7_JDI_SAMSUNG_C2:
    case PANEL_ID_M7_JDI_SAMSUNG_C2_1:
    case PANEL_ID_M7_JDI_SAMSUNG_C2_2:
      mipi_command_samsung_init();
      break;
    case PANEL_ID_M7_JDI_RENESAS:
      mipi_cmd_jdi_renesas_init();
      pr_info("%s panel ID = PANEL_ID_M7_JDI_RENESAS_CMD\n", __func__);
      break;
    default:
      pr_err("%s: panel not supported!!\n", __func__);
    }
}

static const struct i2c_device_id pwm_i2c_id[] = {
	{ "pwm_i2c", 0 },
	{ }
};

static int pwm_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int rc;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_BYTE | I2C_FUNC_I2C))
		return -ENODEV;

	blk_pwm_client = client;

	return rc;
}

static struct i2c_driver pwm_i2c_driver = {
	.driver = {
		.name = "pwm_i2c",
		.owner = THIS_MODULE,
	},
	.probe = pwm_i2c_probe,
	.remove =  __exit_p( pwm_i2c_remove),
	.id_table =  pwm_i2c_id,
};
static void __exit pwm_i2c_remove(void)
{
	i2c_del_driver(&pwm_i2c_driver);
}

static int mipi_m7_lcd_init(void)
{
  int ret;
  
  ret = i2c_add_driver(&pwm_i2c_driver);
  if (ret)
    pr_err(KERN_ERR "%s: failed to add i2c driver\n", __func__);

  mipi_m7_init_cmds(panel_type);

  pwmic_ver = i2c_smbus_read_byte_data(blk_pwm_client, 0x1f);
  pr_info("%s: PWM IC version A%d\n", __func__, pwmic_ver);

  return platform_driver_register(&this_driver);
}
