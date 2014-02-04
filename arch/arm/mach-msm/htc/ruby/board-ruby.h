/* linux/arch/arm/mach-msm/board-ruby.h
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

#ifndef __ARCH_ARM_MACH_MSM_BOARD_RUBY_H
#define __ARCH_ARM_MACH_MSM_BOARD_RUBY_H

#include <mach/board.h>
#include <mach/msm_memtypes.h>
#include <mach/rpm-regulator.h>
#include <linux/regulator/pmic8901-regulator.h>
#include <mach/board-msm8660.h>

#define RUBY_PROJECT_NAME	"ruby"

#define MSM_RAM_CONSOLE_BASE	MSM_HTC_RAM_CONSOLE_PHYS
#define MSM_RAM_CONSOLE_SIZE	MSM_HTC_RAM_CONSOLE_SIZE

/* Memory map */
#define MSM_SMI_BASE         0x38000000
#define MSM_SMI_SIZE         0x4000000

#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_PRIM_BUF_SIZE (960 * 544 * 4 * 3) /* 4 bpp x 3 pages */
#else
#define MSM_FB_PRIM_BUF_SIZE (960 * 544 * 4 * 2) /* 4 bpp x 2 pages */
#endif

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
#define MSM_FB_EXT_BUF_SIZE (1920 * 1088 * 2 * 1) /* 2 bpp x 1 page */
#elif defined(CONFIG_FB_MSM_TVOUT)
#define MSM_FB_EXT_BUF_SIZE (720 * 576 * 2 * 2) /* 2 bpp x 2 pages */
#else
#define MSM_FB_EXT_BUF_SIZE 0
#endif

/* Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE + MSM_FB_EXT_BUF_SIZE, 4096)

#ifdef CONFIG_FB_MSM_OVERLAY0_WRITEBACK
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((960 * 544 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE (0)
#endif  /* CONFIG_FB_MSM_OVERLAY0_WRITEBACK */

#ifdef CONFIG_FB_MSM_OVERLAY1_WRITEBACK
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE roundup((1920 * 1088 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE (0)
#endif  /* CONFIG_FB_MSM_OVERLAY1_WRITEBACK */

#define PHY_BASE_ADDR1       0x48000000
#define SIZE_ADDR1           0x28000000

#define MSM_ION_MM_FW_SIZE   0x200000
#define MSM_ION_MM_SIZE      0x3D00000
#define MSM_ION_MFC_SIZE     0x100000
#define MSM_ION_SF_SIZE      0x3000000
#define MSM_ION_WB_SIZE      0x600000
#define MSM_ION_CAMERA_SIZE  0x2400000
#define MSM_ION_AUDIO_SIZE   0x4CF000

#define MSM_ION_HEAP_NUM     8

#define MSM_ION_MM_FW_BASE   0x38000000
#define MSM_ION_MM_BASE      0x38200000
#define MSM_ION_MFC_BASE     0x3BF00000
#define MSM_ION_CAMERA_BASE  0x40400000
#define MSM_ION_WB_BASE      0x42800000
#define MSM_ION_SF_BASE      0x49800000

/*Gyro*/
#define RUBY_GPIO_PANA_GYRO_SLEEP	(70)
#define RUBY_GPIO_GYRO_ID		(130)
#define RUBY_GPIO_GYRO_DIAG		(41)

/* Direct Keys */
#define RUBY_GPIO_KEY_VOL_DOWN		(103)
#define RUBY_GPIO_KEY_VOL_UP		(104)
#define RUBY_GPIO_KEY_POWER		(125)
#define RUBY_GPIO_KEY_CAPTURE		(94)

/* Wifi */
#define RUBY_GPIO_WIFI_IRQ              (46)
#define RUBY_GPIO_WIFI_SHUTDOWN_N       (62)

/* Sensors */
#define RUBY_GPIO_SENSOR_I2C_SCL	(115)
#define RUBY_GPIO_SENSOR_I2C_SDA	(116)
#define RUBY_GPIO_GYRO_INT		(126)
#define RUBY_GPIO_COMPASS_INT		(128)
#define RUBY_GPIO_GSENSOR_INT_N		(127)

#define RUBY_LAYOUTS			{ \
			{ { 0,  1, 0}, {-1,  0,  0}, {0, 0, 1} }, \
			{ { 0, -1, 0}, { 1,  0,  0}, {0, 0, -1} }, \
			{ {-1,  0, 0}, { 0, -1,  0}, {0, 0,  1} }, \
			{ {-1,  0, 0}, { 0,  0, -1}, {0, 1,  0} }   \
				}
/* General */
#define RUBY_GENERAL_I2C_SCL		(59)
#define RUBY_GENERAL_I2C_SDA		(60)

/* TP */
#define RUBY_TP_I2C_SDA			(51)
#define RUBY_TP_I2C_SCL			(52)
#define RUBY_TP_ATT_N			(117)

/* LCD */
#define GPIO_LCD_TE			(28)
#define GPIO_LCM_RST_N			(137)
#define GPIO_LCM_ID0			(64)
#define GPIO_LCM_ID1			(65)

/* Audio */
#define RUBY_GPIO_AUD_HP_DET		(31)
#define RUBY_AUD_CODEC_RST		(118)
#define RUBY_AUD_QTR_RESET		(158)

/* Battery */
#define RUBY_GPIO_MBAT_IN		(61)
#define RUBY_GPIO_CHG_INT		(124)

/* BT */
#define RUBY_GPIO_BT_UART1_TX		(53)
#define RUBY_GPIO_BT_UART1_RX		(54)
#define RUBY_GPIO_BT_UART1_CTS		(55)
#define RUBY_GPIO_BT_UART1_RTS		(56)
#define RUBY_GPIO_BT_EN			(142)

/* USB and UART */
#define RUBY_GPIO_UART_RX		(105)
#define RUBY_GPIO_UART_TX		(106)

/* Cable detect */
#define RUBY_GPIO_USB_ID		(63)

/* Camera */
#define RUBY_CAM_I2C_SDA		(47)
#define RUBY_CAM_I2C_SCL		(48)
#define RUBY_CLK_SWITCH			(18)
#define RUBY_CAM1_RST			(49)
#define RUBY_CAM1_VCM_PD		(19)
#define RUBY_CAM2_RST			(101)
#define RUBY_CAM1_CAM_ID		(157)

/* Flashlight */
#define RUBY_FLASH_EN			(138)
#define RUBY_TORCH_EN			(30)

/* MHL */
#define RUBY_GPIO_MHL_WAKE_UP		(0)
#define RUBY_GPIO_MHL_USB_SWITCH	(1)
#define RUBY_GPIO_MHL_RESET		(2)
#define RUBY_GPIO_MHL_INT  		(50)

/* SPI */
#define RUBY_SPI_DO			(33)
#define RUBY_SPI_DI			(34)
#define RUBY_SPI_CS			(35)
#define RUBY_SPI_CLK			(36)

/* SD */
#define RUBY_SD_DETECT_PIN		(37)

/* NFC */
#define RUBY_NFC_I2C_SDA		(43)
#define RUBY_NFC_I2C_SCL		(44)
#define RUBY_GPIO_NFC_VEN		(20)
#define RUBY_GPIO_NFC_INT		(58)
#define RUBY_GPIO_NFC_DL		(21)

/* others */
#define RUBY_PSENSOR_INTz		(123)
#define RUBY_REGION_ID			(39)

/* PMIC GPIO definition */
#define PMGPIO(x) (x-1)
#define RUBY_GPIO_KEY_CAMCODER		PMGPIO(22)
#define RUBY_GPIO_KEY_CAMAF		PMGPIO(35)

#define RUBY_LED_3V3			PMGPIO(24)
#define RUBY_AUD_HANDSET_ENO		PMGPIO(18)
#define RUBY_AUD_MIC_SEL		PMGPIO(37)
#define RUBY_WIFI_BT_FAST_CLK		PMGPIO(8)
#define RUBY_WIFI_BT_SLEEP_CLK		PMGPIO(38)
#define RUBY_TP_RST			PMGPIO(23)
#define RUBY_CHG_STAT			PMGPIO(33)
#define RUBY_AUD_REMO_PRES		PMGPIO(7)
#define RUBY_H2W_CABLE_IN1		PMGPIO(36)
#define RUBY_H2W_CABLE_IN2		PMGPIO(9)
#define RUBY_H2W_IO1_CLK		PMGPIO(34)
#define RUBY_H2W_IO2_DAT		PMGPIO(10)
#define RUBY_PSENSOR_PVT_INTz		PMGPIO(15)

extern struct regulator_init_data msm_saw_regulator_pdata_s0;
extern struct regulator_init_data msm_saw_regulator_pdata_s1;
extern struct rpm_regulator_platform_data ruby_rpm_regulator_early_pdata __devinitdata;
extern struct rpm_regulator_platform_data ruby_rpm_regulator_pdata __devinitdata;
extern struct platform_device msm8x60_8901_mpp_vreg __devinitdata;
extern struct pm8901_vreg_pdata pm8901_regulator_pdata[];
extern int pm8901_regulator_pdata_len;
extern struct platform_device msm_adc_device;

int ruby_bluetooth_set_power(int on);
void ruby_headset_pmic_init(void);
void __init ruby_init_cam(void);
void __init ruby_init_mmc(void);
int __init ruby_init_wifi_mmc(void);
void __init ruby_audio_init(void);
int __init ruby_init_keypad(void);
void __init ruby_init_fb(void);
void __init ruby_init_pmic(void);
int __init ruby_wifi_init(void);
void __init ruby_gpio_mpp_init(void);
void __init ruby_init_gpiomux(void);
void __init msm8x60_allocate_fb_region(void);
void __init ruby_pm8901_gpio_mpp_init(void);
void msm8x60_mdp_writeback(struct memtype_reserve *reserve_table);
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
int hdmi_enable_5v(int on);
#endif
#define _GET_REGULATOR(var, name) do {					\
	if (var == NULL) {						\
		var = regulator_get(NULL, name);			\
		if (IS_ERR(var)) {					\
			pr_err("'%s' regulator not found, rc=%ld\n",	\
				name, PTR_ERR(var));			\
			var = NULL;					\
		}							\
	}								\
} while (0)

#endif /* __ARCH_ARM_MACH_MSM_BOARD_RUBY_H */
