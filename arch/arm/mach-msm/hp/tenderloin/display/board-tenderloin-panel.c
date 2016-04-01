/* linux/arch/arm/mach-msm/board-mecha-panel.c
 *
 * Copyright (c) 2011 HTC.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../../../drivers/video/msm/msm_fb.h"
#include "../../../../drivers/video/msm/mipi_dsi.h"
#include "../../../../drivers/video/msm/mdp.h"
#include "../../../../drivers/video/msm/mdp4.h"
#include "../../../../drivers/video/msm/msm_fb_panel.h"
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <mach/panel_id.h>
#include <mach/msm_memtypes.h>
#include <linux/bootmem.h>
#include <mach/msm_gpiomux.h>

#include "../devices.h"
#include "../board-tenderloin.h"
#include <mach/board-msm8660.h>

/* 4 bpp x 3 pages */
#define MSM_FB_PRIM_BUF_SIZE (1024 * 768 * 4 * 3)

/* Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE, 4096)

/** allow the framebuffer's address to be passed from the bootloader on the command line */
static unsigned long fb_phys = 0;
static int __init fb_args(char *str)
{
	fb_phys = memparse(str, NULL);
	return 0;
}
early_param("fb", fb_args);

#define GPIO_LCD_PWR_EN  62
#define GPIO_LVDS_SHDN_N 63
#define GPIO_BACKLIGHT_EN  PM8058_GPIO_PM_TO_SYS(25-1)

static struct resource msm_fb_resources[] = {
	{
		.flags = IORESOURCE_DMA,
	}
};

static struct msm_fb_platform_data msm_fb_pdata = {
	.prim_panel_name = "lcdc_tenderloin",
};

static struct platform_device msm_fb_device = {
	.name              = "msm_fb",
	.id                = 0,
	.num_resources     = ARRAY_SIZE(msm_fb_resources),
	.resource          = msm_fb_resources,
	.dev.platform_data = &msm_fb_pdata,
};

void __init msm8x60_allocate_fb_region(void)
{
	void *addr;
	unsigned long size;

	size = MSM_FB_SIZE;

	if(fb_phys) {
		addr = (void *)fb_phys;
		msm_fb_resources[0].start = (unsigned long)addr;
		msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
		pr_info("passing from bootie %lu bytes at %lx physical for fb\n",
			size, fb_phys);
	} else {
                addr = alloc_bootmem_align(size, 0x1000);
		msm_fb_resources[0].start = __pa(addr);
		msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
		pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
			size, addr, __pa(addr));
	}
}

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors mdp_init_vectors[] = {
	/* For now, 0th array entry is reserved.
	 * Please leave 0 as is and don't use it
	 */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_smi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 175110000,
		.ib = 218887500,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_ebi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000,
		.ib = 270000000 * 2,
	},
};
static struct msm_bus_vectors mdp_vga_vectors[] = {
	/* VGA and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 216000000,
		.ib = 270000000,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_720p_vectors[] = {
	/* 720p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 230400000,
		.ib = 288000000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 230400000,
		.ib = 288000000 * 2,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	/* 1080p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 334080000,
		.ib = 417600000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 334080000,
		.ib = 550000000 * 2,
	},
};

static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
	{
		ARRAY_SIZE(mdp_sd_smi_vectors),
		mdp_sd_smi_vectors,
	},
	{
		ARRAY_SIZE(mdp_sd_ebi_vectors),
		mdp_sd_ebi_vectors,
	},
	{
		ARRAY_SIZE(mdp_vga_vectors),
		mdp_vga_vectors,
	},
	{
		ARRAY_SIZE(mdp_720p_vectors),
		mdp_720p_vectors,
	},
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
};
static struct msm_bus_scale_pdata mdp_bus_scale_pdata = {
	mdp_bus_scale_usecases,
	ARRAY_SIZE(mdp_bus_scale_usecases),
	.name = "mdp",
};
#endif

#define MDP_VSYNC_GPIO	28

static struct msm_panel_common_pdata mdp_pdata = {
        .gpio = MDP_VSYNC_GPIO,
        .mdp_max_clk = 200000000,
#ifdef CONFIG_MSM_BUS_SCALING
	.mdp_bus_scale_table = &mdp_bus_scale_pdata,
#endif
	.mdp_rev = MDP_REV_41,
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
        .mem_hid = BIT(ION_CP_MM_HEAP_ID),
#else
	.mem_hid = MEMTYPE_EBI1,
#endif
	.cont_splash_enabled = 0x01,
        .mdp_iommu_split_domain = 0,
};

static int lcd_panel_gpios[] = {
	0, /* lcdc_pclk */
	1, /* lcdc_hsync*/
	2, /* lcdc_vsync*/
	3, /* lcdc_den */
	4, /* lcdc_red7 */
	5, /* lcdc_red6 */
	6, /* lcdc_red5 */
	7, /* lcdc_red4 */
	8, /* lcdc_red3 */
	9, /* lcdc_red2 */
	10, /* lcdc_red1 */
	11, /* lcdc_red0 */
	12, /* lcdc_grn7 */
	13, /* lcdc_grn6 */
	14, /* lcdc_grn5 */
	15, /* lcdc_grn4 */
	16, /* lcdc_grn3 */
	17, /* lcdc_grn2 */
	18, /* lcdc_grn1 */
	19, /* lcdc_grn0 */
	20, /* lcdc_blu7 */
	21, /* lcdc_blu6 */
	22, /* lcdc_blu5 */
	23, /* lcdc_blu4 */
	24, /* lcdc_blu3 */
	25, /* lcdc_blu2 */
	26, /* lcdc_blu1 */
	27, /* lcdc_blu0 */
};

static int configure_gpiomux_gpios(int on, int gpios[], int cnt)
{
	int ret = 0;
	int i;

	for (i = 0; i < cnt; i++) {
		if (on) {
			ret = msm_gpiomux_get(gpios[i]);
			if (unlikely(ret))
				break;
		} else {
			ret = msm_gpiomux_put(gpios[i]);
			if (unlikely(ret))
				return ret;
		}
	}

	if (ret)
		for (; i >= 0; i--)
			msm_gpiomux_put(gpios[i]);

	return ret;
}

int lcdc_lg_panel_power(int on)
{
  return 0;
}

static bool lcdc_power_on;
extern int lcdc_gpio_request(bool on);

static int lcdc_panel_power(int on)
{
	static bool bPanelPowerOn = false;
        static struct regulator *votg_l10, *votg_vdd5v;
	int rc = 0;

        printk(KERN_ERR "[DISP] %s: ++ %d\n", __func__, on);

	if (!lcdc_power_on) { // If panel is shut down (first init)
		votg_l10 = regulator_get(NULL, "8058_l10");
		if (IS_ERR_OR_NULL(votg_l10)) {
			pr_err("[DISP] %s: unable to get 8058_l10\n", __func__);
		return -ENODEV;
		}

		/* Due to hardware change, it will not use GPIO102 as 5V boost Enable since EVT1*/
		if (board_type < TOPAZ_EVT1) {
			/* VDD_BACKLIGHT_5.0V*/
			votg_vdd5v = regulator_get(NULL, "vdd50_boost");
			if (IS_ERR_OR_NULL(votg_vdd5v)) {
				pr_err("[DISP] %s: unable to get 8901_l4\n", __func__);
				return -ENODEV;
			}
		}

		/* VDD_LVDS_3.3V ENABLE*/
		rc = regulator_set_voltage(votg_l10, 3050000, 3050000);
		if(rc) {
			pr_err("[DISP] %s: Unable to set regulator voltage:"
				" votg_l10\n", __func__);
			return rc;
		}

		/* LVDS_SHDN_N*/
		rc = gpio_request(GPIO_LVDS_SHDN_N,"LVDS_SHDN_N");
		if (rc) {
			pr_err("[DISP] %s: LVDS gpio %d request"
				"failed\n", __func__, GPIO_LVDS_SHDN_N);
			return rc;
		}

		/* LCD_PWR_EN */
		rc = gpio_request(GPIO_LCD_PWR_EN, "LCD_PWR_EN");
		if (rc) {
			pr_err("[DISP] %s: LCD Power gpio %d request"
				"failed\n", __func__, GPIO_LCD_PWR_EN);
			gpio_free(GPIO_LVDS_SHDN_N);
			return rc;
		}

		/* BACKLIGHT */
		rc = gpio_request(GPIO_BACKLIGHT_EN, "BACKLIGHT_EN");
		if (rc) {
			pr_err("[DISP] %s: BACKLIGHT gpio %d request"
				"failed\n", __func__, GPIO_BACKLIGHT_EN);
			gpio_free(GPIO_LVDS_SHDN_N);
			gpio_free(GPIO_LCD_PWR_EN);
			return rc;
		}

		tenderloin_lcdc_steadycfg();
		lcdc_power_on = true;
	}

	if (on) {
		if (bPanelPowerOn)
			return 0;
		rc = regulator_enable(votg_l10);
		if(rc) {
			pr_err("[DISP] %s: Unable to enable the regulator: votg_l10\n", __func__);
			return rc;
		}

		/* Due to hardware change, it will not use GPIO102 as 5V boost Enable since EVT1*/
		if (board_type < TOPAZ_EVT1) {
			/* VDD_BACKLIGHT_5.0V ENABLE*/
			rc = regulator_enable(votg_vdd5v);
			if(rc) {
				pr_err("[DISP] %s: Unable to enable the regulator: votg_vdd5v\n", __func__);
				return rc;
			}
		}

		gpio_set_value_cansleep(GPIO_LCD_PWR_EN, 1);
		gpio_set_value_cansleep(GPIO_LVDS_SHDN_N, 1);
		msleep(20);
		gpio_set_value_cansleep(GPIO_BACKLIGHT_EN, 1);
		mdelay(20);
		bPanelPowerOn = true;
	} else {
		if (!bPanelPowerOn)
			return 0;

		rc = regulator_disable(votg_l10);
		if (rc) {
			pr_err("%s: Unable to disable votg_l10\n",__func__);
			return rc;
		}
            
		/* Due to hardware change, it will not use GPIO102 as 5V boost Enable since EVT1*/
		if (board_type < TOPAZ_EVT1) {
			rc = regulator_disable(votg_vdd5v);
			if (rc) {
				pr_err("%s: Unable to disable votg_vdd5v\n",__func__);
				return rc;
			}
		}
            
		gpio_set_value_cansleep(GPIO_BACKLIGHT_EN, 0);
		mdelay(5);
		gpio_set_value_cansleep(GPIO_LVDS_SHDN_N, 0);
		gpio_set_value_cansleep(GPIO_LCD_PWR_EN, 0);
		msleep(20);
		bPanelPowerOn = false;
	}
    	lcdc_gpio_request(on);
	/* configure lcdc gpios */
        configure_gpiomux_gpios(on, lcd_panel_gpios, ARRAY_SIZE(lcd_panel_gpios));

        return 0;
}

static struct lcdc_platform_data lcdc_pdata = {
	.lcdc_power_save   = lcdc_panel_power,
};

static struct platform_device lcdc_tenderloin_panel_device = {
	.name = "lcdc_tenderloin",
	.id = 0,
};

void __init tenderloin_init_fb(void)
{
	platform_device_register(&msm_fb_device);
	platform_device_register(&lcdc_tenderloin_panel_device);
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("lcdc", &lcdc_pdata);
}

