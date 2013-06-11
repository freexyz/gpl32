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
 *	2013.05.23	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include <stdio.h>

#include "video_codec_callback.h"
#include "FaceDetectKernel.h"
#include "FaceDetectAP.h"
#include "FaceIdentifyAP.h"

#include "fd.h"
#include "frdbm.h"
#include "frio.h"
#include "key.h"
#include "serial.h"

extern int	Scaler_Start(gpImage *src, gpImage *dst);
extern int	Scaler_clip(gpImage *srcImg, gpImage *dstImg, gpRect *clip);
extern int	Scaler_wait_end(void);


#define FACE_IDENTIFY_EN	1

#define WORKING_WIDTH		320
#define WORKING_WIDTH_STEP	((WORKING_WIDTH+7)>>3<<3)
#define MAX_CONDIDATE		512

#define COLOR_CHANNEL		1

#define N_FACE_0		20

#define RETURN(x)		{ret = x; goto Return;}

#define WORKING_ANGLE		25//20
#define COSV			59396//61584
#define SINV			27697//22415
#define MALLOC			gp_malloc
#define FREE			gp_free


#define MAX_RESULT		N_FACE_0//64
gpImage imgIn;
gpRect Face[MAX_RESULT];
gpRect Face_old[MAX_RESULT];
int Count[MAX_RESULT];
int Count_old[MAX_RESULT];

INT32U Face_Detect_Demo_En=0,drawFace_flag=0,train_counter=0,face_verify_flag=0;
INT32U securityLevel,sensor_frame,face_count;
INT32U verify_pass, verify_fail;
INT32U *fiMem;
INT32U *ownerULBP;
unsigned long		fdmode;

#define FaceTaskStackSize	4096
#define C_FACE_QUEUE_MAX	5
INT32U		FaceTaskStack[FaceTaskStackSize];
OS_EVENT	*face_frame_q;
OS_EVENT	*face_detect_end_q;
void		*face_frame_q_stack[C_FACE_QUEUE_MAX];
void		*face_detect_end_q_stack[C_FACE_QUEUE_MAX];

#define STKSZ_FTRAIN	4096
#define QUEUE_FTRAIN	5
static INT32U		ftrain_stack[STKSZ_FTRAIN];
static void		*ft_start_q_stack[QUEUE_FTRAIN];
static void		*ft_end_q_stack[QUEUE_FTRAIN];
static OS_EVENT		*ft_start_q;
static OS_EVENT		*ft_end_q;


/*
 *****************************************************************************
 *
 * for debug
 *
 *****************************************************************************
 */
#define TRACE_NONE		0x00000000UL
#define TRACE_MSG		0x00000001UL
#define TRACE_TRAIN		0x00000002UL
#define TRACE_IDENT		0x00000004UL
#define TRACE_SECURITY		0x00000008UL

#define DRDBM_DEBUG		( \
	TRACE_MSG		| \
	TRACE_TRAIN		| \
	TRACE_IDENT		| \
	TRACE_SECURITY		| \
	TRACE_NONE)

#if (DRDBM_DEBUG & TRACE_MSG)
    #define _MSG(x)		(x)
#else
    #define _MSG(x)
#endif
#if (DRDBM_DEBUG & TRACE_TRAIN)
    #define _TRAIN(x)		(x)
#else
    #define _TRAIN(x)
#endif

#if (DRDBM_DEBUG & TRACE_IDENT)
    #define _IDENT(x)		(x)
#else
    #define _IDENT(x)
#endif
#if (DRDBM_DEBUG & TRACE_SECURITY)
    #define _SECURITY(x)	(x)
#else
    #define _SECURITY(x)
#endif


/*
 *****************************************************************************
 *
 * face detect
 *
 *****************************************************************************
 */
gpRect GPRect_utils(int _x, int _y, int _width, int _height)
{
	gpRect rect;

	rect.x = _x;
	rect.y = _y;
	rect.width = _width;
	rect.height = _height;
	return rect;
}

int faceRoiDetect(gpImage* gray, gpRect* detRect,int *Count)
{
#if FACE_IDENTIFY_EN == 1
	gpRect* faceROI = &detRect[0];
	gpRect* lEyeROI = &detRect[1];
	gpRect* rEyeROI = &detRect[2];

	int HEIGHT = gray->height;
	int WIDTH = gray->width;

	gpRect faceResult[MAX_RESULT];
	gpRect rEyeResult[MAX_RESULT];
	gpRect lEyeResult[MAX_RESULT];
	int faceCount[MAX_RESULT];
	int rEyeCount[MAX_RESULT];
	int lEyeCount[MAX_RESULT];

	// Initialization //
	int ret, faceN, rEyeN, lEyeN, t, int_scale_face, int_scale_eye;
	int xstep_face = 3;
	int xstep_eye = 2;
	int min_face_nbr_h = 5;
	int min_face_nbr_l = 3;
	int min_eye_nbr = 1;
	//int flag_console = 2;
	int min_face_wnd = 50;
	int min_eye_wnd = 20; // 20 is minimum
	int max_wnd = MIN(gray->width, gray->height);
	int maxFaceW = 0;
	int maxFaceCount = min_face_nbr_l;
	int best_face = 0;
	void *WorkMem = 0;
	gpRect Rect;
	int i;
	int offset_x,offset_y;
	gpRect rFace;
	gpRect lFace;

	int_scale_face = 75366; // 1.15
	int_scale_eye = 72089; // 1.1, 65535 = 1

	Rect.x = 0;
	Rect.y = 0;
	Rect.width = gray->width;
	Rect.height = gray->height;

	t = MIN(Rect.width, Rect.height);
	if(max_wnd>t) max_wnd = t;

	t = FaceDetect_Config(0, 0, WIDTH, HEIGHT, 1, MAX_RESULT, 3, 2);

	WorkMem = MALLOC(t);

	//--------------------
	//	Face Detection
	//--------------------
	ret = FaceDetect_Config(WorkMem, t, Rect.width, Rect.height, 1, MAX_RESULT, xstep_face, 2);

	/* setting cascade type (face) */
	FaceDetect_set_detect_obj(WorkMem, CASCADE_FACE);

	FaceDetect_set_ScalerFn(WorkMem, Scaler_Start, Scaler_wait_end, Scaler_clip);
	ret = FaceDetect_SetScale(WorkMem, int_scale_face, min_face_wnd, max_wnd);

	faceN = FaceDetect(WorkMem, gray, &Rect, MAX_RESULT, faceResult, faceCount);

	if (!faceN)
	{
		// release memory //
		if(WorkMem)
		{
			FREE(WorkMem);
			WorkMem = 0;
		}
		return 0;
	}

	i = faceN-1;
	do
	{
		if (faceCount[i] >= min_face_nbr_l)
		{
			if ((maxFaceCount >= min_face_nbr_h) && (faceCount[i] >= min_face_nbr_h))
			{
				if (faceResult[i].width > maxFaceW)
				{
					maxFaceW = faceResult[i].width;
					maxFaceCount = faceCount[i];
					best_face = i;
				}
			}
			else
			{
				if (faceCount[i] >= maxFaceCount)
				{
					maxFaceW = faceResult[i].width;
					maxFaceCount = faceCount[i];
					best_face = i;
				}
			}
		}
	} while (i--);

	if (!maxFaceW)
	{
		// release memory //
		if(WorkMem)
		{
			FREE(WorkMem);
			WorkMem = 0;
		}
		return 0;
	}

	/* Face Position Determination */
	offset_x = faceResult[best_face].width/7;
	offset_y = faceResult[best_face].height/7;
	faceROI->x = faceResult[best_face].x + offset_x;
	faceROI->y = faceResult[best_face].y + offset_y;
	faceROI->width = (short)(faceResult[best_face].width*0.68);
	faceROI->height = faceResult[best_face].height - ((offset_y*49152)>>15);

	//--------------------
	//	Eyes Detection
	//--------------------
	ret = FaceDetect_Config(WorkMem, t, (faceResult[best_face].width>>1), faceResult[best_face].height, 1, MAX_RESULT, xstep_eye, 2);

	// setting cascade type (right eye) //
	FaceDetect_set_detect_obj(WorkMem, CASCADE_REYE);

	FaceDetect_set_ScalerFn(WorkMem, Scaler_Start, Scaler_wait_end, Scaler_clip);
	ret = FaceDetect_SetScale(WorkMem, int_scale_eye, min_eye_wnd, max_wnd);

	rFace = GPRect_utils(faceResult[best_face].x + (faceResult[best_face].width>>1), faceResult[best_face].y, (faceResult[best_face].width>>1), faceResult[best_face].height);
	rEyeN = FaceDetect(WorkMem, gray, &rFace, MAX_RESULT, rEyeResult, rEyeCount);

	// setting cascade type (left eye) //
	FaceDetect_set_detect_obj(WorkMem, CASCADE_LEYE);

	FaceDetect_set_ScalerFn(WorkMem, Scaler_Start, Scaler_wait_end, Scaler_clip);
	ret = FaceDetect_SetScale(WorkMem, int_scale_eye, min_eye_wnd, max_wnd);

	lFace = GPRect_utils(faceResult[best_face].x, faceResult[best_face].y, (faceResult[best_face].width>>1), faceResult[best_face].height);
	lEyeN = FaceDetect(WorkMem, gray, &lFace, MAX_RESULT, lEyeResult, lEyeCount);

	// release memory //
	if(WorkMem)
	{
		FREE(WorkMem);
		WorkMem = 0;
	}

	if (!rEyeN)
		rEyeROI->width = 0;
	else
	{
		int maxEyeCount = min_eye_nbr;
		int most_possible_eye = 0;

		int i = rEyeN - 1;
		do
		{
			if (rEyeCount[i] > maxEyeCount)
			{
				maxEyeCount = rEyeCount[i];
				most_possible_eye = i;
			}
		} while (i--);

		if (maxEyeCount == min_eye_nbr)
		{
			rEyeROI->width = 0;
		}
		else
		{
			i = most_possible_eye;

			// Face Position Determination //
			*rEyeROI = rEyeResult[i];
		}
	}

	if (!lEyeN)
		lEyeROI->width = 0;
	else
	{
		int maxEyeCount = min_eye_nbr;
		int most_possible_eye = 0;

		int i = lEyeN - 1;
		do
		{
			if (lEyeCount[i] > maxEyeCount)
			{
				maxEyeCount = lEyeCount[i];
				most_possible_eye = i;
			}
		} while (i--);

		if (maxEyeCount == min_eye_nbr)
		{
			lEyeROI->width = 0;
		}
		else
		{
			i = most_possible_eye;

			// Face Position Determination //
			*lEyeROI = lEyeResult[i];
		}
	}

	return 1;
#else
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

	FaceDetect_set_ScalerFn(WorkMem, Scaler_Start, Scaler_wait_end, Scaler_clip);
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

	Scaler_Start(gray, &imgWork);

	min_width = 20;
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
#endif
}


/*
 *****************************************************************************
 *
 * draw face
 *
 *****************************************************************************
 */
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
	gpImage	imgIn_old;


	if (!drawFace_flag)
		return;

	DBG_PRINT("pic_face\r\n");

	imgIn_old.width     = C_DISPLAY_DEV_HPIXEL;
	imgIn_old.height    = C_DISPLAY_DEV_VPIXEL;
	imgIn_old.widthStep = C_DISPLAY_DEV_HPIXEL*2;
	imgIn_old.ch        = 2;
//	imgIn_old.format    = IMG_FMT_UYVY;
	imgIn_old.format    = IMG_FMT_YUYV;
	imgIn_old.ptr       = (unsigned char *) frame_buffer;

	drawFace(&imgIn_old, face_count, Face_old, Count_old);
}


/*
 *****************************************************************************
 *
 * security
 *
 *****************************************************************************
 */
int adjustSecurity_get(void)
{
	INT32U temp;

	_SECURITY(DBG_PRINT("(Security) level: %d (easy->hard = 1->5), ",securityLevel));
	switch(securityLevel)
	{
	     case 1:
	          temp = 54614;
	          break;
	     case 2:
	          temp = 81920;
	          break;
	     case 4:
	          temp = 98304;
	          break;
	     case 5:
	          temp = 147456;
	          break;
	     default:
	          temp = 90112;
	          break;
	}
	_SECURITY(DBG_PRINT("value: %d \r\n", temp));
	return temp;
}

int adjustSecurity_set(int set_value)
{
	if(set_value)
	{
	   securityLevel++;
	   if(securityLevel > 5)
	      securityLevel = 5;
	}
	else
	{
	   securityLevel--;
	   if(securityLevel < 1)
	   	  securityLevel = 1;

	}

    return adjustSecurity_get();
}

int adjustSecurity_init(int level)
{
    if(level <=5 && level > 0)
    	securityLevel = level;
    else
    	securityLevel = 3;

    return adjustSecurity_get();
}


/*
 *****************************************************************************
 *
 * face training
 *
 *****************************************************************************
 */
INT32U face_train_set(INT32U frame_buffer)
{
	INT32S  nRet;

	imgIn.width	= C_DISPLAY_DEV_HPIXEL;
	imgIn.height	= C_DISPLAY_DEV_VPIXEL;
	imgIn.widthStep = C_DISPLAY_DEV_HPIXEL*2;
	imgIn.ch	= 2;
	imgIn.format	= IMG_FMT_UYVY;
	imgIn.ptr	= (unsigned char *) frame_buffer;

	frio_lo(FRIO_GPIO1 | FRIO_GPIO2);
	nRet = faceRoiDetect(&imgIn, &Face[0], Count);
	if (nRet) {
		serial_send(STA_TRKH, Face[0].x / 10);
		serial_send(STA_TRKV, Face[0].y / 10);
		serial_send(STA_TRKAREA, ((unsigned long) Face[0].width * (unsigned long) Face[0].height) / 1800);

		frio_hi(FRIO_GPIO1);

		FaceIdentify_Train(&imgIn, &Face[0], ownerULBP, sensor_frame, fiMem);
		_TRAIN(DBG_PRINT("(Train) train frame = %d\r\n", sensor_frame));
		sensor_frame++;

		frio_hi(FRIO_GPIO2);

	} else {
//		_TRAIN(DBG_PRINT("(Train) No Face Founded : %d \r\n",nRet));
	}

	return sensor_frame;
}

void ftrain_start(INT32U frame)
{
	OSQPost(ft_start_q, (void *) frame);
}

INT32U ftrain_end(void)
{
	INT8U	err;
	INT32U	msg;

	msg = (INT32U) OSQAccept(ft_end_q, &err);
	if ((err != OS_NO_ERR) || !msg)
		return 0;
	return msg;
}

static void task_ftrain(void *para)
{
	INT8U	err;
	INT32U	msg;

	ft_start_q = OSQCreate(ft_start_q_stack, QUEUE_FTRAIN);
	if (!ft_start_q)
		_MSG(DBG_PRINT("(Train) ft_start_q - create fail\r\n"));

	ft_end_q = OSQCreate(ft_end_q_stack, QUEUE_FTRAIN);
	if (!ft_end_q)
		_MSG(DBG_PRINT("(Train) ft_end_q - create fail\r\n"));


	while (1) {
		OSQPost(ft_end_q, (void *) msg);

		msg = (INT32U) OSQPend(ft_start_q, 0, &err);
		if ((err != OS_NO_ERR) || !msg)
			continue;

		if (face_train_set(msg) >= 20) {
			frio_lo(FRIO_GPIO1 | FRIO_GPIO2 | FRIO_GPIO3);
			frdb_store(frdb_get_curr());
			_TRAIN(DBG_PRINT("(Train) frdb store\r\n"));
			frio_hi(FRIO_GPIO3);

			serial_send(STA_TRAIN, (1 << frdb_get_curr()));

			sensor_frame  = 0;
			ownerULBP     = (INT32U *) frdb_get_c_train();
			if (ownerULBP == NULL) {
				_TRAIN(DBG_PRINT("(Train) Finished\r\n"));
				fd_chg_mode(MODE_STANDBY);
			}
		}
	}
}

void ftrain_init(unsigned char prio)
{
	OSTaskCreate(task_ftrain, (void *) 0, &ftrain_stack[STKSZ_FTRAIN - 1], prio);
}


/*
 *****************************************************************************
 *
 * face identify
 *
 *****************************************************************************
 */
INT32U face_verify_set(INT32U frame_buffer)
{
	INT32S	nRet;
	INT32U	i;

	imgIn.width	= C_DISPLAY_DEV_HPIXEL;
	imgIn.height	= C_DISPLAY_DEV_VPIXEL;
	imgIn.widthStep = C_DISPLAY_DEV_HPIXEL*2;
	imgIn.ch	= 2;
	imgIn.format	= IMG_FMT_UYVY;
	imgIn.ptr	= (unsigned char*) frame_buffer;

	frio_lo(FRIO_GPIO1 | FRIO_GPIO2);
	nRet = faceRoiDetect(&imgIn, Face, Count);
	if (nRet) {
		serial_send(STA_TRKH, Face[0].x / 10);
		serial_send(STA_TRKV, Face[0].y / 10);
		serial_send(STA_TRKAREA, ((unsigned long) Face[0].width) * ((unsigned long) Face[0].height) / 1800);

		frio_hi(FRIO_GPIO1);

#if FACE_IDENTIFY_EN == 1
		if (face_verify_flag) {
			nRet = FaceIdentify_Verify((gpImage *)&imgIn, (gpRect *)&Face[0],(void *)ownerULBP, (const int)adjustSecurity_get(), (void *)fiMem);
			if (nRet) {
				verify_fail = 0;
				if (verify_pass++ > 1) {
					_IDENT(DBG_PRINT("(Identify) OK\r\n"));

					frio_hi(FRIO_GPIO2);
				}
			} else {
				verify_pass = 0;
				if (verify_fail++ > 9) {
					_IDENT(DBG_PRINT("(Identify) Fail\r\n"));
				}
			}

			if ((verify_pass > 1+1) || (verify_fail > 9+1)) {
				serial_send(STA_IDENT, (1 << frdb_get_curr()));
				verify_pass = 0;
				verify_fail = 0;
				ownerULBP   = (INT32U *) frdb_get_c_ident();
				if (ownerULBP == NULL) {
					fd_chg_mode(MODE_STANDBY);
				}
			}
		} else {
//			drawFace_flag = 1;
			face_count    = nRet;
			for (i=0; i<face_count; i++) {
				Count_old[i]       = Count[i];

				Face_old[i].x	   = Face[i].x;
				Face_old[i].y	   = Face[i].y;
				Face_old[i].width  = Face[i].width;
				Face_old[i].height = Face[i].height;
			}
		}
#endif
	} else {
		verify_pass = 0;
		verify_fail = 0;
//		if (drawFace_flag++ > 2)
			drawFace_flag = 0;

//		DBG_PRINT("(Identify) No Face Founded : %d \r\n",nRet);
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

	//post ready frame to video encode task
	msg_id = (INT32U) OSQAccept(face_detect_end_q, &err);
	if ((err != OS_NO_ERR) || !msg_id)
		return 0;
	return msg_id;
}

static void task_fident(void *para)
{
	INT8U err;
	INT32U msg_id;


	face_frame_q = OSQCreate(face_frame_q_stack, C_FACE_QUEUE_MAX);
	if (!face_frame_q)
		_MSG(DBG_PRINT("(Identify) face_frame_q - create fail\r\n"));

	face_detect_end_q = OSQCreate(face_detect_end_q_stack, C_FACE_QUEUE_MAX);
	if (!face_detect_end_q)
		_MSG(DBG_PRINT("(Identify) face_detect_end_q - create fail\r\n"));

	while (1) {
		msg_id = (INT32U) OSQPend(face_frame_q, 0, &err);
		if ((err != OS_NO_ERR) || !msg_id)
			continue;

		face_verify_set(msg_id);

		// post ready frame to video encode task
		OSQPost(face_detect_end_q, (void *) msg_id);
	}
}

void fident_init(unsigned char prio)
{
	OSTaskCreate(task_fident, (void *) 0, &FaceTaskStack[FaceTaskStackSize - 1], prio);
}

