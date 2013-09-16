#include <stdio.h>
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "Face_Detection.h"
#include "video_codec_callback.h"

#define WORKING_WIDTH		320
#define WORKING_WIDTH_STEP	((WORKING_WIDTH+7)>>3<<3)
#define MAX_CONDIDATE		512

#define COLOR_CHANNEL		1

#define N_FACE_0			20

#define RETURN(x) {ret = x; goto Return;}

#define WORKING_ANGLE		25//20
#define COSV				59396//61584
#define SINV				27697//22415
#define MALLOC	            gp_malloc
#define FREE	            gp_free

#define FaceTaskStackSize         4096
INT32U FaceTaskStack[FaceTaskStackSize];
OS_EVENT *face_frame_q;  
OS_EVENT *face_detect_end_q;
#define C_FACE_QUEUE_MAX			5
void *face_frame_q_stack[C_FACE_QUEUE_MAX];
void *face_detect_end_q_stack[C_FACE_QUEUE_MAX];

#define MAX_RESULT	N_FACE_0//64

gpImage imgIn;
gpRect Face[MAX_RESULT];
int Count[MAX_RESULT];
INT32U Face_Detect_Demo_En=0,drawFace_flag=0,train_counter=0,face_verify_flag=0;
INT32U face_count;
INT32U *fiMem;
INT32U *ownerULBP;
gpRect Face_old[MAX_RESULT];	
int Count_old[MAX_RESULT];

static INT32S scaler_image_start_once(gpImage *src, gpImage *dst)
{	
	extern INT32S scaler_once(INT8U wait_done, gpImage *src, gpImage *dst);
	return scaler_once(1, src, dst);
}

static INT32S scaler_image_start(gpImage *src, gpImage *dst)
{	
	extern INT32S scaler_once(INT8U wait_done, gpImage *src, gpImage *dst);
	return scaler_once(0, src, dst);
}

static INT32S scaler_image_clip(gpImage *srcImg, gpImage *dstImg, gpRect *clip)
{
	extern INT32S scaler_clip(INT8U wait_done, gpImage *src, gpImage *dst, gpRect *clip);
	
	return scaler_clip(0, srcImg, dstImg, clip);
}

static INT32S scaler_image_wait_done(void)
{
	extern INT32S scaler_wait_done(void);
	return scaler_wait_done();
}

void image_color_set(gpImage *img,IMAGE_COLOR_FORMAT type)
{    
		switch(type)
		{
			case IMAGE_COLOR_FORMAT_YUYV:
			     img->format = IMG_FMT_UYVY;
			     break;
			     
			case IMAGE_COLOR_FORMAT_UYVY:
			     img->format = IMG_FMT_YUYV;
			     break;             
     
			case IMAGE_COLOR_FORMAT_Y_ONLY:
		         img->format = IMG_FMT_GRAY;
			     break;     

			default :
			     img->format = IMG_FMT_UYVY;
			     break;
		}
}

int faceRoiDetect(gpImage* gray, gpRect* detRect,int *Count)
{		
    void *WorkMem = 0;
	int ret;
	int t;
	gpImage imgWork = {WORKING_WIDTH, 0, WORKING_WIDTH_STEP, 1, IMG_FMT_GRAY, 0};
	gpRect FullRect;
	long dst2src[2];
	int N, i;
	gpRect *Face0 = 0;
	int *Count0 = 0;

	int scale, min_width, max_wnd;
	
	imgWork.height = WORKING_WIDTH * gray->height / gray->width;

	FullRect.x		= 0;
	FullRect.y		= 0;
	FullRect.width	= imgWork.width;
	FullRect.height	= imgWork.height;

	// WorkMem
	t = FaceDetect_Config(0, 0, imgWork.width, imgWork.height, COLOR_CHANNEL, MAX_CONDIDATE,2,2);
	WorkMem = MALLOC(t);
	if(WorkMem==0) RETURN(-1);
	FaceDetect_Config(WorkMem, t, imgWork.width, imgWork.height, COLOR_CHANNEL, MAX_CONDIDATE,2,2);

	FaceDetect_set_ScalerFn(WorkMem, scaler_image_start, scaler_image_wait_done, scaler_image_clip); 
	FaceDetect_set_angle(WorkMem, 0);
	FaceDetect_set_detect_obj(WorkMem, CASCADE_FACE);

	// imgWork
	imgWork.ptr = (unsigned char*)MALLOC(WORKING_WIDTH_STEP * imgWork.height);
	if(imgWork.ptr==0) RETURN(-1);

	Face0 = (gpRect*)MALLOC(N_FACE_0 * sizeof(gpRect));
	if(Face0==0) RETURN(-1);
	// Count0
	Count0  = (int*)MALLOC(N_FACE_0 * sizeof(int));
	if(Count0==0) RETURN(-1);

	dst2src[1] = dst2src[0] = (long)gray->width   * 65536 / imgWork.width;

	scaler_image_start_once(gray, &imgWork);
	
	min_width = 30;
	max_wnd = imgWork.height < imgWork.width ? imgWork.height : imgWork.width;
	scale = (int)(1.1*65536+0.5);
	FaceDetect_SetScale(WorkMem, scale, min_width, max_wnd);

	N = FaceDetect(WorkMem, &imgWork, &FullRect, N_FACE_0, Face0, Count0);

	for(i=0;i<N;i++)
	{
		Count[i] = Count0[i];

		Face[i].x		= (short)(Face0[i].x		* dst2src[0] >> 16);
		Face[i].y		= (short)(Face0[i].y		* dst2src[1] >> 16);
		Face[i].width	= (short)(Face0[i].width	* dst2src[0] >> 16);
		Face[i].height	= (short)(Face0[i].height	* dst2src[1] >> 16);
	}

	if(Count0) FREE(Count0);
	Count0 = 0;
	if(Face0) FREE(Face0);
	Face0 = 0;
	if(imgWork.ptr) FREE(imgWork.ptr);
	imgWork.ptr = 0;
	if(WorkMem) FREE(WorkMem);
	WorkMem = 0;

	ret = N;
Return:
	return ret;	
}

void DrawRect(gpImage *Image, gpRect *Rect, const unsigned char _color[4])
{
	int i;
	unsigned char *dst1;
	unsigned char *dst2;
	unsigned char *dst11;
	unsigned char *dst21;
	unsigned char *dst12;
	unsigned char *dst22;		
	unsigned char color[4];
	
	if(_color)
	{
		color[0] = _color[0];	color[1] = _color[1];
		color[2] = _color[2];	color[3] = _color[3];
	}
	else
	{
		color[0] = 255;	color[1] = 255;
		color[2] = 255;	color[3] = 255;
	}

	if(Image->format==IMG_FMT_GRAY)
	{
		dst1 = Image->ptr + Rect->x + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x + (Rect->y + Rect->height - 1)* Image->widthStep;
		for(i=0;i<Rect->width;i++)
		{
			*dst1++ = color[0];
			*dst2++ = color[0];
		}
		dst1 = Image->ptr + Rect->x + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x + Rect->width - 1 + Rect->y * Image->widthStep;
		for(i=0;i<Rect->height;i++)
		{
			*dst1 = color[0];
			*dst2 = color[0];
			dst1 += Image->widthStep;
			dst2 += Image->widthStep;
		}
	}
    else if(Image->format==IMG_FMT_YUYV || Image->format==IMG_FMT_YCbYCr || Image->format==IMG_FMT_UYVY || Image->format==IMG_FMT_CbYCrY)
	{
		dst1 = Image->ptr + Rect->x * 2 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x * 2 + (Rect->y + Rect->height - 1)* Image->widthStep;
		dst11 = Image->ptr + (Rect->x) * 2 + (Rect->y-1) * Image->widthStep;
		dst21 = Image->ptr + (Rect->x) * 2 + ((Rect->y+1) + Rect->height - 1)* Image->widthStep;
		dst12 = Image->ptr + (Rect->x) * 2 + (Rect->y-2) * Image->widthStep;
		dst22 = Image->ptr + (Rect->x) * 2 + ((Rect->y+2) + Rect->height - 1)* Image->widthStep;		
		for(i=0;i<Rect->width;i++)
		{
			*dst1++ = color[0];		*dst1++ = color[1];
			*dst2++ = color[0];		*dst2++ = color[1];
			*dst11++ = color[0];	*dst11++ = color[1];
			*dst21++ = color[0];	*dst21++ = color[1];
			*dst12++ = color[0];	*dst12++ = color[1];
			*dst22++ = color[0];	*dst22++ = color[1];			
		}
		dst1 = Image->ptr + Rect->x * 2 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + (Rect->x + Rect->width - 1) * 2 + Rect->y * Image->widthStep;
		dst11 = Image->ptr + (Rect->x-1) * 2 + (Rect->y) * Image->widthStep;
		dst21 = Image->ptr + ((Rect->x+1) + Rect->width - 1) * 2 + (Rect->y) * Image->widthStep;
		dst12 = Image->ptr + (Rect->x-2) * 2 + (Rect->y) * Image->widthStep;
		dst22 = Image->ptr + ((Rect->x+2) + Rect->width - 1) * 2 + (Rect->y) * Image->widthStep;		
		for(i=0;i<Rect->height;i++)
		{
			dst1[0] = color[0];		dst1[1] = color[1];
			dst2[0] = color[0];		dst2[1] = color[1];
			dst11[0] = color[0];	dst11[1] = color[1];
			dst21[0] = color[0];	dst21[1] = color[1];
			dst12[0] = color[0];	dst12[1] = color[1];
			dst22[0] = color[0];	dst22[1] = color[1];			
			dst1 += Image->widthStep;
			dst2 += Image->widthStep;
			dst11 += Image->widthStep;
			dst21 += Image->widthStep;
			dst12 += Image->widthStep;
			dst22 += Image->widthStep;			
		}
	}
	else if(Image->format==IMG_FMT_RGB)
	{
		dst1 = Image->ptr + Rect->x * 3 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x * 3 + (Rect->y + Rect->height - 1)* Image->widthStep;
		for(i=0;i<Rect->width;i++)
		{
			*dst1++ = color[0];		*dst1++ = color[1];		*dst1++ = color[2];
			*dst2++ = color[0];		*dst2++ = color[1];		*dst2++ = color[2];
		}
		dst1 = Image->ptr + Rect->x * 3 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + (Rect->x + Rect->width - 1) * 3 + Rect->y * Image->widthStep;
		for(i=0;i<Rect->height;i++)
		{
			dst1[0] = color[0];		dst1[1] = color[1];		dst1[2] = color[2];
			dst2[0] = color[0];		dst2[1] = color[1];		dst2[2] = color[2];
			dst1 += Image->widthStep;
			dst2 += Image->widthStep;
		}
	}
}

// Draw Rectangular on Face
void drawFace(gpImage *Image, int N, gpRect *Face, int *Count)
{
	int i;

	for(i=0;i<N;i++)
	{
		gpRect Rect;
		
		Rect.x	= Face[i].x;
		Rect.y	= Face[i].y;
		Rect.width	= Face[i].width;
		Rect.height	= Face[i].height;

		if(Count[i]>5)
		{		
			if(Rect.x >= 0 && Rect.y >= 0 && Rect.x + Rect.width <= Image->width && Rect.y + Rect.height <= Image->height)
			{
                //comment out CPU draw face
				DrawRect(Image, &Rect, 0);
			}
		}

	}
}

void pic_face(INT32U frame_buffer)
{
	gpImage imgIn_old;
	
	imgIn_old.width = C_DISPLAY_DEV_HPIXEL;
	imgIn_old.height = C_DISPLAY_DEV_VPIXEL;
	imgIn_old.widthStep = C_DISPLAY_DEV_HPIXEL*2;
	imgIn_old.ch = 2;
	image_color_set((gpImage *)&imgIn_old,IMAGE_COLOR_FORMAT_YUYV);
	imgIn_old.ptr = (unsigned char*)frame_buffer; 
	drawFace(&imgIn_old,face_count, Face_old,Count_old);  
}

INT32U face_verify_set(INT32U frame_buffer)
{	
		INT32S nRet;
		INT32U i;
		
		imgIn.width = C_DISPLAY_DEV_HPIXEL;
		imgIn.height = C_DISPLAY_DEV_VPIXEL;
		imgIn.widthStep = C_DISPLAY_DEV_HPIXEL*2;
		imgIn.ch = 2;
		image_color_set((gpImage *)&imgIn,IMAGE_COLOR_FORMAT_YUYV);
		imgIn.ptr = (unsigned char*)frame_buffer;
							
		nRet = faceRoiDetect(&imgIn,Face,Count);
		if(nRet)
		{
			drawFace_flag=1;	
			face_count=nRet;	
			for(i=0;i<face_count;i++)
			  Count_old[i]=Count[i];

			for(i=0;i<face_count;i++)
			{
			  Face_old[i].x=Face[i].x;
			  Face_old[i].y=Face[i].y;
			  Face_old[i].width=Face[i].width;
			  Face_old[i].height=Face[i].height;			          
			}		   	
		}
		else
		{
		   if(drawFace_flag++ > 2)
		      drawFace_flag=0;
		   DBG_PRINT("/**********No Face Founded**********/ : %d \r\n",nRet);
		}
		
		return 0;   
}

void Face_task_entry(void *para)
{
    INT8U err;
	INT32U msg_id;
	
	face_frame_q = OSQCreate(face_frame_q_stack, C_FACE_QUEUE_MAX);
	if(!face_frame_q)
	   DBG_PRINT("/**********face_frame_q fail**********/ \r\n");
	
	face_detect_end_q = OSQCreate(face_detect_end_q_stack, C_FACE_QUEUE_MAX);
	if(!face_detect_end_q)    
       DBG_PRINT("/**********face_frame_q fail**********/ \r\n");
        
    while(1)
    {
        msg_id = (INT32U) OSQPend(face_frame_q, 0, &err);
		if((err != OS_NO_ERR)||	!msg_id)
			continue;   
        face_verify_set(msg_id);
    	//post ready frame to video encode task
	    OSQPost(face_detect_end_q, (void *)msg_id);
    }
}

void face_frame_set(INT32U frame)
{
    //post ready frame to face detect task
	OSQPost(face_frame_q, (void *)frame);
}

INT32U face_detect_return(void)
{
    INT8U err;
	INT32U msg_id;
    
    //post ready frame to video encode task
    msg_id = (INT32U) OSQAccept(face_detect_end_q,&err);
	if((err != OS_NO_ERR)||	!msg_id)
	   return 0;
	return msg_id; 	   
}

void Face_Detect_demo(void)
{
	VIDEO_ARGUMENT arg;
    
	// Create a user-defined task
	OSTaskCreate(Face_task_entry, (void *) 0, &FaceTaskStack[FaceTaskStackSize - 1], 30); 

	// Initialize display device
	#if C_DISPLAY_DEVICE >= C_TV_QVGA
		tv_init();	
		#if C_DISPLAY_DEVICE == C_TV_QVGA
			tv_start (TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_NON_INTERLACE);	
		#elif C_DISPLAY_DEVICE == C_TV_VGA
			tv_start (TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
		#elif C_DISPLAY_DEVICE == C_TV_D1
			tv_start (TVSTD_NTSC_J, TV_D1, TV_NON_INTERLACE);
		#else
			while(1);
		#endif
	#else
		tft_init();
		tft_start(C_DISPLAY_DEVICE);
	#endif

	user_defined_video_codec_entrance();
	video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder
	
	arg.bScaler = 1;	// must be 1
	arg.TargetWidth = C_DISPLAY_DEV_HPIXEL; 	//encode width
	arg.TargetHeight = C_DISPLAY_DEV_VPIXEL;	//encode height
	arg.SensorWidth	= C_DISPLAY_DEV_HPIXEL;	//sensor input width
	arg.SensorHeight = C_DISPLAY_DEV_VPIXEL;	//sensor input height	
	arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;	//display width
	arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
	arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
	arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;//display buffer height
	arg.VidFrameRate = 30;	//video encode frame rate
	arg.AudSampleRate = 8000;//audio record sample rate
	arg.OutputFormat = DISPLAY_OUTPUT_FORMAT; //display output format 
	video_encode_preview_start(arg);
    Face_Detect_Demo_En = 1;
    
    while(1)		
	   OSTimeDly(1);     
}