#ifndef	__FORMAT_H__
#define __FORMAT_H__

#define format_printf(x)   
#define format_printf1(x,y)   

/***************************************************************************/
#define C_MBR_ZeroSec_Offset			0x000
#define C_MBR_Type_Addr					0x1c2
#define C_MBR_SecOffset_Addr			0x1c6
#define C_MBR_SecNumber_Addr			0x1ca

#define C_DBR_First_Byte				0xeb
#define C_DBR_FAT16_Second_Byte			0x3c
#define C_DBR_FAT32_Second_Byte			0x58
#define C_DBR_Third_Byte				0x90

#define MBR_FAT16_Type	        		0x06
#define MBR_FAT32_Type	        		0x0B

#define DBR_nbyte_Addr					0x0b
#define DBR_nsector_Addr				0x0d
#define DBR_nreserved_Addr				0x0e
#define DBR_nfat_Addr					0x10
#define DBR_ndirent_Addr				0x11
#define DBR_nsize_Addr					0x13
#define DBR_mdesc_Addr					0x15
#define DBR_nfsect_Addr					0x16
#define DBR_hidden_Addr					0x1c			
#define DBR_huge_Addr					0x20
#define DBR_xnfsect_Addr				0x24
#define DBR_xflags_Addr		    		0x28
#define DBR_xfsversion_Addr		   		0x2a
#define DBR_xrootclst_Addr				0x2c
#define DBR_xfsinfosec_Addr				0x30
#define DBR_xbackupsec_Addr		    	0x32
#define DBR_FAT16_VOL_ID				39
#define DBR_FAT16_VOL_LAB			    43
#define DBR_FAT32_VOL_ID				67
#define DBR_FAT32_VOL_LAB			    71

#define DBR_FAT16_FilSysType			54
#define DBR_FAT32_FilSysType			82

#define DBR_end_Addr					0x1fe

struct DSKSZTOSECPERCLUS
{
 	 INT32S DiskSize;
	 INT8S  SecPerClusVal;
};

#endif
