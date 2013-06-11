#include "drv_l2_key_scan.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L2_KEYSCAN) && (_DRV_L2_KEYSCAN == 1)           //
//================================================================//

void key_Scan(void);

INT32U IO1_Data, IO2_Data, old_IO1_Data=0, old_IO2_Data=0, IO1, IO2, IO1_C, IO2_C;
INT32U IO3_Data, IO4_Data, old_IO3_Data=0, old_IO4_Data=0, IO3, IO4, IO3_C, IO4_C;
INT32U IO5_Data, IO6_Data, old_IO5_Data=0, old_IO6_Data=0, IO5, IO6, IO5_C, IO6_C;
INT32U IO7_Data, IO8_Data, old_IO7_Data=0, old_IO8_Data=0, IO7, IO8, IO7_C, IO8_C;
INT32U IO9_Data, old_IO9_Data=0, IO9, IO9_C;

//=============================================================================================
void key_Scan(void)
{
	IO1_Data = 0;
	IO2_Data = 0;
	IO3_Data = 0;
	IO4_Data = 0;
	IO5_Data = 0;
	IO6_Data = 0;
	IO7_Data = 0;
	IO8_Data = 0;
	
	IO1_C = 0;
	IO2_C = 0;
	IO3_C = 0;
	IO4_C = 0;
	IO5_C = 0;
	IO6_C = 0;		
	IO7_C = 0;
	IO8_C = 0;
	
	IO1_Data = gpio_read_io(33);
	IO2_Data = gpio_read_io(34);
	IO3_Data = gpio_read_io(35);
	IO4_Data = gpio_read_io(89);
	IO5_Data = gpio_read_io(85);
	IO6_Data = gpio_read_io(86);
	IO7_Data = gpio_read_io(87);
	IO8_Data = gpio_read_io(88);
	IO9_Data = gpio_read_io(42);
	
	IO1 = 0;
	IO2 = 0;
	IO3 = 0;
	IO4 = 0;
	IO5 = 0;
	IO6 = 0;
	IO7 = 0;
	IO8 = 0;
	
	if(IO1_Data & 0x1 != 0)	
	{
		IO1_C=1;
		if( (old_IO1_Data & 0x1) != 0)
			IO1 = 0;
		else
			IO1 = 1 ;	
	}
	if(IO2_Data & 0x1 != 0)	
	{
		IO2_C=1;
		if( (old_IO2_Data & 0x1) != 0)
			IO2 = 0;
		else
			IO2 = 1 ;	
	}
	
	if(IO3_Data & 0x1 != 0)	
	{
		IO3_C=1;
		if( (old_IO3_Data & 0x1) != 0)
			IO3 = 0;
		else
			IO3 = 1 ;	
	}
	
	if(IO4_Data & 0x1 != 0)	
	{
		IO4_C=1;
		if( (old_IO4_Data & 0x1) != 0)
			IO4 = 0;
		else
			IO4 = 1 ;	
	}
	
	if(IO5_Data & 0x1 != 0)	
	{
		IO5_C=1;
		if( (old_IO5_Data & 0x1) != 0)
			IO5 = 0;
		else
			IO5 = 1 ;	
	}
	
	if(IO6_Data & 0x1 != 0)	
	{
		IO6_C=1;
		if( (old_IO6_Data & 0x1) != 0)
			IO6 = 0;
		else
			IO6 = 1 ;	
	}
	
	if(IO7_Data & 0x1 != 0)	
	{
		IO7_C=1;
		if( (old_IO7_Data & 0x1) != 0)
			IO7 = 0;
		else
			IO7 = 1 ;	
	}
	
	if(IO8_Data & 0x1 != 0)	
	{
		IO8_C=1;
		if( (old_IO8_Data & 0x1) != 0)
			IO8 = 0;
		else
			IO8 = 1 ;	
	}
	
	if(IO9_Data & 0x1 != 0)	
	{
		IO9_C=1;
		if( (old_IO9_Data & 0x1) != 0)
			IO9 = 0;
		else
			IO9 = 1 ;	
	}
	
	old_IO1_Data=IO1_Data;
	old_IO2_Data=IO2_Data;
	old_IO3_Data=IO3_Data;
	old_IO4_Data=IO4_Data;
	old_IO5_Data=IO5_Data;
	old_IO6_Data=IO6_Data;
	old_IO7_Data=IO7_Data;
	old_IO8_Data=IO8_Data;
	old_IO9_Data=IO9_Data;
}	

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L2_KEYSCAN) && (_DRV_L2_KEYSCAN == 1)      //
//================================================================//