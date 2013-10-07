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

#define PHY_BASE_ADDR1  0x48000000
#define SIZE_ADDR1      0x25600000

/* GPIO definition */

/* Direct Keys */
#define TENDERLOIN_GPIO_KEY_POWER          (125)

/* Battery */
#define TENDERLOIN_GPIO_MBAT_IN            (61)
#define TENDERLOIN_GPIO_CHG_INT		(126)

/* Wifi */
#define TENDERLOIN_GPIO_WIFI_IRQ              (46)
#define TENDERLOIN_GPIO_WIFI_SHUTDOWN_N       (96)
/* Sensors */
#define TENDERLOIN_SENSOR_I2C_SDA		(72)
#define TENDERLOIN_SENSOR_I2C_SCL		(73)
#define TENDERLOIN_GYRO_INT               (127)
#define TENDERLOIN_ECOMPASS_INT           (128)
#define TENDERLOIN_GSENSOR_INT           (129)

/* Microp */

/* TP */
#define TENDERLOIN_TP_I2C_SDA           (51)
#define TENDERLOIN_TP_I2C_SCL           (52)
#define TENDERLOIN_TP_ATT_N             (65)
#define TENDERLOIN_TP_ATT_N_XB       (50)

/* LCD */
#define GPIO_LCD_TE	28
#define GPIO_LCM_RST_N			(66)
#define GPIO_LCM_ID			(50)

/* Audio */
#define TENDERLOIN_AUD_CODEC_RST        (67)

/* BT */
#define TENDERLOIN_GPIO_BT_HOST_WAKE      (45)
#define TENDERLOIN_GPIO_BT_UART1_TX       (53)
#define TENDERLOIN_GPIO_BT_UART1_RX       (54)
#define TENDERLOIN_GPIO_BT_UART1_CTS      (55)
#define TENDERLOIN_GPIO_BT_UART1_RTS      (56)
#define TENDERLOIN_GPIO_BT_SHUTDOWN_N     (100)
#define TENDERLOIN_GPIO_BT_CHIP_WAKE      (130)
#define TENDERLOIN_GPIO_BT_RESET_N        (142)

/* USB */
#define TENDERLOIN_GPIO_USB_ID        (63)
#define TENDERLOIN_GPIO_MHL_RESET        (70)
#define TENDERLOIN_GPIO_MHL_INT        (71)
#define TENDERLOIN_GPIO_MHL_USB_SWITCH        (99)

/* Camera */
#define TENDERLOIN_CAM_CAM1_ID           (10)
#define TENDERLOIN_CAM_I2C_SDA           (47)
#define TENDERLOIN_CAM_I2C_SCL           (48)

/* General */
#define TENDERLOIN_GENERAL_I2C_SDA		(59)
#define TENDERLOIN_GENERAL_I2C_SCL		(60)

/* Flashlight */
#define TENDERLOIN_FLASH_EN             (29)
#define TENDERLOIN_FLASH_TORCH             (30)

/* Accessory */
#define TENDERLOIN_GPIO_AUD_HP_DET        (31)

/* SPI */
#define TENDERLOIN_SPI_DO                 (33)
#define TENDERLOIN_SPI_DI                 (34)
#define TENDERLOIN_SPI_CS                 (35)
#define TENDERLOIN_SPI_CLK                (36)

/* PMIC */

/* PMIC GPIO definition */
#define PMGPIO(x) (x-1)
#define TENDERLOIN_VOL_UP             PMGPIO(16)
#define TENDERLOIN_VOL_DN             PMGPIO(17)
#define TENDERLOIN_AUD_HP_EN          PMGPIO(18)
#define TENDERLOIN_HAP_ENABLE         PMGPIO(19)
#define TENDERLOIN_AUD_QTR_RESET      PMGPIO(21)
#define TENDERLOIN_TP_RST             PMGPIO(23)
#define TENDERLOIN_GREEN_LED          PMGPIO(24)
#define TENDERLOIN_AMBER_LED          PMGPIO(25)
#define TENDERLOIN_AUD_MIC_SEL        PMGPIO(26)
#define TENDERLOIN_CHG_STAT	   PMGPIO(33)
#define TENDERLOIN_SDC3_DET           PMGPIO(34)
#define TENDERLOIN_PLS_INT            PMGPIO(35)
#define TENDERLOIN_AUD_REMO_PRES      PMGPIO(37)
#define TENDERLOIN_WIFI_BT_SLEEP_CLK  PMGPIO(38)

extern struct rpm_regulator_platform_data tenderloin_rpm_regulator_pdata __devinitdata;
extern struct rpm_regulator_platform_data tenderloin_rpm_regulator_early_pdata __devinitdata;
extern struct pm8901_vreg_pdata pm8901_regulator_pdata[];
extern int pm8901_regulator_pdata_len;
extern struct platform_device msm_adc_device;

int __init tenderloin_init_mmc(void);
void __init tenderloin_audio_init(void);
int __init tenderloin_init_keypad(void);
void __init tenderloin_init_fb(void);
void __init tenderloin_init_pmic(void);
int __init tenderloin_wifi_init(void);
void __init tenderloin_gpio_mpp_init(void);
void tenderloin_init_gpiomux(void);
void __init msm8x60_allocate_fb_region(void);
void msm8x60_mdp_writeback(struct memtype_reserve *reserve_table);

#endif /* __ARCH_ARM_MACH_MSM_BOARD_TENDERLOIN_H */
