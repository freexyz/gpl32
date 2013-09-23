#include "system_dynamicallyclock.h"
#include "driver_l2.h"

#define status_ok                 0
//define module status
#define UART_CTRL_BUSY            0x8
#define USB_DMA_CTRL_BUSY         0x1
#define USB_CTRL_BUSY             0x0100
#define ADC_CTRL_BUSY             0x80
#define NAND_CTRL_BUSY            0x8000
#define NAND_DMA_CTRL_BUSY        0x1
#define DEFLICKER_CTRL_BUSY       0x100
//define sdram resister shift bit
#define sdram_vaild_4             0xf
#define sdram_vaild_2             0x3
#define sdram_trcd_sel            10
#define sdram_trp_sel             8
#define sdram_tras_sel            4
//define nand resister shift bit
#define nfctrlreg_sel             0xff
#define nand_vaild_3              0x7
#define nand_vaild_2              0x3
#define nand_trp_sel              4
#define nand_th_sel               2
//define Divider check
#define Remainder_CK_DISABLE      0
#define Remainder_CK_ENABLE       1
//Dynamically Clock
INT32S system_set_clock_dynamically(CLK_ENUM target_clk);
//Module status polling
void module_pollingstatus(void);
void polling_sd(void);
void polling_scaler(void);
void polling_jpeg(void);
void polling_dma(void);
void polling_nand(void);
void polling_dac(void);
void polling_spu(void);
void polling_usb(void);
void polling_uart(void);
void polling_adc(void);
void polling_deflicker(void);
INT32S nand_status_get(void);
INT32S uart_status_get(void);
INT32S adc_status_get(void);
INT32S usb_status_get(void);
INT32S deflicker_status_get(void);
//Module initiation
void system_module_init(void);
//Extern API
extern INT32S drvl1_sdc_status_set_wait(INT32U pollbit, INT32U timeout);
//define SDC module
SD_CARD_INFO_STRUCT sd_info;
extern const INT8U speed_table[];

#ifndef __CS_COMPILER__
#pragma arm section rwdata="pwr_ctrl", code="pwr_ctrl"
INT32S system_sdram_MHZ_set(CLK_ENUM system_clk);
#else
INT32S system_set_clock_dynamically(CLK_ENUM target_clk)__attribute__ ((section(".ap_entry")));
INT32S system_sdram_MHZ_set(CLK_ENUM system_clk)__attribute__ ((section(".ap_entry")));
typedef INT32S (*system_dynamically_set_clock)(CLK_ENUM target_clk);
system_dynamically_set_clock gpl32_set_system_clock;
#endif
INT32S system_sdram_timing_set(CLK_ENUM system_clk,INT16U tRC,INT16U tRAS,INT16U tRP,INT16U tRCD);
INT32S system_nand_timing_set(CLK_ENUM system_clk,INT16U tRP,INT16U tH,INT16U tWP);
INT32U system_hardware_Divider_set(INT32U Dividend,INT32U Divisor,INT32U Remainder_CK);
INT32S (*pfunSysClkSet)(INT32U PLL);

INT32S system_set_clock_dynamically(CLK_ENUM target_clk)
{
	INT32U current_clk,i;
  	INT32S status;

  	status=0;
	current_clk = (R_SYSTEM_PLLEN & 0x3F) * 3;
	R_INT_GMASK=0x1;
	pfunSysClkSet = sys_clk_set;
	switch(target_clk)
	{
		case CLK_48MHZ:
			if(current_clk > CLK_48MHZ)
			{	// speed up -> slow down
				system_sdram_MHZ_set(CLK_48MHZ);
				for(i=0;i<10;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
			    pfunSysClkSet(target_clk*1000000);
				for(i=0;i<10;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }			   
			}
			else if(current_clk < CLK_48MHZ)
			{	// slow down -> speed up
				for(i=0;i<10;i++)
				{
					ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				pfunSysClkSet(target_clk*1000000);
				for(i=0;i<5;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				system_sdram_MHZ_set(CLK_48MHZ);
			}   
			R_SYSTEM_CLK_CTRL &= ~(0x0008);        //0 = Current clock is not 96 MHz(USB 48MHz is from fast PLL output)   
			break;
/*
		case CLK_63MHZ:
			if(current_clk > CLK_63MHZ)
			{	// speed up -> slow down
				system_sdram_MHZ_set(CLK_63MHZ);
				for(i=0;i<3;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				pfunSysClkSet(target_clk*1000000);
				for(i=0;i<3;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }			   
			}
			else if(current_clk < CLK_63MHZ)
			{	// slow down -> speed up				
				for(i=0;i<3;i++)
				{
					ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				pfunSysClkSet(target_clk*1000000);
				for(i=0;i<3;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				system_sdram_MHZ_set(CLK_63MHZ);
			}		
			break;
		case CLK_78MHZ:
			if(current_clk > CLK_78MHZ)
			{	// speed up -> slow down
				system_sdram_MHZ_set(CLK_78MHZ);
				for(i=0;i<3;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				pfunSysClkSet(target_clk*1000000);
				for(i=0;i<3;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }			    
			}
			else if(current_clk < CLK_78MHZ)
			{	// slow down -> speed up				
				for(i=0;i<3;i++)
				{
					ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				pfunSysClkSet(target_clk*1000000);
				for(i=0;i<3;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }
				system_sdram_MHZ_set(CLK_78MHZ);
			}
			R_SYSTEM_CLK_CTRL |= 0x0008;           //1 = Current clock is 78MHz(SPU 39MHz is from PLL output/2)  		
			break;
*/
		case CLK_96MHZ:
			if(current_clk < CLK_96MHZ)
			{	// slow down -> speed up				
				for(i=0;i<10;i++)
				{
					ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }			    
				pfunSysClkSet(target_clk*1000000);
				for(i=0;i<10;i++)
				{
				    ASM(NOP);
				    ASM(NOP);
				    ASM(NOP);
			    }			    
				system_sdram_MHZ_set(CLK_96MHZ);
			}
			R_SYSTEM_CLK_CTRL |= 0x0008;           //1 = Current clock is 96MHz(USB 48MHz is from PLL output/2)                        		
			break;
		default:
			status=-1;
			break;	
	}
	R_INT_GMASK=0x0;
	return status;		// return success
}
INT32S system_sdram_MHZ_set(CLK_ENUM system_clk)
{
    #define	oc	0x1		/* Internal test for overing clock */
    
    R_MEM_SDRAM_CBRCYC = ((Common_Refresh_Period*system_clk/1000) & 0xFFFF) * oc;
    return STATUS_OK;	
}
#ifndef __CS_COMPILER__
#pragma arm section rwdata, code
#endif

INT32S system_sdram_timing_set(CLK_ENUM system_clk,INT16U tRC,INT16U tRAS,INT16U tRP,INT16U tRCD)
{
    INT32U TRCD,TRP,TRAS,TRC,command_ns;
    
	if((tRC==NULL)||(tRAS==NULL)||(tRP==NULL)||(tRCD==NULL)||(system_clk==NULL))
		return -1;	
	command_ns=system_hardware_Divider_set(1000,system_clk,Remainder_CK_DISABLE);
	TRC=system_hardware_Divider_set(tRC,command_ns,Remainder_CK_ENABLE);
	TRAS=system_hardware_Divider_set(tRAS,command_ns,Remainder_CK_ENABLE);
	TRP=system_hardware_Divider_set(tRP,command_ns,Remainder_CK_ENABLE);
	TRCD=system_hardware_Divider_set(tRCD,command_ns,Remainder_CK_ENABLE);
	R_MEM_SDRAM_TIMING = (((TRCD&sdram_vaild_2)<<sdram_trcd_sel)|((TRP&sdram_vaild_2)<<sdram_trp_sel)|((TRAS&sdram_vaild_4)<<sdram_tras_sel)|(TRC&sdram_vaild_4));
    return STATUS_OK;		
}

INT32U system_hardware_Divider_set(INT32U Dividend,INT32U Divisor,INT32U Remainder_CK)
{
    INT32U i,temp;
	if(R_GTE_DIVOF!=0x3)
       R_GTE_DIVOF=0x00000003;
	if((Dividend==NULL)||(Divisor==NULL))
	   return 0xFFFFFFFF;
    R_GTE_DIVA=Dividend;
	R_GTE_DIVB=Divisor;
	for (i=0; i<5; i++)
		temp++;
	temp=0;
	if(Remainder_CK==Remainder_CK_DISABLE)
		temp=R_GTE_DIVO;
	else
	{
		temp=R_GTE_DIVR;
		if(temp==0)
			temp=R_GTE_DIVO;
		else
		{
			temp=R_GTE_DIVO;
			temp+=1;
		}
	}	    
	return temp;	
}


INT32S system_nand_timing_set(CLK_ENUM system_clk,INT16U tRP,INT16U tH,INT16U tWP)
{
	INT32U TRP,TH,TWP,command_ns;
    
	if((tRP==NULL)||(tH==NULL)||(tWP==0)||(system_clk==NULL))
	   return -1;
	command_ns=system_hardware_Divider_set(1000,system_clk,Remainder_CK_DISABLE);
	TRP=system_hardware_Divider_set(tRP,command_ns,Remainder_CK_ENABLE);
	TH=system_hardware_Divider_set(tH,command_ns,Remainder_CK_DISABLE);
	if(TH > 0)
	   TH=3;	
	TWP=system_hardware_Divider_set(tWP,command_ns,Remainder_CK_ENABLE);
	R_NF_CTRL &= ~nfctrlreg_sel;
	R_NF_CTRL |= (((TRP&nand_vaild_3)<<nand_trp_sel)|((TH&nand_vaild_2)<<nand_th_sel)|(TWP&nand_vaild_2));
    return STATUS_OK;		
}

INT32S sdc_set_clock(INT32U speed)
{
	INT32U tran_unit, time_value;
	INT32U max_speed;
	
	drvl2_sdc_card_info_get(&sd_info);
	time_value = (sd_info.csd[0] & 0x00000078) >> 3;
	tran_unit = sd_info.csd[0] & 0x00000007;
	if (time_value>0 && time_value<=15) {
		max_speed = (INT32U) (speed_table[time_value-1]);
		if (tran_unit == 0) {
			max_speed = max_speed * 1000000;
		} else if (tran_unit == 1) {
			max_speed = max_speed * 100000;
		} else if (tran_unit == 2) {
			max_speed = max_speed * 1000000;
		} else if (tran_unit == 3) {
			max_speed = max_speed * 10000000;
		} else {
			return -1;
		}
	} else {
		return -1;
	}

	if(speed > max_speed){
	   if (drvl1_sdc_clock_set(0, max_speed)) {
		  return -1;
	   }
	}else{
	    if (drvl1_sdc_clock_set(0, speed)) {
		   return -1;
		}
	}

	return status_ok;		// return success
}

INT32S system_clock_set(CLK_ENUM target_clk)
{
	   INT32S status;
	   
	   module_pollingstatus();
	   #ifndef __CS_COMPILER__
	      status=system_set_clock_dynamically(target_clk);
	   #else
	      gpl32_set_system_clock= (INT32U)system_set_clock_dynamically;
	      status = gpl32_set_system_clock(target_clk);
	   #endif
	   system_module_init();
	   return status;		// return success
}

void module_pollingstatus(void)
{
	polling_sd();
	polling_nand();
	polling_scaler();
	polling_jpeg();
	polling_dma();
	polling_dac();
	polling_spu();
	polling_uart();
	polling_adc();
	polling_usb();
	polling_deflicker();
}
void polling_sd(void)
{	
	INT32S module_status,i;
	//polling status for SD_CTRL_BUSY and SD_CAED_CTRL_BUSY, set timeout=10*2.67us
	if((R_SDC_CTRL & 0x800)==0x800){
	   for(i=0;i < polling_timeout;i++){
	       module_status=drvl1_sdc_status_set_wait(3, 10);
 	       if(module_status==status_ok)
 	          break;   
	       else			  
		      DBG_PRINT("sd polling timeout\r\n");
	   }
	}
}
void polling_scaler(void)
{
	INT32S module_status,i;
	//polling scaler status
	for(i=0;i < polling_timeout;i++){
	   module_status=scaler_status_polling();
	   if(module_status==C_SCALER_STATUS_STOP)
		  break;
	   else if(module_status==C_SCALER_STATUS_BUSY)
		  DBG_PRINT("scaler busy\r\n");
	   else if(module_status==C_SCALER_STATUS_DONE)
	      break;   
	   else if(module_status==C_SCALER_STATUS_INPUT_EMPTY)
		   break;
	}	
}
void polling_jpeg(void)
{
	INT32S module_status,i;
	if(((R_JPG_CTRL & 0x2)==0x2)||((R_JPG_CTRL & 0x1)==0x1)){
	   //polling jpeg status without wait done
	   for(i=0;i < polling_timeout;i++){
	       module_status=jpeg_status_polling(0);
	       if(module_status==C_JPG_STATUS_STOP)
		      break; 
	       else
		      DBG_PRINT("jpeg busy\r\n");
	   }
	}  
}
void polling_dma(void)
{
	INT32S module_status,i;
	//polling dma status
	for(i=0;i < polling_timeout;i++){
	   module_status=dma_status_get(0);
	   if(module_status==status_ok)
		  break; 
	   else
		  DBG_PRINT("dma0 busy\r\n");
	}
	for(i=0;i < polling_timeout;i++){
	   module_status=dma_status_get(1);
	   if(module_status==status_ok)
		  break; 
	   else
		  DBG_PRINT("dma1 busy\r\n");
	}
	for(i=0;i < polling_timeout;i++){
	   module_status=dma_status_get(2);
	   if(module_status==status_ok)
		  break; 
	   else
		  DBG_PRINT("dma2 busy\r\n");
	}
	for(i=0;i < polling_timeout;i++){
	   module_status=dma_status_get(3);
	   if(module_status==status_ok)
		  break; 
	   else
		  DBG_PRINT("dma3 busy\r\n");
	}	
}
void polling_nand(void)
{
	INT32S module_status,i;
	//polling nand status
	if((R_NF_CTRL & 0x1000)==0x1000){
	   for(i=0;i < polling_timeout;i++){
	       module_status=nand_status_get();
	       if(module_status==status_ok)
		      break; 
	       else
		      DBG_PRINT("nand busy\r\n");
	   }
	}	   
}
void polling_dac(void)
{
	INT32S module_status,i;
	//polling adc status
	if(((R_DAC_CHA_CTRL & 0x2000)==0x2000)||((R_DAC_CHB_CTRL & 0x2000)==0x2000)){
	    for(i=0;i < polling_timeout;i++){
	        module_status=dac_dbf_status_get();
            module_status=dac_dma_status_get();
	        if(module_status==status_ok)
		       break; 
	        else if(module_status==-1)
		       break; 
	        else
		       DBG_PRINT("adc busy\r\n");
	    }
	}
}
void polling_spu(void)
{
#if MCU_VERSION < GPL327XX
	INT32S module_status,i;
	//polling spu status
	for(i=0;i < polling_timeout;i++){
	    module_status=SPU_GetChannelStatus();
	    if(module_status==status_ok)
		   break; 
	    else
		   DBG_PRINT("spu busy\r\n");
	} 
#endif 
}
void polling_uart(void)
{
	INT32S module_status,i;
	//polling uart status
	if((R_UART_CTRL & 0x1000)==0x1000){
	    for(i=0;i < polling_timeout;i++){
	       module_status=uart_status_get();
	       if(module_status==status_ok)
		      break; 
	       else
		      DBG_PRINT("uart busy\r\n");
	    }  
	}
}
void polling_adc(void)
{
	INT32S module_status,i;
	//polling adc status
	if((R_ADC_SETUP & 0x4000)==0x4000){
	    for(i=0;i < polling_timeout;i++){
	       module_status=adc_status_get();
	       if(module_status==status_ok)
		      break; 
	       else
		      DBG_PRINT("adc busy\r\n");
	    }  
	}
}
void polling_usb(void)
{
	INT32S module_status,i;
	//polling usb status
	for(i=0;i < polling_timeout;i++){
	    module_status=usb_status_get();
	    if(module_status==status_ok)
		   break; 
	    else
		   DBG_PRINT("usb busy\r\n");  
	}
}
void polling_deflicker(void)
{
	INT32S module_status,i;
	//polling usb status
	for(i=0;i < polling_timeout;i++){
	    module_status=deflicker_status_get();
	    if(module_status==status_ok)
		   break; 
	    else
		   DBG_PRINT("deflicker busy\r\n");  
	}
}
INT32S uart_status_get(void)
{
   INT32U uart_status;
	  if((R_UART_STATUS & UART_CTRL_BUSY)==0)
          uart_status=status_ok;
	  else		  
		  uart_status=-1;  
   
	  return uart_status;  
}
INT32S adc_status_get(void)
{
   INT32U adc_status;
	  if((R_ADC_MADC_CTRL & ADC_CTRL_BUSY)==0x80)
          adc_status=status_ok;
	  else		  
		  adc_status=-1;  
   
	  return adc_status;  
}
INT32S nand_status_get(void)
{
   INT32U nand_status;
	  if(((R_NF_CTRL & NAND_CTRL_BUSY)==0x8000)&&((R_NF_DMA_CTRL & NAND_DMA_CTRL_BUSY)==0))
		  nand_status=status_ok;
	  else		  
		  nand_status=-1;  
   
	  return nand_status;  
}
INT32S usb_status_get(void)
{
   INT32U usb_status;
	  if(((*P_USBD_EPEVENT & USB_CTRL_BUSY)==0)&&((*P_USBD_DMAINT & USB_DMA_CTRL_BUSY)==0))
		  usb_status=status_ok;
	  else		  
		  usb_status=-1;  
   
	  return usb_status;  
}
INT32S deflicker_status_get(void)
{
   INT32U deflicker_status;
	  if((R_DEFLICKER_CTRL & DEFLICKER_CTRL_BUSY)==0)
		  deflicker_status=status_ok;
	  else		  
		  deflicker_status=-1;  
   
	  return deflicker_status;  
}
void system_module_init(void)
{
	int sd0_io, sd1_io;
	 //UART module initiation
	 uart0_buad_rate_set(UART0_BAUD_RATE);
	 //SDC module initiation
#if ((SD_POS == SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)||(SD_DUAL_SUPPORT==1))
	sd0_io = 0;
	sd1_io = 1;
#else
	sd0_io = 1;
	sd1_io = 0;
#endif
	 SD_IO_Request(sd0_io);
	 drvl2_sd_bus_clock_set(25000000); 
	 SD_IO_Release();
#if((SD_DUAL_SUPPORT==1)&&(MCU_VERSION>=GPL326XX_C))
	 SD_IO_Request(sd1_io);
	 drvl2_sd1_bus_clock_set(25000000); 
	 SD_IO_Release();
#endif
}
