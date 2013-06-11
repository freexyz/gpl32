#include "project.h"
#include "gplib_print_string.h"

#if GPLIB_PRINT_STRING_EN == 1

#include <stdarg.h>
#include <stdio.h>

#if _DRV_L1_UART == 1
	#define SEND_DATA(x)	uart0_data_send(x, 1)
	#define GET_DATA(x)		uart0_data_get(x, 1)
#else
	#define SEND_DATA(x)
	#define GET_DATA(x)		(*(x) = '\r')
#endif

#define PRINT_OUTPUT_NONE	0x00 
#define PRINT_OUTPUT_UART	0x01
#define PRINT_OUTPUT_USB	0x02

static INT32U print_output_type = PRINT_OUTPUT_UART;	/* 0 = NONE, 1 = UART, 2 = USB */

/* For USB logger used */
#define USB_LOGGER_BUF_SIZE	512
#define USB_LOGGER_BUF_NUM	10
#define USB_LOGGER_BUF_TOTAL_SIZE	(USB_LOGGER_BUF_SIZE*USB_LOGGER_BUF_NUM)

static CHAR usb_logger_buf[USB_LOGGER_BUF_TOTAL_SIZE] = {0};
CHAR* logger_read_ptr = usb_logger_buf;
CHAR* logger_write_ptr = usb_logger_buf;
//INT32U loggerwritecnt = 0;
//INT32U loggerreadcnt = 0;

static CHAR print_buf[PRINT_BUF_SIZE];

INT32U get_usb_logger_data(void)
{
	INT32U ret = 0;
	
	/* read operation */
#if _OPERATING_SYSTEM != _OS_NONE	// Soft Protect for critical section
	OSSchedLock();
#endif	
	ret = (INT32U)logger_read_ptr;
	//memcpy((CHAR*)dataptr, usb_read_ptr, USB_LOGGER_BUF_SIZE);

	if(logger_read_ptr == logger_write_ptr)
	{
		logger_read_ptr[0] = 0;
	}
	else
	{
		if(logger_read_ptr < (usb_logger_buf + USB_LOGGER_BUF_TOTAL_SIZE - USB_LOGGER_BUF_SIZE))
	    	logger_read_ptr += USB_LOGGER_BUF_SIZE;
	    else
	    	logger_read_ptr = usb_logger_buf;
    }
	
	//loggerwritecnt = (logger_write_ptr - usb_logger_buf) >> 9;
    //loggerreadcnt = (logger_read_ptr - usb_logger_buf) >> 9;
	    	
#if _OPERATING_SYSTEM != _OS_NONE	//exit critical section
	OSSchedUnlock();
#endif
  
    return ret;	
}

void print_string(CHAR *fmt, ...)
{
    va_list v_list;
    INT32S ret;

	if(print_output_type & PRINT_OUTPUT_USB)
	{

#if _OPERATING_SYSTEM != _OS_NONE	// Soft Protect for critical section
		OSSchedLock();
#endif
		/* redirect print_string to usb_logger_buf, write operation */
		va_start(v_list, fmt);
		ret = vsnprintf(logger_write_ptr, USB_LOGGER_BUF_SIZE, fmt, v_list);
		if(ret < 0)
		{
			return;
		}
	    //vsprintf(usb_logger_buf, fmt, v_list);
	    va_end(v_list);
	    
	     
	    /* Update write pointer */
	    if(logger_write_ptr < (usb_logger_buf + USB_LOGGER_BUF_TOTAL_SIZE - USB_LOGGER_BUF_SIZE ))
	    	logger_write_ptr += USB_LOGGER_BUF_SIZE;
	    else
	    	logger_write_ptr = usb_logger_buf;
	    
	    /* Check if write ptr is equal to read ptr, if yes update read ptr */
	    if(logger_write_ptr == logger_read_ptr)
	    {
	    	if(logger_read_ptr < (usb_logger_buf + USB_LOGGER_BUF_TOTAL_SIZE - USB_LOGGER_BUF_SIZE ))
	    		logger_read_ptr += USB_LOGGER_BUF_SIZE;
	    	else
	    		logger_read_ptr = usb_logger_buf;
		}   
	    
	    //loggerwritecnt = (logger_write_ptr - usb_logger_buf) >> 9;
	    //loggerreadcnt = (logger_read_ptr - usb_logger_buf) >> 9;		
#if _OPERATING_SYSTEM != _OS_NONE			//exit critical section
		OSSchedUnlock();
#endif
	}
	
	if(print_output_type & PRINT_OUTPUT_UART)
	{
		CHAR *pt;
	    va_start(v_list, fmt);
	    ret = vsnprintf(print_buf, PRINT_BUF_SIZE, fmt, v_list);
		if(ret < 0)
		{
			return;
		}
	    //vsprintf(print_buf, fmt, v_list);
	    va_end(v_list);
	
	    print_buf[PRINT_BUF_SIZE-1] = 0;
	    pt = print_buf;
	    while (*pt) {
			SEND_DATA(*pt);
			pt++;
		}
	}	
}

void get_string(CHAR *s)
{
    INT8U temp;

    while (1) {
        GET_DATA(&temp);
        SEND_DATA(temp);
        if (temp == '\r') {
            *s = 0;
            return;
        }
        *s++ = (CHAR) temp;
    }
}

void set_print_output_type(INT32U type)
{
	print_output_type = type;	
}	

#else

void print_string(CHAR *fmt, ...)
{
}

void get_string(CHAR *s)
{
}

INT32U get_usb_logger_data(void)
{
}
 

void set_print_output_type(INT32U type)
{
	
}	

#endif		// GPLIB_PRINT_STRING_EN