#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/cy8ctma395.h>
#ifdef CONFIG_HSUART
#include <linux/hsuart.h>
#endif
#include <mach/msm_gpiomux.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "devices-msm8x60.h"
#include "board-tenderloin.h"

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

#define GSBI1_PHYS			0x16000000
#define GSBI8_PHYS			0x19800000
#define GSBI_CTRL			0x0
#define PROTOCOL_CODE(code)		(((code) & 0x7) << 4)
#define UART_WITH_FLOW_CONTROL		0x4
#define I2C_ON_2_PORTS_UART		0x6

static DEFINE_MUTEX(gsbi_init_lock);

int board_gsbi_init(int gsbi, int *inited, u32 prot);

int board_gsbi10_init_cb(void)
{
	static int inited = 0;

        return (board_gsbi_init(10, &inited, I2C_ON_2_PORTS_UART));
}

#ifdef CONFIG_HSUART
// ctp uart
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)

#define CTP_UART_SPEED_SLOW		3000000
#define CTP_UART_SPEED_FAST		4000000

static int ctp_uart_pin_mux(int on)
{
	int rc;
	static int is_on = 0;

	pr_debug("%s: on=%d\n", __func__, on);

	if (on && !is_on)
		rc = msm_gpiomux_get(GPIO_CTP_RX);

	else if (!on && is_on)
		rc = msm_gpiomux_put(GPIO_CTP_RX);

	else
		rc = 0;

	is_on = on;

	return (rc);
}

static struct hsuart_platform_data ctp_uart_data = {
	.dev_name = "ctp_uart",
	.uart_mode = HSUART_MODE_FLOW_CTRL_NONE | HSUART_MODE_PARITY_NONE,
	.uart_speed = CTP_UART_SPEED_SLOW,
	.options = HSUART_OPTION_RX_DM | HSUART_OPTION_SCHED_RT,

	.tx_buf_size = 4096,
	.tx_buf_num = 1,
	.rx_buf_size = 16384,
	.rx_buf_num = 2,
	.max_packet_size = 10240,
	.min_packet_size = 1,
	.rx_latency = CTP_UART_SPEED_FAST/20000,	/* bytes per 500 us */
	.p_board_pin_mux_cb = ctp_uart_pin_mux,
	.p_board_config_gsbi_cb = board_gsbi10_init_cb,
};

static struct platform_device ctp_uart_device = {
	.name = "hsuart",
	.id =  1,
	.dev  = {
		.platform_data = &ctp_uart_data,
	}
};
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
#endif

#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
static int board_cy8ctma395_gpio_request(unsigned gpio, const char *name,
						int request, int *requested,
						struct gpiomux_setting *new,
						struct gpiomux_setting *old,
						int *replaced)
{
	int rc;

	if (request && !*requested) {
		rc = gpio_request(gpio, name);
		if (rc < 0) {
			pr_err("error %d requesting gpio %u (%s)\n", rc, gpio, name);
			goto gpio_request_failed;
		}

		rc = msm_gpiomux_write(gpio, GPIOMUX_ACTIVE, new, old);
		if (rc < 0)  {
			pr_err("error %d muxing gpio %u (%s)\n", rc, gpio, name);
			goto msm_gpiomux_write_failed;
		}

		*replaced = !rc;

		rc = msm_gpiomux_get(gpio);
		if (rc < 0)  {
			pr_err("error %d 'getting' gpio %u (%s)\n", rc, gpio, name);
			goto msm_gpiomux_get_failed;
		}

		*requested = 1;
	}

	else if (!request && *requested) {
		msm_gpiomux_put(gpio);
		msm_gpiomux_write(gpio, GPIOMUX_ACTIVE, *replaced ? old : NULL, NULL);
		gpio_free(gpio);
		*requested = 0;
	}

	return (0);

msm_gpiomux_get_failed:
	msm_gpiomux_write(gpio, GPIOMUX_ACTIVE, *replaced ? old : NULL, NULL);
msm_gpiomux_write_failed:
	gpio_free(gpio);
gpio_request_failed:

	return (rc);
}

static int board_cy8ctma395_swdck_request(int request)
{
	static int replaced = 0;
	static int requested = 0;
	static struct gpiomux_setting scl;

	struct gpiomux_setting swdck = {
		.func = GPIOMUX_FUNC_GPIO,
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_OUT_HIGH,
	};

	return (board_cy8ctma395_gpio_request(GPIO_CTP_SCL, "CY8CTMA395_SWDCK",
						request, &requested, &swdck,
						&scl, &replaced));
}

static int board_cy8ctma395_swdio_request(int request)
{
	static int replaced = 0;
	static int requested = 0;
	static struct gpiomux_setting sda;

	struct gpiomux_setting swdio = {
		.func = GPIOMUX_FUNC_GPIO,
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_OUT_HIGH,
	};

	return (board_cy8ctma395_gpio_request(GPIO_CTP_SDA, "CY8CTMA395_SWDIO",
						request, &requested, &swdio,
						&sda, &replaced));
}

static void board_cy8ctma395_vdd_enable(int enable)
{
	int rc;
	static struct regulator *tp_5v0 = NULL;
	static struct regulator *tp_l10 = NULL;
	static int isPowerOn = 0;

	if (!tp_l10) {
		tp_l10 = regulator_get(NULL, "8058_l10");
		if (IS_ERR(tp_l10)) {
			pr_err("%s: failed to get regulator \"8058_l10\"\n", __func__);
			return;
		}

		rc = regulator_set_voltage(tp_l10, 3050000, 3050000);
		if (rc) {
			pr_err("%s: Unable to set regulator voltage:"
			       " tp_l10\n", __func__);
			regulator_put(tp_l10);
			tp_l10 = NULL;
			return;
		}
	}

	if (!tp_5v0) {
		tp_5v0 = regulator_get(NULL, "vdd50_boost");
		if (IS_ERR(tp_5v0)) {
			pr_err("failed to get regulator 'vdd50_boost' with %ld\n",
				PTR_ERR(tp_5v0));
			regulator_put(tp_l10);
			tp_l10 = NULL;
			tp_5v0 = NULL;
			return;
		}
	}

	if (enable == isPowerOn) {
		return;
	}

	if (enable) {
		rc = regulator_enable(tp_l10);
		if (rc < 0) {
			pr_err("failed to enable regulator '8058_l10' with %d\n", rc);
			return;
		}

		rc = regulator_enable(tp_5v0);
		if (rc < 0) {
			pr_err("failed to enable regulator 'vdd50_boost' with %d\n", rc);
			return;
		}

		// Make sure the voltage is stabilized
		msleep(2);
	}

	else {
		rc = regulator_disable(tp_5v0);
		if (rc < 0) {
			pr_err("failed to disable regulator 'vdd50_boost' with %d\n", rc);
			return;
		}

		rc = regulator_disable(tp_l10);
		if (rc < 0) {
			pr_err("failed to disable regulator '8058_l10' with %d\n", rc);
			return;
		}
	}

	isPowerOn = enable;
}

static struct cy8ctma395_platform_data board_cy8ctma395_data = {
	.swdck_request = board_cy8ctma395_swdck_request,
	.swdio_request = board_cy8ctma395_swdio_request,
	.vdd_enable = board_cy8ctma395_vdd_enable,
	.xres = GPIO_CY8CTMA395_XRES,
	.xres_us = 1000,
	.swdck = GPIO_CTP_SCL,
	.swdio = GPIO_CTP_SDA,
	.swd_wait_retries = 0,
	.port_acquire_retries = 4,
	.status_reg_timeout_ms = 1000,
	.nr_blocks = 256,
};

static struct platform_device board_cy8ctma395_device = {
	.name = CY8CTMA395_DEVICE,
	.id = -1,
	.dev = {
		.platform_data = &board_cy8ctma395_data,
         },
};
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */

static int tenderloin_tp_init(int on)
{
	int ret;
	int tp_gpios[] = {GPIO_CTP_SCL, GPIO_CTP_SDA};
	ret = configure_gpiomux_gpios(on, tp_gpios, ARRAY_SIZE(tp_gpios));
	if (ret < 0) {
		printk(KERN_ERR "%s: Error %d while configuring touch panel gpios.\n", __func__, ret);
	}
	return ret;
}
//devices
static struct platform_device *ts_devices[] __initdata = {
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
 	&board_cy8ctma395_device,
 	&msm_device_uart_dm2,
#ifdef CONFIG_HSUART
 	&ctp_uart_device,
#endif
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
};

void __init tenderloin_init_ts(void)
{
        platform_add_devices(ts_devices, ARRAY_SIZE(ts_devices));
        // call init
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	tenderloin_tp_init (true);
#endif
}

