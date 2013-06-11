#ifndef __FS_H__
#define __FS_H__
 
#define C_FS_VERSION	0x0100		// version 1.00

//============================================================================
#define _FAT_CLUSTER_SIZE_4K		0       //ʹ�ù̶��Ĵش�С
#define FAT_OS						0		//�����Ƿ�ʹ��OS

#define MALLOC_USE					0		//�����Ƿ�ʹ��BUDDY
#define USE_GLOBAL_VAR				1

#define _USE_EXTERNAL_MEMORY		0		//ʹ���ⲿRAM,����buddy����
#define _PART_FILE_OPERATE			1		//�˶����ʾ֧��ɾ�������ļ��Ͳ������

#define _READ_WRITE_SPEED_UP		1		//��ʾ����дһ��sectorʱ����ͨ��bufferֱ�Ӷ�disk���ж�д

#define _SEEK_SPEED_UP				1		//�˶����ʾ�Ƿ��seek���٣���Ҫ������ڴ�ռ�Ͷ�ջ

#define _CACHE_FAT_INFLEXION		1		//���ڹյ��seek���ٷ���

#define SUPPORT_GET_FOLDER_NAME		1

#define CHEZAI_MP3					0
#define MAX_FILE_NUM				48		//ָ�������Դ򿪵��ļ�����Ŀ

#define _DIR_UPDATA					1		//��ʾ��д�Ĺ����п��Ը���dir����Ϣ

//֧��С�����Ĵ��̵ĸ�ʽ�����̶���ʽ��ΪFAT16������MBR����Ҫָ�����̵����sector����ʵ��sector��
//��Ϊ���ǵ��ļ�ϵͳ��֧��FAT12��������������С��16MBʱ��PC��Ѵ�������FAT12�����������������������
//��С��16MB�Ĵ��̸�ʽ��Ϊ����16Mb����ָ����С����������ָ������ʵ�ʵĿռ��С
#define _SMALL_DISK_FORMAT			1

#define DISK_DEFRAGMENT_EN			0		//��ʾ֧�ִ���ɨ��

#define ADD_ENTRY					1		//���浱ǰ������Ŀ¼���ƫ����

#define FIND_EXT					1		//֧����չ�Ĳ����㷨

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
      #define SD_EN             1                 //�˶����ʾ��SD���豸
      #define CFC_EN            1                 //CF CARD is enable
      #define NAND1_EN          1                 //ʹ��NAND FLASH�豸
      #define NAND2_EN          0                 //ʹ��NAND FLASH�豸
      #define NAND3_EN          0                 //ʹ��NAND FLASH�豸
      #define USB_NUM           1                 //֧�ֵ�usb host���豸��, Maximum number is 4 
      #define NOR_EN            0                 //ʹ��NOR FLASH�豸
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



//ָ��֧�ֵĴ�����,ΪSD+NAND FLASH�ķ�����


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