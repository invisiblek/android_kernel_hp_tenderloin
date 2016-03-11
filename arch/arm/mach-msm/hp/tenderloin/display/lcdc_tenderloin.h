#ifndef __LCDC_TENDERLOIN_H__
#define __LCDC_TENDERLOIN_H__


#define GPIO_BACKLIGHT_PWM0 0
#define GPIO_BACKLIGHT_PWM1 1

/**
 * TPS61187's max PWM freq allowance is 22Khz.
 * If PWM_LEVEL is greater than 64 and If we use PMIC8058-PWM,
 * PMIC must be in 9bit modulation mode.
 */
#define PWM_FREQ_HZ 20000
#define PWM_PERIOD_USEC (USEC_PER_SEC / PWM_FREQ_HZ)

// prevent pwm level 3 and under to prevent cut-out
#define PWM_LEVEL_MIN 3
#define PWM_LEVEL_MAX 256

#endif /* !__LCDC_TENDERLOIN_H__ */
