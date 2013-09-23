//================================================================================================
//	demo code function:
//	1. use sd and nand 
//  2. copy avi file from sd to nand 
//  3. play avi file from nand 
//================================================================================================
#include "application.h"
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"

#define USE_DISK	FS_NAND1

#define C_AVI_NODE_SIZE			200
INT16U	avi_nand_node_buf[C_AVI_NODE_SIZE];

struct sfn_info g_nand_fs_info;
struct STFileNodeInfo g_nand_node_info;

extern INT8U Display_Device;
extern void TFT_TD025THED1_Init(void);
//=================================================================================================
//	AVI Decode nand sd demo code
//=================================================================================================
INT32S file_copy(INT16S dist_fd, INT16S src_fd, INT32U filesize)
{
	INT32U nSize, read_size, write_size;
	INT32U buffer_addr, buffer_size = 4096*16;
	
	if(dist_fd < 0 || src_fd < 0)
		return -1;
	
	
	buffer_addr = (INT32U) gp_malloc_align(buffer_size, 4);
	nSize = filesize;
	
	DBG_PRINT("Copying Data\r\n");
	while(1)
	{
		if(nSize > buffer_size)
		{
			read_size = read(src_fd, (INT32U)buffer_addr, buffer_size);
			write_size = write(dist_fd, (INT32U)buffer_addr, read_size);				
			nSize -= write_size; 
			DBG_PRINT(".");
		}
		else
		{
			read_size = read(src_fd, (INT32U)buffer_addr, nSize);
			write_size = write(dist_fd, (INT32U)buffer_addr, read_size);
			DBG_PRINT("\r\n");
			break;
		}
	}
	
	close(src_fd);
	close(dist_fd);
	DrvNand_flush_allblk();
	
	DBG_PRINT("Copying Data Finish\r\n");
	gp_free((void *)buffer_addr);
	
	return 0;
}

void Video_Decode_nand_sd_Demo(void)
{ 
	int  	i, nRet;
	char 	path[64], path_backup[64], folder_name[30][64];
	INT16U  file_index, folder_index, total_folder;
	INT16S  status, fd1, fd2;
	INT32U  nand_folder_nums, nand_file_nums;
	
	f_ppos  ppfile;
	struct f_info  file_information;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;
	VIDEO_INFO 	   video_information;
	
	folder_index = 0;
	
	while(1)
	{
		if( _devicemount(USE_DISK))             
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
	#if	USE_DISK == FS_NAND1
			nRet = DrvNand_lowlevelformat();
			DBG_PRINT("NAND LOW LEVEL FORMAT = %d \r\n", nRet);
			nRet = _format(FS_NAND1, FAT32_Type);	
			DBG_PRINT("Format NAND = %d\r\n", nRet);
			DrvNand_flush_allblk();	
	#endif			
			_deviceunmount(USE_DISK);		
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", FS_NAND1);
			break;
		}
	}
	
	//search nand
	g_nand_node_info.disk = FS_NAND1;
	gp_strcpy((INT8S *) g_nand_node_info.extname, (INT8S *) "avi");
	g_nand_node_info.FileBuffer = avi_nand_node_buf;
	g_nand_node_info.FileBufferSize = C_AVI_NODE_SIZE;
	g_nand_node_info.FolderBuffer = NULL;
	g_nand_node_info.FolderBufferSize = 0;
	GetFileNumEx(&g_nand_node_info, &nand_folder_nums, &nand_file_nums);
	DBG_PRINT("Has %d AVI FILE IN NAND!!!\r\n", nand_file_nums );
	DBG_PRINT("NAND Free Space = %d\r\n", vfsFreeSpace(FS_NAND1));
	
	tv_init();
	tft_init();
	if(Display_Device)
		TFT_TD025THED1_Init();
	else
		tv_start (TVSTD_NTSC_J, TV_QVGA, TV_NON_INTERLACE);

	adc_key_scan_init(); //init key scan
	user_defined_video_codec_entrance();
	video_decode_entrance(); 
				
	while(1)
	{
		adc_key_scan();
		if(ADKEY_IO1)                // Start to decode the specified file 
		{
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
				video_decode_stop();
				
			DBG_PRINT("video_decode_start\r\n");
			arg.bScaler = TRUE;
			arg.bUseDefBuf = FALSE;

			arg.TargetWidth = 320;
			arg.TargetHeight = 240;
			arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV;	
			
			src.type = SOURCE_TYPE_FS;
			src.Format.VideoFormat = MJPEG;//only MJPEG
			
			file_index++;
			if(file_index >= nand_file_nums)
				file_index = 0;
			
			ppfile = GetFileNodeInfo(&g_nand_node_info, file_index++, &g_nand_fs_info);
			GetFileInfo(ppfile, &file_information); 
			src.type_ID.FileHandle = sfn_open(&g_nand_fs_info.f_pos);
			src.type_ID.temp = sfn_open(&g_nand_fs_info.f_pos);		
			if((src.type_ID.FileHandle < 0)||(src.type_ID.temp))
			{
				DBG_PRINT("file open fail\r\n");
				close(src.type_ID.FileHandle);
				close(src.type_ID.temp);
				continue;
			}	
			
			status = video_decode_paser_header(&video_information, arg, src);
			if(status != VIDEO_CODEC_STATUS_OK)
			{
				DBG_PRINT("paser header fail !!!\r\n");
				continue;
			}
	
			video_decode_start(arg, src);
			audio_decode_volume(0x3F);
		}
		else if(ADKEY_IO2)         // Stop decoding the current file
		{
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
			{
				DBG_PRINT("video_decode_stop\r\n");
				video_decode_stop();
			}
		}
		else if(ADKEY_IO3)
		{	//get all folder in sd card
			nRet = _devicemount(FS_SD);
			if(nRet == 0)
			{
				DBG_PRINT("get all folder in sd card\r\n");
				total_folder = 0;
				chdir("C:\\"); 		//change to sd
				nRet = _findfirst("*", &file_information, _A_SUBDIR);
				if(!nRet)
				{
					DBG_PRINT("folder = %s\r\n", file_information.f_name);
					gp_strcpy((INT8S*)folder_name[total_folder++], (INT8S *)file_information.f_name);
				}	
				
				do 
				{ 	
					nRet = _findnext(&file_information);
					if(!nRet)
					{
						DBG_PRINT("folder = %s\r\n", file_information.f_name);
						gp_strcpy((INT8S*)folder_name[total_folder++], (INT8S *)file_information.f_name);
					}
				}while(!nRet);
		
				//unmount
				disk_safe_exit(FS_SD);
				nRet = _deviceunmount(FS_SD);
				if(nRet)
					DBG_PRINT("demount sd fail..\r\n");
			}
			else
			{
				_deviceunmount(FS_SD);
				DBG_PRINT("No SD card\r\n");
			}
		}
		else if(ADKEY_IO4)
		{	//copy avi file from sd special folder to nand	
			nRet = _devicemount(FS_SD);
			if(nRet == 0)
			{
				DBG_PRINT("copy avi file from sd special folder to nand\r\n");
				//copy folder avi file to nand
				gp_strcpy((INT8S*)path_backup, (INT8S*)folder_name[folder_index]);
				gp_strcat((INT8S*)path_backup, (INT8S*)"\\");
				gp_strcpy((INT8S*)path, (INT8S*)path_backup);
				gp_strcat((INT8S*)path, (INT8S*)"*.avi");
				
				chdir("C:\\"); //change to sd	 
				nRet = _findfirst(path, &file_information, D_ALL);
				if(!nRet)
				{
					DBG_PRINT("File = %s, size = %d\r\n", file_information.f_name, file_information.f_size);
					gp_strcpy((INT8S*)path, (INT8S*)path_backup);
					gp_strcat((INT8S*)path, (INT8S*)file_information.f_name);
					
					chdir("C:\\"); //change to sd	
					fd1 = open(path, O_RDONLY);
						
					chdir("A:\\"); //change to nand
					fd2 = open((char *)file_information.f_name, O_WRONLY|O_CREAT|O_TRUNC);
					if(vfsFreeSpace(FS_NAND1) > file_information.f_size)
						file_copy(fd2, fd1, file_information.f_size);
				}
				
				do 
				{
					nRet = _findnext(&file_information);
					if(!nRet)
					{
						DBG_PRINT("File = %s, size = %d\r\n", file_information.f_name, file_information.f_size);
						gp_strcpy((INT8S*)path, (INT8S*)path_backup);
						gp_strcat((INT8S*)path, (INT8S*)file_information.f_name);
						
						chdir("C:\\"); //change to sd	
						fd1 = open(path, O_RDONLY);
						chdir("A:\\"); //change to nand
						fd2 = open((char *)file_information.f_name, O_WRONLY|O_CREAT|O_TRUNC);
						if(vfsFreeSpace(FS_NAND1) > file_information.f_size)	
							file_copy(fd2, fd1, file_information.f_size);
					}
				}while(!nRet);
				
				disk_safe_exit(FS_SD);
				nRet = _deviceunmount(FS_SD);
				if(nRet)
					DBG_PRINT("demount sd fail..\r\n");
				
				//update NAND information 
				g_nand_node_info.disk = FS_NAND1;
				gp_strcpy((INT8S *) g_nand_node_info.extname, (INT8S *) "avi");
				g_nand_node_info.FileBuffer = avi_nand_node_buf;
				g_nand_node_info.FileBufferSize = C_AVI_NODE_SIZE;
				g_nand_node_info.FolderBuffer = NULL;
				g_nand_node_info.FolderBufferSize = 0;
				GetFileNumEx(&g_nand_node_info, &nand_folder_nums, &nand_file_nums);
				DBG_PRINT("Has %d AVI FILE IN NAND!!!\r\n", nand_file_nums);
				DBG_PRINT("NAND Free Space = %d\r\n", vfsFreeSpace(FS_NAND1));
			}
			else
			{
				_deviceunmount(FS_SD);
				DBG_PRINT("No SD card\r\n");
			}
		}
		else if(ADKEY_IO5)
		{		
			folder_index++;
			if(folder_index >= total_folder)
				folder_index = 0;
			DBG_PRINT("Select folder = %s \r\n", folder_name[folder_index]);
		}
		else if(ADKEY_IO6)
		{	//show all nand avi file 
			for(i = 0; i< nand_file_nums; i++)
			{
				ppfile = GetFileNodeInfo(&g_nand_node_info, i, &g_nand_fs_info);
				GetFileInfo(ppfile, &file_information); 
				DBG_PRINT("name = %s", (char *)file_information.f_name);
				DBG_PRINT(" size = %d\r\n", file_information.f_size);
			}
		}
		else if(ADKEY_IO7)
		{
			
		}
		else if(ADKEY_IO8)
		{
			DBG_PRINT("format nand....");	
			nRet = _format(FS_NAND1, FAT32_Type);
			if(nRet)
				DBG_PRINT("fail..\r\n");
			else
				DBG_PRINT("success..\r\n");	
		}	
	}
}
