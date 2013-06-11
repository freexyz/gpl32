#ifndef __GPID_PROCESS_H_
#define __GPID_PROCESS_H_

//======================================================
// Define
//======================================================

//======================================================
// Function declaration
//======================================================
extern void GpidProc_Init(void);
extern void gpid_process(void);
extern void GpidProc_SetRecogRate(INT32U rate);
#if C_RBF_SUPPORT_RBF_V1
extern void gpid_process_v1(void);
#endif

#endif