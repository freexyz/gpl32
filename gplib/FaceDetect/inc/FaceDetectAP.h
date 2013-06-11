#ifndef __FACE_DETECT_V2_AP_H__
#define __FACE_DETECT_V2_AP_H__

#include "define.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FDAP_IMG_SIZE_OVERFLOW	0x80000201
#define FDAP_MEM_ERR			0x80000202
#define FDAP_INVALID_WORKMEM	0x80000203
#define FDAP_PARAM_ERR			0x80000204
#define FDAP_OS_ERR				0x80000205
#define FDAP_WORKMEM_SIZE_ERR	0x80000206
#define FDAP_IMG_FMT_ERR		0x80000207

#define CASCADE_FACE		0
#define CASCADE_LEYE		1
#define CASCADE_REYE		2
#define CASCADE_HAND		3
#define CASCADE_HANDSTONE	4


const char *FaceDetect_GetVersion(void);

int FaceDetect_Config(
	void *WorkMem,
	int WorkMemSize,
	int maxWidth,
	int maxHeight,
	int maxChannel,
	int maxCandidate,
	int xStep,
	int yStep);

int FaceDetect_SetScale(
	void *WorkMem,
	long Scale,
	int minWnd,
	int maxWnd);

int FaceDetect_SetX(
	void *_WorkMem,
	int xStep,
	int xShift);

int FaceDetect(
	void *WorkMem,
	const gpImage *Image, const gpRect *Rect,
	int MaxResult, gpRect *Result, int *Count);

void FaceDetect_GetStatistic(void *WorkMem, int *statistic);

void FaceDetect_set_ScalerFn(
	void *_WorkMem, 
	int (*scaler_start)(gpImage *srcImg, gpImage *dstImg), 
	int (*scaler_end)(void), 
	int (*scaler_clip)(gpImage *srcImg, gpImage *dstImg, gpRect *clip)
);

// angle is clockwise from 0 -> 90 -> 180 -> 270 -> 0 (360)
void FaceDetect_set_angle(void *_WorkMem, int angle);


void FaceDetect_set_detect_obj(void *_WorkMem, int cascade_type);

#ifdef __cplusplus
}
#endif

#endif // __FACE_DETECT_V2_H__
