#ifndef __FACEDETECTV2_KERNEL_H__
#define __FACEDETECTV2_KERNEL_H__

#include "define.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FD_WORKMEM_SIZE_ERR		0x80000101
#define FD_INVALID_WORKMEM		0x80000102
#define FD_IMG_FMT_ERR			0x80000103

#define CASCADE_FACE		0
#define CASCADE_LEYE		1
#define CASCADE_REYE		2
#define CASCADE_HAND		3
#define CASCADE_HANDSTONE	4


const char *FaceDetectKernel_GetVersion(void);


extern int FaceDetectKernel_Group(
	gpRect *Result,
	int *tmpCount,
	int maxResult,
	const gpRect *face_in,
	int *Index,
	int face_cnt);

extern int FaceDetectKernel_Config(
	void *WorkMem,
	int WorkMemSize,
	int maxWidth,
	int xStep,
	int yStep);

extern int FaceDetectKernel_SetX(
	void *WorkMem,
	int xStep,
	int xShift);


#if defined (BODY_GP19)
extern int FDKernel_HW(void *_WorkMem,
	const gpImage *image,
	gpRect *face,
	int maxface);
#else
extern int FaceDetectKernel_Run(
	void *WorkMem,
	const gpImage *image,
	gpRect *face,
	int maxface);
#endif

extern void FaceDetectKernel_ClearStatistic(void *WorkMem);
extern void FaceDetectKernel_GetStatistic(void *WorkMem, int statistic[3]);

// add by Comi 2012.01.30
extern void FaceDetectKernel_set_angle(void *WorkMem, int angle);


// add by Comi 2012.08.01
extern void FaceDetectKernel_set_detect_obj(void *_WorkMem, int cascade_type);


#ifdef __cplusplus
}
#endif


#endif // __FACEDETECTV2_KERNEL_H__
