#ifndef	__GAME_MODE_h__
#define	__GAME_MODE_h__

#include "application.h"

#define UART_DBG_EN 1

#define	GM_UNKNOWN	0
#define	GM_IDLE		1
#define	GM_RUN		2
#define GM_PLAY		3
#define	GM_WAIT		4
#define	GM_DELAY		5
#define	GM_END		6


#define SE_IDLE	0
#define	SE_RUNGM	1
#define	SE_PLAY	2
#define	SE_QUE	3

#define	SE_NO_EVENT		0
#define	SE_HAS_EVENT	1

#define C_OPCODE_MAX	0x23
#define	C_SPCODE_MAX	0x18

#define VAR_CNT 100

#define C_BOOKCODE_INVALID	(0xFFFFFFFF)

#define STACKEVENT	0
#define	STACKCALL	1
#define	STACKNUM	2

//----- Game mode definition -----
#define GMV_TIME_FLAG		0x0A
#define GMV_TIME_COUNT		0x0B	// unit:1 Second
#define GMV_TIME_LIMIT		0x0E	// unit:1 Second


typedef struct
{
	INT32U Mode:3;			// GMIdle~GMEnd
	INT32U RetriggerFlag:1;	// boolean
	INT32U CurrResPlay:16;
	INT32U Unused:12;
	INT32U OPCodeStart;
	INT32U CurrOPCode;
	INT32U CurrGPIDCode;
} stGMState;

typedef struct
{
	INT32U CountFlag:1;		// boolean
	INT32U TimeOutFlag:1;	// boolean
	INT32U Unused:30;
	INT32U Counter;
	INT32U TimeOutSet;
} stGMTMState;

typedef struct
{
	INT32U Mode:3;			// GMIdle~GMEnd
	INT32U RetriggerFlag:1;	// boolean
	INT32U CountFlag:1;		// boolean
	INT32U TimeOutFlag:1;	// boolean
	INT32U CurrResPlay:16;
	INT32U Unused:10;
	INT32U OPCodeStart;
	INT32U CurrOPCode;
	INT32U CurrGPIDCode;
	INT32U Counter;
	INT32U TimeOutSet;
} stVStack;

typedef struct
{
	INT32U Mode:3;		// SEIdle~SEQue
	INT32U QueEvent:4;	// 4-bit is enough for system event index
	INT32U Unused:25;
} stSYSEvnState;


extern stGMState gstGMState;
extern stGMTMState gstGMTMState;
extern stSYSEvnState gstSYSEvnState;
extern stVStack gstVStack[STACKNUM];

extern INT32U variable[VAR_CNT];

extern void GameModeInit(void);
extern BOOLEAN IsGameModeInitDone(void);
extern void ClearGMTMState(void);
extern void ClearGMState(void);
extern void (* const OPCode_Func[C_OPCODE_MAX])(void);
extern void (* const SPCode_Func[C_SPCODE_MAX])(void);
extern void Run_GameMode(INT32S GPIDCode);
extern void Res_ReadScript( void *data, INT32U size );
extern void Interrupt_GameMode( INT32S GPIDCode );
extern INT32S ResBinFile_HandleSystemEvent(INT16U id);
extern INT32U get_max_bookcode(void);
extern void set_max_bookcode(INT32U bookcode);
extern INT32U get_min_bookcode(void);
extern void set_min_bookcode(INT32U bookcode);
extern INT32U get_curr_bookcode(void);
extern void set_curr_bookcode(INT32U bookcode);
extern void GM_HandleSpCode(INT16U SpCode);

#endif
