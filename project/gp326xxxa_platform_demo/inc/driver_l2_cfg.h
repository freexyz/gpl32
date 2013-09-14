#ifndef __DRIVER_L2_CFG_H__
#define __DRIVER_L2_CFG_H__

//=========================Engineer Mode ("Not" defined CUSTOMER_DEFINE)=====================//
#ifndef CUSTOMER_DEFINE  //For Engineer Development and test                                 //
//===========================================================================================//

    #define DRV_L2_ON			    	1
    #define DRV_L2_OFF			  		0
    #define _DRV_L2_SYS_TIMER_EN		DRV_L2_ON
    #define _DRV_L2_KEYSCAN	            DRV_L2_ON
    #define _DRV_L2_EXT_RTC             DRV_L2_OFF
    #define _DRV_L2_ITOUCH	            DRV_L2_OFF
    #define _DRV_L2_USBH                _DRV_L1_USBH
    #define _DRV_L2_USBH_UVC            _DRV_L1_USBH_UVC
    #define _DRV_L2_SPI                 DRV_L2_ON
    
    #if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
    	#define _DRV_L2_IR   	            DRV_L2_OFF
   	    #define _DRV_L2_NAND	            DRV_L2_ON
    #elif (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX) && (MCU_VERSION < GPL327XX)
    	#define _DRV_L2_IR   	            DRV_L2_OFF
    	#define _DRV_L2_NAND	            DRV_L2_ON	
    #elif (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
    	#define _DRV_L2_IR   	            DRV_L2_OFF
    	#define _DRV_L2_NAND	            DRV_L2_OFF
    #else
    	#define _DRV_L2_IR   	            DRV_L2_OFF
    	#define _DRV_L2_NAND	            DRV_L2_OFF
    #endif
    
    #if _DRV_L2_ITOUCH==DRV_L2_ON
    #define	ITOUCH_SCL       IO_E0
    #define	ITOUCH_SDA       IO_E1
    #endif

	//Added by Richard.Woo
	#if(_DRV_L2_EXT_RTC==DRV_L2_ON)
		#define DRV_L2_EXT_ALAEM_EN	DRV_L2_OFF
		#define EXT_RTC_PT7C4372	1
		#define EXT_RTC_DS1302		2
		#define EXT_RTC_HYM8563     3
		//Chioce One
		#define EXT_RTC_TYPE_CHOICE	EXT_RTC_DS1302
	#endif
//======================== Engineer Mode END ================================================//
#else                              //Engineer Difne area END                                 //
//===========================================================================================//

//=========================Customer Mode (Defined CUSTOMER_DEFINE)===========================//
//                    //For Release to customer, follow the command of "customer.h"          //
//======================== Not Modify in this Area ==========================================// 


//========================= CUSTOMER DEFINE END =============================================//
#endif  // not CUSTOMER_DEFINE                                                               //
//========================= CUSTOMER DEFINE END =============================================//


#endif		// __DRIVER_L2_CFG_H__
