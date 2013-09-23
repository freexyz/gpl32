#include "application.h"
#include "drv_l1_sfr.h"
#include "define.h"
#include "FaceDetectAP.h"
#include "FaceDetectKernel.h"
typedef enum
{
  IMAGE_COLOR_FORMAT_YUYV,
  IMAGE_COLOR_FORMAT_UYVY,	        
  IMAGE_COLOR_FORMAT_Y_ONLY        
} IMAGE_COLOR_FORMAT;

extern void face_frame_set(INT32U frame);