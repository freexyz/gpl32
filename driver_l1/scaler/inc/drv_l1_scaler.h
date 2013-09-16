#ifndef __drv_l1_SCALER_H__
#define __drv_l1_SCALER_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

// Control register
#define	C_SCALER_CTRL_IN_RGB1555			0x00000000
#define	C_SCALER_CTRL_IN_RGB565				0x00000001
#define	C_SCALER_CTRL_IN_RGBG				0x00000002
#define	C_SCALER_CTRL_IN_GRGB				0x00000003
#define	C_SCALER_CTRL_IN_YUYV				0x00000004
#define	C_SCALER_CTRL_IN_UYVY				0x00000005
#define	C_SCALER_CTRL_IN_YUYV8X32			0x00100006
#define	C_SCALER_CTRL_IN_YUYV8X64			0x00100007
#define	C_SCALER_CTRL_IN_YUYV16X32			0x00200006
#define	C_SCALER_CTRL_IN_YUYV16X64			0x00200007
#define	C_SCALER_CTRL_IN_YUYV32X32			0x00000006
#define	C_SCALER_CTRL_IN_YUYV64X64			0x00000007
#define	C_SCALER_CTRL_IN_YUV422				0x00000008
#define	C_SCALER_CTRL_IN_YUV420				0x00000009
#define	C_SCALER_CTRL_IN_YUV411				0x0000000A
#define	C_SCALER_CTRL_IN_YUV444				0x0000000B
#define	C_SCALER_CTRL_IN_Y_ONLY				0x0000000C
#define	C_SCALER_CTRL_IN_YUV422V			0x0000000D
#define	C_SCALER_CTRL_IN_YUV411V			0x0000000E
#define	C_SCALER_CTRL_IN_MASK				0x0030000F

#define	C_SCALER_CTRL_OUT_RGB1555			0x00000000
#define	C_SCALER_CTRL_OUT_RGB565			0x00000010
#define	C_SCALER_CTRL_OUT_RGBG				0x00000020
#define	C_SCALER_CTRL_OUT_GRGB				0x00000030
#define	C_SCALER_CTRL_OUT_YUYV				0x00000040
#define	C_SCALER_CTRL_OUT_UYVY				0x00000050
#define	C_SCALER_CTRL_OUT_YUYV8X32			0x00040060
#define	C_SCALER_CTRL_OUT_YUYV8X64			0x00040070
#define	C_SCALER_CTRL_OUT_YUYV16X32			0x00080060
#define	C_SCALER_CTRL_OUT_YUYV16X64			0x00080070
#define	C_SCALER_CTRL_OUT_YUYV32X32			0x00000060
#define	C_SCALER_CTRL_OUT_YUYV64X64			0x00000070
#define	C_SCALER_CTRL_OUT_YUV422			0x00000080
#define	C_SCALER_CTRL_OUT_YUV420			0x00000090
#define	C_SCALER_CTRL_OUT_YUV411			0x000000A0
#define	C_SCALER_CTRL_OUT_YUV444			0x000000B0
#define	C_SCALER_CTRL_OUT_Y_ONLY			0x000000C0
#define	C_SCALER_CTRL_OUT_MASK				0x000C00F0

#define	C_SCALER_CTRL_START					0x00000100
#define	C_SCALER_CTRL_BUSY					0x00000100
#define	C_SCALER_CTRL_RESET					0x00000200
#define	C_SCALER_CTRL_TYPE_YUV				0x00000400
#define	C_SCALER_CTRL_TYPE_YCBCR			0x00000000
#define	C_SCALER_CTRL_INT_ENABLE			0x00000800
#define C_SCALER_CTRL_IN_FIFO_DISABLE		0x00000000
#define C_SCALER_CTRL_IN_FIFO_16LINE		0x00001000
#define C_SCALER_CTRL_IN_FIFO_32LINE		0x00002000
#define C_SCALER_CTRL_IN_FIFO_64LINE		0x00003000
#define C_SCALER_CTRL_IN_FIFO_128LINE		0x00004000
#define C_SCALER_CTRL_IN_FIFO_256LINE		0x00005000
#define C_SCALER_CTRL_IN_FIFO_LINE_MASK		0x00007000
#define C_SCALER_CTRL_OUT_OF_BOUNDRY    	0x00008000
#define C_SCALER_CTRL_VISIBLE_RANGE	    	0x00010000
#define C_SCALER_CTRL_CONTINUOUS_MODE   	0x00020000
#define C_SCALER_CTRL_OUT_FIFO_DISABLE		0x00000000
#define C_SCALER_CTRL_OUT_FIFO_16LINE		0x00400000
#define C_SCALER_CTRL_OUT_FIFO_32LINE		0x00800000
#define C_SCALER_CTRL_OUT_FIFO_64LINE		0x00C00000
#define C_SCALER_CTRL_OUT_FIFO_LINE_MASK	0x00C00000
#define	C_SCALER_CTRL_OUT_FIFO_INT			0x01000000
#define	C_SCALER_CTRL_RGB1555_TRANSPARENT	0x02000000
// Out-of-boundry color register
#define C_SCALER_OUT_BOUNDRY_COLOR_MAX		0x00FFFFFF

// Output width and height registers
#define C_SCALER_OUT_WIDTH_MAX				0x00001FFF		// Maximum 8191 pixels
#define C_SCALER_OUT_HEIGHT_MAX				0x00001FFF		// Maximum 8191 pixels

// Scaler factor registers
#define C_SCALER_X_FACTOR_MAX				0x00FFFFFF
#define C_SCALER_Y_FACTOR_MAX				0x00FFFFFF

// Scaler input x/y start offset registers
#define C_SCALER_X_START_MAX				0x3FFFFFFF
#define C_SCALER_Y_START_MAX				0x3FFFFFFF

/* Scaler out x offset registers */
#define C_SCALER_OUT_X_OFFSET_MAX			0x00001FFF

// Input width and height registers
#define C_SCALER_IN_WIDTH_MAX				0x00001FFF		// Maximum 8191 pixels
#define C_SCALER_IN_HEIGHT_MAX				0x00001FFF		// Maximum 8191 pixels

// Input width and height registers
#define C_SCALER_IN_VISIBLE_WIDTH_MAX		0x00001FFF		// Maximum 8191 pixels
#define C_SCALER_IN_VISIBLE_HEIGHT_MAX		0x00001FFF		// Maximum 8191 pixels

// Interrupt flag register
#define	C_SCALER_INT_PEND					0x00000001
#define	C_SCALER_INT_DONE					0x00000002
#define	C_SCALER_INT_OUT_FULL				0x00000004

// Post effect control register
#define	C_SCALER_HISTOGRAM_EN				0x00000001
#define	C_SCALER_Y_GAMMA_EN					0x00000002
#define	C_SCALER_COLOR_MATRIX_EN			0x00000004
#define	C_SCALER_INTERNAL_LINE_BUFFER		0x00000000
#define	C_SCALER_HYBRID_LINE_BUFFER			0x00000010
#define	C_SCALER_EXTERNAL_LINE_BUFFER		0x00000020
#define	C_SCALER_LINE_BUFFER_MODE_MASK		0x00000030

#define C_SCALER_STATUS_INPUT_EMPTY			0x00000001
#define C_SCALER_STATUS_BUSY				0x00000002
#define C_SCALER_STATUS_DONE				0x00000004
#define C_SCALER_STATUS_STOP				0x00000008
#define C_SCALER_STATUS_TIMEOUT				0x00000010
#define C_SCALER_STATUS_INIT_ERR			0x00000020
#define C_SCALER_STATUS_OUTPUT_FULL			0x00000040

// Backward compatible to old scaler driver
#define C_SCALER_CTRL_FIFO_DISABLE			C_SCALER_CTRL_IN_FIFO_DISABLE
#define C_SCALER_CTRL_FIFO_16LINE			C_SCALER_CTRL_IN_FIFO_16LINE
#define C_SCALER_CTRL_FIFO_32LINE			C_SCALER_CTRL_IN_FIFO_32LINE
#define C_SCALER_CTRL_FIFO_64LINE			C_SCALER_CTRL_IN_FIFO_64LINE
#define C_SCALER_CTRL_FIFO_128LINE			C_SCALER_CTRL_IN_FIFO_128LINE
#define C_SCALER_CTRL_FIFO_256LINE			C_SCALER_CTRL_IN_FIFO_256LINE
//#define scaler_fifo_line_set				scaler_input_fifo_line_set

// scaler semaphore lock
extern void scaler_lock(void);
extern void scaler_unlock(void);

// Scaler init API
extern void scaler_init(void);

// Scaler config APIs
extern INT32S scaler_image_pixels_set(INT32U input_x, INT32U input_y, INT32U output_x, INT32U output_y);		// 1~8000 pixels

extern INT32S scaler_input_pixels_set(INT32U input_x, INT32U input_y);				// 1~8000 pixels, including the padding pixels
extern INT32S scaler_input_visible_pixels_set(INT32U input_x, INT32U input_y);		// 1~8000 pixels, not including the padding pixels
extern INT32S scaler_input_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Must be 4-byte alignment
extern INT32S scaler_input_format_set(INT32U format);								// C_SCALER_CTRL_IN_RGB1555/C_SCALER_CTRL_IN_RGB565/C_SCALER_CTRL_IN_RGBG/C_SCALER_CTRL_IN_GRGB/C_SCALER_CTRL_IN_YUYV/C_SCALER_CTRL_IN_UYVY/C_SCALER_CTRL_IN_YUV422/C_SCALER_CTRL_IN_YUV420/C_SCALER_CTRL_IN_YUV411/C_SCALER_CTRL_IN_YUV444/C_SCALER_CTRL_IN_Y_ONLY/C_SCALER_CTRL_IN_YUV422V/C_SCALER_CTRL_IN_YUV411V
extern INT32S scaler_input_offset_set(INT32U offset_x, INT32U offset_y);			// Set scaler start x and y position offset
extern INT32S scaler_output_offset_set(INT32U x_out_offset);

extern INT32S scaler_output_pixels_set(INT32U factor_x, INT32U factor_y, INT32U output_x, INT32U output_y);		// factor_x:(input_x<<16)/output_x (1~0x00FFFFFF), factor_y:(input_y<<16)/output_y, output_x: must be multiple of 16 when output format is YUV422/YUV420/YUV444, multiple of 32 when output format is YUV411, multiple of 8 for others(Maximum 2040 for YUYV8X32 and YUYV8X64, Maximum 4088 for YUYV32X32 and YUYV32X64, Maximum 8000 pixels for others), output_y: 1~8000 pixels
extern INT32S scaler_output_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Must be 4-byte alignment
extern INT32S scaler_output_format_set(INT32U format);								// C_SCALER_CTRL_OUT_RGB1555/C_SCALER_CTRL_OUT_RGB565/C_SCALER_CTRL_OUT_RGBG/C_SCALER_CTRL_OUT_GRGB/C_SCALER_CTRL_OUT_YUYV/C_SCALER_CTRL_OUT_UYVY/C_SCALER_CTRL_OUT_YUYV8X32/C_SCALER_CTRL_OUT_YUYV8X64/C_SCALER_CTRL_OUT_YUYV16X32/C_SCALER_CTRL_OUT_YUYV16X64/C_SCALER_CTRL_OUT_YUYV32X32/C_SCALER_CTRL_OUT_YUYV64X64/C_SCALER_CTRL_OUT_YUV422/C_SCALER_CTRL_OUT_YUV420/C_SCALER_CTRL_OUT_YUV411/C_SCALER_CTRL_OUT_YUV444/C_SCALER_CTRL_OUT_Y_ONLY

extern INT32S scaler_fifo_line_set(INT32U mode);									// C_SCALER_CTRL_FIFO_DISABLE/C_SCALER_CTRL_FIFO_16LINE/C_SCALER_CTRL_FIFO_32LINE/C_SCALER_CTRL_FIFO_64LINE/C_SCALER_CTRL_FIFO_128LINE/C_SCALER_CTRL_FIFO_256LINE
extern INT32S scaler_input_fifo_line_set(INT32U mode);								// C_SCALER_CTRL_IN_FIFO_DISABLE/C_SCALER_CTRL_IN_FIFO_16LINE/C_SCALER_CTRL_IN_FIFO_32LINE/C_SCALER_CTRL_IN_FIFO_64LINE/C_SCALER_CTRL_IN_FIFO_128LINE/C_SCALER_CTRL_IN_FIFO_256LINE
extern INT32S scaler_output_fifo_line_set(INT32U mode);								// C_SCALER_CTRL_OUT_FIFO_DISABLE/C_SCALER_CTRL_OUT_FIFO_16LINE/C_SCALER_CTRL_OUT_FIFO_32LINE/C_SCALER_CTRL_OUT_FIFO_64LINE
extern INT32S scaler_YUV_type_set(INT32U type);										// C_SCALER_CTRL_TYPE_YUV/C_SCALER_CTRL_TYPE_YCBCR
extern INT32S scaler_out_of_boundary_mode_set(INT32U mode);							// mode: 0=Use the boundary data of the input picture, 1=Use Use color defined in scaler_out_of_boundary_color_set()
extern INT32S scaler_out_of_boundary_color_set(INT32U ob_color);					// The format of ob_color is Y-Cb-Cr
extern INT32S scaler_line_buffer_mode_set(INT32U mode);								// mode: C_SCALER_INTERNAL_LINE_BUFFER/C_SCALER_HYBRID_LINE_BUFFER/C_SCALER_EXTERNAL_LINE_BUFFER
extern INT32S scaler_external_line_buffer_set(INT32U addr);							// Must be 4-byte alignment

// Scaler special mode for Motion-JPEG
extern INT32S scaler_bypass_zoom_mode_enable(void);

// Scaler start, restart and stop function APIs
extern INT32S scaler_start(void);
extern INT32S scaler_restart(void);
extern void scaler_stop(void);

// Scaler device protection APIs, used by Montion-JPEG in JPEG driver
extern INT32S scaler_device_protect(void);
extern void scaler_device_unprotect(INT32S mask);

// Scaler status polling APIs
extern INT32S scaler_wait_idle(void);
extern INT32S scaler_status_polling(void);

// Gamma, color matrix and line buffer mode control
extern INT32S scaler_gamma_switch(INT8U gamma_switch);
extern void scaler_Y_gamma_config(INT8U gamma_table_id, INT8U gain_value);
extern INT32S scaler_color_matrix_switch(INT8U color_matrix_switch);
extern INT32S scaler_color_matrix_config(COLOR_MATRIX_CFG *color_matrix);


#endif		// __drv_l1_SCALER_H__