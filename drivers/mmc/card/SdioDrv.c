/*
 * SdioDrv.c - MSM Linux SDIO driver (platform and OS dependent)
 *
 * The lower SDIO driver (BSP) for MSM on Linux OS.
 * Provides all SDIO commands and read/write operation methods.
 *
 * Copyright (c) 1998 - 2008 Texas Instruments Incorporated
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>

#include <mach/io.h>
#include <mach/gpio.h>
#include <mach/hardware.h>

#include <asm/io.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>

#include "SdioDrvDbg.h"
#include "SdioDrv.h"

#define SDIO_DRIVER_NAME "TIWLAN_SDIO"

extern int mmc_send_io_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr);
extern int mmc_io_rw_direct(struct mmc_card *card, int write, unsigned fn, unsigned addr, u8 in, u8 *out);
extern int mmc_io_rw_extended(struct mmc_card *card, int write, unsigned fn, unsigned addr, int incr_addr, u8 *buf, unsigned blocks, unsigned blksz);
extern int sdio_reset(struct mmc_host *host);

typedef struct ST_sdiodrv {
	struct mmc_host *mmc;
	struct platform_device *pdev;
	void (*BusTxnCB)(void *BusTxnHandle, int status);
	void *BusTxnHandle;
	unsigned int  uBlkSize;
	unsigned int  uBlkSizeShift;
	int (*wlanDrvIf_pm_resume)(void);
	int (*wlanDrvIf_pm_suspend)(void);
	struct device *dev;
	void (*wlanDrvIf_setResumeConfig)(void);
	void (*wlanDrvIf_setSuspendConfig)(void);
} ST_sdiodrv_t;

ST_sdiodrv_t g_drv;

module_param(g_sdio_debug_level1, int, SDIO_DEBUGLEVEL_ERR);
MODULE_PARM_DESC(g_sdio_debug_level1, "TIWLAN SDIO debug level");

int g_sdio_debug_level1 = SDIO_DEBUGLEVEL_ERR;
EXPORT_SYMBOL(g_sdio_debug_level1);

struct platform_device *sdioDrv_get_platform_device(void)
{
	return g_drv.pdev;
}

int sdioDrv_ReadSync(unsigned int uFunc,
			unsigned int uHwAddr,
			void *pData,
			unsigned int uLen,
			unsigned int bIncAddr,
			unsigned int bMore)
{
	printk(KERN_INFO "We don't use sdioDrv_ReadSync, please call sdioDrv_ReadAsync\n");
	return 0;
}

int sdioDrv_WriteSync(unsigned int uFunc,
			unsigned int uHwAddr,
			void *pData,
			unsigned int uLen,
			unsigned int bIncAddr,
			unsigned int bMore)
{
	printk(KERN_INFO "We don't use sdioDrv_WriteSync, please call sdioDrv_WriteAsync\n");
	return 0;
}

#ifdef CMD53_ALL_BLOCK_MODE
#define FN0_FBR2_REG_108                    0x210
#define FN0_FBR2_REG_108_BIT_MASK           0xFFF
int change_block_size(unsigned long size)
{
	unsigned long  uLong;
	unsigned long  uCount = 0;
	int iStatus;

	/* set block size for SDIO block mode */
	do {
	uLong = size;
	iStatus = sdioDrv_WriteSyncBytes(TXN_FUNC_ID_CTRL, FN0_FBR2_REG_108, (unsigned char *)&uLong, 2, 1);
	if (iStatus)
		return -1;

	iStatus = sdioDrv_ReadSyncBytes(TXN_FUNC_ID_CTRL, FN0_FBR2_REG_108, (unsigned char *)&uLong, 2, 1);
	if (iStatus)
		return -2;

	uCount++;

	} while (((uLong & FN0_FBR2_REG_108_BIT_MASK) != size) && (uCount < 10));

	if (uCount >= 10)
		return uCount;
	return 0;
}
#endif

int sdioDrv_ExecuteCmd(unsigned int uCmd,
		unsigned int uArg,
		unsigned int uRespType,
		void *pResponse,
		unsigned int uLen)
{
	int read, i = 0;
	int err;
	struct mmc_command cmd;
	unsigned char *ret = ((unsigned char *)pResponse);

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = uCmd;
	cmd.arg = uArg;
	cmd.flags = uRespType;

	PDEBUG("sdioDrv_ExecuteCmd() starting cmd %02x arg %08x\n", (int)uCmd, (int)uArg)
	printk(KERN_INFO "sdioDrv_ExecuteCmd() starting cmd %02x arg %08x\n", (int)uCmd, (int)uArg);

	err = mmc_wait_for_cmd(g_drv.mmc, &cmd, 3);
	if (err) {
		PERR("sdioDrv_ExecuteCmd fail: %d\n", err)
		return 1;
	}

	/* Copy the result back to the argument*/
	i = 0;
	while ((uLen > 0) && (ret)) {
		read = (uLen < 4) ? uLen : 4;
		memcpy(ret, &cmd.resp[i], uLen);
		ret += read;
		uLen -= read;
		i++;
	}

	return 0;
}

int sdioDrv_ReadAsync(unsigned int uFunc,
		unsigned int uHwAddr,
		void *pData,
		unsigned int uLen,
		unsigned int bBlkMode,
		unsigned int bIncAddr,
		unsigned int bMore)
{
	int iStatus;
	struct mmc_card scard;
	int num_blocks;
	int i, status = 0;

	if (uLen <= 512) {
		for (i = 0; i < uLen ; ++i) {
			status = sdioDrv_ReadSyncBytes(uFunc, uHwAddr, pData, 1, bMore);
			uHwAddr += 1;
			pData = (void *)((unsigned int)pData+1);
			if (status)
				PERR("%s FAILED(%d)!!\n", __func__, status)
		}
		return status;
	}

	i = 0;
	if (bBlkMode) {
		memset(&scard, 0, sizeof(struct mmc_card));
		scard.cccr.multi_block = 1;
		scard.type = MMC_TYPE_SDIO;
		scard.host = g_drv.mmc;

		num_blocks = uLen / g_drv.uBlkSize;
		iStatus = mmc_io_rw_extended(&scard, 0, uFunc, uHwAddr, bIncAddr, pData, num_blocks, g_drv.uBlkSize);

	} else {
		memset(&scard, 0, sizeof(struct mmc_card));
		scard.cccr.multi_block = 0; /*don't use block mode for now*/
		scard.type = MMC_TYPE_SDIO;
		scard.host = g_drv.mmc;

		iStatus = mmc_io_rw_extended(&scard, 0, uFunc, uHwAddr, bIncAddr, pData, 1, uLen);
	}

	if (iStatus != 0)
		PERR("%s FAILED(%d)!!\n", __func__, iStatus)

	return iStatus;


}

/*--------------------------------------------------------------------------------------*/

int sdioDrv_WriteAsync(unsigned int uFunc,
			unsigned int uHwAddr,
			void *pData,
			unsigned int uLen,
			unsigned int bBlkMode,
			unsigned int bIncAddr,
			unsigned int bMore)
{

	int iStatus;
	struct mmc_card scard;
	int num_blocks;
	int i = 0, status = 0;

	if (uLen <= 512) {
		for (i = 0; i < uLen ; ++i) {
			status = sdioDrv_WriteSyncBytes(uFunc, uHwAddr, pData, 1, bMore);
			uHwAddr += 1;
			pData = (void *)((unsigned int)pData+1);
			if (status)
				PERR("%s FAILED(%d)!!\n", __func__, status)
		}
		return status;
	}

	if (bBlkMode) {
		memset(&scard, 0, sizeof(struct mmc_card));
		scard.cccr.multi_block = 1;
		scard.type = MMC_TYPE_SDIO;
		scard.host = g_drv.mmc;

		num_blocks = uLen / g_drv.uBlkSize;
		iStatus = mmc_io_rw_extended(&scard, 1, uFunc, uHwAddr, bIncAddr, pData, num_blocks, g_drv.uBlkSize);

	} else {
		memset(&scard, 0, sizeof(struct mmc_card));
		scard.cccr.multi_block = 0;
		scard.type = MMC_TYPE_SDIO;
		scard.host = g_drv.mmc;

		iStatus = mmc_io_rw_extended(&scard, 1, uFunc, uHwAddr, bIncAddr, pData, 1, uLen);
	}

	if (iStatus != 0)
		PERR(KERN_INFO "%s FAILED(%d)!!\n", __func__, iStatus)

	return iStatus;


}

/*--------------------------------------------------------------------------------------*/

int sdioDrv_ReadSyncBytes(unsigned int  uFunc,
		unsigned int  uHwAddr,
		unsigned char *pData,
		unsigned int  uLen,
		unsigned int  bMore)
{
	unsigned int i;
	int iStatus;
	struct mmc_command cmd;

	for (i = 0; i < uLen; i++) {
		memset(&cmd, 0, sizeof(struct mmc_command));

		cmd.opcode = SD_IO_RW_DIRECT;
		cmd.arg = 0x00000000; /*read*/
		cmd.arg |= uFunc << 28;
		cmd.arg |= 0x00000000; /*not write*/
		cmd.arg |= uHwAddr << 9;
		cmd.arg |= 0; /*no in*/
		cmd.flags = MMC_RSP_PRESENT;

		iStatus = mmc_wait_for_cmd(g_drv.mmc, &cmd, 0);
		if (iStatus) {
			PERR("sdioDrv_WriteSyncBytes() SDIO Command error status = %d\n", iStatus)
			return -1;
		}

		*pData = cmd.resp[0] & 0xFF;

		uHwAddr++;
		pData++;
	}

	return 0;
}

/*--------------------------------------------------------------------------------------*/

int sdioDrv_WriteSyncBytes(unsigned int  uFunc,
		unsigned int  uHwAddr,
		unsigned char *pData,
		unsigned int  uLen,
		unsigned int  bMore)
{
	unsigned int i;
	int iStatus;
	struct mmc_command cmd;

	for (i = 0; i < uLen; i++) {
		memset(&cmd, 0, sizeof(struct mmc_command));

		cmd.opcode = SD_IO_RW_DIRECT;
		cmd.arg = 0x80000000; /*write*/
		cmd.arg |= uFunc << 28;
		cmd.arg |= 0x00000000; /*no out*/
		cmd.arg |= uHwAddr << 9;
		cmd.arg |= *pData; /*in*/
		cmd.flags = MMC_RSP_PRESENT;

		iStatus = mmc_wait_for_cmd(g_drv.mmc, &cmd, 3);
		if (iStatus) {
			PERR("sdioDrv_WriteSyncBytes() SDIO Command error status = %d\n", iStatus)
			return -1;
		}

		uHwAddr++;
		pData++;
	}

	return 0;
}

int emapi_sdio_read(char *out_data_buffer, unsigned long in_source_address, unsigned long data_length)
{
	int status;
	unsigned long bytes_count;
	unsigned long extra_size;
	unsigned int transferSize;
	unsigned int incr_fix = INCREMENT_REG_ADDR; /*address will be inc by HW*/

	if ((data_length%4) != 0) {
		for (bytes_count = 0; bytes_count < data_length; bytes_count++)
			sdioDrv_ReadSyncBytes(TXN_FUNC_ID_WLAN, in_source_address++, out_data_buffer++, 1, 1);
	} else {
		extra_size = data_length;

		while (extra_size > 0) {
			transferSize = (extra_size > SDIO_MAX_PACKET_LEN) ? SDIO_MAX_PACKET_LEN:extra_size;
			status = sdioDrv_ReadAsync(TXN_FUNC_ID_WLAN, in_source_address, 
					out_data_buffer, transferSize, 1, incr_fix, 0);
			if (status != 0) {
				printk(KERN_INFO "emapi_sdio_read fail (addr:0x%x, size:%d) \r\n",
						(unsigned int)in_source_address, transferSize);
				return status;
			}
			out_data_buffer += transferSize;
			in_source_address += ((incr_fix == INCREMENT_REG_ADDR) ? transferSize : 0);
			extra_size -= transferSize;
		}
	}

	return 0;
}
EXPORT_SYMBOL(emapi_sdio_read);

int emapi_sdio_write(char *in_data_buffer,  unsigned long out_target_address, unsigned long out_length)
{
	int status;
	unsigned long bytes_count;
	unsigned long extra_size;
	unsigned int transferSize;
	unsigned int incr_fix = INCREMENT_REG_ADDR; /*address will be inc by HW*/

	if ((out_length%4) != 0) {
		for (bytes_count = 0; bytes_count < out_length; bytes_count++)
			sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, out_target_address++, in_data_buffer++, 1, 1);
	} else {
		extra_size = out_length;

		while (extra_size > 0) {
			transferSize = (extra_size > SDIO_MAX_PACKET_LEN) ? SDIO_MAX_PACKET_LEN:extra_size;
			status = sdioDrv_WriteAsync(TXN_FUNC_ID_WLAN, out_target_address,
					in_data_buffer, transferSize, 1, incr_fix, 0);
			if (status != 0) {
				printk(KERN_INFO "emapi_sdio_write fail (addr:0x%x, size:%d) \r\n",
						(unsigned int)out_target_address, transferSize);
				return status;
			}
			in_data_buffer += transferSize;
			out_target_address += ((incr_fix == INCREMENT_REG_ADDR) ? transferSize : 0);
			extra_size -= transferSize;
		}
	}

	return 0;
}
EXPORT_SYMBOL(emapi_sdio_write);

unsigned int sdioDrv_status(struct device *dev)
{
	return 1;
}

int sdioDrv_ConnectBus(void *fCbFunc,
			void *hCbArg,
			unsigned int uBlkSizeShift,
			unsigned int  uSdioThreadPriority)
{
	g_drv.BusTxnCB      = fCbFunc;
	g_drv.BusTxnHandle  = hCbArg;
	g_drv.uBlkSizeShift = uBlkSizeShift;
	g_drv.uBlkSize      = 1 << uBlkSizeShift;

	return 0;
}

int sdioDrv_DisconnectBus(void)
{
	g_drv.BusTxnCB      = NULL;
	g_drv.BusTxnHandle  = 0;
	g_drv.uBlkSizeShift = 0;
	g_drv.uBlkSize      = 0;

	return 0;
}

/*HTC_CSP_START for emapiTest*/
typedef char	*PUCHAR;

unsigned long g_uMemAddr;
unsigned long g_uMemSize;
unsigned long g_uRegAddr;
unsigned long g_uRegSize;

typedef enum {
	SDIO_SUCCESS = 0,
	SDIO_FAILURE,
	SDIO_BUSY,
	SDIO_BAD_ALIGNMENT,
	SDIO_TIMEOUT,
	SDIO_BAD_INPUT_PARAMS,
	SDIO_BAD_PERIPHERAL_ADDRESS,
	SDIO_PACKET_SIZE_AND_MODE_INCONSISTENCY,
	SDIO_CONTEXT_AND_MODE_INCONSISTENCY,
	SDIO_NO_RESOURCES,
	SDIO_NON_AFFECTED,
} SDIO_Status;

int sdioDrv_SyncReadMethod(char *out_data_buffer,
			unsigned long in_source_address,
			unsigned long data_length)
{
/*use cmd 53*/
	unsigned long extra_size;
	unsigned int transferSize;
	int block_mode = 0;
	SDIO_Status status;
	unsigned int incr_fix = 1;/*INCREMENT_REG_ADDR; //address will be inc by HW automatically*/
	extra_size = data_length;
	if (data_length < g_drv.uBlkSize)
		block_mode = 0;
	else
		block_mode = 1;

	while (extra_size > 0) {
		transferSize = (extra_size > g_drv.uBlkSize)? g_drv.uBlkSize :extra_size;
		printk(KERN_INFO "Aus: readsync  transferSize:%d\n", transferSize);
		status = sdioDrv_ReadAsync(TXN_FUNC_ID_WLAN, in_source_address, out_data_buffer,
				transferSize, block_mode, incr_fix, 0);
		if (status != SDIO_SUCCESS) {
			printk(KERN_INFO "sdioDrv_SyncReadMethod fail (addr:0x%x, size:%d) \r\n",
					(unsigned int)in_source_address, transferSize);
			return status;
		}
		out_data_buffer += transferSize;
		in_source_address += ((incr_fix == 1/*INCREMENT_REG_ADDR*/) ? transferSize : 0);
		extra_size -= transferSize;
	}

	return 0; /*SDIO_SUCCESS;*/
}

int sdioDrv_SyncWriteMethod(char *in_data_buffer,
				unsigned long out_target_address,
				unsigned long out_length)
{
	unsigned int extra_size;
	unsigned int transferSize;
	int block_mode = 0;
	SDIO_Status status;
	unsigned int incr_fix = 1;/*INCREMENT_REG_ADDR;//address will be inc by HW automatically*/
	extra_size = out_length;

	if (out_length < g_drv.uBlkSize)
		block_mode = 0;
	else
		block_mode = 1;

	while (extra_size > 0) {
		int i = 0;
		transferSize = (extra_size > g_drv.uBlkSize)? g_drv.uBlkSize :extra_size;
		printk(KERN_INFO "i= %d -> Aus: writesync  transferSize:%d,  extra_size:%d  g_drv.uBlkSize:%d\n",
				i, transferSize, extra_size, g_drv.uBlkSize);
		status = sdioDrv_WriteAsync(TXN_FUNC_ID_WLAN, out_target_address, in_data_buffer,
				transferSize, block_mode, incr_fix, 0);
		if (status != SDIO_SUCCESS) {
			printk(KERN_INFO "sdioDrv_SyncWriteMethod fail (addr:0x%x, size:%d) \r\n",
					(unsigned int)out_target_address, transferSize);
			return status;
		}
		in_data_buffer += transferSize;
		out_target_address += ((incr_fix == 1/*INCREMENT_REG_ADDR*/) ? transferSize : 0);
		extra_size -= transferSize;
		i++;
	}

	return 0; /*SDIO_SUCCESS;*/
}

int g_start_tnetw_address;
int g_end_tnetw_address;
int g_start_reg_address;

void sdioDrv_Configure_Partition(unsigned long start_addr, unsigned long size)
{
	char data;

	data = (char)(size & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC0, &data, 1, 1);

	data = (char)((size>>8) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC1, &data, 1, 1);

	data = (char)((size>>16) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC2, &data, 1, 1);

	data = (char)((size>>24) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC3, &data, 1, 1);

	/* Set offset */
	data = (char)(start_addr & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC4, &data, 1, 1);

	data = (char)((start_addr>>8) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC5, &data, 1, 1);

	data = (char)((start_addr>>16) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC6, &data, 1, 1);

	data = (char)((start_addr>>24) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC7, &data, 1, 1);

	g_start_tnetw_address = start_addr;
	g_end_tnetw_address = start_addr + size;

}

void sdioDrv_Configure_Partition2(unsigned long start_addr, unsigned long size)
{
	char data;

	/* Configure 17-bits address range in TNETW: */
	/* Set size */
	data = (char)(size & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC8, &data, 1, 1);

	data = (char)((size>>8) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFC9, &data, 1, 1);

	data = (char)((size>>16) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFCA, &data, 1, 1);

	data = (char)((size>>24) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFCB, &data, 1, 1);

	/* Set offset */
	data = (char)(start_addr & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFCC, &data, 1, 1);

	data = (char)((start_addr>>8) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFCD, &data, 1, 1);

	data = (char)((start_addr>>16) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFCE, &data, 1, 1);

	data = (char)((start_addr>>24) & 0xFF);
	sdioDrv_WriteSyncBytes(TXN_FUNC_ID_WLAN, 0x1FFCF, &data, 1, 1);

	g_start_reg_address = start_addr;

}

void eMapiSetPartition(unsigned long uMemAddr, unsigned long uMemSize, unsigned long uRegAddr, unsigned long uRegSize)
{
	g_uRegAddr = uRegAddr;
	g_uRegSize = uRegSize;
	g_uMemAddr = uMemAddr;
	g_uMemSize = uMemSize;

	sdioDrv_Configure_Partition(uMemAddr, uMemSize);
	sdioDrv_Configure_Partition2(uRegAddr, uRegSize);

}
void eMapiConvertSDIOAddress(unsigned hostAddr, unsigned *SDIOAddr)
{
	unsigned uHwAddr = hostAddr;

	/*HTC_PrintMsg(ZONE_NKDBGMSG, "SDIO_ConvertAddress+\r\n");*/
	/* Translate HW address for registers region */
	if ((uHwAddr >= g_uRegAddr) && (uHwAddr <= g_uRegAddr + g_uRegSize))
		uHwAddr = uHwAddr - g_uRegAddr + g_uMemSize;

	/* Translate HW address for memory region */
	else
		uHwAddr = uHwAddr - g_uMemAddr;

	*SDIOAddr = uHwAddr;

}
unsigned eMapiWriteMemory(unsigned Addr, unsigned Length, PUCHAR pData)
{
	unsigned tnetw_addr;
	int istatus;

	eMapiConvertSDIOAddress(Addr, &tnetw_addr);

	istatus = sdioDrv_SyncWriteMethod((char *)pData, tnetw_addr, Length);

	return 0;
}
unsigned eMapiReadMemory(unsigned Addr, unsigned Length, PUCHAR pData)
{
	unsigned tnetw_addr;
	int istatus;

	eMapiConvertSDIOAddress(Addr, &tnetw_addr);

	istatus = sdioDrv_SyncReadMethod((char *)pData, tnetw_addr, Length);

	return 0;
}

void eMapiSdInterfaceTest2(unsigned testsize)
{
	const int size = testsize;
	int default_size = 4;
	unsigned char aTestPattern[size];
	unsigned char aTempBuf[size];
	unsigned int ii = 0;
	unsigned int kk = 0;
	const unsigned int NumberOfTest = 1;

	if (testsize > 2048)
		return ;

	/*init test block*/
	for (ii = 0; ii < testsize; ++ii) {
	if (1 == (ii%4))
		aTestPattern[ii] = 0xB1;
	else if (2 == (ii%4))
		aTestPattern[ii] = 0xC2;
	else if (3 == (ii%4))
		aTestPattern[ii] = 0xD3;
	else
		aTestPattern[ii] = 0xA0;
	}
	ii = 0;

	for (kk = 0; kk < testsize; ++kk)
		aTempBuf[kk] = 0x0;

	eMapiSetPartition(0x40000, 0x14FC0, 0x00300000, 0xB000);

	printk(KERN_INFO "Set Partition Over\n\n");

	for (default_size = 4; default_size <= testsize; default_size += 4) {
		do {
			int bMatch = 0;
			unsigned int jj;
			++ii;

			if (eMapiWriteMemory(0x40000, default_size, (PUCHAR) &aTestPattern) != 0) {
				printk(KERN_INFO "[eMapiSdInterfaceTest] Can't write data to memory!!!\r\n");
				break;
			}
			mdelay(20);

			if (eMapiReadMemory(0x40000, default_size, (PUCHAR) &aTempBuf) != 0) {
				printk(KERN_INFO "[eMapiSdInterfaceTest] Can't read data from memory!!!\r\n");
				break;
			}

			for (jj = 0; jj < default_size; ++jj) {
				if (aTempBuf[jj] != aTestPattern[jj]) {
					printk(KERN_INFO "No j=%d, aTempBuf=0x%x, aTestPattern=0x%x\r\n", jj, aTempBuf[jj], aTestPattern[jj]);
					break;
				} else{
					printk(KERN_INFO "Yes the same j=%d, aTempBuf=0x%x, aTestPattern=0x%x  the same!!\r\n", jj, aTempBuf[jj], aTestPattern[jj]);
				}
			}

			/*clear aTempBuf*/
			for (kk = 0; kk < default_size; ++kk)
				aTempBuf[kk] = 0x0;

			printk(KERN_INFO "clear done ii=%d\n", ii);

			if (jj == default_size) {
				bMatch = 1;
				printk(KERN_INFO "Match ii=%d\n", ii);
			}
			if (!bMatch) {
				printk(KERN_INFO "[eMapiSdInterfaceTest] mismatch at %d!!!\r\n", ii);
				/*break;*/
			}
		} while (ii < NumberOfTest);
	}

	if (ii < NumberOfTest) {
		printk(KERN_INFO "eMapiSdInterfaceTest NG!\r\n");
		return ;
	} else {
		printk(KERN_INFO "eMapiSdInterfaceTest OK!\r\n");
		return ;
	}
}

void eMapiSdInterfaceTest(unsigned testsize)
{
	const int size = testsize;
	unsigned char aTestPattern[size];
	unsigned char aTempBuf[size];
	unsigned int ii = 0;
	unsigned int kk = 0;
	const unsigned int NumberOfTest = 1;

	if (testsize > 2048)
		return ;

	/*init test block*/
	for (ii = 0; ii < testsize; ++ii) {
		if (1 == (ii%4))
			aTestPattern[ii] = 0xB1;
		else if (2 == (ii%4))
			aTestPattern[ii] = 0xC2;
		else if (3 == (ii%4))
			aTestPattern[ii] = 0xD3;
		else
			aTestPattern[ii] = 0xA0;
	}
	ii = 0;
	for (kk = 0; kk < testsize; ++kk)
		aTempBuf[kk] = 0x0;

	eMapiSetPartition(0x40000,
			0x14FC0,
			0x00300000,
			0xB000);

	printk(KERN_INFO "Set Partition Over\n\n");

	do {
		int bMatch = 0;
		unsigned int jj;
		++ii;

		if (eMapiWriteMemory(0x40000, testsize, (PUCHAR) &aTestPattern) != 0) {
			printk(KERN_INFO "[eMapiSdInterfaceTest] Can't write data to memory!!!\r\n");
			break;
		}
		mdelay(20);

		if (eMapiReadMemory(0x40000, testsize, (PUCHAR) &aTempBuf) != 0) {
			printk(KERN_INFO "[eMapiSdInterfaceTest] Can't read data from memory!!!\r\n");
			break;
		}

		for (jj = 0; jj < testsize; ++jj) {
			if (aTempBuf[jj] != aTestPattern[jj]) {
				printk(KERN_INFO "No j=%d, aTempBuf=0x%x, aTestPattern=0x%x\r\n", jj, aTempBuf[jj], aTestPattern[jj]);
				break;
			} else {
				printk(KERN_INFO "Yes the same j=%d, aTempBuf=0x%x, aTestPattern=0x%x  the same!!\r\n", jj, aTempBuf[jj], aTestPattern[jj]);
			}
		}

		/*clear aTempBuf*/
		for (kk = 0; kk < testsize; ++kk)
			aTempBuf[kk] = 0x0;

		printk(KERN_INFO "clear done ii=%d\n", ii);

		if (jj == testsize) {
			bMatch = 1;
			printk(KERN_INFO "Match ii=%d\n", ii);
		}
		if (!bMatch) {
			printk(KERN_INFO "[eMapiSdInterfaceTest] mismatch at %d!!!\r\n", ii);
			/*break;*/
		}
	} while (ii < NumberOfTest);

	if (ii < NumberOfTest) {
		printk(KERN_INFO "eMapiSdInterfaceTest NG!\r\n");
		return ;
	} else {
		printk(KERN_INFO "eMapiSdInterfaceTest OK!\r\n");
		return ;
	}
}

int hPlatform_DevicePowerOff(void)
{
	printk(KERN_INFO "[%s]\n", __func__);
	msleep(10);
	return 0;
}

int hPlatform_DevicePowerOn(void)
{
	printk(KERN_INFO "[%s]\n", __func__);
	return 0;
}

int sdio_command_init(void)
{
	unsigned char  uByte;
	unsigned long  uLong;
	unsigned long  uCount = 0;
	unsigned int   uBlkSize = 1 << 9;
	int iStatus;
	g_drv.uBlkSize = uBlkSize;
	g_drv.uBlkSizeShift = 9;

	/* Init SDIO driver and HW */

	/* Send commands sequence: 0, 5, 3, 7 */
	printk(KERN_INFO "sdioAdapt_ConnectBus  CMD 0 5 3 7\n");
	iStatus = sdioDrv_ExecuteCmd(SD_IO_GO_IDLE_STATE, 0, MMC_RSP_NONE, &uByte, sizeof(uByte));
	if (iStatus) {
	printk(KERN_INFO "%s %d command number: %d failed\n", __func__, __LINE__, SD_IO_GO_IDLE_STATE);
	return iStatus;
	}

	iStatus = sdioDrv_ExecuteCmd(SDIO_CMD5, VDD_VOLTAGE_WINDOW, MMC_RSP_R4/*MMC_RSP_R4 austin change to MMC_RSP_PRESENT*/, &uByte, sizeof(uByte));
	if (iStatus) {
	printk(KERN_INFO "%s %d command number: %d failed\n", __func__, __LINE__, SDIO_CMD5);
	return iStatus;
	}

	iStatus = sdioDrv_ExecuteCmd(SD_IO_SEND_RELATIVE_ADDR, 0, MMC_RSP_R6, &uLong, sizeof(uLong));
	if (iStatus) {
	printk(KERN_INFO "%s %d command number: %d failed\n", __func__, __LINE__, SD_IO_SEND_RELATIVE_ADDR);
	return iStatus;
	}

	iStatus = sdioDrv_ExecuteCmd(SD_IO_SELECT_CARD, uLong, MMC_RSP_R6, &uByte, sizeof(uByte));
	if (iStatus) {
	printk(KERN_INFO "%s %d command number: %d failed\n", __func__, __LINE__, SD_IO_SELECT_CARD);
	return iStatus;
	}

	printk(KERN_INFO "sdioAdapt_ConnectBus  CMD 0 5 3 7 over\n");

	/* NOTE:
	* =====
	* Each of the following loops is a workaround for a HW bug that will be solved in PG1.1 !!
	* Each write of CMD-52 to function-0 should use it as follows:
	* 1) Write the desired byte using CMD-52
	* 2) Read back the byte using CMD-52
	* 3) Write two dummy bytes to address 0xC8 using CMD-53
	* 4) If the byte read in step 2 is different than the written byte repeat the sequence
	*/

#ifdef SDIO_HIGH_SPEED
	PERR("sdioAdapt_ConnectBus: Set HIGH_SPEED bit on register 0x13\n")
	/* CCCR 13 bit EHS(1) */
	iStatus = sdioDrv_ReadSyncBytes(TXN_FUNC_ID_CTRL, 0x13, &uByte, 1, 1)
	PERR2("After r 0x%x, iStatus=%d \n", uByte, iStatus);
	if (iStatus)
		return iStatus;

	uByte |= 0x2; /* set bit #1 EHS */
	iStatus = sdioDrv_WriteSyncBytes(TXN_FUNC_ID_CTRL, 0x13, &uByte, 1, 1)
	PERR2("After w 0x%x, iStatus=%d \n", uByte, iStatus);
	if (iStatus)
		return iStatus;

	iStatus = sdioDrv_ReadSyncBytes(TXN_FUNC_ID_CTRL, 0x13, &uByte, 1, 1)
	PERR2("After r 0x%x, iStatus=%d \n", uByte, iStatus)
	if (iStatus)
		return iStatus;

	PERR1("After CCCR 0x13, uByte=%d \n", (int)uByte)
#endif

	/* set device side bus width to 4 bit (for 1 bit write 0x80 instead of 0x82) */
	do {
	uByte = SDIO_BITS_CODE;
	iStatus = sdioDrv_WriteSyncBytes(TXN_FUNC_ID_CTRL, CCCR_BUS_INTERFACE_CONTOROL, &uByte, 1, 1);
	if (iStatus)
		return iStatus;

	iStatus = sdioDrv_ReadSyncBytes(TXN_FUNC_ID_CTRL, CCCR_BUS_INTERFACE_CONTOROL, &uByte, 1, 1);
	if (iStatus)
		return iStatus;

	printk(KERN_INFO "Skip  w 0xC8, iStatus=%d \n", iStatus);
	uCount++;

	} while ((uByte != SDIO_BITS_CODE) && (uCount < MAX_RETRIES));

	uCount = 0;

	/* allow function 2 */
	do {
	uByte = 4;
	iStatus = sdioDrv_WriteSyncBytes(TXN_FUNC_ID_CTRL, CCCR_IO_ENABLE, &uByte, 1, 1);
	if (iStatus)
		return iStatus;

	iStatus = sdioDrv_ReadSyncBytes(TXN_FUNC_ID_CTRL, CCCR_IO_ENABLE, &uByte, 1, 1);
	if (iStatus)
		return iStatus;

	printk(KERN_INFO "Skip  w 0xC8, after CCCR_IO_ENABLE iStatus=%d \n", iStatus);
	uCount++;

	} while ((uByte != 4) && (uCount < MAX_RETRIES));



	uCount = 0;

	/* set block size for SDIO block mode */
	do {
		uLong = uBlkSize;/*SDIO_BLOCK_SIZE;*/

		iStatus = sdioDrv_WriteSyncBytes(TXN_FUNC_ID_CTRL, FN0_FBR2_REG_108, (unsigned char *)&uLong, 2, 1);
		if (iStatus)
			return iStatus;

		iStatus = sdioDrv_ReadSyncBytes(TXN_FUNC_ID_CTRL, FN0_FBR2_REG_108, (unsigned char *)&uLong, 2, 1);
		if (iStatus)
			return iStatus;

	uCount++;

	} while (((uLong & FN0_FBR2_REG_108_BIT_MASK) != uBlkSize /*SDIO_BLOCK_SIZE*/) && (uCount < MAX_RETRIES));

	if (uCount >= MAX_RETRIES) {
		/* Failed to write CMD52_WRITE to function 0 */
		return (int)uCount;
	}

	return iStatus;
}

static int sdioDrv_probe(void)
{
	int rc = 0;
	struct mmc_ios *ios;
	struct mmc_host *mmc = 0;

	printk(KERN_INFO "[ %s ] BEGIN\n", __func__);
	printk(KERN_INFO "[ %s ] Probing the controller\n", __func__);
	printk(KERN_INFO "[ %s ] Calling pincfg_sdio_power_on_wifi\n", __func__);

	g_drv.pdev = mmci_get_platform_device();
	g_drv.mmc = mmci_get_mmc();

	mmc_claim_host(g_drv.mmc);

	printk(KERN_INFO "SdioDrv_probe  g_drv.pdev:0x%x  g_drv.mmc:0x%x  mmc->index=%d\n",
		   (int)g_drv.pdev, (int)g_drv.mmc, g_drv.mmc->index);

	mmc = g_drv.mmc;

	printk(KERN_INFO "[ %s ] Setting bus width to %d bits, power on, clock %dHz\n", __func__,
			(TIWLAN_SDIO_BUSWIDE == MMC_BUS_WIDTH_4 ? 4 : 1), TIWLAN_SDIO_CLOCK);

	mmc->ios.bus_width = MMC_BUS_WIDTH_1;
	mmc->ios.power_mode = MMC_POWER_ON;
	mmc->ios.vdd = 16;
	mmc->ios.bus_mode = MMC_BUSMODE_OPENDRAIN;
	mmc->ios.chip_select = MMC_CS_DONTCARE;
	mmc->ios.timing = MMC_TIMING_LEGACY;
	mmc->ios.clock= 144000;

	ios = &mmc->ios;

	pr_info("[WLAN] %s: clock %uHz busmode %u powermode %u cs %u Vdd %u "
		"width %u timing %u\n",
		 mmc_hostname(mmc), ios->clock, ios->bus_mode,
		 ios->power_mode, ios->chip_select, ios->vdd,
		 ios->bus_width, ios->timing);

	mmc->ops->set_ios(mmc, ios);

	msleep(100);

	hPlatform_DevicePowerOn();
	rc = sdio_command_init();

	msleep(100);

	mmc->ios.bus_width = MMC_BUS_WIDTH_4;
	mmc->ios.power_mode = MMC_POWER_ON;
	mmc->ios.vdd = 7;
	mmc->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
	mmc->ios.chip_select = MMC_CS_DONTCARE;
	mmc->ios.timing = MMC_TIMING_LEGACY;
	mmc->ios.clock= 25000000;

	ios = &mmc->ios;

	pr_info("[WLAN] %s: clock %uHz busmode %u powermode %u cs %u Vdd %u "
		"width %u timing %u\n",
		 mmc_hostname(mmc), ios->clock, ios->bus_mode,
		 ios->power_mode, ios->chip_select, ios->vdd,
		 ios->bus_width, ios->timing);

	mmc->ops->set_ios(mmc, ios);
	msleep(100);

	return rc;
}

static int sdioDrv_remove(void)
{
	hPlatform_DevicePowerOff();
	mmc_release_host(g_drv.mmc);

	return 0;
}

int sdiodrv_open(struct inode *node_ptr, struct file *file_ptr)
{
	int rc;
	printk(KERN_INFO "sdiodrv_open+\n");
	rc = sdioDrv_probe();

	return rc;
}

int sdiodrv_close(struct inode *node_ptr, struct file *file_ptr)
{
	int rc;
	printk(KERN_INFO "sdiodrv close+\n");
	rc = sdioDrv_remove();

	return rc;
}

static struct file_operations sdiodrv_fops = {
	.owner		= THIS_MODULE,
	.open		= sdiodrv_open,
	.release	= sdiodrv_close,
};

static struct miscdevice sdiodrv_device = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "sdiodrv",
	.fops	= &sdiodrv_fops,
};

static int sdiodrv_init(void)
{
	int rc;

	rc = misc_register(&sdiodrv_device);
	if (rc)
		printk(KERN_ERR "sdiodrv: register fail\n");

	return rc;
}

static void sdiodrv_exit(void)
{
	if (misc_deregister(&sdiodrv_device))
		printk(KERN_ERR "sdiodrv: deregister fail\n");
}

module_init(sdiodrv_init);
module_exit(sdiodrv_exit);

MODULE_DESCRIPTION("TI WLAN SDIO driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS(SDIO_DRIVER_NAME);
MODULE_AUTHOR("Texas Instruments Inc");
