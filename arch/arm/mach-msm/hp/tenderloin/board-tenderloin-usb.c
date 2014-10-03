#include <linux/gpio.h>
#include <linux/msm-charger.h>
#include <mach/mpp.h>
#include <mach/msm_hsusb.h>
#include <mach/socinfo.h>
#include <linux/mfd/pmic8058.h>
#ifdef CONFIG_A6
#include <linux/a6.h>
#endif
#ifdef CONFIG_MDMGPIO
#include <linux/mdmgpio.h>
#endif
#include "devices.h"
#include "devices-msm8x60.h"
#include "board-tenderloin.h"

void max8903b_set_vbus_draw (unsigned ma);

#ifdef CONFIG_USB_PEHCI_HCD

/* Important fields to initialize for Non-PCI based driver*/

/* The base physical memory address assigned for the ISP176x */
#define ISP176x_MEM_BASE 0x1D000000 //base address

/* The memory range assigned for the ISP176x 24k*/
#define ISP176x_MEM_RANGE 0x6000

/* The IRQ number assigned to the ISP176x */
#define ISP176x_IRQ_NUM 	MSM_GPIO_TO_INT(ISP1763_INT_GPIO)


static struct resource isp1763_resources[] = {
	[0] = {
		.flags	= IORESOURCE_MEM,
		.start	= ISP176x_MEM_BASE,
		.end	= (ISP176x_MEM_BASE + ISP176x_MEM_RANGE - 1),		/* 24KB */
	},
	[1] = {
		.flags	= IORESOURCE_IRQ,
		.start	= ISP176x_IRQ_NUM,
		.end	= ISP176x_IRQ_NUM,
	},
};
static void __init msm8x60_cfg_isp1763(void)
{
	isp1763_resources[1].start = gpio_to_irq(ISP1763_INT_GPIO);
	isp1763_resources[1].end = gpio_to_irq(ISP1763_INT_GPIO);
}

int config_gpio_tlmm_table(uint32_t *table, int len)
{
	int n, rc = 0;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
					__func__, table[n], rc);
			break;
		}
	}
	return rc;
}

static uint32_t msmebi2_tlmm_cfgs[]=
{
	//control
	GPIO_CFG(172, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),   // Interrupt (GPIO(INTERRUPT)
	GPIO_CFG(152, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA),   // ISP1763A.RESET_N (GPIO152)
	GPIO_CFG(133, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_CS3_N (GPIO133)
	GPIO_CFG(151, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_OE_N (GPIO151)
	GPIO_CFG(157, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),	 // EBI2_WE_N (GPIO157)

	//Address
	GPIO_CFG(38, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_8 (GPIO38)
	GPIO_CFG(123, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_7 (GPIO123)
	GPIO_CFG(124, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_6 (GPIO124)
	GPIO_CFG(125, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_5 (GPIO125)
	GPIO_CFG(126, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_4 (GPIO126)
	GPIO_CFG(127, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_3 (GPIO127)
	GPIO_CFG(128, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_2 (GPIO128)
	GPIO_CFG(129, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_1 (GPIO129)
	GPIO_CFG(130, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA),   // EBI2_ADDR_0 (GPIO130)

	//Data
	GPIO_CFG(135, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_15 (GPIO135)
	GPIO_CFG(136, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_14 (GPIO136)
	GPIO_CFG(137, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_13 (GPIO137)
	GPIO_CFG(138, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_12 (GPIO138)
	GPIO_CFG(139, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_11 (GPIO139)
	GPIO_CFG(140, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_10 (GPIO140)
	GPIO_CFG(141, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_9 (GPIO141)
	GPIO_CFG(142, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_8 (GPIO142)
	GPIO_CFG(143, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_7 (GPIO143)
	GPIO_CFG(144, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_6 (GPIO144)
	GPIO_CFG(145, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_5 (GPIO145)
	GPIO_CFG(146, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_4 (GPIO146)
	GPIO_CFG(147, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_3 (GPIO147)
	GPIO_CFG(148, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_2 (GPIO148)
	GPIO_CFG(149, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_1 (GPIO149)
	GPIO_CFG(150, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),    // EBI2_AD_0 (GPIO150)
};

static int isp1763_setup_gpio(int enable)
{
	int status = 0;
	static int gpio_requested = 0;
	pr_info("%s\n", __func__);

	if (!gpio_requested)
	{
		config_gpio_tlmm_table(msmebi2_tlmm_cfgs, ARRAY_SIZE(msmebi2_tlmm_cfgs));
		status = gpio_request(ISP1763_INT_GPIO, "isp1763_usb");
		if (status) {
			pr_err("%s:Failed to request GPIO %d\n",
						__func__, ISP1763_INT_GPIO);
			return status;
		}
		status = gpio_request(ISP1763_RST_GPIO, "isp1763_usb");
		if (status) {
			pr_err("%s:Failed to request GPIO %d\n",
						__func__, ISP1763_RST_GPIO);
			gpio_free(ISP1763_INT_GPIO);
			return status;
		}
		gpio_requested = 1;
	}

	if (enable) {
		status = gpio_direction_input(ISP1763_INT_GPIO);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_INT_GPIO);
		}
		status = gpio_direction_output(ISP1763_RST_GPIO, 1);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_RST_GPIO);
		}
		pr_debug("\nISP GPIO configuration done\n");
		return status;
	}
	else
	{
		status = gpio_direction_output(ISP1763_RST_GPIO, 0);
		if (status) {
			pr_err("%s:Failed to configure GPIO %d\n",
					__func__, ISP1763_RST_GPIO);
		}
	}
	return status;
}

static struct isp1763_platform_data isp1763_pdata = {
	.reset_gpio	= ISP1763_RST_GPIO,
	.setup_gpio	= isp1763_setup_gpio
};

static struct platform_device isp1763_device = {
	.name          = "isp1763_usb",
	.num_resources = ARRAY_SIZE(isp1763_resources),
	.resource      = isp1763_resources,
	.dev           = {
		.platform_data = &isp1763_pdata
	}
};

/*
  This function only is charge of modem gpio initialize in board level, and should not
  take responsiblity for enabling/disabling external modem, which will be implemented
  in exmdm (external modem) driver.
*/
static int isp1763_modem_gpio_init(int on)
{
	int rc;
	static int gpio_requested=0;

	int gpio_pwr_3g_en = pin_table[TENDERLOIN_GPIO_3G_3V3_EN];

	if (!gpio_requested)
	{
		gpio_tlmm_config(GPIO_CFG(gpio_pwr_3g_en, 0,GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		rc = gpio_request(gpio_pwr_3g_en, "VDD_3V3_EN");
		if (rc < 0) {
			pr_err("%s: VDD_3V3_EN gpio %d request failed\n", __func__, gpio_pwr_3g_en);
		}
		else
		{
			pr_debug("%s: VDD_3V3_EN gpio %d statu: %d\n", __func__, gpio_pwr_3g_en, gpio_get_value(gpio_pwr_3g_en));
		}
		gpio_direction_output(gpio_pwr_3g_en, 0);
		rc = gpio_request(GPIO_3G_DISABLE_N, "3G_DISABLE_N");
		if (rc < 0) {
			pr_err("%s: GPIO_3G_DISABLE_N gpio %d request failed\n", __func__, GPIO_3G_DISABLE_N);
		}
		else
		{
			pr_debug( "%s: GPIO_3G_DISABLE_N gpio %d status: %d\n", __func__, GPIO_3G_DISABLE_N, gpio_get_value(GPIO_3G_DISABLE_N));
		}
		gpio_direction_output(GPIO_3G_DISABLE_N, 0);

		gpio_tlmm_config(GPIO_CFG(GPIO_3G_WAKE_N, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

		rc = gpio_request(GPIO_3G_WAKE_N, "3G_WAKE");
		if (rc < 0) {
			pr_err("%s: GPIO_3G_WAKE_N gpio %d request failed\n", __func__, GPIO_3G_WAKE_N);
		}

		gpio_direction_input(GPIO_3G_WAKE_N);

		gpio_tlmm_config(GPIO_CFG(GPIO_3G_UIM_CD_N, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		rc = gpio_request(GPIO_3G_UIM_CD_N, "UIM_CD");
		if (rc < 0) {
			printk(KERN_ERR "%s: GPIO_3G_UIM_CD_N gpio %d request failed\n", __func__, GPIO_3G_UIM_CD_N);
		}
		gpio_direction_input(GPIO_3G_UIM_CD_N);
		rc = gpio_request(ISP1763_DACK_GPIO, "ISP1763A_DACK");
		if (rc < 0) {
			pr_err("%s: ISP1763A_DACK gpio %d request failed\n", __func__, ISP1763_DACK_GPIO);
		}

		rc = gpio_request(ISP1763_DREQ_GPIO, "ISP1763A_DREQ");
		if (rc < 0) {
			pr_err("%s: ISP1763A_DREQ gpio %d request failed\n", __func__, ISP1763_DREQ_GPIO);
		}
		gpio_requested = 1;
	}

	gpio_set_value(GPIO_3G_DISABLE_N, 0);
	gpio_set_value(gpio_pwr_3g_en, 0);

	if (on)
	{
		mdelay(300);
		gpio_set_value(gpio_pwr_3g_en, 1);
		gpio_set_value(GPIO_3G_DISABLE_N, 1);
	}

	return 0;
}

static struct platform_device *isp1763_pdev=NULL;
static int isp1763_modem_poweron_status(void)
{
	return isp1763_pdev? 1: 0;
}

static int isp1763_modem_poweron(int on)
{
	int ret=0;
	static struct mutex poweron_lock;
	static int inited = 0;

	pr_info("%s: isp1763_pdev:%p, on:%d\n", __func__, isp1763_pdev, on);

	if (!inited)
	{
		inited = 1;
		mutex_init(&poweron_lock);
	}

	mutex_lock(&poweron_lock);
	if (boardtype_is_3g())
	{
		if (on && !isp1763_pdev)
		{
			isp1763_modem_gpio_init(1);
			pr_info("%s: platform_device_register\n", __func__);
			isp1763_pdev = platform_device_alloc(isp1763_device.name, 0);
			if (!isp1763_pdev)
			{
				mutex_unlock(&poweron_lock);
				pr_err("%s: Platform device allocation failed\n", __func__);
				return -ENOMEM;
			}
			if ((ret=platform_device_add_resources(isp1763_pdev, isp1763_resources, ARRAY_SIZE(isp1763_resources)))!=0)
				goto out_free_resources;
			if ((ret=platform_device_add_data(isp1763_pdev, &isp1763_pdata,
						sizeof(isp1763_pdata)))!=0)
				goto out_free_resources;
			if ((ret = platform_device_add(isp1763_pdev))!=0)
				goto out_free_resources;
		}
		else if (!on && isp1763_pdev)
		{
			msleep(500);
			pr_info("%s: platform_device_unregister\n", __func__);
			platform_device_unregister(isp1763_pdev);
			isp1763_pdev = NULL;
			isp1763_modem_gpio_init(0);
			msleep(500);
		}
	}
	mutex_unlock(&poweron_lock);
	pr_info("%s: isp1763_pdev:%p, on:%d -- return\n", __func__, isp1763_pdev, on);
	return 0;

out_free_resources:
	pr_err("%s: out_free_resources\n", __func__);
	platform_device_put(isp1763_pdev);
	isp1763_pdev = NULL;
	isp1763_modem_gpio_init(0);
	pr_err("%s: return\n", __func__);
	mutex_unlock(&poweron_lock);
	return ret;
}
#endif /* CONFIG_USB_PEHCI_HCD */
#ifdef CONFIG_MDMGPIO
static int set_gpio_value(int gpionum, int value)
{
	gpio_set_value_cansleep(gpionum, value);
	return 0;
}

static int get_gpio_value(int gpionum)
{
	int value;
	value = gpio_get_value_cansleep(gpionum);
	return value;
}

static struct mdmgpio_platform_data mdmgpio_pdata = {
	.uim_cd_gpio = GPIO_3G_UIM_CD_N,
	.mdm_disable_gpio = GPIO_3G_DISABLE_N,
	.set_gpio_value = set_gpio_value,
	.get_gpio_value = get_gpio_value,
#ifdef CONFIG_USB_PEHCI_HCD
	.mdm_poweron    = isp1763_modem_poweron,
	.mdm_poweron_status= isp1763_modem_poweron_status,
#endif
};

struct platform_device mdmgpio_device = {
	.name = "mdmgpio",
	.id = -1,
	.dev = {
		.platform_data = &mdmgpio_pdata,
	},
};
#endif /* CONFIG_MDMGPIO */

#if defined(CONFIG_USB_GADGET_MSM_72K) || defined(CONFIG_USB_EHCI_MSM_72K)
static struct regulator *ldo6_3p3;
static struct regulator *ldo7_1p8;
static struct regulator *vdd_cx;
#define PMICID_INT		PM8058_GPIO_IRQ(PM8058_IRQ_BASE, 36)
notify_vbus_state notify_vbus_state_func_ptr;

#ifdef CONFIG_USB_EHCI_MSM_72K
#define USB_PMIC_ID_DET_DELAY	msecs_to_jiffies(100)
static int pmic_id_notif_supported;
struct delayed_work pmic_id_det;

static int __init usb_id_pin_rework_setup(char *support)
{
	if (strncmp(support, "true", 4) == 0)
		pmic_id_notif_supported = 1;

	return 1;
}
__setup("usb_id_pin_rework=", usb_id_pin_rework_setup);

#endif

#define USB_PHY_SUSPEND_MIN_VDD_DIG_VOL		750000
#define USB_PHY_OPERATIONAL_MIN_VDD_DIG_VOL	1000000
#define USB_PHY_MAX_VDD_DIG_VOL			1320000
static int msm_hsusb_init_vddcx(int init)
{
	int ret = 0;

	if (init) {
		vdd_cx = regulator_get(NULL, "8058_s1");
		if (IS_ERR(vdd_cx)) {
			return PTR_ERR(vdd_cx);
		}

		ret = regulator_set_voltage(vdd_cx,
				USB_PHY_OPERATIONAL_MIN_VDD_DIG_VOL,
				USB_PHY_MAX_VDD_DIG_VOL);
		if (ret) {
			pr_err("%s: unable to set the voltage for regulator"
				"vdd_cx\n", __func__);
			regulator_put(vdd_cx);
			return ret;
		}

		ret = regulator_enable(vdd_cx);
		if (ret) {
			pr_err("%s: unable to enable regulator"
				"vdd_cx\n", __func__);
			regulator_put(vdd_cx);
		}
	} else {
		ret = regulator_disable(vdd_cx);
		if (ret) {
			pr_err("%s: Unable to disable the regulator:"
				"vdd_cx\n", __func__);
			return ret;
		}

		regulator_put(vdd_cx);
	}

	return ret;
}

static int msm_hsusb_config_vddcx(int high)
{
	int max_vol = USB_PHY_MAX_VDD_DIG_VOL;
	int min_vol;
	int ret;

	if (high)
		min_vol = USB_PHY_OPERATIONAL_MIN_VDD_DIG_VOL;
	else
		min_vol = USB_PHY_SUSPEND_MIN_VDD_DIG_VOL;

	ret = regulator_set_voltage(vdd_cx, min_vol, max_vol);
	if (ret) {
		pr_err("%s: unable to set the voltage for regulator"
			"vdd_cx\n", __func__);
		return ret;
	}

	pr_debug("%s: min_vol:%d max_vol:%d\n", __func__, min_vol, max_vol);

	return ret;
}

#define USB_PHY_3P3_VOL_MIN	3300000 /* uV */ //Palm changed to 3.3v
#define USB_PHY_3P3_VOL_MAX	3300000 /* uV */
#define USB_PHY_3P3_HPM_LOAD	50000	/* uA */
#define USB_PHY_3P3_LPM_LOAD	4000	/* uA */

#define USB_PHY_1P8_VOL_MIN	1800000 /* uV */
#define USB_PHY_1P8_VOL_MAX	1800000 /* uV */
#define USB_PHY_1P8_HPM_LOAD	50000	/* uA */
#define USB_PHY_1P8_LPM_LOAD	4000	/* uA */
static int msm_hsusb_ldo_init(int init)
{
	int rc = 0;

	if (init) {
		ldo6_3p3 = regulator_get(NULL, "8058_l6");
		if (IS_ERR(ldo6_3p3))
			return PTR_ERR(ldo6_3p3);

		ldo7_1p8 = regulator_get(NULL, "8058_l7");
		if (IS_ERR(ldo7_1p8)) {
			rc = PTR_ERR(ldo7_1p8);
			goto put_3p3;
		}

		rc = regulator_set_voltage(ldo6_3p3, USB_PHY_3P3_VOL_MIN,
				USB_PHY_3P3_VOL_MAX);
		if (rc) {
			pr_err("%s: Unable to set voltage level for"
				"ldo6_3p3 regulator\n", __func__);
			goto put_1p8;
		}
		rc = regulator_enable(ldo6_3p3);
		if (rc) {
			pr_err("%s: Unable to enable the regulator:"
				"ldo6_3p3\n", __func__);
			goto put_1p8;
		}
		rc = regulator_set_voltage(ldo7_1p8, USB_PHY_1P8_VOL_MIN,
				USB_PHY_1P8_VOL_MAX);
		if (rc) {
			pr_err("%s: Unable to set voltage level for"
				"ldo7_1p8 regulator\n", __func__);
			goto disable_3p3;
		}
		rc = regulator_enable(ldo7_1p8);
		if (rc) {
			pr_err("%s: Unable to enable the regulator:"
				"ldo7_1p8\n", __func__);
			goto disable_3p3;
		}

		return 0;
	}

	regulator_disable(ldo7_1p8);
disable_3p3:
	regulator_disable(ldo6_3p3);
put_1p8:
	regulator_put(ldo7_1p8);
put_3p3:
	regulator_put(ldo6_3p3);
	return rc;
}

static int msm_hsusb_ldo_enable(int on)
{
	int ret = 0;

	if (!ldo7_1p8 || IS_ERR(ldo7_1p8)) {
		pr_err("%s: ldo7_1p8 is not initialized\n", __func__);
		return -ENODEV;
	}

	if (!ldo6_3p3 || IS_ERR(ldo6_3p3)) {
		pr_err("%s: ldo6_3p3 is not initialized\n", __func__);
		return -ENODEV;
	}

	if (on) {
		ret = regulator_set_optimum_mode(ldo7_1p8,
				USB_PHY_1P8_HPM_LOAD);
		if (ret < 0) {
			pr_err("%s: Unable to set HPM of the regulator:"
				"ldo7_1p8\n", __func__);
			return ret;
		}
		ret = regulator_set_optimum_mode(ldo6_3p3,
				USB_PHY_3P3_HPM_LOAD);
		if (ret < 0) {
			pr_err("%s: Unable to set HPM of the regulator:"
				"ldo6_3p3\n", __func__);
			regulator_set_optimum_mode(ldo7_1p8,
				USB_PHY_1P8_LPM_LOAD);
			return ret;
		}
	} else {
		ret = regulator_set_optimum_mode(ldo7_1p8,
				USB_PHY_1P8_LPM_LOAD);
		if (ret < 0)
			pr_err("%s: Unable to set LPM of the regulator:"
				"ldo7_1p8\n", __func__);
		ret = regulator_set_optimum_mode(ldo6_3p3,
				USB_PHY_3P3_LPM_LOAD);
		if (ret < 0)
			pr_err("%s: Unable to set LPM of the regulator:"
				"ldo6_3p3\n", __func__);
	}

	pr_debug("reg (%s)\n", on ? "HPM" : "LPM");
	return ret < 0 ? ret : 0;
 }
#endif
#ifdef CONFIG_USB_EHCI_MSM_72K
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
	static struct regulator *votg_5v_switch;
	static int vbus_is_on;

	/* If VBUS is already on (or off), do nothing. */
	if (on == vbus_is_on)
		return;

	if (!votg_5v_switch) {
		votg_5v_switch = regulator_get(NULL, "8901_usb_otg");
		if (IS_ERR(votg_5v_switch)) {
			pr_err("%s: unable to get votg_5v_switch\n", __func__);
			votg_5v_switch = NULL;
			return;
		}
	}

	if (on) {
		if (regulator_enable(votg_5v_switch)) {
			pr_err("%s: Unable to enable the regulator:"
					" votg_5v_switch\n", __func__);
			return;
		}
	} else {
		if (regulator_disable(votg_5v_switch))
			pr_err("%s: Unable to enable the regulator:"
				" votg_5v_switch\n", __func__);
	}

	vbus_is_on = on;
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
	.phy_info	= (USB_PHY_INTEGRATED | USB_PHY_MODEL_45NM),
	.power_budget	= 390,
};
#endif

static struct msm_otg_platform_data msm_otg_pdata = {
	/* if usb link is in sps there is no need for
	 * usb pclk as dayatona fabric clock will be
	 * used instead
	 */
        //	.pclk_src_name		 = "dfab_usb_hs_clk",
	.pemp_level		 = PRE_EMPHASIS_WITH_20_PERCENT,
	.cdr_autoreset		 = CDR_AUTO_RESET_DISABLE,
	.se1_gating		 = SE1_GATING_DISABLE,
	.bam_disable		 = 1,
	.hsdrvslope		 = 0x05,
#ifdef CONFIG_USB_EHCI_MSM_72K
	.vbus_power = msm_hsusb_vbus_power,
#endif
#ifdef CONFIG_BATTERY_MSM8X60
	.pmic_vbus_notif_init	= msm_hsusb_pmic_vbus_notif_init,
#endif
	.ldo_init		 = msm_hsusb_ldo_init,
	.ldo_enable		 = msm_hsusb_ldo_enable,
	.config_vddcx            = msm_hsusb_config_vddcx,
	.init_vddcx              = msm_hsusb_init_vddcx,
#if defined(CONFIG_BATTERY_MSM8X60)
	.chg_vbus_draw = msm_charger_vbus_draw,
#elif defined (CONFIG_MAX8903B_CHARGER)
	.chg_vbus_draw = max8903b_set_vbus_draw,
#endif
#ifdef CONFIG_A6
	.chg_connected = a6_charger_event,
#endif
};

#ifdef CONFIG_USB_GADGET_MSM_72K
static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.is_phy_status_timer_on = 1,
};
#endif

void __init tenderloin_usb_i2c_init(void)
{
	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 2 &&
			(machine_is_msm8x60_surf() ||
			(machine_is_msm8x60_ffa() &&
			pmic_id_notif_supported))){
		msm_otg_pdata.phy_can_powercollapse = 1;
	}
	msm_device_otg.dev.platform_data = &msm_otg_pdata;

#ifdef CONFIG_USB_GADGET_MSM_72K
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
#endif
}
void board_register_reboot_notifier(void);

void __init tenderloin_usb_init(void)
{
#ifdef CONFIG_MDMGPIO
        platform_device_register(&mdmgpio_device);
#endif
#ifdef CONFIG_USB_EHCI_MSM_72K
        msm_add_host(0, &msm_usb_host_pdata);
#endif
#if defined(CONFIG_USB_PEHCI_HCD) || defined(CONFIG_USB_PEHCI_HCD_MODULE)
	if (boardtype_is_3g()) {
                board_register_reboot_notifier();
		msm8x60_cfg_isp1763();
		isp1763_modem_poweron(1);
	}
#endif
}
