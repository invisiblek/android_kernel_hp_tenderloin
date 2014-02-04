/* linux/arch/arm/mach-msm/board-ruby-wifi.c
*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <asm/mach-types.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <linux/skbuff.h>
#include <linux/wl12xx.h>
#include <linux/wifi_tiwlan.h>
#include <linux/ti_wilink_st.h>
#define WILINK_UART_DEV_NAME "/dev/ttyHS0"

#include "board-ruby.h"

static int wilink_enable(struct kim_data_s *data)
{
	ruby_bluetooth_set_power(1);
	pr_info("%s\n", __func__);
	return 0;
}

static int wilink_disable(struct kim_data_s *data)
{
	ruby_bluetooth_set_power(0);
	pr_info("%s\n", __func__);
	return 0;
}

static int wilink_awake(struct kim_data_s *data)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int wilink_asleep(struct kim_data_s *data)
{
	pr_info("%s\n", __func__);
	return 0;
}

int wilink_suspend(struct platform_device *pdev, pm_message_t state)
{
	pr_info("%s\n", __func__);
	return 0;
}

int wilink_resume(struct platform_device *pdev)
{
	pr_info("%s\n", __func__);
	return 0;
}

static struct ti_st_plat_data wilink_pdata = {
	.dev_name = WILINK_UART_DEV_NAME,
	.flow_cntrl = 1,
	.baud_rate = 3000000,
	.chip_enable = wilink_enable,
	.chip_disable = wilink_disable,
	.chip_awake = wilink_awake,
	.chip_asleep = wilink_asleep,
	.suspend = wilink_suspend,
	.resume = wilink_resume,
};

static struct platform_device btwilink_device = {
	.name = "btwilink",
	.id = -1,
};

static struct platform_device wl1271_device = {
	.name   = "kim",
	.id     = -1,
	.dev.platform_data = &wilink_pdata,
};

static struct wl12xx_platform_data ruby_wlan_data __initdata = {
       .irq = MSM_GPIO_TO_INT(RUBY_GPIO_WIFI_IRQ),
       .board_ref_clock = WL12XX_REFCLOCK_26,
       .board_tcxo_clock = 1,
};

extern int ti_wifi_power(int on);
extern int ruby_wifi_reset(int on);
extern int ruby_wifi_set_carddetect(int on);
extern int ruby_wifi_get_mac_addr(unsigned char *buf);

#define PREALLOC_WLAN_NUMBER_OF_SECTIONS	4
#define PREALLOC_WLAN_NUMBER_OF_BUFFERS		160
#define PREALLOC_WLAN_SECTION_HEADER		24

#define WLAN_SECTION_SIZE_0	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 128)
#define WLAN_SECTION_SIZE_1	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 128)
#define WLAN_SECTION_SIZE_2	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 512)
#define WLAN_SECTION_SIZE_3	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 1024)

#define WLAN_SKB_BUF_NUM	16

static struct sk_buff *wlan_static_skb[WLAN_SKB_BUF_NUM];

typedef struct wifi_mem_prealloc_struct {
	void *mem_ptr;
	unsigned long size;
} wifi_mem_prealloc_t;

static wifi_mem_prealloc_t wifi_mem_array[PREALLOC_WLAN_NUMBER_OF_SECTIONS] = {
	{ NULL, (WLAN_SECTION_SIZE_0 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_1 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_2 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_3 + PREALLOC_WLAN_SECTION_HEADER) }
};

static void *ruby_wifi_mem_prealloc(int section, unsigned long size)
{
	if (section == PREALLOC_WLAN_NUMBER_OF_SECTIONS)
		return wlan_static_skb;
	if ((section < 0) || (section > PREALLOC_WLAN_NUMBER_OF_SECTIONS))
		return NULL;
	if (wifi_mem_array[section].size < size)
		return NULL;
	return wifi_mem_array[section].mem_ptr;
}

int __init ruby_init_wifi_mem(void)
{
	int i;

	for (i = 0; (i < WLAN_SKB_BUF_NUM); i++) {
		if (i < (WLAN_SKB_BUF_NUM/2))
			wlan_static_skb[i] = dev_alloc_skb(4096);
		else
			wlan_static_skb[i] = dev_alloc_skb(8192);
	}
	for (i = 0; (i < PREALLOC_WLAN_NUMBER_OF_SECTIONS); i++) {
		wifi_mem_array[i].mem_ptr = kmalloc(wifi_mem_array[i].size,
							GFP_KERNEL);
		if (wifi_mem_array[i].mem_ptr == NULL)
			return -ENOMEM;
	}
	return 0;
}

static struct resource ruby_wifi_resources[] = {
	[0] = {
		.name		= "device_wifi_irq",
		.start		= MSM_GPIO_TO_INT(RUBY_GPIO_WIFI_IRQ),
		.end		= MSM_GPIO_TO_INT(RUBY_GPIO_WIFI_IRQ),
		.flags		= IORESOURCE_IRQ | IORESOURCE_IRQ_LOWEDGE,
	},
};

static struct wifi_platform_data ruby_wifi_control = {
	.set_power      = ti_wifi_power,
	.set_reset      = ruby_wifi_reset,
	.set_carddetect = ruby_wifi_set_carddetect,
	.mem_prealloc   = ruby_wifi_mem_prealloc,
};

static struct platform_device ruby_wifi_device = {
	.name           = "device_wifi",
	.id             = 1,
	.num_resources  = ARRAY_SIZE(ruby_wifi_resources),
	.resource       = ruby_wifi_resources,
	.dev            = {
		.platform_data = &ruby_wifi_control,
	},
};

int __init ruby_wifi_init(void)
{
	int ret;
	printk(KERN_INFO "%s: start\n", __func__);

	platform_device_register(&wl1271_device);
	platform_device_register(&btwilink_device);

	ruby_init_wifi_mem();

	ret = platform_device_register(&ruby_wifi_device);

	if(wl12xx_set_platform_data(&ruby_wlan_data))
		pr_err("Error setting wl12xx_data\n");

	return ret;
}

