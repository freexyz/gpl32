/*
* Purpose: USB Host interface forFile System
*
* Author: Dominant Yang
*
* Date: 2008/05/09
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01
* History :
          1 2008/05/09 dominant initial  
          2.2008/08/12 bug fix for multi read and write about buffer counting by wschung
*/
//Include files
#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined USB_NUM) && (USB_NUM >= 1)                           //
//================================================================//

// Constant definitions used in this file only go here

// Type definitions used in this file only go here
#define CREAT_DRIVERLAYER_STRUCT
#define		CREAT_DRIVERLAYER_STRUCT
//#define		CREATUSBHOSTVAR
#define C_DISK1_FLAG	0x0001
#define C_DISK2_FLAG	0x0002
#define C_DISK3_FLAG	0x0004
#define C_DISK4_FLAG	0x0008
#define USBHOST_SUPPROT_2K_BLOCK
#define USBHOST_BUFFER_SIZE_4K 		//netac need this

//=========================================================================
#ifdef	USBHOST_SUPPROT_2K_BLOCK

#ifdef USBHOST_BUFFER_SIZE_8K
	#define C_BUFFERSIZE				0x800
	#define C_BUF2SECMASK				0x0f		//one buffer can load 16 sectors
#elif defined USBHOST_BUFFER_SIZE_4K
	#define C_BUFFERSIZE				0x400
	#define C_BUF2SECMASK				0x07		//one buffer can load 8 sectors
#elif defined USBHOST_BUFFER_SIZE_2K
	#define C_BUFFERSIZE				0x200		//2k
	#define C_BUF2SECMASK				0x03		//one buffer can load 4 sectors
#endif

#define C_EMPTY						0
#define C_VALID						1
#define C_MODIFIED					2

typedef struct 
{
	unsigned int Flag;	
	unsigned int Disk;				//usb host disk num, is 0, 1, 2...
	unsigned long BlkNo;			//the block number
	unsigned int Buffer[C_BUFFERSIZE];
}USBHOSTBUFFER;

INT8U fs_usbh_plug_out_flag_get(void);
void fs_usbh_plug_out_flag_reset(void);

// Variables defined in this file
USBHOSTBUFFER gUSBHostBuffer;
INT32U gBlk2BufMask;
INT32U gSec2BlkSft;
INT32U gBlk2BufCnt;
INT16S bUSB_Init;   /* Dominant modified, dont set initial value as -1, after reset, this value will disappear*/
INT16S bUSB_LUN_Init;

//extern void MemPackByteCopyFarEE( ULONG  s ,  ULONG  d ,  INT16U n);

INT32S USB_host_buffer_init(INT16U SectorSize)
{
    int ret = 0;
    
    gUSBHostBuffer.Flag = C_EMPTY;
    gUSBHostBuffer.Disk = 0;
    gUSBHostBuffer.BlkNo = 0;
    
    switch(SectorSize)
    {
    	case 1:				//sector size is 512 byte
    	#ifdef USBHOST_BUFFER_SIZE_8K
			gBlk2BufMask = 0xfffffff0;
			gSec2BlkSft = 0;
			gBlk2BufCnt = 16;
		#elif defined USBHOST_BUFFER_SIZE_4K
			gBlk2BufMask = 0xfffffff8;
			gSec2BlkSft = 0;
			gBlk2BufCnt = 8;
		#elif defined USBHOST_BUFFER_SIZE_2K
			gBlk2BufMask = 0xfffffffc;
			gSec2BlkSft = 0;
			gBlk2BufCnt = 4;
		#endif
    		break;
    		
    	case 2:				//sector size is 1024 byte
    	#ifdef USBHOST_BUFFER_SIZE_8K
			gBlk2BufMask = 0xfffffff8;
			gSec2BlkSft = 1;
			gBlk2BufCnt = 8;
		#elif defined USBHOST_BUFFER_SIZE_4K
			gBlk2BufMask = 0xfffffffc;
			gSec2BlkSft = 1;
			gBlk2BufCnt = 4;
		#elif defined USBHOST_BUFFER_SIZE_2K
			gBlk2BufMask = 0xfffffffe;
			gSec2BlkSft = 1;
			gBlk2BufCnt = 2;
		#endif		
    		break;
    		
    	case 4:				//sector size is 2048 byte
    	#ifdef USBHOST_BUFFER_SIZE_8K
			gBlk2BufMask = 0xfffffffc;
			gSec2BlkSft = 2;
			gBlk2BufCnt = 4;
		#elif defined USBHOST_BUFFER_SIZE_4K
			gBlk2BufMask = 0xfffffffe;
			gSec2BlkSft = 2;
			gBlk2BufCnt = 2;
		#elif defined USBHOST_BUFFER_SIZE_2K
			gBlk2BufMask = 0xffffffff;
			gSec2BlkSft = 2;
			gBlk2BufCnt = 1;
		#endif		
    		break;
    		
    	case 8:				//sector size is 4096 byte
    	#ifdef USBHOST_BUFFER_SIZE_8K
			gBlk2BufMask = 0xfffffffe;
			gSec2BlkSft = 3;
			gBlk2BufCnt = 2;
		#elif defined USBHOST_BUFFER_SIZE_4K
			gBlk2BufMask = 0xffffffff;
			gSec2BlkSft = 3;
			gBlk2BufCnt = 1;
		#elif defined USBHOST_BUFFER_SIZE_2K
			ret = -1;
		#endif		
    		break;
    	
    	default:
    		ret = -1;
    		break;
    }
    return ret;
}

INT32S USB_host_bufferflush(void)
{
    INT16S ret;
    
    if(gUSBHostBuffer.Flag == (C_VALID + C_MODIFIED))
    {
    	ret = MSDC_Write2A(gUSBHostBuffer.BlkNo, gUSBHostBuffer.Buffer, 1, gUSBHostBuffer.Disk);
    	if(ret != 0)
    		return ret;
    }
    gUSBHostBuffer.Flag = C_EMPTY;
    return 0;
} 

INT32S USB_host_initial(void)
{
	INT16S ret,i;
	INT16U	sectorsize;
    bUSB_Init=-1;
	if(bUSB_Init == -1)
	{
		ResetUSBHost = 0;
		bUSB_LUN_Init = 0;
		//USBHostCSWBuffer=arUSBHostCSWBuffer;
		//USBHostBuffer=arUSBHostBuffer;		
		//ptr_USBHostBuffer=USBHostBuffer;
	    //system_usbh_uncheat();
		ret = DrvUSBHMSDCInitial();
		if(ret != 0) 		 // Initial Fail
		{
			//USBHostBuffer = 0;
			//USBHostCSWBuffer = 0;
		    // system_usbh_cheat();
			bUSB_Init = -1;
			return -1;
		}
		
		bUSB_Init = 0;
	}
	ret = DrvUSBHLUNInit(0);
	if(ret != 0)
	{
	  for (i =0 ;i <10; i++)
		   			USBH_Delay(0xFFFF);		
        ret = DrvUSBHMSDCInitial();
    	if(ret != 0)
   		{
   			bUSB_Init = -1;
    		return -1;
	    }
   	    ret = DrvUSBHLUNInit(0);
	    //system_usbh_cheat();
    	if(ret != 0)
		return ret;
	}
	bUSB_LUN_Init |= C_DISK1_FLAG;
	sectorsize = DrvUSBHGetSectorSize();
	ret = USB_host_buffer_init(sectorsize);
    //system_usbh_cheat();
	return ret;
}


INT32S USB_host_uninitial(void)
{
	if(bUSB_Init == 0)
	{
		bUSB_LUN_Init &= (~C_DISK1_FLAG);
		if(bUSB_LUN_Init == 0)
		{
			DrvUSBHMSDCUninitial();
	
			//USBHostBuffer = 0;
			//USBHostCSWBuffer = 0;
			bUSB_Init = -1;
		}
	}
	
	return 0;
}

void USB_host_get_drv_info(struct DrvInfo* info)
{
	info->nSectors = 0xffffffff;		//unknow size
	info->nBytesPerSector = DrvUSBHGetSectorSize() << 9;
}

INT32S USBHost_ReadSector(INT32U blkno , INT32U blkcnt ,  INT32U buf, INT16S dsk)
{
//以下code为支持连续sector地址的读或者写
//支持的最大sector size为512 byte
	INT16U	i, j;
	INT16S	ret;
	INT32U	blk;
	INT16U	cnt;
    //i = DrvUSBHGetSectorSize();
    ret =0;
    for (i =0 ;i<3; i++)
      ret = ret + (INT16S) USB_Host_Signal();    
    if (!ret) return -1;
	if(1 == DrvUSBHGetSectorSize())		//512 byte的sector size
	{		
		i = blkcnt >> 6;
		j = blkcnt & 0x003f;
		
		while(i)
		{
			ret = MSDC_Read28(blkno,(INT32U*) buf, 64, dsk);
			if(ret != 0)
				return ret;
				
			i--;
			buf += 64*512; //bug fix by wschung
			//buf += 64>>9;
			blkno += 64;
		};
		
		if(j)
		{
			ret = MSDC_Read28(blkno,(INT32U*) buf, j, dsk);
		}
		return ret;
	}
	else if(4 == DrvUSBHGetSectorSize())	//2k的sector size，通过buffer进行缓冲
	{
		while(blkcnt)
		{
			blk = blkno >> 2;
			if(blkno & 3)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}	    	
		    	
		    	if(4 - (blkno & 3) > blkcnt)
		    		cnt = blkcnt;
		    	else
		    		cnt = 4 - (blkno & 3);
		    		
		    	//MemPackByteCopyFarEE((INT32U)gUSBHostBuffer.Buffer + ((blkno & 3) << 8), (INT32U)buf, cnt << 8);
				gp_memcpy((INT8S *) buf,(INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 3) << 7)), cnt << 9);
		    			    	
		    	blkcnt -= cnt;
		    	blkno += cnt;
		    	buf += (cnt << 9);
			}
			else if(blkcnt < 4)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}
						    	
		    	//MemPackByteCopyFarEE((INT32U)gUSBHostBuffer.Buffer + ((blkno & 3) << 8), (INT32U)buf, blkcnt << 8);
				gp_memcpy((INT8S *) buf,(INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 3) << 7)), blkcnt << 9);
		    	blkcnt = 0;
			}
			else
			{
				cnt = blkcnt >> 2;
				i = cnt >> 4;
				j = cnt & 0x000f;
				while(i)
				{
					ret = MSDC_Read28(blk,(unsigned int*) buf, 16, dsk);
					if(ret != 0)
						return ret;
					
					i--;
					blk += 16;
					//buf += 64*256; //bug fix by wschung
					buf += 64*512;
					blkno += 64;
					blkcnt -= 64;
				}
				
				if(j)
				{
					ret = MSDC_Read28(blk,(unsigned int*)buf, j, dsk);
					cnt = j << 2;
					buf += (cnt << 9);
					blkno += cnt;
					blkcnt -= cnt;
				}
			}
		}
	}
	else if(8 == DrvUSBHGetSectorSize())	//4k的sector size，通过buffer进行缓冲
	{
		while(blkcnt)
		{
			blk = blkno >> 3;
			if(blkno & 7)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}	    	
		    	
		    	if(8 - (blkno & 7) > blkcnt)
		    		cnt = blkcnt;
		    	else
		    		cnt = 8 - (blkno & 7);
		    		
		    	//MemPackByteCopyFarEE((INT32U)gUSBHostBuffer.Buffer + ((blkno & 3) << 8), (INT32U)buf, cnt << 8);
				gp_memcpy((INT8S *) buf,(INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 7) << 7)), cnt << 9);
		    			    	
		    	blkcnt -= cnt;
		    	blkno += cnt;
		    	buf += (cnt << 9);
			}
			else if(blkcnt < 8)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}
						    	
		    	//MemPackByteCopyFarEE((INT32U)gUSBHostBuffer.Buffer + ((blkno & 3) << 8), (INT32U)buf, blkcnt << 8);
				gp_memcpy((INT8S *) buf,(INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 7) << 7)), blkcnt << 9);
		    	blkcnt = 0;
			}
			else
			{
				cnt = blkcnt >> 3;
				i = cnt >> 3;
				j = cnt & 0x000f;
				while(i)
				{
					ret = MSDC_Read28(blk,(unsigned int*) buf, 8, dsk);
					if(ret != 0)
						return ret;
					
					i--;
					blk += 8;
					buf += 64*512;
					blkno += 64;
					blkcnt -= 64;
				}
				
				if(j)
				{
					ret = MSDC_Read28(blk,(unsigned int*)buf, j, dsk);
					cnt = j << 3;
					buf += (cnt << 9);
					blkno += cnt;
					blkcnt -= cnt;
				}
			}
		}
	}
	else				//other sector size is not support
		return -1;
		
	return 0;
}

INT32S USBHost_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf, INT16S dsk)
{
//以下code为支持连续sector地址的读或者写
//支持的最大sector size为512 byte
	INT16U	i, j;
	INT16S	ret;
	INT32U	blk;
	INT16U	cnt;
    ret =0;
    for (i =0 ;i<3; i++)
      ret = ret + (INT16S) USB_Host_Signal();    
    if (!ret) return -1;
	if(1 == DrvUSBHGetSectorSize())		//512 byte的sector size
	{		
		i = blkcnt >> 6;
		j = blkcnt & 0x003f;
		
		while(i)
		{
			ret = MSDC_Write2A(blkno,(INT32U*)buf, 64, dsk);
			if(ret != 0)
				return ret;
				
			i--;
			buf += 64*512; //bug fix by wschung
			//buf += 64>>9;
			blkno += 64;
		};
		
		if(j)
			ret = MSDC_Write2A(blkno,(INT32U*)buf, j, dsk);
		
		return ret;
	}
	else if(4 == DrvUSBHGetSectorSize())	//2k的sector size，通过buffer进行缓冲
	{
		while(blkcnt)
		{
			blk = blkno >> 2;
			if(blkno & 3)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}	    	
		    	
		    	if(4 - (blkno & 3) > blkcnt)
		    		cnt = blkcnt;
		    	else
		    		cnt = 4 - (blkno & 3);
		    		
		    	//MemPackByteCopyFarEE((INT32U)buf, (INT32U)gUSBHostBuffer.Buffer + ((blkno & 3) << 8), cnt << 8);
 		    	gp_memcpy((INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 3) << 7)),(INT8S *) buf, cnt << 9);
		    	gUSBHostBuffer.Flag |= C_MODIFIED;
		    	
		    	blkcnt -= cnt;
		    	blkno += cnt;
		    	buf += (cnt << 8); 
			}
			else if(blkcnt < 4)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}
						    	
		    	//MemPackByteCopyFarEE((INT32U)buf, (INT32U)gUSBHostBuffer.Buffer, blkcnt << 8);
	 		   	gp_memcpy((INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 3) << 7)),(INT8S *) buf, cnt << 9);
		    	gUSBHostBuffer.Flag |= C_MODIFIED;
		    	
		    	blkcnt = 0;
			}
			else
			{
				cnt = blkcnt >> 2;				
				i = cnt >> 4;
				j = cnt & 0x000f;
				while(i)
				{
					ret = MSDC_Write2A(blk, (unsigned int*)buf, 16, dsk);
					if(ret != 0)
						return ret;
					
					i--;
					blk += 16;
					//buf += 64*256; //bug fix by wschung
					buf += 64>>9;
					blkno += 64;
					blkcnt -= 64;
				}
				
				if(j)
				{
					ret = MSDC_Write2A(blk,(unsigned int*) buf, j, dsk);
					cnt = j << 2;
					buf += (cnt << 9);
					blkno += cnt;
					blkcnt -= cnt;
				}
			}
		}
	}
	else if(8 == DrvUSBHGetSectorSize())	//4k的sector size，通过buffer进行缓冲
	{
		while(blkcnt)
		{
			blk = blkno >> 3;
			if(blkno & 7)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}	    	
		    	
		    	if(8 - (blkno & 7) > blkcnt)
		    		cnt = blkcnt;
		    	else
		    		cnt = 8 - (blkno & 7);
		    		
		    	//MemPackByteCopyFarEE((INT32U)buf, (INT32U)gUSBHostBuffer.Buffer + ((blkno & 3) << 8), cnt << 8);
 		    	gp_memcpy((INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 7) << 7)),(INT8S *) buf, cnt << 9);
		    	gUSBHostBuffer.Flag |= C_MODIFIED;
		    	
		    	blkcnt -= cnt;
		    	blkno += cnt;
		    	buf += (cnt << 9); 
			}
			else if(blkcnt < 8)
			{
				if(!((gUSBHostBuffer.Flag & C_VALID) && (gUSBHostBuffer.BlkNo == blk) && (gUSBHostBuffer.Disk == dsk)))
				{
					ret = USB_host_bufferflush();
				    if(ret != 0)
				    	return ret;
				    
				    ret = MSDC_Read28(blk, gUSBHostBuffer.Buffer, 1, dsk);
				    if(ret != 0)
		    			return ret;
				    
				    gUSBHostBuffer.Flag = C_VALID;
			    	gUSBHostBuffer.BlkNo = blk;
			    	gUSBHostBuffer.Disk = dsk;
				}
						    	
		    	//MemPackByteCopyFarEE((INT32U)buf, (INT32U)gUSBHostBuffer.Buffer, blkcnt << 8);
	 		   	gp_memcpy((INT8S *)(gUSBHostBuffer.Buffer + ((blkno & 7) << 7)),(INT8S *) buf, cnt << 9);
		    	gUSBHostBuffer.Flag |= C_MODIFIED;
		    	
		    	blkcnt = 0;
			}
			else
			{
				cnt = blkcnt >> 3;				
				i = cnt >> 3;
				j = cnt & 0x000f;
				while(i)
				{
					ret = MSDC_Write2A(blk, (unsigned int*)buf, 8, dsk);
					if(ret != 0)
						return ret;
					
					i--;
					blk += 8;
					buf += 64>>9;
					blkno += 64;
					blkcnt -= 64;
				}
				
				if(j)
				{
					ret = MSDC_Write2A(blk,(unsigned int*) buf, j, dsk);
					cnt = j << 3;
					buf += (cnt << 9);
					blkno += cnt;
					blkcnt -= cnt;
				}
			}
		}
	}
	else				//other sector size is not support
		return -1;
		
	return 0;
}

INT32S USB_host_read_sector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
    INT32S ret ;
    INT16S currentlun;
    currentlun =(INT16S)DrvUSBHGetCurrent();
    if(fs_usbh_plug_out_flag_get()==1) {return -1;}
    ret = USBHost_ReadSector(blkno, blkcnt, buf, currentlun);
    if (ret == 0) { 
        return 0;
    } if (ret==USBH_Err_NoMedia) {  /* storage loss, no need to retry */
        fs_usbh_plug_out_flag_en();
    } else {  /*if (ret !=0)*/
          ret = USBHost_ReadSector(blkno, blkcnt, buf, currentlun);
    }
    //system_usbh_cheat();
    return ret;
}

INT32S USB_host_write_sector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
    INT32S ret ;
    INT16S currentlun;
    currentlun =(INT16S)DrvUSBHGetCurrent();
    //system_usbh_uncheat();
    if(fs_usbh_plug_out_flag_get()==1) {return -1;}
	ret = USBHost_WriteSector(blkno, blkcnt, buf, currentlun);
    if (ret == 0) { 
        return 0;
    } if (ret==USBH_Err_NoMedia) {
        fs_usbh_plug_out_flag_en();
        DBG_PRINT ("============>USBH SUPER PLUG OUT DETECTED<===========\r\n");
    } else { /*if (ret !=0)*/
        ret = USBHost_WriteSector(blkno, blkcnt, buf, currentlun);
    }
    	
    //system_usbh_cheat();
    return ret;	
}

INT32S USB_host_flush(void)
{
	return USB_host_bufferflush();
}
#else

//=========================================================================
INT32S USB_host_initial(void)
{
	INT16S ret;
	
	if(bUSB_Init == -1)
	{
		//USBHostCSWBuffer=arUSBHostCSWBuffer;
		//USBHostBuffer=arUSBHostBuffer;
		//ptr_USBHostBuffer=USBHostBuffer;
		
		ret = DrvUSBHMSDCInitial();
		
		if(ret != 0) 		 // Initial Fail
		{
			//USBHostBuffer = 0;
			//USBHostCSWBuffer = 0;
			bUSB_Init = -1;
			return -1;
		}
	
		bUSB_Init = 0;
	}
	
	return DrvUSBHLUNInit(0);
}

INT32S USB_host_uninitial(void)
{
	if(bUSB_Init == 0)
	{
		DrvUSBHMSDCUninitial();

		//USBHostBuffer = 0;
		//USBHostCSWBuffer = 0;
		bUSB_Init = -1;
	}
	
	return 0;
}

void USB_host_get_drv_info(struct DrvInfo* info)
{
	info->nSectors = 0xffffffff;
	info->nBytesPerSector = DrvUSBHGetSectorSize() << 9;
}

INT16S USB_host_read_sector(INT32U blkno , INT16U blkcnt ,  INT32U buf)
{
	INT16U	i, j;
	INT16S	ret;
	
	i = blkcnt >> 6;
	j = blkcnt & 0x003f;
	
	while(i)
	{
		ret = MSDC_Read28(blkno,  (INT32U *)buf, 64, dsk);
		if(ret != 0)
			return ret;
			
		i--;
		//buf += 64*256; //bug fix by wschung
		buf += 64>>9;
		blkno += 64;
	};
	
	if(j)
		ret = MSDC_Read28(blkno,  (INT32U *)buf, j, dsk);
	
	return ret;
	//return MSDC_Read28(blkno,buf, 1, 0);
}

INT32S USB_host_write_sector(INT32U blkno , INT16U blkcnt ,  INT32U buf)
{
	INT16U	i, j;
	INT16S	ret;
	
	i = blkcnt >> 6;
	j = blkcnt & 0x003f;
	
	while(i)
	{
		ret = MSDC_Write2A(blkno, (INT32U *) buf, 64, dsk);
		if(ret != 0)
			return ret;
			
		i--;
		//buf += 64*256; //bug fix by wschung
		buf += 64>>9;
		blkno += 64;
	};
	
	if(j)
		ret = MSDC_Write2A(blkno,  (INT32U *)buf, j, dsk);
	
	return ret;
	//return MSDC_Write2A(blkno,buf, 1, 0);
}

INT32S USB_host_flush(void)
{
	return 0;
}
#endif		//end of SUPPROT_2K_BLOCK



struct Drv_FileSystem FS_USB_driver = {
	"USB" ,
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
	USB_host_initial,
	USB_host_uninitial,
	USB_host_get_drv_info,
	USB_host_read_sector,
	USB_host_write_sector,
	USB_host_flush,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined USB_NUM) && (USB_NUM >= 1)                      //
//================================================================//