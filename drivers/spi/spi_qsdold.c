/* linux/driver/spi/spi_qsd.c 
 *
 * Copyright (C) 2009 Solomon Chiu <solomon_chiu@htc.com>
 *
 * 	This is a temporary solution to substitute Qualcomm's SPI.
 *	Should be replaced by formal SPI driver in the future.
 */
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/module.h>
#include <asm/gpio.h>
#include <mach/msm_iomap.h>

#include <mach/../../devices.h>

#define SPI_CONFIG              (0x00000000)
#define SPI_IO_CONTROL          (0x00000004)
#define SPI_OPERATIONAL         (0x00000030)
#define SPI_ERROR_FLAGS_EN      (0x00000038)
#define SPI_ERROR_FLAGS         (0x00000034)
#define SPI_OUTPUT_FIFO         (0x00000100)

struct spi_device *spidev;

int qspi_send_16bit(unsigned char id, unsigned data)
{
	unsigned char buffer[2];
	int tmp;
	tmp = (id<<13 | data)<<16;

	buffer[0] = tmp >> 24;
	buffer[1] = (tmp & 0x00FF0000) >> 16;
	spi_write(spidev,buffer, 2);
	return 0;
}

int qspi_send_9bit(struct spi_msg *msg)
{
	int tmp = 0;

	if(!spidev) {
		printk("%s: spidev is NULL!\n", __func__);
		return 0;
	}

	spidev->bits_per_word = 9;
	tmp = (0x0 <<8 | msg->cmd)<<23;
	msg->buffer[0] = tmp >> 24;
	msg->buffer[1] = (tmp & 0x00FF0000) >> 16;

	if(msg->len != 0) {
		int i = 0, j;
		for(j = 2; i < msg->len; i++, j+=2){
			tmp &= 0x00000000;
			tmp = (0x1<<8 | *(msg->data+i))<<23;
			msg->buffer[j] = tmp >> 24;
			msg->buffer[j+1] = (tmp & 0x00FF0000) >> 16;
		}
	}

	spi_read_write_lock(spidev, msg, NULL, 2, 1);

	return 0;
}

int qspi_send(unsigned char id, unsigned data)
{
	unsigned char buffer[2];
	int tmp;
	tmp = (0x7000 | id<<9 | data)<<16;

	spidev->bits_per_word = 16;

	buffer[0] = tmp >> 24;
	buffer[1] = (tmp & 0x00FF0000) >> 16;

	spi_write(spidev,buffer,2);
	return 0;
}

static int msm_spi_probe(struct spi_device *spi)
{
	printk("%s\n", __func__);
	spidev = spi;
	return 0 ;
}

static int msm_spi_remove(struct spi_device *pdev)
{
	spidev = NULL;
	return 0;
}

static struct spi_driver spi_qsd = {
	.driver = {
		.name  = "spi_qsd",
		.owner = THIS_MODULE,
	},
	.probe         = msm_spi_probe,
	.remove        = msm_spi_remove,
};

static int __init spi_qsd_init(void)
{
	return spi_register_driver(&spi_qsd);
}
module_init(spi_qsd_init);

static void __exit spi_qsd_exit(void)
{
	spi_unregister_driver(&spi_qsd);
}
module_exit(spi_qsd_exit);

