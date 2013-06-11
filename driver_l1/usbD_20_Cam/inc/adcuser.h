/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    ADCUSER.H
 *      Purpose: Audio Device Class Custom User Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __ADCUSER_H__
#define __ADCUSER_H__


/* Audio Device Class Requests Callback Functions */
extern BOOL ADC_IF_GetRequest (void);
extern BOOL ADC_IF_SetRequest (void);
extern BOOL ADC_EP_GetRequest (void);
extern BOOL ADC_EP_SetRequest (void);


#endif  /* __ADCUSER_H__ */
