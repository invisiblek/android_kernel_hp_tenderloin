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
#ifdef CONFIG_FB_MSM_HDMI_MHL
#include <video/msm_hdmi_modes.h>
#endif

#include "../devices.h"
#include "../board-ruby.h"
#if defined (CONFIG_FB_MSM_MDP_ABL)
#include <linux/fb.h>
#endif

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

void __init msm8x60_allocate_fb_region(void)
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

static struct msm_bus_vectors mdp_ui_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000 * 2,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_vga_vectors[] = {
	/* VGA and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000 * 2,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_720p_vectors[] = {
	/* 720p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 230400000 * 2,
		.ib = 288000000 * 2,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	/* 1080p and less video */
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

static struct msm_bus_scale_pdata mdp_bus_scale_pdata = {
	mdp_bus_scale_usecases,
	ARRAY_SIZE(mdp_bus_scale_usecases),
	.name = "mdp",
};
#endif /* CONFIG_MSM_BUS_SCALING */

struct mdp_table_entry ruby_color_enhancement[] = {
	{0x93400, 0x0222, 0x0},
	{0x93404, 0xFFE4, 0x0},
	{0x93408, 0xFFFD, 0x0},
	{0x9340C, 0xFFF1, 0x0},
	{0x93410, 0x0212, 0x0},
	{0x93414, 0xFFF9, 0x0},
	{0x93418, 0xFFF1, 0x0},
	{0x9341C, 0xFFE6, 0x0},
	{0x93420, 0x022D, 0x0},
	{0x93600, 0x0000, 0x0},
	{0x93604, 0x00FF, 0x0},
	{0x93608, 0x0000, 0x0},
	{0x9360C, 0x00FF, 0x0},
	{0x93610, 0x0000, 0x0},
	{0x93614, 0x00FF, 0x0},
	{0x93680, 0x0000, 0x0},
	{0x93684, 0x00FF, 0x0},
	{0x93688, 0x0000, 0x0},
	{0x9368C, 0x00FF, 0x0},
	{0x93690, 0x0000, 0x0},
	{0x93694, 0x00FF, 0x0},
	{0x90070, 0xCD298008, 0x0},
};

struct mdp_table_entry ruy_auo_gamma[] = {
	{0x94800, 0x000000, 0x0},
	{0x94804, 0x010201, 0x0},
	{0x94808, 0x020202, 0x0},
	{0x9480C, 0x030304, 0x0},
	{0x94810, 0x040405, 0x0},
	{0x94814, 0x050506, 0x0},
	{0x94818, 0x060508, 0x0},
	{0x9481C, 0x070609, 0x0},
	{0x94820, 0x08070A, 0x0},
	{0x94824, 0x09080B, 0x0},
	{0x94828, 0x0A080C, 0x0},
	{0x9482C, 0x0B090E, 0x0},
	{0x94830, 0x0C0A0F, 0x0},
	{0x94834, 0x0D0B10, 0x0},
	{0x94838, 0x0E0B11, 0x0},
	{0x9483C, 0x0F0C12, 0x0},
	{0x94840, 0x100D13, 0x0},
	{0x94844, 0x110E14, 0x0},
	{0x94848, 0x120E15, 0x0},
	{0x9484C, 0x130F16, 0x0},
	{0x94850, 0x141016, 0x0},
	{0x94854, 0x151117, 0x0},
	{0x94858, 0x161118, 0x0},
	{0x9485C, 0x171219, 0x0},
	{0x94860, 0x18131A, 0x0},
	{0x94864, 0x19141A, 0x0},
	{0x94868, 0x1A151B, 0x0},
	{0x9486C, 0x1B151C, 0x0},
	{0x94870, 0x1C161D, 0x0},
	{0x94874, 0x1D171D, 0x0},
	{0x94878, 0x1E181E, 0x0},
	{0x9487C, 0x1F181F, 0x0},
	{0x94880, 0x201920, 0x0},
	{0x94884, 0x211A20, 0x0},
	{0x94888, 0x221B21, 0x0},
	{0x9488C, 0x231C22, 0x0},
	{0x94890, 0x241C22, 0x0},
	{0x94894, 0x251D23, 0x0},
	{0x94898, 0x261E23, 0x0},
	{0x9489C, 0x271F24, 0x0},
	{0x948A0, 0x282025, 0x0},
	{0x948A4, 0x292025, 0x0},
	{0x948A8, 0x2A2126, 0x0},
	{0x948AC, 0x2B2227, 0x0},
	{0x948B0, 0x2C2327, 0x0},
	{0x948B4, 0x2D2428, 0x0},
	{0x948B8, 0x2E2528, 0x0},
	{0x948BC, 0x2F2529, 0x0},
	{0x948C0, 0x30262A, 0x0},
	{0x948C4, 0x31272A, 0x0},
	{0x948C8, 0x32282B, 0x0},
	{0x948CC, 0x33292C, 0x0},
	{0x948D0, 0x34292C, 0x0},
	{0x948D4, 0x352A2D, 0x0},
	{0x948D8, 0x362B2D, 0x0},
	{0x948DC, 0x372C2E, 0x0},
	{0x948E0, 0x382D2F, 0x0},
	{0x948E4, 0x392E2F, 0x0},
	{0x948E8, 0x3A2E30, 0x0},
	{0x948EC, 0x3B2F30, 0x0},
	{0x948F0, 0x3C3030, 0x0},
	{0x948F4, 0x3D3131, 0x0},
	{0x948F8, 0x3E3231, 0x0},
	{0x948FC, 0x3F3332, 0x0},
	{0x94900, 0x403433, 0x0},
	{0x94904, 0x413434, 0x0},
	{0x94908, 0x423535, 0x0},
	{0x9490C, 0x433635, 0x0},
	{0x94910, 0x443735, 0x0},
	{0x94914, 0x453836, 0x0},
	{0x94918, 0x463937, 0x0},
	{0x9491C, 0x473938, 0x0},
	{0x94920, 0x483A39, 0x0},
	{0x94924, 0x493B3A, 0x0},
	{0x94928, 0x4A3C3B, 0x0},
	{0x9492C, 0x4B3D3C, 0x0},
	{0x94930, 0x4C3E3D, 0x0},
	{0x94934, 0x4C3E3E, 0x0},
	{0x94938, 0x4C3E3F, 0x0},
	{0x9493C, 0x4D3F40, 0x0},
	{0x94940, 0x4E4040, 0x0},
	{0x94944, 0x4F4041, 0x0},
	{0x94948, 0x504142, 0x0},
	{0x9494C, 0x514243, 0x0},
	{0x94950, 0x524344, 0x0},
	{0x94954, 0x534445, 0x0},
	{0x94958, 0x544546, 0x0},
	{0x9495C, 0x554546, 0x0},
	{0x94960, 0x564648, 0x0},
	{0x94964, 0x574748, 0x0},
	{0x94968, 0x584849, 0x0},
	{0x9496C, 0x5A4A4A, 0x0},
	{0x94970, 0x5C4B4A, 0x0},
	{0x94974, 0x5D4C4B, 0x0},
	{0x94978, 0x5E4D4C, 0x0},
	{0x9497C, 0x5F4E4D, 0x0},
	{0x94980, 0x604F4F, 0x0},
	{0x94984, 0x615050, 0x0},
	{0x94988, 0x625050, 0x0},
	{0x9498C, 0x635151, 0x0},
	{0x94990, 0x645252, 0x0},
	{0x94994, 0x655353, 0x0},
	{0x94998, 0x665454, 0x0},
	{0x9499C, 0x675556, 0x0},
	{0x949A0, 0x685657, 0x0},
	{0x949A4, 0x695758, 0x0},
	{0x949A8, 0x6A5759, 0x0},
	{0x949AC, 0x6B585A, 0x0},
	{0x949B0, 0x6C595B, 0x0},
	{0x949B4, 0x6D5A5D, 0x0},
	{0x949B8, 0x6E5B5E, 0x0},
	{0x949BC, 0x6F5C5F, 0x0},
	{0x949C0, 0x705D60, 0x0},
	{0x949C4, 0x715D61, 0x0},
	{0x949C8, 0x725E62, 0x0},
	{0x949CC, 0x735F63, 0x0},
	{0x949D0, 0x746064, 0x0},
	{0x949D4, 0x756166, 0x0},
	{0x949D8, 0x766267, 0x0},
	{0x949DC, 0x776368, 0x0},
	{0x949E0, 0x786469, 0x0},
	{0x949E4, 0x79646A, 0x0},
	{0x949E8, 0x7A656B, 0x0},
	{0x949EC, 0x7B666C, 0x0},
	{0x949F0, 0x7C676E, 0x0},
	{0x949F4, 0x7D686F, 0x0},
	{0x949F8, 0x7E6970, 0x0},
	{0x949FC, 0x7F6A71, 0x0},
	{0x94A00, 0x806B72, 0x0},
	{0x94A04, 0x816B74, 0x0},
	{0x94A08, 0x826C75, 0x0},
	{0x94A0C, 0x836D76, 0x0},
	{0x94A10, 0x846E77, 0x0},
	{0x94A14, 0x856F78, 0x0},
	{0x94A18, 0x867079, 0x0},
	{0x94A1C, 0x87717B, 0x0},
	{0x94A20, 0x88727C, 0x0},
	{0x94A24, 0x89737D, 0x0},
	{0x94A28, 0x8A737E, 0x0},
	{0x94A2C, 0x8B747F, 0x0},
	{0x94A30, 0x8C7581, 0x0},
	{0x94A34, 0x8D7682, 0x0},
	{0x94A38, 0x8E7783, 0x0},
	{0x94A3C, 0x8F7884, 0x0},
	{0x94A40, 0x907985, 0x0},
	{0x94A44, 0x917A87, 0x0},
	{0x94A48, 0x927B88, 0x0},
	{0x94A4C, 0x937B89, 0x0},
	{0x94A50, 0x947C8A, 0x0},
	{0x94A54, 0x957D8B, 0x0},
	{0x94A58, 0x967E8D, 0x0},
	{0x94A5C, 0x977F8E, 0x0},
	{0x94A60, 0x98808F, 0x0},
	{0x94A64, 0x998190, 0x0},
	{0x94A68, 0x9A8291, 0x0},
	{0x94A6C, 0x9B8393, 0x0},
	{0x94A70, 0x9C8494, 0x0},
	{0x94A74, 0x9D8595, 0x0},
	{0x94A78, 0x9E8696, 0x0},
	{0x94A7C, 0x9F8697, 0x0},
	{0x94A80, 0xA08798, 0x0},
	{0x94A84, 0xA18899, 0x0},
	{0x94A88, 0xA2899B, 0x0},
	{0x94A8C, 0xA38A9C, 0x0},
	{0x94A90, 0xA48B9D, 0x0},
	{0x94A94, 0xA58C9E, 0x0},
	{0x94A98, 0xA68D9F, 0x0},
	{0x94A9C, 0xA78EA0, 0x0},
	{0x94AA0, 0xA88FA1, 0x0},
	{0x94AA4, 0xA990A2, 0x0},
	{0x94AA8, 0xAA91A4, 0x0},
	{0x94AAC, 0xAB92A5, 0x0},
	{0x94AB0, 0xAC93A6, 0x0},
	{0x94AB4, 0xAD94A7, 0x0},
	{0x94AB8, 0xAE95A8, 0x0},
	{0x94ABC, 0xAF96A9, 0x0},
	{0x94AC0, 0xB097AA, 0x0},
	{0x94AC4, 0xB198AB, 0x0},
	{0x94AC8, 0xB299AC, 0x0},
	{0x94ACC, 0xB39AAD, 0x0},
	{0x94AD0, 0xB49BAE, 0x0},
	{0x94AD4, 0xB59CAF, 0x0},
	{0x94AD8, 0xB69DB0, 0x0},
	{0x94ADC, 0xB79EB1, 0x0},
	{0x94AE0, 0xB89FB2, 0x0},
	{0x94AE4, 0xB9A0B3, 0x0},
	{0x94AE8, 0xBAA1B4, 0x0},
	{0x94AEC, 0xBBA2B5, 0x0},
	{0x94AF0, 0xBCA3B6, 0x0},
	{0x94AF4, 0xBDA4B7, 0x0},
	{0x94AF8, 0xBEA5B8, 0x0},
	{0x94AFC, 0xBFA6B9, 0x0},
	{0x94B00, 0xC0A7BA, 0x0},
	{0x94B04, 0xC1A8BB, 0x0},
	{0x94B08, 0xC2A9BC, 0x0},
	{0x94B0C, 0xC3AABD, 0x0},
	{0x94B10, 0xC4ACBE, 0x0},
	{0x94B14, 0xC5ADBF, 0x0},
	{0x94B18, 0xC6AEC0, 0x0},
	{0x94B1C, 0xC7AFC1, 0x0},
	{0x94B20, 0xC8B0C2, 0x0},
	{0x94B24, 0xC9B1C3, 0x0},
	{0x94B28, 0xCAB2C4, 0x0},
	{0x94B2C, 0xCBB3C5, 0x0},
	{0x94B30, 0xCCB5C6, 0x0},
	{0x94B34, 0xCDB6C6, 0x0},
	{0x94B38, 0xCEB7C7, 0x0},
	{0x94B3C, 0xCFB8C8, 0x0},
	{0x94B40, 0xD0B9C9, 0x0},
	{0x94B44, 0xD1BBCA, 0x0},
	{0x94B48, 0xD2BCCB, 0x0},
	{0x94B4C, 0xD3BDCC, 0x0},
	{0x94B50, 0xD4BECD, 0x0},
	{0x94B54, 0xD5BFCE, 0x0},
	{0x94B58, 0xD6C1CF, 0x0},
	{0x94B5C, 0xD7C2D0, 0x0},
	{0x94B60, 0xD8C3D1, 0x0},
	{0x94B64, 0xD9C4D2, 0x0},
	{0x94B68, 0xDAC6D3, 0x0},
	{0x94B6C, 0xDBC7D3, 0x0},
	{0x94B70, 0xDCC8D4, 0x0},
	{0x94B74, 0xDDCAD5, 0x0},
	{0x94B78, 0xDECBD6, 0x0},
	{0x94B7C, 0xDFCCD7, 0x0},
	{0x94B80, 0xE0CED8, 0x0},
	{0x94B84, 0xE1CFD9, 0x0},
	{0x94B88, 0xE2D1DA, 0x0},
	{0x94B8C, 0xE3D2DB, 0x0},
	{0x94B90, 0xE4D3DC, 0x0},
	{0x94B94, 0xE5D5DD, 0x0},
	{0x94B98, 0xE6D6DE, 0x0},
	{0x94B9C, 0xE7D8DF, 0x0},
	{0x94BA0, 0xE8D9E0, 0x0},
	{0x94BA4, 0xE9DBE2, 0x0},
	{0x94BA8, 0xEADCE3, 0x0},
	{0x94BAC, 0xEBDEE4, 0x0},
	{0x94BB0, 0xECDFE5, 0x0},
	{0x94BB4, 0xEDE1E6, 0x0},
	{0x94BB8, 0xEEE2E7, 0x0},
	{0x94BBC, 0xEFE4E8, 0x0},
	{0x94BC0, 0xF0E5EA, 0x0},
	{0x94BC4, 0xF1E7EB, 0x0},
	{0x94BC8, 0xF2E9EC, 0x0},
	{0x94BCC, 0xF3EAED, 0x0},
	{0x94BD0, 0xF4ECEF, 0x0},
	{0x94BD4, 0xF5EDF0, 0x0},
	{0x94BD8, 0xF6EFF1, 0x0},
	{0x94BDC, 0xF7F1F3, 0x0},
	{0x94BE0, 0xF8F3F4, 0x0},
	{0x94BE4, 0xF9F4F6, 0x0},
	{0x94BE8, 0xFAF6F7, 0x0},
	{0x94BEC, 0xFBF8F9, 0x0},
	{0x94BF0, 0xFCFAFA, 0x0},
	{0x94BF4, 0xFDFBFC, 0x0},
	{0x94BF8, 0xFEFDFD, 0x0},
	{0x94BFC, 0xFFFFFF, 0x0},
	{0x90070, 0x1F, 0x0},
};

int ruby_mdp_gamma(void)
{
	mdp_color_enhancement(ruby_color_enhancement, ARRAY_SIZE(ruby_color_enhancement));

	if (panel_type == PANEL_ID_PYD_AUO_NT)
		mdp_color_enhancement(ruy_auo_gamma, ARRAY_SIZE(ruy_auo_gamma));

	return 0;
}

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = GPIO_LCD_TE,
	.mdp_max_clk = 200000000,
	.mdp_max_bw = 2000000000,
	.mdp_bw_ab_factor = 115,
	.mdp_bw_ib_factor = 150,
#ifdef CONFIG_MSM_BUS_SCALING
	.mdp_bus_scale_table = &mdp_bus_scale_pdata,
#endif
	.mdp_rev = MDP_REV_41,
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
	.mem_hid = BIT(ION_CP_WB_HEAP_ID),
#else
	.mem_hid = MEMTYPE_EBI1,
#endif
	.mdp_iommu_split_domain = 0,
	.mdp_gamma = ruby_mdp_gamma,
};

void __init msm8x60_mdp_writeback(struct memtype_reserve* reserve_table)
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

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct resource hdmi_msm_resources[] = {
	{
		.name  = "hdmi_msm_qfprom_addr",
		.start = 0x00700000,
		.end   = 0x007060FF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_hdmi_addr",
		.start = 0x04A00000,
		.end   = 0x04A00FFF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_irq",
		.start = HDMI_IRQ,
		.end   = HDMI_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

#ifdef CONFIG_FB_MSM_HDMI_MHL
static mhl_driving_params ruby_driving_params[] = {
	{.format = HDMI_VFRMT_640x480p60_4_3,	.reg_a3=0xEB, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_720x480p60_16_9,	.reg_a3=0xEB, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_1280x720p60_16_9,	.reg_a3=0xEB, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_720x576p50_16_9,	.reg_a3=0xEB, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_1920x1080p24_16_9, .reg_a3=0xEB, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_1920x1080p30_16_9, .reg_a3=0xEB, .reg_a6=0x0C},
};
#endif

static int hdmi_core_power(int on, int show);
static int hdmi_cec_power(int on);
static int hdmi_gpio_config(int on);
//static int hdmi_panel_power(int on);

static struct msm_hdmi_platform_data hdmi_msm_data = {
	.irq = HDMI_IRQ,
	.enable_5v = hdmi_enable_5v,
	.core_power = hdmi_core_power,
	.cec_power = hdmi_cec_power,
	.gpio_config = hdmi_gpio_config,
#ifdef CONFIG_FB_MSM_HDMI_MHL
	.driving_params =  ruby_driving_params,
	.driving_params_count = ARRAY_SIZE(ruby_driving_params),
#endif
};

static struct platform_device hdmi_msm_device = {
	.name = "hdmi_msm",
	.id = 0,
	.num_resources = ARRAY_SIZE(hdmi_msm_resources),
	.resource = hdmi_msm_resources,
	.dev.platform_data = &hdmi_msm_data,
};
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */

extern int mipi_lcd_on;
static bool dsi_power_on;

static int mipi_dsi_panel_power(int on)
{
	static struct regulator *rgl_l19, *rgl_l20;
	static bool bPanelPowerOn = false;
	int ret;
	int rc;

        printk(KERN_ERR  "[DISP] %s +++ %d %d\n", __func__, mipi_lcd_on, dsi_power_on == true);
	/* To avoid system crash in shutdown for non-panel case */
	if (panel_type == PANEL_ID_NONE)
		return -ENODEV;

	/* If panel is already on (or off), do nothing. */
	if (!dsi_power_on) {
		rgl_l19 = regulator_get(NULL, "8058_l19");
		if (IS_ERR(rgl_l19)) {
			pr_err("%s: unable to get 8058_l19\n", __func__);
			 return -ENODEV;
		}

		rgl_l20 = regulator_get(NULL, "8058_l20");
		if (IS_ERR(rgl_l20)) {
			pr_err("%s: unable to get 8058_l20\n", __func__);
			 return -ENODEV;
		}

		ret = regulator_set_voltage(rgl_l19, 3000000, 3000000);
		if (ret) {
			pr_err("%s: error setting l19_2v85 voltage\n", __func__);
			 return -ENODEV;
		}

		ret = regulator_set_voltage(rgl_l20, 1800000, 1800000);
		if (ret) {
			pr_err("%s: error setting l20_1v8 voltage\n", __func__);
                        return -EINVAL;
		}

		/* LCM Reset */
		rc = gpio_request(GPIO_LCM_RST_N,
				"LCM_RST_N");
		if (rc) {
			printk(KERN_ERR "%s:LCM gpio %d request"
					"failed\n", __func__,
					GPIO_LCM_RST_N);
			return -EINVAL;
		}
		dsi_power_on = true;
	}

	if (on) {
                if (bPanelPowerOn) return 0;
		if (regulator_enable(rgl_l19)) {
			pr_err("%s: Unable to enable the regulator:"
					" l19_2v85\n", __func__);
			return -ENODEV;
		}
		msleep(5);

		if (regulator_enable(rgl_l20)) {
			pr_err("%s: Unable to enable the regulator:"
				" l20_1v8\n", __func__);
			return -ENODEV;
		}

                if (!mipi_lcd_on) {
                        printk(KERN_ERR "%s: RESET\n", __func__);
			msleep(10);
			gpio_set_value(GPIO_LCM_RST_N, 1);
			msleep(1);
			gpio_set_value(GPIO_LCM_RST_N, 0);
			msleep(1);
			gpio_set_value(GPIO_LCM_RST_N, 1);
			msleep(20);
                }
                bPanelPowerOn = true;
	} else {
                if (!bPanelPowerOn) return 0;
		gpio_set_value(GPIO_LCM_RST_N, 0);
		msleep(5);
		if (regulator_disable(rgl_l20)) {
			pr_err("%s: Unable to enable the regulator:"
				" l20_1v8\n", __func__);
			return -EINVAL;
		}
		msleep(5);
		if (regulator_disable(rgl_l19)) {
			pr_err("%s: Unable to enable the regulator:"
					" l19_2v85\n", __func__);
			return -EINVAL;
		}

                bPanelPowerOn = false;
	}
	return 0;
}

static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.vsync_gpio	= GPIO_LCD_TE,
	.dsi_power_save = mipi_dsi_panel_power,
};

static struct platform_device mipi_dsi_ruby_panel_device = {
	.name = "mipi_ruby",
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

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
int hdmi_enable_5v(int on)
{
	static struct regulator *reg_8901_hdmi_mvs;	/* HDMI_5V */
	static struct regulator *reg_8901_mpp0;		/* External 5V */
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_8901_hdmi_mvs)
          {
		_GET_REGULATOR(reg_8901_hdmi_mvs, "8901_hdmi_mvs");
                return -1;
          }
	if (!reg_8901_mpp0)
          {
		_GET_REGULATOR(reg_8901_mpp0, "8901_mpp0");
                return -1;
          }

	if (on) {
		rc = regulator_enable(reg_8901_mpp0);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8901_mpp0", rc);
			return rc;
		}
		rc = regulator_enable(reg_8901_hdmi_mvs);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8901_hdmi_mvs", rc);
			return rc;
		}
		pr_info("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_8901_hdmi_mvs);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"8901_hdmi_mvs", rc);
		rc = regulator_disable(reg_8901_mpp0);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8901_mpp0", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}

static int hdmi_core_power(int on, int show)
{
	static struct regulator *reg_8058_l16;		/* VDD_HDMI */
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_8058_l16)
		_GET_REGULATOR(reg_8058_l16, "8058_l16");

	if (on) {
		rc = regulator_set_voltage(reg_8058_l16, 1800000, 1800000);
		if (!rc)
			rc = regulator_enable(reg_8058_l16);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8058_l16", rc);
			return rc;
		}

		pr_info("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_8058_l16);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"8058_l16", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}

static int hdmi_gpio_config(int on)
{
	return 0;
}

static int hdmi_cec_power(int on)
{
	static struct regulator *reg_8901_l3;		/* HDMI_CEC */
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_8901_l3)
		_GET_REGULATOR(reg_8901_l3, "8901_l3");

	if (on) {
		rc = regulator_set_voltage(reg_8901_l3, 3300000, 3300000);
		if (!rc)
			rc = regulator_enable(reg_8901_l3);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"8901_l3", rc);
			return rc;
		}
		rc = gpio_request(169, "HDMI_CEC_VAR");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_CEC_VAR", 169, rc);
			goto error;
		}
		pr_info("%s(on): success\n", __func__);
	} else {
		gpio_free(169);
		rc = regulator_disable(reg_8901_l3);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"8901_l3", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
error:
	regulator_disable(reg_8901_l3);
	return rc;
}
#endif

static void __init msm8x60_set_display_params(char *prim_panel, char *ext_panel)
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

int mipi_cmd_ruby_qhd_pt_init(void);

void __init ruby_init_fb(void)
{
	msm8x60_set_display_params("mipi_ruby", "hdmi_msm");
	platform_device_register(&msm_fb_device);
	platform_device_register(&mipi_dsi_ruby_panel_device);
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("mipi_dsi", &mipi_dsi_pdata);
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	platform_device_register(&hdmi_msm_device);
#endif
        msm_fb_register_device("dtv", &dtv_pdata);
}
