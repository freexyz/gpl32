
#ifndef __TYPE_H__
#define __TYPE_H__

//#ifndef NULL
//#define NULL    ((void *)0)
//#endif

//#ifndef FALSE
//#define FALSE   (0)
//#endif

//#ifndef TRUE
//#define TRUE    (1)
//#endif

#define SWAP(x) (x & 0xFF),((x >> 8) & 0xFF)

typedef char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;

typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
typedef unsigned short UWORD;

#endif  /* __TYPE_H__ */
