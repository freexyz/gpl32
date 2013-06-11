/*****************************************************************************
 *               Copyright Generalplus Corp. All Rights Reserved.      
 *                                                                     
 * FileName:       gplib_calendar.c
 * Author:         Lichuanyue  
 * Description:    Created
 * Version:        1.0 
 * Function List:  
 *                 Null
 * History:        
 *                 1>. 2008/07/02 Created
 *****************************************************************************/

#include "gplib_calendar.h"

INT16U GraphicCalendarConfirmWeek(INT16U year, INT8U month, INT8U day);
INT8U GraphicCalendarCalculateDays(INT8U month, INT16U year);
/*****************************************************************************
 Function Name:                                    
 Purposes     : 
 Arguments    : 
 Return Value : 
          		
 <History>                                              
 Date                         Author                         Modification      
2008/07/09                   Lichuanyue                     Created  
*******************************************************************************/
INT16U GraphicCalendarConfirmWeek(INT16U y, INT8U m, INT8U d)
{
    INT8U week;
	INT32S c, ya;
    INT32U j;

	if (m > 2) 
        m = m - 3;
    else 
	{
        m = m + 9;
        y = y - 1;
    }
    c = y / 100;
    ya = y - 100 * c;
    j = (146097L * c) / 4 + (1461L * ya) / 4 + (153L * m + 2) / 5 + d + 1721119L;

	week = ((j + 1) % 7);
    return week;
}
/*****************************************************************************
 Function Name:                                    
 Purposes     : 
 Arguments    : 
 Return Value : 
          		
 <History>                                              
 Date                         Author                         Modification      
2008/07/09                    Lichuanyue                     Created  
*******************************************************************************/
INT8U GraphicCalendarCalculateDays(INT8U month, INT16U year)
{
	const INT8U DaysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
	 
	if((0!=(year%4))||((0==(year%100))&&(0!=(year%400))))
		return DaysInMonth[month - 1];
    else
	{
		if(2 == month)
			return 29;
		else
			return DaysInMonth[month - 1];
	}
}



