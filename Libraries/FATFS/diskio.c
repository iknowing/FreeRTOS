/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "mmc_sd.h"
#include "ds1302.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_status();
		// translate the reslut code here

		return stat;

	case DEV_MMC :
		result = 0;
        if(result == 0)
        {
            stat = RES_OK;
        }
		return stat;

	case DEV_USB :
		//result = USB_disk_status();
		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();
		// translate the reslut code here

		return stat;

	case DEV_MMC :
		//result = MMC_disk_initialize();
		// translate the reslut code here
        
        result = SD_Init();
        if(result == 0)
        {
           stat = RES_OK;
        }
		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();
		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_ERROR;
	int result;

    if(!count)
    {
        return RES_PARERR;
    }
	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here
		//result = RAM_disk_read(buff, sector, count);
		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here
		//result = MMC_disk_read(buff, sector, count);
		// translate the reslut code here

        if(count == 1)
        {
            result = SD_ReadSingleBlock(sector,buff);
        }
        else
        {
            result = SD_ReadMultiBlock(sector,buff,count);
        }
        if(result == 0)
        {
            res = RES_OK;
        }

		return res;

	case DEV_USB :
		// translate the arguments here
		//result = USB_disk_read(buff, sector, count);
		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_ERROR;
	int result;
    
    if(!count)
    {
        return RES_PARERR;
    }

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here
		//result = RAM_disk_write(buff, sector, count);
		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here
		//result = MMC_disk_write(buff, sector, count);
		// translate the reslut code here
        if(count == 1)
        {
            result = SD_WriteSingleBlock(sector,buff);
        }
        else
        {
            result = SD_WriteMultiBlock(sector,buff,count);
        }
        if(result == 0)
        {
            res = RES_OK;
        }

		return res;

	case DEV_USB :
		// translate the arguments here
		//result = USB_disk_write(buff, sector, count);
		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_ERROR;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// Process of the command for the RAM drive
		return res;

	case DEV_MMC :
		// Process of the command for the MMC/SD card
        switch (cmd)
        {
            case CTRL_SYNC :
                    return RES_OK;
            case GET_SECTOR_COUNT :
                   // *(DWORD*)buff = SD0_CardInfo.Capacity/SD0_CardInfo.BlockSize;
            return RES_OK;
            case GET_BLOCK_SIZE :
                    //*(WORD*)buff = SD0_CardInfo.BlockSize;
            return RES_OK;        
            case CTRL_POWER :
                    break;
            case CTRL_LOCK :
                    break;
            case CTRL_EJECT :
                    break;
            /* MMC/SDC command */
            case MMC_GET_TYPE :
                    break;
            case MMC_GET_CSD :
                result = SD_GetCSD(buff);
                if(result == 0)
                {
                    res = RES_OK;
                }
                    return res;
            case MMC_GET_CID :
                result = SD_GetCID(buff);
                if(result == 0)
                {
                    res = RES_OK;
                }
                return res;
            case MMC_GET_OCR :
                    break;
            case MMC_GET_SDSTAT :
                    break;        
        }
		return res;

	case DEV_USB :
		// Process of the command the USB drive
		return res;
	}

	return RES_PARERR;
}

DWORD get_fattime (void)
{
    uint8_t date[7];
    
    DS1302_GetTime(date);
    date[6] = 20;
    
    return   ((DWORD)((date[6]*100+date[5]) - 1980) << 25) 
             | ((DWORD)date[4] << 21)
             | ((DWORD)date[3] << 16)
             | ((DWORD)date[2] << 11)
             | ((DWORD)date[1] << 5)
             | ((DWORD)date[0] >> 1);
}





