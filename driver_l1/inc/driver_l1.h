#ifndef __DRIVER_L1_H__
#define __DRIVER_L1_H__

#include "project.h"
#include "gp_stdlib.h"
#if _OPERATING_SYSTEM != _OS_NONE
#include "os.h"
#endif
#include "driver_l1_cfg.h"

/* Nand driver pseudo header include */
#if _DRV_L1_NAND == 1
#include "./../../nand/inc/drv_l1_nand_flash_ext.h"
#endif

#ifndef __ALIGN_DEFINE__
#define __ALIGN_DEFINE__
    #ifdef __ADS_COMPILER__
        #define ALIGN32 __align(32)
        #define ALIGN16 __align(16)
        #define ALIGN8 __align(8)
        #define ALIGN4 __align(4)
    #else
        #ifdef __CS_COMPILER__
        #define ALIGN32 __attribute__ ((aligned(32)))
        #define ALIGN16 __attribute__ ((aligned(16)))
        #define ALIGN8 __attribute__ ((aligned(8)))
        #define ALIGN4 __attribute__ ((aligned(4)))
        #else
        #define ALIGN32 __align(32)
        #define ALIGN16 __align(16)
        #define ALIGN8 __align(8)
        #define ALIGN4 __align(4)
        #endif
    #endif
#endif

#ifndef __CS_COMPILER__
    #define ASM(asm_code)  __asm {asm_code}  /*ADS embeded asm*/
    #define IRQ            __irq
    #define PACKED 			__packed
#else
    #define ASM(asm_code)  asm(#asm_code);  /*ADS embeded asm*/
    #define IRQ            __attribute__ ((interrupt))
    #define PACKED	   __attribute__((__packed__))
#endif


#ifndef MCU_VERSION
//---------------------------------------------------------------------------
#define	GPL32_A                     0x1                                    //
#define	GPL32_B                     0x2                                    //
#define	GPL32_C                     0x3                                    //
#define	GPL326XX                    0x10                                   //
#define GPL326XX_B                  0x11                                   //
#define MCU_VERSION             GPL32_C                                    //
//---------------------------------------------------------------------------
#endif

#ifdef _PROJ_TYPE
typedef INT32S (*fp_msg_qsend)(void*,INT32U,void*,INT32U,INT32U);
extern void turnkey_uart_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
#endif /*_PROJ_TYPE*/

extern void drv_l1_init(void);
extern void drv_l1_reinit(void);
// ARM
#define	EXCEPTION_UNDEF     0x1
#define	EXCEPTION_SWI       0x2
#define	EXCEPTION_PABORT    0x3
#define	EXCEPTION_DABORT    0x4
#define	EXCEPTION_IRQ       0x5
#define	EXCEPTION_FIQ       0x6

extern void exception_table_init(void);
extern INT32S register_exception_table(INT32U exception, INT32U handler);

extern void fiq_enable(void);
extern void	irq_enable(void);


// Memory Controller
extern void memc_init(void);

// system
extern INT32U  MCLK;
extern INT32U  MHZ;

#ifndef SDRAM_SIZE
#define SDRAM_SIZE SD_64Mb
#endif


#ifndef _SYS_SDRAM_DEF_
#define _SYS_SDRAM_DEF_
typedef enum {
    CL_2=2,
    CL_3=3
} SDRAM_CL;

typedef enum {
    SD_24MHZ=24,
    SD_48MHZ=48,
    SD_96MHZ=96
} SDRAM_CLK_MHZ;

typedef enum {
    SDRAM_OUT_DLY_LV0=0,
    SDRAM_OUT_DLY_LV1 ,
    SDRAM_OUT_DLY_LV2 ,
    SDRAM_OUT_DLY_LV3 ,
    SDRAM_OUT_DLY_LV4 ,
    SDRAM_OUT_DLY_LV5 ,
    SDRAM_OUT_DLY_LV6 ,
    SDRAM_OUT_DLY_LV7 ,
    SDRAM_OUT_DLY_LV8 ,
    SDRAM_OUT_DLY_LV9 ,
    SDRAM_OUT_DLY_LV10,
    SDRAM_OUT_DLY_LV11,
    SDRAM_OUT_DLY_LV12,
    SDRAM_OUT_DLY_LV13,
    SDRAM_OUT_DLY_LV14,
    SDRAM_OUT_DLY_LV15
} SD_CLK_OUT_DELAY_CELL;

typedef enum {
    SDRAM_IN_DLY_LV0=0,
    SDRAM_IN_DLY_LV1 ,
    SDRAM_IN_DLY_LV2 ,
    SDRAM_IN_DLY_LV3 ,
    SDRAM_IN_DLY_LV4 ,
    SDRAM_IN_DLY_LV5 ,
    SDRAM_IN_DLY_LV6 ,
    SDRAM_IN_DLY_LV7 ,
    SDRAM_IN_DLY_DISABLE
} SD_CLK_IN_DELAY_CELL;

typedef enum {
    SD_CLK_PHASE_0=0,
    SD_CLK_PHASE_180
} SD_CLK_PHASE;

typedef enum {
    SD_DISABLE=0,
    SD_ENABLE
} SD_SWITCH;

typedef enum {
    SD_BUS_16bit=0,
    SD_BUS_32bit
} SD_BUS_WIDTH;


typedef enum {
    SD_BANK_1M=0,
    SD_BANK_4M
} SD_BANK_BOUND;

typedef enum {
    SD_16Mb=0x0,
    SD_2MB=SD_16Mb,
    SD_64Mb=0x1,
    SD_8MB=SD_64Mb,
    SD_128Mb=0x2,
    SD_16MB=SD_128Mb,
    SD_256Mb=0x3,
    SD_32MB=SD_256Mb,
    SD_512Mb=0x4,
    SD_64MB=SD_512Mb,
    SD_1024Mb=0x4,
    SD_128MB=SD_1024Mb
} SD_SIZE;

typedef enum {
    tREFI_3o9u=39,
    tREFI_7o8u=78,    /*suggest: when SDRAM size >= 256Mb*/
    tREFI_15o6u=156,  /*suggest: when SDRAM size < 256Mb*/
    tREFI_31o2u=312
} tREFI_ENUM;

#endif  // _SYS_SDRAM_DEF_

extern void system_init(void);
extern INT8U mcu_version_get(void);

// sys-arbit init
extern void system_bus_arbiter_init(void);
// sys-sdram setting
extern INT32S sys_sdram_self_refresh_thread_cycle(INT8U sref_cycle);
extern INT32S sys_sdram_CAS_latency_cycle(SDRAM_CL cl_cycle);
extern INT32S sys_sdram_wakeup_cycle(INT8U RC_cycle);
extern INT32S sys_sdram_refresh_period_ns(INT32U refresh_period_ns);
extern INT32S sys_sdram_MHZ_set(SDRAM_CLK_MHZ sd_clk);
extern INT32S sys_sdram_read_latch_at_CLK_Neg(INT8U ebable_disable);
extern void sys_sdram_input_clock_dly_enable(INT8U ebable_disable);
extern void system_sdram_driving(void);

// sys-PLL setting
extern void system_set_pll(INT32U PLL_CLK);
extern void system_enter_halt_mode(void);
//extern void system_usbh_cheat(void);
//extern void system_usbh_uncheat(void);
extern void system_power_on_ctrl(void);
extern void system_da_ad_pll_en_set(BOOLEAN status);
extern INT32U system_rtc_counter_get(void);
extern void system_rtc_counter_set(INT32U rtc_cnt);
extern void sys_weak_6M_set(BOOLEAN status);
extern void sys_reg_sleep_mode_set(BOOLEAN status);
extern void sys_ir_power_handler(void);
extern void sys_ir_power_set_pwrkey(INT8U code);
extern void system_clks_disable0(INT32U CLK_MASK);
extern void system_clks_disable1(INT32U CLK_MASK);
extern void system_clks_enable0(INT32U CLK_MASK);
extern void system_clks_enable1(INT32U CLK_MASK);
extern void sys_sdram_auto_refresh_set(INT8U cycle);
extern void sys_ir_opcode_clear(void);
extern INT32U sys_ir_opcode_get(void);
extern INT32U sys_ir_power_get_pwrkey(void);
extern INT32S sdram_calibrationed_flag(void);
/*iRAM system functions*/
#ifndef _SDRAM_DRV
#define _SDRAM_DRV
typedef enum {
    SDRAM_DRV_4mA=0x0000,
    SDRAM_DRV_8mA=0x5555,
    SDRAM_DRV_12mA=0xAAAA,
    SDRAM_DRV_16mA=0xFFFF
} SDRAM_DRV;
#endif

#ifndef _NEG_SAMPLE
#define _NEG_SAMPLE
typedef enum {
    NEG_OFF=0x0,
    NEG_ON=0x2
} NEG_SAMPLE;
#endif

extern INT32S sys_pll_set(INT32U PLL_CLK, INT8U sdram_out_dly, INT8U sdram_in_dly, SD_SIZE sdram_size, NEG_SAMPLE neg, SDRAM_DRV sdram_driving);
extern INT32S sys_clk_set(INT32U PLL_CLK);

// Cache Controller
extern INT32S cache_lockdown(INT32U addr, INT32U size);
extern INT32S cache_drain_range(INT32U addr, INT32U size);
extern INT32S cache_invalid_range(INT32U addr, INT32U size);
extern INT32S cache_burst_mode_enable(void);
extern void cache_burst_mode_disable(void);
extern void cache_init(void);
extern void cache_enable(void);
extern void cache_disable(void);
extern void cache_write_back_enable(void);
extern void cache_write_back_disable(void);

// Vector Interrupt Controller
#define VIC_ALM_SCH_HMS     1
#define VIC_TMB_ABC         2
#define VIC_3               3
#define VIC_4               4
#define VIC_SPI1            5
#define VIC_EXT_AB          6
#define VIC_KEY_CHANGE      7
#define VIC_KEY_SCAN        8
#define VIC_TIMER3          9
#define VIC_TIMER2          10
#define VIC_11              11
#define VIC_DFR             12
#define VIC_SPI0            13
#define VIC_UART            14
#define VIC_CF              15
#define VIC_MS              16
#define VIC_TIMER1          17
#define VIC_18              18
#define VIC_SD              19
#define VIC_NFC             20
#define VIC_USB             21
#define VIC_SCALER          22
#define VIC_JPEG            23
#define VIC_DMA             24
#define VIC_TIMER0          25
#define VIC_26              26
#define VIC_PPU             27
#define VIC_ADCF            28
#define VIC_ADC             29
#define VIC_AUDB            30
#define VIC_AUDA            31
#define VIC_UNEXPECT        32
#define VIC_MAX_IRQ         33  // Don't forget to modify this when new interrupt source is added

#define VIC_SPU_PW          1
#define VIC_SPU_BEAT        2
#define VIC_SPU_ENV         3
#define VIC_SPU_FIQ         4
#define VIC_MAX_FIQ         5   // Don't forget to modify this when new fast interrupt source is added

extern void vic_init(void);
extern INT32U vic_global_mask_set(void);
extern void vic_global_mask_restore(INT32U old_mask);
extern INT32S vic_irq_register(INT32U vic_num, void (*isr)(void));
extern INT32S vic_irq_unregister(INT32U vic_num);
extern INT32S vic_irq_enable(INT32U vic_num);
extern INT32S vic_irq_disable(INT32U vic_num);
extern INT32S vic_fiq_register(INT32U vic_num, void (*isr)(void));
extern INT32S vic_fiq_unregister(INT32U vic_num);
extern INT32S vic_fiq_enable(INT32U vic_num);
extern INT32S vic_fiq_disable(INT32U vic_num);

//XDC
//extern INT32U XD_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);		//Daniel 01/08
extern INT32U XD_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer, INT16U ReadTimes);	//Daniel 01/08
extern INT32U XD_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer, INT16U wTargetLogicBlk);	//Daniel 01/05
extern INT32U XD_WritePhyPage_Mark_BAD_block(INT32U wPhysicPageNum, INT32U WorkBuffer, INT16U wTargetLogicBlk);	//Daniel 03/18
extern INT32U XD_ErasePhyBlock(INT32U wPhysicBlkNum);		//Daniel 02/24
extern INT32U XD_ErasePhyBlock_withFF(INT32U wPhysicBlkNum);	//Daniel 03/05
extern INT32U XD_id_init(void);		//Daniel 02/24
extern void	  XDC_Getinfo(INT16U* PageSize,INT16U* BlkPageNum,INT32U* NandBlockNum);	//Daniel 00/24
extern INT32U XD_Read_C_Area_16byte(INT32U wPhysicPageNum, INT32U WorkBuffer);		//Daniel 02/24
extern void   xdc_uninit(void);		//Daniel 02/24

extern void xd_cs_io_low(void);		//Daniel 03/09
extern void xd_cs_io_high(void);	//Daniel 03/09
extern INT32U XD_read_Status(void);	//Daniel 03/19
extern INT32U xd_reset(void);	//Daniel 03/09

extern void xDC_Plug_Status_Set(INT8U Status);
extern INT8U xDC_Plug_Status_Get(void);

#ifndef NAND_PAGE_TYPE
#define NAND_LARGEBLK	0x00000000
#define NAND_SMALLBLK  	0x00000001
#define NAND_PAGE_TYPE  		NAND_LARGEBLK
#endif

#ifndef NAND_WP_IO
#define NAND_WP_PIN_NONE    0xFF
#define NAND_WP_IO          NAND_WP_PIN_NONE//IO_E9
#endif


#ifndef NAND_CS_ENUM_DEF
#define NAND_CS_ENUM_DEF
typedef enum
{
    NAND_CS1=0x2,   /* IOF1 */
    NAND_CS2=0x4,   /* IOF2 */
    NAND_CS3=0x8    /* IOF3 */
} NAND_CS_ENUM;
#endif

#ifndef NAND_SHARE_MODE_DEF
#define NAND_SHARE_MODE_DEF
typedef enum
{
    NF_SHARE_PIN_MODE,
    NF_NON_SHARE_MODE
} NAND_SAHRE_MODE_ENUM;
#endif


extern void Nand_Uninit(void);
extern INT32U Nand_Init(void);
extern INT32U NandReset(void);
extern INT32U Nand_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_ErasePhyBlock(INT32U wPhysicBlkNum);
extern void nand_flash_bchtable_alloc(INT16S *alpha_to ,INT16U *index_of);
extern void Nand_Getinfo(INT16U* PageSize,INT16U* BlkPageNum,INT32U* NandBlockNum,INT32U* NandID);
extern void nand_small_page_cs_pin_reg(NAND_CS_ENUM nand_cs);
extern void nand_wp_pin_reg(INT8U gpio_pin_number);
extern void nand_large_page_cs_reg(NAND_CS_ENUM nand_large_page_cs);
extern void nand_share_mode_reg(NAND_SAHRE_MODE_ENUM share_or_nonshare);

// Nand1/Nand2/XD swap
#ifndef XD_PIN_ENUM_DEF
#define XD_PIN_ENUM_DEF
    typedef enum
    {
      XD_CS1=0x2,   /* IOF1 */
      XD_CS2=0x4,   /* IOF2 */
      XD_CS3=0x8    /* IOF3 */
    } XD_CS_ENUM;

    typedef enum
    {
      XD_IOC12_15,
      XD_IOC6_9,
      XD_IOG5_6_10_11
    } XD_CTRL_PIN_ENUM;

    typedef enum
    {
      XD_DATA_IOB13_8,
      XD_DATA_IOD5_0,
      XD_DATA_IOE5_0,
      XD_DATA_IOA13_8
    } XD_DATA0_5_ENUM;

    typedef enum
    {
      XD_DATA_IOB15_14,
      XD_DATA_IOD7_6,
      XD_DATA_IOE7_6,
      XD_DATA_IOA15_14
    } XD_DATA6_7_ENUM;

    typedef enum
    {
        ID_NAND,
        ID_NAND1=ID_NAND,
        ID_XD,
        ID_NAND2=ID_XD,
        ID_NULL
    } NAND_XD_ENUM;

    typedef enum {
        XD_ONLY_MODE=0,
        XD_NAND_MODE=1,
        XD_NAND1_NAND2_MODE=2,
        XD_PAD_MODE_MAX,
        XD_PAD_MODE_NULL=0xFF
    } XD_PAD_MODE;
#endif  //XD_PIN_ENUM_DEF

extern void nand_xd_swap_to(NAND_XD_ENUM swap_to_id);
extern void xd_cs_pin_reg(XD_CS_ENUM xd_cs);
extern void xdc_nand_pad_mode_set(XD_PAD_MODE mode);
extern XD_PAD_MODE xdc_nand_mode_get(void);
extern INT32U XD_WritePhyPage_Mark_BAD_block(INT32U wPhysicPageNum, INT32U WorkBuffer, INT16U wTargetLogicBlk);	//Daniel 03/18
extern INT32U XD_ErasePhyBlock(INT32U wPhysicBlkNum);		//Daniel 01/05
extern INT32U XD_ErasePhyBlock_withFF(INT32U wPhysicBlkNum);		//Daniel 03/05
// DMA Controller
#define C_DMA_STATUS_WAITING        0
#define C_DMA_STATUS_DONE           1
#define C_DMA_STATUS_TIMEOUT        -1

typedef enum {
    DMA_DATA_WIDTH_1BYTE = 1,
    DMA_DATA_WIDTH_2BYTE = 2,
    DMA_DATA_WIDTH_4BYTE = 4
} DMA_DW_ENUM;

typedef struct {
    INT32U s_addr;                          // Source address. Must align to data width
    INT32U t_addr;                          // Target address. Must align to data width
    INT32U count;                           // Transfer count(1~0x00FFFFFF). Total transfer length = width * count
    DMA_DW_ENUM width;
    INT8U timeout;                          // 0: No timeout, 1~255: 1/256 ~ 255/256 second
    INT8U channel;                          // This member is used internally by DMA driver. Don't modify its value.
    INT8S *notify;                          // NULL: notification is not needed. DMA will set C_DMA_STATUS_DONE/C_DMA_STATUS_TIMEOUT when transfer is finish.
} DMA_STRUCT;

extern void dma_init(void);
extern INT32S dma_transfer(DMA_STRUCT *dma_struct);
extern INT32S dma_transfer_wait_ready(DMA_STRUCT *dma_struct);
#if _OPERATING_SYSTEM != _OS_NONE
extern INT32S dma_transfer_with_queue(DMA_STRUCT *dma_struct, OS_EVENT *os_q);
extern INT32S dma_transfer_with_double_buf(DMA_STRUCT *dma_struct, OS_EVENT *os_q);
extern INT32S dma_transfer_double_buf_set(DMA_STRUCT *dma_struct);
extern INT32S dma_transfer_double_buf_free(DMA_STRUCT *dma_struct);
extern INT32S dma_dbf_status_get(INT8U channel);
extern INT32S dma_status_get(INT8U channel);
#endif
extern INT32S dma_memory_fill(INT32U t_addr, INT8U value, INT32U byte_count);

// UART
extern void uart0_init(void);
extern void uart0_buad_rate_set(INT32U bps);
extern void uart0_rx_enable(void);
extern void uart0_rx_disable(void);
extern void uart0_tx_enable(void);
extern void uart0_tx_disable(void);
extern INT32S uart0_word_len_set(INT8U word_len);
extern INT32S uart0_stop_bit_size_set(INT8U stop_size);
extern INT32S uart0_parity_chk_set(INT8U status, INT8U parity);
extern void uart0_data_send(INT8U data, INT8U wait);
extern INT32S uart0_data_get(INT8U *data, INT8U wait);

// GPIO
/* Direction Register Input/Output definition */
#define GPIO_INPUT              0       /* IO in input */
#define GPIO_OUTPUT             1       /* IO in output */

/* Attribution Register High/Low definition */
#define ATTRIBUTE_HIGH          1
#define INPUT_NO_RESISTOR       1
#define OUTPUT_UNINVERT_CONTENT 1
#define ATTRIBUTE_LOW           0
#define INPUT_WITH_RESISTOR     0
#define OUTPUT_INVERT_CONTENT   0
#define DATA_HIGH               1
#define DATA_LOW                0

/* Direction Register Input/Output definition */
#define HAL_GPIO_INPUT          0   /* IO in input */
#define HAL_GPIO_OUTPUT         1   /* IO in output */

#ifndef __GPIO_TYPEDEF__
#define __GPIO_TYPEDEF__

typedef enum {
    IO_A0=0,
    IO_A1 ,
    IO_A2 ,
    IO_A3 ,
    IO_A4 ,
    IO_A5 ,
    IO_A6 ,
    IO_A7 ,
    IO_A8 ,
    IO_A9 ,
    IO_A10,
    IO_A11,
    IO_A12,
    IO_A13,
    IO_A14,
    IO_A15,
    IO_B0 ,
    IO_B1 ,
    IO_B2 ,
    IO_B3 ,
    IO_B4 ,
    IO_B5 ,
    IO_B6 ,
    IO_B7 ,
    IO_B8 ,
    IO_B9 ,
    IO_B10,
    IO_B11,
    IO_B12,
    IO_B13,
    IO_B14,
    IO_B15,
    IO_C0 ,
    IO_C1 ,
    IO_C2 ,
    IO_C3 ,
    IO_C4 ,
    IO_C5 ,
    IO_C6 ,
    IO_C7 ,
    IO_C8 ,
    IO_C9 ,
    IO_C10,
    IO_C11,
    IO_C12,
    IO_C13,
    IO_C14,
    IO_C15,
    IO_D0 ,
    IO_D1 ,
    IO_D2 ,
    IO_D3 ,
    IO_D4 ,
    IO_D5 ,
    IO_D6 ,
    IO_D7 ,
    IO_D8 ,
    IO_D9 ,
    IO_D10,
    IO_D11,
    IO_D12,
    IO_D13,
    IO_D14,
    IO_D15,
    IO_E0 ,
    IO_E1 ,
    IO_E2 ,
    IO_E3 ,
    IO_E4 ,
    IO_E5 ,
    IO_E6 ,
    IO_E7 ,
    IO_E8 ,
    IO_E9 ,
    IO_E10,
    IO_E11,
    IO_E12,
    IO_E13,
    IO_E14,
    IO_E15,
    IO_F0 ,
    IO_F1 ,
    IO_F2 ,
    IO_F3 ,
    IO_F4 ,
    IO_F5 ,
    IO_F6 ,
    IO_F7 ,
    IO_F8 ,
    IO_F9 ,
    IO_F10,
    IO_F11,
    IO_F12,
    IO_F13,
    IO_F14,
    IO_F15,
    IO_G0 ,
    IO_G1 ,
    IO_G2 ,
    IO_G3 ,
    IO_G4 ,
    IO_G5 ,
    IO_G6 ,
    IO_G7 ,
    IO_G8 ,
    IO_G9 ,
    IO_G10,
    IO_G11,
    IO_G12,
    IO_G13,
    IO_G14,
    IO_G15,
    IO_H0 ,
    IO_H1 ,
    IO_H2 ,
    IO_H3 ,
    IO_H4 ,
    IO_H5 ,
    IO_H6 ,
    IO_H7 ,
    IO_H8 ,
    IO_H9 ,
    IO_H10,
    IO_H11,
    IO_H12,
    IO_H13,
    IO_H14,
    IO_H15,
    IO_I0 ,
    IO_I1 ,
    IO_I2 ,
    IO_I3 ,
    IO_I4 ,
    IO_I5 ,
    IO_I6 ,
    IO_I7 ,
    IO_I8 ,
    IO_I9 ,
    IO_I10,
    IO_I11,
    IO_I12,
    IO_I13,
    IO_I14,
    IO_I15      
} GPIO_ENUM;

#endif  //__GPIO_TYPEDEF__

#ifndef _GPIO_MONITOR_DEF_
#define _GPIO_MONITOR_DEF_
typedef struct {
    INT8U KS_OUT74_EN   ;
    INT8U KS_OUT31_EN   ;
    INT8U KS_OUT0_EN    ;
    INT8U KS_EN         ;
    INT8U TS_all_en     ;
    INT8U SEN_EN        ;
    INT8U KEYN_IN2_EN   ;
    INT8U KSH_EN        ;
    INT8U ND_SHARE_EN   ;
    INT8U NAND_EN       ;
    INT8U UART_EN       ;
    INT8U TFTEN         ;
    INT8U TFT_MODE1     ;
    INT8U TFT_MODE0     ;
    INT8U EN_CF         ;
    INT8U EN_MS         ;
    INT8U EN_SD123      ;
    INT8U EN_SD         ;
    INT8U USEEXTB       ;
    INT8U USEEXTA       ;
    INT8U SPI1_EN       ;
    INT8U SPI0_EN       ;
    INT8U CCP_EN2       ;
    INT8U CCP_EN1       ;
    INT8U CCP_EN0       ;
    INT8U KEYC_EN       ;
    INT8U CS3_0_EN_i3   ;
    INT8U CS3_0_EN_i2   ;
    INT8U CS3_0_EN_i1   ;
    INT8U CS3_0_EN_i0   ;
    INT8U XD31_16_EN    ;
    INT8U BKOEB_EN      ;
    INT8U BKUBEB_EN     ;
    INT8U XA24_19EN3    ;
    INT8U XA24_19EN2    ;
    INT8U XA24_19EN1    ;
    INT8U XA24_19EN0    ;
    INT8U SDRAM_CKE_EN  ;
    INT8U SDRAM_CSB_EN  ;
    INT8U XA24_19EN5    ;
    INT8U XA24_19EN4    ;
    INT8U XA14_12_EN2   ;
    INT8U XA14_12_EN1   ;
    INT8U XA14_12_EN0   ;
    INT8U EPPEN         ;
    INT8U MONI_EN       ;
} GPIO_MONITOR;

#endif  //_GPIO_MONITOR_DEF_

#ifndef _GPIO_DRVING_DEF_
#define _GPIO_DRVING_DEF_

typedef enum {
    IOA_DRV_4mA=0x0,
    IOA_DRV_8mA=0x1,
    IOA_DRV_12mA=0x2,
    IOA_DRV_16mA=0x3,
/* IOB Driving Options */
    IOB_DRV_4mA=0x0,
    IOB_DRV_8mA=0x1,
    IOB_DRV_12mA=0x2,
    IOB_DRV_16mA=0x3,
/* IOC Driving Options */
    IOC_DRV_4mA=0x0,
    IOC_DRV_8mA=0x1,
    IOC_DRV_12mA=0x2,
    IOCB_DRV_16mA=0x3,
/* IOD Driving Options */
    IOD_DRV_4mA=0x0,
    IOD_DRV_8mA=0x1,
    IOD_DRV_12mA=0x2,
    IOD_DRV_16mA=0x3,
/* IOE Driving Options */
    IOE_DRV_4mA=0x0,
    IOE_DRV_8mA=0x1,
    IOE_DRV_12mA=0x2,
    IOE_DRV_16mA=0x3,
/* IOF Driving Options */
    IOF_DRV_4mA=0x0,
    IOF_DRV_8mA=0x1,
    IOF_DRV_12mA=0x2,
    IOF_DRV_16mA=0x3,
    IOF0_DRV_8mA=0x1,
    IOF1_DRV_8mA=0x1,
    IOF2_DRV_8mA=0x1,
    IOF3_DRV_8mA=0x1,
    IOF4_DRV_8mA=0x1,
    IOF5_DRV_8mA=0x1,
    IOF13_DRV_8mA=0x1,
    IOF14_DRV_8mA=0x1,
    IOF15_DRV_8mA=0x1,
    IOF4_DRV_12mA=0x2,
    IOF4_DRV_16mA=0x3,
/* IOG Driving Options */
    IOG_DRV_4mA=0x0,
    IOG_DRV_8mA=0x1,
    IOG_DRV_12mA=0x2,
    IOG_DRV_16mA=0x3,
    IOG0_DRV_8mA=0x1,
    IOG1_DRV_8mA=0x1,
    IOG2_DRV_8mA=0x1,
    IOG3_DRV_8mA=0x1,
    IOG4_DRV_8mA=0x1,

    IOG5_DRV_8mA=0x1,
    IOG5_DRV_12mA=0x2,
    IOG5_DRV_16mA=0x3,

    IOG6_DRV_8mA=0x1,
    IOG6_DRV_12mA=0x2,
    IOG6_DRV_16mA=0x3,

    IOG7_DRV_8mA=0x1,
    IOG7_DRV_12mA=0x2,
    IOG7_DRV_16mA=0x3,

    IOG8_DRV_8mA=0x1,
    IOG8_DRV_12mA=0x2,
    IOG8_DRV_16mA=0x3,

    IOG9_DRV_8mA=0x1,
    IOG9_DRV_12mA=0x2,
    IOG9_DRV_16mA=0x3,
/* IOH Driving Options */
    IOH_DRV_4mA=0x0,
    IOH_DRV_8mA=0x1,
    IOH_DRV_12mA=0x2,
    IOH_DRV_16mA=0x3,
    IOH0_DRV_8mA=0x1,
    IOH1_DRV_8mA=0x1,
    IOH1_DRV_12mA=0x2,
    IOH1_DRV_16mA=0x3,
    IOH2_DRV_8mA=0x1,
    IOH3_DRV_8mA=0x1,
    IOH4_DRV_8mA=0x1,
    IOH5_DRV_8mA=0x1,
/* IOI Driving Options */
    IOI_DRV_4mA=0x0,
    IOI_DRV_8mA=0x1,
    IOI_DRV_12mA=0x2,
    IOI_DRV_16mA=0x3     
} IO_DRV_LEVEL;

#endif

extern void gpio_init(void);
extern BOOLEAN gpio_init_io(INT32U port, BOOLEAN direction);
extern BOOLEAN gpio_read_io(INT32U port);
extern BOOLEAN gpio_write_io(INT32U port, BOOLEAN data);
extern BOOLEAN gpio_init_io(INT32U port, BOOLEAN direction);
extern BOOLEAN gpio_set_port_attribute(INT32U port, BOOLEAN attribute);
extern BOOLEAN gpio_set_memcs(INT8U cs_port,INT8U cs_io);
extern BOOLEAN gpio_set_sdramio(void);
extern BOOLEAN gpio_drving_init_io(GPIO_ENUM port, IO_DRV_LEVEL gpio_driving_level);
extern void gpio_monitor(GPIO_MONITOR *gpio_monitor);
extern void gpio_drving_init(void);
extern void gpio_drving_uninit(void);
extern void gpio_set_bkueb(BOOLEAN status);
extern void gpio_set_ice_en(BOOLEAN status);
extern void gpio_sdram_swith(INT32U port, BOOLEAN data);
// TIMER

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

/* Each Tiny count time is 2.666... us*/
/* Eample TINY_WHILE((reg_flag==0x00000001),0xE000)*/
#define TINY_WHILE(BREAK_CONDITION,TIMEOUT_TINY_COUNT)  \
{\
   INT32S ttt1 = tiny_counter_get();\
   INT32S dt=0;\
   while(!((BREAK_CONDITION) || (dt>TIMEOUT_TINY_COUNT)))\
   {\
        dt = ((tiny_counter_get() | 0x10000) - ttt1) & (0xFFFF);\
   }\
}


#define MINY_WHILE(BREAK_CONDITION,TIMEOUT_MINY_COUNT)  \
{\
   INT32S dtm=0;\
   INT32S dtn = tiny_counter_get();\
   while(!((BREAK_CONDITION) || (dtm>TIMEOUT_MINY_COUNT)))\
   {\
        if ((((tiny_counter_get() | 0x10000) - dtn) & (0xFFFF))>=375)\
        {dtm++;dtn=tiny_counter_get();}\
   }\
}

#ifndef _SW_TIMER_ID
#define _SW_TIMER_ID

typedef enum
{
    SW_TIMER_0=0,
    SW_TIMER_1,
    SW_TIMER_2,
    SW_TIMER_3,
    SW_TIMER_4,
    SW_TIMER_5,
    SW_TIMER_6,
    SW_TIMER_7,
    SW_TIMER_8,
    SW_TIMER_9,
    SW_TIMER_MAX
} SW_TIMER_ID;

#endif


extern INT32S timer_pwm_setup(INT32U timer_id, INT32U freq_hz, INT8U pwm_duty_cycle_percent, PWMXSEL_ENUM NRO_NRZ);
extern void timer_init(void);
extern INT32S timer_freq_setup(INT32U timer_id, INT32U freq_hz, INT32U times_counter, void(* TimerIsrFunction)(void));
extern INT32S dac_timer_freq_setup(INT32U freq_hz);
extern INT32S adc_timer_freq_setup(INT32U timer_id, INT32U freq_hz);
extern INT32S timer_stop(INT32U timer_id);
extern INT32U TimerCountReturn(INT32U Timer_Id);
extern void TimerCountSet(INT32U Timer_Id, INT32U set_count);
extern void time_base_reset(void);
extern INT32S timer_start_without_isr(INT32U timer_id, TIMER_SOURCE_ENUM timer_source);
extern INT32S timer_msec_setup(INT32U timer_id, INT32U msec, INT32U times_counter, void(* TimerIsrFunction)(void));
extern void timer_counter_reset(INT32U timer_id);
extern void timer_init(void);
/* if times_counter == 0 , then isr will repeat witoout limit, until stop the timer*/
extern INT32S timer_freq_setup(INT32U timer_id, INT32U freq_hz, INT32U times_counter, void(* TimerIsrFunction)(void));
extern INT32S timer_msec_setup(INT32U timer_id, INT32U msec, INT32U times_counter,void (*TimerIsrFunction)(void));
extern INT32S dac_timer_freq_setup(INT32U freq_hz);  /* for dac timer (TimerE)*/
extern INT32S timer_stop(INT32U timer_id);  /* only used for timer A,B,C and Timer E and F -> no effect*/
extern INT32S time_base_setup(INT32U time_base_id, TMBXS_ENUM tmbxs_enum_hz, INT32U times_counter, void(* TimeBaseIsrFunction)(void));
extern INT32S time_base_stop(INT32U time_base_id);
extern void sw_timer_ms_delay(INT32U msec);
extern void sw_timer_us_delay(INT32U usec);
extern INT32U sw_timer_init(INT32U TimerId, INT32U freq_hz);
extern INT32U sw_timer_get_counter_L(void);
extern INT32U sw_timer_get_counter_H(void);
extern void  drv_msec_wait(INT32U m_sec) ;
extern INT32S timerD_counter_init(void);
extern INT32S tiny_counter_get(void);
extern void sw_timer_with_isr_phy_init(TIMER_ID_ENUM phy_timer_id, INT16U freq_Hz_base);
extern void sw_timer_with_isr_phy_uninit(TIMER_ID_ENUM phy_timer_id);
extern void sw_timer_freq_setup(SW_TIMER_ID sw_timer_id, INT16U freq_Hz, void (*TimerIsrFunction)(void));
extern INT32U sw_timer_counter_get(SW_TIMER_ID sw_timer_id);
extern void sw_timer_counter_reset(SW_TIMER_ID sw_timer_id);
extern void sw_timer_start_without_isr(SW_TIMER_ID sw_timer_id, INT16U freq_Hz);
extern void sw_timer_stop(SW_TIMER_ID sw_timer_id);
extern INT32S timer_freq_setup_lock(TIMER_ID_ENUM timer_id);
extern INT32S timer_freq_setup_unlock(TIMER_ID_ENUM timer_id);
extern INT32S timer_freq_setup_status_get(TIMER_ID_ENUM timer_id);
// SPI
typedef enum
{
    SPI_LBM_NORMAL, /* no loop back */
    SPI_LBM_LOOP_BACK
} SPI_LOOP_BACK_MODE;

typedef enum
{
    SPI_0,
    SPI_1,
    SPI_MAX_NUMS
} SPI_CHANNEL;

/* master mode clock selection */
typedef enum
{
    SYSCLK_2,   /* SYSCLK/2 */
    SYSCLK_4,
    SYSCLK_8,
    SYSCLK_16,
    SYSCLK_32,
    SYSCLK_64,
    SYSCLK_128
} SPI_CLK;

typedef enum
{
    PHA0_POL0,
    PHA0_POL1,
    PHA1_POL0,
    PHA1_POL1
} SPI_CLK_PHA_POL;

extern void spi_init(INT8U spi_num);
extern void spi_lbm_set(INT8U spi_num, INT8S status);
extern INT32S spi_clk_set(INT8U spi_num, INT8S spi_clk);
extern INT32S spi_transceive(INT8U spi_num, INT8U *tx_data, INT32U tx_len, INT8U *rx_data, INT32U rx_len);
extern INT32S spi_disable(INT8U spi_num);
extern INT32S spi_pha_pol_set(INT8U spi_num, INT8U pha_pol);
extern void spi_cs_by_internal_set(INT8U spi_num,INT8U pin_num, INT8U active);
extern void spi_cs_by_external_set(INT8U spi_num);
extern INT32S spi_transceive_cpu(INT8U spi_num, INT8U *tx_data, INT32U tx_len, INT8U *rx_data, INT32U rx_len);
extern INT32S spi_clk_rate_set(INT8U spi_num, INT32U clk_rate);
// RTC
typedef struct __rtc
{
	INT32U		rtc_sec;    /* seconds [0,59]  */
	INT32U		rtc_min;    /* minutes [0,59]  */
	INT32U		rtc_hour;   /* hours [0,23]  */
	INT32U      rtc_day;    /* gpy0200 day count,[ 0,4095] */
} t_rtc;

typedef enum
{
	RTC_ALM_INT_INDEX,
	RTC_SCH_INT_INDEX,
	RTC_HR_INT_INDEX,
	RTC_MIN_INT_INDEX,
	RTC_SEC_INT_INDEX,
	RTC_HSEC_INT_INDEX,
	RTC_DAY_INT_INDEX,
	RTC_INT_MAX
} RTC_INT_INDEX;

typedef enum
{
	RTC_SCH_16HZ,
	RTC_SCH_32HZ,
	RTC_SCH_64HZ,
	RTC_SCH_128HZ,
	RTC_SCH_256HZ,
	RTC_SCH_512HZ,
	RTC_SCH_1024HZ,
	RTC_SCH_2048HZ
} RTC_SCH_PERIOD;

#define RTC_ALMEN     (1 << 10)  /* alarm function enable */
#define RTC_HMSEN     (1 <<  9)  /* H/M/S function enable */
#define RTC_SCHEN     (1 <<  8)  /* scheduler function enbale */

#define RTC_ALM_IEN       (1 << 10)  /* alarm interrupt enbale */
#define RTC_SCH_IEN       (1 <<  8)  /* scheduler interrupt enbale */
#define RTC_DAY_IEN      (1 <<  4)  /* hour interrupt enbale */
#define RTC_HR_IEN        (1 <<  3)  /* hour interrupt enbale */
#define RTC_MIN_IEN       (1 <<  2)  /* min interrupt enbale */
#define RTC_SEC_IEN       (1 <<  1)  /* alarm interrupt enbale */
#define RTC_HALF_SEC_IEN  (1 <<  0)  /* alarm interrupt enbale */

#define RTC_EN     0xFFFFFFFF
#define RTC_DIS    0

#define RTC_EN_MASK     0xFF

extern void rtc_init(void);
extern INT32S rtc_callback_set(INT8U int_idx, void (*user_isr)(void));
extern INT32S rtc_callback_clear(INT8U int_idx);
extern void rtc_alarm_set(t_rtc *rtc_time);
extern void rtc_time_get(t_rtc *rtc_time);
extern void rtc_time_set(t_rtc *rtc_time);
extern void rtc_schedule_set(INT8U freq);
extern void rtc_alarm_get(t_rtc *rtc_time);
extern INT32U rtc_irq_flag_get(void);
extern BOOLEAN rtc_day_int_get(void);

#if MCU_VERSION < GPL326XX
extern void rtc_function_set(INT32U mask, INT32U value);
extern void rtc_int_set(INT32U mask, INT32U value);
#else
extern void rtc_function_set(INT8U mask, INT8U value);
extern void rtc_int_set(INT8U mask, INT8U value);
extern void rtc_schedule_enable(INT8U freq);
extern void rtc_schedule_disable(void);
extern void rtc_day_get(t_rtc *rtc_time);
extern void rtc_day_set(t_rtc *rtc_time);
#endif

#define GPX_RTC_ALMOEN    (1 << 0)  /* alarm output singnal enable */
#define GPX_RTC_EN        (1 << 3)  /* RTC function enable */
#define GPX_RTC_VAEN      (1 << 4)  /* Voltage comparator enbale */
#define GPX_RTC_PWREN     (1 << 5)  /* set alarm output signal to high */

#define GPX_RTC_ALM_IEN       (1 <<  4)  /* alarm interrupt enbale */
#define GPX_RTC_DAY_IEN       (1 <<  5)  /* hour interrupt enbale */
#define GPX_RTC_HR_IEN        (1 <<  3)  /* hour interrupt enbale */
#define GPX_RTC_MIN_IEN       (1 <<  2)  /* min interrupt enbale */
#define GPX_RTC_SEC_IEN       (1 <<  1)  /* alarm interrupt enbale */
#define GPX_RTC_HALF_SEC_IEN  (1 <<  0)  /* alarm interrupt enbale */

//MATRE KEYSCAN
extern void matre_keyscaninit(void);
extern void matre_scaninit(void);

// DAC
extern void dac_init(void);
extern void dac_enable_set(BOOLEAN status);
extern void dac_disable(void);
extern void dac_cha_data_put(INT16S *data,INT32U len, INT8S *notify);
extern void dac_chb_data_put(INT16S *data,INT32U len, INT8S *notify);
extern INT32S dac_cha_data_dma_put(INT16S *data,INT32U len, INT8S *notify);
extern INT32S dac_chb_data_dma_put(INT16S *data,INT32U len, INT8S *notify);
#if _OPERATING_SYSTEM != _OS_NONE
extern INT32S dac_cha_dbf_put(INT16S *data,INT32U len, OS_EVENT *os_q);
extern INT32S dac_cha_dbf_set(INT16S *data,INT32U len);
extern void dac_dbf_free(void);
extern INT32S dac_dbf_status_get(void);
extern INT32S dac_dma_status_get(void);
#endif
extern void dac_fifo_level_set(INT8U cha_level,INT8U chb_level);
extern void dac_sample_rate_set(INT32U hz);
extern void dac_mono_set(void);
extern void dac_stereo_set(void);
extern void dac_stereo_indi_buffer_set(void);
extern void dac_timer_stop(void);
extern void dac_pga_set(INT8U gain);
extern void dac_vref_set(BOOLEAN status);
extern INT8U dac_pga_get(void);

//ADC

#define ADC_AS_TIMER_C     0
#define ADC_AS_TIMER_D     1
#define ADC_AS_TIMER_E     2
#define ADC_AS_TIMER_F     3

#define ADC_LINE_0         0
#define ADC_LINE_1         1
#define ADC_LINE_2         2
#define ADC_LINE_3         3

extern void adc_init(void);
extern void adc_fifo_level_set(INT8U level);
extern void adc_auto_ch_set(INT8U ch);
extern void adc_manual_ch_set(INT8U ch);
extern void adc_manual_callback_set(void (*user_isr)(INT16U data));
extern INT32S adc_manual_sample_start(void);
extern INT32S adc_auto_sample_start(void);
extern INT32S adc_auto_data_dma_get(INT16U *data, INT32U len, INT8S *notify);
extern INT32S adc_auto_data_get(INT16U *data, INT32U len, INT8S *notify);
extern void   adc_auto_sample_stop(void);
extern INT32S adc_sample_rate_set(INT8U timer_id, INT32U hz);
extern INT32S adc_timer_stop(INT8U timer_id);
extern void adc_fifo_clear(void);
extern INT32S adc_user_line_in_en(INT8U line_id,BOOLEAN status);
extern void tp_callback_set(void (*user_isr)(void));
extern void adc_tp_en(BOOLEAN status);
extern void adc_tp_int_en(BOOLEAN status);
extern void adc_vref_enable_set(BOOLEAN status);

//MIC
extern void mic_init(void);
extern void mic_auto_int_set(BOOLEAN status);
extern void mic_fifo_level_set(INT8U level);
extern void mic_fifo_clear(void);
extern INT32S mic_auto_sample_start(void);
extern void mic_auto_sample_stop(void);
extern INT32S mic_sample_rate_set(INT8U timer_id, INT32U hz);
extern INT32S mic_timer_stop(INT8U timer_id);
extern void mic_vref_enable_set(BOOLEAN status);
extern void mic_agc_enable_set(BOOLEAN status);

#if (defined MCU_VERSION) && (MCU_VERSION == GPL326XXB)
//add for DAGC
extern void mic_set_pga_gain(INT16U booster_gain, INT16U pga_gain);
extern INT16U mic_get_pga_gain(void);
extern INT16U mic_get_booster_gain(void);
extern void mic_dagc_enable(INT16U enable);
extern INT16U mic_dagc_get_status(void);
extern void mic_dagc_set_update_freq(INT16U update_freq);
extern void mic_dagc_setup(INT16U mode, INT16U threshold, INT16U att_scale, INT16U rel_scale, INT16U dis_zc);
extern INT16U mic_dagc_get_threshold(void);
extern void mic_dagc_set_att_rel_time(INT16U att_time, INT16U rel_time);
extern INT16U mic_dagc_get_att_time(void);
extern INT16U mic_dagc_get_rel_time(void);
#endif

//CMOS SENSOR
// CSI Flag Type Definitions
#define	FT_CSI_CCIR656			(1<<0)
#define	FT_CSI_YUVIN			(1<<1)
#define	FT_CSI_YUVOUT			(1<<2)
#define FT_CSI_RGB1555			(1<<3)

extern void sccb_init(void);
extern INT32S sccb_write_Reg8Data8(INT8U id, INT8U addr, INT8U data);
extern INT32S sccb_read_Reg8Data8(INT8U id, INT8U addr, INT8U *data);
extern INT32S sccb_write_Reg8Data16(INT8U id, INT8U addr, INT16U data);
extern INT32S sccb_read_Reg8Data16(INT8U id, INT8U addr, INT16U *data);
extern INT32S sccb_write_Reg16Data8(INT8U id, INT16U addr, INT8U data); 
extern INT32S sccb_read_Reg16Data8(INT8U id, INT16U addr, INT8U *data); 
extern INT32S sccb_write_Reg16Data16(INT8U id, INT16U addr, INT16U data);  
extern INT32S sccb_read_Reg16Data16(INT8U id,	INT16U addr, INT16U *data); 
extern void CSI_Init (INT16S nWidthH, INT16S nWidthV, INT16U uFlag, INT32U uFrmBuf0, INT32U uFrmBuf1);
extern void Sensor_Bluescreen_Enable(void);
extern void Sensor_Cut_Enable(void);

//TFT
#define TFT_ENABLE    0xFFFFFFFF
#define TFT_DIS       0

#define TFT_CLK_DIVIDE_1        0x0
#define TFT_CLK_DIVIDE_2        0x2
#define TFT_CLK_DIVIDE_3        0x4
#define TFT_CLK_DIVIDE_4        0x6
#define TFT_CLK_DIVIDE_5        0x8
#define TFT_CLK_DIVIDE_6        0xA
#define TFT_CLK_DIVIDE_7        0xC
#define TFT_CLK_DIVIDE_8        0xE

#if MCU_VERSION == GPL326XX
#define TFT_CLK_DIVIDE_9        0x10
#define TFT_CLK_DIVIDE_10       0x12
#define TFT_CLK_DIVIDE_11       0x14
#define TFT_CLK_DIVIDE_12       0x16
#define TFT_CLK_DIVIDE_13       0x18
#define TFT_CLK_DIVIDE_14       0x1A
#define TFT_CLK_DIVIDE_15       0x1C
#define TFT_CLK_DIVIDE_16       0x1E
#define TFT_CLK_DIVIDE_17       0x20
#define TFT_CLK_DIVIDE_18       0x22
#define TFT_CLK_DIVIDE_19       0x24
#define TFT_CLK_DIVIDE_20       0x26
#define TFT_CLK_DIVIDE_21       0x28
#define TFT_CLK_DIVIDE_22       0x2A
#define TFT_CLK_DIVIDE_23       0x2C
#define TFT_CLK_DIVIDE_24       0x2E
#define TFT_CLK_DIVIDE_25       0x30
#define TFT_CLK_DIVIDE_26       0x32
#define TFT_CLK_DIVIDE_27       0x34
#define TFT_CLK_DIVIDE_28       0x36
#define TFT_CLK_DIVIDE_29       0x38
#define TFT_CLK_DIVIDE_30       0x3A
#define TFT_CLK_DIVIDE_31       0x3C
#define TFT_CLK_DIVIDE_32       0x3E
#endif

typedef enum {
	BACKLIGHT_LEVEL_0,
	BACKLIGHT_LEVEL_1,
	BACKLIGHT_LEVEL_2,
	BACKLIGHT_LEVEL_3,
	BACKLIGHT_LEVEL_4,
	BACKLIGHT_LEVEL_5,
	BACKLIGHT_LEVEL_6,
	BACKLIGHT_LEVEL_7
}ENUM_BACK_LIGHT;


extern void tft_init(void);
extern void tft_start(INT32U model);
extern void tft_clk_set(INT32U clk);
extern void tft_dith_en_set(BOOLEAN status);
extern void tft_slide_en_set(BOOLEAN status);
extern INT32S tft_led_backlight_level_set(INT8U level);
extern INT32S tft_ccfl_backlight_level_set(INT8U level);
extern INT32S tft_pwm0_en_set(BOOLEAN status);
extern INT32S tft_backlight_en_set(BOOLEAN status);
extern void  tft_start_with_pwm(INT32U model,BOOLEAN pwm0_vghl_en,BOOLEAN pwm1_backlight_en);
extern void tft_tft_en_set(BOOLEAN status);
extern void tft_i2c_write(INT32U adr, INT32U value);
extern void tft_gamma_en_set(BOOLEAN status);
extern void tft_display_buffer_set(INT32U value);
extern void tft_display_range_save(void);
extern void tft_restricted_display(INT32U w, INT32U h);
extern void tft_display_range_restore(void);


//MATRE KEYSCAN
extern void matre_keyscaninit(void);
extern void matre_scaninit(void);

// JPEG
// GPL326XXB JPEG DECODE WITH UNION FIFO  
#define C_JPG_CTRL_YUV444				0x00000000
#define C_JPG_CTRL_YUV422				0x00000010
#define C_JPG_CTRL_YUV420				0x00000020
#define C_JPG_CTRL_YUV411				0x00000030
#define C_JPG_CTRL_GRAYSCALE			0x00000040
#define C_JPG_CTRL_YUV422V				0x00000050
#define C_JPG_CTRL_YUV411V				0x00000060
#define C_JPG_CTRL_YUV420H2				0x00000120
#define C_JPG_CTRL_YUV420V2				0x00000220
#define C_JPG_CTRL_YUV411H2				0x00000130
#define C_JPG_CTRL_YUV411V2				0x00000260

#define C_JPG_FIFO_DISABLE				0x00000000
#define C_JPG_FIFO_16LINE				0x00000009
#define C_JPG_FIFO_32LINE				0x00000001
#define C_JPG_FIFO_64LINE				0x00000003
#define C_JPG_FIFO_128LINE				0x00000005
#define C_JPG_FIFO_256LINE				0x00000007

#define C_JPG_STATUS_DECODING			0x00000001
#define C_JPG_STATUS_ENCODING			0x00000002
#define C_JPG_STATUS_INPUT_EMPTY		0x00000004
#define C_JPG_STATUS_OUTPUT_FULL		0x00000008
#define C_JPG_STATUS_DECODE_DONE		0x00000010
#define C_JPG_STATUS_ENCODE_DONE		0x00000020
#define C_JPG_STATUS_STOP				0x00000040
#define C_JPG_STATUS_TIMEOUT			0x00000080
#define C_JPG_STATUS_INIT_ERR			0x00000100
#define C_JPG_STATUS_RST_VLC_DONE		0x00000200
#define C_JPG_STATUS_RST_MARKER_ERR		0x00000400
#define C_JPG_STATUS_SCALER_DONE		0x00008000

// JPEG init API
extern void jpeg_init(void);

// JPEG APIs for setting Quantization table
extern INT32S jpeg_quant_luminance_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_quant_chrominance_set(INT32U offset, INT32U len, INT16U *val);

// JPEG APIs for setting Huffman table
extern INT32S jpeg_huffman_dc_lum_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_dc_lum_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_dc_lum_huffval_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_dc_chrom_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_dc_chrom_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_dc_chrom_huffval_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_lum_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_ac_lum_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_lum_huffval_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_chrom_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_ac_chrom_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_chrom_huffval_set(INT32U offset, INT32U len, INT8U *val);

// JPEG APIs for setting image relative parameters
extern INT32S jpeg_restart_interval_set(INT16U interval);
extern INT32S jpeg_image_size_set(INT16U hsize, INT16U vsize);		// hsize<=8000, vsize<=8000, Decompression: hsize must be multiple of 16, vsize must be at least multiple of 8
extern INT32S jpeg_yuv_sampling_mode_set(INT32U mode);				// mode=C_JPG_CTRL_YUV422/C_JPG_CTRL_YUV420/C_JPG_CTRL_YUV444/C_JPG_CTRL_YUV411/C_JPG_CTRL_GRAYSCALE/C_JPG_CTRL_YUV422V/C_JPG_CTRL_YUV411V/C_JPG_CTRL_YUV420H2/C_JPG_CTRL_YUV420V2/C_JPG_CTRL_YUV411H2/C_JPG_CTRL_YUV411V2
extern INT32S jpeg_clipping_mode_enable(void);						// Decompression: When clipping mode is enabled, JPEG will output image data according to jpeg_image_clipping_range_set()
extern INT32S jpeg_clipping_mode_disable(void);
extern INT32S jpeg_image_clipping_range_set(INT16U start_x, INT16U start_y, INT16U width, INT16U height);	// x, y, width, height must be at lease 8-byte alignment
extern INT32S jpeg_level2_scaledown_mode_enable(void);				// Decompression: When level2 scale-down mode is enabled, output size will be 1/2 in width and height
extern INT32S jpeg_level2_scaledown_mode_disable(void);
extern INT32S jpeg_level8_scaledown_mode_enable(void);				// Decompression: When level8 scale-down mode is enabled, output size will be 1/8 in width and height
extern INT32S jpeg_level8_scaledown_mode_disable(void);
extern INT32S jpeg_using_scaler_mode_enable(void);					// Decompression: When this mode is enabled, JPEG and Scaler engines work together without zoom function. Only YUV420 and YUV422 are supported in this mode.
extern INT32S jpeg_using_scaler_mode_disable(void);
extern INT32S jpeg_level2_scaledown_union_mode_enable(INT32U enable);
extern INT32S jpeg_level4_scaledown_union_mode_enable(INT32U enable);
extern INT32S jpeg_encode_bitstream_offset_set(INT32U length, INT32U offset);
extern void jpeg_decode_use_scaler_sram(INT32U enable);
extern void jpeg_decode_use_mjpeg_output(INT32U enable);
extern INT32S jpeg_progressive_mode_enable(void);					// Decompression: When this mode is enabled, JPEG will do de-quantization and IDCT only. VLD huffman decoding is bypassed.
extern INT32S jpeg_progressive_mode_disable(void);
extern INT32S jpeg_yuyv_encode_mode_enable(void);					// Compression: When this mode is enabled, JPEG will enocde YUYV data instead of Y/U/V separating data. Only YUV420 and YUV422 are supported in this mode. Scaler should not be used under this mode.
extern INT32S jpeg_yuyv_encode_mode_disable(void);

// JPEG APIs for setting Y/Cb/Cr or YUYV data
extern INT32S jpeg_yuv_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Compression: input addresses(8-byte alignment for normal compression, 16-bytes alignment for YUYV data compression); Decompression: output addresses(8-byte alignment)
extern INT32S jpeg_multi_yuv_input_init(INT32U len);				// Compression: When YUYV mode is used, it should be the length of first YUYV data that will be compressed. It should be sum of Y,U and V data when YUV separate mode is used

// JPEG APIs for setting entropy encoded data address and length
extern INT32S jpeg_vlc_addr_set(INT32U addr);						// Compression: output VLC address, addr must be 16-byte alignment. Decompression: input VLC address
extern INT32S jpeg_vlc_maximum_length_set(INT32U len);				// Decompression: JPEG engine stops when maximum VLC bytes are read(Maximum=0x03FFFFFF)
extern INT32S jpeg_multi_vlc_input_init(INT32U len);				// Decompression: length(Maximum=0x000FFFFF) of first VLC buffer

// JPEG APIs for setting output FIFO line
extern INT32S jpeg_fifo_line_set(INT32U line);						// Decompression: FIFO line number(C_JPG_FIFO_DISABLE/C_JPG_FIFO_ENABLE/C_JPG_FIFO_16LINE/C_JPG_FIFO_32LINE/C_JPG_FIFO_64LINE/C_JPG_FIFO_128LINE/C_JPG_FIFO_256LINE)

// JPEG start, restart and stop function APIs
extern INT32S jpeg_compression_start(void);
extern INT32S jpeg_multi_yuv_input_restart(INT32U y_addr, INT32U u_addr, INT32U v_addr, INT32U len);	// Compression: Second, third, ... YUV addresses(8-byte alignment for normal compression, 16-bytes alignment for YUYV data compression) and lengths(Maximum=0x000FFFFF)
extern INT32S jpeg_decompression_start(void);
extern INT32S jpeg_multi_vlc_input_restart(INT32U addr, INT32U len);// Decompression: Second, third, ... VLC addresses(16-byte alignment) and lengths(Maximum=0x000FFFFF)
extern INT32S jpeg_yuv_output_restart(void);						// Decompression: Restart JPEG engine when FIFO is full
extern void jpeg_stop(void);

// JPEG API for scaler ISR to send Done message
extern void jpeg_using_scaler_decode_done(void);

// JPEG status polling API
extern INT32S jpeg_status_polling(INT32U wait);						// If wait is 0, this function returns immediately. Return value:C_JPG_STATUS_INPUT_EMPTY/C_JPG_STATUS_OUTPUT_FULL/C_JPG_STATUS_DECODE_DONE/C_JPG_STATUS_ENCODE_DONE/C_JPG_STATUS_STOP/C_JPG_STATUS_TIMEOUT/C_JPG_STATUS_INIT_ERR
extern INT32U jpeg_compress_vlc_cnt_get(void);						// This API returns the total bytes of VLC data stream that JPEG compression has been generated

// Scaler
#define	C_SCALER_CTRL_IN_RGB1555		0x00000000
#define	C_SCALER_CTRL_IN_RGB565			0x00000001
#define	C_SCALER_CTRL_IN_RGBG			0x00000002
#define	C_SCALER_CTRL_IN_GRGB			0x00000003
#define	C_SCALER_CTRL_IN_YUYV			0x00000004
#define	C_SCALER_CTRL_IN_UYVY			0x00000005
#define	C_SCALER_CTRL_IN_YUYV8X32		0x00100006
#define	C_SCALER_CTRL_IN_YUYV8X64		0x00100007
#define	C_SCALER_CTRL_IN_YUYV16X32		0x00200006
#define	C_SCALER_CTRL_IN_YUYV16X64		0x00200007
#define	C_SCALER_CTRL_IN_YUYV32X32		0x00000006
#define	C_SCALER_CTRL_IN_YUYV64X64		0x00000007
#define	C_SCALER_CTRL_IN_YUV422			0x00000008
#define	C_SCALER_CTRL_IN_YUV420			0x00000009
#define	C_SCALER_CTRL_IN_YUV411			0x0000000A
#define	C_SCALER_CTRL_IN_YUV444			0x0000000B
#define	C_SCALER_CTRL_IN_Y_ONLY			0x0000000C
#define	C_SCALER_CTRL_IN_YUV422V		0x0000000D
#define	C_SCALER_CTRL_IN_YUV411V		0x0000000E

#define	C_SCALER_CTRL_OUT_RGB1555		0x00000000
#define	C_SCALER_CTRL_OUT_RGB565		0x00000010
#define	C_SCALER_CTRL_OUT_RGBG			0x00000020
#define	C_SCALER_CTRL_OUT_GRGB			0x00000030
#define	C_SCALER_CTRL_OUT_YUYV			0x00000040
#define	C_SCALER_CTRL_OUT_UYVY			0x00000050
#define	C_SCALER_CTRL_OUT_YUYV8X32		0x00040060
#define	C_SCALER_CTRL_OUT_YUYV8X64		0x00040070
#define	C_SCALER_CTRL_OUT_YUYV16X32		0x00080060
#define	C_SCALER_CTRL_OUT_YUYV16X64		0x00080070
#define	C_SCALER_CTRL_OUT_YUYV32X32		0x00000060
#define	C_SCALER_CTRL_OUT_YUYV64X64		0x00000070
#define	C_SCALER_CTRL_OUT_YUV422		0x00000080
#define	C_SCALER_CTRL_OUT_YUV420		0x00000090
#define	C_SCALER_CTRL_OUT_YUV411		0x000000A0
#define	C_SCALER_CTRL_OUT_YUV444		0x000000B0
#define	C_SCALER_CTRL_OUT_Y_ONLY		0x000000C0

#define	C_SCALER_CTRL_TYPE_YUV			0x00000400
#define	C_SCALER_CTRL_TYPE_YCBCR		0x00000000

#define C_SCALER_CTRL_IN_FIFO_DISABLE	0x00000000
#define C_SCALER_CTRL_IN_FIFO_16LINE	0x00001000
#define C_SCALER_CTRL_IN_FIFO_32LINE	0x00002000
#define C_SCALER_CTRL_IN_FIFO_64LINE	0x00003000
#define C_SCALER_CTRL_IN_FIFO_128LINE	0x00004000
#define C_SCALER_CTRL_IN_FIFO_256LINE	0x00005000

#define C_SCALER_CTRL_OUT_FIFO_DISABLE	0x00000000
#define C_SCALER_CTRL_OUT_FIFO_16LINE	0x00400000
#define C_SCALER_CTRL_OUT_FIFO_32LINE	0x00800000
#define C_SCALER_CTRL_OUT_FIFO_64LINE	0x00C00000

// Post effect control register
#define	C_SCALER_HISTOGRAM_EN				0x00000001
#define	C_SCALER_Y_GAMMA_EN					0x00000002
#define	C_SCALER_COLOR_MATRIX_EN			0x00000004
#define	C_SCALER_INTERNAL_LINE_BUFFER		0x00000000
#define	C_SCALER_HYBRID_LINE_BUFFER			0x00000010
#define	C_SCALER_EXTERNAL_LINE_BUFFER		0x00000020
#define	C_SCALER_LINE_BUFFER_MODE_MASK		0x00000030

#define C_SCALER_STATUS_INPUT_EMPTY		0x00000001
#define C_SCALER_STATUS_BUSY			0x00000002
#define C_SCALER_STATUS_DONE			0x00000004
#define C_SCALER_STATUS_STOP			0x00000008
#define C_SCALER_STATUS_TIMEOUT			0x00000010
#define C_SCALER_STATUS_INIT_ERR		0x00000020
#define C_SCALER_STATUS_OUTPUT_FULL		0x00000040

// Backward compatible to old scaler driver
#define C_SCALER_CTRL_FIFO_DISABLE		C_SCALER_CTRL_IN_FIFO_DISABLE
#define C_SCALER_CTRL_FIFO_16LINE		C_SCALER_CTRL_IN_FIFO_16LINE
#define C_SCALER_CTRL_FIFO_32LINE		C_SCALER_CTRL_IN_FIFO_32LINE
#define C_SCALER_CTRL_FIFO_64LINE		C_SCALER_CTRL_IN_FIFO_64LINE
#define C_SCALER_CTRL_FIFO_128LINE		C_SCALER_CTRL_IN_FIFO_128LINE
#define C_SCALER_CTRL_FIFO_256LINE		C_SCALER_CTRL_IN_FIFO_256LINE

// Scaler init API
extern void scaler_init(void);

// Scaler config APIs
extern INT32S scaler_image_pixels_set(INT32U input_x, INT32U input_y, INT32U output_x, INT32U output_y);		// 1~8000 pixels

extern INT32S scaler_input_pixels_set(INT32U input_x, INT32U input_y);				// 1~8000 pixels, including the padding pixels
extern INT32S scaler_input_visible_pixels_set(INT32U input_x, INT32U input_y);		// 1~8000 pixels, not including the padding pixels
extern INT32S scaler_input_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Must be 4-byte alignment
extern INT32S scaler_input_format_set(INT32U format);								// C_SCALER_CTRL_IN_RGB1555/C_SCALER_CTRL_IN_RGB565/C_SCALER_CTRL_IN_RGBG/C_SCALER_CTRL_IN_GRGB/C_SCALER_CTRL_IN_YUYV/C_SCALER_CTRL_IN_UYVY/C_SCALER_CTRL_IN_YUV422/C_SCALER_CTRL_IN_YUV420/C_SCALER_CTRL_IN_YUV411/C_SCALER_CTRL_IN_YUV444/C_SCALER_CTRL_IN_Y_ONLY/C_SCALER_CTRL_IN_YUV422V/C_SCALER_CTRL_IN_YUV411V
extern INT32S scaler_input_offset_set(INT32U offset_x, INT32U offset_y);			// Set scaler start x and y position offset

extern INT32S scaler_output_pixels_set(INT32U factor_x, INT32U factor_y, INT32U output_x, INT32U output_y);		// factor_x:(input_x<<16)/output_x (1~0x00FFFFFF), factor_y:(input_y<<16)/output_y, output_x: must be multiple of 16 when output format is YUV422/YUV420/YUV444, multiple of 32 when output format is YUV411, multiple of 8 for others(Maximum 2040 for YUYV8X32 and YUYV8X64, Maximum 4088 for YUYV32X32 and YUYV32X64, Maximum 8000 pixels for others), output_y: 1~8000 pixels
extern INT32S scaler_output_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Must be 4-byte alignment
extern INT32S scaler_output_format_set(INT32U format);								// C_SCALER_CTRL_OUT_RGB1555/C_SCALER_CTRL_OUT_RGB565/C_SCALER_CTRL_OUT_RGBG/C_SCALER_CTRL_OUT_GRGB/C_SCALER_CTRL_OUT_YUYV/C_SCALER_CTRL_OUT_UYVY/C_SCALER_CTRL_OUT_YUYV8X32/C_SCALER_CTRL_OUT_YUYV8X64/C_SCALER_CTRL_OUT_YUYV16X32/C_SCALER_CTRL_OUT_YUYV16X64/C_SCALER_CTRL_OUT_YUYV32X32/C_SCALER_CTRL_OUT_YUYV64X64/C_SCALER_CTRL_OUT_YUV422/C_SCALER_CTRL_OUT_YUV420/C_SCALER_CTRL_OUT_YUV411/C_SCALER_CTRL_OUT_YUV444/C_SCALER_CTRL_OUT_Y_ONLY

extern INT32S scaler_fifo_line_set(INT32U mode);									// C_SCALER_CTRL_FIFO_DISABLE/C_SCALER_CTRL_FIFO_16LINE/C_SCALER_CTRL_FIFO_32LINE/C_SCALER_CTRL_FIFO_64LINE/C_SCALER_CTRL_FIFO_128LINE/C_SCALER_CTRL_FIFO_256LINE
extern INT32S scaler_input_fifo_line_set(INT32U mode);								// C_SCALER_CTRL_IN_FIFO_DISABLE/C_SCALER_CTRL_IN_FIFO_16LINE/C_SCALER_CTRL_IN_FIFO_32LINE/C_SCALER_CTRL_IN_FIFO_64LINE/C_SCALER_CTRL_IN_FIFO_128LINE/C_SCALER_CTRL_IN_FIFO_256LINE
extern INT32S scaler_output_fifo_line_set(INT32U mode);								// C_SCALER_CTRL_OUT_FIFO_DISABLE/C_SCALER_CTRL_OUT_FIFO_16LINE/C_SCALER_CTRL_OUT_FIFO_32LINE/C_SCALER_CTRL_OUT_FIFO_64LINE
extern INT32S scaler_YUV_type_set(INT32U type);										// C_SCALER_CTRL_TYPE_YUV/C_SCALER_CTRL_TYPE_YCBCR
extern INT32S scaler_out_of_boundary_mode_set(INT32U mode);							// mode: 0=Use the boundary data of the input picture, 1=Use Use color defined in scaler_out_of_boundary_color_set()
extern INT32S scaler_out_of_boundary_color_set(INT32U ob_color);					// The format of ob_color is Y-Cb-Cr
extern INT32S scaler_line_buffer_mode_set(INT32U mode);								// mode: C_SCALER_INTERNAL_LINE_BUFFER/C_SCALER_HYBRID_LINE_BUFFER/C_SCALER_EXTERNAL_LINE_BUFFER
extern INT32S scaler_external_line_buffer_set(INT32U addr);							// Must be 4-byte alignment

// Scaler special mode for Motion-JPEG
extern INT32S scaler_bypass_zoom_mode_enable(void);

// Scaler start, restart and stop function APIs
extern INT32S scaler_start(void);
extern INT32S scaler_restart(void);
extern void scaler_stop(void);

// Scaler device protection APIs, used by Montion-JPEG in JPEG driver
extern INT32S scaler_device_protect(void);
extern void scaler_device_unprotect(INT32S mask);

// Scaler status polling APIs
extern INT32S scaler_wait_idle(void);
extern INT32S scaler_status_polling(void);


/* Dominant add , 04/14/2008 */
// scaler backen process
typedef struct {
	INT32U SCM_A11;  /*SCM: Scalar Color Matrix*/
    INT32U SCM_A12;
    INT32U SCM_A13;
    INT32U SCM_A21;
    INT32U SCM_A22;
    INT32U SCM_A23;
    INT32U SCM_A31;
    INT32U SCM_A32;
    INT32U SCM_A33;
} COLOR_MATRIX_CFG;

extern INT32S scaler_gamma_switch(INT8U gamma_switch);
extern void scaler_Y_gamma_config(INT8U gamma_table_id, INT8U gain_value);
extern INT32S scaler_color_matrix_switch(INT8U color_matrix_switch);
extern INT32S scaler_color_matrix_config(COLOR_MATRIX_CFG *color_matrix);


#if _DRV_L1_GTE == 1
//division
#define R_GTE_DIVA					(*((volatile INT32U *) 0xF60040A0))
#define R_GTE_DIVB					(*((volatile INT32U *) 0xF60040A4))
#define R_GTE_DIVOF					(*((volatile INT32U *) 0xF60040A8))
#define R_GTE_DIVO					(*((volatile INT32U *) 0xF60040AC))
#define R_GTE_DIVR					(*((volatile INT32U *) 0xF60040B0))

#define INT16U_DIVISION(dividend, divisor, quotient, residue) \
{\
  	R_GTE_DIVOF = 7;\
  	R_GTE_DIVA = (INT32U) (dividend);\
  	R_GTE_DIVB = (INT32U) (divisor);\
    (quotient) = (INT16U) R_GTE_DIVO;\
    (residue) = (INT16U) R_GTE_DIVR;\
}

#define INT16S_DIVISION(dividend, divisor, quotient, residue) \
{\
  	R_GTE_DIVOF = 6;\
  	R_GTE_DIVA = (INT32S) (dividend);\
  	R_GTE_DIVB = (INT32S) (divisor);\
    (quotient) = (INT16S) R_GTE_DIVO;\
    (residue) = (INT16S) R_GTE_DIVR;\
}

#define INT32U_DIVISION(dividend, divisor, quotient, residue) \
{\
  	R_GTE_DIVOF = 7;\
  	R_GTE_DIVA = (INT32U) (dividend);\
  	R_GTE_DIVB = (INT32U) (divisor);\
    (quotient) = R_GTE_DIVO;\
    (residue) = R_GTE_DIVR;\
}

#define INT32S_DIVISION(dividend, divisor, quotient, residue) \
{\
  	R_GTE_DIVOF = 6;\
  	R_GTE_DIVA = (INT32S) (dividend);\
  	R_GTE_DIVB = (INT32S) (divisor);\
    (quotient) = (INT32S) R_GTE_DIVO;\
    (residue) = (INT32S) R_GTE_DIVR;\
}
#endif

#define	R_RANDOM0					(*((volatile INT32U *) 0xD0500380))
#define	R_RANDOM1					(*((volatile INT32U *) 0xD0500384))

#define GET_RANDOM0()				R_RANDOM0
#define GET_RANDOM1()				R_RANDOM1

// SD Controller: APIs for SD controller
INT32S drvl1_sdc_init(INT32U device_id);
INT32S drvl1_sdc_enable(INT32U device_id);
INT32S drvl1_sdc_disable(INT32U device_id);
INT32S drvl1_sdc_controller_busy_wait(INT32U device_id, INT32U timeout);
INT32S drvl1_sdc_clock_set(INT32U device_id, INT32U speed);
INT32S drvl1_sdc_block_len_set(INT32U device_id, INT32U size);
INT32S drvl1_sdc_bus_width_set(INT32U device_id, INT8U width);
INT32S drvl1_sdc_stop_controller(INT32U device_id, INT32U timeout);
// SD Controller: APIs for SD card
extern INT32S drvl1_sdc_card_init_74_cycles(INT32U device_id);
extern INT32S drvl1_sdc_card_busy_wait(INT32U device_id, INT32U timeout);
extern INT32S drvl1_sdc_command_send(INT32U device_id, INT32U command, INT32U argument);
extern INT32S drvl1_sdc_response_get(INT32U device_id, INT32U *response, INT32U timeout);
extern INT32S drvl1_sdc_data_get(INT32U device_id, INT32U *buf, INT32U timeout);
extern INT32S drvl1_sdc_command_complete_wait(INT32U device_id, INT32U timeout);
extern void drvl1_sdc_clear_rx_data_register(INT32U device_id);
extern INT32S drvl1_sdc_read_data_by_dma(INT32U device_id, INT32U *buffer, INT32U sector_count, INT8S *poll_status);
extern INT32S drvl1_sdc_write_data_by_dma(INT32U device_id, INT32U *buffer, INT32U sector_count, INT8S *poll_status);
extern INT32S drvl1_sdc_data_complete_wait(INT32U device_id, INT32U timeout);
extern INT32S drvl1_sdc_data_crc_status_get(INT32U device_id);

// WatchDog
extern void watchdog_init(void);
extern INT32U watchdog_ctrl(INT32U WDGPD, INT32U WDGS, INT32U WDGEN);
extern INT32U watchdog_clear(void);

// USB Host
#ifndef __USBH_TYPEDEF__
#define __USBH_TYPEDEF__
typedef struct  {
		unsigned long DataTransferLength;
		unsigned int CBWFlag;
		unsigned int LUN;
		unsigned int CBLength;
}CBW_Struct,*BW_Struct;
typedef struct  {
		unsigned int OPCode;
		unsigned long LBA;
		unsigned int TXParameter;
}SCSI_Struct;
#endif //__USBH_TYPEDEF__

extern INT16U  g_usbh_conntect;
extern CBW_Struct  CBWStruct;
extern SCSI_Struct SCSIStruct;

extern INT32U arUSBHostBuffer[64];
extern INT32U arUSBHostCSWBuffer[16];
extern INT32U INTINEP;
extern INT32U BulkINEP ;
extern INT32U BulkOUTEP;
extern INT32U EP0MaxPacketSize;
extern INT32U ResetUSBHost ;
extern INT32U R_Device_Address;

extern INT32U DeviceClass;
extern INT32U  USBH_MaxLUN;
extern INT32U  INTPipeDataToggle ;
extern INT32U  BulkINDataToggle  ;
extern INT32U  BulkOUTDataToggle ;
extern INT32U  R_USBHDMAEN       ;
extern INT32U  R_USBHLUNStatus   ;
extern INT32U  USBSectorSize	   ;
extern INT32U  CapacitySize	   ;

extern INT32U InterfaceProtocol;
extern INT32U InterfaceNumber;
extern INT32U InterfaceSubClass;
extern INT32U InterfaceNumber;
extern INT32U INTPipeDataToggle;
extern INT32U USB_Host_Disable(void);
extern INT32U USB_Host_Uninitial(void);
extern INT32U USB_Host_Initial(void) ;
extern void USBH_INT_CLR_IMMEDIATE(void);
extern INT32U USBH_INT_CLR(void);
extern INT32U USBH_INT_R(void);
extern void USBH_INT_W(INT32U value);
extern INT32U usb_host_drv_l1_init(void);
extern void F_Delay(void);
extern void USBH_Delay(INT32U t);
extern void USB_Host_DevAdr(INT32U addr);
extern void USB_Host_EndPoint(INT32U edp);
extern INT32U USBH_Device_Reset(void);
extern INT32U CheckPortStatus(void);
extern INT32U Data_IN(INT32U DataTransferLength ,INT32U* GetHostBuffer_PTR);
extern INT32U GetConfigurationNumber(void);
extern INT32U DecodeDescriptorConfiguration(void);
extern INT32U GetRequestData(void);
extern INT32U EP0StandardCommand(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U EP0_Data_Stage_OUT(INT32U Data_Number);
extern INT32U EP0_Data_Stage_IN(INT32U Data_Number);
extern INT32U Send_EP0_Status_Stage(INT32U idata);
extern INT32U Send_EP0_Status_Stage(INT32U idata);
extern INT32U INT_IN(void);
extern void USB_Host_Handup(INT16U dpdm);

// HAL layer functions
extern void F_Delay(void);
extern void USBH_Delay(INT32U t);
extern void USB_Host_DevAdr(INT32U addr);
extern void USB_Host_EndPoint(INT32U edp);
extern INT32U USBH_Device_Reset(void);
extern void Enable_USB_PLUG_IN(void);
extern void Disable_USB_PLUG_IN(void);
extern void Enable_USB_PLUG_OUT(void);
extern void Disable_USB_PLUG_OUT(void);
extern void USB_Host_Enable(void);
extern INT32U USB_Host_Signal(void);
extern void USB_Host_Clrirqflag(void);
extern void USBPlugOutEN(void);
extern void GetDeviceLength(INT32U Lun);
extern void USB_Host_ClrFIFO(void);

#if MCU_VERSION < GPL326XX
extern INT32S USBH_Device_Plug_IN(void);
#else
extern INT32S USBH_Device_Plug_IN(INT8U wait);
#endif

extern INT32U CSWStatus(INT32U * GetHostBuffer_PTR);
extern INT32U GetMaxLUNNumber(void);
extern void USBH_SELF_RST(void);
extern INT32U USBG_GET_ACK_CNT(void);
// CF card function definition
extern void     cfc_uninit(void);
extern INT32S	cfc_initial(void);
extern INT32U	cfc_get_total_sector(void);
extern INT32S	cfc_read_sector(INT32U sector, INT32U *buffer);
extern INT32S	cfc_write_sector(INT32U sector, INT32U *buffer);
extern INT32S	cfc_read_multi_sector(INT32U sector, INT32U *buffer, INT32U num);
extern INT32S	cfc_write_multi_sector(INT32U sector, INT32U *buffer, INT32U num);

extern  INT32S	cfc_multiread_cmds(INT32U sector, INT32U num);
extern  INT32S	cfc_read_dma( INT32U *buffer ,INT8U ifwait,INT32U seccount);
extern  INT32S	cfc_multiwrite_cmds(INT32U sector, INT32U num);
extern  INT32S	cfc_write_dma( INT32U *buffer,INT8U ifwait,INT32U seccount);

// Deflicker
extern void deflicker_init(void);
extern INT32S deflicker_start(INT32U resolution, INT32U wait_mode, INT32U *input_buffer, INT32U *output_buffer, OS_EVENT *reply_queue);

// PPU
#define	C_PPU_TEXT1							0
#define	C_PPU_TEXT2							1
#define	C_PPU_TEXT3							2
#define	C_PPU_TEXT4							3
#define R_PPU_SPRITE_X0					(*((volatile INT32U *) 0xD0500300))	// Sprite X0 register
#define R_PPU_SPRITE_Y0					(*((volatile INT32U *) 0xD0500304))	// Sprite Y0 register
#define R_PPU_SPRITE_X1					(*((volatile INT32U *) 0xD0500308))	// Sprite X1 register
#define R_PPU_SPRITE_Y1					(*((volatile INT32U *) 0xD050030C))	// Sprite Y1 register
#define R_PPU_SPRITE_X2					(*((volatile INT32U *) 0xD0500310))	// Sprite X2 register
#define R_PPU_SPRITE_Y2					(*((volatile INT32U *) 0xD0500314))	// Sprite Y2 register
#define R_PPU_SPRITE_X3					(*((volatile INT32U *) 0xD0500318))	// Sprite X3 register
#define R_PPU_SPRITE_Y3					(*((volatile INT32U *) 0xD050031C))	// Sprite Y3 register
#define R_PPU_SPRITE_W0					(*((volatile INT32U *) 0xD0500320))	// Sprite Word 1 register
#define R_PPU_SPRITE_W1					(*((volatile INT32U *) 0xD0500324))	// Sprite Word 2 register
#define R_PPU_SPRITE_W2					(*((volatile INT32U *) 0xD0500328))	// Sprite Word 5 register
#define R_PPU_SPRITE_W3					(*((volatile INT32U *) 0xD050032C))	// Sprite Word 6 register
#define R_PPU_SPRITE_W4					(*((volatile INT32U *) 0xD0500330))	// Sprite Word 7 register

#define PPU_SPRITE_25D_POSITION_CONVERT(x0,y0,x1,y1,x2,y2,x3,y3, w1,w2,w5,w6,w7) \
{\
	R_PPU_SPRITE_X0 = (INT32U) (x0);\
	R_PPU_SPRITE_Y0 = (INT32U) (y0);\
	R_PPU_SPRITE_X1 = (INT32U) (x1);\
	R_PPU_SPRITE_Y1 = (INT32U) (y1);\
	R_PPU_SPRITE_X2 = (INT32U) (x2);\
	R_PPU_SPRITE_Y2 = (INT32U) (y2);\
	R_PPU_SPRITE_X3 = (INT32U) (x3);\
	R_PPU_SPRITE_Y3 = (INT32U) (y3);\
	w1 = R_PPU_SPRITE_W0;\
	w2 = R_PPU_SPRITE_W1;\
	w5 = R_PPU_SPRITE_W2;\
	w6 = R_PPU_SPRITE_W3;\
	w7 = R_PPU_SPRITE_W4;\
}

#define	R_PPU_SPRITE_CTRL					(*((volatile INT32U *) 0xD0500108))
#define cdm_enable                          0x20000
#define GPSP_CMD_COMPARE()                  (R_PPU_SPRITE_CTRL & cdm_enable)

typedef struct {
	INT32U update_register_flag;			// This flag indicates which parts of the register sets should be updated
// Updated by DMA engine
#define C_UPDATE_REG_SET_PALETTE0			 0x01000000
#define C_UPDATE_REG_SET_PALETTE1			 0x02000000
#define C_UPDATE_REG_SET_PALETTE2			 0x04000000
#define C_UPDATE_REG_SET_PALETTE3			 0x08000000
#define C_UPDATE_REG_SET_HORIZONTAL_MOVE	 0x10000000
#define C_UPDATE_REG_SET_TEXT1_HCOMPRESS	 0x20000000
#define C_UPDATE_REG_SET_TEXT3_25D			 0x40000000
#define C_UPDATE_REG_SET_SPRITE_ATTRIBUTE	 0x80000000
#define C_UPDATE_REG_SET_SPRITE_EX_ATTRIBUTE 0x00800000
#define C_UPDATE_REG_SET_DMA_MASK			 0xFF800000
// Updated by CPU
#define C_UPDATE_REG_SET_PPU				0x00000001
#define C_UPDATE_REG_SET_TEXT1				0x00000002
#define C_UPDATE_REG_SET_TEXT2				0x00000004
#define C_UPDATE_REG_SET_TEXT3				0x00000008
#define C_UPDATE_REG_SET_TEXT4				0x00000010
#define C_UPDATE_REG_SET_SPRITE				0x00000020

	// Registers moved by PPU DMA engine
	INT32U ppu_palette0_ptr;				// Updated when C_UPDATE_REG_SET_PALETTE0 is set in update_register_flag
	INT32U ppu_palette1_ptr;				// Updated when C_UPDATE_REG_SET_PALETTE1 is set in update_register_flag
	INT32U ppu_palette2_ptr;				// Updated when C_UPDATE_REG_SET_PALETTE2 is set in update_register_flag
	INT32U ppu_palette3_ptr;				// Updated when C_UPDATE_REG_SET_PALETTE3 is set in update_register_flag
	INT32U ppu_horizontal_move_ptr;			// Updated when C_UPDATE_REG_SET_HORIZONTAL_MOVE is set in update_register_flag
	INT32U ppu_text1_hcompress_ptr;			// Updated when C_UPDATE_REG_SET_TEXT1_HCOMPRESS is set in update_register_flag
	INT32U ppu_text3_25d_ptr;				// Updated when C_UPDATE_REG_SET_TEXT3_25D is set in update_register_flag
	INT32U ppu_sprite_attribute_ptr;		// Updated when C_UPDATE_REG_SET_SPRITE_ATTRIBUTE is set in update_register_flag
    INT32U ppu_sprite_ex_attribute_ptr;     // Updated when C_UPDATE_REG_SET_SPRITE_EX_ATTRIBUTE is set in update_register_flag 
    
	// PPU relative registers. Updated when C_UPDATE_REG_SET_PPU is set in update_register_flag
	INT16U ppu_blending_level;				// R_PPU_BLENDING
	INT16U ppu_fade_control;				// R_PPU_FADE_CTRL
	INT32U ppu_palette_control;				// R_PPU_PALETTE_CTRL
	INT32U ppu_rgb565_transparent_color;	// R_PPU_BLD_COLOR
	INT32U ppu_window1_x;					// R_PPU_WINDOW0_X
	INT32U ppu_window1_y;					// R_PPU_WINDOW0_Y
	INT32U ppu_window2_x;					// R_PPU_WINDOW1_X
	INT32U ppu_window2_y;					// R_PPU_WINDOW1_Y
	INT32U ppu_window3_x;					// R_PPU_WINDOW2_X
	INT32U ppu_window3_y;					// R_PPU_WINDOW2_Y
	INT32U ppu_window4_x;					// R_PPU_WINDOW3_X
	INT32U ppu_window4_y;					// R_PPU_WINDOW3_Y
	INT32U ppu_enable;						// R_PPU_ENABLE
	INT32U ppu_misc;                        // R_PPU_MISC                        
	INT32U ppu_free_mode;                   // R_FREE_SIZE  	
	INT32U ppu_special_effect_rgb;			// R_PPU_RGB_OFFSET

	// TEXT relative registers
	struct ppu_text_register_sets {
		INT16U position_x;					// R_PPU_TEXT1_X_OFFSET
		INT16U position_y;					// R_PPU_TEXT1_Y_OFFSET
		INT16U offset_x;					// R_PPU_TEXT1_X_OFFSET
		INT16U offset_y;					// R_PPU_TEXT1_Y_OFFSET
		INT32U attribute;					// R_PPU_TEXT1_ATTRIBUTE
		INT32U control;						// R_PPU_TEXT1_CTRL
		INT32U n_ptr;						// P_PPU_TEXT1_N_PTR
		INT32U a_ptr;						// P_PPU_TEXT1_A_PTR
		INT16U sine;						// R_PPU_TEXT1_SINE
		INT16U cosine;						// R_PPU_TEXT1_COSINE
		INT32U segment;						// R_PPU_TEXT1_SEGMENT
	} text[4];
	INT16U ppu_vcompress_value;				// R_PPU_VCOMP_VALUE
	INT16U ppu_vcompress_offset;			// R_PPU_VCOMP_OFFSET
	INT16U ppu_vcompress_step;				// R_PPU_VCOMP_STEP
	INT16U text3_25d_y_compress;			// R_PPU_Y25D_COMPRESS

	// Sprite relative registers. Updated when C_UPDATE_REG_SET_SPRITE is set in update_register_flag
	INT32U sprite_control;					// R_PPU_SPRITE_CTRL
	INT32U sprite_segment;					// R_PPU_SPRITE_SEGMENT
	INT32U extend_sprite_control;			// R_PPU_EXTENDSPRITE_CONTROL
	INT32U extend_sprite_addr;				// R_PPU_EXTENDSPRITE_ADDR
} PPU_REGISTER_SETS, *PPU_REGISTER_SETS_PTR;

extern void ppu_init(void);
extern void ppu_display_resolution_lock(void);
extern void ppu_display_resolution_unlock(void);
extern INT32S ppu_frame_buffer_add(INT32U *frame_buf);
extern INT32S ppu_frame_buffer_get(void);
extern INT32S ppu_frame_buffer_display(INT32U *frame_buf);
extern INT32S ppu_tft_static_frame_set(INT32U *frame_buf);
extern INT32S ppu_tv_static_frame_set(INT32U *frame_buf);
extern INT32S ppu_deflicker_mode_set(INT32U value);
extern INT32S ppu_go(PPU_REGISTER_SETS *p_register_set, INT32U wait_start, INT32U wait_done);
extern INT8U ppu_frame_mode_busy_get(void);
extern INT32S ppu_notifier_register(void (*notifier)(void));
extern void ppu_notifier_unregister(void);
extern void set_ppu_free_control(INT8U INTL,INT16U H_size,INT16U V_size);
extern INT32S Hblenk_Enable_Set(INT32U value);
extern void Hblenk_Line_Offset_Set(INT32U line_offset);
extern void Hblenk_Go(void);
extern INT32S Hblenk_Wait(void);
extern INT32S set_ppu_go(PPU_REGISTER_SETS *p_register_set);
extern INT32S ppu_setting_update(PPU_REGISTER_SETS *p_register_set);
//====================================================================================================
//	TV Export Functions - Begin
//====================================================================================================
extern void tv_init(void);
extern void tv_start(INT32S nTvStd, INT32S nResolution, INT32S nNonInterlace);
//  nTvStd
#define TVSTD_NTSC_M                    0
#define TVSTD_NTSC_J                    1
#define TVSTD_NTSC_N                    2
#define TVSTD_PAL_M                     3
#define TVSTD_PAL_B                     4
#define TVSTD_PAL_N                     5
#define TVSTD_PAL_NC                    6
#define TVSTD_NTSC_J_NONINTL            7
#define TVSTD_PAL_B_NONINTL             8
//  nResolution
#define TV_QVGA                         0
#define TV_HVGA                         1
#define TV_D1                           2
//  nNonInterlace
#define TV_INTERLACE                    0
#define TV_NON_INTERLACE                1
//====================================================================================================
//	TV Export Functions - End
//====================================================================================================

//====================================================================================================
//	SPU Export Functions - Begin
//====================================================================================================
extern void SPU_Init(void);
extern INT32U SPU_Get_DrvVersion(void);
extern void SPU_Clear_SRAM(void);
extern void SPU_Clear_Register(void);
extern void SPU_Enable_Channel(INT8U ChannelIndex);
extern void SPU_Disable_Channel(INT8U ChannelIndex);
extern void SPU_Enable_MultiChannel(INT32U ChannelBit);
extern void SPU_Disable_MultiChannel(INT32U ChannelBit);
extern INT32U SPU_GetChannelEnableStatus(void);
extern void SPU_Set_MainVolume(INT8U VolumeData);
extern INT8U SPU_Get_MainVolume(void);
extern void SPU_Enable_FIQ_Channel(INT8U ChannelIndex);
extern void SPU_Disable_FIQ_Channel(INT8U ChannelIndex);
extern void SPU_Enable_FIQ_MultiChannel(INT32U ChannelBit);
extern void SPU_Disable_FIQ_MultiChannel(INT32U ChannelBit);
extern void SPU_Clear_FIQ_Status(INT32U ChannelBit);
extern INT32U SPU_Get_FIQ_Status(void);
extern void SPU_Set_BeatBaseCounter(INT16U BeatBaseCounter);
extern INT16U SPU_Get_BeatBaseCounter(void);
extern void SPU_Set_BeatCounter(INT16U BeatCounter);
extern INT16U SPU_Get_BeatCounter(void);
extern void SPU_Enable_BeatIRQ(void);
extern void SPU_Disable_BeatIRQ(void);
extern void SPU_Clear_BeatIRQ_Flag(void);
extern INT8U SPU_Get_BeatIRQ_Flag(void);
extern void SPU_Set_EnvelopeClock(INT8U EnvClock, INT8U ChannelIndex);
extern void SPU_Set_EnvRampDown(INT8U RampDownChannel);
extern void SPU_Set_EnvRampDownMultiChannel(INT32U ChannelBit);
extern INT32U SPU_Get_EnvRampDown(void);
extern void SPU_Clear_Ch_StopFlag(INT8U ChannelIndex);
extern void SPU_Clear_MultiCh_StopFlag(INT32U ChannelBit);
extern INT32U SPU_Get_Ch_StopStatus(void);
extern void SPU_EnableChannelZC(INT8U ChannelIndex);
extern void SPU_DisableChannelZC(INT8U ChannelIndex);
extern void SPU_AccumulatorInit(void);
extern INT8U SPU_Read_FOF_Status(void);
extern void SPU_Clear_FOF(void);
extern void SPU_SingleChVolumeSelect(INT8U VolumeSelect);
extern INT8U SPU_GetSingleChVolumeSetting(void);
extern void SPU_InterpolationON(void);
extern void SPU_InterpolationOFF(void);
extern void SPU_HQ_InterpolationON(void);
extern void SPU_HQ_InterpolationOFF(void);
extern void SPU_CompressorON(void);
extern void SPU_CompressorOFF(void);
extern void SPU_ClearSaturateFlag(void);
extern INT8U SPU_ReadSaturateFlag(void);
extern void SPU_SetCompressorRatio(INT8U ComRatio);
extern INT8U SPU_GetCompressorRatio(void);
extern void SPU_EnableCompZeroCrossing(void);
extern void SPU_DisableCompZeroCrossing(void);
extern void SPU_SetReleaseTimeScale(INT8U ReleaseTimeScale);
extern INT8U SPU_ReadReleaseTimeScale(void);
extern void SPU_SetAttackTimeScale(INT8U AttackTimeScale);
extern INT8U SPU_ReadAttackTimeScale(void);
extern void SPU_SetCompressThreshold(INT8U CompThreshold);
extern INT8U SPU_ReadCompressThreshold(void);
extern void SPU_SelectRMS_Mode(void);
extern void SPU_SelectPeakMode(void);
extern INT32U SPU_GetChannelStatus(void);
extern void SPU_SendToSoftChannel_Left(INT16U PCM_Data);
extern void SPU_SendToSoftChannel_Right(INT16U PCM_Data);
extern INT16U SPU_GetSPU_PlusSoftOutLeft(void);
extern INT16U SPU_GetSPU_PlusSoftOutRight(void);
extern INT16U SPU_GetSPU_OutLeft(void);
extern INT16U SPU_GetSPU_OutRight(void);
extern void SPU_EnableChannelRepeat(INT8U ChannelIndex);
extern void SPU_DisableChannelRepeat(INT8U ChannelIndex);
extern void SPU_EnableMultiChannelRepeat(INT32U ChannelBit);
extern void SPU_DisableMultiChannelRepeat(INT32U ChannelBit);
extern void SPU_EnvelopeAutoMode(INT8U ChannelIndex);
extern void SPU_EnvelopeManualMode(INT8U ChannelIndex);
extern void SPU_SetChannelRelease(INT8U ChannelIndex);
extern INT32U SPU_GetEnvelopeIRQ_Status(void);
extern void SPU_ClearEnvelopeIRQ_Status(INT32U ChannelBit);
extern void SPU_EnablePitchBend(INT8U ChannelIndex);
extern void SPU_DisablePitchBend(INT8U ChannelIndex);
extern void SPU_SetReleaseTime(INT8U ReleaseTime);
extern INT8U SPU_ReadReleaseTime(void);
extern void SPU_SetAttackTime(INT8U AttackTime);
extern INT8U SPU_ReadAttackTime(void);
extern void BenkAddr(INT8U BenkAddr);
extern void SPU_ClearPWFOV_Flag(void);
extern void SPU_EnablePW_RightChannel(void);
extern void SPU_DisablePW_RightChannel(void);
extern INT8U SPU_GetPWFOV_Flag(void);
extern void SPU_EnablePostWaveOutputSilence(void);
extern void SPU_DisablePostWaveOutputSilence(void);
extern void SPU_SetPostWave_Signed(void);
extern void SPU_SetPostWave_Unsigned(void);
extern void SPU_ClearPostWaveIRQ_Status(void);
extern INT8U SPU_GetPostWaveIRQ_Status(void);
extern void SPU_EnablePostWaveIRQ(void);
extern void SPU_DisablePostWaveIRQ(void);
extern void SPU_SetPostWaveClock_288K(void);
extern void SPU_SetPostWaveClock_281K(void);
extern void SPU_EnablePostWaveLPF(void);
extern void SPU_DisablePostWaveLPF(void);
extern void SPU_EnablePostWaveDownSample(void);
extern void SPU_DisablePostWaveDownSample(void);
extern void SPU_EnablePostWaveToDMA(void);
extern void SPU_DisablePostWaveToDMA(void);
extern void SPU_SetStartAddress(INT32U StartAddr, INT8U ChannelIndex);
extern void SPU_SetLoopAddress(INT32U LoopAddr, INT8U ChannelIndex);
extern void SPU_GetStartAddress(INT32U *StartAddr, INT8U ChannelIndex);
extern void SPU_GetLoopAddress(INT32U *LoopAddr, INT8U ChannelIndex);
extern void SPU_SetToneColorMode(INT8U ToneColorMode, INT8U ChannelIndex);
extern void SPU_Set_16bit_Mode(INT8U ChannelIndex);
extern void SPU_Set_8bit_Mode(INT8U ChannelIndex);
extern void SPU_SetADPCM_Mode(INT8U ChannelIndex);
extern void SPU_SetPCM_Mode(INT8U ChannelIndex);
extern void SPU_SetVelocity(INT8U VelocityValue, INT8U ChannelIndex);
extern void SPU_SetPan(INT8U PanValue, INT8U ChannelIndex);
extern void SPU_SetEnvelope_0(INT16U Envelope_0, INT8U ChannelIndex);
extern void SPU_SetEnvelopeIncrement(INT8U EnvInc, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeIncrement(INT8U ChannelIndex);
extern void SPU_SetEnvelopePostive(INT8U ChannelIndex);
extern void SPU_SetEnvelopeNegative(INT8U ChannelIndex);
extern void SPU_SetEnvelopeTarget(INT8U EnvTarget, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeTarget(INT8U ChannelIndex);
extern void SPU_SetEnvelopeData(INT8U EnvData, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeData(INT8U ChannelIndex);
extern void SPU_SetEnvelopeCounter(INT8U EnvCounter, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeCounter(INT8U ChannelIndex);
extern void SPU_SetEnvelope_1(INT16U Envelope_1, INT8U ChannelIndex);
extern void SPU_SetEnvelopeReloadData(INT8U EnvReloadData, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeReloadData(INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatMode(INT8U ChannelIndex);
extern void SPU_SetEnvelopeNormalMode(INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatCounter(INT8U RepeatCounter, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeRepeatCounter(INT8U ChannelIndex);
extern void SPU_SetEnvelopeAddress(INT32U EnvelopeAddr, INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_Enable(INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_Disable(INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_FireAddress(INT16U FireAddr, INT8U ChannelIndex);
extern void SPU_SetWaveData_0(INT16U WDD_0, INT8U ChannelIndex);
extern void SPU_SetWaveData(INT16U WDD, INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatAddrOffset(INT16U EAOffset, INT8U ChannelIndex);
extern void SPU_SetEnvelopeRampDownOffset(INT8U RampDownOffset, INT8U ChannelIndex);
extern void SPU_SelectADPCM_Mode(INT8U ChannelIndex);
extern void SPU_SelectADPCM36_Mode(INT8U ChannelIndex);
extern void SPU_SetADPCM_PointNumber(INT8U PointNumber, INT8U ChannelIndex);
extern void SPU_SetPhase(INT32U Phase, INT8U ChannelIndex);
extern INT32U SPU_ReadPhase(INT8U ChannelIndex);
extern void SPU_SetPhaseAccumulator(INT32U PhaseAcc, INT8U ChannelIndex);
extern INT32U SPU_ReadPhaseAccumulator(INT8U ChannelIndex);
extern void SPU_SetTargetPhase(INT32U TargetPhase, INT8U ChannelIndex);
extern INT32U SPU_ReadTargetPhase(INT8U ChannelIndex);
extern void SPU_SetRampDownClock(INT8U RampDownClock, INT8U ChannelIndex);
extern void SPU_SetPhaseOffset(INT16U PhaseOffset, INT8U ChannelIndex);
extern void SPU_SetPhaseIncrease(INT8U ChannelIndex);
extern void SPU_SetPhaseDecrease(INT8U ChannelIndex);
extern void SPU_SetPhaseTimeStep(INT8U PhaseTimeStep, INT8U ChannelIndex);
extern void SPU_SetPhaseControl(INT16U PhaseControl, INT8U ChannelIndex);
extern INT16U SPU_ReadPhaseControl(INT8U ChannelIndex);
extern void SPU_FIQ_Register(INT8U FIQ_Index, void (*F_ISR)(void));
extern void SPU_FIQ_Enable(INT8U FIQ_Index);
extern void SPU_FIQ_Disable(INT8U FIQ_Index);
extern void SPU_Test_IRQ_Register(INT8U IRQ_Index, void (*F_ISR)(void));
extern void SPU_Test_IRQ_Enable(INT8U IRQ_Index);
extern void SPU_Test_IRQ_Disable(INT8U IRQ_Index);
extern void SPU_Test_Timer_IRQ_Setup(INT32U timer_id, INT32U freq_hz, void (*TimerIsrFunction)(void));
//====================================================================================================
//	SPU Export Functions - End
//====================================================================================================
//usb devicde

typedef struct {
	INT8U	unLunNum;			//Lun number.
	INT8U	unLunType;			//Lun type.
	//unsigned long 	ulNandStartAddr;	//Indicate any operation on this lun should be offset ulNandStartAddr sector, only for LunType_NandDrive or LunType_CDROM.
	INT32U 	ulSecSize;		//Nand flash size sector
	INT16U 	unStatus;			//Status, write protect, no media, media change.
}str_USB_Lun_Info;

extern INT32U   Sense_Code;
extern INT32U  	R_USB_Suspend  ;
extern INT32U  	CSW_Residue;
extern INT32U   USBD_PlugIn;

extern void usbd_setuserstring(INT32U size,INT8U *pbuf);
extern void usbd_setlun(INT16U ilun);
extern void SetLuntpye(INT8U lun, INT8U ltype);
extern void usb_isr_clear(void);
extern void usb_isr(void);
extern void usbd_interrupt_register(void (*pt_function)(void));
extern void usbd_ep0descriptor_register(INT32U (*pt_function)(INT32U nTableType));
extern void usbd_suspend_register(void (*pt_function)(void));
extern void usbd_resume_register(void (*pt_function)(void));
extern void usbd_set_configuration_register(void (*pt_function)(void));
extern void usbd_reset_register(void (*pt_function)(void));
extern void usbd_write10_register(INT32S (*pt_function)(str_USB_Lun_Info* luninfo));
extern void usbd_read10_register(INT32S (*pt_function)(str_USB_Lun_Info* luninfo));
extern void usbd_writeprotected_register (INT32U (*pt_function)(void));
extern void usbd_vendorcallback_register (INT32U (*pt_function)(void));
extern void usbd_CDROM_ReadSector_register(INT32S (*pt_function)(INT32U blkno, INT32U blkcnt, INT32U buf));//huanghe for cdrom 090605
extern void usb_initial(void);
extern void usb_initial_udisk(void);
extern void usb_uninitial(void);
extern void usb_reset(void);
extern void usb_isr(void);
extern void usb_std_service(INT32U unUseLoop);
extern void usb_std_service_udisk(INT32U unUseLoop);
extern void usb_msdc_service( INT32U unUseLoop);
extern  INT32U Receive_From_USB_DMA_USB( INT32U* USB_Buffer_PTR,INT8U playload, INT8U ifcheck);
extern  INT32U Send_To_USB_DMA_USB( INT32U* USB_Buffer_PTR,INT8U playload, INT8U ifcheck);
extern  INT32U GetTransLeng(void);
extern  INT32U GetLBA(void);
extern  INT32U GetCMDValueEx(INT32U index );
extern  void CommandFail(INT32U nSenseIndex);
extern void GetDriveStatus(void);
extern void GetICVersion(void);
extern void SetVenderID(void);
extern void RegWrite(INT32U REG ,INT32U Count);
extern void RegRead(INT32U REG,INT32U Count);
extern INT32U GetDL_length(void);
void JDataWrite(INT32U REG ,INT32U Count,INT32U type);

#if MCU_VERSION >= GPL326XX
void USB_PHY_I2C_CMD(INT8U addr, INT8U data,INT8U type);
#endif

extern void usbd_phy_clock_on(void);


// mspro card driver
#define	C_MS_TIMEOUT				0x0007ffff
extern INT32U MS_Page_Size;
extern INT32U MS_Block_Size;
extern INT32U MS_UsrDataBlk_Size;
extern INT32U MS_RWPage_Size;
extern INT32U MS_TotalPage_Size;
extern INT16U *g_LtP_table;
extern void ms_uninitial(void);
extern INT32S ms_initial(void);
extern INT32U drvmsc_gettotalsector(void);
extern INT32S ms_read_multi_sector(INT32U blkno, INT32U blkcnt, INT32U buf);
extern INT32S ms_write_multi_sector(INT32U blkno, INT32U blkcnt, INT32U buf);
extern void   ms_table_init(INT16U *pbuf);
extern void   ms_hand_set(INT16U ihand);
extern void ms_detective_flag_set(INT8U flag);

extern INT32S ms_multiread_cmd(INT32U blkno, INT32U blkcnt);
extern INT32S ms_read_dma(INT32U *buf,INT8U ifwait);
extern INT32S ms_multiwrite_cmd(INT32U blkno, INT32U blkcnt);
extern INT32S ms_write_dma(INT32U *buf,INT8U ifwait);
extern INT32S ms_readwrite_end(void);


/* be used by msc */
extern INT32U MS_SET_RW_ADDR(INT32U wsize,INT32U waddr,INT32U rsize,INT32U raddr);
extern INT32U MS_READ_REG(INT32U rsize0,INT32U raddr0);// must be less than 4 bytes
extern INT32U MS_WRITE_REG(INT32U wsize0,INT32U waddr0,INT32U data0);// must be less than 4 bytes
extern INT32U MS_GET_INT(void);
extern INT32U MS_SET_CMD(INT32U ARGCMD);

//external interrupt driver
typedef enum
{
	EXTA,
	EXTB
}EXTAB_ENUM;

typedef enum
{
	FALLING,
	RISING
}EXTAB_EDGE_ENUM;

extern void ext_int_init(void);
extern void extab_int_clr(INT8U ext_src);
extern void extab_edge_set(INT8U ext_src, INT8U edge_type);
extern void extab_enable_set(INT8U ext_src, BOOLEAN status);
extern void extab_user_isr_set(INT8U ext_src,void (*user_isr)(void));
extern void extab_user_isr_clr(INT8U ext_src);


//---------------------------------------------------------SPU_L1 080911
// P_SPU_CH_EN (0xD0400E00)
// bit[15:0] : channel[15:0] enable

// P_SPU_CH_EN_HIGH (0xD0400E80)
// bit[15:0] : channel[31:16] enable

// P_SPU_MAIN_VOLUME (0xD0400E04)
// bit[6:0] : spu main volume[6:0]
// bit[15:7] : reserved

// P_SPU_CH_FIQ_EN (0xD0400E08)
// bit[15:0] : channel[15:0] FIQ enable

// P_SPU_CH_FIQ_EN_HIGH(0xD0400E88)
// bit[15:0] : channel[31:16] FIQ enable

// P_SPU_CH_FIQ_STATUS (0xD0400E0C)
// bit[15:0] : channel[15:0] FIQ status

// P_SPU_CH_FIQ_STATUS_HIGH (0xD0400E8C)
// bit[15:0] : channel[31:16] FIQ status

// P_SPU_BEAT_BASE_COUNTER (0xD0400E10)
// bit[10:0] : beat base counter[10:0]
// bit[15:11] : reserved

// P_SPU_BEAT_COUNTER (0xD0400E14)
// bit[13:0] : beat count
// bit[14] : current beat IRQ event status
#define C_BEAT_IRQ_STATUS			0x4000
// bit[15] : beat IRQ enable
#define C_BEAT_IRQ_EN				0x8000

// P_SPU_ENV_CLK_CH0_3 (0xD0400E18)
// bit[3:0] : channel[0] envelope clock selection
// bit[7:4] : channel[1] envelope clock selection
// bit[11:8] : channel[2] envelope clock selection
// bit[15:12] : channel[3] envelope clock selection

// P_SPU_ENV_CLK_CH4_7 (0xD0400E1C)
// bit[3:0] : channel[4] envelope clock selection
// bit[7:4] : channel[5] envelope clock selection
// bit[11:8] : channel[6] envelope clock selection
// bit[15:12] : channel[7] envelope clock selection

// P_SPU_ENV_CLK_CH8_11 (0xD0400E20)
// bit[3:0] : channel[8] envelope clock selection
// bit[7:4] : channel[9] envelope clock selection
// bit[11:8] : channel[10] envelope clock selection
// bit[15:12] : channel[11] envelope clock selection

// P_SPU_ENV_CLK_CH12_15 (0xD0400E24)
// bit[3:0] : channel[12] envelope clock selection
// bit[7:4] : channel[13] envelope clock selection
// bit[11:8] : channel[14] envelope clock selection
// bit[15:12] : channel[15] envelope clock selection

// P_SPU_ENV_CLK_CH16_19 (0xD0400E98)
// bit[3:0] : channel[16] envelope clock selection
// bit[7:4] : channel[17] envelope clock selection
// bit[11:8] : channel[18] envelope clock selection
// bit[15:12] : channel[19] envelope clock selection

// P_SPU_ENV_CLK_CH20_23 (0xD0400E9C)
// bit[3:0] : channel[20] envelope clock selection
// bit[7:4] : channel[21] envelope clock selection
// bit[11:8] : channel[22] envelope clock selection
// bit[15:12] : channel[23] envelope clock selection

// P_SPU_ENV_CLK_CH24_27 (0xD0400EA0)
// bit[3:0] : channel[24] envelope clock selection
// bit[7:4] : channel[25] envelope clock selection
// bit[11:8] : channel[26] envelope clock selection
// bit[15:12] : channel[27] envelope clock selection

// P_SPU_ENV_CLK_CH28_31 (0xD0400EA4)
// bit[3:0] : channel[28] envelope clock selection
// bit[7:4] : channel[29] envelope clock selection
// bit[11:8] : channel[30] envelope clock selection
// bit[15:12] : channel[31] envelope clock selection

// P_SPU_ENV_RAMP_DOWN (0xD0400E28)
// bit[15:0] : channel[15:0] ramp down start

// P_SPU_ENV_RAMP_DOWN_HIGH (0xD0400EA8)
// bit[15:0] : channel[31:16] ramp down start

// P_SPU_CH_STOP_STATUS (0xD0400E2C)
// bit[15:0] : channel[15:0] stop status

// P_SPU_CH_STOP_STATUS_HIGH (0xD0400EAC)
// bit[15:0] : channel[31:16] stop status

// P_SPU_CH_ZC_ENABLE (0xD0400E30)
// bit[15:0] : channel[15:0] zero crossing enable

// P_SPU_CH_ZC_ENABLE_HIGH (0xD0400EB0)
// bit[15:0] : channel[31:16] zero crossing enable

// P_SPU_CONTROL_FLAG (0xD0400E34)
// bit[2:0] : reserved
// bit[3] : Initial channel's accumulator
#define C_INIT_ACC					0x0008
// bit[4] : reserved
// bit[5] : sample rate of tone color too high flag
#define C_FOF_FLAG					0x0020
// bit[7:6] : volume of single channel
#define C_CH_VOL_SEL				0x00C0
#define C_CH_VOL_1_DIV_32			0x0000		// 1/32
#define C_CH_VOL_1_DIV_8			0x0040		// 1/8
#define C_CH_VOL_1_DIV_2			0x0080		// 1/4
#define C_CH_VOL_1_DIV_1			0x00C0		// 1
// bit[8] : reserved
// bit[9] : 1: interpolation off, 0: interpolation on
#define C_NO_INTER					0x0200
// bit[10] : 1: high quality interpolation off, 0: high quality interpolation on
#define C_NO_HIGH_INTER				0x0400
// bit[11] : 1: compressor on, 0: compressor off
#define C_COMP_EN					0x0800
// bit[14:12] : reserved
// bit[15] : signal saturate flag
#define C_SATURATE					0x8000

// P_SPU_COMPRESSOR_CTRL (0xD0400E38)
// bit[2:0] : compress ratio
#define C_COMPRESS_RATIO			0x0007
#define C_COMP_RATIO_1_DIV_2		0x0000
#define C_COMP_RATIO_1_DIV_3		0x0001
#define C_COMP_RATIO_1_DIV_4		0x0002
#define C_COMP_RATIO_1_DIV_5		0x0003
#define C_COMP_RATIO_1_DIV_6		0x0004
#define C_COMP_RATIO_1_DIV_7		0x0005
#define C_COMP_RATIO_1_DIV_8		0x0006
#define C_COMP_RATIO_1_DIV_INIF		0x0007
// bit[3] : 1: disable zero cross, 0: enable zero cross
#define C_DISABLE_ZC				0x0008
// bit[5:4] : release time scale
#define C_RELEASE_SCALE				0x0030
#define C_RELEASE_TIME_MUL_1		0x0000
#define C_RELEASE_TIME_MUL_4		0x0010
#define C_RELEASE_TIME_MUL_16		0x0020
#define C_RELEASE_TIME_MUL_64		0x0030
// bit[7:6] : attack time scale
#define C_ATTACK_SCALE				0x00C0
#define C_ATTACK_TIME_MUL_1			0x0000
#define C_ATTACK_TIME_MUL_4			0x0040
#define C_ATTACK_TIME_MUL_16		0x0080
#define C_ATTACK_TIME_MUL_64		0x00C0
// bit[14:8] : compress threshold, 0x01~0x7F
#define C_COMPRESS_THRESHOLD		0x7F00
// bit[15] : peak mode
#define C_COMPRESS_PEAK_MODE		0x8000

// P_SPU_CH_STATUS (0xD0400E3C)
// bit[15:0] : channel[15:0] status

// P_SPU_CH_STATUS_HIGH (0xD0400EBC)
// bit[15:0] : channel[31:16] status

// P_SPU_WAVE_IN_LEFT (0xD0400E40)
// bit[15:0] : the data is mixed with the left output of SPU

// P_SPU_WAVE_IN_RIGHT (0xD0400E44)
// bit[15:0] : the data is mixed with the right output of SPU

// P_SPU_WAVE_OUT_LEFT (0xD0400E48)
// bit[15:0] : the data is the final left output outcome of 32-channel + software channel

// P_SPU_POST_WAVE_OUT_LEFT (0xD0400EC8)
// bit[15:0] : the data is the left output port of 32-channel

// P_SPU_WAVE_OUT_RIGHT (0xD0400E4C)
// bit[15:0] : the data is the final right output outcome of 32-channel + software channel

// P_SPU_POST_WAVE_OUT_RIGHT (0xD0400ECC)
// bit[15:0] : the data is the right output port of 32-channel

// P_SPU_CH_REPEAT_EN (0xD0400E50)
// bit[15:0] : channel[15:0] envelope repeat enable

// P_SPU_CH_REPEAT_EN_HIGH (0xD0400ED0)
// bit[15:0] : channel[31:16] envelope repeat enable

// P_SPU_CH_ENV_MODE (0xD0400E54)
// bit[15:0] : channel[15:0] envelope mode, 1: manual mode, 0: auto mode

// P_SPU_CH_ENV_MODE_HIGH (0xD0400ED4)
// bit[15:0] : channel[31:16] envelope mode, 1: manual mode, 0: auto mode

// P_SPU_CH_TONE_RELEASE (0xD0400E58)
// bit[15:0] : channel[15:0] tone release

// P_SPU_CH_TONE_RELEASE_HIGH (0xD0400ED8)
// bit[15:0] : channel[31:16] tone release

// P_SPU_CH_IRQ_STATUS (0xD0400E5C)
// bit[15:0] : channel[15:0] envelope IRQ status

// P_SPU_CH_IRQ_STATUS_HIGH (0xD0400EDC)
// bit[15:0] : channel[31:16] envelope IRQ status

// P_SPU_CH_PITCH_BEND_EN (0xD0400E60)
// bit[15:0] : channel[15:0] pitch bend enable

// P_SPU_CH_PITCH_BEND_EN_HIGH (0xD0400EE0)
// bit[15:0] : channel[31:16] pitch bend enable

// P_SPU_ATTACK_RELEASE_TIME (0xD0400E68)
// bit[7:0] : release time control
// bit[15:8] : attack time control

// P_SPU_BENK_ADDR (0xD0400E7C)
// bit[8:0] : wave tables bank address
// bitp15:9] : reserved

// P_SPU_POST_WAVE_CTRL (0xD0400E94)
// bit[0] : internal post wave output queue to DMA overflow
#define C_PW_OVERFLOW				0x0001
// bit[4:1] : reserved
// bit[5] : post wave output right channel enable  0: LLLLLL... 1: LRLRLR....
#define C_PW_LR						0x0020
// bit[6] : 1: post wave output to DAC, 0: post wave do not output to DAC
#define C_PW_TO_DAC					0x0040
// bit[7] : 1: post wave is signed, 0: post wave is unsigned
#define C_PW_SIGNED					0x0080
// bit[9:8] : reserved
// bit[10] : 1: post wave IRQ active, 0: post wave IRQ inactive
#define C_PW_IRQ_ACTIVE				0x0400
// bit[11] : 1: post wave IRQ enable, 0: post wave IRQ disable
#define C_PW_IRQ_ENABLE				0x0800
// bit[12] : 1: post wave processing in 288KHz, 0: post wave processing is not valid
#define C_PW_CLOCK_SET				0x1000
// bit[13] : 1: enable post wave low pass filter, 0: disable post wave low pass filter
#define C_PW_LPF_ENABLE				0x2000
// bit[14] : 1: post wave down sample by 6, 0: post wave not down sample
#define C_PW_DOWN_SAMPLE			0x4000
// bit[15] : 1: post wave output enable, 0: post wave output disable
#define C_PW_DMA					0x8000

// P_SPU_WAVE_ADDR (0xD0401000 + 0x40*X)
// bit[15:0] : wave address[15:0]

// P_SPU_MODE (0xD0401004 + 0x40*X)
// bit[5:0] : wave address[21:16]
// bit[11:6] : loop address[21:6]
// bit[13:12] : tone color control mode
#define C_TONE_COLOR_MODE			0x3000
#define C_SW_MODE					0x0000
#define C_HW_AUTO_END_MODE			0x1000
#define C_HW_AUTO_REPEAT_MODE		0x2000
#define C_HW_AUTO_REPEAT_MODE1		0x3000
// bit[14] : 1: 16-bit data mode, 0: 8-bit data mode
#define C_16BIT_DATA_MODE			0x4000
// bit[15] : 1: ADPCM mode, 0: PCM mode
#define C_ADPCM_MODE				0x8000

// P_SPU_LOOP_ADDR (0xD0401008 + 0x40*X)
// bit[15:0] : loop address[15:0]

// P_SPU_PAN_VELOCITY (0xD040100C + 0x40*X)
// bit[6:0] :  channel volumn[6:0]
// bit[7] : reserved
// bit[14:8] : pan value[6:0]
// bit[15] : reserved

// P_SPU_ENVELOPE_0 (0xD0401010 + 0x40*X)
// bit[6:0] : envelope increment[6:0]
// bit[7] : 1: envelope direction is negative, 0: envelope direction is positive
#define C_ENVELOPE_SIGN				0x0080
// bit[14:8] : envelope target[6:0]
// bit[15] : reserved

// P_SPU_ENVELOPE_DATA (0xD0401014 + 0x40*X)
// bit[6:0] : envelope data, EDD[6:0]
// bit[7] : reserved
// [15:8] : envelope counter[7:0]

// P_SPU_ENVELOPE_1 (0xD0401018 + 0x40*X)
// bit[7:0] : envelope counter re-load data[7:0]
// bit[8] : 1: envelope repeat mode, 0: envelope normal mode
#define C_ENVELOPE_REPEAT			0x0100
// bit[15:9] : envelope repeat counter[6:0]

// P_SPU_ENV_ADDR_HIGH (0xD040101C + 0x40*X)
// bit[5:0] : envelope address[21:16]
// bit[6] : 1: envelope IRQ enable, 0: envelope IRQ disable
#define C_ENVELOPE_IRQ_ENABLE		0x0040
// bit[15:7] : envelope IRQ fire address[8:0]

// P_SPU_ENV_ADDR_LOW (0xD0401020 + 0x40*X)
// bit[15:0] : envelope address[15:0]

// P_SPU_WAVE_DATA_0 (0xD0401024 + 0x40*X)
// bit[15:0] : previous tone-color data value

// P_SPU_LOOP_CTRL (0xD0401028 + 0x40*X)
// bit[8:0] : envelope repeat address offset[8:0]
// bit[15:9] : envelope ramp down offset[6:0]

// P_SPU_WAVE_DATA (0xD040102C + 0x40*X)
// bit[15:0] : current tone-color data value

// P_SPU_ADPCM_SEL (0xD0401034 + 0x40*X)
// bit[8:0] : reserved
// bit[13:9] : point number of first frame in ADPCM36 mode
// bit[14] : reserved
// bit[15] : 1: ADPCM36 mode, 0: ADPCM mode
#define C_ADPCM36_MODE				0x8000

// P_SPU_PHASE_HIGH (0xD0401800 + 0x40*X)
// bit[2:0] : tone color pitch phase[18:16]
// bit[15:3] : reserved

// P_SPU_PHASE_ACC_HIGH (0xD0401804 + 0x40*X)
// bit[2:0] : phase accumulator[18:16]
// bit[15:3] : reserved

// P_SPU_TARGET_PHASE_HIGH (0xD0401808 + 0x40*X)
// bit[2:0] : pitch bend target phase[18:16]
// bit[15:3] : reserved

// P_SPU_RAMP_DOWN_CLK (0xD040180C + 0x40*X)
// bit[2:0] : envelope ramp down clock selection
#define C_RAMP_DOWN_CLK_0738us		0x0000
#define C_RAMP_DOWN_CLK_2955us		0x0001
#define C_RAMP_DOWN_CLK_11821us		0x0002
#define C_RAMP_DOWN_CLK_47284us		0x0003
#define C_RAMP_DOWN_CLK_189137us	0x0004
#define C_RAMP_DOWN_CLK_756548us	0x0005
#define C_RAMP_DOWN_CLK_1513ms		0x0006
#define C_RAMP_DOWN_CLK_1513ms_2	0x0007
// bit[15:3] : reserved

// P_SPU_PHASE (0xD0401810 + 0x40*X)
// bit[15:0] : tone color pitch phase[15:0]

// P_SPU_PHASE_ACC (0xD0401814 + 0x40*X)
// bit[15:0] : tone color accumulator[15:0]

// P_SPU_TARGET_PHASE (0xD0401818 + 0x40*X)
// bit[15:0] : pitch bend target phase

// P_SPU_PHASE_CTRL (0xD040181C + 0x40*X)
// bit[11:0] : pitch bend phase offset[11:0]
// bit[12] : 1: pahse decrease, 0: phase increase
#define C_PHASE_SIGN				0x1000
// bit[15:13] : pitch bend phase time step
#define C_PHASE_TIME_STEP_0114us	0x0000
#define C_PHASE_TIME_STEP_0227us	0x2000
#define C_PHASE_TIME_STEP_0455us	0x4000
#define C_PHASE_TIME_STEP_0909us	0x6000
#define C_PHASE_TIME_STEP_1819us	0x8000
#define C_PHASE_TIME_STEP_3637us	0xA000
#define C_PHASE_TIME_STEP_7274us	0xC000
#define C_PHASE_TIME_STEP_14549us	0xE000

extern void SPU_Init(void);
extern INT32U SPU_Get_DrvVersion(void);
extern void   SPU_Clear_SRAM(void);
extern void   SPU_Clear_Register(void);
//enable channel
extern void SPU_Enable_Channel(INT8U ChannelIndex);
extern void SPU_Disable_Channel(INT8U ChannelIndex);
extern void SPU_Enable_MultiChannel(INT32U ChannelBit);
extern void SPU_Disable_MultiChannel(INT32U ChannelBit);
extern INT32U SPU_GetChannelEnableStatus(void);
//main volume
extern void  SPU_Set_MainVolume(INT8U VolumeData);
extern INT8U SPU_Get_MainVolume(void);
//channel fiq
extern void SPU_Enable_FIQ_Channel(INT8U ChannelIndex);
extern void SPU_Disable_FIQ_Channel(INT8U ChannelIndex);
extern void SPU_Enable_FIQ_MultiChannel(INT32U ChannelBit);
extern void SPU_Disable_FIQ_MultiChannel(INT32U ChannelBit);
//fiq status
extern void   SPU_Clear_FIQ_Status(INT32U ChannelBit);
extern INT32U SPU_Get_FIQ_Status(void);
//beat count
extern void   SPU_Set_BeatBaseCounter(INT16U BeatBaseCounter);
extern INT16U SPU_Get_BeatBaseCounter(void);
extern void   SPU_Set_BeatCounter(INT16U BeatCounter);
extern INT16U SPU_Get_BeatCounter(void);
INT8U SPU_Get_BeatIRQ_Enable_Flag(void);
//beat irq
extern void SPU_Enable_BeatIRQ(void);
extern void SPU_Disable_BeatIRQ(void);
extern void SPU_Clear_BeatIRQ_Flag(void);
extern INT8U SPU_Get_BeatIRQ_Flag(void);
//envelope ramp down
extern void SPU_Set_EnvelopeClock(INT8U EnvClock, INT8U ChannelIndex);
extern void SPU_Set_EnvRampDown(INT8U RampDownChannel);
extern void SPU_Set_EnvRampDownMultiChannel(INT32U ChannelBit);
extern INT32U SPU_Get_EnvRampDown(void);
//channel stop flag
extern void SPU_Clear_Ch_StopFlag(INT8U ChannelIndex);
extern void SPU_Clear_MultiCh_StopFlag(INT32U ChannelBit);
extern INT32U SPU_Get_Ch_StopStatus(void);
//compressor control
extern void SPU_EnableChannelZC(INT8U ChannelIndex);
extern void SPU_DisableChannelZC(INT8U ChannelIndex);
extern void SPU_AccumulatorInit(void);
extern INT8U SPU_Read_FOF_Status(void);
extern void SPU_Clear_FOF(void);
//channel volume
extern void SPU_SingleChVolumeSelect(INT8U VolumeSelect);
extern INT8U SPU_GetSingleChVolumeSetting(void);
//control flag
extern void SPU_InterpolationON(void);
extern void SPU_InterpolationOFF(void);
extern void SPU_HQ_InterpolationON(void);
extern void SPU_HQ_InterpolationOFF(void);
extern void SPU_CompressorON(void);
extern void SPU_CompressorOFF(void);
extern void SPU_ClearSaturateFlag(void);
extern INT8U SPU_ReadSaturateFlag(void);
extern void SPU_SetCompressorRatio(INT8U ComRatio);
extern INT8U SPU_GetCompressorRatio(void);
extern void SPU_EnableCompZeroCrossing(void);
extern void SPU_DisableCompZeroCrossing(void);
extern void SPU_SetReleaseTimeScale(INT8U ReleaseTimeScale);
extern INT8U SPU_ReadReleaseTimeScale(void);
extern void SPU_SetAttackTimeScale(INT8U AttackTimeScale);
extern INT8U SPU_ReadAttackTimeScale(void);
extern void SPU_SetCompressThreshold(INT8U CompThreshold);
extern INT8U SPU_ReadCompressThreshold(void);
extern void SPU_SelectRMS_Mode(void);
extern void SPU_SelectPeakMode(void);
//channel status
extern INT32U SPU_GetChannelStatus(void);
//software channel
extern void SPU_SendToSoftChannel_Left(INT16U PCM_Data);
extern void SPU_SendToSoftChannel_Right(INT16U PCM_Data);
extern INT16U SPU_GetSPU_PlusSoftOutLeft(void);
extern INT16U SPU_GetSPU_PlusSoftOutRight(void);
//wave out
extern INT16U SPU_GetSPU_OutLeft(void);
extern INT16U SPU_GetSPU_OutRight(void);
//channel repeat
extern void SPU_EnableChannelRepeat(INT8U ChannelIndex);
extern void SPU_DisableChannelRepeat(INT8U ChannelIndex);
extern void SPU_EnableMultiChannelRepeat(INT32U ChannelBit);
extern void SPU_DisableMultiChannelRepeat(INT32U ChannelBit);
//envelop auto mode
extern void SPU_EnvelopeAutoMode(INT8U ChannelIndex);
extern void SPU_EnvelopeManualMode(INT8U ChannelIndex);
//channel release
extern void SPU_SetChannelRelease(INT8U ChannelIndex);
//envelop IRQ
extern INT32U SPU_GetEnvelopeIRQ_Status(void);
extern void SPU_ClearEnvelopeIRQ_Status(INT32U ChannelBit);
//pitchbend control
extern void SPU_EnablePitchBend(INT8U ChannelIndex);
extern void SPU_DisablePitchBend(INT8U ChannelIndex);
//release attack time
extern void SPU_SetReleaseTime(INT8U ReleaseTime);
extern INT8U SPU_ReadReleaseTime(void);
extern void SPU_SetAttackTime(INT8U AttackTime);
extern INT8U SPU_ReadAttackTime(void);
//bank
extern void SPU_SetBenkAddr(INT16U BenkAddr);
//post process
extern void SPU_ClearPWFOV_Flag(void);
extern void SPU_EnablePW_RightChannel(void);
extern void SPU_DisablePW_RightChannel(void);
extern INT8U SPU_GetPWFOV_Flag(void);
extern void SPU_EnablePostWaveOutputSilence(void);
extern void SPU_DisablePostWaveOutputSilence(void);
extern void SPU_SetPostWave_Signed(void);
extern void SPU_SetPostWave_Unsigned(void);
extern void SPU_ClearPostWaveIRQ_Status(void);
extern INT8U SPU_GetPostWaveIRQ_Status(void);
extern void SPU_EnablePostWaveIRQ(void);
extern void SPU_DisablePostWaveIRQ(void);
extern void SPU_SetPostWaveClock_288K(void);
extern void SPU_SetPostWaveClock_281K(void);
extern void SPU_EnablePostWaveLPF(void);
extern void SPU_DisablePostWaveLPF(void);
extern void SPU_EnablePostWaveDownSample(void);
extern void SPU_DisablePostWaveDownSample(void);
extern void SPU_EnablePostWaveToDMA(void);
extern void SPU_DisablePostWaveToDMA(void);
//wave start/loop address
extern void SPU_SetStartAddress(INT32U StartAddr, INT8U ChannelIndex);
extern void SPU_SetLoopAddress(INT32U LoopAddr, INT8U ChannelIndex);
//tone color mode
extern void SPU_SetToneColorMode(INT8U ToneColorMode, INT8U ChannelIndex);
extern void SPU_Set_16bit_Mode(INT8U ChannelIndex);
extern void SPU_Set_8bit_Mode(INT8U ChannelIndex);
extern void SPU_SetADPCM_Mode(INT8U ChannelIndex);
extern void SPU_SetPCM_Mode(INT8U ChannelIndex);
//volicety/pan
extern void SPU_SetVelocity(INT8U VelocityValue, INT8U ChannelIndex);
extern void SPU_SetPan(INT8U PanValue, INT8U ChannelIndex);
// envelop set
extern void SPU_SetEnvelope_0(INT16U Envelope_0, INT8U ChannelIndex);
extern void SPU_SetEnvelopeIncrement(INT8U EnvInc, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeIncrement(INT8U ChannelIndex);
extern void SPU_SetEnvelopePostive(INT8U ChannelIndex);
extern void SPU_SetEnvelopeNegative(INT8U ChannelIndex);
extern void SPU_SetEnvelopeTarget(INT8U EnvTarget, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeTarget(INT8U ChannelIndex);
extern void SPU_SetEnvelopeData(INT8U EnvData, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeData(INT8U ChannelIndex);
extern void SPU_SetEnvelopeCounter(INT8U EnvCounter, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeCounter(INT8U ChannelIndex);
extern void SPU_SetEnvelope_1(INT16U Envelope_1, INT8U ChannelIndex);
extern void SPU_SetEnvelopeReloadData(INT8U EnvReloadData, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeReloadData(INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatMode(INT8U ChannelIndex);
extern void SPU_SetEnvelopeNormalMode(INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatCounter(INT8U RepeatCounter, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeRepeatCounter(INT8U ChannelIndex);
extern void SPU_SetEnvelopeAddress(INT32U EnvelopeAddr, INT8U ChannelIndex);
//envelop irq
extern void SPU_SetEnvelopeIRQ_Enable(INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_Disable(INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_FireAddress(INT16U FireAddr, INT8U ChannelIndex);
extern void SPU_SetWaveData_0(INT16U WDD_0, INT8U ChannelIndex);
extern void SPU_SetWaveData(INT16U WDD, INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatAddrOffset(INT16U EAOffset, INT8U ChannelIndex);
extern void SPU_SetEnvelopeRampDownOffset(INT8U RampDownOffset, INT8U ChannelIndex);
//ADPCM36 mode
extern void SPU_SelectADPCM_Mode(INT8U ChannelIndex);
extern void SPU_SelectADPCM36_Mode(INT8U ChannelIndex);
extern void SPU_SetADPCM_PointNumber(INT8U PointNumber, INT8U ChannelIndex);
extern void SPU_ClearADCPM36_Mode(INT8U ChannelIndex);
//set phase
extern void SPU_SetPhase(INT32U Phase, INT8U ChannelIndex);
extern INT32U SPU_ReadPhase(INT8U ChannelIndex);
extern void SPU_SetPhaseAccumulator(INT32U PhaseAcc, INT8U ChannelIndex);
extern INT32U SPU_ReadPhaseAccumulator(INT8U ChannelIndex);
extern void SPU_SetTargetPhase(INT32U TargetPhase, INT8U ChannelIndex);
extern INT32U SPU_ReadTargetPhase(INT8U ChannelIndex);
extern void SPU_SetRampDownClock(INT8U RampDownClock, INT8U ChannelIndex);
extern void SPU_SetPhaseOffset(INT16U PhaseOffset, INT8U ChannelIndex);
extern void SPU_SetPhaseIncrease(INT8U ChannelIndex);
extern void SPU_SetPhaseDecrease(INT8U ChannelIndex);
extern void SPU_SetPhaseTimeStep(INT8U PhaseTimeStep, INT8U ChannelIndex);
extern void SPU_SetPhaseControl(INT16U PhaseControl, INT8U ChannelIndex);
extern INT16U SPU_ReadPhaseControl(INT8U ChannelIndex);
//fiq set
extern void SPU_FIQ_Register(INT8U FIQ_Index, void (*F_ISR)(void));
extern void SPU_FIQ_Enable(INT8U FIQ_Index);
extern void SPU_FIQ_Disable(INT8U FIQ_Index);
extern void SPU_Test_IRQ_Register(INT8U IRQ_Index, void (*F_ISR)(void));
extern void SPU_Test_IRQ_Enable(INT8U IRQ_Index);
extern void SPU_Test_IRQ_Disable(INT8U IRQ_Index);
extern void SPU_Test_Timer_IRQ_Setup(INT32U timer_id, INT32U freq_hz, void (*TimerIsrFunction)(void));
//-------------------------------------------------------------------

typedef enum
{
	GPY0200_USER_ISR_ONKEY1,
	GPY0200_USER_ISR_ADAPTER,
	GPY0200_USER_ISR_BAT34,
	GPY0200_USER_ISR_ALARM,
	GPY0200_USER_ISR_HSEC,
	GPY0200_USER_ISR_SEC,
	GPY0200_USER_ISR_MIN,
	GPY0200_USER_ISR_HOUR,
	GPY0200_USER_ISR_DAY,
	GPY0200_USER_ISR_SCH,
	GPY0200_USER_ISR_MAX
} GPY0200_USER_ISR_INDEX;

typedef enum
{
	GPY0200_BAT_LEVEL_3_2 = 0,
	GPY0200_BAT_LEVEL_3_4,
	GPY0200_BAT_LEVEL_3_6,
	GPY0200_BAT_LEVEL_4_0,
	GPY0200_BAT_LEVEL_FULL
} GPY0200_BAT_LEVEL;

typedef enum
{
	GPY0200_ADAPTER_IN = 0,
	GPY0200_ADAPTER_OUT
} GPY0200_ADAPTER_PLUG_STATUS;

typedef enum
{
	ADAPTER_IN = GPY0200_ADAPTER_IN,
	ADAPTER_OUT
} ADAPTER_PLUG_STATUS;

typedef enum
{
	MA_50 = 0,
	MA_100,
	MA_200,
	MA_400
} GPY0200_CHARGER_CURRENT;

#define 	TRIGGER_ALARM				0x00aa
#define 	TRIGGER_DAY					0x5a5a
#define GPY0200_RTC_HMSEN     (1 << 4)  /* H/M/S function enable */
#define GPY0200_RTC_SCHEN     (1 << 5)  /* scheduler function enbale */
#define GPY0200_RTC_ALMEN     (1 << 6)  /* alarm function enable */

#define GPY0200_RTC_ALM_IEN       (1 << 6)  /* alarm interrupt enbale */
#define GPY0200_RTC_SCH_IEN       (1 << 5)  /* scheduler interrupt enbale */
#define GPY0200_RTC_DAY_IEN       (1 << 4)  /* hour interrupt enbale */
#define GPY0200_RTC_HR_IEN        (1 << 3)  /* hour interrupt enbale */
#define GPY0200_RTC_MIN_IEN       (1 << 2)  /* min interrupt enbale */
#define GPY0200_RTC_SEC_IEN       (1 << 1)  /* alarm interrupt enbale */
#define GPY0200_RTC_HALF_SEC_IEN  (1 << 0)  /* alarm interrupt enbale */

#define GPY0200_INT_ON_KEY1   1
#define GPY0200_INT_ON_KEY2   2
#define GPY0200_INT_ADP       4
#define GPY0200_INT_ALM       8
#define GPY0200_INT_BAT34     0x10
#define GPY0200_INT_HMS       0x20
#define GPY0200_INT_SCH       0x40
#define GPY0200_INT_IO        0x80

#define GPY0200_EN_SEC    (1 << 0)
#define GPY0200_EN_MIN    (1 << 1)
#define GPY0200_EN_HR     (1 << 2)
#define GPY0200_EN_DAY    (1 << 3)

extern void gpy0200_i2c_init (INT32U nSCL, INT32U nSDA);
extern void gpy0200_i2c_write (INT16U addr, INT16U data);
extern INT16U gpy0200_i2c_read (INT16U addr);

extern void gpy0200_init(void);
extern void gpy0200_rtc_time_set(t_rtc *rtc_time,INT8U mask);
extern void gpy0200_rtc_hms_get(t_rtc *rtc_time);
extern void gpy0200_rtc_day_get(t_rtc *rtc_time);
extern void gpy0200_rtc_alarm_set(t_rtc *rtc_time, INT8U mask);
extern void gpy0200_rtc_alarm_get(t_rtc *rtc_time);
extern INT32S gpy0200_callback_set(INT8U int_idx, void (*user_isr)(void));
extern void gpy0200_rtc_function_set(INT8U mask, INT8U value);
extern void gpy0200_rtc_int_set(INT8U mask, INT8U value);
extern void gpy0200_irq_mask_set(INT8U mask, INT8U value);
extern void gpy0200_ldo33a_turn_off(void);
extern void gpy0200_ldo33b_set(BOOLEAN status);
extern void gpy0200_audio_en_set(BOOLEAN status);
extern INT8U gpy0200_battery_level_get(void);
extern INT8U gpy0200_adapter_plug_status_get(void);
extern void gpy0200_ldo50_en_set(BOOLEAN status);
extern void gpy0200_rtc_alarm_condition_set(INT8U mask,INT8U value);
extern void gpy0200_charger_current_set(INT8U value);
extern INT8U gpy0200_charger_current_get(void);
extern void gpy0200_tft_en_set(BOOLEAN status);
extern void gpy0200_vdac_en_set(BOOLEAN status);
extern void gpy0200_soft_reset(void);
extern void gpy0200_aging_pattern_en_set(BOOLEAN status,INT8U index);
extern void gpy0200_sram_data_wirte(INT8U offset,INT8U *data,INT8U len);
extern void gpy0200_sram_data_read(INT8U offset,INT8U *data,INT8U len);

// drv_l1_tools.c
extern INT32S mem_transfer_dma(INT32U src, INT32U dest, INT32U len);

// drv_l1_efuse.c
extern INT16U efuse_customer_id_read(void);

#endif 		// __DRIVER_L1_H__

