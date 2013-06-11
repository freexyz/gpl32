#ifndef __DEFINE_H__
#define __DEFINE_H__

#define IMG_FMT_GRAY	0
#define IMG_FMT_YUYV	1
#define IMG_FMT_YCbYCr	2
#define IMG_FMT_RGB		3
#define IMG_FMT_UYVY	4
#define IMG_FMT_CbYCrY	5

#define MAX(x, y)	((x) >= (y) ? (x) : (y))
#define MIN(x, y)	((x) <  (y) ? (x) : (y))
#define ABS(x)		((x) >= 0 ? (x) : -(x))

typedef struct
{
	short width;
	short height;
	short vyuy;
	char  format;
	unsigned char *ptr;
} gpScaler;

typedef struct
{
	short width;
	short height;
	short widthStep;
	char  ch;
	char  format;
	unsigned char *ptr;
} gpImage;

typedef struct
{
    short x;
    short y;
    short width;
    short height;
} gpRect;

typedef struct
{
    short width;
    short height;
} gpSize;

typedef struct
{
	short x, y;
} gpPoint;

#endif // __DEFINE_H__
