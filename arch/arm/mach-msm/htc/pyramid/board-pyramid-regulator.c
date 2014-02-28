/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/regulator/pmic8901-regulator.h>
#include <linux/regulator/msm-gpio-regulator.h>
#include <mach/rpm-regulator.h>

#include "board-pyramid.h"

#define VREG_CONSUMERS(_id)                                       \
  static struct regulator_consumer_supply vreg_consumers_##_id[]

// CONSUMERS
// PM8058
VREG_CONSUMERS(PM8058_L0) = {
	REGULATOR_SUPPLY("8058_l0",		NULL),
};
VREG_CONSUMERS(PM8058_L1) = {
	REGULATOR_SUPPLY("8058_l1",		NULL),
};
VREG_CONSUMERS(PM8058_L2) = {
	REGULATOR_SUPPLY("8058_l2",		NULL),
};
VREG_CONSUMERS(PM8058_L3) = {
	REGULATOR_SUPPLY("8058_l3",		NULL),
};
VREG_CONSUMERS(PM8058_L4) = {
	REGULATOR_SUPPLY("8058_l4",		NULL),
};
VREG_CONSUMERS(PM8058_L5) = {
	REGULATOR_SUPPLY("8058_l5",		NULL),
};
VREG_CONSUMERS(PM8058_L6) = {
	REGULATOR_SUPPLY("8058_l6",		NULL),
        REGULATOR_SUPPLY("HSUSB_3p3",           "msm_otg"),
};
VREG_CONSUMERS(PM8058_L7) = {
	REGULATOR_SUPPLY("8058_l7",		NULL),
        REGULATOR_SUPPLY("HSUSB_1p8",           "msm_otg"),
};
VREG_CONSUMERS(PM8058_L8) = {
	REGULATOR_SUPPLY("8058_l8",		NULL),
};
VREG_CONSUMERS(PM8058_L9) = {
	REGULATOR_SUPPLY("8058_l9",		NULL),
};
VREG_CONSUMERS(PM8058_L10) = {
	REGULATOR_SUPPLY("8058_l10",		NULL),
};
VREG_CONSUMERS(PM8058_L11) = {
	REGULATOR_SUPPLY("8058_l11",		NULL),
};
VREG_CONSUMERS(PM8058_L12) = {
	REGULATOR_SUPPLY("8058_l12",		NULL),
};
VREG_CONSUMERS(PM8058_L13) = {
	REGULATOR_SUPPLY("8058_l13",		NULL),
};
VREG_CONSUMERS(PM8058_L14) = {
	REGULATOR_SUPPLY("8058_l14",		NULL),
};
VREG_CONSUMERS(PM8058_L15) = {
	REGULATOR_SUPPLY("8058_l15",		NULL),
};
VREG_CONSUMERS(PM8058_L16) = {
	REGULATOR_SUPPLY("8058_l16",		NULL),
};
VREG_CONSUMERS(PM8058_L17) = {
	REGULATOR_SUPPLY("8058_l17",		NULL),
};
VREG_CONSUMERS(PM8058_L18) = {
	REGULATOR_SUPPLY("8058_l18",		NULL),
};
VREG_CONSUMERS(PM8058_L19) = {
	REGULATOR_SUPPLY("8058_l19",		NULL),
};
VREG_CONSUMERS(PM8058_L20) = {
	REGULATOR_SUPPLY("8058_l20",		NULL),
};
VREG_CONSUMERS(PM8058_L21) = {
	REGULATOR_SUPPLY("8058_l21",		NULL),
};
VREG_CONSUMERS(PM8058_L22) = {
	REGULATOR_SUPPLY("8058_l22",		NULL),
};
VREG_CONSUMERS(PM8058_L23) = {
	REGULATOR_SUPPLY("8058_l23",		NULL),
};
VREG_CONSUMERS(PM8058_L24) = {
	REGULATOR_SUPPLY("8058_l24",		NULL),
};
VREG_CONSUMERS(PM8058_L25) = {
	REGULATOR_SUPPLY("8058_l25",		NULL),
};
VREG_CONSUMERS(PM8058_S0) = {
	REGULATOR_SUPPLY("8058_s0",		NULL),
};
VREG_CONSUMERS(PM8058_S1) = {
	REGULATOR_SUPPLY("8058_s1",		NULL),
        REGULATOR_SUPPLY("HSUSB_VDDCX",         "msm_otg"),
};
VREG_CONSUMERS(PM8058_S2) = {
	REGULATOR_SUPPLY("8058_s2",		NULL),
};
VREG_CONSUMERS(PM8058_S3) = {
	REGULATOR_SUPPLY("8058_s3",		NULL),
};
VREG_CONSUMERS(PM8058_S4) = {
	REGULATOR_SUPPLY("8058_s4",		NULL),
};
VREG_CONSUMERS(PM8058_LVS0) = {
	REGULATOR_SUPPLY("8058_lvs0",		NULL),
};
VREG_CONSUMERS(PM8058_LVS1) = {
	REGULATOR_SUPPLY("8058_lvs1",		NULL),
};
VREG_CONSUMERS(PM8058_NCP) = {
	REGULATOR_SUPPLY("8058_ncp",		NULL),
};

// PM8901
VREG_CONSUMERS(PM8901_L0) = {
	REGULATOR_SUPPLY("8901_l0",		NULL),
};
VREG_CONSUMERS(PM8901_L1) = {
	REGULATOR_SUPPLY("8901_l1",		NULL),
};
VREG_CONSUMERS(PM8901_L2) = {
	REGULATOR_SUPPLY("8901_l2",		NULL),
};
VREG_CONSUMERS(PM8901_L3) = {
	REGULATOR_SUPPLY("8901_l3",		NULL),
};
VREG_CONSUMERS(PM8901_L4) = {
	REGULATOR_SUPPLY("8901_l4",		NULL),
};
VREG_CONSUMERS(PM8901_L5) = {
	REGULATOR_SUPPLY("8901_l5",		NULL),
};
VREG_CONSUMERS(PM8901_L6) = {
	REGULATOR_SUPPLY("8901_l6",		NULL),
};
VREG_CONSUMERS(PM8901_S0) = {
	REGULATOR_SUPPLY("8901_s0",		NULL),
};
VREG_CONSUMERS(PM8901_S1) = {
	REGULATOR_SUPPLY("8901_s1",		NULL),
};
VREG_CONSUMERS(PM8901_S2) = {
	REGULATOR_SUPPLY("8901_s2",		NULL),
};
VREG_CONSUMERS(PM8901_S3) = {
	REGULATOR_SUPPLY("8901_s3",		NULL),
};
VREG_CONSUMERS(PM8901_S4) = {
	REGULATOR_SUPPLY("8901_s4",		NULL),
};
VREG_CONSUMERS(PM8901_LVS0) = {
	REGULATOR_SUPPLY("8901_lvs0",		NULL),
};
VREG_CONSUMERS(PM8901_LVS1) = {
	REGULATOR_SUPPLY("8901_lvs1",		NULL),
};
VREG_CONSUMERS(PM8901_LVS2) = {
	REGULATOR_SUPPLY("8901_lvs2",		NULL),
};
VREG_CONSUMERS(PM8901_LVS3) = {
	REGULATOR_SUPPLY("8901_lvs3",		NULL),
};
VREG_CONSUMERS(PM8901_MVS0) = {
	REGULATOR_SUPPLY("8901_mvs0",		NULL),
};
VREG_CONSUMERS(EXT_5V) = {
	REGULATOR_SUPPLY("ext_5v",              NULL),
	REGULATOR_SUPPLY("8901_mpp0",           NULL),
};

VREG_CONSUMERS(PM8901_USB_OTG) = {
	REGULATOR_SUPPLY("8901_usb_otg",	NULL),
};
VREG_CONSUMERS(PM8901_HDMI_MVS) = {
	REGULATOR_SUPPLY("8901_hdmi_mvs",	NULL),
};

VREG_CONSUMERS(PM8058_S2_PC) = {
	REGULATOR_SUPPLY("8058_s2_pc",		NULL),
};
VREG_CONSUMERS(PM8901_S4_PC) = {
	REGULATOR_SUPPLY("8901_s4_pc",		NULL),
};



#define RPM_VREG_INIT(_id, _min_uV, _max_uV, _modes, _ops, _apply_uV, \
		      _default_uV, _peak_uA, _avg_uA, _pull_down, _pin_ctrl, \
		      _freq, _pin_fn, _force_mode, _sleep_set_force_mode, \
		      _state, _sleep_selectable, _always_on) \
	{ \
		.init_data = { \
			.constraints = { \
				.valid_modes_mask	= _modes, \
				.valid_ops_mask		= _ops, \
				.min_uV			= _min_uV, \
				.max_uV			= _max_uV, \
				.input_uV		= _min_uV, \
				.apply_uV		= _apply_uV, \
				.always_on		= _always_on, \
			}, \
			.consumer_supplies	= vreg_consumers_##_id, \
			.num_consumer_supplies	= \
				ARRAY_SIZE(vreg_consumers_##_id), \
		}, \
		.id			= RPM_VREG_ID_##_id, \
		.default_uV		= _default_uV, \
		.peak_uA		= _peak_uA, \
		.avg_uA			= _avg_uA, \
		.pull_down_enable	= _pull_down, \
		.pin_ctrl		= _pin_ctrl, \
		.freq			= RPM_VREG_FREQ_##_freq, \
		.pin_fn			= _pin_fn, \
		.force_mode		= _force_mode, \
		.sleep_set_force_mode	= _sleep_set_force_mode, \
		.state			= _state, \
		.sleep_selectable	= _sleep_selectable, \
	}

/* Pin control initialization */
#define RPM_PC(_id, _always_on, _pin_fn, _pin_ctrl) \
	{ \
		.init_data = { \
			.constraints = { \
				.valid_ops_mask	= REGULATOR_CHANGE_STATUS, \
				.always_on	= _always_on, \
			}, \
			.num_consumer_supplies	= \
					ARRAY_SIZE(vreg_consumers_##_id##_PC), \
			.consumer_supplies	= vreg_consumers_##_id##_PC, \
		}, \
		.id	  = RPM_VREG_ID_##_id##_PC, \
		.pin_fn	  = RPM_VREG_PIN_FN_8660_##_pin_fn, \
		.pin_ctrl = _pin_ctrl, \
	}

/*
 * The default LPM/HPM state of an RPM controlled regulator can be controlled
 * via the peak_uA value specified in the table below.  If the value is less
 * than the high power min threshold for the regulator, then the regulator will
 * be set to LPM.  Otherwise, it will be set to HPM.
 *
 * This value can be further overridden by specifying an initial mode via
 * .init_data.constraints.initial_mode.
 */

#define RPM_LDO(_id, _always_on, _pd, _sleep_selectable, _min_uV, _max_uV, \
		_init_peak_uA) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, RPM_VREG_PIN_CTRL_NONE, NONE, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_SMPS(_id, _always_on, _pd, _sleep_selectable, _min_uV, _max_uV, \
		 _init_peak_uA, _freq) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_FAST | \
		      REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE | \
		      REGULATOR_MODE_STANDBY, REGULATOR_CHANGE_VOLTAGE | \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE | \
		      REGULATOR_CHANGE_DRMS, 0, _min_uV, _init_peak_uA, \
		      _init_peak_uA, _pd, RPM_VREG_PIN_CTRL_NONE, _freq, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_VS(_id, _always_on, _pd, _sleep_selectable) \
	RPM_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL | REGULATOR_MODE_IDLE, \
		      REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_MODE, 0, 0, \
		      1000, 1000, _pd, RPM_VREG_PIN_CTRL_NONE, NONE, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define RPM_NCP(_id, _always_on, _pd, _sleep_selectable, _min_uV, _max_uV) \
	RPM_VREG_INIT(_id, _min_uV, _max_uV, REGULATOR_MODE_NORMAL, \
		      REGULATOR_CHANGE_VOLTAGE | REGULATOR_CHANGE_STATUS, 0, \
		      _min_uV, 1000, 1000, _pd, RPM_VREG_PIN_CTRL_NONE, NONE, \
		      RPM_VREG_PIN_FN_8660_ENABLE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, \
		      RPM_VREG_FORCE_MODE_8660_NONE, RPM_VREG_STATE_OFF, \
		      _sleep_selectable, _always_on)

#define GPIO_VREG(_id, _reg_name, _gpio_label, _gpio, _supply_regulator) \
	[GPIO_VREG_ID_##_id] = { \
		.init_data = { \
			.constraints = { \
				.valid_ops_mask	= REGULATOR_CHANGE_STATUS, \
			}, \
			.num_consumer_supplies	= \
					ARRAY_SIZE(vreg_consumers_##_id), \
			.consumer_supplies	= vreg_consumers_##_id, \
			.supply_regulator	= _supply_regulator, \
		}, \
		.regulator_name = _reg_name, \
		.gpio_label	= _gpio_label, \
		.gpio		= _gpio, \
	}

#define SAW_VREG_INIT(_id, _name, _min_uV, _max_uV) \
	{ \
		.constraints = { \
			.name		= _name, \
			.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE | \
					  REGULATOR_CHANGE_STATUS, \
			.min_uV		= _min_uV, \
			.max_uV		= _max_uV, \
		}, \
		.num_consumer_supplies	= ARRAY_SIZE(vreg_consumers_##_id), \
		.consumer_supplies	= vreg_consumers_##_id, \
	}

#define PM8901_VREG_INIT(_id, _min_uV, _max_uV, _modes, _ops, _apply_uV, \
			 _always_on) \
	{ \
		.init_data = { \
			.constraints = { \
				.valid_modes_mask = _modes, \
				.valid_ops_mask = _ops, \
				.min_uV = _min_uV, \
				.max_uV = _max_uV, \
				.input_uV = _min_uV, \
				.apply_uV = _apply_uV, \
				.always_on = _always_on, \
			}, \
			.consumer_supplies = vreg_consumers_PM8901_##_id, \
			.num_consumer_supplies = \
				ARRAY_SIZE(vreg_consumers_PM8901_##_id), \
		}, \
		.id = PM8901_VREG_ID_##_id, \
	}

#define PM8901_VREG_INIT_MPP(_id, _active_high) \
	PM8901_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL, \
			REGULATOR_CHANGE_STATUS, 0, 0, _active_high)

#define PM8901_VREG_INIT_VS(_id) \
	PM8901_VREG_INIT(_id, 0, 0, REGULATOR_MODE_NORMAL, \
			REGULATOR_CHANGE_STATUS, 0, 0)

struct pm8901_vreg_pdata pm8901_regulator_pdata[] = {
	PM8901_VREG_INIT_VS(USB_OTG),
	PM8901_VREG_INIT_VS(HDMI_MVS),
};

int pm8901_regulator_pdata_len __devinitdata =
         ARRAY_SIZE(pm8901_regulator_pdata);

#define LDO50HMIN	RPM_VREG_8660_LDO_50_HPM_MIN_LOAD
#define LDO150HMIN	RPM_VREG_8660_LDO_150_HPM_MIN_LOAD
#define LDO300HMIN	RPM_VREG_8660_LDO_300_HPM_MIN_LOAD
#define SMPS_HMIN	RPM_VREG_8660_SMPS_HPM_MIN_LOAD
#define FTS_HMIN	RPM_VREG_8660_FTSMPS_HPM_MIN_LOAD
#define GPIO_VREG_ID_EXT_5V		0

/* GPIO regulator constraints */
static struct gpio_regulator_platform_data msm_gpio_regulator_pdata[] = {
	GPIO_VREG(EXT_5V, "ext_5v", "ext_5v_en",
					PM8901_MPP_PM_TO_SYS(0), 0),
};

/* GPIO regulator */
struct platform_device msm8x60_8901_mpp_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8901_MPP_PM_TO_SYS(0),
	.dev	= {
		.platform_data =
			&msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_5V],
	},
};

struct pm8xxx_mpp_init_info {
	unsigned			mpp;
	struct pm8xxx_mpp_config_data	config;
};

void __init pyramid_pm8901_gpio_mpp_init(void)
{
	int rc;

	struct pm8xxx_mpp_init_info pm8901_vreg_mpp0 = {
		.mpp	= PM8901_MPP_PM_TO_SYS(0),
		.config =  {
			.type	= PM8XXX_MPP_TYPE_D_OUTPUT,
			.level	= PM8901_MPP_DIG_LEVEL_VPH,
		},
	};

	/*
	 * Set PMIC 8901 MPP0 active_high to 0 for surf and charm_surf. This
	 * implies that the regulator connected to MPP0 is enabled when
	 * MPP0 is low.
	 */
        msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_5V].active_low = 0;
        pm8901_vreg_mpp0.config.control = PM8XXX_MPP_DOUT_CTRL_LOW;

	rc = pm8xxx_mpp_config(pm8901_vreg_mpp0.mpp, &pm8901_vreg_mpp0.config);
	if (rc)
		pr_err("%s: pm8xxx_mpp_config: rc=%d\n", __func__, rc);
}

/* SAW regulator constraints */
struct regulator_init_data msm_saw_regulator_pdata_s0 =
	/*	      ID  vreg_name	       min_uV   max_uV */
	SAW_VREG_INIT(PM8901_S0, "8901_s0",	       800000, 1325000);
struct regulator_init_data msm_saw_regulator_pdata_s1 =
	SAW_VREG_INIT(PM8901_S1, "8901_s1",	       800000, 1325000);

/* RPM regulator constraints */
static struct rpm_regulator_init_data rpm_regulator_early_init_data[] __devinitdata = {
	/*	 ID       a_on pd ss min_uV   max_uV   init_ip    freq */
	RPM_SMPS(PM8058_S0, 0, 1, 1,  500000, 1250000, SMPS_HMIN, 1p92),
	RPM_SMPS(PM8058_S1, 0, 1, 1,  500000, 1250000, SMPS_HMIN, 1p92),
};

static struct rpm_regulator_init_data rpm_regulator_init_data[] __devinitdata = {
	/*	ID        a_on pd ss min_uV   max_uV   init_ip */
	RPM_LDO(PM8058_L0,  0, 0, 0, 1200000, 1200000, LDO150HMIN),
	RPM_LDO(PM8058_L1,  0, 1, 0, 1350000, 1350000, LDO300HMIN),
	RPM_LDO(PM8058_L2,  0, 1, 0, 1800000, 2600000, LDO300HMIN),
	RPM_LDO(PM8058_L3,  0, 1, 0, 1800000, 3000000, LDO150HMIN),
	RPM_LDO(PM8058_L4,  0, 1, 0, 2850000, 2850000,  LDO50HMIN),
	RPM_LDO(PM8058_L5,  0, 0, 0, 2850000, 2850000, LDO300HMIN),
	RPM_LDO(PM8058_L6,  0, 0, 0, 3000000, 3600000,  LDO50HMIN),
	RPM_LDO(PM8058_L7,  0, 0, 0, 1800000, 1800000,  LDO50HMIN),
	RPM_LDO(PM8058_L8,  0, 1, 0, 2850000, 2850000, LDO300HMIN),
	RPM_LDO(PM8058_L9,  0, 1, 0, 1800000, 1800000, LDO300HMIN),
	RPM_LDO(PM8058_L10, 0, 1, 0, 2850000, 2850000, LDO300HMIN),
	RPM_LDO(PM8058_L11, 0, 1, 0, 1800000, 1800000, LDO150HMIN),
	RPM_LDO(PM8058_L12, 0, 1, 0, 1800000, 1800000, LDO150HMIN),
	RPM_LDO(PM8058_L13, 0, 1, 0, 2050000, 2050000, LDO300HMIN),
	RPM_LDO(PM8058_L14, 0, 1, 0, 2850000, 2850000, LDO300HMIN),
	RPM_LDO(PM8058_L15, 0, 1, 0, 2800000, 2800000, LDO300HMIN),
	RPM_LDO(PM8058_L16, 1, 1, 1, 1800000, 1800000, LDO300HMIN),
	RPM_LDO(PM8058_L17, 0, 1, 0, 2600000, 2600000, LDO150HMIN),
	RPM_LDO(PM8058_L18, 0, 1, 1, 2200000, 2200000, LDO150HMIN),
	RPM_LDO(PM8058_L19, 0, 1, 0, 1800000, 1800000, LDO150HMIN),
	RPM_LDO(PM8058_L20, 0, 1, 0, 1800000, 1800000, LDO150HMIN),
	RPM_LDO(PM8058_L21, 1, 0, 0, 1200000, 1200000, LDO150HMIN),
	RPM_LDO(PM8058_L22, 0, 0, 0, 1140000, 1300000, LDO300HMIN),
	RPM_LDO(PM8058_L23, 0, 0, 0, 1200000, 1200000, LDO300HMIN),
	RPM_LDO(PM8058_L24, 0, 1, 0, 1200000, 1200000, LDO150HMIN),
	RPM_LDO(PM8058_L25, 0, 1, 0, 1200000, 1200000, LDO150HMIN),

	/*	 ID       a_on pd ss min_uV   max_uV   init_ip    freq */
	RPM_SMPS(PM8058_S2, 0, 1, 0, 1200000, 1400000, SMPS_HMIN, 1p92),
	RPM_SMPS(PM8058_S3, 1, 1, 0, 1800000, 1800000, SMPS_HMIN, 1p92),
	RPM_SMPS(PM8058_S4, 1, 1, 0, 2200000, 2200000, SMPS_HMIN, 1p92),

	/*     ID         a_on pd ss */
	RPM_VS(PM8058_LVS0, 0, 1, 0),
	RPM_VS(PM8058_LVS1, 0, 1, 0),

	/*	ID        a_on pd ss min_uV   max_uV */
	RPM_NCP(PM8058_NCP, 0, 1, 0, 1800000, 1800000),

	/*	ID        a_on pd ss min_uV   max_uV   init_ip */
	RPM_LDO(PM8901_L0,  0, 1, 0, 1200000, 1200000, LDO300HMIN),
	RPM_LDO(PM8901_L1,  0, 1, 0, 3100000, 3100000, LDO300HMIN),
	RPM_LDO(PM8901_L2,  0, 0, 0, 2850000, 3300000, LDO300HMIN),
	RPM_LDO(PM8901_L3,  0, 1, 0, 3300000, 3300000, LDO300HMIN),
	RPM_LDO(PM8901_L4,  0, 1, 0, 1800000, 1800000, LDO300HMIN),
	RPM_LDO(PM8901_L5,  0, 0, 0, 2850000, 2850000, LDO300HMIN),
	RPM_LDO(PM8901_L6,  0, 1, 0, 2200000, 2200000, LDO300HMIN),

	/*	 ID       a_on pd ss min_uV   max_uV   init_ip   freq */
	RPM_SMPS(PM8901_S2, 0, 1, 0, 1300000, 1300000, FTS_HMIN, 1p60),
	RPM_SMPS(PM8901_S3, 0, 1, 0, 1100000, 1100000, FTS_HMIN, 1p60),
	RPM_SMPS(PM8901_S4, 0, 1, 0, 1140000, 1300000, FTS_HMIN, 1p60),

	/*	ID        a_on pd ss */
	RPM_VS(PM8901_LVS0, 0, 0, 0),
	RPM_VS(PM8901_LVS1, 0, 1, 0),
	RPM_VS(PM8901_LVS2, 0, 1, 0),
	RPM_VS(PM8901_LVS3, 0, 1, 0),
	RPM_VS(PM8901_MVS0, 0, 1, 0),

	/*     ID         a_on pin_func pin_ctrl */
	RPM_PC(PM8058_S2,   0, ENABLE,  RPM_VREG_PIN_CTRL_PM8058_A0),
	RPM_PC(PM8901_S4,   0, ENABLE,  RPM_VREG_PIN_CTRL_PM8901_A0),
};

struct rpm_regulator_platform_data pyramid_rpm_regulator_early_pdata __devinitdata = {
	.init_data		= rpm_regulator_early_init_data,
	.num_regulators		= ARRAY_SIZE(rpm_regulator_early_init_data),
	.version		= RPM_VREG_VERSION_8660,
	.vreg_id_vdd_mem	= RPM_VREG_ID_PM8058_S0,
	.vreg_id_vdd_dig	= RPM_VREG_ID_PM8058_S1,
};

struct rpm_regulator_platform_data pyramid_rpm_regulator_pdata __devinitdata = {
	.init_data		= rpm_regulator_init_data,
	.num_regulators		= ARRAY_SIZE(rpm_regulator_init_data),
	.version		= RPM_VREG_VERSION_8660,
};

