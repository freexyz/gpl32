#include "driver_l1.h"
#include "drv_l1_sfr.h"

#if _DRV_L1_GPY0200 == 1

#define	 gpy0200_i2c_SCL      0
#define	 gpy0200_i2c_SDA      1

#define ADDR_WR_CODE   0xf3
#define ADDR_RD_CODE   0xb3
#define DATA_WR_CODE   0xd5
#define DATA_RD_CODE   0x95

// Global Variables for gpy0200_i2c Library  
INT32U gpy0200SCL;
INT32U gpy0200SDA;

void gpy0200_i2c_delay (INT16U i);
void gpy0200_i2c_start (void);
void gpy0200_i2c_stop (void);
void gpy0200_i2c_w_phase (INT16U value);
INT16U gpy0200_i2c_r_phase (void);
void gpy0200_i2c_init (INT32U nSCL, INT32U nSDA);
void gpy0200_i2c_write (INT16U addr, INT16U data);
INT16U gpy0200_i2c_read (INT16U addr);						

//====================================================================================================
// gpy0200_i2c Control C Code
// using SDA and SDL to control sensor interface
//====================================================================================================

//====================================================================================================
//	Description:	Delay function
//	Syntax:			gpy0200_i2c_delay (
//						INT16U i
//					);
//	Return: 		None
//====================================================================================================
void gpy0200_i2c_delay (
	INT16U i
) {
	INT16U j;

	for (j=0;j<(i<<2);j++)
		i=i;
}

//===================================================================
//	Description:	Start of data transmission
//	Function:		gpy0200_i2c_start
//	Syntax:			void gpy0200_i2c_start (void)
//	Input Paramter:
//					none
//	Return: 		none
//	Note:			Please refer to gpy0200_i2c spec
//===================================================================
void gpy0200_i2c_start (void)
{
	gpio_write_io(gpy0200SCL, DATA_HIGH);					//SCL1
	gpy0200_i2c_delay (1);
	gpio_write_io(gpy0200SDA, DATA_HIGH);					//SDA1
	gpy0200_i2c_delay (1);
	gpio_write_io(gpy0200SDA, DATA_LOW);					//SDA0	
	gpy0200_i2c_delay (1);
}

//===================================================================
//	Description:	Stop of data transmission
//	Function:		gpy0200_i2c_stop
//	Syntax:			void gpy0200_i2c_stop (void)
//	Input Paramter:
//					none
//	Return: 		none
//	Note:			Please refer to gpy0200_i2c spec
//===================================================================
void gpy0200_i2c_stop (void)
{
	gpy0200_i2c_delay (1);
	gpio_write_io(gpy0200SDA, DATA_LOW);					//SDA0	
	gpy0200_i2c_delay (1);
	gpio_write_io(gpy0200SCL, DATA_HIGH);					//SCL1
	gpy0200_i2c_delay (1);
	gpio_write_io(gpy0200SDA, DATA_HIGH);					//SDA1
	gpy0200_i2c_delay (1);
}

//===================================================================
//	Description:	Phase write process
//	Function:		gpy0200_i2c_w_phase
//	Syntax:			void gpy0200_i2c_w_phase (INT16U value)
//	Input Paramter:
//					INT16U value:	output data
//	Return: 		none
//	Note:			Please refer to gpy0200_i2c spec
//===================================================================
void gpy0200_i2c_w_phase (INT16U value)
{
	INT16U i;

	for (i=0;i<8;i++)
	{
		gpio_write_io(gpy0200SCL, DATA_LOW);					//SCL0
		gpy0200_i2c_delay (1);
		if (value & 0x80)
			gpio_write_io(gpy0200SDA, DATA_HIGH);				//SDA1
		else
			gpio_write_io(gpy0200SDA, DATA_LOW);				//SDA0
		gpy0200_i2c_delay (1);
		gpio_write_io(gpy0200SCL, DATA_HIGH);					//SCL1
		gpy0200_i2c_delay(2);
		value <<= 1;
	}
	// The 9th bit transmission
	gpio_write_io(gpy0200SCL, DATA_LOW);						//SCL0
	gpio_init_io(gpy0200SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	gpy0200_i2c_delay(2);
	gpio_write_io(gpy0200SCL, DATA_HIGH);						//SCL1
	gpy0200_i2c_delay(2);
	gpio_write_io(gpy0200SCL, DATA_LOW);						//SCL0
	gpio_init_io(gpy0200SDA, GPIO_OUTPUT);						//SDA is Hi-Z mode
}

//===================================================================
//	Description:	Phase read process
//	Function:		gpy0200_i2c_r_phase
//	Syntax:			INT16U gpy0200_i2c_r_phase (void)
//	Input Paramter: none
//	Return: 		INT16U			: input data
//	Note:			Please refer to gpy0200_i2c spec
//===================================================================
INT16U gpy0200_i2c_r_phase (void)
{
	INT16U i;
	INT16U data;

	gpio_init_io(gpy0200SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	data = 0x00;
	for (i=0;i<8;i++)
	{
		gpio_write_io(gpy0200SCL, DATA_LOW);					//SCL0
		gpy0200_i2c_delay(2);
		gpio_write_io(gpy0200SCL, DATA_HIGH);					//SCL1
		data <<= 1;
		data |=( gpio_read_io(gpy0200SDA));
		gpy0200_i2c_delay(2);
	}
	// The 9th bit transmission
	gpio_write_io(gpy0200SCL, DATA_LOW);						//SCL0
	gpio_init_io(gpy0200SDA, GPIO_OUTPUT);						//SDA is output mode
	gpio_write_io(gpy0200SDA, DATA_LOW);						//SDA0
	gpy0200_i2c_delay(2);
	gpio_write_io(gpy0200SCL, DATA_HIGH);						//SCL1
	gpy0200_i2c_delay(2);
	gpio_write_io(gpy0200SCL, DATA_LOW);						//SCL0
	return data;		
}

//====================================================================================================
//	Description:	gpy0200_i2c Initialization
//	Syntax:			void gpy0200_i2c_init (
//						INT16S nSCL,				// Clock Port No
//													Ex. PORTA.9 = 0x09	/ PORTB.3 = 0x13
//						INT16S nSDA				// Data Port No
//													Ex. PORTA.10 = 0x0A	/ PORTB.4 = 0x14
//					);
//	Return: 		None
//====================================================================================================
void gpy0200_i2c_init (
	INT32U nSCL,				// Clock Port No
	INT32U nSDA				// Data Port No
) {
	gpy0200SCL = nSCL;
	gpy0200SDA = nSDA;
	
	gpio_set_port_attribute(gpy0200SCL, ATTRIBUTE_HIGH);
	gpio_set_port_attribute(gpy0200SDA, ATTRIBUTE_HIGH);
	gpio_init_io(gpy0200SCL, GPIO_OUTPUT);				//set dir	 	
	gpio_init_io(gpy0200SDA, GPIO_OUTPUT);				//set dir
}

//====================================================================================================
//	Description:	gpy0200_i2c write register process
//	Syntax:			void gpy0200_i2c_write (
//						INT16U addr,				// Register Address
//						INT16U data				// Register Data
//					);
//	Return: 		None
//	Note:			Please refer to gpy0200_i2c spec
//====================================================================================================
void gpy0200_i2c_write (
	INT16U addr,				// Register Address
	INT16U data				  	// Register Data
) {
	// Data re-verification
	//id &= 0xFF;
	addr &= 0xFF;
	data &= 0xFF;

#if 0
	// Serial bus output mode initialization
	gpio_set_port_attribute(gpy0200SCL, ATTRIBUTE_HIGH);
	gpio_set_port_attribute(gpy0200SDA, ATTRIBUTE_HIGH);
	gpio_init_io(gpy0200SCL, GPIO_OUTPUT);				//set dir	 	
	gpio_init_io(gpy0200SDA, GPIO_OUTPUT);				//set dir	 
#endif

	// 3-Phase write transmission cycle is starting now ...
	gpio_write_io(gpy0200SCL, DATA_HIGH);				//SCL1		
	gpio_write_io(gpy0200SDA, DATA_HIGH);				//SDA0
	gpy0200_i2c_start ();									// Transmission start
	gpy0200_i2c_w_phase (ADDR_WR_CODE);								// Phase 1
	gpy0200_i2c_w_phase (addr);							// Phase 2
	gpy0200_i2c_w_phase (DATA_WR_CODE);	
	gpy0200_i2c_w_phase (data);							// Phase 3
	gpy0200_i2c_stop ();									// Transmission stop
}

//====================================================================================================
//	Description:	gpy0200_i2c read register process
//	Syntax:			INT16U gpy0200_i2c_read (
//						INT16U addr				// Register Address
//					);
//	Input Paramter:
//					INT16U id		: slave device id
//					INT16U addr	: register address
//	Return:
//					INT16U 		: register data
//	Note:			Please refer to gpy0200_i2c spec
//====================================================================================================
INT16U gpy0200_i2c_read (
		INT16U addr					// Register Address
) {
	INT16U data;
	
	// Data re-verification
	//id &= 0xFF;
	addr &= 0xFF;

#if 0
	// Serial bus output mode initialization
	gpio_set_port_attribute(gpy0200SCL, ATTRIBUTE_HIGH);
	gpio_set_port_attribute(gpy0200SDA, ATTRIBUTE_HIGH);
	gpio_init_io(gpy0200SCL, GPIO_OUTPUT);				//set dir	 	
	gpio_init_io(gpy0200SDA, GPIO_OUTPUT);				//set dir
#endif

	// 2-Phase write transmission cycle is starting now ...
	gpio_write_io(gpy0200SCL, DATA_HIGH);				//SCL1		
	gpio_write_io(gpy0200SDA, DATA_HIGH);				//SDA0
	
	gpy0200_i2c_start ();									// Transmission start
	gpy0200_i2c_w_phase (ADDR_RD_CODE);								// Phase 1
	gpy0200_i2c_w_phase (addr);							// Phase 2
	gpy0200_i2c_w_phase (DATA_RD_CODE);
	data = gpy0200_i2c_r_phase();							// Phase 2
	gpy0200_i2c_stop ();									// Transmission stop
	return data;
}

#endif

