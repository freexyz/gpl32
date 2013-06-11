#ifndef __FS_H__
#define __FS_H__
 
#define C_FS_VERSION	0x0100		// version 1.00

//============================================================================
#define _FAT_CLUSTER_SIZE_4K		0       //使用固定的簇大小
#define FAT_OS						0		//定义是否使用OS

#define MALLOC_USE					0		//定义是否使用BUDDY
#define USE_GLOBAL_VAR				1

#define _USE_EXTERNAL_MEMORY		0		//使用外部RAM,并用buddy管理
#define _PART_FILE_OPERATE			1		//此定义表示支持删除部分文件和插入操作

#define _READ_WRITE_SPEED_UP		1		//表示读或写一个sector时不需通过buffer直接对disk进行读写

#define _SEEK_SPEED_UP				1		//此定义表示是否对seek加速，需要额外的内存空间和堆栈

#define _CACHE_FAT_INFLEXION		1		//基于拐点的seek加速方法

#define SUPPORT_GET_FOLDER_NAME		1

#define CHEZAI_MP3					0
#define MAX_FILE_NUM				48		//指定最多可以打开的文件的数目

#define _DIR_UPDATA					1		//表示在写的过程中可以更新dir的信息

//支持小容量的磁盘的格式化，固定格式化为FAT16，不带MBR，需要指定磁盘的最大sector数和实际sector数
//因为我们的文件系统不支持FAT12，而当磁盘容量小于16MB时，PC会把磁盘认做FAT12，所以做了这个函数，可以
//把小于16MB的磁盘格式化为大于16Mb（可指定大小），并可以指定磁盘实际的空间大小
#define _SMALL_DISK_FORMAT			1

#define DISK_DEFRAGMENT_EN			0		//表示支持磁盘扫描

#define ADD_ENTRY					1		//保存当前所处的目录项的偏移量

#define FIND_EXT					1		//支持扩展的查找算法

#define  ALIGN_16                   32     //modify by wangzhq
#define SUPPORT_VOLUME_FUNCTION		1
#if CHEZAI_MP3 == 1
	#define READ_ONLY				1	
#endif

#define FS_FORMAT_SPEEDUP			1		// support fs format speedup, the file system will malloc 8kB buffer

//============================================================================
#ifndef CUSTOMER_DEFINE  /* Engineer Mode */
  #ifndef _FS_DEFINE_
  #define _FS_DEFINE_
      #define FS_NAND1    0
      #define FS_NAND2    1
      #define FS_SD       2
      #define FS_MS       3
      #define FS_CF       4
      #define FS_XD       5
      #define FS_USBH     6
      #define FS_NOR      7
      #define FS_DEV_MAX  8
/* DEV MODE */
      #define MSC_EN            1
      #define SD_EN             1                 //此定义表示带SD卡设备
      #define CFC_EN            1                 //CF CARD is enable
      #define NAND1_EN          1                 //使用NAND FLASH设备
      #define NAND2_EN          0                 //使用NAND FLASH设备
      #define NAND3_EN          0                 //使用NAND FLASH设备
      #define USB_NUM           1                 //支持的usb host的设备数, Maximum number is 4 
      #define NOR_EN            0                 //使用NOR FLASH设备
      #define XD_EN             0 
      #define RAMDISK_EN        0
    //#define MAX_DISK_NUM     (MSC_EN + SD_EN + CFC_EN + NAND1_EN + NAND2_EN + NAND3_EN + USB_NUM + NOR_EN + RAMDISK_EN)
    #define MAX_DISK_NUM      FS_DEV_MAX
    #define MAX_LUN_NUMS      (MSC_EN + SD_EN + CFC_EN + NAND1_EN + NAND2_EN + NAND3_EN + USB_NUM + NOR_EN + RAMDISK_EN)
  #endif // _FS_DEFINE_ 
#else /* Customer Mode */
    #ifndef _FS_DEFINE_
    #define _FS_DEFINE_  
      #define FS_NAND1    0
      #define FS_NAND2    1
      #define FS_SD       2
      #define FS_MS       3
      #define FS_CF       4
      #define FS_XD       5
      #define FS_USBH     6
      #define FS_NOR      7
      #define FS_DEV_MAX  8

      /* Storage Support Suite*/
      #define STG_SD_MSC                       ((1<<4*FS_SD)|(1<<4*FS_MS))
      #define STG_SD_MSC_USB                   ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH))
      #define STG_SD_MSC_XD_USB                ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_XD))
      #define STG_SD_MSC_NAND1_USB             ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_NAND1))
      #define STG_SD_MSC_NAND1_XD_USB          ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_NAND1)|(1<<4*FS_XD))
      #define STG_SD_MSC_NAND1_CF_XD_USB       ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_NAND1)|(1<<4*FS_XD)|(1<<4*FS_CF))
      #define STG_SD_MSC_NAND1_NAND2_CF_XD_USB ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_NAND1)|(1<<4*FS_XD)|(1<<4*FS_CF)|(1<<4*FS_NAND2))
      #define STG_SD_MSC_CF_XD_USB             ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_XD)|(1<<4*FS_CF))
      #define STG_SD				           (1<<4*FS_SD)
      #define STG_SD_USB             		   ((1<<4*FS_SD)|(1<<4*FS_USBH))
      #define STG_SD_MSC_NOR_USB               ((1<<4*FS_SD)|(1<<4*FS_MS)|(1<<4*FS_USBH)|(1<<4*FS_NOR))
      
      #if (STG_SUPPORT_MODE == STG_MODE_0)
            #define STG_MODE  STG_SD_MSC
      #elif  (STG_SUPPORT_MODE == STG_MODE_1)
            #define STG_MODE  STG_SD_MSC_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_2)
            #define STG_MODE  STG_SD_MSC_XD_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_3)
            #define STG_MODE  STG_SD_MSC_NAND1_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_4)
            #define STG_MODE  STG_SD_MSC_NAND1_XD_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_5)
            #define STG_MODE  STG_SD_MSC_NAND1_CF_XD_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_6)
            #define STG_MODE  STG_SD_MSC_NAND1_NAND2_CF_XD_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_7)
            #define STG_MODE  STG_SD_MSC_CF_XD_USB
      #elif  (STG_SUPPORT_MODE == STG_MODE_8)
            #define STG_MODE  STG_SD
      #elif  (STG_SUPPORT_MODE == STG_MODE_9)
            #define STG_MODE  STG_SD_USB  
      #elif (STG_SUPPORT_MODE == STG_MODE_10) 
      		 #define STG_MODE  STG_NAND1_NAND2                 
      #elif  (STG_SUPPORT_MODE == STG_MODE_11)
            #define STG_MODE  STG_SD_MSC_NOR_USB
      #endif

      #define NOSUPPORT         0
      #define MSC_EN            ((STG_MODE>>4*FS_MS)&0x1)
      #define SD_EN             ((STG_MODE>>4*FS_SD)&0x1)
      #define CFC_EN            ((STG_MODE>>4*FS_CF)&0x1)
      #define NAND1_EN          ((STG_MODE>>4*FS_NAND1)&0x1)
      #define NAND2_EN          ((STG_MODE>>4*FS_NAND2)&0x1)
      #define USB_NUM           ((STG_MODE>>4*FS_USBH)&0x1)
      #define XD_EN             ((STG_MODE>>4*FS_XD)&0x1)
      #define RAMDISK_EN        NOSUPPORT
      #define NOR_EN            NOSUPPORT
      #define NAND3_EN          NOSUPPORT
      #define MAX_DISK_NUM      FS_DEV_MAX
      #define MAX_LUN_NUMS      (MSC_EN + SD_EN + CFC_EN + NAND1_EN + XD_EN + NAND2_EN + CDROM_EN)
    #endif // _FS_DEFINE_
#endif



//指定支持的磁盘数,为SD+NAND FLASH的分区数


//============================================================================

//#define LFN_API				1
#define LFN_FLAG			1
#define WITHFAT32			1
#define WITHFAT12			1
#ifndef READ_ONLY
#define WITH_CDS_PATHSTR	1				// Remember current position
#else
 #define WITH_CDS_PATHSTR	0				// Remember current position
#endif 

#define NUMBUFF				4

#define FSDRV_MAX_RETRY		1

#define C_MAX_DIR_LEVEL		16				// Max directory deepth level
#define	C_FILENODEMARKDISTANCE	100

#define FD32_LFNPMAX 		260				// Max length for a long file name path, including the NULL terminator
#define FD32_LFNMAX  		255				// Max length for a long file name, including the NULL terminator

#if FS_FORMAT_SPEEDUP == 1
	#define C_FS_FORMAT_BUFFER_SECTOR_SIZE		32
	#define C_FS_FORMAT_BUFFER_SIZE				(512 * C_FS_FORMAT_BUFFER_SECTOR_SIZE)	// 8192 Byte

	#define GLOBAL_BUFFER						1
	#define ALLOCATE_BUFFER						2
	#define FS_FORMAT_SPEEDUP_BUFFER_SOURCE		ALLOCATE_BUFFER
#else
	#define GLOBAL_BUFFER						1
	#define ALLOCATE_BUFFER						2
	#define FS_FORMAT_SPEEDUP_BUFFER_SOURCE		GLOBAL_BUFFER
#endif

#endif		// __FS_H__