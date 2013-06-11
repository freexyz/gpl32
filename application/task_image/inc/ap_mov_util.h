#ifndef __AP_MOV_UTIL_H__
#define __AP_MOV_UTIL_H__
#include "ap_mov_mjpeg.h"

extern	INT32S 	quicktime_read_char(INT16U fin);																										
extern	INT32S 	quicktime_read_int16(INT16U fin);																										
extern	INT32S 	quicktime_read_int24(INT16U fin);																										
extern	INT32S 	quicktime_read_int32(INT16U fin);																										
extern	INT32U 	quicktime_read_uint32(INT16U fin);																										
extern	INT64U 	quicktime_read_int64(INT16U fin);																										
extern	FP32 	quicktime_read_fixed16(INT16U fin);																										
extern	FP32 	quicktime_read_fixed32(INT16U fin);																										
extern	FP64 	quicktime_read_double64(INT16U fin);																										
extern	void 	quicktime_read_pascal(INT16U fin, INT8S *data);																										
#endif// __AP_MOV_UTIL_H__