/*
 ******************************************************************************
 * fd.c
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.09.18	T.C.		modify for gpl32 v1.0.5 for ZT patch v0.1
 *
 ******************************************************************************
 */
#include <stdio.h>

#include "Face_Detection.h"
#include "FaceIdentifyAP.h"
#include "video_codec_callback.h"

#include "drv_l2_ad_key_scan.h"

#include "fd.h"
#include "frdbm.h"
#include "serial.h"
//#include "frio.h"
//#include "key.h"


/* for security level */
static unsigned long	securityLevel;


/* for face detection */
#define WORKING_WIDTH		320
#define WORKING_WIDTH_STEP	((WORKING_WIDTH + 7) >> 3 << 3)
#define MAX_CONDIDATE		512
#define COLOR_CHANNEL		1
#define N_FACE_0		20
#define MAX_RESULT		N_FACE_0//64
#define RETURN(x)		{ret = x; goto Return;}
#define MALLOC			gp_malloc
#define FREE			gp_free

static gpRect			Face[MAX_RESULT];
static gpRect			Face_old[MAX_RESULT];
static int			Count[MAX_RESULT];
static int			Count_old[MAX_RESULT];


/* for face detection task */
#define STKSZ_FDTASK		4096
static INT32U			fdtask_stack[STKSZ_FDTASK];

#define STKSZ_FACE_QUEUE	5
static OS_EVENT			*face_frame_q;
static OS_EVENT			*face_detect_end_q;
static void			*face_frame_q_stack[STKSZ_FACE_QUEUE];
static void			*face_detect_end_q_stack[STKSZ_FACE_QUEUE];

static gpImage			imgIn;
static INT32U			face_count;
static unsigned long		fdmode;
static unsigned long		train_count;
static unsigned long		ident_count;

INT32U Face_Detect_Demo_En=0, drawFace_flag=0;

INT32U *fiMem;
INT32U *ownerULBP;

#define SZ_ULBP			1888


/*
 *****************************************************************************
 *
 * for debug
 *
 *****************************************************************************
 */
#define TRACE_NONE		0x00000000UL
#define TRACE_MSG		0x00000001UL
#define TRACE_DETECT		0x00000002UL
#define TRACE_SECURITY		0x00000004UL

#define DRDBM_DEBUG		( \
	TRACE_MSG		| \
	TRACE_DETECT		| \
	TRACE_SECURITY		| \
	TRACE_NONE)

#if (DRDBM_DEBUG & TRACE_MSG)
    #define _MSG(x)		(x)
#else
    #define _MSG(x)
#endif
#if (DRDBM_DEBUG & TRACE_DETECT)
    #define _FD(x)		(x)
#else
    #define _FD(x)
#endif
#if (DRDBM_DEBUG & TRACE_SECURITY)
    #define _SECURITY(x)	(x)
#else
    #define _SECURITY(x)
#endif


/*
 *****************************************************************************
 *
 * face detection
 *
 *****************************************************************************
 */
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


/*
 *****************************************************************************
 *
 * security level
 *
 *****************************************************************************
 */
int securitylvl_get(void)
{
	unsigned long	temp;

	switch (securityLevel) {
	case 1:		temp = 54614;	break;
	case 2:		temp = 81920;	break;
	case 4:		temp = 98304;	break;
	case 5:		temp = 147456;	break;
	default:	temp = 90112;	break;
	}

	_SECURITY(DBG_PRINT("(security) level: %d (easy->hard = 1->5), value: %d\r\n", securityLevel, temp));
	return temp;
}

int securitylvl_set(int set_value)
{
	if (set_value) {
		securityLevel++;
		if (securityLevel > 5) {
			securityLevel = 5;
		}
	} else {
		securityLevel--;
		if (securityLevel < 1) {
			securityLevel = 1;
		}
	}
	return securitylvl_get();
}

int securitylvl_init(int level)
{
	if (level <= 5 && level > 0) {
		securityLevel = level;
	} else {
		securityLevel = 3;
	}
	return securitylvl_get();
}


/*
 *****************************************************************************
 *
 * face detection task
 *
 *****************************************************************************
 */
static INT32U face_detection(INT32U frame_buffer)
{
	INT32S	nRet;
	INT32U	i;

	imgIn.width	= C_DISPLAY_DEV_HPIXEL;
	imgIn.height	= C_DISPLAY_DEV_VPIXEL;
	imgIn.widthStep	= C_DISPLAY_DEV_HPIXEL * 2;
	imgIn.ch	= 2;
	image_color_set((gpImage *) &imgIn, IMAGE_COLOR_FORMAT_YUYV);
	imgIn.ptr	= (unsigned char *) frame_buffer;

	nRet = faceRoiDetect(&imgIn, Face, Count);
	if (nRet) {
		/* draw rectangular on face */
		drawFace_flag = 1;
		face_count    = nRet;
		for (i=0; i<face_count; i++) {
			Count_old[i] = Count[i];
		}
		for (i=0; i<face_count; i++) {
			  Face_old[i].x      = Face[i].x;
			  Face_old[i].y      = Face[i].y;
			  Face_old[i].width  = Face[i].width;
			  Face_old[i].height = Face[i].height;
		}

		switch (fdmode) {
		/* recognition */
		case 1:
			nRet = FaceIdentify_Verify(&imgIn, &Face[0], ownerULBP, securitylvl_get(), fiMem);
			if (nRet) {
				if (ident_count++ > 1) {
					fdmode    = 0;
					ownerULBP = (INT32U *) frdb_get_c_ident();
					_FD(DBG_PRINT("(Identify) OK...\r\n"));
					_FD(DBG_PRINT("\033[1;36m(Identify)\033[0m End\r\n"));
				}
			} else {
				ident_count = 0;
				_FD(DBG_PRINT("(Identify) Fail...\r\n"));
			}
			break;

		/* training */
		case 2:
			FaceIdentify_Train(&imgIn, &Face[0], ownerULBP, train_count, fiMem);
			_FD(DBG_PRINT("(Training) Value : %d OK \r\n", train_count));
			train_count++;
			if (train_count >= 20) {
				fdmode = 0;
				frdb_store(frdb_get_curr());
				_FD(DBG_PRINT("(Training) frdb store\r\n"));
				ownerULBP = (INT32U *) frdb_get_c_train();
				_FD(DBG_PRINT("\033[1;36m(Training)\033[0m Finish\r\n"));
			}
			break;

		/* nothing */
		default:
			break;
		}

	} else {
		if (drawFace_flag++ > 2) {
			drawFace_flag = 0;
		}
//		_FD(DBG_PRINT("No Face Founded : %d \r\n", nRet));
	}
	return 0;
}

void face_frame_set(INT32U frame)
{
	//post ready frame to face detect task
	OSQPost(face_frame_q, (void *) frame);
}

INT32U face_detect_return(void)
{
	INT8U	err;
	INT32U	msg_id;

	// post ready frame to video encode task
	msg_id = (INT32U) OSQAccept(face_detect_end_q, &err);
	if ((err != OS_NO_ERR) || !msg_id) {
		return 0;
	}
	return msg_id;
}

static void fd_task(void *para)
{
	INT8U	err;
	INT32U	msg_id;

	face_frame_q = OSQCreate(face_frame_q_stack, STKSZ_FACE_QUEUE);
	if (!face_frame_q) {
		DBG_PRINT("face_frame_q fail\r\n");
	}

	face_detect_end_q = OSQCreate(face_detect_end_q_stack, STKSZ_FACE_QUEUE);
	if (!face_detect_end_q) {
		DBG_PRINT("face_detect_end_q fail\r\n");
	}

	while (1) {
		msg_id = (INT32U) OSQPend(face_frame_q, 0, &err);
		if ((err != OS_NO_ERR) || !msg_id) {
			continue;
		}

		face_detection(msg_id);

		// post ready frame to video encode task
		OSQPost(face_detect_end_q, (void *) msg_id);
	}
}


/*
 *****************************************************************************
 *
 * face detection mainloop
 *
 *****************************************************************************
 */
void fd(unsigned char prio)
{
	VIDEO_ARGUMENT	arg;
    	int		memSize;

	int		c;
	unsigned char	s;
	int		adkey1 = 0;
	int		adkey2 = 0;
	int		adkey3 = 0;
	int		adkey4 = 0;
	int		adkey5 = 0;


	// Create a user-defined task
	OSTaskCreate(fd_task, (void *) 0, &fdtask_stack[STKSZ_FDTASK - 1], prio);

	// Initialize display device
#if C_DISPLAY_DEVICE >= C_TV_QVGA
	tv_init();
	#if C_DISPLAY_DEVICE == C_TV_QVGA
		tv_start(TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_NON_INTERLACE);
	#elif C_DISPLAY_DEVICE == C_TV_VGA
		tv_start(TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
	#elif C_DISPLAY_DEVICE == C_TV_D1
		tv_start(TVSTD_NTSC_J, TV_D1, TV_NON_INTERLACE);
	#else
		while (1);
	#endif
#else
	tft_init();
	tft_start(C_DISPLAY_DEVICE);
#endif

	user_defined_video_codec_entrance();
	video_encode_entrance();				// initialize and allocate the resources needed by Video decoder

	arg.bScaler		= 1;				// must be 1
	arg.TargetWidth		= C_DISPLAY_DEV_HPIXEL;		// encode width
	arg.TargetHeight	= C_DISPLAY_DEV_VPIXEL;		// encode height
	arg.SensorWidth		= C_DISPLAY_DEV_HPIXEL;		// sensor input width
	arg.SensorHeight	= C_DISPLAY_DEV_VPIXEL;		// sensor input height
	arg.DisplayWidth	= C_DISPLAY_DEV_HPIXEL;		// display width
	arg.DisplayHeight	= C_DISPLAY_DEV_VPIXEL;		// display height
	arg.DisplayBufferWidth	= C_DISPLAY_DEV_HPIXEL;		// display buffer width
	arg.DisplayBufferHeight	= C_DISPLAY_DEV_VPIXEL;		// display buffer height
	arg.VidFrameRate	= 30;				// video encode frame rate
	arg.AudSampleRate	= 8000;				// audio record sample rate
	arg.OutputFormat	= DISPLAY_OUTPUT_FORMAT;	// display output format
	video_encode_preview_start(arg);

	Face_Detect_Demo_En = 1;

	frdb_init(3*1024*1024);

	memSize	  = FaceIdentify_MemCalc();
	fiMem	  = (INT32U *) gp_malloc_align((memSize), 8);
//	ownerULBP = (INT32U *) gp_malloc_align((SZ_ULBP*20), 8);
	ownerULBP = (INT32U *) frdb_get_valid();
	securitylvl_init(3);
	adc_key_scan_init();					// init key scan


	while (1) {
		adc_key_scan();

		/* training */
		if (ADKEY_IO1) {
			if (!adkey1) {
				adkey1++;
				train_count = 0;
				if (fdmode == 2) {
					fdmode = 0;
					_FD(DBG_PRINT("\033[1;36mCTraining)\033[0m Stop\r\n"));
				} else {
					fdmode = 2;
//					gp_memset((INT8S *) ownerULBP, 0, (SZ_ULBP*20));
					_FD(DBG_PRINT("\033[1;36m(Training)\033[0m Start\r\n"));

					frdb_set_c_train(-1);
					ownerULBP = (INT32U *) frdb_get_c_train();
				}
			}
		} else {
			adkey1 = 0;
		}

		/* recognition */
		if (ADKEY_IO2) {
			if (!adkey2) {
				adkey2++;
				ident_count = 0;
				if (fdmode == 1) {
					fdmode = 0;
					_FD(DBG_PRINT("\033[1;36m(Identify)\033[0m Stop\r\n"));
				} else {
					fdmode = 1;
					_FD(DBG_PRINT("\033[1;36m(Identify)\033[0m Start\r\n"));

					frdb_set_c_ident(-1);
					ownerULBP = (INT32U *) frdb_get_c_ident();
				}
			}
		} else {
			adkey2 = 0;
		}

		/* security level dec. */
		if (ADKEY_IO3) {
			if (!adkey3) {
				adkey3++;
				securitylvl_set(0);
			}
		} else {
			adkey3 = 0;
		}

		/* security level inc. */
		if (ADKEY_IO4) {
			if (!adkey4) {
				adkey4++;
				securitylvl_set(1);
			}
		} else {
			adkey4 = 0;
		}

		/* erase */
		if (ADKEY_IO5) {
			if (!adkey5) {
				adkey5++;
				c = 0xFF;
				s = (unsigned char) (c & ~CMD_MASK);
				for (c=0; c<FRDB_NUM; c++) {
					if (s & 0x08) {
						frdb_erase(c);
					}
					s >>= 1;
				}
			}
		} else {
			adkey5 = 0;
		}
	}
}

