#include <linux/gpio.h>
#include <linux/msm-charger.h>
#include <mach/mpp.h>
#include <mach/msm_hsusb.h>
#include <mach/socinfo.h>
#include <linux/mfd/pmic8058.h>
#include <linux/i2c.h>
#ifdef CONFIG_A6
#include <linux/a6.h>
#endif
#include "devices.h"
#include "devices-msm8x60.h"
#include "board-tenderloin.h"
#include <mach/gpiomux.h>
#include <mach/mpm.h>
#ifdef CONFIG_A6
#include <linux/a6_sbw_interface.h>
#include <linux/a6.h>
#include "a6_sbw_impl_tenderloin.h"
#endif

/* helper function to manipulate group of gpios (msm_gpiomux)*/
static int configure_gpiomux_gpios(int on, int gpios[], int cnt)
{
	int ret = 0;
	int i;

	for (i = 0; i < cnt; i++) {
		//printk(KERN_ERR "%s:pin(%d):%s\n", __func__, gpios[i], on?"on":"off");
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

#ifdef CONFIG_A6
/* The following #defines facilitate selective inclusion of a specific a6 wakeup strategy:
   [constraint]: A6_PMIC_EXTERNAL_WAKE and A6_INTERNAL_WAKE are mutually exclusive
   A6_PMIC_EXTERNAL_WAKE: configures a6 driver to use PMIC-based LPG PWM for wakeup
   A6_INTERNAL_WAKE: configures for a6-based internal wake
   if neither defined: configures A6 driver to keep a6 constantly awake using SBW_WAKEUP pin
*/
#define A6_INTERNAL_WAKE
/* a6 wakeup selection */
/*
A6_TCK_GPIO 	157
A6_TDIO_GPIO 	158
A6_WAKEUP_GPIO 	155
A6_MSM_INT_GPIO 	156
*/
static struct a6_sbw_interface 	sbw_ops_impl_0;
static struct a6_sbw_interface 	sbw_ops_impl_1;
#ifdef A6_INTERNAL_WAKE
static struct a6_wake_ops 	a6_wake_ops_impl_0;
static struct a6_wake_ops 	a6_wake_ops_impl_1;
#endif

static int a6_0_sbw_gpio_config[] = {
	/* first A6 config */
	TENDERLOIN_A6_0_TCK,
	TENDERLOIN_A6_0_WAKEUP,
	TENDERLOIN_A6_0_TDIO,
};

static int a6_0_sbw_gpio_config_3g[] = {
	/* first A6 config */
	TENDERLOIN_A6_0_TCK_3G,
	TENDERLOIN_A6_0_WAKEUP_3G,
	TENDERLOIN_A6_0_TDIO_3G,
};

static int a6_1_sbw_gpio_config[] = {
	/* second A6 config */
	TENDERLOIN_A6_1_TCK,
	TENDERLOIN_A6_1_WAKEUP,
	TENDERLOIN_A6_1_TDIO,
};

static int a6_1_sbw_gpio_config_3g[] = {
	/* second A6 config */
	TENDERLOIN_A6_1_TCK_3G,
	TENDERLOIN_A6_1_WAKEUP_3G,
	TENDERLOIN_A6_1_TDIO_3G,
};

static int a6_sbw_init_imp(struct a6_platform_data* plat_data)
{
	int rc = 0;

	rc = configure_gpiomux_gpios(1, (int *)plat_data->sbw_init_gpio_config,
			plat_data->sbw_init_gpio_config_size);
	if (rc < 0) {
		printk(KERN_ERR "%s: failed to configure A6 SBW gpios.\n", __func__);
	}

	return rc;
}


static int a6_sbw_deinit_imp(struct a6_platform_data* plat_data)
{
	int rc = 0;

	rc = configure_gpiomux_gpios(0, (int *)plat_data->sbw_deinit_gpio_config,
				plat_data->sbw_deinit_gpio_config_size);
	if (rc < 0) {
		printk(KERN_ERR "%s: failed to de-configure A6 SBW gpios.\n", __func__);
	}

	return rc;
}


static struct a6_platform_data tenderloin_a6_0_platform_data = {
	.dev_name			= A6_DEVICE_0,
	.pwr_gpio			= TENDERLOIN_A6_0_MSM_IRQ,
	.sbw_tck_gpio			= TENDERLOIN_A6_0_TCK,
	.sbw_tdio_gpio			= TENDERLOIN_A6_0_TDIO,
	.sbw_wkup_gpio			= TENDERLOIN_A6_0_WAKEUP,
	.sbw_ops			= &sbw_ops_impl_0,

	.sbw_init_gpio_config		= a6_0_sbw_gpio_config,
	.sbw_init_gpio_config_size 	= ARRAY_SIZE(a6_0_sbw_gpio_config),
	.sbw_deinit_gpio_config		= a6_0_sbw_gpio_config,
	.sbw_deinit_gpio_config_size 	= ARRAY_SIZE(a6_0_sbw_gpio_config),

	.sbw_init		= a6_sbw_init_imp,
	.sbw_deinit		= a6_sbw_deinit_imp,

	.power_supply_connected = 1,
};

static struct a6_platform_data tenderloin_a6_1_platform_data = {
	.dev_name			= A6_DEVICE_1,
	.pwr_gpio       		= TENDERLOIN_A6_1_MSM_IRQ,
	.sbw_tck_gpio			= TENDERLOIN_A6_1_TCK,
	.sbw_tdio_gpio			= TENDERLOIN_A6_1_TDIO,
	.sbw_wkup_gpio			= TENDERLOIN_A6_1_WAKEUP,
	.sbw_ops			= &sbw_ops_impl_1,

	.sbw_init_gpio_config		= a6_1_sbw_gpio_config,
	.sbw_init_gpio_config_size 	= ARRAY_SIZE(a6_1_sbw_gpio_config),
	.sbw_deinit_gpio_config		= a6_1_sbw_gpio_config,
	.sbw_deinit_gpio_config_size 	= ARRAY_SIZE(a6_1_sbw_gpio_config),

	.sbw_init		= a6_sbw_init_imp,
	.sbw_deinit		= a6_sbw_deinit_imp,

	.power_supply_connected = 0,
};


static int a6_0_config_data[] = {
	TENDERLOIN_A6_0_MSM_IRQ,
};

static int a6_0_config_data_3g[] = {
	TENDERLOIN_A6_0_MSM_IRQ_3G,
};

static int a6_1_config_data[] = {
	TENDERLOIN_A6_1_MSM_IRQ,
};

static int a6_1_config_data_3g[] = {
	TENDERLOIN_A6_1_MSM_IRQ_3G,
};

static struct i2c_board_info a6_0_i2c_board_info = {
	I2C_BOARD_INFO( A6_DEVICE_0, (0x62>>1)),
	.platform_data = NULL,
};

static struct i2c_board_info a6_1_i2c_board_info = {
	I2C_BOARD_INFO( A6_DEVICE_1, (0x64>>1)),
	.platform_data = NULL,
};

#ifdef A6_INTERNAL_WAKE
struct a6_internal_wake_interface_data {
	int	wake_enable: 1;
	int	wake_period: 9;
	int	wake_gpio;
};

static struct a6_internal_wake_interface_data a6_0_wi_data =
{
	.wake_enable = 1,
	.wake_period = 16,
};

static struct a6_internal_wake_interface_data a6_1_wi_data =
{
	.wake_enable = 1,
	.wake_period = 16,
};


static int32_t a6_force_wake(void* data)
{
	struct a6_internal_wake_interface_data *pdata = (struct a6_internal_wake_interface_data *)data;
	unsigned long flags = 0;

	a6_disable_interrupts(flags);
	gpio_set_value(pdata->wake_gpio, 1);
	udelay(1);
	gpio_set_value(pdata->wake_gpio, 0);
	udelay(1);
	gpio_set_value(pdata->wake_gpio, 1);
	a6_enable_interrupts(flags);

	msleep(30);

	return 0;
}

static int a6_force_sleep(void* data)
{
	struct a6_internal_wake_interface_data *pdata = (struct a6_internal_wake_interface_data *)data;

	gpio_set_value(pdata->wake_gpio, 0);

	return 0;
}

static int a6_internal_wake_enable_state(void* data)
{
	return (((struct a6_internal_wake_interface_data *)data)->wake_enable);
}

static int a6_internal_wake_period(void* data)
{
	return (((struct a6_internal_wake_interface_data *)data)->wake_period);
}
#endif // #ifdef A6_INTERNAL_WAKE


void __init tenderloin_init_a6(void)
{
	// Fixup A6 platform data
	tenderloin_a6_0_platform_data.pwr_gpio_wakeup_cap =
		msm_mpm_is_app_irq_wakeup_capable(gpio_to_irq(tenderloin_a6_0_platform_data.pwr_gpio));

	tenderloin_a6_1_platform_data.pwr_gpio_wakeup_cap =
		msm_mpm_is_app_irq_wakeup_capable(gpio_to_irq(tenderloin_a6_1_platform_data.pwr_gpio));

	printk(KERN_ERR "Registering a6_0 device.\n");
	a6_0_i2c_board_info.platform_data = &tenderloin_a6_0_platform_data;
	i2c_register_board_info(MSM_GSBI8_QUP_I2C_BUS_ID, &a6_0_i2c_board_info, 1);
	a6_sbw_init_imp(&tenderloin_a6_0_platform_data);

	printk(KERN_ERR "Registering a6_1 device.\n");
	a6_1_i2c_board_info.platform_data = &tenderloin_a6_1_platform_data;
	i2c_register_board_info(MSM_GSBI8_QUP_I2C_BUS_ID, &a6_1_i2c_board_info, 1);
	a6_sbw_init_imp(&tenderloin_a6_1_platform_data);

	if(boardtype_is_3g()) {
		configure_gpiomux_gpios(1, a6_0_config_data_3g, ARRAY_SIZE(a6_0_config_data_3g));
		configure_gpiomux_gpios(1, a6_1_config_data_3g, ARRAY_SIZE(a6_1_config_data_3g));
	}
	else {
		configure_gpiomux_gpios(1, a6_0_config_data, ARRAY_SIZE(a6_0_config_data));
		configure_gpiomux_gpios(1, a6_1_config_data, ARRAY_SIZE(a6_1_config_data));
	}

#ifdef A6_INTERNAL_WAKE
	((struct a6_platform_data*)a6_0_i2c_board_info.platform_data)->wake_ops = &a6_wake_ops_impl_0;
	((struct a6_platform_data*)a6_1_i2c_board_info.platform_data)->wake_ops = &a6_wake_ops_impl_1;

	// for non-PMIC external wakes, use sbw_wkup_gpio for force wakes...
	a6_0_wi_data.wake_gpio =
		((struct a6_platform_data *)a6_0_i2c_board_info.platform_data)->sbw_wkup_gpio;
	a6_wake_ops_impl_0.data = &a6_0_wi_data;
	a6_wake_ops_impl_0.enable_periodic_wake = NULL;
	a6_wake_ops_impl_0.disable_periodic_wake = NULL;
	a6_wake_ops_impl_0.internal_wake_enable_state = &a6_internal_wake_enable_state;
	a6_wake_ops_impl_0.internal_wake_period = &a6_internal_wake_period;
	a6_wake_ops_impl_0.force_wake = &a6_force_wake;
	a6_wake_ops_impl_0.force_sleep = &a6_force_sleep;

	a6_1_wi_data.wake_gpio =
		((struct a6_platform_data *)a6_1_i2c_board_info.platform_data)->sbw_wkup_gpio;
	a6_wake_ops_impl_1.data = &a6_1_wi_data;
	a6_wake_ops_impl_1.enable_periodic_wake = NULL;
	a6_wake_ops_impl_1.disable_periodic_wake = NULL;
	a6_wake_ops_impl_1.internal_wake_enable_state = &a6_internal_wake_enable_state;
	a6_wake_ops_impl_1.internal_wake_period = &a6_internal_wake_period;
	a6_wake_ops_impl_1.force_wake = &a6_force_wake;
	a6_wake_ops_impl_1.force_sleep = &a6_force_sleep;
#else
	((struct a6_platform_data*)a6_0_i2c_board_info.platform_data)->wake_ops = NULL;
	((struct a6_platform_data*)a6_1_i2c_board_info.platform_data)->wake_ops = NULL;
#endif

	sbw_ops_impl_0.a6_per_device_interface.SetSBWTCK = &a6_0_set_sbwtck;
	sbw_ops_impl_0.a6_per_device_interface.ClrSBWTCK = &a6_0_clr_sbwtck;
	sbw_ops_impl_0.a6_per_device_interface.SetSBWTDIO = &a6_0_set_sbwtdio;
	sbw_ops_impl_0.a6_per_device_interface.ClrSBWTDIO = &a6_0_clr_sbwtdio;
	sbw_ops_impl_0.a6_per_device_interface.SetInSBWTDIO = &a6_0_set_in_sbwtdio;
	sbw_ops_impl_0.a6_per_device_interface.SetOutSBWTDIO = &a6_0_set_out_sbwtdio;
	sbw_ops_impl_0.a6_per_device_interface.GetSBWTDIO = &a6_0_get_sbwtdio;
	sbw_ops_impl_0.a6_per_device_interface.SetSBWAKEUP = &a6_0_set_sbwakeup;
	sbw_ops_impl_0.a6_per_device_interface.ClrSBWAKEUP = &a6_0_clr_sbwakeup;
	sbw_ops_impl_0.a6_per_target_interface.delay = a6_delay_impl;

	sbw_ops_impl_1.a6_per_device_interface.SetSBWTCK = &a6_1_set_sbwtck;
	sbw_ops_impl_1.a6_per_device_interface.ClrSBWTCK = &a6_1_clr_sbwtck;
	sbw_ops_impl_1.a6_per_device_interface.SetSBWTDIO = &a6_1_set_sbwtdio;
	sbw_ops_impl_1.a6_per_device_interface.ClrSBWTDIO = &a6_1_clr_sbwtdio;
	sbw_ops_impl_1.a6_per_device_interface.SetInSBWTDIO = &a6_1_set_in_sbwtdio;
	sbw_ops_impl_1.a6_per_device_interface.SetOutSBWTDIO = &a6_1_set_out_sbwtdio;
	sbw_ops_impl_1.a6_per_device_interface.GetSBWTDIO = &a6_1_get_sbwtdio;
	sbw_ops_impl_1.a6_per_device_interface.SetSBWAKEUP = &a6_1_set_sbwakeup;
	sbw_ops_impl_1.a6_per_device_interface.ClrSBWAKEUP = &a6_1_clr_sbwakeup;
	sbw_ops_impl_1.a6_per_target_interface.delay = a6_delay_impl;
}
#endif // CONFIG_A6


void __init tenderloin_a6_fixup_pins(void)
{
	// A6
	tenderloin_a6_0_platform_data.pwr_gpio = pin_table[TENDERLOIN_A6_0_MSM_IRQ_PIN];
	tenderloin_a6_0_platform_data.sbw_tck_gpio = pin_table[TENDERLOIN_A6_0_TCK_PIN];
	tenderloin_a6_0_platform_data.sbw_tdio_gpio = pin_table[TENDERLOIN_A6_0_TDIO_PIN];
	tenderloin_a6_0_platform_data.sbw_wkup_gpio = pin_table[TENDERLOIN_A6_0_WAKEUP_PIN];

	tenderloin_a6_1_platform_data.pwr_gpio = pin_table[TENDERLOIN_A6_1_MSM_IRQ_PIN];
	tenderloin_a6_1_platform_data.sbw_tck_gpio = pin_table[TENDERLOIN_A6_1_TCK_PIN];
	tenderloin_a6_1_platform_data.sbw_tdio_gpio = pin_table[TENDERLOIN_A6_1_TDIO_PIN];
	tenderloin_a6_1_platform_data.sbw_wkup_gpio = pin_table[TENDERLOIN_A6_1_WAKEUP_PIN];

	if(boardtype_is_3g()) {
		tenderloin_a6_0_platform_data.sbw_init_gpio_config = a6_0_sbw_gpio_config_3g;
		tenderloin_a6_0_platform_data.sbw_init_gpio_config_size = ARRAY_SIZE(a6_0_sbw_gpio_config_3g);
		tenderloin_a6_0_platform_data.sbw_deinit_gpio_config = a6_0_sbw_gpio_config_3g;
		tenderloin_a6_0_platform_data.sbw_deinit_gpio_config_size = ARRAY_SIZE(a6_0_sbw_gpio_config_3g);

		tenderloin_a6_1_platform_data.sbw_init_gpio_config = a6_1_sbw_gpio_config_3g;
		tenderloin_a6_1_platform_data.sbw_init_gpio_config_size = ARRAY_SIZE(a6_1_sbw_gpio_config_3g);
		tenderloin_a6_1_platform_data.sbw_deinit_gpio_config = a6_1_sbw_gpio_config_3g;
		tenderloin_a6_1_platform_data.sbw_deinit_gpio_config_size = ARRAY_SIZE(a6_1_sbw_gpio_config_3g);

		// A6 irq
		/* 3G EVT4 boards use the same A6 gpios as DVT */
		if (board_type >= TOPAZ_3G_EVT4) {
			a6_0_config_data_3g [0] = TENDERLOIN_A6_0_MSM_IRQ_3G_DVT;
			a6_1_config_data_3g [0] = TENDERLOIN_A6_1_MSM_IRQ_3G_DVT;
			a6_0_sbw_gpio_config_3g [0] = TENDERLOIN_A6_0_TCK_3G_DVT;
		}

	} else {
		// A6 irq
		if (board_type >= TOPAZ_DVT) {
			a6_0_config_data [0] = TENDERLOIN_A6_0_MSM_IRQ_DVT;
			a6_1_config_data [0] = TENDERLOIN_A6_1_MSM_IRQ_DVT;
		}
	}

	a6_boardtype(pin_table);
}
