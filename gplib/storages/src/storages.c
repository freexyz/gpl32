/*
* Purpose: Storages polling and Flash access
*
* Author: wschung
*
* Date: 2008/11/19
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.06
* History :
*         1 2008/06/06 Created by wschung.
          2 Add UHOST initial detection
          3 IOG[5] as SD/MS/XD card detection pin. If card pin changed ,polling each stroage
          4 IOG[2] as CF card detection pin. If card pin changed ,polling each stroage
          5 uninitial storage after polling.
          6.Add Nand initial
          7.usbh pluginout detection mov to system task
          8.Add Pin detection before storage_init()
          9.Modify storage_sdms_detection() for xdc 
          10.Add Xdc and uhost detection pin (IOH1)
           
* Note    : (USB Host using interrupt)
*/
#include "storages.h"  

  #define C_BKCS1_PIN  81 //IOF1
  #define C_BKCS2_PIN  82 //IOF2
#if (BOARD_TYPE == BOARD_TK_V4_1) || (BOARD_TYPE == BOARD_GPY0200_EMU_V2_0) || (BOARD_TYPE == BOARD_TK35_V1_0)
  #define C_WP_D_PIN       IO_G6
  #define C_STORAGE_CD_PIN IO_G5
  #define C_CFC_RST_PIN    0x61//IOG[1]
  #define C_CFC_CD_PIN     0x62//IOG[2]
  #define C_USBDEVICE_PIN  83 //IOF3
//  #define C_USBHOST_PIN   IO_H1//IOH[1]
  #define C_USBHOST_PIN    	IO_H3
  #define C_USBHOST_PWR_PIN IO_H5
#elif BOARD_TYPE == BOARD_TK_V4_0
  #define C_WP_D_PIN       0x64//IOG[4]
  #define C_STORAGE_CD_PIN 0x63//IOG[3]
  #define C_CFC_RST_PIN    0x61//IOG[1]
  #define C_CFC_CD_PIN     0x62//IOG[2]
  #define C_USBDEVICE_PIN  83 //IOF3
//  #define C_USBHOST_PIN   IO_H1//IOH[1]
#elif (BOARD_TYPE == BOARD_TK_V4_3) || (BOARD_TYPE == BOARD_TK_A_V1_0)
  #define C_WP_D_PIN       	IO_G6
  #define C_STORAGE_CD_PIN 	IO_G5
  #define C_CFC_RST_PIN    	IO_G4
  #define C_CFC_CD_PIN     	IO_G2
  #define C_USBDEVICE_PIN  	IO_H4
  #define C_USBHOST_PIN    	IO_H1
  #define C_USBHOST_PWR_PIN IO_H5
#elif (BOARD_TYPE == BOARD_TK_V4_4) || (BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY) || (BOARD_TYPE == BOARD_TK_8D_V1_0) || (BOARD_TYPE == BOARD_TK_8D_V2_0)
  #define C_WP_D_PIN       	IO_G6
  #define C_STORAGE_CD_PIN 	IO_G5
  #define C_CFC_RST_PIN    	IO_G4
  #define C_CFC_CD_PIN     	IO_G2
  #define C_USBDEVICE_PIN  	IO_H4
  #if (USBH_DETECTION_MODE == USBH_GPIO_D_2 ) 
    #define C_USBHOST_PIN     IO_H5
    #define C_USBHOST_PWR_PIN IO_H3
  #else    
    #define C_USBHOST_PIN     IO_H3
    #define C_USBHOST_PWR_PIN IO_H5
  #endif     
#elif (BOARD_TYPE == BOARD_TK_A_V2_0)
  #define C_WP_D_PIN       	IO_G6
  #define C_STORAGE_CD_PIN 	IO_G5
  #define C_CFC_RST_PIN    0x61//IOG[1]
  #define C_CFC_CD_PIN     0x62//IOG[2]
  #define C_USBDEVICE_PIN  	IO_H4
  #define C_USBHOST_PWR_PIN IO_H3
#elif (BOARD_TYPE == BOARD_TK_A_V2_1_5KEY)
  #define C_WP_D_PIN       	IO_G6
  #define C_STORAGE_CD_PIN 	IO_G5
  #define C_CFC_RST_PIN    0x61//IOG[1]
  #define C_CFC_CD_PIN     0x62//IOG[2]
  #define C_USBDEVICE_PIN  	IO_H4
  #define C_USBHOST_PWR_PIN IO_H3
#elif BOARD_TYPE == BOARD_TK_32600_V1_0 
  #define C_WP_D_PIN       	IO_H3
  #define C_STORAGE_CD_PIN 	IO_H5
  #define C_USBDEVICE_PIN  	IO_H4
  #define C_USBHOST_PIN     IO_F8
  #define C_USBHOST_PWR_PIN IO_H2
#elif BOARD_TYPE < BOARD_TURNKEY_BASE
  #define C_CFC_RST_PIN     IO_G1
  #define C_CFC_CD_PIN      IO_G2
  //#define C_USBHOST_PIN   IO_H1
#if BOARD_TYPE == BOARD_EMU_V2_0
  #define C_WP_D_PIN       	IO_G6
  #define C_STORAGE_CD_PIN 	IO_G5
  #define C_USBDEVICE_PIN   IO_F3
#elif BOARD_TYPE == BOARD_DEMO_GPL32XXX
  #define C_WP_D_PIN       	IO_F3
  #define C_STORAGE_CD_PIN 	IO_F2
  #define C_USBDEVICE_PIN   IO_F5
#elif BOARD_TYPE == BOARD_DEMO_GPL326XX
  #define C_WP_D_PIN       	IO_H7
  #define C_STORAGE_CD_PIN 	IO_H6
  #define C_USBDEVICE_PIN   IO_F5  
#elif BOARD_TYPE == BOARD_DEMO_GPL327XX
  #define C_WP_D_PIN       	IO_F7
  #define C_STORAGE_CD_PIN 	IO_F6
  #define C_USBDEVICE_PIN   IO_C3 
#elif BOARD_TYPE == BOARD_DEMO_GPL326XXB
  #define C_WP_D_PIN       	IO_F8
  #define C_STORAGE_CD_PIN 	IO_A12
  #define C_USBDEVICE_PIN   IO_F5 
#else
  #define C_WP_D_PIN       	IO_G6
  #define C_STORAGE_CD_PIN 	IO_G5
  #define C_USBDEVICE_PIN   IO_F3 
#endif
#endif

#define C_DATA3_PIN       38//IOC[6]
#define C_DATA1_PIN       40//IOC[8]
#define C_DATA2_PIN       41//IOC[9]

//////////////////////////////////////////
//add by crf
#define C_MODE_PIN   	  IO_A0
OS_EVENT *USBTaskQ;
////////////////////////////////////////

#ifndef  USBH_DETECTION_MODE
  #define USBH_DETECTION_MODE USBH_DP_D
#endif

#ifdef _PROJ_TYPE
#define MSG_STOR_PLUG_IN              0x10000506
#define MSG_STOR_PLUG_OUT             0x10000507
#if  _PROJ_TYPE == _PROJ_TURNKEY
	DRV_PLUG_STATUS_ST sys_stor_msg;
	fp_msg_qsend stor_msg_QSend;
	void   *stor_msgQId;
	INT32U stor_msgId;

	DRV_PLUG_STATUS_ST sys_usbh_msg;
	fp_msg_qsend usbh_msg_QSend;
	void   *usbh_msgQId;
	INT32U usbh_msgId;

    DRV_PLUG_STATUS_ST sys_noise_msg;
	fp_msg_qsend noise_msg_QSend;
	void   *noise_msgQId;
	INT32U noise_msgId;
  #endif
#endif

INT8U   s_usbh_isq_lock = 0;
INT16U  s_state;
INT16U  storage_ststbl[8];
INT8U   s_other_cd_pin;    // CD PIN STATE
INT8U   s_cf_cd_pin;       // CD PIN STATE
INT8U   s_usbd_pin;        // CD PIN STATE
INT8U   s_usbh_dpdm;       // usbh signal
INT8U   s_card_work;       // usbh signal

////////////////////////////////////////////////
//add by crf
extern INT8U   s_usbmode_check_pin;
extern BOOLEAN usb_plug_states_flag;
extern BOOLEAN Usb_Dvice_WebCam_Demo_turn_on_flag;
///////////////////////////////////////////////

#if USB_NUM >= 1  
#if(USBH_DETECTION_MODE == USBH_GPIO_D )
INT8U   s_usbh_pin;        // usbh detection pin
INT8U   s_wait_dp_state =0;
INT16U  s_dp_cnt =0;
INT16U  s_wait_time =0;
#elif(USBH_DETECTION_MODE == USBH_GPIO_D_1 )
INT8U   s_usbh_pin;        // usbh detection pin
INT8U   s_wait_dp_state =0;
INT16U  s_dp_cnt =0;
INT16U  s_wait_time =0;
INT8U   s_detection_state =0;
#elif(USBH_DETECTION_MODE == USBH_GPIO_D_2 )
//INT8U   s_usbh_pin;        // usbh detection pin
INT8U   s_usbh_pin;        // usbh detection pin
INT8U   s_wait_dp_state =0;
INT16U  s_dp_cnt =0;
INT16U  s_wait_time =0;
INT16U  s_usbh_pwr_state =0;
INT16U  s_usbh_pwr_wait  =0;
INT16U  s_usbh_pwr_cnt   =0;
#endif
#endif

INT8U   s_usbh_usbd_state =0;        //
//INT16U   usbh_debouce;       //
//INT16U   usbh_noise_debouce;       //
//INT16U   usbh_noise_flag;

PIN_BOUCE  stor_pin_bouce[5];
//INT32U  storage_test =0;       //
/*
struct Usb_Storage_Access *UsbReadWrite[5] = {
 &USBD_MS_ACCESS,
 &USBD_SD_ACCESS,
 &USBD_CFC_ACCESS,
 &USBD_NF_ACCESS,
};
*/

struct Usb_Storage_Access *UsbReadWrite[MAX_DISK_NUM] = {
#if NAND1_EN == 1
//Domi1
	&USBD_NF_ACCESS,
#elif (MAX_DISK_NUM>0)
    NULL,
#endif

/* B:\> */
#if NAND2_EN == 1
//#err
	&USBD_NF_ACCESS_2,
#elif (MAX_DISK_NUM>1)
    NULL,
#endif

/* C:\> */
#if SD_EN == 1
	&USBD_SD_ACCESS,
#elif (MAX_DISK_NUM>2)
    NULL,
#endif

/* D:\> */
#if MSC_EN == 1
	&USBD_MS_ACCESS,
#elif (MAX_DISK_NUM>3)
    NULL,
#endif

/* E:\> */
#if CFC_EN == 1
//Domi4
	&USBD_CFC_ACCESS,
#elif (MAX_DISK_NUM>4)
    NULL,
#endif

/* F:\> */
#if XD_EN == 1
	&USBD_XDC_ACCESS,
#elif (MAX_DISK_NUM>5)
    NULL,
#endif

/* G:\> */
#if USB_NUM >= 1
	NULL,
#elif (MAX_DISK_NUM>6)
    NULL,
#endif

/* H:\> */
//#if USB_NUM >= 2
//    NULL,
//#elif (MAX_DISK_NUM>7)
//    NULL,
//#endif
#if NOR_EN == 1
	&USBD_SPI_ACCESS,
#elif (MAX_DISK_NUM>7)
    NULL,
#endif

/* I:\> */
#if USB_NUM >= 3
	NULL,
#elif (MAX_DISK_NUM>8)
    NULL,
#endif

/* N:\> */
#if NAND_APP_EN == 1
	&USBD_NF_APP_ACCESS,
#elif (MAX_DISK_NUM>9)
    NULL,
#endif

/* J:\> */
#if USB_NUM >= 4
	NULL,
#elif (MAX_DISK_NUM>11)
    NULL,
#endif

/* K:\> */
#if NAND3_EN == 1
	&FS_NAND3_driver,
#elif (MAX_DISK_NUM>11)
    NULL,
#endif

/* L:\> */
#if 0 // NOR_EN == 1
	&USBD_SPI_ACCESS,
#elif (MAX_DISK_NUM>11)
    NULL,
#endif

/* M:\> */
#if RAMDISK_EN == 1
	&USBD_RAMDISK_ACCESS,
#elif (MAX_DISK_NUM>12)
    NULL,
#endif
};
#if MSC_EN == 1
	extern INT16U *buffer_ms;
#endif
	
static void storage_detection_isr(void);
//void storage_polling_null(void);
//void storages_cfc_poweroff(void);
void storage_polling_usbh(void);
void usb_h_and_d_isr(void);
#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
BIN_STG_PLUG bin_stg_plug_get(void);
BIN_STG_PLUG_TYPE bin_stg_change_type_get(void);
void bin_stg_init(void);
#endif

#if(USBH_DETECTION_MODE ==USBH_SWITCH_D)
void storage_H_D_switch(INT32U ifH);
#endif

#if(USBH_DETECTION_MODE == USBH_GPIO_D_2 )
void usbh_poll_dp_down(void);
#endif

#if 0

INT8U storage_debouce(INT8U pin,INT32U ioport)
{
  INT16U  current_cd_pin,change_cnt,debounce_cnt;
  INT16U   noise_count;
  current_cd_pin = gpio_read_io(ioport);
  change_cnt  =0;
  noise_count =0;
  if (current_cd_pin != pin) //
  {
    for (debounce_cnt=0; debounce_cnt<=500; debounce_cnt++)
	{
		if (change_cnt >= 500)
		{
			break;
		}
		sw_timer_us_delay(1000);
		current_cd_pin = gpio_read_io(ioport);
        if (current_cd_pin == pin)
		{
		    noise_count++;
			change_cnt++;
			debounce_cnt = 0;
			continue;
		}
	}
  }
  if (noise_count>200) current_cd_pin |=0x80 ; //bit 7 as nois bit
  return current_cd_pin;
}

#else

INT8U storage_debouce(INT8U pin,INT32U ioport,INT16U  pin_id)
{
    INT16U  current_cd_pin;
    current_cd_pin = gpio_read_io(ioport);
	if (current_cd_pin != pin)             //
  	{
       stor_pin_bouce[pin_id].debouce++;
	}
	else
	{
	   if (stor_pin_bouce[pin_id].debouce >=8) //if noise happen  // 62ms
	   {
#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
	      if (!stor_pin_bouce[pin_id].noise_flag)
	      {
  	       // DBG_PRINT ("NOISY2~~~\r\n");
  	        if (pin_id ==0) {
 	          sys_noise_msg.storage_id =DRV_FS_DEV_SDMS ;
 	        }else  if (pin_id ==2)
 	        {
 	          sys_noise_msg.storage_id = DRV_FS_DEV_CF;
 	         }
             noise_msg_QSend(noise_msgQId,noise_msgId,(void*)&sys_noise_msg,sizeof(DRV_PLUG_STATUS_ST),0);
          }
#endif
	      stor_pin_bouce[pin_id].noise_flag   =1; //noise event!!
	      stor_pin_bouce[pin_id].noise_debouce =0;
	   }
       stor_pin_bouce[pin_id].debouce =0;
	}
	if (stor_pin_bouce[pin_id].noise_flag)  stor_pin_bouce[pin_id].noise_debouce++;

    if (stor_pin_bouce[pin_id].debouce ==64) //64 x (1/128)  = 500ms
    {
       if (stor_pin_bouce[pin_id].noise_flag) //clear noise event!!
       {
          stor_pin_bouce[pin_id].noise_flag   =0;
      	  stor_pin_bouce[pin_id].noise_debouce=0;
       }
       stor_pin_bouce[pin_id].debouce =0;
       return current_cd_pin;
    }
    if ( stor_pin_bouce[pin_id].noise_debouce > 102) //800ms
    {
    	stor_pin_bouce[pin_id].noise_flag    =0 ;
     	stor_pin_bouce[pin_id].noise_debouce =0 ;
     	current_cd_pin |=0x80 ; //bit 7 as nois bit
      	return current_cd_pin;
    }
    return pin;
}

#endif

#if USB_NUM >= 1
#if((USBH_DETECTION_MODE == USBH_GPIO_D ) || (USBH_DETECTION_MODE == USBH_GPIO_D_1 )|| (USBH_DETECTION_MODE == USBH_GPIO_D_2 ))
INT16U usbh_wait_dp_up(INT16U poling_id)
{
  INT16U ret =0;
  poling_id =(poling_id & 0x00ff);
  if  (poling_id == 0x87)      //start state
  {
    s_wait_dp_state =1;
    s_wait_time     =0;
    s_dp_cnt        =0; 
  }
  else if(poling_id == 0x07)  // end state
  {
    s_wait_dp_state =0;    
    ret = 0x07;
  }
  else                       
  {
   if (s_wait_dp_state ==1)   // wait state
   {
     s_wait_time++;
     if ( USB_Host_Signal())
      s_dp_cnt++;
     else    
      s_dp_cnt=0;      
     if (s_dp_cnt >64)      // over 500ms dp is pulled up
     {
        s_wait_dp_state =0;
        ret = 0x87;  //plug in message
     }     
     if (s_wait_time >1280)  // ((over 10 sec ))The pendriver maybe crash !
     {
        s_wait_dp_state =0;
     }         
   }
   else
   {
     ret = poling_id;     
   }
  }  
  return ret ;  
}
#endif

#if(USBH_DETECTION_MODE == USBH_GPIO_D_2 )
void usbh_poll_dp_down()
{
   INT16U  current_cd_pin;
    current_cd_pin =(INT16U) USB_Host_Signal();
  	if (current_cd_pin != s_usbh_dpdm) // IO detection & debouce 10ms
  	{
       stor_pin_bouce[3].debouce++;
	}
	else
	{
	   if (stor_pin_bouce[3].debouce >=20) //if noise happen  // 62ms
	   {	   
	      stor_pin_bouce[3].noise_flag   =1; //noise event!!
	   }
       stor_pin_bouce[3].debouce =0;
	}
	//if (stor_pin_bouce[3].noise_flag)
	//{
 	//    USB_Host_Handup(current_cd_pin);
	//	stor_pin_bouce[3].noise_debouce++;
	//}
    if (stor_pin_bouce[3].debouce ==100) //64 x (1/128)  = 500ms
    {
       if (stor_pin_bouce[3].noise_flag) //clear noise event!!
       {
          stor_pin_bouce[3].noise_flag   =0;
      	  stor_pin_bouce[3].noise_debouce=0;
       }
       stor_pin_bouce[3].debouce =0;
       s_usbh_dpdm =  current_cd_pin ;
	   storage_ststbl[3] = current_cd_pin;
       USB_Host_Handup(s_usbh_dpdm);
   }
}
#endif

#endif

void usb_h_and_d_isr(void)
{
   //DBG_PRINT ("usb isr!\r\n");   
   if (USBD_PlugIn)
   {
#if ((defined _DRV_L1_USBD) ) &&(_DRV_L1_USBD ==CUSTOM_ON)    
	usb_isr();
#endif	
	 USBH_INT_CLR_IMMEDIATE();
   }
   else
   {
   if (s_usbh_isq_lock)
     USBH_INT_CLR_IMMEDIATE();
   else
	usbh_isr();
   }
	//usb_isr_clear();
}

#if CFC_EN == 1
void storages_cfc_reset(void)  //PWR ON and OFF 
{
    gpio_write_io(C_DATA3_PIN, 0);
    gpio_write_io(C_DATA1_PIN, 0);
    gpio_write_io(C_DATA2_PIN, 0);
    gpio_write_io(C_CFC_RST_PIN, 1);
    sw_timer_us_delay(200000);
    //sw_timer_ms_delay(200);
    gpio_write_io(C_CFC_RST_PIN, 0);
    sw_timer_us_delay(200000);
}
void storages_cfc_poweroff(void)
{
  if (BOARD_TYPE != BOARD_EMU_V2_0 ) //CF_INS of BOARD_EMU_V2_0 Board is connect to V33CF
  {
    	gpio_write_io(C_CFC_RST_PIN, 1);//PMOS OFF
  }
}
#endif
//////////////////////////////////////////////////////////
//
//  Storage polling(MS/SD/CF/XD) and USBhost irq registing
//////////////////////////////////////////////////////////

#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
void turnkey_stor_detection_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
    stor_msg_QSend = (fp_msg_qsend) msg_qsend;
	stor_msgQId = msgq_id;
	stor_msgId = msg_id;
}

void turnkey_usbh_detection_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
    usbh_msg_QSend = (fp_msg_qsend) msg_qsend;
	usbh_msgQId = msgq_id;
	usbh_msgId = msg_id;
}

void turnkey_stor_noise_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
    noise_msg_QSend = (fp_msg_qsend) msg_qsend;
	noise_msgQId    = msgq_id;
	noise_msgId     = msg_id;
}

#endif
/////////////////////////////////////////////////////////////////////////////////////////
//devicetype :bit   0     1     2      3          4         5        6      7
//            type  CF   SDC    MS   UHOST(DP)   USBD    Nandflash   XD    UHOST_PIN
///////////////////////////////////////////////////////////////////////////////////////
void storages_init(INT16U devicetpye )
{
#if USB_NUM >= 1
  INT16U DP_cnt;  
#endif
  INT16U i;
//#if _OPERATING_SYSTEM == 1
	/* register USB host */
#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT8U err = NULL;
	OSSemPend(gFS_sem, 0, &err);
#endif
  
#if 0 //for irq 
   usbh_detection_init();
#endif   

   // I/O PIN initial for detection
//#endif
  //GPIO settings
  gpio_set_sdramio(); //set unused io of sdram
  #if BOARD_TYPE != BOARD_TK_32600_V1_0
  gpio_set_memcs(3,0);//cs3 as usb device cd pin
  #endif
  //GPIO G[5] CD PIN
  gpio_init_io(C_STORAGE_CD_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_STORAGE_CD_PIN, 0);
  gpio_write_io(C_STORAGE_CD_PIN,1);
  //GPIO G[2] as CD PIN

#if CFC_EN == 1
  gpio_init_io(C_CFC_CD_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_CFC_CD_PIN, 1);
#endif

  //GPIO F[3] as USB device plugin/out detection PIN (INPUT LOW)
  gpio_init_io(C_USBDEVICE_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_USBDEVICE_PIN, 0);
  gpio_write_io(C_USBDEVICE_PIN,0);
#if USB_NUM >= 1  
#if(USBH_DETECTION_MODE == USBH_GPIO_D )
  //GPIO  USB Host  plugin/out detection PIN 
  
#if((BOARD_TYPE == BOARD_TK_V4_3) || (BOARD_TYPE ==BOARD_TK_A_V1_0) )
  gpio_sdram_swith(5,1); //set SDRAM CKE as I/O
#endif
  gpio_init_io(C_USBHOST_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_USBHOST_PIN, 1);
 // gpio_write_io(C_USBHOST_PIN,0);  
 //GPIO  USB Host  Power control
  gpio_init_io(C_USBHOST_PWR_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_USBHOST_PWR_PIN, 1);
  gpio_write_io(C_USBHOST_PWR_PIN,1);  
  
#elif (USBH_DETECTION_MODE == USBH_GPIO_D_1 )  
#if((BOARD_TYPE == BOARD_TK_V4_3) || (BOARD_TYPE ==BOARD_TK_A_V1_0) )
  gpio_sdram_swith(5,1); //set SDRAM CKE as I/O
#endif
  gpio_init_io(C_USBHOST_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_USBHOST_PIN, 1);
 // gpio_write_io(C_USBHOST_PIN,0);  
 //GPIO  USB Host  Power control
  gpio_init_io(C_USBHOST_PWR_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_USBHOST_PWR_PIN, 1);
  gpio_write_io(C_USBHOST_PWR_PIN,0);  
  
#elif (USBH_DETECTION_MODE == USBH_SWITCH_D )  
//C_USBHOST_PIN is output mode for H switch
  gpio_init_io(C_USBHOST_PIN,GPIO_OUTPUT); 
  gpio_set_port_attribute(C_USBHOST_PIN, 1);
  gpio_write_io(C_USBHOST_PIN,0);
  //C_USBHOST_PWR_PIN is output mode for D switch
  gpio_init_io(C_USBHOST_PWR_PIN,GPIO_OUTPUT); 
  gpio_set_port_attribute(C_USBHOST_PWR_PIN, 1);
  gpio_write_io(C_USBHOST_PWR_PIN,1); 
#elif (USBH_DETECTION_MODE == USBH_GPIO_D_2 )    
#if((BOARD_TYPE == BOARD_TK_V4_3) || (BOARD_TYPE ==BOARD_TK_A_V1_0) )
  gpio_sdram_swith(5,1); //set SDRAM CKE as I/O
#endif
  gpio_init_io(C_USBHOST_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_USBHOST_PIN, 1);
 // gpio_write_io(C_USBHOST_PIN,0);  
 //GPIO  USB Host  Power control
  gpio_init_io(C_USBHOST_PWR_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_USBHOST_PWR_PIN, 1);
  gpio_write_io(C_USBHOST_PWR_PIN,0);  
#endif

#if ((BOARD_TYPE ==BOARD_TK_A_V2_0) || (BOARD_TYPE == BOARD_TK_A_V2_1_5KEY))
  //GPIO  USB Host  Power control
  gpio_init_io(C_USBHOST_PWR_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_USBHOST_PWR_PIN, 1);
  gpio_write_io(C_USBHOST_PWR_PIN,1);
#endif 
#if (USBH_DETECTION_MODE == USBH_GPIO_D_2 )    
  gpio_write_io(C_USBHOST_PWR_PIN,0);
  s_usbh_pwr_state =0;
#endif

#endif



  //GPIO G[6] as WPB PIN
  gpio_init_io(C_WP_D_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_WP_D_PIN, 0);
  gpio_write_io(C_WP_D_PIN,1);
  
#if CFC_EN == 1
  //GPIO G[2] as CFC rest pin
  gpio_init_io(C_CFC_RST_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_CFC_RST_PIN, 1);
  gpio_write_io(C_CFC_RST_PIN, 0);
#endif
  //
  gpio_init_io(C_DATA3_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_DATA3_PIN, 1);
  gpio_init_io(C_DATA1_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_DATA1_PIN, 1);
  gpio_init_io(C_DATA2_PIN,GPIO_OUTPUT);
  gpio_set_port_attribute(C_DATA2_PIN, 1);

  sw_timer_us_delay(90000); /* wait port stable */
  
  s_other_cd_pin = gpio_read_io(C_STORAGE_CD_PIN);
#if CFC_EN == 1
  s_cf_cd_pin = gpio_read_io(C_CFC_CD_PIN);
#endif
  s_usbd_pin  = gpio_read_io(C_USBDEVICE_PIN); 

  if (!s_usbd_pin)
  {
  	      usb_uninitial();
#if  MCU_VERSION  >= GPL326XX //usb2.0 Host  	      
          USB_Host_Initial();  	      
#else          
  	      USB_Host_Enable();
#endif  	      
  }
#if (USBH_DETECTION_MODE == USBH_GPIO_D )       
  if (s_usbd_pin)
     gpio_write_io(C_USBHOST_PWR_PIN,0);
       
#elif (USBH_DETECTION_MODE == USBH_GPIO_D_1 )     
  if (s_usbd_pin)
    s_detection_state = 1;
  else
  {
    gpio_write_io(C_USBHOST_PWR_PIN,1);   
    sw_timer_us_delay(50); 
    
  }
#elif (USBH_DETECTION_MODE == USBH_SWITCH_D )       
  if (!s_usbd_pin)
    storage_H_D_switch(1);
#elif (USBH_DETECTION_MODE == USBH_GPIO_D_2 )     
 
#endif

#if MSC_EN == 1  
  ms_hand_set(!s_other_cd_pin);//send to ms/pro cd status
#endif

  //uninital all
  gpio_write_io(C_DATA3_PIN, 1);
  gpio_write_io(C_DATA1_PIN, 1);
  gpio_write_io(C_DATA2_PIN, 1);
  drvl2_sd_card_remove();
  gpio_write_io(C_DATA3_PIN, 0);
  gpio_write_io(C_DATA1_PIN, 0);
  gpio_write_io(C_DATA2_PIN, 0);

#if CFC_EN == 1
  cfc_uninit();
#endif

#if MSC_EN == 1
  ms_uninitial();

  //ms tbl
 if (buffer_ms != NULL){
     gp_free(buffer_ms);
     buffer_ms =NULL;
  }
#endif

  s_state =0;
  for (i=0;i<8;i++)
     storage_ststbl[i] =0;
     
#if CFC_EN == 1             
 if( (devicetpye &0x01) && (!s_cf_cd_pin) )
  {

       storages_cfc_reset(); 
       storage_ststbl[0] =!(INT16U) cfc_initial();
       cfc_uninit();
  }
#endif

  if (!s_other_cd_pin)
  {
#if SD_EN == 1
	  if (devicetpye &0x02)
	  {
    	 gpio_write_io(C_DATA3_PIN, 1);
	     gpio_write_io(C_DATA1_PIN, 1);
    	 gpio_write_io(C_DATA2_PIN, 1);
	     storage_ststbl[1] = !(INT16U) drvl2_sd_init();
		 drvl2_sd_card_remove();
	  }
#endif

#if MSC_EN == 1
  	  if (devicetpye &0x04)
  	  {
    	 if (!storage_ststbl[1])
      	 {
        	gpio_write_io(C_DATA3_PIN, 0);
     		gpio_write_io(C_DATA1_PIN, 0);
        	gpio_write_io(C_DATA2_PIN, 0);
        	buffer_ms = (INT16U*) gp_malloc(8192*2);
        	if (buffer_ms != NULL){
       			 ms_table_init(buffer_ms);
       			 ms_detective_flag_set(1);
	       		 storage_ststbl[2] = !ms_initial();
	       		 ms_detective_flag_set(0);
            }
//        	ms_uninitial();		//Daniel add for Aqua entering problem with MS card
//         	gp_free(buffer_ms);
//         	buffer_ms =NULL;
     	 }
      }
#endif

#if XD_EN == 1
  	  if (devicetpye &0x40)
  	  {
    	 if ((!storage_ststbl[1]) && (!storage_ststbl[2]))
      	 {
        	gpio_write_io(C_DATA3_PIN, 0);
     		gpio_write_io(C_DATA1_PIN, 0);
        	gpio_write_io(C_DATA2_PIN, 0);
      	 	//DBG_PRINT ("storage initial start! \r\n");	
      	 	storage_ststbl[6]  = !DrvXD_initial();
      	 	DrvXD_uninit();
         }
    }       
#endif
  
  }
  else		//Daniel add for Aqua entering problem with MS card plug-in
  {
#if MSC_EN == 1
  	  if (devicetpye &0x04)
  	  {
    	 if (!storage_ststbl[1])
      	 {
        	gpio_write_io(C_DATA3_PIN, 0);
     		gpio_write_io(C_DATA1_PIN, 0);
        	gpio_write_io(C_DATA2_PIN, 0);
        	buffer_ms = (INT16U*) gp_malloc(8192*2);
        	if (buffer_ms != NULL){
       			 ms_table_init(buffer_ms);
       			 ms_detective_flag_set(1);
	       		 storage_ststbl[2] = !ms_initial();
	       		 ms_detective_flag_set(0);
            }
     	 }
      }
#endif  
  }

#if USB_NUM >= 1
// s_usbh_dpdm = (INT8U) USB_Host_Signal();  
#if(USBH_DETECTION_MODE == USBH_GPIO_D )
  s_usbh_pin = gpio_read_io(C_USBHOST_PIN);  
  if (s_usbh_pin)
  {   
      DP_cnt =0;
	  for(i = 0 ; i <3000; i++)
	  {
	    sw_timer_ms_delay(1); 
    	if ( USB_Host_Signal() )
	       DP_cnt ++;       
        if (DP_cnt >500) break;	//over 500ms       
	  }  
	  if (DP_cnt >500)
	    s_usbh_dpdm =1;
	  else  
	    s_usbh_dpdm =0;
  }else
  {
     s_usbh_dpdm =0;
  } 
#elif( (USBH_DETECTION_MODE == USBH_DP_D ) || ( USBH_DETECTION_MODE == USBH_SWITCH_D) )
  DP_cnt =0;
  for(i = 0 ; i <1000; i++)
  {
    sw_timer_ms_delay(1); 
   	if ( USB_Host_Signal() )
	       DP_cnt ++;       
    if (DP_cnt >500) break;	 //over 500ms              
  }  
  if (DP_cnt >500)
    s_usbh_dpdm =1;
  else  
    s_usbh_dpdm =0;      
#elif(USBH_DETECTION_MODE == USBH_GPIO_D_1 )    
  if (s_detection_state ==0)
  {
	  s_usbh_pin = gpio_read_io(C_USBHOST_PIN);  
	  if (s_usbh_pin)
	  {  
    	  DP_cnt =0;
		  for(i = 0 ; i <1000; i++)
		  {
	    	sw_timer_ms_delay(1); 
	    	if ( USB_Host_Signal() )
		       DP_cnt ++;       
        	if (DP_cnt >500) break;	//over 500ms       
		  }  
	  	  if (DP_cnt >500)
	      	s_usbh_dpdm =1;
	      else  
	        s_usbh_dpdm =0;
	  }else
      {
	      s_usbh_dpdm =0;
	  } 	      
      s_detection_state =3;	  
  }else
  {
     s_usbh_dpdm =0;
  } 
#elif(USBH_DETECTION_MODE == USBH_GPIO_D_2 )      
  s_usbh_pin = gpio_read_io(C_USBHOST_PIN);  
  if (s_usbh_pin)
  {  
     if (!s_usbd_pin)
     {
           //pwn 5v
          gpio_write_io(C_USBHOST_PWR_PIN,1); 
    	  sw_timer_us_delay(5000); 
          //USB_Host_Initial();
          s_usbh_pwr_state =1;
	      DP_cnt =0;
		  for(i = 0 ; i <3000; i++)
		  {
		    sw_timer_ms_delay(1); 
	    	if ( USB_Host_Signal() )
		       DP_cnt ++;       
		    else   
  		       DP_cnt=0;
        	if (DP_cnt >500) break;	//over 500ms       
		  }  
		  if (DP_cnt >500)
		  {
	    	s_usbh_dpdm =1;
	      }
		  else  
		  {
		    s_usbh_dpdm =0;
		  } 
	 }	
	     	    
  }else
  {
     s_usbh_dpdm =0;
  } 
#endif  
  USB_Host_Handup(s_usbh_dpdm);//send to uhost  DP status      
  if ((devicetpye &0x08) && (s_usbh_dpdm) )
  {
       s_usbh_isq_lock =1;
       //system_usbh_uncheat();
       storage_ststbl[3] = !DrvUSBHMSDCInitial();
       //system_usbh_cheat();
       s_usbh_dpdm       =  storage_ststbl[3];
       //DrvUSBHMSDCUninitial();
  } 
  
#if((USBH_DETECTION_MODE == USBH_GPIO_D )||(USBH_DETECTION_MODE == USBH_GPIO_D_1 ) ||(USBH_DETECTION_MODE == USBH_GPIO_D_2 ))
  if (devicetpye &0x80)
 		 storage_ststbl[7] =  s_usbh_pin ;  
#endif 		   
#endif

  if (devicetpye &0x10)
  {
       storage_ststbl[4] = s_usbd_pin;
  }


#if (NAND1_EN == 1)
  if (devicetpye &0x20)
  {
#if NAND_CS_POS == NF_CS_AS_BKCS1
       gpio_init_io(C_BKCS1_PIN,GPIO_OUTPUT);
       gpio_set_port_attribute(C_BKCS1_PIN, 1);
       gpio_write_io(C_BKCS1_PIN, 1);  
#elif NAND_CS_POS == NF_CS_AS_BKCS2       
       gpio_init_io(C_BKCS2_PIN,GPIO_OUTPUT);
       gpio_set_port_attribute(C_BKCS2_PIN, 1);
       gpio_write_io(C_BKCS2_PIN, 1);  
#endif
#if MCU_VERSION <= GPL326XX_C
#if (NAND_APP_EN == 1) //for nand app area testing
       storage_ststbl[5] = !(INT16U)NAND_APP_Initial();
       NAND_APP_Uninitial();
#else	//for nand data area
       storage_ststbl[5] = !(INT16U)NAND_Initial();
       NAND_Uninitial();
#endif
#else
       storage_ststbl[5] = !(INT16U)NAND_Initial();
       NAND_Uninitial();
#endif
      // nand_flash_uninit();
  }
#elif (NOR_EN == 1)
	if (devicetpye &0x20)
	{
		storage_ststbl[5] = !(INT16U)spi_flash_disk_initial();//1;
	}
#endif

  //register interrupter_isq and timer_isr
  vic_irq_register(VIC_USB, usb_h_and_d_isr);
  vic_irq_enable(VIC_USB);
  
  storage_polling_start();
  s_usbh_isq_lock =0;
  s_card_work =0;
  //0:SD 1:USBD 2:CF 3:USBH 4:UHOSH_PIN
  for(i = 0 ; i <5; i++)
  {
    stor_pin_bouce[i].debouce = 0;
    stor_pin_bouce[i].noise_debouce = 0;
    stor_pin_bouce[i].noise_flag = 0;
  }
#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
   	OSSemPost(gFS_sem);
#endif

}


//INT16U htest =0;
//INT16U htest2 =0;

#if (USB_NUM >= 1)
#if(USBH_DETECTION_MODE == USBH_DP_D )
void storage_polling_usbh(void)
{
   INT16U  current_cd_pin;
    current_cd_pin =(INT16U) USB_Host_Signal();
  	if (current_cd_pin != s_usbh_dpdm) // IO detection & debouce 10ms
  	{
       stor_pin_bouce[3].debouce++;
	}
	else
	{
	   if (stor_pin_bouce[3].debouce >=20) //if noise happen  // 62ms
	   {
	   
#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
	      if (!stor_pin_bouce[3].noise_flag)
	      {
  	       //DBG_PRINT ("USBH NAUGHTY PLUG.\r\n");
	        sys_noise_msg.storage_id = DRV_FS_DEV_USBH;
            noise_msg_QSend(noise_msgQId,noise_msgId,(void*)&sys_noise_msg,sizeof(DRV_PLUG_STATUS_ST),0);
          }
#endif

	      stor_pin_bouce[3].noise_flag   =1; //noise event!!
	      stor_pin_bouce[3].noise_debouce =0;
	   }
       stor_pin_bouce[3].debouce =0;
	}
	if (stor_pin_bouce[3].noise_flag)
	{
 	    USB_Host_Handup(current_cd_pin);
		stor_pin_bouce[3].noise_debouce++;
	}
    if (stor_pin_bouce[3].debouce ==64) //64 x (1/128)  = 500ms
    {
       if (stor_pin_bouce[3].noise_flag) //clear noise event!!
       {
          stor_pin_bouce[3].noise_flag   =0;
      	  stor_pin_bouce[3].noise_debouce=0;
       }
       stor_pin_bouce[3].debouce =0;
       s_usbh_dpdm =  current_cd_pin ;
	   storage_ststbl[3] = current_cd_pin;
       s_usbh_dpdm  = current_cd_pin;
       USB_Host_Handup(s_usbh_dpdm);

#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
       if (current_cd_pin)
       {
    	 sys_stor_msg.plug_status = DRV_PLGU_IN;
//    	 htest++;  DBG_PRINT ("usbH IN %x \r\n",htest);
       }
       else
       {
         USBH_SELF_RST();
	      sys_stor_msg.plug_status = DRV_PLGU_OUT;
//          htest++;  DBG_PRINT ("usbH OUT %x \r\n",htest);
       }
  	   sys_stor_msg.storage_id = DRV_FS_DEV_USBH;
       stor_msg_QSend(stor_msgQId,stor_msgId,(void*)&sys_stor_msg,sizeof(DRV_PLUG_STATUS_ST),0);
#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
       if (current_cd_pin)
       {
         bin_stg_plug_status_set(BIN_PLUG_IN);
       }
       else
       {
          bin_stg_plug_status_set(BIN_PLUG_OUT);
       }
       bin_stg_plug_type_set(BIN_STG_USBH_PLUG);
#endif

   }
#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
   if (stor_pin_bouce[3].noise_debouce == 100) //800ms
   {
      stor_pin_bouce[3].noise_flag    =0 ;
      stor_pin_bouce[3].noise_debouce =0 ;
      if (current_cd_pin)
      {
    	    sys_stor_msg.plug_status = DRV_PLGU_OUT;
     	    sys_stor_msg.storage_id = DRV_FS_DEV_USBH;
//            htest++;  DBG_PRINT ("usbH OUT %x \r\n",htest);
      		stor_msg_QSend(stor_msgQId,stor_msgId,(void*)&sys_stor_msg,sizeof(DRV_PLUG_STATUS_ST),0);
            sys_stor_msg.plug_status = DRV_PLGU_IN;
//            htest++;  DBG_PRINT ("usbH IN %x \r\n",htest);
	        stor_msg_QSend(stor_msgQId,stor_msgId,(void*)&sys_stor_msg,sizeof(DRV_PLUG_STATUS_ST),0);
//            htest2++; DBG_PRINT ("uH Noise %x \r\n",htest2);
      }
   }
#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
   if (stor_pin_bouce[3].noise_debouce == 100) //800ms
   {
      stor_pin_bouce[3].noise_flag    =0 ;
      stor_pin_bouce[3].noise_debouce =0 ;
      if (current_cd_pin)
      {
            bin_stg_plug_type_set(BIN_STG_USBH_PLUG);
            bin_stg_plug_status_set(BIN_PLUG_IN);
      }
   }   
#endif
}
#endif//USBH_DP_D

#if(USBH_DETECTION_MODE == USBH_SWITCH_D )
INT8U storage_H_debouce()
{
    INT16U  current_cd_pin;
    current_cd_pin =(INT16U) USB_Host_Signal();
	if (current_cd_pin != s_usbh_dpdm)             //
  	{
       stor_pin_bouce[3].debouce++;
	}
	else
	{
	   if (stor_pin_bouce[3].debouce >=20) //if noise happen  // 62ms
	   {
#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
	      if (!stor_pin_bouce[3].noise_flag)
	      {
   	        sys_noise_msg.storage_id = DRV_FS_DEV_USBH;
            noise_msg_QSend(noise_msgQId,noise_msgId,(void*)&sys_noise_msg,sizeof(DRV_PLUG_STATUS_ST),0); 
          }
#endif
	      stor_pin_bouce[3].noise_flag   =1; //noise event!!
	      stor_pin_bouce[3].noise_debouce =0;
	   }
       stor_pin_bouce[3].debouce =0;
	}
	if (stor_pin_bouce[3].noise_flag)  stor_pin_bouce[3].noise_debouce++;

    if (stor_pin_bouce[3].debouce ==64) //64 x (1/128)  = 500ms
    {
       if (stor_pin_bouce[3].noise_flag) //clear noise event!!
       {
          stor_pin_bouce[3].noise_flag   =0;
      	  stor_pin_bouce[3].noise_debouce=0;
       }
       stor_pin_bouce[3].debouce =0;
       return current_cd_pin;
    }
    if ( stor_pin_bouce[3].noise_debouce > 100) //800ms
    {
    	stor_pin_bouce[3].noise_flag    =0 ;
     	stor_pin_bouce[3].noise_debouce =0 ;
     	current_cd_pin |=0x80 ; //bit 7 as nois bit
      	return current_cd_pin;
    }
    return s_usbh_dpdm;
}


#endif //USBH_SWITCH_D

#endif //USB_NUM >= 1

#if 0
void storage_polling_null(void)
{
	sys_release_timer_isr(storage_detection_isr);
}
#endif
void storage_polling_start(void)
{
	sys_registe_timer_isr(storage_detection_isr);
}

void storage_polling_stop(void)
{
	sys_release_timer_isr(storage_detection_isr);
}

#define R_IOSRSEL	                (*((volatile INT32U *) 0xC0000104))  /*Dominant add, 06/17/2008*/
#define R_MEM_IO_CTRL               (*((volatile INT32U *) 0xD0200060))  /* Dominant add, 04/14/2008 */
#define R_FUNPOS1					(*((volatile INT32U *) 0xC0000114))
INT16U storage_polling(void)
{
  INT8U  current_cd_pin;
  INT16U result;
 // DBG_PRINT ("%x \r\n",R_FUNPOS1);

  result =0;
  //polling SDC PIN
  current_cd_pin = storage_debouce(s_other_cd_pin,C_STORAGE_CD_PIN,0);
  if((current_cd_pin & 0x0f) != s_other_cd_pin)
  {
    current_cd_pin &=0x7F;
   	result=1;
    s_other_cd_pin  = current_cd_pin;
   	if (!current_cd_pin)
       	   result |= 0x80;
#if MSC_EN == 1       	   
    ms_hand_set(!current_cd_pin);
#endif           	   
    return result;
  }else if(current_cd_pin & 0x80) //if noise
  {
    current_cd_pin &=0x7F;
    s_other_cd_pin  = current_cd_pin;
   	result=0x41;   //noise process
       	if (!s_other_cd_pin) {
       	   result |= 0x80;   //IN
        }else {
           result =   0;
        }
        
     return result;
  }
#if MSC_EN == 1       	
  if (stor_pin_bouce[0].noise_flag)
      ms_hand_set(!current_cd_pin);
#endif
  
 


  //polling CF PIN
#if CFC_EN == 1
  current_cd_pin = storage_debouce(s_cf_cd_pin,C_CFC_CD_PIN,2);
  if ((current_cd_pin & 0x0f) != s_cf_cd_pin)
  {
    current_cd_pin &=0x7F;
    result=2;
    s_cf_cd_pin  = current_cd_pin;
    if (!current_cd_pin)
           result |= 0x80;
    return result;
  }else if(current_cd_pin & 0x80) //if noise
  {
    current_cd_pin &=0x7F;
    s_cf_cd_pin  = current_cd_pin;
   	result=0x42;    //noise process
   	if (!s_cf_cd_pin)
       	   result |= 0x80;
    else
           result =   0;
     return result;
  }
#endif

#if(USBH_DETECTION_MODE == USBH_GPIO_D )
/////////////////////////////////////////////////////////////////////////////////////
  //polling USBD PIN
 // if ( !s_usbh_pin) 
  current_cd_pin = storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
  if ((current_cd_pin & 0x0f) != s_usbd_pin)
  {
    	current_cd_pin &=0x7F;
		result=4;
	    storage_ststbl[4] = current_cd_pin;
    	s_usbd_pin  = current_cd_pin;
	    if (current_cd_pin)
    	     result |= 0x80;   
#if USB_NUM >= 1    	      	     
        s_usbh_pin = storage_debouce(s_usbh_pin,C_USBHOST_PIN,4);
    	storage_ststbl[7] = s_usbh_pin;
    	if ( s_usbh_pin)
    	{
    	  if (s_usbh_usbd_state ==1)
    	  {  
    	    if (result ==0x04)   	         
    	    {
    	      s_usbh_usbd_state =0;
     	      result =0x87; //inint uhost    	  
     	    }
    	    else
      	      result =0;
    	  }else        	  
    	  {
    	    //GPIO  USB Host  Power OFF
		 	gpio_write_io(C_USBHOST_PWR_PIN,0);
    	    s_usbh_usbd_state = 1;
    	    result =0x07; //remove uhost    	  
    	  }    	  
    	}else{    	    	   
    	   if ((result ==0x04) && (s_usbh_usbd_state ==1)) 
    	   {
    	       	//GPIO  USB Host  Power ON
 		 	    gpio_write_io(C_USBHOST_PWR_PIN,1);
  	       	 	result =0;
    	    	s_usbh_usbd_state =0;         	  
    	    }
      	} 
#endif      	
	    return result;
  }else if(current_cd_pin & 0x80) //if noise
  {
    	current_cd_pin &=0x7F;
	    storage_ststbl[4] = current_cd_pin;
    	s_usbd_pin  = current_cd_pin;
	   	result=0x44;    //noise process
   		if (s_usbd_pin)
       		   result |= 0x80;
	    else
    	       result =   0;    
#if USB_NUM >= 1     	       	           	           	       
        s_usbh_pin = storage_debouce(s_usbh_pin,C_USBHOST_PIN,4);
    	storage_ststbl[7] = s_usbh_pin;    	    	
    	if ( s_usbh_pin)
    	{
    	  if (s_usbh_usbd_state ==1)
    	  {  
      	      result =0;
    	  }else        	  
    	  {
    	      //GPIO  USB Host  Power OFF
 		 	  gpio_write_io(C_USBHOST_PWR_PIN,0);
    	      s_usbh_usbd_state = 1;
    	      result =0x07; //remove uhost    	  
    	  }    	  
    	}  	  
#endif    	  	       
    	return result;
  }
#if USB_NUM >= 1
    //polling USBHOST PIN    
  current_cd_pin = storage_debouce(s_usbh_pin,C_USBHOST_PIN,4);
  if ((current_cd_pin & 0x0f) != s_usbh_pin)
  {
    current_cd_pin &=0x7F;
    result=7;
    storage_ststbl[7] = current_cd_pin;
    s_usbh_pin  = current_cd_pin;    
    if (current_cd_pin)
         result |= 0x80;         
         
    if ((s_usbh_usbd_state ==0) && (result ==0x87) && (s_usbd_pin) )
    {
        gpio_write_io(C_USBHOST_PWR_PIN,0);
        s_usbh_usbd_state =1;
        result =0x04; //remove usbd
    }
    else if (s_usbh_usbd_state ==1)
    {    
         s_usbd_pin =storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
         storage_ststbl[4] =s_usbd_pin;
         if (!s_usbd_pin)
         {
              s_usbh_usbd_state =0;         
         }      
         result=0;                         
    }         
    return result;
  }else if(current_cd_pin & 0x80) //if noise
  {
    current_cd_pin &=0x7F;
    storage_ststbl[7] = current_cd_pin;
    s_usbh_pin  = current_cd_pin;
   	result=0x67;    //noise process
   	if (s_usbh_pin)
       	   result |= 0x80;
    else
           result =   0;
           
    if ((s_usbh_usbd_state ==0) && (result ==0x87) && (s_usbd_pin) )
    {
        gpio_write_io(C_USBHOST_PWR_PIN,0);
        s_usbh_usbd_state =1;
        result =0x04; //remove usbd
    }                                  
    else if (s_usbh_usbd_state ==1)
    {    
         s_usbd_pin =storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
         storage_ststbl[4] =s_usbd_pin;
         if (!s_usbd_pin)
                 s_usbh_usbd_state =0;         
         result=0;                         
    } 
    return result;
  }
  
#endif
/////////////////////////////////////////////////////////////////////////////////////
#elif(USBH_DETECTION_MODE == USBH_GPIO_D_1)

 switch (s_detection_state)
 {
   case 1: //for Device
        current_cd_pin = storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
        if ((current_cd_pin & 0x0f) != s_usbd_pin)
        {
            current_cd_pin &=0x7F;
		    result=4;
		    storage_ststbl[4] = current_cd_pin;
		    s_usbd_pin  = current_cd_pin;
		    if (current_cd_pin)
		    {
        	 result |= 0x80;   
        	}
        	else      	         	 
        	{
        	 s_detection_state =2; 
        	}
		    return result;           
		    
        }else if(current_cd_pin & 0x80) //if noise
        {
		    current_cd_pin &=0x7F;
		    storage_ststbl[4] = current_cd_pin;
		    s_usbd_pin  = current_cd_pin;
		   	result=0x44;    //noise process
		   	if (s_usbd_pin)
       		   result |= 0x80;
		    else
		    {
        	   result =   0;           	 
               s_detection_state =2; 
            }
		    return result;        
        }else if (current_cd_pin==0) 
        {
         s_detection_state =2; 
        }
        break;
   case 2://open uhost pwd
        gpio_write_io(C_USBHOST_PWR_PIN,1);  
        s_detection_state =3; 
        return 0;    
        break;   
   case 3://for uhost 
    	current_cd_pin = storage_debouce(s_usbh_pin,C_USBHOST_PIN,4);
		if ((current_cd_pin & 0x0f) != s_usbh_pin)
		{
		    current_cd_pin &=0x7F;
		    result=7;
		    storage_ststbl[7] = current_cd_pin;
		    s_usbh_pin  = current_cd_pin;    
		    if (current_cd_pin)
		    {
        		 result |= 0x80;                  
            }
            else      	         	 
        	{
	        	 s_detection_state =4; 
        	}		 
		    return result;
		}else if(current_cd_pin & 0x80) //if noise
		{
  		    current_cd_pin &=0x7F;
		    storage_ststbl[7] = current_cd_pin;
		    s_usbh_pin  = current_cd_pin;
		   	result=0x67;    //noise process
		   	if (s_usbh_pin)
		   	{
       			   result |= 0x80;
       	    }
		    else
		    {
        		   result =   0;   
 				   s_detection_state =4;         		           
            }
	  		return result;
		}
        else if (current_cd_pin==0) 
        {
           s_detection_state =4; 
        }
        break;
   case 4://close uhost pwd
        gpio_write_io(C_USBHOST_PWR_PIN,0);  
        s_detection_state =1; 
        return 0;            
   default:
        break;         
 }
/////////////////////////////////////////////////////////////////////////////////////
#elif(USBH_DETECTION_MODE ==USBH_SWITCH_D)
/////////////////////////////////////////////////////////////////////////////////////
  //polling USBD PIN
  current_cd_pin = storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
  if ((current_cd_pin & 0x0f) != s_usbd_pin)
  {
    	current_cd_pin &=0x7F;
		result=4;
	    if (current_cd_pin)
    	     result |= 0x80;         	
  }else if(current_cd_pin & 0x80) //if noise
  {
    	current_cd_pin &=0x7F;
	   	result=0x44;    //noise process
   		if (current_cd_pin)
       		   result |= 0x80;
	    else
    	       result =   0;        	  	       
  }  
  if (result)
  {  
#if USB_NUM >= 1
   if ( s_usbh_dpdm )
   {
       result =0;
   }
   else
   {   
	  storage_ststbl[4] = current_cd_pin;
  	  s_usbd_pin  = current_cd_pin;  
  	  if (s_usbd_pin)
	  	  storage_H_D_switch(0);
  	  else
  	  {
  	      usb_uninitial();
  	      USB_Host_Enable();
	  	  storage_H_D_switch(1);
	  }
      return result;   
   }  
#else  
	  storage_ststbl[4] = current_cd_pin;
  	  s_usbd_pin  = current_cd_pin;  
      return result;
#endif  	  
  }
  
#if USB_NUM >= 1
    //polling USBHOST PIN             
  current_cd_pin = storage_H_debouce();
  if ((current_cd_pin & 0x0f) != s_usbh_dpdm)
  {
    current_cd_pin &=0x7F;
    result=7;
    if (current_cd_pin)
         result |= 0x80;         
                 
  }else if(current_cd_pin & 0x80) //if noise
  {
    current_cd_pin &=0x7F;
   	result=0x67;    //noise process
   	if (current_cd_pin)
       	   result |= 0x80;
    else
           result =   0;           
  }
  if (result)
  { 
     if (s_usbd_pin)
     {
        result =0;
     }
     else
     {  
	 	 storage_ststbl[3] = current_cd_pin;
		 s_usbh_dpdm  = current_cd_pin;  	   
	     USB_Host_Handup(s_usbh_dpdm); 
		 return result;  
	 }
  }
#endif

#elif(USBH_DETECTION_MODE == USBH_GPIO_D_2 )
/////////////////////////////////////////////////////////////////////////////////////
  //polling USBD PIN
 // if ( !s_usbh_pin) 
 #if USB_NUM >= 1
 if (s_usbh_pwr_wait)
 {
   s_usbh_pwr_cnt++;
   if((s_usbh_pwr_cnt >600) ||((s_usbh_pwr_cnt >64) &&  USB_Host_Signal()))
   {
     s_usbh_pwr_wait =0;
     return 0x87;
   }
   return 0;
 }
 #endif
  current_cd_pin = storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
  if ((current_cd_pin & 0x0f) != s_usbd_pin)
  {
    	current_cd_pin &=0x7F;
		result=4;
	    storage_ststbl[4] = current_cd_pin;
    	s_usbd_pin  = current_cd_pin;
	    if (current_cd_pin)
    	     result |= 0x80;   
    	if (s_usbd_pin)
    	{
#if USB_NUM >= 1    	      	         	
	   		if (s_usbh_dpdm)
    	    {
  	  		    //GPIO  USB Host  Power OFF
		 		gpio_write_io(C_USBHOST_PWR_PIN,0);
		 		s_usbh_pwr_state =0;
    	    	s_usbh_usbd_state = 1;
    	    	s_usbh_dpdm =0;
	   		    result =0x07; //remove uhost    	      	
    		}    		    		 
#endif    		
    	}
    	else
    	{
#if USB_NUM >= 1    	      	         	
        	s_usbh_pin = storage_debouce(s_usbh_pin,C_USBHOST_PIN,4);
	    	storage_ststbl[7] = s_usbh_pin;
    	    if ( s_usbh_pin)
    	    {
  	      	  	 //GPIO  USB Host  Power OFF
		 		 gpio_write_io(C_USBHOST_PWR_PIN,1);
		 		 s_usbh_pwr_state =1;
		 		 s_usbh_pwr_wait =1;
		 		 s_usbh_pwr_cnt  =0;
    		     s_usbh_usbd_state =0;
    		     s_usbh_dpdm =1;
    		     result =0;
     		 	 //result =0x87; //inint uhost    	      	    
    	    }    	
#endif      	    	    
    	}    	
	    return result;
  }else if(current_cd_pin & 0x80) //if noise
  {
    	current_cd_pin &=0x7F;
	    storage_ststbl[4] = current_cd_pin;
    	s_usbd_pin  = current_cd_pin;
	   	result=0x44;    //noise process
   		if (s_usbd_pin)
       		   result |= 0x80;
	    else
    	       result =   0;    
#if USB_NUM >= 1     	       	           	           	       
	   	if (s_usbh_dpdm)
    	{
    	  if (s_usbh_usbd_state ==1)
    	  {  
      	      result =0;
    	  }else        	  
    	  {
    	      //GPIO  USB Host  Power OFF
 		 	  gpio_write_io(C_USBHOST_PWR_PIN,0);
 		 	  s_usbh_pwr_state =0;
    	      s_usbh_usbd_state = 1;
          	  s_usbh_dpdm =0;
    	      result =0x07; //remove uhost    	  
    	  }    	  
    	}  	  
#endif    	  	       
    	return result;
  }
#if USB_NUM >= 1
    //polling USBHOST PIN    
  if (s_usbh_pwr_state ==0) 
  {
  		current_cd_pin = storage_debouce(s_usbh_pin,C_USBHOST_PIN,4);
		if ((current_cd_pin & 0x0f) != s_usbh_pin)
		{
		    current_cd_pin &=0x7F;
		    result=7;
		    storage_ststbl[7] = current_cd_pin;
		    s_usbh_pin  = current_cd_pin;    
		    if (current_cd_pin)
        		 result |= 0x80;  
        	if (s_usbd_pin)	    
        	{             
			    if (result ==0x87)
			    {
			        s_usbh_usbd_state =1;
        			result =0x04; //remove usbd
			    }
			    else if ( result ==0x07 )
			    {    
     			    result =0x84; //init usbd
			    }         
			}else
			{
			    if (result ==0x87) 
			    {
  			 		 gpio_write_io(C_USBHOST_PWR_PIN,1);
		 			 s_usbh_pwr_state =1;
		 			 s_usbh_pwr_wait =1;
		 			 s_usbh_pwr_cnt  =0;
		 			 s_usbh_dpdm =1;
		 			 result =0;	      	    
		    	}
		    }
		    return result;
		}else if(current_cd_pin & 0x80) //if noise
		{
		    current_cd_pin &=0x7F;
		    storage_ststbl[7] = current_cd_pin;
		    s_usbh_pin  = current_cd_pin;
		   	result=0x47;    //noise process
		   	if (s_usbh_pin)
       			   result |= 0x80;
		    else
        		   result =   0;           
        	if (s_usbd_pin)	    
        	{             
			    if (result & 0x80)
			    {
			        s_usbh_usbd_state =1;
        			result =0x04; //remove usbd
			    }
			    else if ( result ==0 )
			    {    
     			    result =0x84; //init usbd
			    }         
			}else
			{
			    if (result & 0x80) 
			    {
  			 		 gpio_write_io(C_USBHOST_PWR_PIN,1);
		 			 s_usbh_pwr_state =1;
		 			 s_usbh_pwr_wait =1;
		 			 s_usbh_pwr_cnt  =0;
		 			 s_usbh_dpdm =1;
 		 			 result =0;	      	    
		    	}
		    }
		    return result;
    	}		    
  }else
  {
     usbh_poll_dp_down();
     if (s_usbh_dpdm==0)
     {   
     	  //uhost remove
	 	  gpio_write_io(C_USBHOST_PWR_PIN,0);
		  s_usbh_pwr_state =0;
          result =  0x07;
		  return result; 

     }       
  }
  
  
#endif

/////////////////////////////////////////////////////////////////////////////////////
#elif(USBH_DETECTION_MODE == USBH_DP_D )
 if(Usb_Dvice_WebCam_Demo_turn_on_flag){
  current_cd_pin = storage_debouce(s_usbmode_check_pin,C_MODE_PIN,2);
  if ((current_cd_pin & 0x0f) != s_usbmode_check_pin)
  {
    current_cd_pin &=0x7F;
    s_usbmode_check_pin  = current_cd_pin;
  }else if(current_cd_pin & 0x80) //if noise
  {
    current_cd_pin &=0x7F;
    s_usbmode_check_pin  = current_cd_pin;
  }
  if(s_usbd_pin)
  {	
  	if(usb_plug_states_flag==0)
  	{
	  	if(s_usbmode_check_pin)
	  	{
	  		OSQPost(USBTaskQ, (void *) MSG_USBD_PLUG_IN);
	  	}
	  	else
	  	{
		  	OSQPost(USBTaskQ, (void *) MSG_USBCAM_PLUG_IN);
  		}
  	}
  	usb_plug_states_flag =1;
  }
  else if(s_usbd_pin ==0)
  {
  	usb_plug_states_flag =0;
  }

}
  //polling USBD
  current_cd_pin = storage_debouce(s_usbd_pin,C_USBDEVICE_PIN,1);
  if ((current_cd_pin & 0x0f) != s_usbd_pin)
  {
    current_cd_pin &=0x7F;
    result=4;
    storage_ststbl[4] = current_cd_pin;
	
#if 0	
	usbd_phy_clock_on();
#endif	
	    
    s_usbd_pin  = current_cd_pin;
    if (current_cd_pin)
         result |= 0x80;
    return result;
  }else if(current_cd_pin & 0x80) //if noise
  {
    current_cd_pin &=0x7F;
    storage_ststbl[4] = current_cd_pin;
    s_usbd_pin  = current_cd_pin;
   	result=0x44;    //noise process
   	if (s_usbd_pin)
       	   result |= 0x80;
    else
           result =   0;
     return result;
  }

///////////////////////////////////////////////////////////////////////////////////// 
#endif

#if USB_NUM >= 1
  if (usbh_mount_status())
         return result;
#if(USBH_DETECTION_MODE == USBH_DP_D )
  if (!USBD_PlugIn && !current_cd_pin)
  {
  	  storage_polling_usbh();
  }else
  {
      s_usbh_dpdm  = 0;
      stor_pin_bouce[3].debouce =0;
  }
#endif

#endif
  return result;
}
// Polling by system timer

static INT8U sw_plugout_entry_flag=0;  /*for SD/MS super plug one time entry flag*/
static INT8U sw_usbh_plugout_entry_flag=0; /*for USBH super plug one time entry flag*/

static void storage_detection_isr(void)
{
  INT16U storage_id;

  if (s_card_work) return;

  storage_id = storage_polling();
  
#if USB_NUM >= 1  
#if(USBH_DETECTION_MODE == USBH_GPIO_D )
 //2 power contol usbh
 if(storage_id == 0x04)           	  
 {
      //GPIO  USB Host  Power ON
      if (!s_usbd_pin)
		    gpio_write_io(C_USBHOST_PWR_PIN,1);
 } 
#endif  
#endif
  if (fs_sd_ms_plug_out_flag_get()==1)
  {
    if (sw_plugout_entry_flag==0)  /* super plug out triggle */
    {
        storage_id = 0x41;   /*force remove sd/ms card*/
        sw_plugout_entry_flag = 1;  /* force declare sd card plug out*/          
    }
    if (gpio_read_io(C_STORAGE_CD_PIN)==1)  /*physical mother card plug out*/
    { 
        storage_id=0;
        DBG_PRINT ("====>SD/MS Mother Body Plug Out End<====\r\n");
        fs_sd_ms_plug_out_flag_reset();
        sw_plugout_entry_flag=0;
    }else
    { 
        fs_sd_ms_plug_out_flag_reset();
        sw_plugout_entry_flag=0;        
    }
  }

#if USB_NUM >= 1  
#if((USBH_DETECTION_MODE == USBH_GPIO_D ) || (USBH_DETECTION_MODE == USBH_GPIO_D_1 ))
 //1 poll dp
 storage_id = usbh_wait_dp_up(storage_id);       
#endif  
#endif

  if (fs_usbh_plug_out_flag_get()==1)
  {
    if (storage_id&0xF == 0x7){     /*void to access usbh detection */   
        storage_id=0;
    }
    if (sw_usbh_plugout_entry_flag==0)  /* super plug out triggle */
    {
        storage_id = 0x47;   /*force remove sd card*/
        sw_usbh_plugout_entry_flag = 1;  /* force declare sd card plug out*/          
    }
    if (!USB_Host_Signal())  /*physical mother card plug out*/
    {
        USBH_SELF_RST();
        DBG_PRINT ("====>USB Mother Body Super PlugOut End<====\r\n");
        fs_usbh_plug_out_flag_reset();
        sw_usbh_plugout_entry_flag = 0;
    }
  }
  if (storage_id !=0)
  {

#if  (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
		switch(storage_id & 0x0f) {
    		case 1:
				sys_stor_msg.storage_id = DRV_FS_DEV_SDMS;
				break;
			case 2:
				sys_stor_msg.storage_id = DRV_FS_DEV_CF;
				break;
			case 4:
				sys_stor_msg.storage_id = DRV_FS_DEV_USBD;
#if ( (BOARD_TYPE == BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_A_V2_0) || (BOARD_TYPE == BOARD_TK_A_V2_1_5KEY))
//by pass for 7A mode
                return;
#endif				
				break;
#if USB_NUM >= 1 //for UHOST PIN				
			case 7:
				sys_stor_msg.storage_id = DRV_FS_DEV_USBH;
				break;				
#endif				
			default:
				break;
		}

		if (storage_id & 0x40 ) {

        	sys_stor_msg.plug_status = DRV_PLGU_OUT;
        	//htest++;  DBG_PRINT ("card out %x \r\n",htest);
      	    stor_msg_QSend(stor_msgQId,stor_msgId,(void*)&sys_stor_msg,sizeof(DRV_PLUG_STATUS_ST),0);
		}
		if (storage_id & 0x80 ) {
        	sys_stor_msg.plug_status = DRV_PLGU_IN;
        	//htest++;  DBG_PRINT ("card in %x \r\n",htest);
        }
        else {
        	sys_stor_msg.plug_status = DRV_PLGU_OUT;
        	//htest++;  DBG_PRINT ("card out %x \r\n",htest);
        }
        if (!(storage_id & 0x40) || (storage_id & 0x80 ))
        {             
	      stor_msg_QSend(stor_msgQId,stor_msgId,(void*)&sys_stor_msg,sizeof(DRV_PLUG_STATUS_ST),0);
	    }
#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
		switch(storage_id & 0x0f) {
    		case 1:
                bin_stg_plug_type_set(BIN_STG_SDMS_PLUG);
				break;
			case 2:
                bin_stg_plug_type_set(BIN_STG_CF_PLUG);
				break;
			case 4:
                bin_stg_plug_type_set(BIN_STG_USBD_PLUG);
				break;
			default:
				break;
		}

		if (storage_id & 0x40 ) {
      	    bin_stg_plug_status_set(BIN_PLUG_OUT);
		}
		if (storage_id & 0x80 ) {
        	bin_stg_plug_status_set(BIN_PLUG_IN);
        }
        else {
        	bin_stg_plug_status_set(BIN_PLUG_OUT);
        }
#endif
   }
}
// Query by upper layer
void storage_sdms_detection(void)
{
   s_card_work =1;
  if (!s_other_cd_pin)
  {
#if XD_EN == 1
	//XDC
     gpio_write_io(C_DATA3_PIN, 0);		
     gpio_write_io(C_DATA1_PIN, 0);
     gpio_write_io(C_DATA2_PIN, 0);
     if (DrvXD_initial()==0)			
   	 	storage_ststbl[6]  = 1;
   	 DrvXD_uninit();
   	 if (storage_ststbl[6]) {
   	    s_card_work =0;
		return;
	}
#else
    storage_ststbl[6]  = 0;         
#endif

#if SD_EN == 1
 if (!storage_ststbl[6])
   {
    //SDC
    gpio_write_io(C_DATA3_PIN, 1);
    gpio_write_io(C_DATA1_PIN, 1);
    gpio_write_io(C_DATA2_PIN, 1);
    if ( drvl2_sd_init()==0)				
	    storage_ststbl[1] = 1;
	drvl2_sd_card_remove();
	if (storage_ststbl[1]) {
 	    s_card_work =0;
		return;
	}
   }
#else
    storage_ststbl[1]  = 0;
#endif  

#if MSC_EN == 1
	if ((!storage_ststbl[6]) && (!storage_ststbl[1]))
	{
	//MS
	gpio_write_io(C_DATA3_PIN, 0);
    gpio_write_io(C_DATA1_PIN, 0);
    gpio_write_io(C_DATA2_PIN, 0);
    if (buffer_ms == NULL){
       	 buffer_ms = (INT16U*) gp_malloc(8192*2);
    }
    ms_table_init(buffer_ms);
    ms_detective_flag_set(1);
    if (ms_initial() ==0)					
	    storage_ststbl[2] = 1;
	ms_detective_flag_set(0);
    gp_free(buffer_ms);
    buffer_ms =NULL;
    ms_uninitial();
    if (storage_ststbl[2]) {
      s_card_work =0;
	  return;
	}	
	}	
#else
   storage_ststbl[2]  = 0;
#endif

  }
  else
  {
	  storage_ststbl[1]  = 0;
	  storage_ststbl[2]  = 0;
	  storage_ststbl[6]  = 0;
  }
    //others......
  s_card_work =0;    
}

void storage_cfc_detection(void)  
{
  
   s_card_work =1;
  if (!s_cf_cd_pin)
  {
#if CFC_EN == 1
    gpio_write_io(C_DATA3_PIN, 0);
    gpio_write_io(C_DATA1_PIN, 0);
    gpio_write_io(C_DATA2_PIN, 0);
    storages_cfc_reset();//power up
    storage_ststbl[0] = !(INT16U) cfc_initial();
    cfc_uninit();
#else
    storage_ststbl[0] =0;
#endif
  }
  else
  {
#if CFC_EN == 1  
   storages_cfc_poweroff();
#endif   
   storage_ststbl[0] =0;
  }
  s_card_work =0;

}


INT16U storage_detection(INT16U type)
{
  return storage_ststbl[type];
}

#if 0
void storage_usbd_state_exit(void)
{
#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
 sys_stor_msg.plug_status = DRV_PLGU_OUT;
 sys_stor_msg.storage_id = DRV_FS_DEV_USBD;
 stor_msg_QSend(stor_msgQId,stor_msgId,(void*)&sys_stor_msg,sizeof(DRV_PLUG_STATUS_ST),0);
 OSTimeDly(50);
#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
 bin_stg_plug_type_set(BIN_STG_USBD_PLUG);
 bin_stg_plug_status_set(BIN_PLUG_OUT);
 OSTimeDly(50);
#endif 
}
#endif

#define STG_CFG_CF      0x0001
#define STG_CFG_SD      0x0002
#define STG_CFG_MS      0x0004
#define STG_CFG_USBH    0x0008
#define STG_CFG_USBD    0x0010
#define STG_CFG_NAND1   0x0020
#define STG_CFG_XD      0x0040
INT16U storage_usbd_detection(void)
{
  INT16U stg_init_val=0;	
  //GPIO F[3] as USB device plugin/out detection PIN (INPUT LOW)
  gpio_init_io(C_USBDEVICE_PIN,GPIO_INPUT);
  gpio_set_port_attribute(C_USBDEVICE_PIN, 0);
  gpio_write_io(C_USBDEVICE_PIN,0);
  sw_timer_us_delay(10);  // wait usb detect pin gpio stable
  //sw
  s_usbd_pin  = gpio_read_io(C_USBDEVICE_PIN);
  storage_ststbl[4] = s_usbd_pin;
  if (s_usbd_pin) 
  {  
    stg_init_val |= STG_CFG_USBD;
#if ( (BOARD_TYPE == BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_A_V2_0) ||(BOARD_TYPE == BOARD_TK_A_V2_1_5KEY))
    stg_init_val &=~STG_CFG_USBD;
#endif
    
    if(NAND1_EN == 1) {stg_init_val |= STG_CFG_NAND1;}
    if(SD_EN == 1) {stg_init_val |= STG_CFG_SD;}
    if(MSC_EN == 1) {stg_init_val |= STG_CFG_MS;}
    //if(USB_NUM >= 1) {stg_init_val |= STG_CFG_USBH;}
	if(CFC_EN == 1) {stg_init_val |= STG_CFG_CF;}
	if(XD_EN == 1) {stg_init_val |= STG_CFG_XD;}
	storages_init(stg_init_val);
  }
  return (INT16U)s_usbd_pin;
}

#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
BIN_STG_PLUG bin_stg_plug_get(void)
{
    BIN_STG_PLUG ret;

    ret = bin_stg_plug_status_get();
    bin_stg_plug_status_set(BIN_PLUG_NONE);
    return ret;
}

BIN_STG_PLUG_TYPE bin_stg_change_type_get(void)
{
    BIN_STG_PLUG_TYPE ret;

    ret = bin_stg_plug_type_get();
    bin_stg_plug_type_set(BIN_STG_NONE_PLUG);
    return ret;
}

void bin_stg_init(void)
{
    INT16U stg_init_val=0;
    
    bin_stg_plug_status_set(BIN_PLUG_NONE);
    bin_stg_plug_type_set(BIN_STG_NONE_PLUG);
    stg_init_val |= STG_CFG_USBD;
#if ( (BOARD_TYPE == BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_A_V2_0) ||(BOARD_TYPE == BOARD_TK_A_V2_1_5KEY))
    stg_init_val &=~STG_CFG_USBD;
#endif
    if((NAND1_EN == 1) || (NOR_EN == 1)) {stg_init_val |= STG_CFG_NAND1;}
    if(SD_EN == 1) {stg_init_val |= STG_CFG_SD;}
    if(MSC_EN == 1) {stg_init_val |= STG_CFG_MS;}
    if(USB_NUM >= 1) {stg_init_val |= STG_CFG_USBH;}
	if(CFC_EN == 1) {stg_init_val |= STG_CFG_CF;}
	if(XD_EN == 1) {stg_init_val |= STG_CFG_XD;}    
	storages_init(stg_init_val);
}
#endif


INT32U storage_wpb_detection(void)
{
 return  gpio_read_io(C_WP_D_PIN) ;
}
#if(USBH_DETECTION_MODE ==USBH_SWITCH_D)
void storage_H_D_switch(INT32U ifH)
{
  if (ifH)
  {
     gpio_write_io(C_USBHOST_PWR_PIN,0);   
     gpio_write_io(C_USBHOST_PIN,1);  
  }
  else
  {
    gpio_write_io(C_USBHOST_PIN,0);
    gpio_write_io(C_USBHOST_PWR_PIN,1);   
  }
}
#endif


INT16U storage_usbh_detection(void)
{
INT16U usbh_cd =0;
#if USB_NUM >= 1
// s_usbh_dpdm = (INT8U) USB_Host_Signal();  
#if( (USBH_DETECTION_MODE == USBH_DP_D ) || ( USBH_DETECTION_MODE == USBH_SWITCH_D) )
  INT16U DP_cnt,i;  
  DP_cnt =0;
  for(i = 0 ; i <1000; i++)
  {
    sw_timer_ms_delay(1); 
   	if ( USB_Host_Signal() )
	       DP_cnt ++;       
    if (DP_cnt >500) break;	 //over 500ms              
  }  
  if (DP_cnt >500)
    usbh_cd =1;
  else  
    usbh_cd =0;      
#else
  usbh_cd = gpio_read_io(C_USBHOST_PIN);  
#endif  
#endif
  return (INT16U)usbh_cd;
}




