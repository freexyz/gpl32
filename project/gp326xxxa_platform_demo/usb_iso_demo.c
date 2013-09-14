
#include "application.h"
#include "driver_l1_cfg.h"
#include "drv_l2_usbh_uvc.h" 
#include "task_video_decoder.h"

extern void usbh_uvc_task_entry(void *parm);

#define UVC_FRAME_TaskStackSize 0x2000

//#define DISP_TYPE	C_TV_QVGA
#define DISP_TYPE	C_TV_VGA
#if DISP_TYPE == C_TV_QVGA
	#define TV_TYPE		TV_QVGA
	#define RES_WIDTH	320
	#define RES_HIGH	240
#elif DISP_TYPE == C_TV_VGA
	#define TV_TYPE		TV_HVGA
	#define RES_WIDTH	640
	#define RES_HIGH	480
#endif

#define DISP_SIZE	RES_WIDTH*RES_HIGH*2

#define BUF_NUM		3

unsigned * g_pUVC_FRAME_STACK;

void USBH_ISO_Demo(void)
{
	unsigned char *buf[BUF_NUM];
	unsigned i;
	unsigned msg_id;
	unsigned buf_cnt = 0;
	unsigned *tv_buf;
	MSG_Q_ID uvc_state = msgQCreate(32, 32,0);
	
	tv_start(TVSTD_NTSC_M, TV_TYPE, TV_INTERLACE);

	Drv_USBH_Init();
	/* ----- Wait for USB connect ----- */
	while(1)
	{
		if(Drv_USBH_Detect()== TRUE)
			break;		
	}
	/* ----- Initial USB device ----- */
	Drv_USBH_DevInit();
	/* ----- Display Webcam information ----- */
	if(g_stDev[0].type==UC_VIDEO)
	{
		ST_UVC_STREAM_INFO stream;
		unsigned i,j,k;
		DBG_PRINT("\r\nUSB Webcam Information Display\r\n");
		stream = (*(ST_UVC_INFO*)g_stDev[0].para).STREAM_INFO;
		DBG_PRINT("Format Type Number: %d\r\n",stream.FormatTypeNum);
		DBG_PRINT("Capture Mode: %d\r\n",stream.CaptureMode);
		for(i=0;i<stream.FormatTypeNum;i++)
		{
			char str[3][256] = {"Uncompressed YUV2","Uncompressed NV12","Motion-JPEG"};
			DBG_PRINT("\tFormat Index %d: %s Type with %d resolutions\r\n",stream.Format_INFO[i].FormatIndex,&str[stream.Format_INFO[i].FormatType-1][0],stream.Format_INFO[i].FrameTypeNum);	
			for(j=0;j<stream.Format_INFO[i].FrameTypeNum;j++)
			{
				ST_UVC_FRAME_INFO frame = stream.Format_INFO[i].FRAME_INFO[j];
				DBG_PRINT("\t\t %d X %d (%d), FPS:",frame.res.Width,frame.res.Height,frame.FrameIndex);
				if(frame.FrameInterType)
				{
					for(k=0;k<frame.FrameInterType;k++)
						DBG_PRINT("%3d, ",10000000/frame.FrameInter[k]);
				}
				else
				{
					DBG_PRINT(" min:%3d, max:%3d, step:%3d,",10000000/frame.MinFrameInter,10000000/frame.MaxFrameInter,10000000/frame.FrameStep);	
				}
				DBG_PRINT("\r\n");
			}
			DBG_PRINT("\tStill Image with %3d resolutions\r\n",stream.Format_INFO[i].STILL_INFO.IMG_NUM);
			for(j=0;j<stream.Format_INFO[i].STILL_INFO.IMG_NUM;j++)
			{
				ST_Resolution res = stream.Format_INFO[i].STILL_INFO.res[j];
				DBG_PRINT("\t\t %d X %d\r\n ",res.Width, res.Height);
			}
		}
	}
	g_pUVC_FRAME_STACK = gp_malloc_align(UVC_FRAME_TaskStackSize*sizeof(unsigned),4);
	/* ----- Create a USB video class task ----- */
	OSTaskCreate(usbh_uvc_frame_task_entry, (void *) 0, &g_pUVC_FRAME_STACK[UVC_FRAME_TaskStackSize - 1], 50);
	/* ----- For Video Class device ----- */
	OSTimeDly(10);
	if(g_stDev[0].type==UC_VIDEO)
	{
		ST_UVC_STREAM_INFO stream;
		ST_UVC_INIT_CTRL ctrl;
		ST_UVC_PREVIEW_CTRL prev;
		unsigned i;
		stream = (*(ST_UVC_INFO*)g_stDev[0].para).STREAM_INFO;
		/* ----- Select frame type as MJPG ----- */
		for(i=0;i<stream.FormatTypeNum;i++)
		{
			if(stream.Format_INFO[i].FormatType == UVC_DATA_MJPEG)
				break;	
		}
		if(i>=stream.FormatTypeNum)
		{
			DBG_PRINT("Do not support Motion JPG \r\n");
			goto out;
		}
		else
			prev.format_index = i;
		/* ---- Select resolution ----- */
		for(i=0;i<stream.Format_INFO[prev.format_index].FrameTypeNum;i++)
		{
			ST_Resolution res = stream.Format_INFO[prev.format_index].FRAME_INFO[i].res;
			if((res.Width == RES_WIDTH)&&(res.Height == RES_HIGH))
				break;
		}
		if(i>=stream.Format_INFO[prev.format_index].FrameTypeNum)
		{
			DBG_PRINT("Do not support Resolution w: %d, h: %d\r\n", RES_WIDTH, RES_HIGH);
			goto out;
		}
		else
		{
			prev.frame_index = i;
			ctrl.dev = 0;
			ctrl.ret = uvc_state;
			msgQSend(uvc_frame_msg_q_id,MSG_UVC_INIT,&ctrl,sizeof(ST_UVC_INIT_CTRL),MSG_PRI_NORMAL);
			msgQReceive(uvc_state, &msg_id, NULL, 0);
			for( i=0 ; i < BUF_NUM; i++)
			{
				buf[i] = gp_malloc_align( DISP_SIZE , 32);
				memset(buf[i], 0x00, DISP_SIZE);
			}
			
			tv_buf = gp_malloc_align(DISP_SIZE, 32);
			prev.buf = buf;
			prev.buf_num = BUF_NUM;
			prev.ln = DISP_SIZE;
			msgQSend(uvc_frame_msg_q_id, MSG_UVC_PREVIEW_EN, &prev, sizeof(prev), MSG_PRI_NORMAL);
			msgQReceive(uvc_state, &msg_id, NULL, 0);
		}
	}
	/* ----- Start preview -----*/
	for(i=0;i<50000;i++)
	{
		
		msgQReceive(uvc_state, &msg_id,NULL,0);
		if(msg_id==MSG_PREVIEW_FRAME_END)
		{
			mjpeg_info mjpg;
			mjpg.raw_data_addr = (unsigned int)buf[buf_cnt];
			mjpg.raw_data_size = DISP_SIZE;
			mjpg.output_addr = (unsigned int)tv_buf;
			mjpg.output_format = C_SCALER_CTRL_OUT_YUYV;
			/* ----- Decode JPG ----- */
			mjpeg_decode_without_scaler(&mjpg);
			/* ----- TV display -----*/
			video_codec_show_image(DISP_TYPE, (INT32U)tv_buf, DISP_TYPE, IMAGE_OUTPUT_FORMAT_YUYV);   
			buf_cnt = (buf_cnt+1)%BUF_NUM;
		}	
	}
	/* ----- Disable preview ----- */
	msgQSend(uvc_frame_msg_q_id,MSG_UVC_PREVIEW_DIS,0,0,MSG_PRI_NORMAL);
	OSTimeDly(300);
out:
	DBG_PRINT("\r\n Test Finish\r\n");
	while(1)
	{
		OSTimeDly(100);
	}
}