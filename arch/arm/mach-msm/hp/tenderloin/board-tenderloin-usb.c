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
	{
		.flags	= IORESOURCE_MEM,
		.start	= ISP176x_MEM_BASE,
		.end	= (ISP176x_MEM_BASE + ISP176x_MEM_RANGE - 1),		/* 24KB */
	},
	{
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

static int isp1763_setup_gpio(int enable)
{
	int status = 0;
	static int gpio_requested = 0;
	pr_info("%s\n", __func__);

	if (!gpio_requested)
	{
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
#if 0
	int rc;
	static int gpio_requested=0;

	int gpio_pwr_3g_en = pin_table[TENDERLOIN_GPIO_3G_3V3_EN];

	if (!gpio_requested)
	{
		rc = gpio_request(gpio_pwr_3g_en, "VDD_3V3_EN");
		if (rc < 0) {
			pr_err("%s: VDD_3V3_EN gpio %d request failed\n", __func__, gpio_pwr_3g_en);
		}
		else
		{
			pr_debug("%s: VDD_3V3_EN gpio %d statu: %d\n", __func__, gpio_pwr_3g_en, gpio_get_value(gpio_pwr_3g_en));
		}

		rc = gpio_request(GPIO_3G_DISABLE_N, "3G_DISABLE_N");
		if (rc < 0) {
			pr_err("%s: GPIO_3G_DISABLE_N gpio %d request failed\n", __func__, GPIO_3G_DISABLE_N);
		}
		else
		{
			pr_debug( "%s: GPIO_3G_DISABLE_N gpio %d status: %d\n", __func__, GPIO_3G_DISABLE_N, gpio_get_value(GPIO_3G_DISABLE_N));
		}

		rc = gpio_request(GPIO_3G_WAKE_N, "3G_WAKE");
		if (rc < 0) {
			pr_err("%s: GPIO_3G_WAKE_N gpio %d request failed\n", __func__, GPIO_3G_WAKE_N);
		}

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

#endif
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
struct delayed_work pmic_id_det;
static void pmic_id_detect(struct work_struct *w)
{
	int val = gpio_get_value_cansleep(PM8058_GPIO_PM_TO_SYS(36));
	pr_info("%s(): gpio_read_value = %d\n", __func__, val);

	if (notify_vbus_state_func_ptr)
		(*notify_vbus_state_func_ptr) (val);
}

static irqreturn_t pmic_id_on_irq(int irq, void *data)
{
	/*
	 * Spurious interrupts are observed on pmic gpio line
	 * even though there is no state change on USB ID. Schedule the
	 * work to to allow debounce on gpio
	 */
	schedule_delayed_work(&pmic_id_det, USB_PMIC_ID_DET_DELAY);

	return IRQ_HANDLED;
}

static int msm_hsusb_pmic_id_notif_init(void (*callback)(int online), int init)
{
	unsigned ret = -ENODEV;

	if (!callback)
		return -EINVAL;

	if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) != 2) {
		pr_debug("%s: USB_ID pin is not routed to PMIC"
					"on V1 surf/ffa\n", __func__);
		return -ENOTSUPP;
	}

	if (machine_is_msm8x60_ffa() || machine_is_tenderloin()) {
		pr_debug("%s: USB_ID is not routed to PMIC"
			"on V2 ffa\n", __func__);
		return -ENOTSUPP;
	}

	if (init) {
		notify_vbus_state_func_ptr = callback;
		ret = pm8xxx_mpp_config_digital_out(1,
			PM8901_MPP_DIG_LEVEL_L5, 1);
		if (ret) {
			pr_err("%s: MPP2 configuration failed\n", __func__);
			return -ENODEV;
		}
		INIT_DELAYED_WORK(&pmic_id_det, pmic_id_detect);
		ret = request_threaded_irq(PMICID_INT, NULL, pmic_id_on_irq,
			(IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING),
						"msm_otg_id", NULL);
		if (ret) {
			pm8xxx_mpp_config_digital_out(1,
					PM8901_MPP_DIG_LEVEL_L5, 0);
			pr_err("%s:pmic_usb_id interrupt registration failed",
					__func__);
			return ret;
		}
	} else {
		free_irq(PMICID_INT, 0);
		cancel_delayed_work_sync(&pmic_id_det);
		notify_vbus_state_func_ptr = NULL;
		ret = pm8xxx_mpp_config_digital_out(1,
			PM8901_MPP_DIG_LEVEL_L5, 0);
		if (ret) {
			pr_err("%s:MPP2 configuration failed\n", __func__);
			return -ENODEV;
		}
	}
	return 0;
}
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
	static struct regulator *ext_5v_reg;
	static int vbus_is_on;

	/* If VBUS is already on (or off), do nothing. */
	if (on == vbus_is_on)
		return;

	if (!votg_5v_switch) {
		votg_5v_switch = regulator_get(NULL, "8901_usb_otg");
		if (IS_ERR(votg_5v_switch)) {
			pr_err("%s: unable to get votg_5v_switch\n", __func__);
			return;
		}
	}
	if (!ext_5v_reg) {
		ext_5v_reg = regulator_get(NULL, "8901_mpp0");
		if (IS_ERR(ext_5v_reg)) {
			pr_err("%s: unable to get ext_5v_reg\n", __func__);
			return;
		}
	}
	if (on) {
		if (regulator_enable(ext_5v_reg)) {
			pr_err("%s: Unable to enable the regulator:"
					" ext_5v_reg\n", __func__);
			return;
		}
		if (regulator_enable(votg_5v_switch)) {
			pr_err("%s: Unable to enable the regulator:"
					" votg_5v_switch\n", __func__);
			return;
		}
	} else {
		if (regulator_disable(votg_5v_switch))
			pr_err("%s: Unable to enable the regulator:"
				" votg_5v_switch\n", __func__);
		if (regulator_disable(ext_5v_reg))
			pr_err("%s: Unable to enable the regulator:"
				" ext_5v_reg\n", __func__);
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
	.hsdrvslope		 = 0x05,
#ifdef CONFIG_USB_EHCI_MSM_72K
	.pmic_id_notif_init = msm_hsusb_pmic_id_notif_init,
#endif
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
