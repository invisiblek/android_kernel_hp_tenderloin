/* linux/arch/arm/mach-msm/display/operaul-panel.c
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

#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/bootmem.h>
#include <asm/mach-types.h>
#include <mach/msm_bus_board.h>
#include <mach/msm_memtypes.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>
#include <linux/ion.h>
#include <mach/ion.h>
#include <linux/regulator/consumer.h>

#include "devices.h"
#include "../board-operaul.h"
#include <mach/panel_id.h>
#include <mach/debug_display.h>
#include <asm/system_info.h>
#include <linux/leds.h>
#include "../../../../drivers/video/msm/msm_fb.h"
#include "../../../../drivers/video/msm/mipi_dsi.h"
#include "../../../../drivers/video/msm/mdp4.h"

#define RESOLUTION_WIDTH 768
#define RESOLUTION_HEIGHT 1376

#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_PRIM_BUF_SIZE \
		(roundup((RESOLUTION_WIDTH * RESOLUTION_HEIGHT * 4), 4096) * 3) 
#else
#define MSM_FB_PRIM_BUF_SIZE \
		(roundup((RESOLUTION_WIDTH * RESOLUTION_HEIGHT * 4), 4096) * 2) 
#endif

#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE, 4096)

#ifdef CONFIG_FB_MSM_OVERLAY0_WRITEBACK
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((1376 * 768 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE (0)
#endif  

#ifdef CONFIG_FB_MSM_OVERLAY1_WRITEBACK
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE roundup((1920 * 1088 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE (0)
#endif  

#define MDP_VSYNC_GPIO 0

#define MIPI_CMD_ORISE_QHD_PANEL_NAME	"mipi_cmd_orise_qhd"
#define MIPI_CMD_HIMAX_720P_PANEL_NAME	"mipi_cmd_himax_720p"
#define HDMI_PANEL_NAME	"hdmi_msm"
#define TVOUT_PANEL_NAME	"tvout_msm"

static struct resource msm_fb_resources[] = {
	{
		.flags = IORESOURCE_DMA,
	},
};

static int operaul_detect_panel(const char *name)
{
#if 0
	if (system_rev >= 1) {
		if (!strncmp(name, MIPI_CMD_HIMAX_720P_PANEL_NAME,
					strnlen(MIPI_CMD_HIMAX_720P_PANEL_NAME,
						PANEL_NAME_MAX_LEN))) {
			pr_info("%s: Support (%s)\n",__func__, name);
			return 0;
		}
	}
	else {
		if (!strncmp(name, MIPI_CMD_ORISE_QHD_PANEL_NAME,
					strnlen(MIPI_CMD_ORISE_QHD_PANEL_NAME,
						PANEL_NAME_MAX_LEN))) {
			pr_info("%s: Support (%s)\n",__func__, name);
			return 0;
		}
	}

	PR_WARN("%s: not supported '%s'\n", __func__, name);
#endif

	return -ENODEV;
}

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = operaul_detect_panel,
};

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources     = ARRAY_SIZE(msm_fb_resources),
	.resource          = msm_fb_resources,
	.dev.platform_data = &msm_fb_pdata,
};

void __init operaul_allocate_fb_region(void)
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
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
static struct msm_bus_vectors hdmi_as_primary_vectors[] = {
	
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 2000000000,
		.ib = 2000000000,
	},
};
static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
};
#else
static struct msm_bus_vectors mdp_ui_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000 * 2,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_vga_vectors[] = {
	
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000 * 2,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_720p_vectors[] = {
	
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 230400000 * 2,
		.ib = 288000000 * 2,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 334080000 * 2,
		.ib = 417600000 * 2,
	},
};

static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
	{
		ARRAY_SIZE(mdp_ui_vectors),
		mdp_ui_vectors,
	},
	{
		ARRAY_SIZE(mdp_ui_vectors),
		mdp_ui_vectors,
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
#endif

static struct msm_bus_scale_pdata mdp_bus_scale_pdata = {
	mdp_bus_scale_usecases,
	ARRAY_SIZE(mdp_bus_scale_usecases),
	.name = "mdp",
};

#endif

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = MDP_VSYNC_GPIO,
	.mdp_max_clk = 200000000,
	.mdp_max_bw = 2000000000,
	.mdp_bw_ab_factor = 115,
	.mdp_bw_ib_factor = 150,
#ifdef CONFIG_MSM_BUS_SCALING
	.mdp_bus_scale_table = &mdp_bus_scale_pdata,
#endif
	.mdp_rev = MDP_REV_43,
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
	.mem_hid = BIT(ION_CP_MM_HEAP_ID),
#else
	.mem_hid = MEMTYPE_EBI1,
#endif
	.mdp_iommu_split_domain = 0,
};

void __init operaul_mdp_writeback(struct memtype_reserve* reserve_table)
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

static struct dcs_cmd_req cmdreq;
static bool dsi_power_on;
static bool first_inited = true;

static struct dsi_cmd_desc nvt_LowTemp_wrkr_enter[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xFF, 0xEE}},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, 2, (char[]){0x26, 0x08}},
};

static struct dsi_cmd_desc nvt_LowTemp_wrkr_exit[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0x26, 0x00}},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 10, 2, (char[]){0xFF, 0x00}},
};

static int mipi_dsi_panel_power(int on)
{
	static struct regulator *reg_l2;
	static struct regulator *reg_l10;

	int rc;

	PR_DISP_INFO("%s: power %s.\n", __func__, on ? "on" : "off");

	if (!dsi_power_on) {
		
		reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8038_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			return -ENODEV;
		}
		rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		
		reg_l10 = regulator_get(&msm_mipi_dsi1_device.dev,
				"8038_l10");
		if (IS_ERR(reg_l10)) {
			pr_err("could not get 8038_l10, rc = %ld\n",
				PTR_ERR(reg_l10));
			return -ENODEV;
		}
		rc = regulator_set_voltage(reg_l10, 3000000, 3000000);
		if (rc) {
			pr_err("set_voltage l10 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		dsi_power_on = true;

		if (first_inited) {
			first_inited = false;

			rc = regulator_set_optimum_mode(reg_l2, 100000);
			if (rc < 0) {
				pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
				return -EINVAL;
			}
			rc = regulator_enable(reg_l2);
			if (rc) {
				pr_err("enable l2 failed, rc=%d\n", rc);
				return -ENODEV;
			}
			
			rc = regulator_set_optimum_mode(reg_l10, 100000);
			if (rc < 0) {
				pr_err("set_optimum_mode l10 failed, rc=%d\n", rc);
				return -EINVAL;
			}
			rc = regulator_enable(reg_l10);
			if (rc) {
				pr_err("enable l10 failed, rc=%d\n", rc);
				return -ENODEV;
			}
			return 0;
		}
	}

	if (on) {
		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l10, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l10 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		gpio_set_value(MSM_LCD_RSTz, 0);
		msleep(5);

		gpio_set_value(MSM_LCMIO_1V8_EN, 1);
		msleep(1);

		
		rc = regulator_enable(reg_l10);
		if (rc) {
			pr_err("enable l10 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		msleep(12);

		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}

		gpio_set_value(MSM_LCD_RSTz, 1);
		msleep(1);

		if (panel_type == PANEL_ID_OPA_SHARP_NT_ROTA || panel_type == PANEL_ID_OPA_SHARP_NT) {
			cmdreq.cmds = nvt_LowTemp_wrkr_enter;
			cmdreq.cmds_cnt = ARRAY_SIZE(nvt_LowTemp_wrkr_enter);
			cmdreq.flags = CMD_REQ_COMMIT;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;
			mipi_dsi_cmdlist_put(&cmdreq);

			cmdreq.cmds = nvt_LowTemp_wrkr_exit;
			cmdreq.cmds_cnt = ARRAY_SIZE(nvt_LowTemp_wrkr_exit);
			cmdreq.flags = CMD_REQ_COMMIT;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;
			mipi_dsi_cmdlist_put(&cmdreq);

			msleep(10);
		}

		gpio_set_value(MSM_LCD_RSTz, 0);
		msleep(1);
		gpio_set_value(MSM_LCD_RSTz, 1);
		msleep(25);
	} else {
		gpio_set_value(MSM_LCD_RSTz, 0);
		msleep(12);

		rc = regulator_disable(reg_l10);
		if (rc) {
			pr_err("disable reg_l10 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_set_optimum_mode(reg_l10, 100);
			if (rc < 0) {
				pr_err("set_optimum_mode l10 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		msleep(5);
		gpio_set_value(MSM_LCMIO_1V8_EN, 0);

		rc = regulator_disable(reg_l2);
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_set_optimum_mode(reg_l2, 100);
			if (rc < 0) {
				pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
	}
	return 0;
}


static struct mipi_dsi_panel_platform_data operaul_pdata = {
	.dlane_swap = 0,
#ifdef CONFIG_BACKLIGHT_WLED_CABC
	.enable_wled_bl_ctrl = 0x0,
#else
	.enable_wled_bl_ctrl = 0x1,
#endif
};

static struct platform_device mipi_dsi_operaul_panel_device = {
	.name = "mipi_operaul",
	.id = 0,
	.dev = {
		.platform_data = &operaul_pdata,
	}
};

static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.vsync_gpio = MDP_VSYNC_GPIO,
	.dsi_power_save = mipi_dsi_panel_power,
};

#ifdef CONFIG_MSM_CABC_VIDEO_ENHANCE
static void operaul_set_cabc(struct msm_fb_data_type *mfd, int mode)
{
	PR_DEBUG("%s: mode=%d\n",  __FUNCTION__, mode);

	if (mode == 0) {
		cmdreq.cmds = cabc_UI_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(cabc_UI_cmds);
	} else {
		cmdreq.cmds = cabc_moving_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(cabc_moving_cmds);
	}
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;
	mipi_dsi_cmdlist_put(&cmdreq);
}
#endif


void __init operaul_init_fb(void)
{
	platform_device_register(&msm_fb_device);

	if(panel_type != PANEL_ID_NONE && board_mfg_mode() != 5) {
		platform_device_register(&mipi_dsi_operaul_panel_device);
		msm_fb_register_device("mdp", &mdp_pdata);
		msm_fb_register_device("mipi_dsi", &mipi_dsi_pdata);
	}
}

