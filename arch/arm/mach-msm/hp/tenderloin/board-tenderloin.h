/* linux/arch/arm/mach-msm/board-spade.h
 *
 * Copyright (C) 2010-2011 HTC Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_ARM_MACH_MSM_BOARD_TENDERLOIN_H
#define __ARCH_ARM_MACH_MSM_BOARD_TENDERLOIN_H

#include <mach/board.h>
#include <mach/msm_memtypes.h>
#include <mach/rpm-regulator.h>
#include <linux/regulator/pmic8901-regulator.h>
#include "board-tenderloin-pins.h"
#include <mach/board-msm8660.h>

#define TENDERLOIN_PROJECT_NAME	"tenderloin"

#define MSM_RAM_CONSOLE_BASE	MSM_HTC_RAM_CONSOLE_PHYS
#define MSM_RAM_CONSOLE_SIZE	MSM_HTC_RAM_CONSOLE_SIZE

/* Memory map */

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
#define QCE_SIZE		0x10000
#define QCE_0_BASE		0x18500000
#endif

#define PHY_BASE_ADDR1  0x40200000
#define SIZE_ADDR1      0x25600000

/* Macros assume PMIC GPIOs start at 0 */
#define PM8058_GPIO_BASE			NR_MSM_GPIOS
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_GPIO_BASE)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)		(sys_gpio - PM8058_GPIO_BASE)
#define PM8058_MPP_BASE				(PM8058_GPIO_BASE + PM8058_GPIOS)
#define PM8058_MPP_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8058_MPP_BASE)
#define PM8058_MPP_SYS_TO_PM(sys_gpio)		(sys_gpio - PM8058_MPP_BASE)
#define PM8058_IRQ_BASE				(NR_MSM_IRQS + NR_GPIO_IRQS)

#define PM8901_GPIO_BASE			(PM8058_GPIO_BASE + \
						PM8058_GPIOS + PM8058_MPPS)
#define PM8901_GPIO_PM_TO_SYS(pm_gpio)		(pm_gpio + PM8901_GPIO_BASE)
#define PM8901_GPIO_SYS_TO_PM(sys_gpio)		(sys_gpio - PM901_GPIO_BASE)
#define PM8901_IRQ_BASE				(PM8058_IRQ_BASE + \
						NR_PMIC8058_IRQS)
#define GPIO_LCD_TE	28

#define UART1DM_RTS_GPIO		56
#define UART1DM_CTS_GPIO		55
#define UART1DM_RX_GPIO			54
#define UART1DM_TX_GPIO			53

#define BT_RST_N			138
#define BT_POWER			130
#define BT_WAKE				131
#define BT_HOST_WAKE			129

#define BT_RST_N_3G			122
#define BT_POWER_3G			110
#define BT_WAKE_3G			131
#define BT_HOST_WAKE_3G			50

//#ifdef CONFIG_KEYBOARD_GPIO_PE
/* GPIO Keys */
#define CORE_NAVI_GPIO			40
#define VOL_UP_GPIO			103
#define VOL_DN_GPIO			104
#define VOL_UP_GPIO_3G			(PM8058_GPIO_PM_TO_SYS(6-1))
#define VOL_DN_GPIO_3G			(PM8058_GPIO_PM_TO_SYS(7-1))
#define JACK_DET_GPIO		67
//#endif

#if defined(CONFIG_MAX8903B_CHARGER) || defined(CONFIG_CHARGER_MAX8903)
/* max8903b control GPIOs */
#define MAX8903B_GPIO_DC_CHG_MODE	42
#define MAX8903B_GPIO_USB_CHG_MODE	133
#define MAX8903B_GPIO_USB_CHG_MODE_3G	134
#define MAX8903B_GPIO_USB_CHG_SUS	33
#define MAX8903B_GPIO_CHG_D_ISET_1	34
#define MAX8903B_GPIO_CHG_D_ISET_2	30
#define MAX8903B_GPIO_CHG_EN		41
#define MAX8903B_GPIO_DC_OK		140
#define MAX8903B_GPIO_DC_OK_3G		86
#define MAX8903B_GPIO_STATUS_N		36
#define MAX8903B_GPIO_FAULT_N		35
#endif  //CONFIG_MAX8903B_CHARGER

#define MXT1386_TS_PEN_IRQ_GPIO		123
#define MXT1386_TS_PEN_IRQ_GPIO_3G	45
#define MXT1386_TS_PWR_RST_GPIO		70

#define GPIO_CTP_WAKE			123
#define GPIO_CTP_WAKE_3G		45
#define GPIO_CTP_SCL			73
#define GPIO_CTP_SDA			72
#define GPIO_CTP_RX			71
#define GPIO_CY8CTMA395_XRES		70

#define TENDERLOIN_GPIO_WL_HOST_WAKE	93
#define TENDERLOIN_GPIO_HOST_WAKE_WL	137
#define TENDERLOIN_GPIO_HOST_WAKE_WL_3G	80
#define TENDERLOIN_GPIO_WLAN_RST_N	135
#define TENDERLOIN_GPIO_WLAN_RST_N_3G	28

/* a6 */
#define TENDERLOIN_A6_0_TCK		157
#define TENDERLOIN_A6_0_WAKEUP		155
#define TENDERLOIN_A6_0_TDIO		158
#define TENDERLOIN_A6_0_MSM_IRQ		156
#define TENDERLOIN_A6_0_MSM_IRQ_DVT	37

#define TENDERLOIN_A6_1_TCK		115
#define TENDERLOIN_A6_1_WAKEUP		141
#define TENDERLOIN_A6_1_TDIO		116
#define TENDERLOIN_A6_1_MSM_IRQ		132
#define TENDERLOIN_A6_1_MSM_IRQ_DVT	94


#define TENDERLOIN_A6_0_TCK_3G		68
#define TENDERLOIN_A6_0_TCK_3G_DVT	156
#define TENDERLOIN_A6_0_WAKEUP_3G	155
#define TENDERLOIN_A6_0_TDIO_3G		170
#define TENDERLOIN_A6_0_MSM_IRQ_3G	156
#define TENDERLOIN_A6_0_MSM_IRQ_3G_DVT	37

#define TENDERLOIN_A6_1_TCK_3G		115
#define TENDERLOIN_A6_1_WAKEUP_3G	78
#define TENDERLOIN_A6_1_TDIO_3G		116
#define TENDERLOIN_A6_1_MSM_IRQ_3G	132
#define TENDERLOIN_A6_1_MSM_IRQ_3G_DVT	94

/* camera */
#define TENDERLOIN_CAM_I2C_DATA		47
#define TENDERLOIN_CAM_I2C_CLK		48
#define TENDERLOIN_CAMIF_MCLK		32
#define TENDERLOIN_WEBCAM_RST		106
#define TENDERLOIN_WEBCAM_PWDN		107

/* light sensor */
#define TENDERLOIN_LS_INT       126

/* gyro */
#define TENDERLOIN_GYRO_INT		125
#define TENDERLOIN_GYRO_INT_3G		75

/* G-sensor */
#define TENDERLOIN_GSENS_INT		124


/* audio */
#define TENDERLOIN_AUD_HEAD_MIC_DET_IRQ_GPIO 57
#define TENDERLOIN_AUD_LDO1_EN		        66
#define TENDERLOIN_AUD_LDO2_EN		        108

/* lighting */
#define LM8502_LIGHTING_INT_IRQ_GPIO 	128
#define LM8502_LIGHTING_EN_GPIO 	121
#define LM8502_LIGHTING_INT_IRQ_GPIO_3G	77

/* usb host */
#define ISP1763_INT_GPIO		172
#define ISP1763_DACK_GPIO		169
#define ISP1763_DREQ_GPIO		29
#define ISP1763_RST_GPIO		152
#define GPIO_3G_3V3_EN			82
#define GPIO_3G_3V3_EN_DVT		106
#define GPIO_3G_DISABLE_N		171
#define GPIO_3G_WAKE_N			38
#define GPIO_3G_UIM_CD_N		61

/* pmic */
#define PMIC1_APC_USR_IRQ_N		88
#define PMIC2_APC_USR_IRQ_N		91

/* sensors i2c gpio */
#define GPIO_SENSORS_SDA		43
#define GPIO_SENSORS_SCL		44
#define I2C_RECOVER_CLOCK_CYCLES	36

/*System force boot dis pin*/
#define GPIO_FORCE_BOOT_DIS		154

extern struct rpm_regulator_platform_data tenderloin_rpm_regulator_pdata __devinitdata;
extern struct rpm_regulator_platform_data tenderloin_rpm_regulator_early_pdata __devinitdata;
extern struct pm8901_vreg_pdata pm8901_regulator_pdata[];
extern struct platform_device tenderloin_8901_mpp_vreg __devinitdata;
extern int pm8901_regulator_pdata_len;
extern struct platform_device msm_adc_device;
extern u32 board_type;
enum topaz_board_types {
	TOPAZ_PROTO = 0,
	TOPAZ_PROTO2,
	TOPAZ_EVT1,
	TOPAZ_EVT2,
	TOPAZ_EVT3,
	TOPAZ_DVT,
	TOPAZ_PVT,
	TOPAZ_3G_PROTO,
	TOPAZ_3G_PROTO2,
	TOPAZ_3G_EVT1,
	TOPAZ_3G_EVT2,
	TOPAZ_3G_EVT3,
	TOPAZ_3G_EVT4,
	TOPAZ_3G_DVT,
	TOPAZ_3G_PVT
};

int __init tenderloin_init_mmc(void);
void __init tenderloin_audio_init(void);
int __init tenderloin_init_keypad(void);
int __init tenderloin_init_wifi(void);
void __init tenderloin_init_fb(void);
void __init tenderloin_init_pmic(void);
int __init tenderloin_wifi_init(void);
void __init tenderloin_gpio_mpp_init(void);
void __init tenderloin_init_gpiomux(void);
void __init tenderloin_pm8901_gpio_mpp_init(void);
void tenderloin_lcdc_steadycfg(void);
void __init msm8x60_allocate_fb_region(void);
void __init tenderloin_init_ts(void);
void __init tenderloin_usb_i2c_init(void);
void __init tenderloin_usb_init(void);
void msm8x60_gpiomux_lcdc_steadycfg(void);
void __init tenderloin_init_a6(void);
void __init msm8x60_init_mmc(void);

static inline u8 boardtype_is_3g(void)
{
  return (board_type >= TOPAZ_3G_PROTO);
}

#define _GET_REGULATOR(var, name) do {              \
    var = regulator_get(NULL, name);            \
    if (IS_ERR(var)) {                  \
        panic("'%s' regulator not found, rc=%ld\n",    \
            name, IS_ERR(var));         \
    }                           \
} while (0)

#endif /* __ARCH_ARM_MACH_MSM_BOARD_TENDERLOIN_H */
