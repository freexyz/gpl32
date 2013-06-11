#ifndef __drv_l1_TIMER_H__
#define __drv_l1_TIMER_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"
#include "drv_l1_tools.h"

#define SYSCLK MCLK 
/* Direction Register Input/Output definition */
#define INPUT			        0		/* IO in input */
#define OUTPUT			        1		/* IO in output */
#define TIMER_FAIL  0
#define TIMER_OK    1
#define TIMER_TRUE  1
#define TIMER_FALSE 0

#define TIMER_USED        1
#define TIMER_NOT_USED    0

#define TIME_BASE_FAIL  0
#define TIME_BASE_OK    1

#define TIME_BASE_FALSE    0
#define TIME_BASE_TRUE     1

/*Timer Counter Bits Mode define*/
#define TIMER_COUNTER_BIT16     16
#define TIMER_COUNTER_BIT32     32
#define TIMER_COUNTER_BIT_MODE  TIMER_COUNTER_BIT16


/*TIMER register address define*/                                                                                              
#define EACH_DATA_REG_OFFSET       	0x00000008                                                                                 
#define EACH_REG_OFFSET             0x00000004                                                                                 
#define TIMER_BASE_ADDR             0xC0020000                                                                                  
/* TimerA address define */                                                                                                    
#define TIMERA_CTRL_ADDR            (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x00)  /*0xC0020000*/                                     
#define TIMERA_CCP_CTRL_ADDR        (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x01)  /*0xC0020004*/                                     
#define TIMERA_PRELOAD_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x02)  /*0xC0020008*/ /*User set the value to count from*/
#define TIMERA_CCP_REG_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x03)  /*0xC002000C*/                                     
#define TIMERA_UPCOUNT_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x04)  /*0xC0020010*/                                     
/* TimerB address define */                                                                                                    
#define TIMERB_CTRL_ADDR            (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x08)  /*0xC0020020*/                                     
#define TIMERB_CCP_CTRL_ADDR        (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x09)  /*0xC0020024*/                                     
#define TIMERB_PRELOAD_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x0A)  /*0xC0020028*/                                     
#define TIMERB_CCP_REG_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x0B)  /*0xC002002C*/                                     
#define TIMERB_UPCOUNT_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x0C)  /*0xC0020030*/                                     
/* TimerC address define */                                                                                                    
#define TIMERC_CTRL_ADDR            (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x10)  /*0xC0020040*/                                     
#define TIMERC_CCP_CTRL_ADDR        (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x11)  /*0xC0020044*/                                     
#define TIMERC_PRELOAD_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x12)  /*0xC0020048*/                                     
#define TIMERC_CCP_REG_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x13)  /*0xC002004C*/                                     
#define TIMERC_UPCOUNT_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x14)  /*0xC0020050*/                                     
/* TimerD address define */                                                                                                    
#define TIMERD_CTRL_ADDR            (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x18)  /*0xC0020060*/                                     
#define TIMERD_PRELOAD_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x1A)  /*0xC0020068*/                                     
#define TIMERD_UPCOUNT_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x1C)  /*0xC0020070*/                                                                                                                                                                 
/* TimerE address define */                                                                                                    
#define TIMERE_CTRL_ADDR            (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x20)  /*0xC0020080*/                                                   
#define TIMERE_PRELOAD_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x22)  /*0xC0020088*/                                                   
#define TIMERE_UPCOUNT_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x24)  /*0xC0020090*/                                                   
/* TimerF address define */                                                                                                    
#define TIMERF_CTRL_ADDR            (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x28)  /*0xC00200A0*/                                       
#define TIMERF_PRELOAD_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x2A)  /*0xC00200A8*/                                       
#define TIMERF_UPCOUNT_ADDR         (TIMER_BASE_ADDR+EACH_REG_OFFSET*0x2C)  /*0xC00200B0*/   

// Time Base: 0xC0030000
#define TIMEBASE_BASE_ADDR          0xC0030000 
#define TIMEBASEA_CTRL_ADDR         (TIMEBASE_BASE_ADDR+EACH_REG_OFFSET*0x00)  /*0xC0030000*/
#define TIMEBASEB_CTRL_ADDR         (TIMEBASE_BASE_ADDR+EACH_REG_OFFSET*0x01)  /*0xC0030004*/
#define TIMEBASEC_CTRL_ADDR         (TIMEBASE_BASE_ADDR+EACH_REG_OFFSET*0x02)  /*0xC0030008*/
#define TIMEBASE_RESET_ADDR         (TIMEBASE_BASE_ADDR+EACH_REG_OFFSET*0x08)  /*0xC0030020*/


/*TIMER MASK definition*/
#define TIMER_INT_FLAG_MASK     0x00008000
#define GET16BITS_MASK          0x0000FFFF
#define GET32BITS_MASK          0xFFFFFFFF
#if TIMER_COUNTER_BIT_MODE==TIMER_COUNTER_BIT32
#define GETBITS_MASK            GET32BITS_MASK
#else
#define GETBITS_MASK            GET16BITS_MASK
#endif

#define SET_TMXIE               0x00004000
#define SET_TMXEN               0x00002000
#define TIMER_PRELOAD_COUNTER_SET(TIMERID,START_COUNT) DRV_Reg32(TIMERA_PRELOAD_ADDR+EACH_REG_OFFSET*8*TIMERID) = (START_COUNT & GETBITS_MASK); //R_TIMERA_PRELOAD &= (start_count & GET16BITS_MASK)
#define TIMER_CCP_REG_SET(TIMERID,TMXUCR) DRV_Reg32(TIMERA_CCP_REG_ADDR+EACH_REG_OFFSET*8*TIMERID) = (TMXUCR & GETBITS_MASK); 
#define TIMERA_INT_ENABLE       R_TIMERA_CTRL |= SET_TMXIE
#define TIMERA_INT_DISABLE      R_TIMERA_CTRL &= ~SET_TMXIE
#define TIMERB_INT_ENABLE       R_TIMERB_CTRL |= SET_TMXIE
#define TIMERB_INT_DISABLE      R_TIMERB_CTRL &= ~SET_TMXIE
#define TIMERC_INT_ENABLE       R_TIMERC_CTRL |= SET_TMXIE
#define TIMERC_INT_DISABLE      R_TIMERC_CTRL &= ~SET_TMXIE
#define TIMERD_INT_ENABLE       R_TIMERD_CTRL |= SET_TMXIE
#define TIMERD_INT_DISABLE      R_TIMERD_CTRL &= ~SET_TMXIE
#define TIMERE_INT_ENABLE       R_TIMERE_CTRL |= SET_TMXIE
#define TIMERE_INT_DISABLE      R_TIMERE_CTRL &= ~SET_TMXIE


/*TIME BASE MASK definition*/
#define SET_TMBXEN               0x00002000

//#define SYSCLK                  48000000


#define LOWEST_BIT_MASK         0x00000001
#define LOW_HELF_16_BIT_MASK         0x0000FFFF

typedef enum 
{
	SRCBSEL_DEFAULT=0,
	SRCB_2048Hz_SELECT=SRCBSEL_DEFAULT,
	SRCB_1024Hz_SELECT,
	SRCB_256Hz_SELECT,
	SRCB_TIMEBASE_B_SELECT,
	SRCB_TIMEBASE_A_SELECT,
	SRCB_0_,
	SRCB_1_,
	SRCB_SELECT_MAX=SRCB_1_
} SRCBSEL_ENUM;

typedef enum 
{
	SRCASEL_DEFAULT=0,
	SRCA_SYSCLK_DIV2_SELECT=SRCASEL_DEFAULT,
	SRCA_SYSCLK_DIV256_SELECT,
	SRCA_32768Hz_SELECT,
	SRCA_8192Hz_SELECT,
	SRCA_4096Hz_SELECT,
	SRCA_1_,
	SRCA_TIMER_OVERFLOW,
	SRCA_EXTA_WITH_PRESCALE,
	SRCA_0_,
	SRCA_SELECT_MAX=SRCA_0_
} SRCASEL_ENUM;

/* auto reload enum */
typedef enum 
{
	AUTO_RELOAD_ENABLE=0,
	AUTO_RELOAD_DISABLE
} AUTO_RELOAD_ENUM;

typedef enum 
{
	EXTBSEL_DEFAULT=0,  
	EXTB_EVERY_FALLING=EXTBSEL_DEFAULT,
	EXTB_EVERY_RISING,
	EXTB_EVERY_4_RISING,
	EXTB_EVERY_16_RISING,
	EXTB_EVERY_MAX
} EXTBSEL_ENUM;


typedef enum 
{
	EXTASEL_DEFAULT=0,  
	EXTA_EVERY_FALLING=EXTASEL_DEFAULT,
	EXTA_EVERY_RISING,
	EXTA_EVERY_4_RISING,
	EXTA_EVERY_16_RISING,
	EXTA_EVERY_MAX
} EXTASEL_ENUM;

typedef enum 
{
	TIMER_DISABLE=0,
	TIMER_ENABLE
} TMXEN_ENUM;   /*if enable, timer will start counting*/


typedef enum 
{
	TIMER_INT_DISABLE=0,
	TIMER_INT_ENABLE
} TMXIE_ENUM;   /*if enable, timer interrupt will start counting*/

typedef enum 
{
	TIMER_INT_FLAG_DEFAULT=0,
	TIMER_INT_FLAG_FALSE=TIMER_INT_FLAG_DEFAULT,
	TIMER_INT_FLAG_TRUE
} TMXIF_ENUM;   

typedef enum
{
	CMP_HIGH_PULSE_ON_CCP=0,
	CMP_LOW_PULSE_ON_CCP,
	CMP_UNAFFECTED_ON_CCP  
} CMPXSEL_ENUM;

typedef enum
{
	CAP_EVERY_FALLING=0,
	CAP_EVERY_RISING  
} CAPSEL_ENUM;

typedef enum
{
	CCP_MODE_DISABLED=0,
	CAPTURE_ENABLED=0x1,
	COMPARISON_ENABLED=0x2,
	PWM_ENABLED=0x3,
	CCPXEN_MAX
} CCPXEN_ENUM;


/* Time base enum begin */

typedef enum 
{
	TMBXEN_DISABLE=0,
	TMBXEN_ENABLE
} TMBXEN_ENUM; 

typedef enum 
{
	TMBXIE_DISABLE=0,
	TMBXIE_ENABLE
} TMBXIE_ENUM; 

typedef enum 
{
	TMBXIF_DISABLE=0,
	TMBXIF_ENABLE
} TMBXIF_ENUM; 


#ifndef __TIMER_TYPEDEF__
#define __TIMER_TYPEDEF__

typedef enum
{
	TIMER_SOURCE_256HZ=0,
	TIMER_SOURCE_1024HZ,
	TIMER_SOURCE_2048HZ,
	TIMER_SOURCE_4096HZ,
	TIMER_SOURCE_8192HZ,
	TIMER_SOURCE_32768HZ,
	TIMER_SOURCE_MCLK_DIV_256,
	TIMER_SOURCE_MCLK_DIV_2,
	TIMER_SOURCE_MAX
} TIMER_SOURCE_ENUM;


typedef enum
{
	TMBAS_1HZ=1,
	TMBAS_2HZ,
	TMBAS_4HZ,
	TMBBS_8HZ=0,
	TMBBS_16HZ,
	TMBBS_32HZ,
	TMBBS_64HZ,
	TMBCS_128HZ=0,
	TMBCS_256HZ,
	TMBCS_512HZ,
	TMBCS_1024HZ,
	TMBXS_MAX
} TMBXS_ENUM;

typedef enum
{
	TIMER_A=0,
	TIMER_B,
	TIMER_C,
	TIMER_D,
	TIMER_E,
	TIMER_F,
	TIMER_RTC,
	TIMER_ID_MAX
} TIMER_ID_ENUM;

typedef enum
{
	TIME_BASE_A=0,
	TIME_BASE_B,
	TIME_BASE_C,
	TIME_BASE_ID_MAX
} TIME_BASE_ID_ENUM;

typedef enum
{
	PWM_NRO_OUTPUT=0,
	PWM_NRZ_OUTPUT  
} PWMXSEL_ENUM;

#endif  //__TIMER_TYPEDEF__

extern void timer_init(void);
/* if times_counter == 0 , then isr will repeat witoout limit, until stop the timer*/
extern INT32S timer_freq_setup(INT32U timer_id, INT32U freq_hz, INT32U times_counter, void(* TimerIsrFunction)(void));
extern INT32S timer_msec_setup(INT32U timer_id, INT32U msec, INT32U times_counter,void (*TimerIsrFunction)(void));
extern INT32S dac_timer_freq_setup(INT32U freq_hz);  /* for dac timer (TimerE)*/
extern INT32S adc_timer_freq_setup(INT32U timer_id, INT32U freq_hz);
extern INT32S timer_stop(INT32U timer_id);  /* only used for timer A,B,C and Timer E and F -> no effect*/
extern INT32S time_base_setup(INT32U time_base_id, TMBXS_ENUM tmbxs_enum_hz, INT32U times_counter, void(* TimeBaseIsrFunction)(void));
extern INT32S time_base_stop(INT32U time_base_id);
extern INT32U TimerCountReturn(INT32U Timer_Id);
extern void TimerCountSet(INT32U Timer_Id, INT32U set_count);
extern void time_base_reset(void);
extern INT32S timer_start_without_isr(INT32U timer_id, TIMER_SOURCE_ENUM timer_source);
extern void timer_counter_reset(INT32U timer_id);
extern INT32S timer_pwm_setup(INT32U timer_id, INT32U freq_hz, INT8U pwm_duty_cycle_percent, PWMXSEL_ENUM NRO_NRZ);
extern INT32S timerD_counter_init(void);
extern INT32S tiny_counter_get(void);
extern INT32S timer_freq_setup_lock(TIMER_ID_ENUM timer_id);
extern INT32S timer_freq_setup_unlock(TIMER_ID_ENUM timer_id);
extern INT32S timer_freq_setup_status_get(TIMER_ID_ENUM timer_id);
#endif  /*__drv_l1_TIMER_H__*/
