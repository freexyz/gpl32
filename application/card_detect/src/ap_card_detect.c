#include "ap_card_detect.h"
/* define */
//#define C_WP_D_PIN       102//IOG[6]	//20090202 roy
#define C_STORAGE_CD_PIN 101//IOG[5], sd, ms, and xd card detect pin
#define C_CFC_RST_PIN     97//IOG[1], cf card reset pin
#define C_CFC_CD_PIN      98//IOG[2], cf card detect pin
#define C_USBDEVICE_PIN   82//IOF[2] = BKCS2, usb device detect pin

#define C_DATA3_PIN       38//IOC[6]
#define C_DATA1_PIN       40//IOC[8]
#define C_DATA2_PIN       41//IOC[9]

/* global function */
void (*cf_card_CallBack)(void);
void (*sdms_card_CallBack)(void);
void (*usb_h_CallBack)(void);
void (*usb_d_CallBack)(void);

/* gplobal varaible */
static INT8U 	card_detect_plug_status; 
static INT8U   	usbh_irq_lock = 0;
static INT16U  	card_detect_status[7];
static INT8U  	other_card_detect_pin;    
static INT8U   	cf_card_detect_pin;       
static INT8U   	usbd_card_detect_pin;       
static INT8U   	usbh_card_detect_pin;
static PIN_BOUCE  card_detect_pin_bouce[4];    

/* extern varaible */
extern INT16U *buffer_ms; 

void card_detect_set_cf_callback(void (*cf_callback)(void))
{
	cf_card_CallBack = cf_callback;
}

void card_detect_set_sdms_callback(void (*sdms_callback)(void))
{
	sdms_card_CallBack = sdms_callback;
}

void card_detect_set_usb_h_callback(void (*usb_h_callback)(void))
{
	usb_h_CallBack = usb_h_callback;
}

void card_detect_set_usb_d_callback(void (*usb_d_callback)(void))
{
	usb_d_CallBack = usb_d_callback;
}

INT8U card_detect_get_plug_status(void)
{
	return card_detect_plug_status;
}

INT8U card_detect_debouce(INT8U pin,INT32U ioport,INT16U  pin_id)
{
    INT16U  current_cd_pin;
    current_cd_pin = gpio_read_io(ioport);
	if (current_cd_pin != pin)             //
  	{
       card_detect_pin_bouce[pin_id].debouce++;
	}
	else
	{
	   if (card_detect_pin_bouce[pin_id].debouce >=8) //if noise happen  // 62ms
	   {
	      card_detect_pin_bouce[pin_id].noise_flag   =1; //noise event!!
	      card_detect_pin_bouce[pin_id].noise_debouce =0;
	   }
       card_detect_pin_bouce[pin_id].debouce =0;
	}
	if (card_detect_pin_bouce[pin_id].noise_flag)  card_detect_pin_bouce[pin_id].noise_debouce++;

    if (card_detect_pin_bouce[pin_id].debouce ==64) //64 x (1/128)  = 500ms
    {
       if (card_detect_pin_bouce[pin_id].noise_flag) //clear noise event!!
       {
          card_detect_pin_bouce[pin_id].noise_flag   =0;
      	  card_detect_pin_bouce[pin_id].noise_debouce=0;
       }
       card_detect_pin_bouce[pin_id].debouce =0;
       return current_cd_pin;
    }
    if ( card_detect_pin_bouce[pin_id].noise_debouce > 102) //800ms
    {
    	card_detect_pin_bouce[pin_id].noise_flag    =0 ;
     	card_detect_pin_bouce[pin_id].noise_debouce =0 ;
     	current_cd_pin |=0x80 ; //bit 7 as nois bit
      	return current_cd_pin;
    }
    return pin;
}

void card_detect_usb_h_d_isr(void)
{
	if (USBD_PlugIn)
   	{
		usb_isr();
	 	USBH_INT_CLR_IMMEDIATE();
   	}
   	else
   	{
		if (usbh_irq_lock)
	     	USBH_INT_CLR_IMMEDIATE();
	   	else
			usbh_isr();
   	}
}

#if CFC_EN == 1
void card_detect_cfc_reset(void)
{
    gpio_write_io(C_DATA3_PIN, 0);
    gpio_write_io(C_DATA1_PIN, 0);
    gpio_write_io(C_DATA2_PIN, 0);
    gpio_write_io(C_CFC_RST_PIN, 1);
    
    sw_timer_us_delay(200000);
    gpio_write_io(C_CFC_RST_PIN, 0);
    sw_timer_us_delay(200000);
}
#endif

///////////////////////////////////////////////////////////////////////
//devicetype :bit   0   1   2    3      4        5       6
//            type  CF SDC  MS  UHOST  USBD  Nandflash   XD
//////////////////////////////////////////////////////////////////////
void card_detect_init(INT16U devicetpye)
{
	INT16U i;

   	//usbh_detection_init();
  	
  	//GPIO G[5] as CD PIN
  	gpio_init_io(C_STORAGE_CD_PIN,GPIO_INPUT);
  	gpio_set_port_attribute(C_STORAGE_CD_PIN, 1);
  	
  	//GPIO G[2] as CD PIN
  	gpio_init_io(C_CFC_CD_PIN,GPIO_INPUT);
  	gpio_set_port_attribute(C_CFC_CD_PIN, 1);

  	//GPIO F[2] as USB device plugin/out detection PIN (INPUT LOW)
  	gpio_set_sdramio(); //set unused io of sdram
  	gpio_set_memcs(2,0);//cs2 as usb device cd pin
  	gpio_init_io(C_USBDEVICE_PIN,GPIO_INPUT);
  	gpio_set_port_attribute(C_USBDEVICE_PIN, 0);
  	gpio_write_io(C_USBDEVICE_PIN,0);
  
  	//GPIO G[6] as WPB PIN
  	//gpio_init_io(C_WP_D_PIN,GPIO_INPUT);		//20090202 roy
  	//gpio_set_port_attribute(C_WP_D_PIN, 1);	//20090202 roy
  
  	//GPIO G[2] as CFC rest pin
  	gpio_init_io(C_CFC_RST_PIN,GPIO_OUTPUT);
  	gpio_set_port_attribute(C_CFC_RST_PIN, 1);
  	gpio_write_io(C_CFC_RST_PIN, 0);
  
  	gpio_init_io(C_DATA3_PIN,GPIO_OUTPUT);
 	gpio_set_port_attribute(C_DATA3_PIN, 1);
  	gpio_init_io(C_DATA1_PIN,GPIO_OUTPUT);
  	gpio_set_port_attribute(C_DATA1_PIN, 1);
  	gpio_init_io(C_DATA2_PIN,GPIO_OUTPUT);
  	gpio_set_port_attribute(C_DATA2_PIN, 1);

  	sw_timer_us_delay(15000); /* wait port stable */
  	other_card_detect_pin = gpio_read_io(C_STORAGE_CD_PIN);
  	cf_card_detect_pin = gpio_read_io(C_CFC_CD_PIN);
  	usbd_card_detect_pin = gpio_read_io(C_USBDEVICE_PIN);
  	usbh_card_detect_pin = (INT8U) USB_Host_Signal();

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
	if (buffer_ms != NULL)
	{
		gp_free(buffer_ms);
		buffer_ms =NULL;
	}
#endif
	
	for(i = 0;i < 7; i++)
		card_detect_status[i] =0;

#if CFC_EN == 1             
	if( (devicetpye &0x01) && (!cf_card_detect_pin) )
	{
		card_detect_cfc_reset(); 
		card_detect_status[0] =!(INT16U) cfc_initial();
		cfc_uninit();
	}
#endif

	if (!other_card_detect_pin)
	{
#if SD_EN == 1
		if (devicetpye &0x02)
		{
			gpio_write_io(C_DATA3_PIN, 1);
			gpio_write_io(C_DATA1_PIN, 1);
			gpio_write_io(C_DATA2_PIN, 1);
			card_detect_status[1] = !(INT16U) drvl2_sd_init();
			drvl2_sd_card_remove();
		}
#endif
#if MSC_EN == 1
		if (devicetpye &0x04)
		{
			if (!card_detect_status[1])
			{
				gpio_write_io(C_DATA3_PIN, 0);
				gpio_write_io(C_DATA1_PIN, 0);
				gpio_write_io(C_DATA2_PIN, 0);
				buffer_ms = (INT16U*) gp_malloc(8192*2);
				if (buffer_ms != NULL)
				{
		 			ms_table_init(buffer_ms);
		 			card_detect_status[2] = !ms_initial();
				}
				ms_uninitial();
				gp_free(buffer_ms);
				buffer_ms =NULL;
			}
		}
#endif
#if XD_EN == 1
		if (devicetpye &0x40)
		{
			if ((!card_detect_status[1]) && (!card_detect_status[2]))
			{
				card_detect_status[6]  = DrvXD_initial(); 	
				DrvXD_uninit();
			}
		}       
#endif  
	}

#if USB_NUM >= 1
	if ((devicetpye &0x08) && (usbh_card_detect_pin) )
	{
		usbh_irq_lock =1;
		card_detect_status[3] = !DrvUSBHMSDCInitial();
		usbh_card_detect_pin =  card_detect_status[3];
	}
#endif
	if (devicetpye &0x10)
	{
		card_detect_status[4] = usbd_card_detect_pin;
	}

#if (NAND1_EN == 1)
	if (devicetpye &0x20)
	{
		card_detect_status[5] = !(INT16U)NAND_Initial();
		NAND_Uninitial();
	}
#endif

	//register interrupter_isq and timer_isr
	vic_irq_register(VIC_USB, card_detect_usb_h_d_isr);
	vic_irq_enable(VIC_USB);
	card_detect_polling_start();
	usbh_irq_lock =0;

	//0:SD 1:USBD 2:CF 3:USBH
	for(i = 0 ; i <4; i++)
	{
		card_detect_pin_bouce[i].debouce = 0;
		card_detect_pin_bouce[i].noise_debouce = 0;
		card_detect_pin_bouce[i].noise_flag = 0;
	}
}

#if (USB_NUM >= 1)
void card_detect_polling_usbh(void)
{
   	INT16U  current_cd_pin;
    current_cd_pin =(INT16U) USB_Host_Signal();
  	if (current_cd_pin != usbh_card_detect_pin) // IO detection & debouce 10ms
  	{
       card_detect_pin_bouce[3].debouce++;
	}
	else
	{
	   if (card_detect_pin_bouce[3].debouce >=20) //if noise happen  // 62ms
	   {
	      card_detect_pin_bouce[3].noise_flag   =1; //noise event!!
	      card_detect_pin_bouce[3].noise_debouce =0;
	   }
       card_detect_pin_bouce[3].debouce =0;
	}
	if (card_detect_pin_bouce[3].noise_flag)
	{
 	    USB_Host_Handup(current_cd_pin);
		card_detect_pin_bouce[3].noise_debouce++;
	}
    if (card_detect_pin_bouce[3].debouce ==64) //64 x (1/128)  = 500ms
    {
       if (card_detect_pin_bouce[3].noise_flag) //clear noise event!!
       {
          card_detect_pin_bouce[3].noise_flag   =0;
      	  card_detect_pin_bouce[3].noise_debouce=0;
       }
       card_detect_pin_bouce[3].debouce =0;
       usbh_card_detect_pin =  current_cd_pin ;
	   card_detect_status[3] = current_cd_pin;
       usbh_card_detect_pin  = current_cd_pin;
       USB_Host_Handup(usbh_card_detect_pin);

       if (current_cd_pin)
    	 card_detect_plug_status = DRV_PLGU_IN;
       else
	      card_detect_plug_status = DRV_PLGU_OUT;
      
       if(usb_h_CallBack != NULL)	
		  (*usb_h_CallBack)();
   }

   if (card_detect_pin_bouce[3].noise_debouce == 100) //800ms
   {
      card_detect_pin_bouce[3].noise_flag    =0 ;
      card_detect_pin_bouce[3].noise_debouce =0 ;
      if (current_cd_pin)
      {
      		if(usb_h_CallBack != NULL)	
      		{
      			card_detect_plug_status = DRV_PLGU_OUT;
				(*usb_h_CallBack)();
      			
      			card_detect_plug_status = DRV_PLGU_IN;
      			(*usb_h_CallBack)();
      		}
      }
   }
}
#endif

void card_detect_polling_start(void)
{
	sys_registe_timer_isr(card_detection_isr);
}

INT16U card_detect_polling(void)
{
	INT8U  current_cd_pin;
	INT16U result = 0;
	
	//polling SDC	
	current_cd_pin = card_detect_debouce(other_card_detect_pin, C_STORAGE_CD_PIN, 0);
	if((current_cd_pin & 0x0f) != other_card_detect_pin)
	{
		current_cd_pin &=0x7F;
		result=1;
		other_card_detect_pin = current_cd_pin;
		if (!current_cd_pin)
			result |= 0x80;
#if MSC_EN == 1       	   
		ms_hand_set(!current_cd_pin);
#endif           	   
		return result;
	}
	else if(current_cd_pin & 0x80) //if noise
	{
		current_cd_pin &=0x7F;
		other_card_detect_pin  = current_cd_pin;
		result=0x41;   //noise process
		if (!other_card_detect_pin)
			result |= 0x80;   //IN
		else
			result = 0;
		
		return result;
	}
#if MSC_EN == 1       	
	if (card_detect_pin_bouce[0].noise_flag)
		ms_hand_set(!current_cd_pin);
#endif

	//polling CF
#if CFC_EN == 1
	current_cd_pin = card_detect_debouce(cf_card_detect_pin, C_CFC_CD_PIN, 2);
	if ((current_cd_pin & 0x0f) != cf_card_detect_pin)
	{
		current_cd_pin &=0x7F;
		result=2;
		cf_card_detect_pin  = current_cd_pin;
		if (!current_cd_pin)
			result |= 0x80;
		
		return result;
	}
	else if(current_cd_pin & 0x80) //if noise
	{
		current_cd_pin &=0x7F;
		cf_card_detect_pin  = current_cd_pin;
		result=0x42;    //noise process
		if (!cf_card_detect_pin)
			result |= 0x80;
		else
			result = 0;
		
		return result;
	}
#endif
	//polling USBD
	current_cd_pin = card_detect_debouce(usbd_card_detect_pin,C_USBDEVICE_PIN,1);
	if ((current_cd_pin & 0x0f) != usbd_card_detect_pin)
	{
		current_cd_pin &=0x7F;
		result=4;
		card_detect_status[4] = current_cd_pin;
		usbd_card_detect_pin = current_cd_pin;
		if (current_cd_pin)
			result |= 0x80;
		
		return result;
	}
	else if(current_cd_pin & 0x80) //if noise
	{
		current_cd_pin &=0x7F;
		card_detect_status[4] = current_cd_pin;
		usbd_card_detect_pin  = current_cd_pin;
		result=0x44;    //noise process
		if (usbd_card_detect_pin)
			result |= 0x80;
		else
			result = 0;
		
		return result;
	}

#if USB_NUM >= 1
	if (usbh_mount_status())
		return result;
	
	if (!usbd_card_detect_pin)
	{
		card_detect_polling_usbh();
	}
#endif

	return result;
}

void card_detection_isr(void)
{
	INT16U storage_id;

	storage_id = card_detect_polling();
	if (storage_id !=0)
	{
		if(storage_id & 0x80)
			card_detect_plug_status = DRV_PLGU_IN;
		else 
			card_detect_plug_status = DRV_PLGU_OUT;

		if(storage_id & 0x01) //sd,ms,xd
		{
			if(sdms_card_CallBack != NULL)	
				(*sdms_card_CallBack)();
		}
		else if(storage_id & 0x02)//cf
		{
			if(cf_card_CallBack != NULL)		
				(*cf_card_CallBack)();
		}
		else if(storage_id & 0x04) //usb d
		{
			if(usb_d_CallBack != NULL)	
				(*usb_d_CallBack)();
		}
		else if(storage_id & 0x08)// usb h
		{
			if(usb_h_CallBack != NULL)	
				(*usb_h_CallBack)();
		}
	}
}

void sdms_detection(void)
{
  if (!other_card_detect_pin)
  {
#if SD_EN == 1
    //SDC
    gpio_write_io(C_DATA3_PIN, 1);
    gpio_write_io(C_DATA1_PIN, 1);
    gpio_write_io(C_DATA2_PIN, 1);
    card_detect_status[1] = !(INT16U) drvl2_sd_init();
	drvl2_sd_card_remove();
	if (card_detect_status[1]) {
		return;
	}
#else
    card_detect_status[1]  = 0;
#endif
#if MSC_EN == 1
	//MS
	gpio_write_io(C_DATA3_PIN, 0);
    gpio_write_io(C_DATA1_PIN, 0);
    gpio_write_io(C_DATA2_PIN, 0);
    if (buffer_ms == NULL){
       	 buffer_ms = (INT16U*) gp_malloc(8192*2);
    }
    ms_table_init(buffer_ms);
    card_detect_status[2] = !ms_initial();
    gp_free(buffer_ms);
    buffer_ms =NULL;
    ms_uninitial();
    if (card_detect_status[2]) {
	  return;
	}	
#else
   card_detect_status[2]  = 0;
#endif

#if XD_EN == 1
   if ((!card_detect_status[1]) && (!card_detect_status[2]))
   {
   	 	card_detect_status[6]  = DrvXD_initial(); 	
   	 	DrvXD_uninit();
   }
#else
   card_detect_status[6]  = 0;         
#endif      
  }
  else
  {
	  card_detect_status[1]  = 0;
	  card_detect_status[2]  = 0;
	  card_detect_status[6]  = 0;
  }
    //others......
}

void cfc_detection(void)
{
  if (!cf_card_detect_pin)
  {
#if CFC_EN == 1
    gpio_write_io(C_DATA3_PIN, 0);
    gpio_write_io(C_DATA1_PIN, 0);
    gpio_write_io(C_DATA2_PIN, 0);
    card_detect_cfc_reset();
    card_detect_status[0] = !(INT16U) cfc_initial();
    cfc_uninit();
#else
    card_detect_status[0] = 0;
#endif
  }
  else
   	card_detect_status[0] = 0;
}
/////////////////////////////////////////////////////////////////////////
//devicetype :typ = 0   1   2    3      4        5      6 
//                  CF SDC  MS  UHOST  USBD  Nandflash  XD
////////////////////////////////////////////////////////////////////////
INT16U card_detection(INT16U type)
{
  	return card_detect_status[type];
}
