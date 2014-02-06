#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include <asm/uaccess.h>
#include <asm/mach-types.h>

/* sdio function */
#include <linux/mmc/core.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>

#include "sdio_emapi.h"

static struct _emapi emapi;

/* calibration read func */
static int emapi_calibration_read(char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	unsigned char *nvs;
	unsigned long len;

	nvs = get_wifi_nvs_ram();
	if (nvs) {
		/* max size is 2048, we read max size by default*/
		len = 2048;
		memcpy((void *)page, (void *) nvs, len);
		printk("%s - read %d bytes\n", __func__, (unsigned int)len);
		return len;
	} else {
		printk("%s - no calibration data\n", __func__);
		return -EIO;
	}

	return 0;
}

static int emapi_calibration_write(struct file *file, const char __user *buffer,
		unsigned long count, void *data)
{
	printk("%s do nothing\n", __func__);
	return 0;
}

/* most content happens here */
static long emapi_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int error = 0;
	int retval = 0;
	struct sdio_request req;

	if (_IOC_TYPE(cmd) != MAGIC)
		return -ENOTTY;
	if (_IOC_NR(cmd) > EMAPI_IOC_MAXNR)
		return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		error = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		error = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (error)
		return -ENOTTY;

	if ((cmd != EMAPI_IOC_OPEN) && (cmd != EMAPI_IOC_CLOSE)) {
		if (copy_from_user(&req, (void __user *) arg, sizeof(struct sdio_request)))
			return -EFAULT;
	}

	switch (cmd) {
		case EMAPI_IOC_OPEN:
			printk("sdio open\n");
			break;
		case EMAPI_IOC_CLOSE:
			printk("sdio close\n");
			break;
		case EMAPI_IOC_READ:
			/* printk("sdio read, addr:%x, len:%d\n", req.addr, req.len); */
			retval = emapi_sdio_read(req.buf, req.addr, req.len);
			break;
		case EMAPI_IOC_WRITE:
			/* printk("sdio write, addr:%x, len:%d\n", req.addr, req.len); */
			retval = emapi_sdio_write(req.buf, req.addr, req.len);
			break;
		default:
			printk("emapi: unknown command\n");
			return -ENOTTY;
	};

	return retval;
}

static int emapi_open(struct inode *inode, struct file *filp)
{
	printk("emapi is open\n");
	return nonseekable_open(inode, filp);
}

static int emapi_release(struct inode *inode, struct file *filp)
{
	printk("emapi is close\n");
	return 0;
}

static struct file_operations emapi_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= emapi_ioctl,
	.open		= emapi_open,
	.release	= emapi_release,
};

static struct miscdevice emapi_device = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEV_NAME,
	.fops	= &emapi_fops,
};

static int emapi_init_module(void)
{
	int rc;

	rc = misc_register(&emapi_device);
	if (rc) {
		printk("emapi: register fail\n");
		return rc;
	}
	emapi.calibration = create_proc_entry("emapi_calibration", 0644, NULL);
	if (!emapi.calibration) {
		printk("emapi: alloc calibration error\n");
		rc = PTR_ERR(emapi.calibration);
		if (misc_deregister(&emapi_device))
			printk("emapi: deregister fail\n");
		return rc;
	}

	emapi.calibration->read_proc = emapi_calibration_read;
	emapi.calibration->write_proc = emapi_calibration_write;
	emapi.calibration->size = WIFI_NVS_MAX_SIZE;
	emapi.sdio_status = 0;
	return 0;
}

static void emapi_exit_module(void)
{
	if (misc_deregister(&emapi_device))
		printk("emapi: deregister fail\n");

	remove_proc_entry("emapi_calibration", NULL);
}

module_init(emapi_init_module);
module_exit(emapi_exit_module);

MODULE_AUTHOR("bowgo_tsai@htc.com");
MODULE_LICENSE("GPL");
