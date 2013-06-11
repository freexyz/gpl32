#ifndef __COMAIR_TEST_H__
#define __COMAIR_TEST_H__

#define	F0	19126				/* Tone F0, frequency = 19126Hz */
#define	F1	19688				/* Tone F1, frequency = 19688Hz */
#define	F2	21374				/* Tone F2, frequency = 21374Hz */
#define	F3	20812				/* Tone F3, frequency = 20812Hz */
#define	F4	20250				/* Tone F4, frequency = 20250Hz */
#define	C_COMAIR_TIMERA_DATA	40000		/* Initial timer target(frequency) : 40KHz */
#define	F0_PRELOAD				0x10000 - (MCLK/2/F0)
#define	F1_PRELOAD				0x10000 - (MCLK/2/F1)
#define	F2_PRELOAD				0x10000 - (MCLK/2/F2)
#define	F3_PRELOAD				0x10000 - (MCLK/2/F3)
#define	F4_PRELOAD				0x10000 - (MCLK/2/F4)
#define	F_OFF_PRELOAD			0x10000 - (MCLK/2/C_COMAIR_TIMERA_DATA)
#define	C_COMAIR_IOTX_SEND_MAX_COUNT	163		/* 4096(Hz) * 0.04(s) = 163. Every 1/4096 sec will execute COMAIR_IOTX_Send() once, so 40ms will execute 163 times. */
#define	C_COMAIR_IOTX_RAMP_TIMER		3		/* In order to prevent generating beep sounds between each tones in one command, ramp up/down 3 preloads in 1/4096 sec. */

typedef enum {
	COMAIR_SEND_ERROR_NONE=0,
	COMAIR_SEND_ERROR_CMD_START_TO_SEND,
	COMAIR_SEND_ERROR_CMD_PROCESSING,
	COMAIR_SEND_ERROR_CMD_INVALID
} COMAIR_SEND_STATUS;

typedef enum {
	COMAIR_SEND_STANDBY=0,
	COMAIR_SEND_RAMPDOWN,
	COMAIR_SEND_RAMPUP,
	COMAIR_SEND_SENDING
} COMAIR_SEND_RESULT;

typedef enum {
	COMAIR_CMD_INVALID=0,
	COMAIR_CMD_VALID
} COMAIR_CMD_CHECK;

typedef enum {
	SINGLE_CMD=0,
	MULTI_CMD
} COMAIR_CMD_TYPE;

/* Global API */
void comair_tx_init(void);		/* Initialize COMAIR timers, pincode, and command table */
extern COMAIR_SEND_STATUS send_comair_cmd_single_cmd(INT8U cmd);	/* Send single command */
extern COMAIR_SEND_STATUS send_comair_cmd_multi_cmd(INT16U cmd);	/* Send multiple command */
extern void set_comair_pincode_cmd_gen(INT16U pin);	/* setup pincode and generate commands by pincode */
extern void start_comair_timer(void);		/* start timers which relate with COMAIR */
extern void stop_comair_timer(void);		/* stop sending command */
extern BOOLEAN get_comair_timer_status(void);


//=============================================================
//=============================================================
/* Local API */
void send_comair_command(void);	/* Start to send COMAIR commands */
COMAIR_CMD_CHECK cmd_validation(INT16U cmd);
//COMAIR_SEND_RESULT get_comair_status(void);		/* Return current play_status */
void comair_iotx_send_isr(void);
void comair_pincode_set(INT16U pin);
void comair_cmd_generator(INT8U command);
void comair_update_pwm_timer(void);

#endif	/*__COMAIR_TEST_H__ */
