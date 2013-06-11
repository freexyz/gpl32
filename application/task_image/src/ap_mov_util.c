#include "ap_mov_util.h"

INT32S quicktime_read_char(INT16U fin);
INT32S quicktime_read_int16(INT16U fin);
INT32S quicktime_read_int24(INT16U fin);
INT32S quicktime_read_int32(INT16U fin);
INT32U quicktime_read_uint32(INT16U fin);
INT64U quicktime_read_int64(INT16U fin);
FP32 quicktime_read_fixed16(INT16U fin);
FP32 quicktime_read_fixed32(INT16U fin);
FP64 quicktime_read_double64(INT16U fin);
void quicktime_read_pascal(INT16U fin, INT8S *data);

INT32S quicktime_read_char(INT16U fin)
{
	CHAR result;
	read(fin, (INT32U)&result,1);
	return result;
}

INT32S quicktime_read_int16(INT16U fin)
{
	INT32S result,a,b;
	INT8U data[2];
	read(fin, (INT32U)data, 2);
	a = data[0];
	b = data[1];
	result = (a << 8) | b;
	return result;
}

INT32S quicktime_read_int24(INT16U fin)
{
	INT32S result,a,b,c;
	INT8U data[3];
	read(fin, (INT32U)data, 3);
	a = data[0];
	b = data[1];
	c = data[2];

	result = (a << 16) | (b << 8) | c;
	return result;
}

INT32S quicktime_read_int32(INT16U fin)
{
	INT32S result,a,b,c,d;
	INT8U data[4];
	read(fin, (INT32U)data, 4);
	a = data[0];
	b = data[1];
	c = data[2];
	d = data[3];

	result = (a << 24) | (b << 16) | (c << 8)| d;
	return result;
}

INT32U quicktime_read_uint32(INT16U fin)
{
	INT32U result;
	INT32U a, b, c, d;
	INT8U data[4];

	read(fin, (INT32U)data, 4);
	a = data[0];
	b = data[1];
	c = data[2];
	d = data[3];

	result = (a << 24) | (b << 16) | (c << 8) | d;
	return result;
}

INT64U quicktime_read_int64(INT16U fin)
{
	INT64U result, a, b, c, d, e, f, g, h;
	INT8U data[8];

	read(fin, (INT32U)data, 8);
	a = data[0];
	b = data[1];
	c = data[2];
	d = data[3];
	e = data[4];
	f = data[5];
	g = data[6];
	h = data[7];

	result = (a << 56) | 
		(b << 48) | 
		(c << 40) | 
		(d << 32) | 
		(e << 24) | 
		(f << 16) | 
		(g << 8) | 
		h;
	return (INT64U)result;
}

FP32 quicktime_read_fixed16(INT16U fin)
{
	INT8U data[2];
	read(fin, (INT32U)data, 2);
	
	if(data[1])
		return (FP32)data[0] + (FP32)data[1] / 256;
	else
		return (FP32)data[0];
}

FP32 quicktime_read_fixed32(INT16U fin)
{
	INT32U a, b, c, d;
	INT8U data[4];

	read(fin, (INT32U)data, 4);
	a = data[0];
	b = data[1];
	c = data[2];
	d = data[3];
	
	a = (a << 8) + b;
	b = (c << 8) + d;

	if(b)
		return (FP32)a + (FP32)b / 65536;
	else
		return (FP32)a;
}

static FP64 double64_be_read (INT8U *cptr)
{       
  INT32S exponent, negative ;
  FP64	 dvalue ;

  negative = (cptr [0] & 0x80) ? 1 : 0 ;
  exponent = ((cptr [0] & 0x7F) << 4) | ((cptr [1] >> 4) & 0xF) ;

  /* Might not have a 64 bit long, so load the mantissa into a double. */
  dvalue = (((cptr [1] & 0xF) << 24) | (cptr [2] << 16) | (cptr [3] << 8) | cptr [4]) ;
  dvalue += ((cptr [5] << 16) | (cptr [6] << 8) | cptr [7]) / ((double) 0x1000000) ;

  if (exponent == 0 && dvalue == 0.0)
    return 0.0 ;

  dvalue += 0x10000000 ;
  exponent = exponent - 0x3FF ;
  dvalue = dvalue / ((FP64) 0x10000000) ;

  if (negative)
  	dvalue *= -1 ;

  if (exponent > 0)
    dvalue *= (1 << exponent) ;
  else if (exponent < 0)
  {
    exponent = ~exponent;
    dvalue /= (1 << exponent) ;
  }
  return dvalue ;
} 

FP64 quicktime_read_double64(INT16U fin)
{
	INT8U b[8];
	read(fin, (INT32U)b, 8);
	return double64_be_read(b);
}

void quicktime_read_pascal(INT16U fin, INT8S *data)
{
	INT8S len = quicktime_read_char(fin);
	read(fin, (INT32U)data, len);
	data[(INT32S)len] = 0;
}