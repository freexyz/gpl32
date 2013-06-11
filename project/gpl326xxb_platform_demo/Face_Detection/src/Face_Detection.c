#include <stdio.h>
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "Face_Detection.h"
#include "FaceIdentifyAP.h"
#include "video_codec_callback.h"

// NAND or SPI version.
#define ZTKF_SPI

// Face recognition status is output through IOs.
#define ZTKF_IO_STATUS

// Status IO assignment.
#ifdef ZTKF_SPI
#define ZTKIO_MODE          IO_A12
#define ZTKIO_FACEDETECTED  IO_A13
#define ZTKIO_FUNCCOMPLETE  IO_A14
#define ZTKIO_DATABASEFULL  IO_A15
#else // ZTKF_SPI
#define ZTKIO_MODE          IO_C1
#define ZTKIO_FACEDETECTED  IO_C2
#define ZTKIO_FUNCCOMPLETE  IO_C3
#define ZTKIO_DATABASEFULL  IO_C4
#endif // ZTKF_SPI


#define FACE_IDENTIFY_EN    1

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
INT32U securityLevel,sensor_frame,verify_counter,face_count;
INT32U *fiMem;
INT32U *ownerULBP;
gpRect Face_old[MAX_RESULT];	
int Count_old[MAX_RESULT];

#ifdef ZTKF_IO_STATUS
void ZTK_Delay(int iCycle)
{
    int i;
    int j;

    for (i = 0; i < iCycle; i++)
    {
        j = i;
    }
}

void ZTK_InitStatusIO()
{
	gpio_init_io(ZTKIO_MODE, GPIO_OUTPUT);
	gpio_init_io(ZTKIO_FACEDETECTED, GPIO_OUTPUT);
	gpio_init_io(ZTKIO_FUNCCOMPLETE, GPIO_OUTPUT);
	gpio_init_io(ZTKIO_DATABASEFULL, GPIO_OUTPUT);

	gpio_drving_init_io(ZTKIO_MODE,(IO_DRV_LEVEL) IO_DRIVING_16mA);////IO_DRIVING_16mA
	gpio_drving_init_io(ZTKIO_FACEDETECTED,(IO_DRV_LEVEL) IO_DRIVING_16mA);////IO_DRIVING_16mA
	gpio_drving_init_io(ZTKIO_FUNCCOMPLETE,(IO_DRV_LEVEL) IO_DRIVING_16mA);////IO_DRIVING_16mA
	gpio_drving_init_io(ZTKIO_DATABASEFULL,(IO_DRV_LEVEL) IO_DRIVING_16mA);////IO_DRIVING_16mA

    // Default status.
	gpio_write_io(ZTKIO_MODE, 0);
	gpio_write_io(ZTKIO_FACEDETECTED, 0);
	gpio_write_io(ZTKIO_FUNCCOMPLETE, 0);
	gpio_write_io(ZTKIO_DATABASEFULL, 0);
}

void ZTK_OutputEnterTrainingStatus()
{
	gpio_write_io(ZTKIO_MODE, 0);
}

void ZTK_OutputEnterRecognitionStatus()
{
	gpio_write_io(ZTKIO_MODE, 1);
}

void ZTK_OutputFaceDetectedStatus()
{
	gpio_write_io(ZTKIO_FACEDETECTED, 1);
}

void ZTK_OutputFaceTrainedStatus()
{
	gpio_write_io(ZTKIO_FUNCCOMPLETE, 1);
}

void ZTK_OutputFaceRecognizedStatus()
{
	gpio_write_io(ZTKIO_FUNCCOMPLETE, 1);
}

void ZTK_OutputDatabaseEmptyStatus()
{
	gpio_write_io(ZTKIO_DATABASEFULL, 0);
}

void ZTK_OutputDatabaseFullStatus()
{
	gpio_write_io(ZTKIO_DATABASEFULL, 1);
}

void ZTK_ClearFaceDetectedStatus()
{
	gpio_write_io(ZTKIO_FACEDETECTED, 0);
}

void ZTK_ClearFaceProcessSuccessStatus()
{
	gpio_write_io(ZTKIO_FUNCCOMPLETE, 0);
}
#endif // ZTKF_IO_STATUS

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
	imgIn_old.format = IMG_FMT_UYVY;
	imgIn_old.ptr = (unsigned char*)frame_buffer;     
	
	#if FACE_IDENTIFY_EN == 0
	  drawFace(&imgIn_old,face_count, Face_old,Count_old);
	#endif   
}

int adjustSecurity_get(void)
{			
	INT32U temp;
	
	DBG_PRINT("Security Level: %d (easy->hard = 1->5)\r\n",securityLevel);
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
	DBG_PRINT("Security Value: %d \r\n",temp);	
	
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

INT32U face_train_set(INT32U frame_buffer)
{
		INT32S  nRet;
		
		imgIn.width = C_DISPLAY_DEV_HPIXEL;
		imgIn.height = C_DISPLAY_DEV_VPIXEL;
		imgIn.widthStep = C_DISPLAY_DEV_HPIXEL*2;
		imgIn.ch = 2;
		imgIn.format = IMG_FMT_UYVY;
		imgIn.ptr = (unsigned char*)frame_buffer;			
		nRet = faceRoiDetect(&imgIn,&Face[0],Count); 

		if(nRet)
		{  
#ifdef ZTKF_IO_STATUS
            ZTK_OutputFaceDetectedStatus();
#endif // ZTKF_IO_STATUS
        
			FaceIdentify_Train(&imgIn,&Face[0], ownerULBP, sensor_frame, fiMem);
			DBG_PRINT("Face Train Value : %d OK \r\n",sensor_frame);
			sensor_frame++;
            
#ifdef ZTKF_IO_STATUS
            ZTK_OutputFaceTrainedStatus();
            ZTK_Delay(100000);

            if (sensor_frame < 20)
            {
                ZTK_ClearFaceProcessSuccessStatus();
            }
            else
            {
                ZTK_OutputDatabaseFullStatus();
            }
            
            ZTK_ClearFaceDetectedStatus();
#endif // ZTKF_IO_STATUS

		}
		else
		    DBG_PRINT("/**********Face Train : No Face Founded**********/ : %d \r\n",nRet);   
                   
        return sensor_frame;
}

INT32U face_verify_set(INT32U frame_buffer)
{	
		INT32S nRet;
		#if FACE_IDENTIFY_EN == 0
			INT32U i;
		#endif
		
		imgIn.width = C_DISPLAY_DEV_HPIXEL;
		imgIn.height = C_DISPLAY_DEV_VPIXEL;
		imgIn.widthStep = C_DISPLAY_DEV_HPIXEL*2;
		imgIn.ch = 2;
		imgIn.format = IMG_FMT_UYVY;
		imgIn.ptr = (unsigned char*)frame_buffer;

		nRet = faceRoiDetect(&imgIn,Face,Count);

		if(nRet)
		{
			#if FACE_IDENTIFY_EN == 1		   
				if(face_verify_flag)
				{
#ifdef ZTKF_IO_STATUS
                    ZTK_OutputFaceDetectedStatus();
#endif // ZTKF_IO_STATUS

					nRet=FaceIdentify_Verify((gpImage *)&imgIn, (gpRect *)&Face[0],(void *)ownerULBP, (const int)adjustSecurity_get(), (void *)fiMem);
					if(nRet)
					{
					    if(verify_counter++ > 1)
					    {
						    face_verify_flag=0;
						    DBG_PRINT("/**********Face Identify : OK**********/\r\n");
						    DBG_PRINT("/**********Face Identify Demo End**********/\r\n"); 
#ifdef ZTKF_IO_STATUS
                            ZTK_OutputFaceRecognizedStatus();
                            ZTK_Delay(100000);
#endif // ZTKF_IO_STATUS
					    }					     		            
					}
					else
					{
						verify_counter=0;
						DBG_PRINT("/**********Face Identify : Fail**********/\r\n");
					}
            
#ifdef ZTKF_IO_STATUS
                    ZTK_ClearFaceDetectedStatus();
#endif // ZTKF_IO_STATUS
				}
			#else
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
			#endif
		}
		else
		{
		   verify_counter=0;
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
    #if FACE_IDENTIFY_EN == 1
    	int memSize;
    #endif
    
	// Create a user-defined task
	OSTaskCreate(Face_task_entry, (void *) 0, &FaceTaskStack[FaceTaskStackSize - 1], 30); 

	// Initialize display device
//	tv_init();	
//	tv_start (TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_INTERLACE);	
	
	tft_init();
	tft_start(TPO_TD025THD1);
	
	user_defined_video_codec_entrance();
	video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder

#ifdef ZTKF_IO_STATUS
    ZTK_InitStatusIO();
#endif // ZTKF_IO_STATUS

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

    #if FACE_IDENTIFY_EN == 1
		memSize = FaceIdentify_MemCalc();
		fiMem = (INT32U *)gp_malloc_align((memSize),8);    
	    ownerULBP = (INT32U *)gp_malloc_align((1888*20),8);
	    adjustSecurity_init(3);
	    adc_key_scan_init();                    //init key scan
    #endif 
    
    while(1)
    {
    	#if FACE_IDENTIFY_EN == 1
	    	adc_key_scan();
			if(ADKEY_IO1)
			{
                face_verify_flag = 0;

				if(!face_verify_flag)
				{
				   sensor_frame=0;
				   if(train_counter)
				   {
				   		train_counter=0;
				   		DBG_PRINT("/**********Face Train Finish**********/ \r\n");    
				   }
				   else
				   {
					   gp_memset((INT8S *)ownerULBP,0,(1888*20));
					   train_counter=1;
					   DBG_PRINT("/**********Face Train Start**********/ \r\n");
#ifdef ZTKF_IO_STATUS
                        ZTK_ClearFaceProcessSuccessStatus();
                        ZTK_ClearFaceDetectedStatus();
                        ZTK_OutputDatabaseEmptyStatus();
                        ZTK_OutputEnterTrainingStatus();                       
#endif // ZTKF_IO_STATUS
				   }
				}               
			}

			if(ADKEY_IO2)
			{
//				if(face_verify_flag)
//				{
//				    face_verify_flag=0;
//				    DBG_PRINT("/**********Face Identify Demo End**********/\r\n");               
//				}
//				else
				{
				    face_verify_flag=1;
				    verify_counter=0;
				    DBG_PRINT("/**********Face Identify Demo Start**********/\r\n");               
				}
                
#ifdef ZTKF_IO_STATUS
                ZTK_ClearFaceProcessSuccessStatus();
                ZTK_ClearFaceDetectedStatus();
                ZTK_OutputEnterRecognitionStatus();                       
#endif // ZTKF_IO_STATUS
			}
			
			if(ADKEY_IO3)
			{
                adjustSecurity_set(0);
			}
			
			if(ADKEY_IO4)
			{
                adjustSecurity_set(1);
			}						
		#else		
		    OSTimeDly(1);
		#endif
    
    }      
}
