
/*
* Description: This task reads data from specified file into specified buffer
*
* Author: Tristan Yang
*
* Date: 2008/02/25
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "turnkey_filesrv_task.h"

//#define TK_FILESRV_DEBUG


#define C_FS_Q_MAX				128
#define C_FS_MAX_MSG			128
#define C_FS_MSG_LENGTH			128

#define C_FS_SCAN_Q_MAX			10
#define C_FS_SCAN_MAX_MSG		10
#define C_FS_SCAN_MSG_LENGTH	4

#define C_FILE_SCAN_COUNT		12		// you can scan 5 disk at one time

typedef struct
{
	INT8S	flag;
	struct STFileNodeInfo *pstFNodeInfo;
	STDiskFindControl stDiskFindControl;
} STFileScanCtrl;

MSG_Q_ID	fs_msg_q_id;
MSG_Q_ID	fs_scan_msg_q_id;

static INT32U gFileScanCount;
STFileScanCtrl gstFileScanCtrl[C_FILE_SCAN_COUNT];

static void FileSrvMount(STMountPara *para);
static void FileSrvRead(P_TK_FILE_SERVICE_STRUCT para);
static void FileUserDefineRead(P_TK_FILE_SERVICE_STRUCT para);	// added by Bruce, 2008/10/27
static void FileSrvCopy(TK_FILE_COPY_STRUCT *para);
static void FileSrvDelete(TK_FILE_DELETE_STRUCT *para);
static void FileSrvSave(TK_FILE_SAVE_STRUCT *para);
#if GPLIB_NVRAM_SPI_FLASH_EN == 1
static void FileSrvNVRAMAudioGPRSRead(P_TK_FILE_SERVICE_STRUCT para);
static void FileSrvNVRAMAudioAppRead(P_TK_FILE_SERVICE_STRUCT para);
static void FileSrvNVRAMAudioAppPackedRead(P_TK_FILE_SERVICE_STRUCT para);
static INT32S file_service_nvram_audio_gprs_read(INT16U sec_offset, INT32U buf_addr,INT16U sec_cnt);
static void FileSrvNvL3Read(P_TK_FILE_SERVICE_STRUCT para);
//static void FileSrvNvL3_Fast_Read(P_TK_FILE_SERVICE_STRUCT para);
static INT32S file_service_spi_l3_read(INT8U *src_name, INT32U buf_addr);
#endif
static void FileSrvScanFileInit(void);
static void FileSrvScanFileStart(STScanFilePara *para);

void filesrv_task_init(void)
{
	fs_msg_q_id = msgQCreate(C_FS_Q_MAX, C_FS_MAX_MSG, C_FS_MSG_LENGTH);
	if(fs_msg_q_id == NULL) {
		DBG_PRINT("Create file service message queue faile\r\n");
	}

	fs_scan_msg_q_id = msgQCreate(C_FS_SCAN_Q_MAX, C_FS_SCAN_MAX_MSG, C_FS_SCAN_MSG_LENGTH);
	if(fs_scan_msg_q_id == NULL)
	{
		DBG_PRINT("Create file scan service message queue faile\r\n");
	}

    FileSrvScanFileInit();
}

void filesrv_task_entry(void *parm)
{
	INT32U msg_id;
	INT32S status;
	INT8U  para[C_FS_MSG_LENGTH];

	filesrv_task_init();
	while(1)
	{
		status = msgQReceive(fs_msg_q_id, &msg_id, (void *)para, C_FS_MSG_LENGTH);
		if(status != 0)
		{
			continue;
		}
		switch(msg_id)
		{
			case MSG_FILESRV_MOUNT:
				FileSrvMount((STMountPara*)para);
				break;
			case MSG_FILESRV_FS_READ:
				FileSrvRead((P_TK_FILE_SERVICE_STRUCT)para);
				break;
			case MSG_FILESRV_USER_DEFINE_READ:	// added by Bruce, 2008/10/27
				FileUserDefineRead((P_TK_FILE_SERVICE_STRUCT)para);
				break;
#if GPLIB_NVRAM_SPI_FLASH_EN == 1
			case MSG_FILESRV_NVRAM_AUDIO_GPRS_READ:
				FileSrvNVRAMAudioGPRSRead((P_TK_FILE_SERVICE_STRUCT)para);
				break;
			case MSG_FILESRV_NVRAM_AUDIO_APP_READ:
				FileSrvNVRAMAudioAppRead((P_TK_FILE_SERVICE_STRUCT)para);
				break;
			case MSG_FILESRV_NVRAM_AUDIO_APP_PACKED_READ:
				FileSrvNVRAMAudioAppPackedRead((P_TK_FILE_SERVICE_STRUCT)para);
				break;
			case MSG_FILESRV_SPI_L3_READ:
				FileSrvNvL3Read((P_TK_FILE_SERVICE_STRUCT)para);
				break;
//			case MSG_FILESRV_SPI_L3_FAST_READ:
//		    	FileSrvNvL3_Fast_Read((P_TK_FILE_SERVICE_STRUCT)para);
//				break;
#endif
			case MSG_FILESRV_FLUSH:
				OSQFlush(((P_TK_FILE_SERVICE_STRUCT) para)->result_queue);
				OSQPost(((P_TK_FILE_SERVICE_STRUCT) para)->result_queue, (void *) C_FILE_SERVICE_FLUSH_DONE);
				break;
			case MSG_FILESRV_SCAN:
				FileSrvScanFileStart((STScanFilePara *)para);
				msgQSend(fs_scan_msg_q_id, MSG_FILESRV_SCAN_CONTINUE, NULL, 0, MSG_PRI_NORMAL);
				break;
			case MSG_FILESRV_AVIAUDIO_FS_READ:
				FileSrv_AVIAudio_Read((STAVIAUDIOBufConfig *)para);
				break;
			case MSG_FILESRV_AVIVIDEO_FS_READ:
				FileSrv_AVIVideo_Read(*(INT32U *)para);
				break;
			case MSG_FILESRV_COPY:
				FileSrvCopy((TK_FILE_COPY_STRUCT *)para);
				break;
			case MSG_FILESRV_DEL:
				FileSrvDelete((TK_FILE_DELETE_STRUCT *)para);
				break;
			case MSG_FILESRV_SAVE:
				FileSrvSave((TK_FILE_SAVE_STRUCT *)para);
				break;
			default:
				break;
		}
	}
}

void file_scan_task_init(void)
{

}

void file_scan_task_entry(void *parm)
{
	INT32U msg_id;
	INT32S status;
	INT8U  para[C_FS_SCAN_MSG_LENGTH];

	file_scan_task_init();
	while(1)
	{
		status = msgQReceive(fs_scan_msg_q_id, &msg_id, (void *)para, C_FS_SCAN_MSG_LENGTH);
		if(status != 0)
		{
			continue;
		}
		switch(msg_id)
		{
			case MSG_FILESRV_SCAN_CONTINUE:
				status = FileSrvScanFileContinue();
				if(status)
				{
					msgQSend(fs_scan_msg_q_id, MSG_FILESRV_SCAN_CONTINUE, NULL, 0, MSG_PRI_NORMAL);
				}
				break;
			default:
				break;
		}
	}
}

static void FileSrvMount(STMountPara *para)
{
	INT32S status;

	status = _devicemount(para->disk);					/* receive mount parameter */
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) status);	/* send return status */
	}
}

static void FileSrvRead(P_TK_FILE_SERVICE_STRUCT para)
{
	INT32S read_cnt;

	read_cnt = read(para->fd, para->buf_addr, para->buf_size);
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) read_cnt);
	}
}

static void FileSrvCopy(TK_FILE_COPY_STRUCT *para)
{
#if _PROJ_TYPE == _PROJ_TURNKEY 	
	para->status = storage_fcopy(para->src_storage_id, para->file_type, para->file_idx, para->dest_storage_id, para->dest_name);
#endif	
	msgQSend(ApQ, MSG_FILESRV_COPY_REPLY, (void *)para, sizeof(TK_FILE_COPY_STRUCT), MSG_PRI_NORMAL);
}

static void FileSrvDelete(TK_FILE_DELETE_STRUCT *para)
{
	para->status = sfn_unlink(&para->fpos);
	msgQSend(ApQ, MSG_FILESRV_DELETE_REPLY, (void *)para, sizeof(TK_FILE_DELETE_STRUCT), MSG_PRI_NORMAL);
}

static void FileSrvSave(TK_FILE_SAVE_STRUCT *para)
{
#if _PROJ_TYPE == _PROJ_TURNKEY 
	para->status = sys_save(para->p_buffer, para->size, para->ext_name, para->dest_storage_id, para->dest_name);
#endif
	msgQSend(ApQ, MSG_FILESRV_SAVE_REPLY, (void *)para, sizeof(TK_FILE_SAVE_STRUCT), MSG_PRI_NORMAL);
}

INT32S file_server_copy(TK_FILE_COPY_STRUCT *para)
{
	return msgQSend(fs_msg_q_id, MSG_FILESRV_COPY, (void *)para, sizeof(TK_FILE_COPY_STRUCT), MSG_PRI_NORMAL);
}

INT32S file_server_delete(TK_FILE_DELETE_STRUCT *para)
{
	return msgQSend(fs_msg_q_id, MSG_FILESRV_DEL, (void *)para, sizeof(TK_FILE_DELETE_STRUCT), MSG_PRI_NORMAL);
}

INT32S file_server_save(TK_FILE_SAVE_STRUCT *para)
{
	return msgQSend(fs_msg_q_id, MSG_FILESRV_SAVE, (void *)para, sizeof(TK_FILE_SAVE_STRUCT), MSG_PRI_NORMAL);
}

static void FileUserDefineRead(P_TK_FILE_SERVICE_STRUCT para)	// added by Bruce, 2008/10/27
{
	INT32S read_cnt;

	switch(para->main_channel)
	{
		case AUDIO_CHANNEL_SPU:
			if (audio_bg_move_data != NULL)
				read_cnt = audio_bg_move_data(para->buf_addr, para->buf_size, para->data_start_addr, para->data_offset);
			break;

		case AUDIO_CHANNEL_DAC:
			if(audio_move_data != NULL)
				read_cnt = audio_move_data(para->buf_addr, para->buf_size, para->data_start_addr, para->data_offset);
			break;

		default:
			read_cnt = -2;// -2 means AUDIO_READ_FAIL;
			break;
	}

	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) read_cnt);
	}
}

#if GPLIB_NVRAM_SPI_FLASH_EN == 1
static void FileSrvNVRAMAudioGPRSRead(P_TK_FILE_SERVICE_STRUCT para)
{
	INT32S read_cnt;

	read_cnt = file_service_nvram_audio_gprs_read(para->spi_para.sec_offset, para->buf_addr, para->spi_para.sec_cnt);
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) read_cnt);
	}
}

static INT32S file_service_nvram_audio_gprs_read(INT16U sec_offset, INT32U buf_addr,INT16U sec_cnt)
{
#if _PROJ_TYPE == _PROJ_TURNKEY 
	if (resource_audio_load(sec_offset, (INT32U *)buf_addr ,sec_cnt) != STATUS_OK) {
		return -1;
	}
#endif
	return sec_cnt*512;
}

static void FileSrvNVRAMAudioAppRead(P_TK_FILE_SERVICE_STRUCT para)
{
	INT32S read_cnt;

	read_cnt = nv_read(para->fd, para->buf_addr, para->buf_size);
	if(read_cnt == 0) {
		read_cnt = para->buf_size;
	} else {
		read_cnt = -1;
	}
	
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) read_cnt);
	}
}

static void FileSrvNVRAMAudioAppPackedRead(P_TK_FILE_SERVICE_STRUCT para)
{
	INT32S read_cnt = 0;
#if _PROJ_TYPE == _PROJ_TURNKEY 
	read_cnt = nvp_fread(para->fd, para->buf_addr, para->buf_size);
#endif	
	if (read_cnt == 0) {
		read_cnt = para->buf_size;
	}

	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) read_cnt);
	}
}

#if 0
static void FileSrvNvL3_Fast_Read(P_TK_FILE_SERVICE_STRUCT para)
{
	INT32S read_cnt;
	INT32U handle = 0 ;
	INT32U size   = 0;

	handle = nv_open(para->spi_para.src_name);
	if(handle == 0xffff) {
		return ;
	}
	size = nv_lseek(handle,0,SEEK_END);
	nv_lseek(handle,0,SEEK_SET);
	if (nv_fast_read(handle,(INT32U)para->buf_addr, size) != 0) {
		return ;
	}
	read_cnt = file_service_spi_l3_read(para->spi_para.src_name, para->buf_addr);
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) size);
	}
}
#endif

static void FileSrvNvL3Read(P_TK_FILE_SERVICE_STRUCT para)
{
	INT32S read_cnt;

	read_cnt = file_service_spi_l3_read(para->spi_para.src_name, para->buf_addr);
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) read_cnt);
	}
}

static INT32S file_service_spi_l3_read(INT8U *src_name, INT32U buf_addr)
{
	INT32U handle = 0 ;
	INT32U size   = 0;

	handle = nv_open(src_name);
	if(handle == 0xffff) {
		return -1;
	}

	size = nv_lseek(handle,0,SEEK_END);
	nv_lseek(handle,0,SEEK_SET);

	if (nv_read(handle,(INT32U)buf_addr, size) != 0) {
		return -1;
	}

	return size;
}
#endif

static void FileSrvScanFileInit(void)
{
	INT32S i;

	gFileScanCount = 0;
    for(i = 0; i < C_FILE_SCAN_COUNT; i++)
    {
    	gstFileScanCtrl[i].flag = 0;
    }
}

// if scan file faile, what to do??
static void FileSrvScanFileStart(STScanFilePara *para)
{
	INT32S i;
	INT32S status;

	for(i = 0; i < C_FILE_SCAN_COUNT; i++)
	{
		if(!gstFileScanCtrl[i].flag)
		{
			break;
		}
	}
	if(i == C_FILE_SCAN_COUNT)
	{
		DBG_PRINT("no enough file service struct to use\r\n");
		return;
	}

#ifdef TK_FILESRV_DEBUG
	DBG_PRINT("start%d\t\t%s\r\n", i, para->pstFNodeInfo->pExtName ? para->pstFNodeInfo->pExtName : para->pstFNodeInfo->extname);
#endif

	gstFileScanCtrl[i].pstFNodeInfo = para->pstFNodeInfo;
	status = file_search_start(gstFileScanCtrl[i].pstFNodeInfo, &gstFileScanCtrl[i].stDiskFindControl);
	gstFileScanCtrl[i].flag = 1;
	if(para->result_queue)
	{
		OSQPost(para->result_queue, (void *) status);
	}
}

INT32S FileSrvScanFileContinue(void)
{
	INT32U i = 0;
	INT32S status;

	while(1)
	{
		if(gstFileScanCtrl[gFileScanCount].flag)
		{
			status = file_search_continue(gstFileScanCtrl[gFileScanCount].pstFNodeInfo, &gstFileScanCtrl[gFileScanCount].stDiskFindControl);
			if(status == 1)		// search complete
			{
				gstFileScanCtrl[gFileScanCount].flag = 0;		// free gstFileScanCtrl[i]
			#ifdef TK_FILESRV_DEBUG
				DBG_PRINT("end%d\r\n", gFileScanCount);
			#endif
			}
		#ifdef TK_FILESRV_DEBUG
			else
			{
				DBG_PRINT("c%d\r\n", gFileScanCount);
			}
		#endif

			gFileScanCount++;
			if(gFileScanCount == C_FILE_SCAN_COUNT)
			{
				gFileScanCount = 0;
			}
			return 1;
		}

		gFileScanCount++;
		if(gFileScanCount == C_FILE_SCAN_COUNT)
		{
			gFileScanCount = 0;
		}

		i++;
		if(i == C_FILE_SCAN_COUNT)
		{
			break;
		}
	}
	return 0;
}

//========================================================
//Function Name:WaitScanFile
//Syntax:		void ScanFileWait(struct STFileNodeInfo *pstFNodeInfo, INT32S index)
//Purpose:		wait for search file
//Note:
//Parameters:   pstFNodeInfo	/* the point to file node information struct */
//				index			/* the file index you want to find */
//Return:
//=======================================================
void ScanFileWait(struct STFileNodeInfo *pstFNodeInfo, INT32S index)
{
	if( (pstFNodeInfo->flag == 1) || 				/* file is search complete */
		(pstFNodeInfo->MaxFileNum - 1 > index) )	/* have enough file been searched */
	{
		return;
	}

	while(pstFNodeInfo->flag != 1)
	{
		OSTimeDly(1);
		if(pstFNodeInfo->MaxFileNum - 1 > index)	/* have enough file been searched */
		{
			break;
		}
		if(0 != _changedisk(pstFNodeInfo->disk))
		{
			break;
		}
		if(pstFNodeInfo->FileBuffer == NULL)
		{
			break;
		}
	}
}
