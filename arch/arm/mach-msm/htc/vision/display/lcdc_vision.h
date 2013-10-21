#ifndef __LCDC_VISION__
#define __LCDC_VISION__

#define SONY_PANEL              0x1             /*Set bit 0 as 1 when it is SONY PANEL*/
#define SONY_PWM_SPI            0x2             /*Set bit 1 as 1 as PWM_SPI mode, otherwise it is PWM_MICROP mode*/
#define SONY_GAMMA              0x4             /*Set bit 2 as 1 when panel contains GAMMA table in its NVM*/
#define SONY_RGB666             0x8             /*Set bit 3 as 1 when panel is 18 bit, otherwise it is 16 bit*/
#define SONY_PANEL_SPI           (SONY_PANEL | SONY_PWM_SPI | SONY_GAMMA | SONY_RGB666)

#define SONYWVGA_MIN_VAL		10
#define SONYWVGA_MAX_VAL		250
#define SONYWVGA_DEFAULT_VAL	(SONYWVGA_MIN_VAL +		\
					 (SONYWVGA_MAX_VAL -	\
					  SONYWVGA_MIN_VAL) / 2)

#define SONYWVGA_BR_DEF_USER_PWM         143
#define SONYWVGA_BR_MIN_USER_PWM         30
#define SONYWVGA_BR_MAX_USER_PWM         255
#define SONYWVGA_BR_DEF_PANEL_PWM        128
#define SONYWVGA_BR_MIN_PANEL_PWM        8
#define SONYWVGA_BR_MAX_PANEL_PWM        255
#define SONYWVGA_BR_DEF_PANEL_UP_PWM    132
#define SONYWVGA_BR_MIN_PANEL_UP_PWM    9
#define SONYWVGA_BR_MAX_PANEL_UP_PWM    255

#endif /* !__LCDC_VISION__ */


