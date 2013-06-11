/****************************************************************/
/*                                                              */
/*                            cds.h                             */
/*                                                              */
/*                  Current Directory structures                */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/****************************************************************/
#ifndef __CDS_H__
#define __CDS_H__

#if LFN_FLAG == 1
#define MAX_CDSPATH FD32_LFNPMAX
#else
#define MAX_CDSPATH 67
#endif

struct cds {
#if WITH_CDS_PATHSTR == 1
  INT8S cdsCurrentPath[MAX_CDSPATH]; 
#endif
  INT16U cdsFlags;           /* see below */
  struct dpb *cdsDpb;   /* if != 0, associated DPB */
  CLUSTER cdsStrtClst;   
};

/* Bits for cdsFlags (OR combination)                           */
#define CDSNETWDRV      0x8000
#define CDSPHYSDRV      0x4000
#define CDSJOINED       0x2000 /* not in combination with NETWDRV or SUBST */
#define CDSSUBST        0x1000 /* not in combination with NETWDRV or JOINED */
#define CDS_HIDDEN     (1 << 7)    /* hide drive from redirector's list */

/* NETWORK PHYSICAL        meaning
   0       0               drive not accessable
   0       1               local file system
   1       0               networked file system (UNC naming convention)
   1       1               installable file system (IFS)
*/
#define CDSMODEMASK        (CDSNETWDRV | CDSPHYSDRV)
 
/* #define CDSVALID        (CDSNETWDRV | CDSPHYSDRV) */
#define CDSVALID CDSMODEMASK

#endif
