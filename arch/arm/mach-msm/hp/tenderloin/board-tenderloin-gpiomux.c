/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/bootmem.h>
#include <asm/mach-types.h>
#include <asm/mach/mmc.h>
#include <mach/msm_bus_board.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>
#include "devices.h"
#include "board-tenderloin.h"
#include "gpiomux-8x60.h"

static struct gpiomux_setting console_uart = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

/* This I2C active configuration applies to GSBI3 and GSBI4 */
static struct gpiomux_setting i2c_active = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
static struct gpiomux_setting ebi2_a_d = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting ebi2_oe = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting ebi2_we = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting ebi2_cs2 = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting ps_hold = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_12MA,
	.pull = 0,
};

static struct gpiomux_setting ebi2_cs3 = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};
#endif

#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
static struct gpiomux_setting ebi2_cs4 = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting ebi2_adv = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};
#endif

#ifdef CONFIG_MSM8X60_AUDIO
static struct gpiomux_setting aux_pcm_active_config = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting aux_pcm_suspend_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};
#endif

static struct gpiomux_setting uart1dm_active = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting mdp_vsync_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting mdp_vsync_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting pmic_suspended_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

#ifdef CONFIG_MSM_GSBI9_UART
static struct gpiomux_setting uart9dm_active = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA ,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting gsbi9 = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};
#endif

#define GPIOMUX_CFG(f, d, p) {.func = f, .drv = d, .pull = p};
#define GPIOMUX_DCFG(f, d, p, r) {.func = f, .drv = d, .pull = p, .dir = r};
#define GPIOMUX_SUSP_DEF(pin, suspend)\
	{ .gpio = pin, .settings = {\
			[GPIOMUX_SUSPENDED] = suspend, },\
	}
#define GPIOMUX_ACT_SUSP_DEF(pin, active, suspend)\
	{ .gpio = pin, .settings = {\
			[GPIOMUX_ACTIVE] = active,\
			[GPIOMUX_SUSPENDED] = suspend, },\
	}
static struct gpiomux_setting gpio_in_2m_pu =
  GPIOMUX_CFG(                                                          \
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_UP)

#define GPIO_IN_2M_PU &gpio_in_2m_pu

static struct gpiomux_setting gpio_in_2m_pd = 
  GPIOMUX_CFG(                                  \
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_DOWN)

#define GPIO_IN_2M_PD &gpio_in_2m_pd

static struct gpiomux_setting gpio_in_2m_pn = 
  GPIOMUX_CFG(                                                          \
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define GPIO_IN_2M_PN &gpio_in_2m_pn

static struct gpiomux_setting gpio_in_8m_pu = 
  GPIOMUX_CFG(                                                          \
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)

#define GPIO_IN_8M_PU &gpio_in_8m_pu

static struct gpiomux_setting gpio_in_8m_pd = 
  GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_DOWN)

#define GPIO_IN_8M_PD &gpio_in_8m_pd

static struct gpiomux_setting gpio_outl_2m_pn = 
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO,GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)
#define GPIO_OUTL_2M_PN &gpio_outl_2m_pn

static struct gpiomux_setting gpio_outl_8m_pn = 
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)
#define GPIO_OUTL_8M_PN &gpio_outl_8m_pn

static struct gpiomux_setting gpio_outh_2m_pn = 
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_2M_PN &gpio_outh_2m_pn

static struct gpiomux_setting gpio_outh_8m_pn = 
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_8M_PN &gpio_outh_8m_pn

#ifdef CONFIG_A6
static struct gpiomux_setting gpio_outh_8m_pu = 
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_8M_PU &gpio_outh_8m_pu
#endif

static struct gpiomux_setting gpio_in_2m_pk = 
  GPIOMUX_CFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_KEEPER)
#define GPIO_IN_2M_PK &gpio_in_2m_pk

#define CONSOLE_UART &console_uart

#define GSBI1 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, 0)
#define GSBI3 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, 0)
#define GSBI4 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, 0)
#define GSBI7 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_12MA, 0)
#define GSBI8 GPIOMUX_CFG(GPIOMUX_FUNC_1, 0, 0)

#define PS_HOLD	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_12MA, 0)

#define EBI2_A_D GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_OE  GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_WE	 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_CS2 GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_CS3 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_CS4 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_ADV GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)

#define USB_ISP1763_ACTV_CFG GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, \
					 GPIOMUX_PULL_NONE)
#define USB_ISP1763_SUSP_CFG GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, 0, \
					 GPIOMUX_PULL_DOWN)

#define SDCC1_DAT_0_3_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC1_DAT_4_7_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC1_CLK_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_16MA, GPIOMUX_PULL_NONE)
#define SDCC1_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_UP)

#define SDCC2_DAT_0_3_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC2_DAT_4_7_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC2_CLK_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_16MA, GPIOMUX_PULL_NONE)
#define SDCC2_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define SDCC5_DAT_0_3_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC5_DAT_4_7_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC5_CLK_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_16MA, GPIOMUX_PULL_NONE)
#define SDCC5_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define AUX_PCM_ACTIVE_CONFIG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define AUX_PCM_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_NONE)

#define UART1DM_ACTIVE &uart1dm_active

static struct gpiomux_setting uart1dm_out_active = 
  GPIOMUX_DCFG(\
	GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)
#define UART1DM_OUT_ACTIVE &uart1dm_out_active

static struct gpiomux_setting uart1dm_suspended_out_high = 
	GPIOMUX_DCFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define UART1DM_SUSPENDED_OUT_HIGH &uart1dm_suspended_out_high

static struct gpiomux_setting uart1dm_suspended_in_high = 
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_UP)
#define UART1DM_SUSPENDED_IN_HIGH &uart1dm_suspended_in_high

#define MI2S_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MI2S_SUSPEND_CFG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

static struct gpiomux_setting lcdc_active_cfg = 
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_14MA, GPIOMUX_PULL_NONE)
#define LCDC_ACTIVE_CFG &lcdc_active_cfg

static struct gpiomux_setting lcdc_active_cfg_2m = 
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define LCDC_ACTIVE_CFG_2M &lcdc_active_cfg_2m

#define LCDC_SUSPEND_CFG GPIO_IN_2M_PD

#define HDMI_SUSPEND_CFG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define MDM2AP_STATUS_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MDM2AP_STATUS_SUSPEND_CFG \
	GPIOMUX_CFG(0, 0, GPIOMUX_PULL_NONE)

#ifdef CONFIG_A6
static struct gpiomux_setting cam_gpio_outh_8m_pn =
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define CAM_F1_OUTH_8M_PN &cam_gpio_outh_8m_pn
#endif

static struct gpiomux_setting cam_gpio_outl_8m_pn =
  GPIOMUX_DCFG(\
		GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)
#define CAM_F1_OUTL_8M_PN &cam_gpio_outl_8m_pn

#define MDM2AP_SYNC_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MDM2AP_SYNC_SUSPEND_CFG \
	GPIOMUX_CFG(0, 0, GPIOMUX_PULL_NONE)

#define BT_CFG_IN GPIO_IN_2M_PN

#define BT_CFG_OUT GPIOMUX_DCFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)

static struct msm_gpiomux_config msm8x60_gsbi_configs[] __initdata = {
	{
		.gpio      = 43,
		.settings = {
                        [GPIOMUX_SUSPENDED] = &i2c_active,
			[GPIOMUX_ACTIVE]    = &i2c_active,
		},
	},
	{
		.gpio      = 44,
		.settings = {
                        [GPIOMUX_SUSPENDED] = &i2c_active,
			[GPIOMUX_ACTIVE]    = &i2c_active,
		},
	},
};

#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
static struct msm_gpiomux_config msm8x60_ebi2_configs[] __initdata = {
	{
		.gpio      = 40,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_cs2,
		},
	},
	{
		.gpio      = 92,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ps_hold,
		},
	},
	{
		.gpio      = 123,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 124,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 125,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 127,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 128,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 129,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 130,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
	/* ISP VDD_3V3_EN */
	{
		.gpio      = 132,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_cs4,
		},
	},
#endif
	{
		.gpio      = 133,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_cs3,
		},
	},
	{
		.gpio      = 135,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 136,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 137,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 138,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 139,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 140,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 141,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 142,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 143,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 144,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 145,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 146,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 147,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 148,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 149,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 150,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_a_d,
		},
	},
	{
		.gpio      = 151,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_oe,
		},
	},
	{
		.gpio      = 153,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_adv,
		},
	},
	{
		.gpio      = 157,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ebi2_we,
		},
	},
};
#endif

#ifdef CONFIG_USB_PEHCI_HCD
static struct msm_gpiomux_config msm8x60_isp_usb_configs[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(ISP1763_INT_GPIO, GPIO_IN_2M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(ISP1763_DACK_GPIO, GPIO_OUTH_2M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(ISP1763_DREQ_GPIO, GPIO_IN_2M_PD, GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(ISP1763_RST_GPIO, GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_3V3_EN, GPIO_OUTL_2M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_DISABLE_N, GPIO_OUTH_2M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_WAKE_N, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_UIM_CD_N, GPIO_IN_2M_PU, GPIO_IN_2M_PU),
};
#endif

static struct msm_gpiomux_config msm8x60_uart_configs[] __initdata = {
	{ /* UARTDM_TX */
		.gpio      = 53,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_OUT_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_OUT_HIGH,
		},
	},
	{ /* UARTDM_RX */
		.gpio      = 54,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_IN_HIGH,
		},
	},
	{ /* UARTDM_CTS */
		.gpio      = 55,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_IN_HIGH,
		},
	},
	{ /* UARTDM_RFR */
		.gpio      = 56,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_OUT_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_OUT_HIGH,
		},
	},
	{ /* DEBUG_RX */
		.gpio      = 117,
		.settings = {
			[GPIOMUX_SUSPENDED] = CONSOLE_UART,
		},
	},
	{ /* DEBUG_TX */
		.gpio      = 118,
		.settings = {
			[GPIOMUX_SUSPENDED] = CONSOLE_UART,
		},
	},
};

#ifdef CONFIG_MSM8X60_AUDIO
static struct msm_gpiomux_config msm8x60_aux_pcm_configs[] __initdata = {
	{
		.gpio = 111,
		.settings = {
			[GPIOMUX_ACTIVE]    = &aux_pcm_active_config,
			[GPIOMUX_SUSPENDED] = &aux_pcm_suspend_config,
		},
	},
	{
		.gpio = 112,
		.settings = {
			[GPIOMUX_ACTIVE]    = &aux_pcm_active_config,
			[GPIOMUX_SUSPENDED] = &aux_pcm_suspend_config,
		},
	},
	{
		.gpio = 113,
		.settings = {
			[GPIOMUX_ACTIVE]    = &aux_pcm_active_config,
			[GPIOMUX_SUSPENDED] = &aux_pcm_suspend_config,
		},
	},
	{
		.gpio = 114,
		.settings = {
			[GPIOMUX_ACTIVE]    = &aux_pcm_active_config,
			[GPIOMUX_SUSPENDED] = &aux_pcm_suspend_config,
		},
	},
};
#endif

/* boot configuration: pins are already muxed */
/* TBD: review as gpiomux evolves */
static struct msm_gpiomux_config msm8x60_lcdc_configs[] = {
	/* lcdc_pclk */
	{
		.gpio = 0,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_hsync */
	{
		.gpio = 1,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_vsync */
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_den */
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red7 */
	{
		.gpio = 4,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_red6 */
	{
		.gpio = 5,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_red5 */
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red4 */
	{
		.gpio = 7,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red3 */
	{
		.gpio = 8,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red2 */
	{
		.gpio = 9,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red1 */
	{
		.gpio = 10,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red0 */
	{
		.gpio = 11,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn7 */
	{
		.gpio = 12,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_grn6 */
	{
		.gpio = 13,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_grn5 */
	{
		.gpio = 14,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn4 */
	{
		.gpio = 15,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn3 */
	{
		.gpio = 16,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn2 */
	{
		.gpio = 17,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn1 */
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn0 */
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu7 */
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_blu6 */
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_blu5 */
	{
		.gpio = 22,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu4 */
	{
		.gpio = 23,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu3 */
	{
		.gpio = 24,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu2 */
	{
		.gpio = 25,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu1 */
	{
		.gpio = 26,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu0 */
	{
		.gpio = 27,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
};

/* steady configuration */
/* TBD: review as gpiomux evolves */
static struct msm_gpiomux_config msm8x60_lcdc_steady_configs[] = {
	/* lcdc_pclk */
	{
		.gpio = 0,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_hsync */
	{
		.gpio = 1,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_vsync */
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_den */
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red7 */
	{
		.gpio = 4,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red6 */
	{
		.gpio = 5,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red5 */
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red4 */
	{
		.gpio = 7,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red3 */
	{
		.gpio = 8,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red2 */
	{
		.gpio = 9,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red1 */
	{
		.gpio = 10,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red0 */
	{
		.gpio = 11,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn7 */
	{
		.gpio = 12,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn6 */
	{
		.gpio = 13,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn5 */
	{
		.gpio = 14,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn4 */
	{
		.gpio = 15,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn3 */
	{
		.gpio = 16,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn2 */
	{
		.gpio = 17,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn1 */
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn0 */
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu7 */
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu6 */
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu5 */
	{
		.gpio = 22,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu4 */
	{
		.gpio = 23,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu3 */
	{
		.gpio = 24,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu2 */
	{
		.gpio = 25,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu1 */
	{
		.gpio = 26,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu0 */
	{
		.gpio = 27,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
};

static struct msm_gpiomux_config msm8x60_mdp_vsync_configs[] __initdata = {
	{
		.gpio = 28,
		.settings = {
			[GPIOMUX_ACTIVE]    = &mdp_vsync_active_cfg,
			[GPIOMUX_SUSPENDED] = &mdp_vsync_suspend_cfg,
		},
	},
};

/* Because PMIC drivers do not use gpio-management routines and PMIC
 * gpios must never sleep, a "good enough" config is obtained by placing
 * the active config in the 'suspended' slot and leaving the active
 * config invalid: the suspended config will be installed at boot
 * and never replaced.
 */
/*
 * Set the pmic gpio configuration as hw team recommended
 */
static struct msm_gpiomux_config msm8x60_pmic_configs[] __initdata = {
	{
		.gpio = 88,
		.settings = {
			[GPIOMUX_SUSPENDED] = &pmic_suspended_cfg,
		},
	},
	{
		.gpio = 91,
		.settings = {
			[GPIOMUX_SUSPENDED] = &pmic_suspended_cfg,
		},
	},
};

#ifdef CONFIG_WEBCAM_MT9M113
#define TENDERLOIN_CAM_I2C_DATA		47
#define TENDERLOIN_CAM_I2C_CLK		48
#define TENDERLOIN_CAMIF_MCLK		32
#define TENDERLOIN_WEBCAM_RST		106
#define TENDERLOIN_WEBCAM_PWDN		107

static struct msm_gpiomux_config msm8x60_cam_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_CAM_I2C_DATA,  CAM_F1_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_CAM_I2C_CLK, CAM_F1_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_CAMIF_MCLK,  CAM_F1_OUTL_8M_PN, GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_WEBCAM_RST, GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_WEBCAM_PWDN, GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
};
#endif


#define BT_RST_N_ACTIVE_CFG GPIO_OUTH_8M_PN
#define BT_RST_N_SUSPENDED_CFG GPIO_OUTH_2M_PN

#define BT_POWER_ACTIVE_CFG GPIO_OUTL_8M_PN
#define BT_POWER_SUSPENDED_CFG GPIO_IN_2M_PK

#define BT_WAKE_ACTIVE_CFG GPIO_OUTL_8M_PN
#define BT_WAKE_SUSPENDED_CFG GPIO_OUTL_2M_PN

#define BT_HOST_WAKE_ACTIVE_CFG GPIO_IN_8M_PU
#define BT_HOST_WAKE_SUSPENDED_CFG GPIO_IN_8M_PU

static struct msm_gpiomux_config msm8x60_bt_configs[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(BT_RST_N, \
			BT_RST_N_ACTIVE_CFG, BT_RST_N_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_POWER, \
			BT_POWER_ACTIVE_CFG, BT_POWER_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_WAKE, \
			BT_WAKE_ACTIVE_CFG, BT_WAKE_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_HOST_WAKE, \
			BT_HOST_WAKE_ACTIVE_CFG, BT_HOST_WAKE_SUSPENDED_CFG),
};


static struct msm_gpiomux_config msm8x60_bt_configs_3g[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(BT_RST_N_3G, \
			BT_RST_N_ACTIVE_CFG, BT_RST_N_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_POWER_3G, \
			BT_POWER_ACTIVE_CFG, BT_POWER_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_WAKE_3G, \
			BT_WAKE_ACTIVE_CFG, BT_WAKE_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_HOST_WAKE_3G, \
			BT_HOST_WAKE_ACTIVE_CFG, BT_HOST_WAKE_SUSPENDED_CFG),

};

#ifdef CONFIG_KEYBOARD_GPIO_PE
static struct msm_gpiomux_config msm8x60_kbdgpio_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(VOL_DN_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(VOL_UP_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(CORE_NAVI_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(JACK_DET_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};

static struct msm_gpiomux_config msm8x60_kbdgpio_cfgs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(CORE_NAVI_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};
#endif

#ifdef CONFIG_MAX8903B_CHARGER

static struct msm_gpiomux_config msm8x60_charger_cfgs[] __initdata = {
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_CHG_MODE,  GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_MODE, GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_SUS,  GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_1, GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_2, GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_EN,       GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_OK,        GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_STATUS_N,     GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_FAULT_N,      GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
};

static struct msm_gpiomux_config msm8x60_charger_cfgs_3g[] __initdata = {
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_CHG_MODE,  	GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_MODE_3G,	GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_SUS,  	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_1, 	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_2, 	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_EN,       	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_OK_3G,    	GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_STATUS_N,     	GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_FAULT_N,      	GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
};
#endif

#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
#define TOUCHSCREEN_GPIOCFG GPIOMUX_DCFG(\
	GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)

static struct msm_gpiomux_config msm8x60_touchscreen_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(72, TOUCHSCREEN_GPIOCFG, GPIO_IN_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(73, TOUCHSCREEN_GPIOCFG, GPIO_IN_2M_PN),
};
#endif

static struct msm_gpiomux_config msm8x60_lighting_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_INT_IRQ_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_EN_GPIO, GPIO_OUTL_8M_PN, GPIO_OUTL_2M_PN)
};

static struct msm_gpiomux_config msm8x60_lighting_cfgs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_INT_IRQ_GPIO_3G, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_EN_GPIO, GPIO_OUTL_8M_PN, GPIO_OUTL_2M_PN)
};


static struct msm_gpiomux_config msm8x60_sensor_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GYRO_INT,  GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GSENS_INT, GPIO_IN_2M_PN, GPIO_IN_2M_PN),
};

static struct msm_gpiomux_config msm8x60_sensor_cfgs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GYRO_INT_3G, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};

/* wifi */
static struct msm_gpiomux_config msm8x60_wlan_configs[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_WL_HOST_WAKE, \
			GPIO_IN_8M_PD, GPIO_IN_8M_PD),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_HOST_WAKE_WL, \
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_WLAN_RST_N, \
			GPIO_OUTL_8M_PN, GPIO_IN_2M_PK),
};


static struct msm_gpiomux_config msm8x60_wlan_configs_3g[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_WL_HOST_WAKE, \
			GPIO_IN_8M_PD, GPIO_IN_8M_PD),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_HOST_WAKE_WL_3G, \
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_HOST_WAKE_WL_3G, \
			GPIO_OUTL_8M_PN, GPIO_IN_2M_PK),

};

static struct msm_gpiomux_config msm8x60_system_gpio_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(GPIO_FORCE_BOOT_DIS, GPIO_OUTL_2M_PN, GPIO_OUTL_2M_PN),
};

#ifdef CONFIG_A6

// Currently the suspend configuration for A6 is not used
#define TENDERLOIN_A6_TCK_ACTIVE_CFG GPIO_OUTH_8M_PN
#define TENDERLOIN_A6_TCK_SUSPENDED_CFG GPIO_IN_2M_PU

#define TENDERLOIN_A6_WAKEUP_ACTIVE GPIO_OUTL_8M_PN
#define TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG GPIO_OUTL_2M_PN

#define TENDERLOIN_A6_TDIO_ACTIVE_CFG GPIO_OUTH_8M_PU
#define TENDERLOIN_A6_TDIO_SUSPENDED_CFG GPIO_IN_2M_PU

#define TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG GPIO_IN_8M_PU
#define TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG GPIO_IN_2M_PU

/* a6 */
static struct msm_gpiomux_config msm8x60_a6_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),
};


static struct msm_gpiomux_config msm8x60_a6_configs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK_3G, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ_3G, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK_3G, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ_3G, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

};
static struct msm_gpiomux_config msm8x60_a6_configs_dvt [] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),
};


static struct msm_gpiomux_config msm8x60_a6_configs_3g_dvt [] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK_3G_DVT, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ_3G_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK_3G, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ_3G_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

};
#endif

#ifdef CONFIG_MSM8X60_AUDIO
static struct msm_gpiomux_config msm8x60_aud_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_AUD_LDO1_EN, \
			GPIO_OUTL_8M_PN, GPIO_OUTH_2M_PN),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_AUD_LDO2_EN, \
			GPIO_OUTL_8M_PN, GPIO_OUTH_2M_PN),
};
#endif

static struct gpiomux_setting ctp_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
static struct msm_gpiomux_config ctp_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_RX,
                             LCDC_ACTIVE_CFG_2M,
                             GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CY8CTMA395_XRES,
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_WAKE,
			GPIO_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SCL,
                             &ctp_active_cfg,
                             GPIO_IN_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SDA,
                             &ctp_active_cfg,
                             GPIO_IN_2M_PN),
};
static struct gpiomux_setting ctp_active_cfg_3g =
  GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE)

static struct msm_gpiomux_config ctp_configs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_RX,
                             &lcdc_active_cfg_2m,
                             GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CY8CTMA395_XRES,
                             GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_WAKE_3G,
                             GPIO_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SCL,
                             &ctp_active_cfg_3g,
                             GPIO_IN_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SDA,
                             &ctp_active_cfg_3g,
                             GPIO_IN_2M_PN),
};
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */

/*
 *
 */
struct msm_gpiomux_configs
tenderloin_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs, ARRAY_SIZE(msm8x60_bt_configs)},
	{msm8x60_wlan_configs, ARRAY_SIZE(msm8x60_wlan_configs)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	{msm8x60_mdp_vsync_configs, ARRAY_SIZE(msm8x60_mdp_vsync_configs)},
	{msm8x60_sensor_cfgs, ARRAY_SIZE(msm8x60_sensor_cfgs)},
	{msm8x60_lighting_cfgs, ARRAY_SIZE(msm8x60_lighting_cfgs)},
#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs, ARRAY_SIZE(msm8x60_charger_cfgs)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs, ARRAY_SIZE(msm8x60_a6_configs)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif

	{msm8x60_gsbi_configs, ARRAY_SIZE(msm8x60_gsbi_configs)},
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},


#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs, ARRAY_SIZE(ctp_configs)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
	{NULL, 0},
};

struct msm_gpiomux_configs
tenderloin_3g_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs_3g, ARRAY_SIZE(msm8x60_bt_configs_3g)},
	{msm8x60_wlan_configs_3g, ARRAY_SIZE(msm8x60_wlan_configs_3g)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	//{msm8x60_mdp_vsync_configs, ARRAY_SIZE(msm8x60_mdp_vsync_configs)},
	{msm8x60_sensor_cfgs_3g, ARRAY_SIZE(msm8x60_sensor_cfgs_3g)},
	{msm8x60_lighting_cfgs_3g, ARRAY_SIZE(msm8x60_lighting_cfgs_3g)},

#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs_3g, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs_3g, ARRAY_SIZE(msm8x60_charger_cfgs_3g)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs_3g, ARRAY_SIZE(msm8x60_a6_configs)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif
#ifdef CONFIG_USB_PEHCI_HCD
	{msm8x60_isp_usb_configs, ARRAY_SIZE(msm8x60_isp_usb_configs)},
	{msm8x60_ebi2_configs, ARRAY_SIZE(msm8x60_ebi2_configs)},
#endif
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},

#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs_3g, ARRAY_SIZE(ctp_configs_3g)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */

	{NULL, 0},
};

struct msm_gpiomux_configs
tenderloin_dvt_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs, ARRAY_SIZE(msm8x60_bt_configs)},
	{msm8x60_wlan_configs, ARRAY_SIZE(msm8x60_wlan_configs)},
        {msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
        {msm8x60_mdp_vsync_configs, ARRAY_SIZE(msm8x60_mdp_vsync_configs)},
	{msm8x60_sensor_cfgs, ARRAY_SIZE(msm8x60_sensor_cfgs)},
	{msm8x60_lighting_cfgs, ARRAY_SIZE(msm8x60_lighting_cfgs)},
#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs, ARRAY_SIZE(msm8x60_charger_cfgs)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs_dvt, ARRAY_SIZE(msm8x60_a6_configs_dvt)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},

#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs, ARRAY_SIZE(ctp_configs)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
	{NULL, 0},
};

struct msm_gpiomux_configs
tenderloin_3g_dvt_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs_3g, ARRAY_SIZE(msm8x60_bt_configs_3g)},
	{msm8x60_wlan_configs_3g, ARRAY_SIZE(msm8x60_wlan_configs_3g)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	//{msm8x60_mdp_vsync_configs, ARRAY_SIZE(msm8x60_mdp_vsync_configs)},
	{msm8x60_sensor_cfgs_3g, ARRAY_SIZE(msm8x60_sensor_cfgs_3g)},
	{msm8x60_lighting_cfgs_3g, ARRAY_SIZE(msm8x60_lighting_cfgs_3g)},

#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs_3g, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs_3g, ARRAY_SIZE(msm8x60_charger_cfgs_3g)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs_3g_dvt, ARRAY_SIZE(msm8x60_a6_configs_3g_dvt)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif
#ifdef CONFIG_USB_PEHCI_HCD
	{msm8x60_isp_usb_configs, ARRAY_SIZE(msm8x60_isp_usb_configs)},
	{msm8x60_ebi2_configs, ARRAY_SIZE(msm8x60_ebi2_configs)},
#endif

	{msm8x60_gsbi_configs, ARRAY_SIZE(msm8x60_gsbi_configs)},
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},

#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs_3g, ARRAY_SIZE(ctp_configs_3g)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
	{NULL, 0},
};

/* HP_Hover[20101105][Workaround]Add special gpio request interface here for lcdc,
 * to keep the related gpio's reference counter balenced, since we set the lcdc
 * active when bootup(ref=1).
 */
int lcdc_gpio_request(bool on)
{
	int n, ret=0;
	static bool first_time = true;

for (n = 0; n < ARRAY_SIZE(msm8x60_lcdc_configs); n++) {
		if (on) {
			ret = gpio_request(msm8x60_lcdc_configs[n].gpio, "LCDC_GPIO");
			if (unlikely(ret)) {
				printk("%s not able to get gpio\n", __func__);
				break;
			}
			if (first_time) {
				msm_gpiomux_put(msm8x60_lcdc_configs[n].gpio);
			}
		} else {
			gpio_free(msm8x60_lcdc_configs[n].gpio);
		}
	}

	if (ret) {
		for (n--; n >= 0; n--)
			gpio_free(msm8x60_lcdc_configs[n].gpio);
	}
	else {
		first_time = false;
	}
	return ret;

}

static int lcdc_gpio_get(void)
{
	int n,ret=0;
	for (n = 0; n < ARRAY_SIZE(msm8x60_lcdc_configs); n++) {
		ret = msm_gpiomux_get(msm8x60_lcdc_configs[n].gpio);
		if(unlikely(ret)){
			printk("%s not able to get gpio\n", __func__);
			break;
		}
	}
	return ret;
}


void __init tenderloin_init_gpiomux(void)
{
	int rc;
        struct msm_gpiomux_configs *cfgs;

	rc = msm_gpiomux_init(NR_GPIO_IRQS);
	if (rc) {
		pr_err("%s failure: %d\n", __func__, rc);
		return;
	}

	if(boardtype_is_3g()) {
		/* 3G EVT4 boards use the same gpiomux cfg as DVT */
		if (board_type >= TOPAZ_3G_EVT4) {
                  printk(KERN_ERR "%s: >= TOPAZ_3G_EVT4\n", __func__);
                        cfgs = tenderloin_3g_dvt_gpiomux_cfgs;
		} else {
                  printk(KERN_ERR "%s: < TOPAZ_3G_EVT4\n", __func__);
                        cfgs = tenderloin_3g_gpiomux_cfgs;
		}

	} else {
		if (board_type >= TOPAZ_DVT) {
                  printk(KERN_ERR "%s: >= TOPAZ_DVT\n", __func__);
			cfgs = tenderloin_dvt_gpiomux_cfgs;
		} else {
                  printk(KERN_ERR "%s: < TOPAZ_DVT\n", __func__);
			cfgs = tenderloin_gpiomux_cfgs;
		}
	}
	rc = lcdc_gpio_get();
	if (rc) {
		pr_err("%s failure: lcdc_gpio_get: %d\n", __func__, rc);
		return;
	}
	while (cfgs->cfg) {
		msm_gpiomux_install(cfgs->cfg, cfgs->ncfg);
		++cfgs;
	}
}

void tenderloin_lcdc_steadycfg(void)
{
	msm_gpiomux_install(msm8x60_lcdc_steady_configs,
			ARRAY_SIZE(msm8x60_lcdc_steady_configs));
}
