#include <linux/delay.h>
#include <linux/pwm.h>
#include <mach/panel_id.h>
#include <mach/gpio.h>
#include <linux/spi/spi.h>
#include <mach/atmega_microp.h>
#include "../../../../drivers/video/msm/msm_fb.h"
#include "lcdc_vision.h"

static struct msm_panel_common_pdata *lcdc_vision_pdata;
static uint8_t last_val_pwm = SONYWVGA_BR_DEF_PANEL_PWM;
unsigned int g_unblank_stage = 0;

extern int qspi_send_9bit(struct spi_msg *msg);

#define LCM_CMD(_cmd, ...)					\
{                                                               \
        .cmd = _cmd,                                            \
        .data = (u8 []){__VA_ARGS__},                           \
        .len = sizeof((u8 []){__VA_ARGS__}) / sizeof(u8)        \
}
//2010-5-21 Rev May21-2(Wx, Wy)=(0.306, 0.315) Gamma = 2.2
static struct spi_msg SONY_TFT_INIT_TABLE[] = {
        //Change to level 2
	LCM_CMD(0xF1, 0x5A, 0x5A),
	LCM_CMD(0xFA, 0x32, 0x3F, 0x3F, 0x29, 0x3E, 0x3C, 0x3D, 0x2C,
		0x27, 0x3D, 0x2E, 0x31, 0x3A, 0x34, 0x36, 0x1A, 0x3F, 0x3F, 0x2E,
		0x40, 0x3C, 0x3C, 0x2B, 0x25, 0x39, 0x25, 0x23, 0x2A, 0x20, 0x22,
		0x00, 0x3F, 0x3F, 0x2F, 0x3E, 0x3C, 0x3C, 0x2A, 0x23, 0x35, 0x1E,
		0x18, 0x1C, 0x0C, 0x0E),
	LCM_CMD(0xFB, 0x00, 0x0D, 0x09, 0x0C, 0x26, 0x2E, 0x31, 0x22,
		0x19, 0x33, 0x22, 0x23, 0x21, 0x17, 0x00, 0x00, 0x25, 0x1D, 0x1F,
		0x35, 0x3C, 0x3A, 0x26, 0x1B, 0x34, 0x23, 0x23, 0x1F, 0x12, 0x00,
		0x00, 0x3F, 0x31, 0x33, 0x43, 0x48, 0x41, 0x2A, 0x1D, 0x35, 0x23,
		0x23, 0x21, 0x10, 0x00),
        // F3h Power control
	LCM_CMD(0xF3, 0x00, 0x10, 0x25, 0x01, 0x2D, 0x2D, 0x24, 0x2D, 0x10,
		0x10, 0x0A, 0x37),
        // F4h VCOM Control
	LCM_CMD(0xF4, 0x88, 0x20, 0x00, 0xAF, 0x64, 0x00, 0xAA, 0x64, 0x00, 0x00),
        //Change to level 1
	LCM_CMD(0xF0, 0x5A, 0x5A),
};


static struct spi_msg SONY_GAMMA_UPDATE_TABLE[] = {
	LCM_CMD(0x53, 0x24),
	LCM_CMD(0xF0, 0x5A, 0x5A),
	LCM_CMD(0xF1, 0x5A, 0x5A),
	LCM_CMD(0xD0, 0x5A, 0x5A),
	LCM_CMD(0xC2, 0x53, 0x12),
};
static struct spi_msg SAG_SONY_GAMMA_UPDATE_TABLE[] = {
	LCM_CMD(0x53, 0x24),
	LCM_CMD(0xF0, 0x5A, 0x5A),
	LCM_CMD(0xF1, 0x5A, 0x5A),
	LCM_CMD(0xD0, 0x5A, 0x5A),
	LCM_CMD(0xC2, 0x36, 0x12),//Change PWM to 13k for HW's request
};

static int lcm_write_tb(struct spi_msg cmd_table[], unsigned size)
{
	int i;

	for (i = 0; i < size; i++)
		qspi_send_9bit(&cmd_table[i]);
	return 0;
}

static char shrink_pwm = 0x00;
static struct spi_msg gamma_update = {
	.cmd = 0x51,
	.len = 1,
	.data = &shrink_pwm,
};

static struct spi_msg unblank_msg = {
	.cmd = 0x29,
	.len = 0,
};

static struct spi_msg blank_msg= {
	.cmd = 0x28,
	.len = 0,
};

static struct spi_msg init_cmd = {
	.cmd = 0x11,
	.len = 0,
};

static char init_data = 0x05;
static struct spi_msg init_cmd2 = {
	.cmd = 0x3A,
	.len = 1,
	.data = &init_data,
};

inline int is_sony_spi(void)
{
	if( panel_type == PANEL_ID_SAG_SONY )
		return ( (panel_type & BL_MASK) == BL_SPI ? 1 : 0 );
	else
		return ( panel_type & SONY_PWM_SPI ? 1 : 0 );
}

inline int is_sony_with_gamma(void)
{
	if(panel_type == PANEL_ID_SAG_SONY)
		return 1;
	else
		return (panel_type & SONY_GAMMA ? 1 : 0);
}

inline int is_sony_RGB666(void)
{
	if(panel_type == PANEL_ID_SAG_SONY)
		return ((panel_type & DEPTH_MASK) == DEPTH_RGB666 ? 1 : 0);
	else
		return (panel_type & SONY_RGB666 ? 1 : 0);
}

static int
sonywvga_panel_shrink_pwm(int brightness)
{
	int level;
	unsigned int min_pwm, def_pwm, max_pwm;

	if(!is_sony_spi()) {
		min_pwm = SONYWVGA_BR_MIN_PANEL_UP_PWM;
		def_pwm = SONYWVGA_BR_DEF_PANEL_UP_PWM;
		max_pwm = SONYWVGA_BR_MAX_PANEL_UP_PWM;
	} else {
		min_pwm = SONYWVGA_BR_MIN_PANEL_PWM;
		def_pwm = SONYWVGA_BR_DEF_PANEL_PWM;
		max_pwm = SONYWVGA_BR_MAX_PANEL_PWM;
	}

	if (brightness <= SONYWVGA_BR_DEF_USER_PWM) {
		if (brightness <= SONYWVGA_BR_MIN_USER_PWM)
			level = min_pwm;
		else
			level = (def_pwm - min_pwm) *
				(brightness - SONYWVGA_BR_MIN_USER_PWM) /
				(SONYWVGA_BR_DEF_USER_PWM - SONYWVGA_BR_MIN_USER_PWM) +
				min_pwm;
	} else
		level = (max_pwm - def_pwm) *
		(brightness - SONYWVGA_BR_DEF_USER_PWM) /
		(SONYWVGA_BR_MAX_USER_PWM - SONYWVGA_BR_DEF_USER_PWM) +
		def_pwm;

	return level;
}

static int lcdc_vision_panel_off(struct platform_device *pdev)
{
        return 0;
}

/*
 * Caller must make sure the spi is ready
 * */
static void lcdc_vision_set_gamma_val(int val)
{
	uint8_t data[4] = {0, 0, 0, 0};

	if (!is_sony_spi()) {
		//turn on backlight
		data[0] = 5;
		data[1] = sonywvga_panel_shrink_pwm(val);
		data[3] = 1;
		microp_i2c_write(0x25, data, 4);
	} else {
		shrink_pwm = sonywvga_panel_shrink_pwm(val);
		qspi_send_9bit(&gamma_update);
		if( panel_type == PANEL_ID_SAG_SONY )
			lcm_write_tb(SAG_SONY_GAMMA_UPDATE_TABLE,  ARRAY_SIZE(SAG_SONY_GAMMA_UPDATE_TABLE));
		else
			lcm_write_tb(SONY_GAMMA_UPDATE_TABLE,  ARRAY_SIZE(SONY_GAMMA_UPDATE_TABLE));
	}
	last_val_pwm = val;
}

static int lcdc_vision_panel_on(struct platform_device *pdev)
{
	hr_msleep(100);
	printk(KERN_ERR "%s: will send unblank\n",__func__);
	qspi_send_9bit(&unblank_msg);
	printk(KERN_ERR "%s: good!\n",__func__);
	hr_msleep(20);

	//init gamma setting
	if(!is_sony_with_gamma())
		lcm_write_tb(SONY_TFT_INIT_TABLE,
			ARRAY_SIZE(SONY_TFT_INIT_TABLE));

	lcdc_vision_set_gamma_val(last_val_pwm);
	g_unblank_stage = 1;        

	return 0;
}

static int __devinit lcdc_vision_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
                lcdc_vision_pdata = pdev->dev.platform_data;
		return 0;
	}

        printk(KERN_ERR "%s: ++\n", __func__);

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = lcdc_vision_probe,
	.driver = {
		.name   = "lcdc_vision",
	},
};

static struct msm_fb_panel_data vision_panel_data = {
	.on = lcdc_vision_panel_on,
	.off = lcdc_vision_panel_off,
};

static int lcdc_vision_lcd_init(void);

static int lcdc_dev_id;

int lcdc_vision_device_register(struct msm_panel_info *pinfo)
{
	struct platform_device *pdev = NULL;
	int ret;

	ret = lcdc_vision_lcd_init();
	if (ret) {
		pr_err("lcdc_vision_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("lcdc_vision", ++lcdc_dev_id);
	if (!pdev)
		return -ENOMEM;

	vision_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &vision_panel_data,
		sizeof(vision_panel_data));
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

static int lcdc_vision_lcd_init(void)
{
	return platform_driver_register(&this_driver);
}


