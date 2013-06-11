
/* pseudo header include */
#include "state_usb.h"

static void state_usb_init(void);
static void state_usb_resume(void);
static void state_usb_exit(void);
#if _PROJ_TYPE == _PROJ_TURNKEY
static void state_usb_suspend(EXIT_FLAG_ENUM exit_flag);
#endif
extern void usb_msdc_state_exit(void);
extern void usb_msdc_state(void);

void state_usb_init(void)
{
#if SUPPORT_CHARGE_MODULE == CUSTOM_ON
	charge_module_show();
#endif	
#if _PROJ_TYPE == _PROJ_TURNKEY		
    sys_usbd_state_flag_set(C_USBD_STATE_IN);
#endif	
	usb_msdc_state_initial();

}

void state_usb_resume(void)
{  
#if _PROJ_TYPE == _PROJ_TURNKEY 
	sys_usbd_state_flag_set(C_USBD_STATE_IN);
#endif
}

void state_usb_entry(void* para1)
{
    STATE_ENTRY_PARA *state_para;
    EXIT_FLAG_ENUM  exit_flag ;
#if _PROJ_TYPE == _PROJ_TURNKEY    
    INT32U          msg_id;
    INT8U stg_plugout_retry=0;
#endif
    state_para = (STATE_ENTRY_PARA*) para1;
   
    exit_flag = EXIT_RESUME;
	if (state_para->state_entry_mode != STATE_RESUME_MODE) {    
    state_usb_init();
	DBG_PRINT("USBD state Entry\r\n");
	} else {
        state_usb_resume();
    DBG_PRINT("USBD state Resume\r\n");
	}
#if _PROJ_TYPE == _PROJ_TURNKEY 	
	audio_bg_send_stop();
	while(1)
	{
    /*Program the Q pending switch case*/
		if(msgQReceive(ApQ, &msg_id, ApQ_para, AP_QUEUE_MSG_MAX_LEN)==STATUS_FAIL) {continue;}	
		if (msg_id>EVENT_KEY_BASE && msg_id<=EVENT_KEY_MAX) {
			continue;
		}
		#if _PROJ_TYPE == _PROJ_TURNKEY 
		if (umi_osd_menu_process_message(&msg_id, (KEY_EVENT_ST *) ApQ_para)) {
			continue;
		}
		#endif
		//DBG_PRINT("usb ApQ <= %x\r\n",msg_id);
		switch(msg_id)
		{
        	case EVENT_USBD_ISR_MSG:
           		//msgQFlush(ApQ);
                usb_std_service(0);
	            usb_msdc_service(0);
    	        break;

     		case EVENT_STORAGE_CHANGE:
     		#if _PROJ_TYPE == _PROJ_TURNKEY 
                pop_up_stg_msg_mount_sts_set();
            #endif    
                DBG_PRINT(">>>======>EVENT_USB_PC_OUT \r\n");
		      	    //DBG_PRINT("Plug\r\n");
 	      	    usbd_check_lun((DRV_PLUG_STATUS_ST *)ApQ_para);
    	        break;
           case EVENT_USB_PC_OUT:    	        
#if NAND1_EN == 1
              	if (storage_detection(5)) DrvNand_flush_allblk();
#endif              
                   //state_usb_exit();  /* dominant mark */
      	        exit_flag = EXIT_BREAK;           
      	        break;
           case EVENT_KEY_BACK:                            
	            exit_flag = EXIT_BREAK;
    	        break;
       	   case EVENT_APQ_ERR_MSG: //continue processing Audio
			    audio_confirm_handler((STAudioConfirm *) ApQ_para);
				break;  
    	   case EVENT_KEY_MEDIA_PLAY:
			    break;
		   case EVENT_KEY_PETS:
		   #if SETUP_SUPPORT_AUTO_PET==CUSTOM_ON
				ap_auto_pet_init(umi_sleep_time_index_get());//auto pet set 
		   #endif    
			    break;   
           case EVENT_USB_PC_IN:
               DBG_PRINT(">>>======>EVENT_USB_PC_IN DDetect\r\n");
               //break;       
    	   default: /* default procedure */
        		exit_flag = apq_sys_msg_handler(msg_id);
        		//Check itself if PC is remove (EXCEPT PROCESS)
            	break;        
		}
		#if 1      
            if (!storage_detection(4))
            {   
                stg_plugout_retry++;
                OSTimeDly(10);
                DBG_PRINT(">>>======>storage_detection() retry times:%d\r\n",stg_plugout_retry);
                if (stg_plugout_retry>7)
                {
                    exit_flag = EXIT_BREAK;
                    DBG_PRINT("USBD PLUG OUT times out:%d, BREAK!!!!\r\n",stg_plugout_retry);
                }
                storage_usbd_detection();
            }
        #endif  
        #if 1
         //Check other PC plug in and out message
            if  (exit_flag == EXIT_BREAK)
            {
    		  if (msgQSizeGet(ApQ)!=0) {
      			while(1)
      			{
	   			  msgQReceive(ApQ, &msg_id, ApQ_para, AP_QUEUE_MSG_MAX_LEN);
   			  	  //DBG_PRINT("usbd state %x\r\n",msg_id);
    		  	  //if ((msg_id==EVENT_USB_PC_IN ) || (msg_id==EVENT_STORAGE_CHANGE))
    	  		  if (msg_id==EVENT_USB_PC_IN ) 
	    	  	  {
                        exit_flag = EXIT_RESUME;
          		  }
	      	  	  if (msg_id==EVENT_USB_PC_OUT ) 
    	  	  	  {
      	  	            exit_flag = EXIT_BREAK;
      		  	  }
            	  if (msgQSizeGet(ApQ)==0) {break;}
	         	}
    		   }        
              }
         #endif     
	     if (exit_flag != EXIT_RESUME)  {
    	    break;
		 }
  	}
    
	if ((exit_flag == EXIT_BREAK) || (exit_flag == EXIT_TO_SHORTCUT)) {
        state_usb_exit();  	    	
		} else if (exit_flag == EXIT_SUSPEND) {
       state_usb_suspend(exit_flag);
		}
#else
	usb_msdc_state();
  	state_usb_exit();  
#endif   
}

#define STG_CFG_CF      0x0001
#define STG_CFG_SD      0x0002
#define STG_CFG_MS      0x0004
#define STG_CFG_USBH    0x0008
#define STG_CFG_USBD    0x0010
#define STG_CFG_NAND1   0x0020
#define STG_CFG_XD      0x0040
void state_usb_exit(void)
{
   INT16U stg_init_val=0;

   DBG_PRINT("USBD state Exit\r\n");  
   /* allow umi to access statck state */
#if _PROJ_TYPE == _PROJ_TURNKEY
    sys_usbd_state_flag_set(C_USBD_STATE_OUT);
    suspend_status_finished();
#endif
   
#if SUPPORT_CHARGE_MODULE == CUSTOM_ON
	battery_exit();
#endif
    usb_msdc_state_exit();
   //reinitial cards and NF
#if NAND1_EN == 1
 	if (storage_detection(5)) DrvNand_flush_allblk();
#endif

#if NOR_EN == 1
 	 spi_flash_disk_flush();
#endif

  if((NAND1_EN == 1) || (NOR_EN == 1)) {stg_init_val |= STG_CFG_NAND1;}
  if(SD_EN == 1) {stg_init_val |= STG_CFG_SD;}
  if(MSC_EN == 1) {stg_init_val |= STG_CFG_MS;}
  if(CFC_EN == 1) {stg_init_val |= STG_CFG_CF;}
  if(XD_EN == 1) {stg_init_val |= STG_CFG_XD;}
#if _PROJ_TYPE == _PROJ_TURNKEY	 
//Remount usbh at USBH_SWITCH_D mode
#if (USBH_DETECTION_MODE == USBH_SWITCH_D )  
  if(USB_NUM >= 1) {stg_init_val |= STG_CFG_USBH;}  
#endif
#endif  
  // storages_init(0x27);
  if(Usb_Dvice_WebCam_Demo_turn_on_flag ==0){ //modify by crf
   	storages_init(stg_init_val);
  }

#if _PROJ_TYPE == _PROJ_TURNKEY      
   storage_renumeration();
#endif
  // usb_msdc_state_exit();   
}

#if _PROJ_TYPE == _PROJ_TURNKEY
void state_usb_suspend(EXIT_FLAG_ENUM exit_flag)
{
    DBG_PRINT("USBD state Suspend\r\n");
	
	sys_usbd_state_flag_set(C_USBD_STATE_OUT);
	/* store reume need parameter to state globle variable */

	/* store resume need parameter to NVRAM (option) */

	/* push state entry function address to state stack for exception_handle_state */
    suspend_state_stack_push(umi_current_state_get());    
}
#endif
