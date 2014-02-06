/*
 * Copyright (C) 2008 HTC, Inc.
 * Author: Max Tsai <max_tsai@htc.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef SDIO_EMAPI_H
#define SDIO_EMAPI_H

#include <linux/ioctl.h>

struct sdio_request {
	unsigned long   addr;
	unsigned long   len;
	char     *buf;
};

struct _emapi {
	struct class		*emapi_class;
	struct device		*device;
	dev_t			emapi_cdevno;
	struct proc_dir_entry	*calibration;
	int			sdio_status;
};

#define DEV_NAME "emapi"

/* Documentation/ioctl-number.txt */
/* the same with linux/wireless.h */
#define MAGIC			0x8B

#define EMAPI_IOC_GETMEM	_IOR(MAGIC, 1, unsigned long)
#define EMAPI_IOC_SETMEM	_IOW(MAGIC, 2, unsigned long)
#define EMAPI_IOC_GETMAC	_IOR(MAGIC, 3, unsigned long)
#define EMAPI_IOC_SETMAC	_IOW(MAGIC, 4, unsigned long)
#define EMAPI_IOC_SETPAR	_IOW(MAGIC, 5, unsigned long)
#define EMAPI_IOC_SETPAR2	_IOW(MAGIC, 6, unsigned long)
#define EMAPI_IOC_GETPAR	_IOR(MAGIC, 7, unsigned long)
#define EMAPI_IOC_GETEEPROM	_IOR(MAGIC, 8, unsigned long)
#define EMAPI_IOC_SETEEPROM	_IOW(MAGIC, 9, unsigned long)
#define EMAPI_IOC_TADDR2SADDR   _IOW(MAGIC, 10, unsigned long)
#define EMAPI_IOC_INIT		_IOW(MAGIC, 11, unsigned long)

#define EMAPI_IOC_READ		_IOR(MAGIC, 12, unsigned long)
#define EMAPI_IOC_WRITE		_IOW(MAGIC, 13, unsigned long)
#define EMAPI_IOC_CLOSE		_IOW(MAGIC, 14, unsigned long)
#define EMAPI_IOC_OPEN		_IOW(MAGIC, 15, unsigned long)

#define EMAPI_IOC_MAXNR	15

#define WIFI_NVS_MAX_SIZE 2048

extern unsigned char *get_wifi_nvs_ram(void);
extern int emapi_sdio_read(char*, unsigned long, unsigned long);
extern int emapi_sdio_write(char*, unsigned long, unsigned long);

#endif  /* SDIO_EMAPI_H */

