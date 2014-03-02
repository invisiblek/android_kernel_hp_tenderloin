/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
*
* File Name          : lsm303dlh.h
* Authors            : MSH - Motion Mems BU - Application Team
*		     : Carmine Iascone (carmine.iascone@st.com)
*		     : Matteo Dameno (matteo.dameno@st.com)
* Version            : V 1.6.0
* Date               : 2011/02/28
* Description        : LSM303DLH 6D module sensor API
*
********************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* THE PRESENT SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES
* OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, FOR THE SOLE
* PURPOSE TO SUPPORT YOUR APPLICATION DEVELOPMENT.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
*******************************************************************************/

#ifndef __LSM303DLH_H__
#define __LSM303DLH_H__

#define SAD0L				0x00
#define SAD0H				0x01
#define LSM303DLH_ACC_I2C_SADROOT	0x0C
#define LSM303DLH_ACC_I2C_SAD_L	((LSM303DLH_ACC_I2C_SADROOT<<1)|SAD0L)
#define LSM303DLH_ACC_I2C_SAD_H	((LSM303DLH_ACC_I2C_SADROOT<<1)|SAD0H)
#define	LSM303DLH_ACC_DEV_NAME	"lsm303dlh_acc_sysfs"


#define LSM303DLH_MAG_I2C_SAD		0x1E
#define	LSM303DLH_MAG_DEV_NAME	"lsm303dlh_mag_sysfs"


#define LSM303DLH_ACC_MIN_POLL_PERIOD_MS 1
#define LSM303DLH_MAG_MIN_POLL_PERIOD_MS 5

#define LSM303DLH_ACC_DEFAULT_INT1_GPIO (-EINVAL)
#define LSM303DLH_ACC_DEFAULT_INT2_GPIO (-EINVAL)


/************************************************/
/* 	Accelerometer section defines	 	*/
/************************************************/

/* Accelerometer Sensor Full Scale */
#define	LSM303DLH_ACC_FS_MASK		0x30
#define LSM303DLH_ACC_G_2G 		0x00
#define LSM303DLH_ACC_G_4G 		0x10
#define LSM303DLH_ACC_G_8G 		0x30

/* Accelerometer Sensor Operating Mode */
#define LSM303DLH_ACC_ENABLE		0x01
#define LSM303DLH_ACC_DISABLE		0x00
#define LSM303DLH_ACC_PM_NORMAL		0x20
#define LSM303DLH_ACC_PM_OFF		LSM303DLH_ACC_DISABLE


/* RESUME STATE INDICES */
#define	RES_CTRL_REG1		0
#define	RES_CTRL_REG2		1
#define	RES_CTRL_REG3		2
#define	RES_CTRL_REG4		3
#define	RES_CTRL_REG5		4
#define	RES_REFERENCE		5

#define	RES_INT_CFG1		6
#define	RES_INT_THS1		7
#define	RES_INT_DUR1		8
#define	RES_INT_CFG2		9
#define	RES_INT_THS2		10
#define	RES_INT_DUR2		11

#define	RESUME_ENTRIES		12
/* end RESUME STATE INDICES */

/************************************************/
/* 	Magnetometer section defines	 	*/
/************************************************/

/* Magnetometer Sensor Full Scale */
#define LSM303DLH_MAG_H_1_3G		0x20
#define LSM303DLH_MAG_H_1_9G		0x40
#define LSM303DLH_MAG_H_2_5G		0x60
#define LSM303DLH_MAG_H_4_0G		0x80
#define LSM303DLH_MAG_H_4_7G		0xA0
#define LSM303DLH_MAG_H_5_6G		0xC0
#define LSM303DLH_MAG_H_8_1G		0xE0

/* Magnetic Sensor Operating Mode */
#define LSM303DLH_MAG_NORMAL_MODE	0x00
#define LSM303DLH_MAG_POS_BIAS		0x01
#define LSM303DLH_MAG_NEG_BIAS		0x02
#define LSM303DLH_MAG_CC_MODE		0x00
#define LSM303DLH_MAG_SC_MODE		0x01
#define LSM303DLH_MAG_SLEEP_MODE	0x03


#ifdef __KERNEL__
struct lsm303dlh_acc_platform_data {

	int poll_interval;
	int min_interval;

	u8 g_range;

	u8 axis_map_x;
	u8 axis_map_y;
	u8 axis_map_z;

	u8 negate_x;
	u8 negate_y;
	u8 negate_z;

	int (*init)(void);
	void (*exit)(void);
	int (*power_on)(void);
	int (*power_off)(void);

	/* set gpio_int[1,2] either to the choosen gpio pin number or to -EINVAL
	 * if leaved unconnected
	 */
	int gpio_int1;
	int gpio_int2;
};

struct lsm303dlh_acc_data {
	struct i2c_client *client;
	struct lsm303dlh_acc_platform_data *pdata;

	struct mutex lock;
	struct delayed_work input_work;

	struct input_dev *input_dev;

	int hw_initialized;
	/* hw_working=-1 means not tested yet */
	int hw_working;
	int selftest_enabled;

	atomic_t enabled;
	int on_before_suspend;

	u8 sensitivity;

	u8 resume_state[RESUME_ENTRIES];

	int irq1;
	struct work_struct irq1_work;
	struct workqueue_struct *irq1_work_queue;
	int irq2;
	struct work_struct irq2_work;
	struct workqueue_struct *irq2_work_queue;

	u8 reg_addr;

	int ext_adap_enabled;
	void *ext_handle;
};

struct lsm303dlh_mag_platform_data {

	int poll_interval;
	int min_interval;

	u8 h_range;

	u8 axis_map_x;
	u8 axis_map_y;
	u8 axis_map_z;

	u8 negate_x;
	u8 negate_y;
	u8 negate_z;

	int (*init)(void);
	void (*exit)(void);
	int (*power_on)(void);
	int (*power_off)(void);

};

struct lsm303dlh_mag_data {
	struct i2c_client *client;
	struct lsm303dlh_mag_platform_data *pdata;

	struct mutex lock;

	struct input_polled_dev *input_poll_dev;

	int hw_initialized;
	atomic_t enabled;
	atomic_t self_test_enabled;

	u16 xy_sensitivity;
	u16 z_sensitivity;

	u8 resume_state[3];

	u8 reg_addr;

	int on_before_suspend;
	int ext_adap_enabled;
	void *ext_handle;
};
#endif /* __KERNEL__ */

#endif  /* __LSM303DLH_H__ */
