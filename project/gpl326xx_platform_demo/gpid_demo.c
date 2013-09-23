#include "application.h"
#include "drv_l2_ad_key_scan.h"
#include "res_bin_file_mgr.h"
#include "gpid_audio.h"
#include "gpid_process.h"
#include "Book_Select.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1
//#define USE_DISK	FS_USBH

#define MAX_SONG_INDEX	10

#define AP_LOCATE_RES_BIN_FILE		0

//=============================================================================
// Static functions
//=============================================================================
#if AP_LOCATE_RES_BIN_FILE
static INT32S LocateAndOpenResBinFile(void);
#endif

INT16U file_node_buf[MAX_SONG_INDEX*20];
	
//=================================================================================================
//	GPID demo code
//=================================================================================================
void gpid_demo(void)
{
	char path[24];
	INT32S ret;
	struct STFileNodeInfo file_node_info;
	INT32U file_num;
	INT32U folder_num;
	
	while(1)
	{	
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
		#if USE_DISK == FS_NAND1
			ret = _format(FS_NAND1, FAT32_Type); 
			DrvNand_flush_allblk();   
			_deviceunmount(FS_NAND1); 
		#endif
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			break;
		}
	}
	
	switch(USE_DISK)
	{
	case FS_SD:	
		gp_memcpy((INT8S*)path, (INT8S*)"C:\\gpid_demo.bin", sizeof(path));
		break;
	case FS_NAND1:	
		gp_memcpy((INT8S*)path, (INT8S*)"A:\\gpid_demo.bin", sizeof(path));
		break;	
	case FS_USBH:	
		gp_memcpy((INT8S*)path, (INT8S*)"G:\\gpid_demo.bin", sizeof(path));
		break;		
	}

	// Open a res bin file and play resource inside it
	// Find .bin files on the storage
	file_node_info.disk = USE_DISK;
	gp_strcpy((INT8S *) file_node_info.extname, (INT8S *)"bin");
	file_node_info.pExtName = NULL;
	file_node_info.FileBuffer = file_node_buf;
	file_node_info.FileBufferSize = MAX_SONG_INDEX*40;
	file_node_info.FolderBuffer =NULL; 
	file_node_info.FolderBufferSize = 0;
	file_node_info.filter			=NULL;
	GetFileNumEx(&file_node_info, &folder_num, &file_num);
	if (file_num == 0)
 		while(1);

#if AP_LOCATE_RES_BIN_FILE
	ret = LocateAndOpenResBinFile();
	if (ret >= 0)
	{
		ret = RBF_Open(0);
		if (ret != C_RBF_RC_SUCCESS)
			while(1);
	}
#else
	// Associate res bin files with .bin files on a certain storage
	ret = RBF_FindFirstBin(USE_DISK);
	if (ret != C_RBF_RC_SUCCESS)
		while (1);

	// Open the first bin file on the storage
	ret = RBF_Open(0);
	if (ret != C_RBF_RC_SUCCESS)
		while (1);
#endif	// #if AP_LOCATE_RES_BIN_FILE

	// Initialize and allocate the resources needed by audio decoder
	audio_decode_entrance();
	gpid_audio_init(RBF_GetFd());

	API_GpidInitial();
	
	GpidProc_Init();
	GpidProc_SetRecogRate(8);
	
	DBG_PRINT("Start GPID Demo\r\n");

	while (1)
	{
		// RBF v1: book selection cannot be enabled
		// RBF v2: book selection can be enabled or disabled
		// RBF v3: book selection can be enabled or disabled
		if ( !BS_IsBookSelectEnabled() )
		{
#if C_RBF_SUPPORT_RBF_V1
			if ( RBF_GetVersion() == 0x01 )
				gpid_process_v1();
#endif
		}

#if C_RBF_SUPPORT_RBF_V2 || C_RBF_SUPPORT_RBF_V3
		gpid_process();
#endif

		OSTimeDly(1);
	}
}

//=============================================================================
// Note: Find the first .bin as res bin file without checking it.
//=============================================================================
#if AP_LOCATE_RES_BIN_FILE
static INT32S
LocateAndOpenResBinFile(void)
{
	INT32S ret;
	INT16S fd, fd_gm;
	struct f_info file;

	chdir("C:\\");

	ret = _findfirst(((CHAR *)"*.bin"), &file, D_ALL);
	if(ret != 0)
		return (-1);

	while (1)
	{
		fd = open((CHAR *)file.f_name,O_RDONLY);
	    if(fd >= 0)
	    {
			fd_gm = open((CHAR *)file.f_name,O_RDONLY);
			if(fd_gm < 0)
			{
				close(fd);
				return (-1);
			}

			RBF_SetFd(fd, fd_gm);
			return 0;
		}

	    ret = _findnext(&file);
	    if (ret != 0)
	    	return (-1);
	}
}
#endif
