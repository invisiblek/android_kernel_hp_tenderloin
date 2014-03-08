#ifndef __MPU_WRAPPER_H__
#define __MPU_WRAPPER_H__

#ifdef CONFIG_MACH_HTC
#include <linux/mpu_htc.h>
#else
#include <linux/mpu_stock.h>
#endif

#endif
