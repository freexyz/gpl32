/*
* Purpose: De-flicker driver
*
* Author: Tristan Yang
*
* Date: 2009/06/18
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*/

#include "drv_l1_deflicker.h"


#if (defined _DRV_L1_DEFLICKER) && (_DRV_L1_DEFLICKER == 1)

static INT32U deflicker_control_flag = 0;
static OS_EVENT *sem_deflicker_engine = NULL;
static OS_EVENT *sem_deflicker_done = NULL;
static OS_EVENT *deflicker_free_queue = NULL;
static OS_EVENT *deflicker_request_queue = NULL;
static void *deflicker_free_queue_buffer[C_DEFLICKER_DRV_MAX_REQUEST_NUM];
static void *deflicker_request_queue_buffer[C_DEFLICKER_DRV_MAX_REQUEST_NUM];
static DEFLICKER_REQUEST_STRUCT deflicker_request_struct[C_DEFLICKER_DRV_MAX_REQUEST_NUM];
static INT32U deflicker_current_frame = 0;
static OS_EVENT *deflicker_reply_queue = NULL;

void deflicker_reg_fill(INT32U resolution, INT32U *input_buffer, INT32U *output_buffer, OS_EVENT *reply_queue);
INT32S deflicker_device_protect(void);
void deflicker_device_unprotect(INT32S mask);
void deflicker_isr(void);

void deflicker_init(void)
{
	INT8U error;
	INT8U i;

	if (deflicker_control_flag & C_DEFLICKER_CONTROL_INITIATED) {
		return;
	}
	
	vic_irq_disable(VIC_DFR);

	// Register De-flicker ISR
	vic_irq_register(VIC_DFR, deflicker_isr);

	R_DEFLICKER_CTRL = C_DEFLICKER_CTRL_RESET;			// Reset de-flicker engine
	R_DEFLICKER_PARA = 0x0321;							// Set section threshold
	R_DEFLICKER_INT = C_DEFLICKER_INT_PEND_DONE;		// Clear pending interrupt flag
	R_DEFLICKER_CTRL = C_DEFLICKER_CTRL_INT;			// Enable de-flicker interrupt

	deflicker_control_flag = C_DEFLICKER_CONTROL_INITIATED;
	if (sem_deflicker_engine) {
		OSSemSet(sem_deflicker_engine, 1, &error);
	} else {
		sem_deflicker_engine = OSSemCreate(1);
	}
	if (sem_deflicker_done) {
		OSSemSet(sem_deflicker_done, 0, &error);
	} else {
		sem_deflicker_done = OSSemCreate(0);
	}

	if (deflicker_free_queue) {
		OSQDel(deflicker_free_queue, OS_DEL_ALWAYS, (INT8U *) &i);
	}
	deflicker_free_queue = OSQCreate(&deflicker_free_queue_buffer[0], C_DEFLICKER_DRV_MAX_REQUEST_NUM);
	if (deflicker_request_queue) {
		OSQDel(deflicker_request_queue, OS_DEL_ALWAYS, (INT8U *) &i);
	}
	deflicker_request_queue = OSQCreate(&deflicker_request_queue_buffer[0], C_DEFLICKER_DRV_MAX_REQUEST_NUM);
	for (i=0; i<C_DEFLICKER_DRV_MAX_REQUEST_NUM; i++) {
		OSQPost(deflicker_free_queue, (void *) &deflicker_request_struct[i]);
	}

	vic_irq_enable(VIC_DFR);
}

// resolution: 0=QVGA, 1=VGA, 2=D1
// wait_mode: 0=No wait, 1=Wait in queue, 2=Wait until deflicker engine is available, 3=Wait until deflicker output buffer is done
INT32S deflicker_start(INT32U resolution, INT32U wait_mode, INT32U *input_buffer, INT32U *output_buffer, OS_EVENT *reply_queue)
{
	DEFLICKER_REQUEST_STRUCT *req_struct;
	INT8U error;

	if (!input_buffer || !output_buffer) {
		return -1;
	}

	if (!(deflicker_control_flag & C_DEFLICKER_CONTROL_INITIATED)) {
		deflicker_init();
	}

	// Obtain sem_deflicker_engine semaphore
	if (wait_mode == 0) {		// Return immediately when deflicker engine is busy
		if (!OSSemAccept(sem_deflicker_engine)) {
			return -1;
		}
	} else if (wait_mode == 1) {		// Wait in queue when deflicker engine is busy
		if (!OSSemAccept(sem_deflicker_engine)) {
			req_struct = (DEFLICKER_REQUEST_STRUCT *) OSQAccept(deflicker_free_queue, &error);
			if (!req_struct || error!=OS_NO_ERR) {
				return -1;
			}
			
			// Save this deflicker request in queue
			req_struct->resolution = resolution;
			req_struct->input_buffer = input_buffer;
			req_struct->output_buffer = output_buffer;
			req_struct->reply_queue = reply_queue;
			OSQPost(deflicker_request_queue, (void *) req_struct);
			
			return 0;
		}
	} else if (wait_mode==2 || wait_mode==3) {		// Wait until deflicker engine is not busy
		OSSemPend(sem_deflicker_engine, 0, &error);
		if (error != OS_NO_ERR) {
			return -1;
		}
	} else {
		return -1;
	}

	if (wait_mode == 3) {		// Wait until deflicker output buffer is done
		deflicker_control_flag |= C_DEFLICKER_CONTROL_WAIT_DONE;
	} else {
		deflicker_control_flag &= ~C_DEFLICKER_CONTROL_WAIT_DONE;
	}
	
	deflicker_reg_fill(resolution, input_buffer, output_buffer, reply_queue);

	if (wait_mode == 3) {		// Wait until deflicker output buffer is done
		OSSemPend(sem_deflicker_done, 0, &error);
	}

	return 0;
}

void deflicker_reg_fill(INT32U resolution, INT32U *input_buffer, INT32U *output_buffer, OS_EVENT *reply_queue)
{
	INT32U ctrl;
	
	deflicker_current_frame = (INT32U) output_buffer;
	deflicker_reply_queue = reply_queue;
	
	R_DEFLICKER_INPTRL = (INT32U) input_buffer;
	R_DEFLICKER_OUTPTRL = (INT32U) output_buffer;
	
	ctrl = R_DEFLICKER_CTRL & ~(C_DEFLICKER_CTRL_320_240|C_DEFLICKER_CTRL_640_480|C_DEFLICKER_CTRL_720_480);
	if (resolution == 1) {		// 640x480
		ctrl |= (C_DEFLICKER_CTRL_START | C_DEFLICKER_CTRL_640_480);
	} else if (resolution == 2) {		// 720x480
		ctrl |= (C_DEFLICKER_CTRL_START | C_DEFLICKER_CTRL_720_480);
	} else {		// 320x240
		ctrl |= (C_DEFLICKER_CTRL_START | C_DEFLICKER_CTRL_320_240);
	}
	R_DEFLICKER_CTRL = ctrl;
}


INT32S deflicker_device_protect(void)
{
	return vic_irq_disable(VIC_DFR);
}

void deflicker_device_unprotect(INT32S mask)
{
	if (mask == 0) {						// Clear device interrupt mask
		vic_irq_enable(VIC_DFR);
	} else if (mask == 1) {
		vic_irq_disable(VIC_DFR);
	} else {								// Something is wrong, do nothing
		return;
	}
}

void deflicker_isr(void)
{
	DEFLICKER_REQUEST_STRUCT *req_struct;
	INT8U error;
	
	R_DEFLICKER_INT = C_DEFLICKER_INT_PEND_DONE;

	if (deflicker_reply_queue) {
		OSQPost(deflicker_reply_queue, (void *) deflicker_current_frame);
		deflicker_reply_queue = NULL;
	}
	
	// If current task is waiting for deflicker operation to be done, notify it by setting this semaphore
	if (deflicker_control_flag & C_DEFLICKER_CONTROL_WAIT_DONE) {
		deflicker_control_flag &= ~C_DEFLICKER_CONTROL_WAIT_DONE;
		OSSemPost(sem_deflicker_done);
	}
	
	// Post sem_deflicker_engine semaphore to release deflicker engine here
	req_struct = (DEFLICKER_REQUEST_STRUCT *) OSQAccept(deflicker_request_queue, &error);
	if (req_struct && error==OS_NO_ERR) {
		deflicker_reg_fill(req_struct->resolution, req_struct->input_buffer, req_struct->output_buffer, req_struct->reply_queue);
		OSQPost(deflicker_free_queue, (void *) req_struct);
		
		return;
	}
	OSSemPost(sem_deflicker_engine);
}

#endif 		// _DRV_L1_DEFLICKER
