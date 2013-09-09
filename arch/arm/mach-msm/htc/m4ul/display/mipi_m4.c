#include <mach/panel_id.h>
#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_m4.h"

static struct mipi_dsi_panel_platform_data *mipi_m4_pdata;

// Functions
static int mipi_m4_lcd_init(void);
static void mipi_m4_set_backlight(struct msm_fb_data_type *mfd);

// Cmds Pointers
static struct dsi_cmd_desc *cmd_on_cmds = NULL;
static struct dsi_cmd_desc *display_on_cmds = NULL;
static struct dsi_cmd_desc *display_off_cmds = NULL;
static struct dsi_cmd_desc *backlight_cmds = NULL;
// Cmds
#include "mipi_m4_cmds.c"

static int cmd_on_cmds_count = 0;
static int display_on_cmds_count = 0;
static int display_off_cmds_count = 0;
static int backlight_cmds_count = 0;

// Backlight
static int wled_trigger_initialized;
DEFINE_LED_TRIGGER(bkl_led_trigger);
static int cur_bl_level = 0;

static int m4_send_display_cmds(struct dsi_cmd_desc *cmd, int cnt)
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

int mipi_lcd_on = 1;
static int mipi_m4_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi  = &mfd->panel_info.mipi;

        m4_send_display_cmds(display_on_cmds, display_on_cmds_count);

	m4_send_display_cmds(cmd_on_cmds, cmd_on_cmds_count);

        mipi_lcd_on = 1;

	return 0;
}

static int mipi_m4_lcd_off(struct platform_device *pdev)
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
          {
            m4_send_display_cmds(display_off_cmds, display_off_cmds_count);
            if (wled_trigger_initialized)
              led_trigger_event(bkl_led_trigger, 0);
            else
              pr_err("%s: wled trigger is not initialized!\n", __func__);
          }

        mipi_lcd_on = 0;

	return 0;
}

static unsigned char m4_shrink_pwm(int val)
{
	unsigned int pwm_min, pwm_default, pwm_max;
	unsigned char shrink_br = BRI_SETTING_MAX;

	pwm_min = 12;
	if (panel_type == PANEL_ID_KIWI_AUO_NT)
		pwm_default = 76;
	else
		pwm_default = 68;
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

inline void mipi_dsi_set_backlight(struct msm_fb_data_type *mfd, int level)
{
	struct mipi_panel_info *mipi;

	led_pwm1[1] = m4_shrink_pwm((unsigned char)level);

	if (mipi_m4_pdata && (mipi_m4_pdata->enable_wled_bl_ctrl)
	    && (wled_trigger_initialized)) {
		led_trigger_event(bkl_led_trigger, led_pwm1[1]);
		return;
	}

	mipi  = &mfd->panel_info.mipi;

        m4_send_display_cmds(backlight_cmds, backlight_cmds_count);

#ifdef CONFIG_BACKLIGHT_WLED_CABC
	
	if (wled_trigger_initialized) {
		led_trigger_event(bkl_led_trigger, mfd->bl_level);
	}
#endif
	return;
}

static void mipi_m4_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_dsi_set_backlight(mfd, mfd->bl_level);

	cur_bl_level = mfd->bl_level;
}

static int __devinit mipi_m4_lcd_probe(struct platform_device *pdev)
{
	if (panel_type == PANEL_ID_KIWI_SHARP_HX) {
		cmd_on_cmds = sharp_hx_cmd_on_cmds;
		cmd_on_cmds_count = ARRAY_SIZE(sharp_hx_cmd_on_cmds);
	} else if (panel_type == PANEL_ID_KIWI_AUO_NT) {
		cmd_on_cmds = auo_nt_cmd_on_cmds;
		cmd_on_cmds_count = ARRAY_SIZE(auo_nt_cmd_on_cmds);
	}

	display_on_cmds = sharp_display_on_cmds;
	display_on_cmds_count = ARRAY_SIZE(sharp_display_on_cmds);
	display_off_cmds = sharp_display_off_cmds;
	display_off_cmds_count = ARRAY_SIZE(sharp_display_off_cmds);
	backlight_cmds = sharp_hx_cmd_backlight_cmds;
	backlight_cmds_count = ARRAY_SIZE(sharp_hx_cmd_backlight_cmds);

        if (pdev->id == 0) {
          mipi_m4_pdata = pdev->dev.platform_data;
          return 0;
        }

	msm_fb_add_device(pdev);
        return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_m4_lcd_probe,
	.driver = {
		.name   = "mipi_m4",
	},
};

static struct msm_fb_panel_data m4_panel_data = {
	.on		= mipi_m4_lcd_on,
	.off		= mipi_m4_lcd_off,
	.set_backlight  = mipi_m4_set_backlight,
};

static int ch_used[3];

int mipi_m4_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

        ret = mipi_m4_lcd_init();
        if (ret) {
          pr_err("mipi_m4_lcd_init() failed with ret %u\n", ret);
          return ret;
        }

	pdev = platform_device_alloc("mipi_m4", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	m4_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &m4_panel_data,
		sizeof(m4_panel_data));
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

static int mipi_m4_lcd_init(void)
{
  if (panel_type == PANEL_ID_NONE) {
    pr_info("%s panel ID = PANEL_ID_NONE\n", __func__);
    return 0;
  }

  led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
  pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
  wled_trigger_initialized = 1;

  return platform_driver_register(&this_driver);
}
