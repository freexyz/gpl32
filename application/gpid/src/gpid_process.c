//***************************************************************************************** 
// Topic: GPID process
// Plateform: 
// Description: 
// Note:
//*****************************************************************************************
#include "application.h"
#include "res_bin_file_mgr.h"
#include "game_mode.h"
#include "gpid_audio.h"
#include "Book_Select.h"

//=============================================================================
// Definition
//=============================================================================
// GPID-related parameters which are defined in the boot hdr of GPID101A
#define C_GPID_PARA_DECODE_FAIL_THRESHOLD	3

// For warn sound
#define C_WS_WARN_TYPE_NO_MATCHING_BOOK				0
#define C_WS_WARN_TYPE_NEED_TO_SELECT_BOOK_FIRST	1

//=============================================================================
// Global variables
//=============================================================================
INT8U cnt_GpidDecodeFail;
BOOLEAN FirstPlaySoundFlag;		//When player play sound first, this flag is TRUE
BOOLEAN EntrySE_Flag;			//Entry System Event. This Flag is TRUE

//=============================================================================
// Static variables
//=============================================================================
static INT32U gRecogInterval;		// Decode every (gRecogInterval/1024) seconds
static BOOLEAN gIsRecogTimeout;		// 1 if a new recognition needs to start
static BOOLEAN gIsAlwaysRecog;		// TRUE if we don't want any idle time between recognitions
static INT32U gLastGpidCode;		// The last recognized GPID code
static INT32U gDecFailCnt;
extern INT32U gSPGpidCode[];
extern INT16U gSPCode[];

//=============================================================================
// Local functions
//=============================================================================
static void HandleDecodeDone(INT32U code);
static void HandleDecodeFail(void);
static void PlayWarnSound(INT8U idx);

//========================================================
// Purpose   : Initialize GPID processs related data
// Note      :
// Parameter : None
// Return    : None
//========================================================
void GpidProc_Init(void)
{
	gRecogInterval = 0;
	gIsRecogTimeout = FALSE;
	gIsAlwaysRecog = FALSE;
	gLastGpidCode = C_GPID_INVALID_GPID_CODE;
	gDecFailCnt = 0;
	FirstPlaySoundFlag = FALSE;
	EntrySE_Flag = FALSE;

#if ENABLE_BOOK_SELECTION
	BS_EnableBookSelect(TRUE);
	BS_SetBookCodeRange((INT32U)(9*1024UL*1024UL), (INT32U)(15*1024UL*1024UL-1));
	BS_SetContentCodeRange(0, (INT32U)(9*1024UL*1024UL-1));
#else
	BS_EnableBookSelect(FALSE);
	BS_SetContentCodeRange(0, (INT32U)(9*1024UL*1024UL-1));
#endif
}

//========================================================
// Purpose   : Control all GPID flow so that it can be used in the main loop in main()
// Note      :
// Parameter : None
// Return    : None
//========================================================
void gpid_process(void)
{
	long l_ret;
	INT32U code;

	//======================== For game mode ==================================
	//system event priority is higer than game mode
	if( gstSYSEvnState.Mode==SE_PLAY && gpid_audio_get_status()==C_GPID_AUDIO_STATUS_STOP )
		gstSYSEvnState.Mode = SE_IDLE;

	if( gstSYSEvnState.Mode == SE_QUE && gstGMState.Mode!=GM_PLAY )
		ResBinFile_HandleSystemEvent(gstSYSEvnState.QueEvent);

	if( gstGMTMState.TimeOutFlag==TRUE )
	{
		gstGMTMState.TimeOutFlag=FALSE;
		gstGMState.Mode = GM_RUN;
	}
	if( gstGMState.Mode==GM_PLAY && gpid_audio_get_status()==C_GPID_AUDIO_STATUS_STOP )
	{
		if( gstSYSEvnState.Mode==SE_QUE )
		{
			gstGMState.Mode = GM_RUN;
			ResBinFile_HandleSystemEvent(gstSYSEvnState.QueEvent);
		}
		else
			gstGMState.Mode = GM_RUN;
	}
	if( gstGMState.Mode==GM_RUN && (gstSYSEvnState.Mode==SE_IDLE || gstSYSEvnState.Mode==SE_RUNGM) )
		Run_GameMode(-1);
	//=========================================================================

	if (gIsRecogTimeout || gIsAlwaysRecog)
	{
		l_ret = API_GpidService(&code);
		switch (l_ret)
		{
		case C_GPID_SERVICE_DECODE_DONE:
			HandleDecodeDone(code);
			
			cnt_GpidDecodeFail = 0;
			gIsRecogTimeout = FALSE;	// End this decode cycle
			break;

		case C_GPID_SERVICE_IGNORE:
			break;

		// In case of minor errors, retry.
		// Minor errors are usually due to AE adjustment.
		case C_GPID_SERVICE_MINOR_DECODE_ERROR:
			cnt_GpidDecodeFail += 1;
			if (cnt_GpidDecodeFail > 2)
			{
				cnt_GpidDecodeFail = 0;
				gIsRecogTimeout = FALSE;
			}
			break;

		// In case of minor errors, give up
		case C_GPID_SERVICE_MAJOR_DECODE_ERROR:
			cnt_GpidDecodeFail = 0;
			gIsRecogTimeout = FALSE;	// End this decode cycle
			HandleDecodeFail();
			break;

		default:
			gIsRecogTimeout = FALSE;	// End this decode cycle
			break;
		}
	}
}

#if C_RBF_SUPPORT_RBF_V1
//=============================================================================
// Note
//=============================================================================
void
gpid_process_v1(void)
{
	long l_ret;
	unsigned int NewSong;
	unsigned long	R_LastBarcode;			//used for debounce
	INT32U code;

	if (gIsRecogTimeout || gIsAlwaysRecog)
	{
		l_ret = API_GpidService(&code);
		switch (l_ret)	
		{
			case C_GPID_SERVICE_DECODE_DONE:	//barcode function
					//R_LastBarcode = code;

#ifdef C_OID_VERSION1
					// GPID1 code is only 15-bit
					code &= 0x00007FFF;
#endif
				    if ((code >= RBF_GetGpidStartCode()) && (code <= RBF_GetGpidEndCode()))
					{
						NewSong = code - RBF_GetGpidStartCode();

						// If the latest code is the same with the last one and the audio is still playing
						// then skip this action.
						if ( (gLastGpidCode == NewSong) && (gpid_audio_get_status() != C_GPID_AUDIO_STATUS_STOP) )
						{
							gIsRecogTimeout = FALSE;
							break;
						}

						gLastGpidCode = NewSong;
						gpid_audio_stop();
						RBF_SetCurrRcIdx(NewSong);
						gpid_audio_play();
					}

					cnt_GpidDecodeFail = 0;
					gIsRecogTimeout = FALSE;
					R_LastBarcode = code;
					break;
					
			case C_GPID_SERVICE_MINOR_DECODE_ERROR:			//decode get error
					cnt_GpidDecodeFail += 1;
					R_LastBarcode = 0;
					if (cnt_GpidDecodeFail > 2)
					{
						cnt_GpidDecodeFail = 0;
						gIsRecogTimeout = FALSE;
					}
					break;
					
			case C_GPID_SERVICE_MAJOR_DECODE_ERROR:		//nothing
					cnt_GpidDecodeFail = 0;
					R_LastBarcode = 0;
					gIsRecogTimeout = FALSE;
			case C_GPID_SERVICE_IGNORE: 	//continue capture image
					break;
			
			default:
					gIsRecogTimeout = FALSE;
					break;
		}
	}
}
#endif

//========================================================
// Purpose   : Callback when recognition interval times out.
// Note      :
// Parameter : None
// Return    : None
//========================================================
void recog_interval_timer_isr(void)
{
	static INT32U cnt = 0;

	cnt++;

	if (cnt > gRecogInterval)
	{
		gIsRecogTimeout = TRUE;
		cnt = 0;
	}
}

//========================================================
// Purpose   : Set GPID recognition rate.
//========================================================
void GpidProc_SetRecogRate(INT32U rate)
{
	if (rate != 0)
	{
		gIsAlwaysRecog = FALSE;

		gRecogInterval = 128/rate;
		sys_registe_timer_isr(recog_interval_timer_isr);
	}
	else
	{
		// rate = 0 means user wants to do GPID decoding at the fastest rate
		gIsRecogTimeout = TRUE;
		gIsAlwaysRecog = TRUE;
	}
}

//========================================================
// Note:
//========================================================
static void HandleDecodeDone(INT32U code)
{
	INT32S ret, ret1;
	INT8U play_state;
#if C_RBF_SUPPORT_RBF_V2
	INT32U i, SpSize;
#endif
#if C_RBF_SUPPORT_RBF_V3
	INT32U obj_offset, OPCodeStart;
#endif

	// Game mode is always triggered by a GPID code and thus can be initialized upon the first successful decoding.
	// Another purpose is to let the randomization seed in GameModeInit() be random enough since the seed is based
	// on software timer and the time at which user clicks the first code is random.
	//				by Craig Yang 12/16/2010
	if (!IsGameModeInitDone())
		GameModeInit();

	if( code==gLastGpidCode )
		return;
	
	gDecFailCnt = 0;

	//If system event occur
	//if( gstSYSEvnState.Mode==SE_PLAY )		//Marking this code to make System Evnet can enter
	//	return;
		
	play_state = gpid_audio_get_status();
	
	//Check if need to Select Book
	if( BS_IsBookSelectEnabled() )
	{
		//check if gpid code is book code or not
		if ( BS_GetCodeType(code) == C_BS_CODE_TYPE_BOOK)
		{
			// 1. Don't waste time to open the same book code twice
			// 2. Two consecutive decode results are the same and the previous SYS_BOOK_CHANGE
			//    sound will be stopped by the subsequent CtrlWrap_Sound_Stop() which does not
			//    make sense.
			//																					by Craig Yang 11/07/2010
			
			if (code == BS_GetCurrBookCode())
			{
			//xx for test
			//xx To test if we can play audio for a long time, return only when the sound is being played
			//xx and put the sensor on a book code to play the BOOK_CHANGE sound repeatedly.
			//	if (play_state == C_PLAY_STATE_PLAYING)
			//		return;

				return;
			}
			

			//find new book
			ret = RBF_FindBinByBookCode(code, C_RBF_FB_FIND_NEW_ONLY);
			if ( ret != C_RBF_RC_SUCCESS )
			{
				PlayWarnSound(C_WS_WARN_TYPE_NO_MATCHING_BOOK);
				return;
			}

			//Stop sound and clear game state
			//
			// The old RBF will be closed soon so we have to stop sound
			// playing to prevent reading sound data from the old RBF.
			gpid_audio_stop();
			ClearGMState();
			ClearGMTMState();

			RBF_Close();
			RBF_FindBinByBookCode(code, C_RBF_FB_FIND_NEW_AND_CLOSE_OLD);
			ret1 = RBF_Open(0);
			if (ret1 == C_RBF_RC_SUCCESS)
			{
				gpid_audio_init(RBF_GetFd());
				BS_SetCurrBookCode(code);
				ResBinFile_HandleSystemEvent(SYS_BOOK_CHANGE);
			}
			else
			{
				ret1 = C_RBF_RC_FAIL;
			}
			return;
		}
	}
	
	//if no book exist
	if(!RBF_IsBookExist())
	{
		PlayWarnSound(C_WS_WARN_TYPE_NEED_TO_SELECT_BOOK_FIRST);
		return;
	}

	// Check if code is the same as last one after making sure that
	// the code is not a book code. At this point, the res bin file
	// is opened and its version is known.
#if C_RBF_SUPPORT_RBF_V2
	if (RBF_GetVersion() == 0x02)
	{
		if( code != gLastGpidCode )
			gLastGpidCode=code;
		else
			return;
	}
#endif

#if C_RBF_SUPPORT_RBF_V3
	if (RBF_GetVersion() == 0x03)
	{
		// In v3(GPID4u), many different coordinates might correspond to the same object offset.
		// We don't have to run the object code unless the object offset is different from the previous one.
		ret = RBF_GetStartObj_DontMoveFd(code, &obj_offset);
		if ((ret == C_RBF_RC_SUCCESS) && (obj_offset != 0xFFFFFFFF) && (obj_offset != gLastGpidCode))
			gLastGpidCode=obj_offset;
		else
			return;
	}
#endif

	//check if gpid code is special code
#if C_RBF_SUPPORT_RBF_V2
	if ( RBF_GetVersion() == 0x02)
	{
		SpSize = RBF_GetSpCodeSize();
		if( SpSize!=0 )
		{
			for( i=0; i<SpSize/4; i++ )
			{
				if( code==gSPGpidCode[i] )
				{
					GM_HandleSpCode(gSPCode[i]);
					return;
				}
			}
		}
	}
#endif

#if C_RBF_SUPPORT_RBF_V3
	if ( RBF_GetVersion() == 0x03)
	{
		if (RBF_IsSupportSysEvent())
		{
			// The MSB of the code's start offset in block event table is 1, the remaining 31 bits represent special code.
			ret = RBF_GetStartObj_DontMoveFd( code, &OPCodeStart);
			if( OPCodeStart!=0xFFFFFFFF && (OPCodeStart&0x80000000) )
			{
				GM_HandleSpCode((INT16U)(OPCodeStart&0x7FFFFFFF));
				return;
			}
		}
	}
#endif

	if(gstSYSEvnState.Mode==SE_PLAY && gstGMState.RetriggerFlag==TRUE)           //Valleys, When Volume Up/Down System Event Sound is interrupt and GMState is GMDelay or GMRun     
		gstGMState.Mode = GM_IDLE;                                               //Reset GMState 

	if (
		(gstGMState.Mode == GM_WAIT) ||
#if C_RBF_SUPPORT_RBF_V2
		((RBF_GetVersion() == 0x02) && (gstGMState.RetriggerFlag==TRUE) && (code != gstGMState.CurrGPIDCode)) ||
#endif
#if C_RBF_SUPPORT_RBF_V3
		((RBF_GetVersion() == 0x03) && (gstGMState.RetriggerFlag==TRUE) && (obj_offset != gstGMState.CurrGPIDCode)) ||
#endif
		(gstGMState.RetriggerFlag==FALSE && (gstGMState.Mode == GM_IDLE))
	)
	{
		if(gstGMState.Mode!=GM_WAIT)
		{
			gpid_audio_to_n(code);
			ret = C_RBF_RC_SUCCESS;
		}
		else
			ret = C_RBF_RC_SUCCESS;
			
		if (ret == C_RBF_RC_SUCCESS)
		{
#ifdef UART_DBG_EN
			DBG_PRINT("\nGPID code %lu decoded.\r\n", code);
#endif
			Run_GameMode(code);
		}
	}
}

//========================================================
// Note:
//========================================================
static void HandleDecodeFail(void)
{
	gDecFailCnt++;
	if(gDecFailCnt>=C_GPID_PARA_DECODE_FAIL_THRESHOLD)
	{
		gLastGpidCode = C_GPID_INVALID_GPID_CODE;
		gDecFailCnt=0;
		gstGMState.CurrGPIDCode = C_GPID_INVALID_GPID_CODE; //for retrigger self
	}
}

//========================================================
// Note:
//========================================================
static void PlayWarnSound(INT8U idx)
{
	// [TODO] Need to play PCM sound. Not supported yet.
}