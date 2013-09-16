#ifndef __MMP_H__
#define __MMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "MultiMediaParser_Cfg.h"

// error id
#define MEDIAPARSER_FILE_READ_ERR		0x80000001
#define MEDIAPARSER_FORMAT_ERR			0x80000002
#define MEDIAPARSER_KERNEL_ERR			0x80000003
#define MEDIAPARSER_MEM_ERR				0x80000004
#define MEDIAPARSER_VID_BUF_SIZE_ERR	0x80000005
#define MEDIAPARSER_EXDATA_OVERFLOW		0x80000006
#define MEDIAPARSER_PARAM_ERR			0x80000007
#define MEDIAPARSER_FILE_SEEK_ERR		0x80000008
#define MEDIAPARSER_NO_STREAM_EXIST		0x80000009
#define MEDIAPARSER_NO_INDEX			0x8000000A
#define MEDIAPARSER_OS_ERR				0x8000000B


#define WAVE_FORMAT_PCM				(0x0001)
#define WAVE_FORMAT_MULAW			(0x0007)
#define WAVE_FORMAT_ALAW			(0x0006) 
#define WAVE_FORMAT_ADPCM			(0x0002) 
#define WAVE_FORMAT_DVI_ADPCM		(0x0011) 
#define WAVE_FORMAT_MPEGLAYER3		(0x0055) 
// #define WAVE_FORMAT_AAC			(0xA106) 
#define WAVE_FORMAT_RAW_AAC1		(0x00FF)
#define WAVE_FORMAT_MPEG_ADTS_AAC	(0x1600)

#ifdef WIN32
	#include "stdio.h"
	typedef FILE *FILEHANDLE;
#endif

#ifdef __CC_ARM
	typedef int FILEHANDLE;
#endif

typedef struct 
{
	unsigned short	wFormatTag;
	unsigned short	nChannels;
	unsigned long	nSamplesPerSec;
	unsigned long	nAvgBytesPerSec;
	unsigned short	nBlockAlign;
	unsigned short	wBitsPerSample;
	unsigned short	cbSize;
} GP_WAVEFORMATEX;	// strf

typedef struct
{ 
	unsigned long biSize; 
	long biWidth; 
	long biHeight; 
	unsigned short biPlanes; 
	unsigned short  biBitCount;
	unsigned long biCompression; 
	unsigned long biSizeImage; 
	long biXPelsPerMeter; 
	long biYPelsPerMeter; 
	unsigned long biClrUsed; 
	unsigned long biClrImportant; 
} GP_BITMAPINFOHEADER; 

// Parser
const char *MultiMediaParser_GetVersion(void);
void *MultiMediaParser_Create(
	int fid,
	long FileSize,
	int AudRingLen,
	int VidBufSize,
	int prio,
	const void *FcnTab,
	int (*ErrHandler)(int ErrID),
	int *ErrID);
	
void MultiMediaParser_Delete(void *hWorkMem);
int MultiMediaParser_SeekTo(void *hWorkMem, long Sec);
void MultiMediaParser_SetFrameDropLevel(void *hWorkMem, int n);
int MultiMediaParser_EnableAudStreaming(void *hWorkMem, int flag);
int MultiMediaParser_SetReversePlay(void *hWorkMem, int flag);

// Audio Functions
const GP_WAVEFORMATEX *MultiMediaParser_GetAudInfo(const void *hWorkMem, long *Len);
int MultiMediaParser_IsEOA(const void *hWorkMem);
long MultiMediaParser_GetAudTrackDur(const void *hWorkMem);
const char *MultiMediaParser_GetAudRing(void *hWorkMem);
int MultiMediaParser_GetAudRingLen(void *hWorkMem);
int MultiMediaParser_GetAudRingWI(void *hWorkMem);
int MultiMediaParser_RefillAudBuf(void *hWorkMem, int RI);

// Video Function
const GP_BITMAPINFOHEADER *MultiMediaParser_GetVidInfo(const void *hWorkMem, long *Len);
long MultiMediaParser_GetVidTickRate(const void *hWorkMem);
long MultiMediaParser_GetVidTotalSamples(const void *hWorkMem);
int MultiMediaParser_IsEOV(const void *hWorkMem);
int MultiMediaParser_GetNumberOfFrameInVidBuf(const void *hWorkMem);
long MultiMediaParser_GetVidCurTime(const void *hWorkMem);
long MultiMediaParser_GetVidTrackDur(const void *hWorkMem);
int MultiMediaParser_RegisterReadFunc(int ReadFunc(short, unsigned int, unsigned int));
#if SEEK64_EN == 0
int MultiMediaParser_RegisterSeekFunc(int SeekFunc(short, int, short));
#else
int MultiMediaParser_RegisterSeekFunc(long long SeekFunc(short, long long, short));
#endif
const char *MultiMediaParser_GetVidBuf(void *hWorkMem, long *Len, long *Duration, int *ErrID);

#ifdef __cplusplus
}
#endif

#endif // __MMP_H__
