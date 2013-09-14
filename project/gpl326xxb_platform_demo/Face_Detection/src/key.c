/*
 ******************************************************************************
 * key.c
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.06.06	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "application.h"

#include "video_codec_callback.h"
#include "FaceDetectKernel.h"
#include "FaceDetectAP.h"
#include "FaceIdentifyAP.h"

#include "fd.h"
#include "frdbm.h"
#include "frio.h"
#include "serial.h"
#include "key.h"

/*
 * for debug
 */
#define TRACE_NONE		0x00000000UL
#define TRACE_MSG		0x00000001UL
#define TRACE_KEY		0x00000002UL
#define TRACE_MODE		0x00000004UL

#define DRDBM_DEBUG		( \
	TRACE_MSG		| \
	TRACE_KEY		| \
	TRACE_MODE		| \
	TRACE_NONE)

#if (DRDBM_DEBUG & TRACE_MSG)
    #define _MSG(x)		(x)
#else
    #define _MSG(x)
#endif
#if (DRDBM_DEBUG & TRACE_KEY)
    #define _KEY(x)		(x)
#else
    #define _KEY(x)
#endif
#if (DRDBM_DEBUG & TRACE_MODE)
    #define _MODE(x)		(x)
#else
    #define _MODE(x)
#endif


/*
 *****************************************************************************
 *
 * mode
 *
 *****************************************************************************
 */
void fd_chg_mode(int mode)
{
	int	i, n;


	switch (mode) {
	case MODE_TRAIN:
		face_verify_flag = 0;
		if (train_counter == 0) {
			_MODE(DBG_PRINT("(Train) Start\r\n"));
		} else {
			_MODE(DBG_PRINT("(Train) Restart\r\n"));
		}
		sensor_frame  = 0;
		train_counter = 1;
		ownerULBP     = (INT32U *) frdb_get_c_train();
		frio_set(FRIO_GPIO0, FRIO_GPIO0|FRIO_GPIO1);
		break;

	case MODE_IDENT:
		train_counter    = 0;
		if (face_verify_flag == 0) {
			_MODE(DBG_PRINT("(Identify) Start\r\n"));
		} else {
			_MODE(DBG_PRINT("(Identify) Restart\r\n"));
		}
		verify_fail	 = 0;
		verify_pass	 = 0;
		face_verify_flag = 1;
		ownerULBP	 = (INT32U *) frdb_get_c_ident();
		frio_set(FRIO_GPIO1, FRIO_GPIO0|FRIO_GPIO1);
		break;

	case MODE_STANDBY:
	default:
		_MODE(DBG_PRINT("(Standby) Start\r\n"));
		face_verify_flag = 0;
		sensor_frame     = 0;
		train_counter    = 0;
		verify_fail	 = 0;
		verify_pass	 = 0;

		frio_set(FRIO_GPIO0|FRIO_GPIO1, FRIO_GPIO0|FRIO_GPIO1);

		for (i=0, n=0; i<FRDB_NUM; i++) {
			if (frdb_is_valid(i)) {
				n++;
			}
		}
		if (n) {
			frio_hi(FRIO_GPIO4);
		} else {
			frio_lo(FRIO_GPIO4);
		}
		break;
	}
}

/*
 *****************************************************************************
 *
 * key event
 *
 *****************************************************************************
 */
void fd_demo(void)
{
	VIDEO_ARGUMENT	arg;
    	int		memSize;
	int		c;
	unsigned char	s;
	int		adkey1 = 0;
	int		adkey2 = 0;
	int		adkey3 = 0;
	int		adkey4 = 0;
	int		adkey5 = 0;


	// Create a user-defined task
	frio_init(30);
	serial_init(41);

	fident_init(36);
	ftrain_init(37);


	// Initialize display device
//	tv_init();
//	tv_start (TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_INTERLACE);

	tft_init();
	tft_start(TPO_TD025THD1);

	user_defined_video_codec_entrance();
	video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder

	arg.bScaler		= 1;				// must be 1
	arg.TargetWidth		= C_DISPLAY_DEV_HPIXEL;		//encode width
	arg.TargetHeight	= C_DISPLAY_DEV_VPIXEL;		//encode height
	arg.SensorWidth		= C_DISPLAY_DEV_HPIXEL;		//sensor input width
	arg.SensorHeight	= C_DISPLAY_DEV_VPIXEL;		//sensor input height
	arg.DisplayWidth	= C_DISPLAY_DEV_HPIXEL;		//display width
	arg.DisplayHeight	= C_DISPLAY_DEV_VPIXEL;		//display height
	arg.DisplayBufferWidth	= C_DISPLAY_DEV_HPIXEL;		//display buffer width
	arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;		//display buffer height
	arg.VidFrameRate	= 30;				//video encode frame rate
	arg.AudSampleRate	= 8000;				//audio record sample rate
	arg.OutputFormat	= DISPLAY_OUTPUT_FORMAT;	//display output format
	video_encode_preview_start(arg);
	Face_Detect_Demo_En	= 1;

	memSize	  = FaceIdentify_MemCalc();
	fiMem	  = (INT32U *) gp_malloc_align((memSize), 8);
//	ownerULBP = (INT32U *) gp_malloc_align((1888*20), 8);
	adjustSecurity_init(3);
	adc_key_scan_init();					 //init key scan

	frdb_init(3*1024*1024);
	ownerULBP = (INT32U *) frdb_get_valid();

	fd_chg_mode(MODE_STANDBY);

	while (1) {
		adc_key_scan();

		/* training */
		if (ADKEY_IO1) {
			if (adkey1 == 0) {
				adkey1++;
				frdb_set_c_train(-1);
				fd_chg_mode(MODE_TRAIN);
			}
		} else {
			adkey1 = 0;
		}

		/* identify */
		if (ADKEY_IO2) {
			if (adkey2 == 0) {
				adkey2++;
				frdb_set_c_ident(-1);
				fd_chg_mode(MODE_IDENT);
			}
		} else {
			adkey2 = 0;
		}

		/* security dec. */
		if (ADKEY_IO3) {
			if (adkey3 == 0) {
				adkey3++;
				adjustSecurity_set(0);
			}
		} else {
			adkey3 = 0;
		}

		/* security inc. */
		if (ADKEY_IO4) {
			if (adkey4 == 0) {
				adkey4++;
				adjustSecurity_set(1);
			}
		} else {
			adkey4 = 0;
		}

		/* erase */
		if (ADKEY_IO5) {
			if (adkey5 == 0) {
				adkey5++;
				c = 0xFF;
				s = (unsigned char) (c & ~CMD_MASK);
				for (c=0; c<FRDB_NUM; c++) {
					if (s & 0x08) {
						frdb_erase(c);
					}
					s >>= 1;
				}
				fd_chg_mode(MODE_STANDBY);
			}
		} else {
			adkey5 = 0;
		}
	}
}
