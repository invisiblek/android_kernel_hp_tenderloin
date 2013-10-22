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
#include <mach/proc_comm.h>
#include <mach/vreg.h>

#include "../devices.h"
#include "../board-vision.h"
#if defined (CONFIG_FB_MSM_MDP_ABL)
#include <linux/fb.h>
#endif

struct vreg *vreg_ldo19, *vreg_ldo20;
struct vreg *vreg_ldo12;

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("[GPIO] %s: gpio_tlmm_config(%#x)=%d\n",
					__func__, table[n], rc);
			break;
		}
	}
}

inline int is_samsung_panel(void)
{
  return (panel_type == SAMSUNG_PANEL || panel_type == SAMSUNG_PANELII)? 1 : 0;
}

static inline int is_sony_panel(void)
{
  return (panel_type == SONY_PANEL_SPI)? 1 : 0;
}

#define LCM_GPIO_CFG(gpio, func)                                        \
  GPIO_CFG(gpio, func, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA)
static uint32_t display_on_gpio_table[] = {
  LCM_GPIO_CFG(VISION_LCD_PCLK, 1),
  LCM_GPIO_CFG(VISION_LCD_DE, 1),
  LCM_GPIO_CFG(VISION_LCD_VSYNC, 1),
  LCM_GPIO_CFG(VISION_LCD_HSYNC, 1),
  LCM_GPIO_CFG(VISION_LCD_G2, 1),
  LCM_GPIO_CFG(VISION_LCD_G3, 1),
  LCM_GPIO_CFG(VISION_LCD_G4, 1),
  LCM_GPIO_CFG(VISION_LCD_G5, 1),
  LCM_GPIO_CFG(VISION_LCD_G6, 1),
  LCM_GPIO_CFG(VISION_LCD_G7, 1),
  LCM_GPIO_CFG(VISION_LCD_B3, 1),
  LCM_GPIO_CFG(VISION_LCD_B4, 1),
  LCM_GPIO_CFG(VISION_LCD_B5, 1),
  LCM_GPIO_CFG(VISION_LCD_B6, 1),
  LCM_GPIO_CFG(VISION_LCD_B7, 1),
  LCM_GPIO_CFG(VISION_LCD_R3, 1),
  LCM_GPIO_CFG(VISION_LCD_R4, 1),
  LCM_GPIO_CFG(VISION_LCD_R5, 1),
  LCM_GPIO_CFG(VISION_LCD_R6, 1),
  LCM_GPIO_CFG(VISION_LCD_R7, 1),
};

static uint32_t display_off_gpio_table[] = {
  LCM_GPIO_CFG(VISION_LCD_PCLK, 0),
  LCM_GPIO_CFG(VISION_LCD_DE, 0),
  LCM_GPIO_CFG(VISION_LCD_VSYNC, 0),
  LCM_GPIO_CFG(VISION_LCD_HSYNC, 0),
  LCM_GPIO_CFG(VISION_LCD_G2, 0),
  LCM_GPIO_CFG(VISION_LCD_G3, 0),
  LCM_GPIO_CFG(VISION_LCD_G4, 0),
  LCM_GPIO_CFG(VISION_LCD_G5, 0),
  LCM_GPIO_CFG(VISION_LCD_G6, 0),
  LCM_GPIO_CFG(VISION_LCD_G7, 0),
  LCM_GPIO_CFG(VISION_LCD_B0, 0),
  LCM_GPIO_CFG(VISION_LCD_B3, 0),
  LCM_GPIO_CFG(VISION_LCD_B4, 0),
  LCM_GPIO_CFG(VISION_LCD_B5, 0),
  LCM_GPIO_CFG(VISION_LCD_B6, 0),
  LCM_GPIO_CFG(VISION_LCD_B7, 0),
  LCM_GPIO_CFG(VISION_LCD_R0, 0),
  LCM_GPIO_CFG(VISION_LCD_R3, 0),
  LCM_GPIO_CFG(VISION_LCD_R4, 0),
  LCM_GPIO_CFG(VISION_LCD_R5, 0),
  LCM_GPIO_CFG(VISION_LCD_R6, 0),
  LCM_GPIO_CFG(VISION_LCD_R7, 0),
};

static uint32_t display_gpio_table[] = {
  VISION_LCD_PCLK,
  VISION_LCD_DE,
  VISION_LCD_VSYNC,
  VISION_LCD_HSYNC,
  VISION_LCD_G2,
  VISION_LCD_G3,
  VISION_LCD_G4,
  VISION_LCD_G5,
  VISION_LCD_G6,
  VISION_LCD_G7,
  VISION_LCD_B0,
  VISION_LCD_B3,
  VISION_LCD_B4,
  VISION_LCD_B5,
  VISION_LCD_B6,
  VISION_LCD_B7,
  VISION_LCD_R0,
  VISION_LCD_R3,
  VISION_LCD_R4,
  VISION_LCD_R5,
  VISION_LCD_R6,
  VISION_LCD_R7,
};

#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_PRIM_BUF_SIZE (800 * 480 * 4 * 3) /* 4 bpp x 3 pages */
#else
#define MSM_FB_PRIM_BUF_SIZE (800 * 480 * 4 * 2) /* 4 bpp x 2 pages */
#endif

#define MSM_FB_EXT_BUF_SIZE     0

#ifdef CONFIG_FB_MSM_OVERLAY0_WRITEBACK
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((800 * 480 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE (0)
#endif  /* CONFIG_FB_MSM_OVERLAY0_WRITEBACK */

#ifdef CONFIG_FB_MSM_OVERLAY1_WRITEBACK
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE roundup((1920 * 1088 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE (0)
#endif  /* CONFIG_FB_MSM_OVERLAY1_WRITEBACK */

/* Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE + MSM_FB_EXT_BUF_SIZE + MSM_FB_OVERLAY0_WRITEBACK_SIZE, 4096)

#define GPIO_LCD_PWR_EN  62
#define GPIO_LVDS_SHDN_N 63
#define GPIO_BACKLIGHT_EN  PM8058_GPIO_PM_TO_SYS(25-1)

static struct resource msm_fb_resources[] = {
	{
		.flags = IORESOURCE_DMA,
	}
};

static struct msm_fb_platform_data msm_fb_pdata;

static struct platform_device msm_fb_device = {
	.name              = "msm_fb",
	.id                = 0,
	.num_resources     = ARRAY_SIZE(msm_fb_resources),
	.resource          = msm_fb_resources,
	.dev.platform_data = &msm_fb_pdata,
};

void __init msm7x30_allocate_fb_region(void)
{
	void *addr;
	unsigned long size;

	size = MSM_FB_SIZE;
        addr = alloc_bootmem_align(size, 0x1000);
        msm_fb_resources[0].start = __pa(addr);
        msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
        pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
                size, addr, __pa(addr));
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

#ifdef CONFIG_FB_MSM_LCDC_DSUB
static struct msm_bus_vectors mdp_sd_smi_vectors[] = {
	/* Default case static display/UI/2d/3d if FB SMI */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 388800000,
		.ib = 486000000,
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
		.ab = 388800000,
		.ib = 486000000 * 2,
	},
};
static struct msm_bus_vectors mdp_vga_vectors[] = {
	/* VGA and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 458092800,
		.ib = 572616000,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 458092800,
		.ib = 572616000 * 2,
	},
};
static struct msm_bus_vectors mdp_720p_vectors[] = {
	/* 720p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 471744000,
		.ib = 589680000,
	},
	/* Master and slaves can be from different fabrics */
       {
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
	       .ab = 471744000,
	       .ib = 589680000 * 2,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	/* 1080p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 575424000,
		.ib = 719280000,
	},
	/* Master and slaves can be from different fabrics */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 575424000,
		.ib = 719280000 * 2,
	},
};

#else
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

#endif
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
#define MDP_VSYNC_GPIO			28

static struct msm_panel_common_pdata mdp_pdata = {
  //        .gpio = MDP_VSYNC_GPIO,
        .gpio = 30,
	.hw_revision_addr = 0xac001270,
        //        .mdp_max_clk = 200000000,
	.mdp_max_clk = 192000000,
	.mdp_rev = MDP_REV_40,
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
        .mem_hid = BIT(ION_CP_WB_HEAP_ID),
#else
        .mem_hid = MEMTYPE_EBI0,
#endif
};

void __init msm7x30_mdp_writeback(struct memtype_reserve* reserve_table)
{
	mdp_pdata.ov0_wb_size = MSM_FB_OVERLAY0_WRITEBACK_SIZE;
	mdp_pdata.ov1_wb_size = MSM_FB_OVERLAY1_WRITEBACK_SIZE;
#if defined(CONFIG_ANDROID_PMEM) && !defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	reserve_table[mdp_pdata.mem_hid].size +=
		mdp_pdata.ov0_wb_size;
	reserve_table[mdp_pdata.mem_hid].size +=
		mdp_pdata.ov1_wb_size;
#endif
}

extern int lcdc_lcd_on;
static bool lcdc_power_on;

static int lcdc_panel_power(int on)
{
  static struct vreg *vreg_ldo19, *vreg_ldo20, *vreg_ldo12;
  static bool bPanelPowerOn = false;
  int rc;
  
  printk(KERN_ERR "%s: ++\n", __func__);
  
  if (panel_type == PANEL_ID_NONE)
    return -ENODEV;
  
  if (!lcdc_power_on) 
    {
      vreg_ldo12 = vreg_get(NULL, "gp9");
      if (IS_ERR_OR_NULL(vreg_ldo12)) {
        pr_err("%s: gp9 vreg get failed (%ld)\n",
               __func__, PTR_ERR(vreg_ldo12));
        return -ENODEV;
      }
      
      vreg_ldo19 = vreg_get(NULL, "wlan2");
      if (IS_ERR_OR_NULL(vreg_ldo19)) {
        pr_err("%s: wlan2 vreg get failed (%ld)\n",
               __func__, PTR_ERR(vreg_ldo19));
        return -ENODEV;
      }
      
      vreg_ldo20 = vreg_get(NULL, "gp13");
      if (IS_ERR_OR_NULL(vreg_ldo20)) {
        pr_err("%s: gp13 vreg get failed (%ld)\n",
               __func__, PTR_ERR(vreg_ldo20));
        return -ENODEV;
      }
      
      rc = vreg_set_level(vreg_ldo12, 2850);
      if (rc) {
        pr_err("%s: vreg LDO12(gp9) set level failed (%d)\n",
               __func__, rc);
        return -EINVAL;
      }
      
      rc = vreg_set_level(vreg_ldo19, 1800);
      if (rc) {
        pr_err("%s: vreg LDO19 set level failed (%d)\n",
               __func__, rc);
        return -EINVAL;
      }
      
      if (is_samsung_panel())
        rc = vreg_set_level(vreg_ldo20, 2850);
      else
        rc = vreg_set_level(vreg_ldo20, 2600);
      if (rc) {
        pr_err("%s: vreg LDO20 set level failed (%d)\n",
               __func__, rc);
        return -EINVAL;
      }
      rc = gpio_request(VISION_LCD_RSTz, "LCM_RST_N");
      if (rc) {
        printk(KERN_ERR "%s:LCM gpio %d request failed, rc=%d\n", __func__,  VISION_LCD_RSTz, rc);
        return -EINVAL;
      }
      if (system_rev <= 1)
        {
          rc = gpio_request(VISION_EL_EN, "EL_EN");
          if (rc) {
            printk(KERN_ERR "%s: EL_EN gpio %d request failed, rc=%d\n", __func__,  VISION_EL_EN, rc);
            return -EINVAL;
          }
        }
      lcdc_power_on = true;
    }
  
  if (on) 
    {
      printk(KERN_ERR "%s: on\n", __func__);
      if (bPanelPowerOn) return 0;
      /* turn on L19 for OJ. Note: must before L12 */
      rc = vreg_enable(vreg_ldo19);
      if (rc) {
        pr_err("%s: LDO19 vreg enable failed (%d)\n",
               __func__, rc);
        return -EINVAL;
      }
      msleep(5);
      rc = vreg_enable(vreg_ldo12);
      if (rc) {
        pr_err("%s: LDO12 vreg enable failed (%d)\n",
               __func__, rc);
        return -EINVAL;
      }
      msleep(5);
      rc = vreg_enable(vreg_ldo20);
      if (rc) {
        pr_err("%s: LDO20 vreg enable failed (%d)\n",
               __func__, rc);
        return -EINVAL;
      }
      if (!lcdc_lcd_on) 
        {
          msleep(10);
          gpio_set_value(VISION_LCD_RSTz, 1);
          msleep(10);
          gpio_set_value(VISION_LCD_RSTz, 0);
          msleep(500);
          gpio_set_value(VISION_LCD_RSTz, 1);
          msleep(10);
          /* XA, XB board has HW panel issue, need to set EL_EN pin */
          if(system_rev <= 1)
            gpio_set_value(VISION_EL_EN, 1);
        }
      usleep(60);
      bPanelPowerOn = true;
    }
  else 
    {
      if (!bPanelPowerOn) return 0;
      msleep(10);
      if(system_rev <= 1)
        gpio_set_value(VISION_EL_EN, 0);
      gpio_set_value(VISION_LCD_RSTz, 0);
      msleep(120);
      
      rc = vreg_disable(vreg_ldo12);
      if (rc)
        {
          pr_err("%s: LDO12, 19, 20 vreg disable failed (%d)\n",
                 __func__, rc);
          return -ENODEV;
        }
      msleep(5);
      rc = vreg_disable(vreg_ldo19);
      if (rc)
        {
          pr_err("%s: LDO12, 19, 20 vreg disable failed (%d)\n",
                 __func__, rc);
          return -ENODEV;
        }
      msleep(5);
      rc = vreg_disable(vreg_ldo20);
      if (rc)
        {
          pr_err("%s: LDO12, 19, 20 vreg disable failed (%d)\n",
                 __func__, rc);
          return -ENODEV;
        }
      msleep(5);
      bPanelPowerOn = false;
    }
  return 0;
}

static int panel_gpio_switch(int on)
{
  uint32_t pin, id;

  config_gpio_table(
                    !!on ? display_on_gpio_table : display_off_gpio_table,
                    ARRAY_SIZE(display_on_gpio_table));

  if (!on) {
    for (pin = 0; pin < ARRAY_SIZE(display_gpio_table); pin++) {
      gpio_set_value(display_gpio_table[pin], 0);
    }
    id = GPIO_CFG(VISION_LCD_R6, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA);
    msm_proc_comm(PCOM_RPC_GPIO_TLMM_CONFIG_EX, &id, 0);
    id = GPIO_CFG(VISION_LCD_R7, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA);
    msm_proc_comm(PCOM_RPC_GPIO_TLMM_CONFIG_EX, &id, 0);
    gpio_set_value(VISION_LCD_R6, 0);
    gpio_set_value(VISION_LCD_R7, 0);
  }
  return 0;
}

/* a hacky interface to control the panel power */
static int lcdc_config_gpios(int on)
{
        int rc;
	printk(KERN_INFO "%s: power goes to %d\n", __func__, on);

        rc = lcdc_panel_power(on);
        if (rc)
          {
            printk(KERN_ERR "%s: panel_power failed!\n", __func__);
            return rc;
          }
        rc = panel_gpio_switch(on);
	if (rc)
          {
            printk(KERN_ERR "%s: panel_gpio_switch failed!\n", __func__);
            return rc;
          }
        return 0;
}

static struct lcdc_platform_data lcdc_pdata = {
  //	.lcdc_power_save   = lcdc_panel_power,
	.lcdc_power_save   = lcdc_config_gpios,
};

static struct platform_device lcdc_vision_panel_device = {
	.name = "lcdc_vision",
	.id = 0,
};

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors dtv_bus_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};
static struct msm_bus_vectors dtv_bus_def_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 566092800 * 2,
		.ib = 707616000 * 2,
	},
};
static struct msm_bus_paths dtv_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(dtv_bus_init_vectors),
		dtv_bus_init_vectors,
	},
	{
		ARRAY_SIZE(dtv_bus_def_vectors),
		dtv_bus_def_vectors,
	},
};
static struct msm_bus_scale_pdata dtv_bus_scale_pdata = {
	dtv_bus_scale_usecases,
	ARRAY_SIZE(dtv_bus_scale_usecases),
	.name = "dtv",
};
#endif

static struct lcdc_platform_data dtv_pdata = {
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table = &dtv_bus_scale_pdata,
#endif
};

void __init msm7x30_set_display_params(char *prim_panel, char *ext_panel)
{
	if (strnlen(prim_panel, PANEL_NAME_MAX_LEN)) {
		strlcpy(msm_fb_pdata.prim_panel_name, prim_panel,
			PANEL_NAME_MAX_LEN);
		pr_debug("msm_fb_pdata.prim_panel_name %s\n",
			msm_fb_pdata.prim_panel_name);
	}

	if (strnlen(ext_panel, PANEL_NAME_MAX_LEN)) {
		strlcpy(msm_fb_pdata.ext_panel_name, ext_panel,
			PANEL_NAME_MAX_LEN);
		pr_debug("msm_fb_pdata.ext_panel_name %s\n",
			msm_fb_pdata.ext_panel_name);
	}
}

void __init vision_fb_init(void)
{
  printk(KERN_ERR "%s: Sony=%d Samsung=%d Other=%d\n", __func__, is_sony_panel(), panel_type == SAMSUNG_PANEL, panel_type != SAMSUNG_PANEL && !is_sony_panel());

	msm7x30_set_display_params("lcdc_vision", "hdmi_msm");
	platform_device_register(&msm_fb_device);
	platform_device_register(&lcdc_vision_panel_device);
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("lcdc", &lcdc_pdata);
        msm_fb_register_device("dtv", &dtv_pdata);
}

