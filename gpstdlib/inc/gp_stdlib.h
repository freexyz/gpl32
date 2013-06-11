#ifndef __GP_STD_LIB_H__
#define __GP_STD_LIB_H__

#include "project.h"

/******************************************************************************
 * External Functions
 ******************************************************************************/ 
extern INT32S gp_strcmp(INT8S *s,INT8S *t);                 
extern INT8S *gp_strcpy(INT8S *t,INT8S *s);                 
extern INT32S gp_strlen(INT8S *s);                          
extern INT32S gp_strncmp(INT8S *dest,INT8S *src,INT32U Len);
extern INT8S gp_toupper(INT8S c);                           
extern INT8S *gp_strcat(INT8S *s,INT8S *t);                 
extern INT32S gp_memcpy(INT8S *dest, INT8S *src, INT32U Len);
extern INT32S gp_memset(INT8S *dest,INT8U byte, INT32U Len);
extern INT32S gp_memcmp(INT8S* dest, INT8S* src, INT32U Len);
/******************************************************************************
 * Define functions
 ******************************************************************************/ 
//#define gp_strcpy(t,s) gp_strcpy2((INT8S*)t,(INT8S*)s); 


#endif