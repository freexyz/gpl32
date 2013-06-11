#ifndef __ERROR_H__
#define __ERROR_H__

/* Internal system error returns                                */
#define SUCCESS         0       /* Function was successful      */
#define DE_INVLDFUNC    -1      /* Invalid function number      */
#define DE_FILENOTFND   -2      /* File not found               */
#define DE_PATHNOTFND   -3      /* Path not found               */
#define DE_TOOMANY      -4      /* Too many open files          */
#define DE_ACCESS       -5      /* Access denied                */
#define DE_INVLDHNDL    -6      /* Invalid handle               */
#define DE_MCBDESTRY    -7      /* Memory control blocks shot   */
#define DE_NOMEM        -8      /* Insufficient memory          */
#define DE_INVLDMCB     -9      /* Invalid memory control block */
#define DE_INVLDENV     -10     /* Invalid enviornement         */
#define DE_INVLDFMT     -11     /* Invalid format               */
#define DE_INVLDACC     -12     /* Invalid access               */
#define DE_INVLDDATA    -13     /* Invalid data                 */
#define DE_INVLDDRV     -15     /* Invalid drive                */
#define DE_RMVCUDIR     -16     /* Attempt remove current dir   */
#define DE_DEVICE       -17     /* Not same device              */
#define DE_MAX_FILE_NUM       -18     /* No more files                */
#define DE_WRTPRTCT     -19     /* No more files                */
#define DE_BLKINVLD     -20     /* invalid block                */
#define DE_INVLDBUF     -24     /* invalid buffer size, ext fnc */
#define DE_SEEK         -25     /* error on file seek           */
#define DE_HNDLDSKFULL  -28     /* handle disk full (?)         */
#define DE_INVLDPARM    -87     /* invalid parameter			*/
#define DE_UNITABERR    -88     /* unitab error					*/
#define DE_TOOMANYFILES	-89		/* to many files				*/

#define DE_DEADLOCK	-36
#define DE_LOCK		-39

#define DE_INVLDCDFILE	-48		/* invalid cd file name */	//by wq add 2004.11.18
#define DE_NOTEMPTY		-49		/* DIR NOT EMPTY */	//by yongliang add 2004.11.18
#define DE_ISDIR		-50		/* Is a directory name          */  // 2004-11-19 Yongliang add
#define DE_FILEEXISTS   -80		/* File exists                  */
#define DE_DEVICEBUSY	-90
#define DE_NAMETOOLONG	-100	/* specified path name too long */	// 2005-1-11 Yonglianga add
#define DE_FILENAMEINVALID -110	/* Invalid */	// 2005-1-11 Yongliang add

/* Critical error flags                                         */
#define EFLG_READ       0x00    /* Read error                   */
#define EFLG_WRITE      0x01    /* Write error                  */
#define EFLG_RSVRD      0x00    /* Error in rserved area        */
#define EFLG_FAT        0x02    /* Error in FAT area            */
#define EFLG_DIR        0x04    /* Error in dir area            */
#define EFLG_DATA       0x06    /* Error in data area           */
#define EFLG_ABORT      0x08    /* Handler can abort            */
#define EFLG_RETRY      0x10    /* Handler can retry            */
#define EFLG_IGNORE     0x20    /* Handler can ignore           */
#define EFLG_CHAR       0x80    /* Error in char or FAT image   */

/* error results returned after asking user                     */
/* MS-DOS compatible -- returned by CriticalError               */
#define CONTINUE        0
#define RETRY           1
#define ABORT           2
#define FAIL            3

#endif
