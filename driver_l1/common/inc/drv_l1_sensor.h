#ifndef __drv_l1_SENSOR_H__
#define __drv_l1_SENSOR_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

/* sensor configure setting */
#if BOARD_TYPE == BOARD_EMU_V2_0
	#define	 SCCB_SCL			IO_F5	
	#define	 SCCB_SDA   		IO_C1	
	#define	 SENSOR_ENABLE		IO_G11
	#define	 SENSOR_RESET	 	0xFF
#elif BOARD_TYPE == BOARD_DEMO_GPL32XXX
	#define	 SCCB_SCL   		IO_G11	
	#define	 SCCB_SDA   		IO_G10
	#define	 SENSOR_ENABLE		IO_G13
	#define	 SENSOR_RESET	 	0xFF		
#elif BOARD_TYPE == BOARD_DEMO_GPL326XX
	#define	 SCCB_SCL   		IO_B9	
	#define	 SCCB_SDA   		IO_B8
	#define	 SENSOR_ENABLE		IO_H7
	#define	 SENSOR_RESET	 	0xFF	
#elif (BOARD_TYPE == BOARD_DEMO_GPL327XX) || (BOARD_TYPE == BOARD_DEMO_GP327XXXA)
	#define	 SCCB_SCL   		IO_C2	
	#define	 SCCB_SDA   		IO_C1
	#define	 SENSOR_ENABLE		IO_C3
	#define	 SENSOR_RESET	 	0xFF
#elif (BOARD_TYPE == BOARD_DEMO_GPL326XXB) || (BOARD_TYPE == BOARD_DEMO_GP326XXXA)
	#define	 SCCB_SCL   		IO_C2	
	#define	 SCCB_SDA   		IO_C1
	#define	 SENSOR_ENABLE		IO_C3
	#define	 SENSOR_RESET	 	0xFF
#else
	#define	 SCCB_SCL   		IO_F5	
	#define	 SCCB_SDA   		IO_C1
	#define	 SENSOR_ENABLE		IO_G11
	#define	 SENSOR_RESET	 	0xFF	
#endif

#define CSI_CLOCK_SYS_CLK_DIV2		0
#define CSI_CLOCK_27MHZ				1
#define CSI_CLOCK_SYS_CLK_DIV4		2
#define CSI_CLOCK_13_5MHZ			3
#if (defined __HSB0357_DRV_C__)
	#define CSI_CLOCK				CSI_CLOCK_SYS_CLK_DIV4
#elif (defined __OV3640_MIPI_DRV_C__)
	#define CSI_CLOCK				CSI_CLOCK_13_5MHZ
#else
	#define CSI_CLOCK				CSI_CLOCK_27MHZ
#endif

// For OID sensor
#define OID_SENSOR_CHIP_VERSION_A600	0x012A
#define OID_SENSOR_CHIP_VERSION_A601	0x023E
#define OID_SENSOR_CHIP_VERSION_UNKNOWN	0xFFFF

#define	OV6680_ID				0xC0
#define OV7680_ID				0x42
#define OV7670_ID				0x42
#define OV9655_ID				0x60
#define OV9660_ID				0x60
#define OV2655_ID				0x60
#define OV5642_ID				0x60
#define OV3640_ID				0x78
#define OV7675_ID				0x42
#define OV2643_ID				0x60
#define OID_SENSOR_ID			0x92
#define BF3710_ID				0xDC
#define HSB0357_ID				0x60

extern void sccb_init(void);
extern INT32S sccb_write_Reg8Data8(INT8U id, INT8U addr, INT8U data);
extern INT32S sccb_read_Reg8Data8(INT8U id, INT8U addr, INT8U *data);
extern INT32S sccb_write_Reg8Data16(INT8U id, INT8U addr, INT16U data);
extern INT32S sccb_read_Reg8Data16(INT8U id, INT8U addr, INT16U *data);
extern INT32S sccb_write_Reg16Data8(INT8U id, INT16U addr, INT8U data); 
extern INT32S sccb_read_Reg16Data8(INT8U id, INT16U addr, INT8U *data); 
extern INT32S sccb_write_Reg16Data16(INT8U id, INT16U addr, INT16U data);  
extern INT32S sccb_read_Reg16Data16(INT8U id,	INT16U addr, INT16U *data); 
 
extern void CSI_WaitCaptureComplete (void);
extern void Sensor_Bluescreen_Enable(void);
extern void Sensor_Bluescreen_Disable(void); 
extern void Sensor_Cut_Enable(void);
extern void Sensor_Cut_Disable(void);
extern void Sensor_Black_Enable(void);
extern void Sensor_Black_Disable(void); 
extern void Sensor_EnableFrameEndInterrupt(BOOLEAN en);

extern void OID_Sensor_Write_Register(INT16U addr,	INT16U data);
extern INT16U OID_Sensor_Read_Register(INT16U addr);
extern INT16U OID_Sensor_GetChipVersion(void);
extern void OID_Sensor_SetEnablePin(void);
extern void OID_Sensor_Enable(BOOLEAN en);
extern void OID_Sensor_CSI_Enable(BOOLEAN en);
extern void OID_Sensor_And_CSI_Enable(BOOLEAN en);
extern void OID_Sensor_SetResetPin(void);
extern void OID_Sensor_Reset(BOOLEAN is_back_to_normal_state);
extern void OID_Sensor_SetFrameBuffB(INT32U buff);
extern void OID_Sensor_ChangeFrameBuff(BOOLEAN is_valid);

#endif	/*__drv_l1_SENSOR_H__*/
