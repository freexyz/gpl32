/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    UVCUSER.H
 *      Purpose: USB Video Device Class User module Definitions
 *      Version: V1.00
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __UVCUSER_H__
#define __UVCUSER_H__

/* UVC Requests Callback Functions */
extern BOOL UVC_SetCur  (void);
extern BOOL UVC_GetCur  (void);
extern BOOL UVC_GetMin  (void);
extern BOOL UVC_GetMax  (void);
extern BOOL UVC_GetRes  (void);
extern BOOL UVC_GetLen  (void);
extern BOOL UVC_GetInfo (void);
extern BOOL UVC_GetDef  (void);


#endif  /* __UVCUSER_H__ */

