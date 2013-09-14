/******************************************************************************
 * Privete Paresent Header
 ******************************************************************************/
#ifndef __DRV_L1_SFR_H__
#define __DRV_L1_SFR_H__



/******************************************************************************
 * Session: GPIO SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: GPIO SFR
 ******************************************************************************/

/******************************************************************************
 * GPIO: 0xC0000000
 ******************************************************************************/
#define R_IOA_I_DATA        	    (*((volatile INT32U *) 0xC0000000))
#define R_IOA_O_DATA        	    (*((volatile INT32U *) 0xC0000004))
#define R_IOA_DIR	                (*((volatile INT32U *) 0xC0000008))
#define R_IOA_ATT	                (*((volatile INT32U *) 0xC000000C))
#define R_IOA_DRV                   (*((volatile INT32U *) 0xC0000010))
#define R_IOB_I_DATA        	    (*((volatile INT32U *) 0xC0000020))
#define R_IOB_O_DATA        	    (*((volatile INT32U *) 0xC0000024))
#define R_IOB_DIR	                (*((volatile INT32U *) 0xC0000028))
#define R_IOB_ATT	                (*((volatile INT32U *) 0xC000002C))
#define R_IOB_DRV                   (*((volatile INT32U *) 0xC0000030))
#define R_IOC_I_DATA        	    (*((volatile INT32U *) 0xC0000040))
#define R_IOC_O_DATA        	    (*((volatile INT32U *) 0xC0000044))
#define R_IOC_DIR	                (*((volatile INT32U *) 0xC0000048))
#define R_IOC_ATT	                (*((volatile INT32U *) 0xC000004C))
#define R_IOC_DRV	                (*((volatile INT32U *) 0xC0000050))
#define R_IOD_I_DATA        	    (*((volatile INT32U *) 0xC0000060))
#define R_IOD_O_DATA        	    (*((volatile INT32U *) 0xC0000064))
#define R_IOD_DIR	                (*((volatile INT32U *) 0xC0000068))
#define R_IOD_ATT	                (*((volatile INT32U *) 0xC000006C))
#define R_IOD_DRV	                (*((volatile INT32U *) 0xC0000070))
#define R_IOE_I_DATA        	    (*((volatile INT32U *) 0xC0000080))
#define R_IOE_O_DATA        	    (*((volatile INT32U *) 0xC0000084))
#define R_IOE_DIR	                (*((volatile INT32U *) 0xC0000088))
#define R_IOE_ATT	                (*((volatile INT32U *) 0xC000008C))
#define R_IOE_DRV	                (*((volatile INT32U *) 0xC0000090))
#define R_IOF_I_DATA        	    (*((volatile INT32U *) 0xC00000A0))
#define R_IOF_O_DATA        	    (*((volatile INT32U *) 0xC00000A4))
#define R_IOF_DIR	                (*((volatile INT32U *) 0xC00000A8))
#define R_IOF_ATT	                (*((volatile INT32U *) 0xC00000AC))
#define R_IOF_DRV	                (*((volatile INT32U *) 0xC00000B0))
#define R_IOG_I_DATA	            (*((volatile INT32U *) 0xC00000C0))
#define R_IOG_O_DATA	            (*((volatile INT32U *) 0xC00000C4))
#define R_IOG_DIR	                (*((volatile INT32U *) 0xC00000C8))
#define R_IOG_ATT	                (*((volatile INT32U *) 0xC00000CC))
#define R_IOG_DRV	                (*((volatile INT32U *) 0xC00000D0))
#define R_IOH_I_DATA	            (*((volatile INT32U *) 0xC00000E0))
#define R_IOH_O_DATA	            (*((volatile INT32U *) 0xC00000E4))
#define R_IOH_DIR	                (*((volatile INT32U *) 0xC00000E8))
#define R_IOH_ATT	                (*((volatile INT32U *) 0xC00000EC))
#define R_IOH_DRV	                (*((volatile INT32U *) 0xC00000F0))

#if MCU_VERSION >= GPL326XX
#define R_IOA_DRV_H                 (*((volatile INT32U *) 0xC0000014))
#define R_IOB_DRV_H                 (*((volatile INT32U *) 0xC0000034))
#define R_IOC_DRV_H	                (*((volatile INT32U *) 0xC0000054))
#define R_FUNPOS2					(*((volatile INT32U *) 0xC0000174))
#endif

#if (MCU_VERSION >= GPL326XXB) || (MCU_VERSION < GPL327XX)
#define R_IOI_I_DATA	            (*((volatile INT32U *) 0xC0000180))
#define R_IOI_O_DATA	            (*((volatile INT32U *) 0xC0000184))
#define R_IOI_DIR	                (*((volatile INT32U *) 0xC0000188))
#define R_IOI_ATT	                (*((volatile INT32U *) 0xC000018C))
#define R_IOI_DRV	                (*((volatile INT32U *) 0xC0000190))



#endif



#define R_IOSMTSEL	           		(*((volatile INT32U *) 0xC0000100))
#define R_IOSRSEL	                (*((volatile INT32U *) 0xC0000104))  /*Dominant add, 06/17/2008*/
#define R_IO_SWITCH_ND	            (*((volatile INT32U *) 0xC0000108))
#define R_FUNPOS0    	            (*((volatile INT32U *) 0xC0000108))  /*Dominant add, 04/14/2008*/
#define R_KEYCH						(*((volatile INT32U *) 0xC0000110))
#define R_FUNPOS1					(*((volatile INT32U *) 0xC0000114))
#define R_FUNPOS3                   (*((volatile INT32U *) 0xC0000118))
#define R_MEMCTRL					(*((volatile INT32U *) 0xC0000120))
#define R_MEM_DRV	                (*((volatile INT32U *) 0xC0000124))
#define R_PWM_CTRL	                (*((volatile INT32U *) 0xC0000134))
#define R_GPIOCTRL                  (*((volatile INT32U *) 0xC0000114))  /*Dominant add, 04/18/2008*/
#define R_SYSMONICTRL               (*((volatile INT32U *) 0xC0000140))  /*Dominant add, 06/17/2008*/
#define R_ANALOG_CTRL				(*((volatile INT32U *) 0xC0000130))
#define R_PWMCTRL	                (*((volatile INT32U *) 0xC0000134))
#define R_SYSMONI_CTRL  			(*((volatile INT32U *) 0xC0000140))
#define R_SMONI0					(*((volatile INT32U *) 0xC0000150))
#define R_SMONI1					(*((volatile INT32U *) 0xC0000154))
#define R_XD_DCTRL					(*((volatile INT32U *) 0xC0000160))
#define R_XD_DCTRL1					(*((volatile INT32U *) 0xC0000164))
#define R_XA_DCTRL					(*((volatile INT32U *) 0xC0000168))
#define R_XA_DCTRL1					(*((volatile INT32U *) 0xC000016C))
#define R_OTR_DCTRL					(*((volatile INT32U *) 0xC0000170))

#define R_IOA_DATA                  R_IOA_I_DATA
#define R_IOA_BUFFER                R_IOA_O_DATA
#define R_IOA_ATTRIB                R_IOA_ATT
#define R_IOB_DATA                  R_IOB_I_DATA
#define R_IOB_BUFFER                R_IOB_O_DATA
#define R_IOB_ATTRIB                R_IOB_ATT
#define R_IOC_DATA                  R_IOC_I_DATA
#define R_IOC_BUFFER                R_IOC_O_DATA
#define R_IOC_ATTRIB                R_IOC_ATT
#define R_IOD_DATA                  R_IOD_I_DATA
#define R_IOD_BUFFER                R_IOD_O_DATA
#define R_IOD_ATTRIB                R_IOD_ATT
#define R_IOE_DATA                  R_IOE_I_DATA
#define R_IOE_BUFFER                R_IOE_O_DATA
#define R_IOE_ATTRIB                R_IOE_ATT
#define R_IOF_DATA                  R_IOF_I_DATA
#define R_IOF_BUFFER                R_IOF_O_DATA
#define R_IOF_ATTRIB                R_IOF_ATT
#define R_IOG_DATA                  R_IOG_I_DATA
#define R_IOG_BUFFER                R_IOG_O_DATA
#define R_IOG_ATTRIB                R_IOG_ATT
#define R_IOH_DATA                  R_IOH_I_DATA
#define R_IOH_BUFFER                R_IOH_O_DATA
#define R_IOH_ATTRIB                R_IOH_ATT


/******************************************************************************
 * Session: Timer1 SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Timer1 SFR
 ******************************************************************************/

/******************************************************************************
 * Timer1: 0xC0020000
 ******************************************************************************/
#define R_TIMERA_CTRL				(*((volatile INT32U *) 0xC0020000))
#define R_TIMERA_CCP_CTRL			(*((volatile INT32U *) 0xC0020004))
#define R_TIMERA_PRELOAD			(*((volatile INT32U *) 0xC0020008))
#define R_TIMERA_CCP_REG			(*((volatile INT32U *) 0xC002000C))
#define R_TIMERA_UPCOUNT			(*((volatile INT32U *) 0xC0020010))

#define R_TIMERB_CTRL				(*((volatile INT32U *) 0xC0020020))
#define R_TIMERB_CCP_CTRL			(*((volatile INT32U *) 0xC0020024))
#define R_TIMERB_PRELOAD			(*((volatile INT32U *) 0xC0020028))
#define R_TIMERB_CCP_REG			(*((volatile INT32U *) 0xC002002C))
#define R_TIMERB_UPCOUNT			(*((volatile INT32U *) 0xC0020030))

#define R_TIMERC_CTRL				(*((volatile INT32U *) 0xC0020040))
#define R_TIMERC_CCP_CTRL			(*((volatile INT32U *) 0xC0020044))
#define R_TIMERC_PRELOAD			(*((volatile INT32U *) 0xC0020048))
#define R_TIMERC_CCP_REG			(*((volatile INT32U *) 0xC002004C))
#define R_TIMERC_UPCOUNT			(*((volatile INT32U *) 0xC0020050))

#define R_TIMERD_CTRL				(*((volatile INT32U *) 0xC0020060))
#define R_TIMERD_PRELOAD			(*((volatile INT32U *) 0xC0020068))
#define R_TIMERD_UPCOUNT			(*((volatile INT32U *) 0xC0020070))

#define R_TIMERE_CTRL				(*((volatile INT32U *) 0xC0020080))
#define R_TIMERE_PRELOAD			(*((volatile INT32U *) 0xC0020088))
#define R_TIMERE_UPCOUNT			(*((volatile INT32U *) 0xC0020090))

#define R_TIMERF_CTRL				(*((volatile INT32U *) 0xC00200A0))
#define R_TIMERF_PRELOAD			(*((volatile INT32U *) 0xC00200A8))
#define R_TIMERF_UPCOUNT			(*((volatile INT32U *) 0xC00200B0))


/******************************************************************************
 * Session: Time Base SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Time Base SFR
 ******************************************************************************/

/******************************************************************************
 * Time Base: 0xC0030000
 ******************************************************************************/
#define R_TIMEBASEA_CTRL			(*((volatile INT32U *) 0xC0030000))
#define R_TIMEBASEB_CTRL			(*((volatile INT32U *) 0xC0030004))
#define R_TIMEBASEC_CTRL			(*((volatile INT32U *) 0xC0030008))
#define R_TIMEBASE_RESET    		(*((volatile INT32U *) 0xC0030020))


/******************************************************************************
 * Session: RTC SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: RTC SFR
 ******************************************************************************/

/******************************************************************************
 * RTC: 0xC0040000
 ******************************************************************************/
#define R_RTC_SEC		    		(*((volatile INT32U *) 0xC0040000))
#define R_RTC_MIN		    		(*((volatile INT32U *) 0xC0040004))
#define R_RTC_HOUR		    		(*((volatile INT32U *) 0xC0040008))
#define R_RTC_ALARM_SEC	    		(*((volatile INT32U *) 0xC0040010))
#define R_RTC_ALARM_MIN				(*((volatile INT32U *) 0xC0040014))
#define R_RTC_ALARM_HOUR			(*((volatile INT32U *) 0xC0040018))
#define R_RTC_CTRL		    		(*((volatile INT32U *) 0xC0040050))
#define R_RTC_INT_STATUS			(*((volatile INT32U *) 0xC0040054))
#define R_RTC_INT_CTRL				(*((volatile INT32U *) 0xC0040058))
#define R_RTC_BUSY 					(*((volatile INT32U *) 0xC004005C))

/******************************************************************************
 * Session: GPL32600 independent power RTC SFR                                                           
 * Layer: Driver Layer 1                                                             
 * Date: 
 * Note: RTC SFR                                                         
 ******************************************************************************/
 
/******************************************************************************
 * RTC: 0xC0090000
 ******************************************************************************/
#define R_RTC_IDPWR_CTRL		    	(*((volatile INT32U *) 0xC0090000))
#define R_RTC_IDPWR_CTRL_FLAG		    (*((volatile INT32U *) 0xC0090004))
#define R_RTC_IDPWR_ADDR		    	(*((volatile INT32U *) 0xC0090008))
#define R_RTC_IDPWR_WDATA		    	(*((volatile INT32U *) 0xC009000C))
#define R_RTC_IDPWR_RDATA		    	(*((volatile INT32U *) 0xC0090010))


/******************************************************************************
 * Session: Key Scan SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Key Scan SFR
 ******************************************************************************/

/******************************************************************************
 * Key Scan: 0xC0050000
 ******************************************************************************/
#define R_KEYSCAN_CTRL0					(*((volatile INT32U *) 0xC0050000))
#define R_KEYSCAN_CTRL1					(*((volatile INT32U *) 0xC0050004))
#define R_KEYSCAN_ADDR					(*((volatile INT32U *) 0xC0050008))
#define R_KEYSCAN_VELOCITY				(*((volatile INT32U *) 0xC005000C))

#define P_KEYSCAN_DATA0					((volatile INT32U *) 0xC0050020)
#define P_KEYSCAN_DATA1					((volatile INT32U *) 0xC0050024)
#define P_KEYSCAN_DATA2					((volatile INT32U *) 0xC0050028)
#define P_KEYSCAN_DATA3					((volatile INT32U *) 0xC005002c)
#define P_KEYSCAN_DATA4					((volatile INT32U *) 0xC0050030)
#define P_KEYSCAN_DATA5					((volatile INT32U *) 0xC0050034)
#define P_KEYSCAN_DATA6					((volatile INT32U *) 0xC0050038)
#define P_KEYSCAN_DATA7					((volatile INT32U *) 0xC005003C)
#define P_KEYSCAN_DATA8					((volatile INT32U *) 0xC0050040)
#define P_KEYSCAN_DATA9					((volatile INT32U *) 0xC0050044)
#define P_KEYSCAN_DATA10				((volatile INT32U *) 0xC0050048)

/******************************************************************************
 * Session: UART_IRDA SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: UART_IRDA SFR
 ******************************************************************************/

/******************************************************************************
 * Key Scan: 0xC0060000
 ******************************************************************************/
#define P_UART_DATA					((volatile INT32U *) 0xC0060000)

#define R_UART_DATA					(*((volatile INT32U *) 0xC0060000))
#define R_UART_RX_STATUS			(*((volatile INT32U *) 0xC0060004))
#define R_UART_CTRL					(*((volatile INT32U *) 0xC0060008))
#define R_UART_BAUD_RATE			(*((volatile INT32U *) 0xC006000C))
#define R_UART_STATUS				(*((volatile INT32U *) 0xC0060010))
#define R_UART_FIFO					(*((volatile INT32U *) 0xC0060014))
#define R_UART_TXDLY				(*((volatile INT32U *) 0xC0060018))
#define R_IRDA_BUAD_RATE			(*((volatile INT32U *) 0xC006001C))
#define R_IRDA_CTRL					(*((volatile INT32U *) 0xC0060020))
#define R_IRDA_LOWPOWER				(*((volatile INT32U *) 0xC0060024))


/******************************************************************************
 * Session: USB SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: USB SFR
 ******************************************************************************/



/******************************************************************************
 * USB: 0xC0070000
 ******************************************************************************/
#if  MCU_VERSION  >= GPL326XX
	#define	P_USBD_BASE  		    0xC00E0000
#else
    #define	P_USBD_BASE		        0xC0070000
#endif
#define	P_USBH_BASE					0xC0070000

/*USB Device*/
#define	P_USBD_CONFIG				((volatile INT32U *) (P_USBD_BASE + 0x00C0))
#define	P_USBD_DEVICE				((volatile INT32U *) (P_USBD_BASE + 0x015C))
#define	P_USBD_FUNCTION				((volatile INT32U *) (P_USBD_BASE + 0x00C4))
#define	P_USBD_DMAINT				((volatile INT32U *) (P_USBD_BASE + 0x0164))
#define	P_USBD_PMR					((volatile INT32U *) (P_USBD_BASE + 0x00C8))
#define	P_USBD_EP0DATA				((volatile INT32U *) (P_USBD_BASE + 0x00CC))
#define	P_USBD_BIDATA				((volatile INT32U *) (P_USBD_BASE + 0x00D0))
#define	P_USBD_BODATA				((volatile INT32U *) (P_USBD_BASE + 0x00D4))
#define	P_USBD_INTINDATA			((volatile INT32U *) (P_USBD_BASE + 0x00D8))
#define	P_USBD_NULLPKT				((volatile INT32U *) (P_USBD_BASE + 0x0160))
#define	P_USBD_EPEVENT				((volatile INT32U *) (P_USBD_BASE + 0x00DC))
#define	P_USBD_GLOINT				((volatile INT32U *) (P_USBD_BASE + 0x00E0))
#define	P_USBD_INTEN				((volatile INT32U *) (P_USBD_BASE + 0x00E4))
#define	P_USBD_INT					((volatile INT32U *) (P_USBD_BASE + 0x00E8))
#define	P_USBD_SCINTEN				((volatile INT32U *) (P_USBD_BASE + 0x00EC))
#define	P_USBD_SCINT				((volatile INT32U *) (P_USBD_BASE + 0x00F0))
#define	P_USBD_EPAUTOSET			((volatile INT32U *) (P_USBD_BASE + 0x00F4))
#define	P_USBD_EPSETSTALL			((volatile INT32U *) (P_USBD_BASE + 0x00F8))
#define	P_USBD_EPBUFCLEAR			((volatile INT32U *) (P_USBD_BASE + 0x00FC))
#define	P_USBD_EPEVNTCLEAR			((volatile INT32U *) (P_USBD_BASE + 0x0100))
#define	P_USBD_EP0WRTCOUNT			((volatile INT32U *) (P_USBD_BASE + 0x0104))
#define	P_USBD_BOWRTCOUNT			((volatile INT32U *) (P_USBD_BASE + 0x0108))
#define	P_USBD_EP0BUFPOINTER		((volatile INT32U *) (P_USBD_BASE + 0x010C))
#define P_USBD_BIBUFPOINTER	        ((volatile INT32U *) (P_USBD_BASE + 0x0110))
#define P_USBD_BOBUFPOINTER	        ((volatile INT32U *) (P_USBD_BASE + 0x0114))
#define P_USBD_ISOOBUFPOINTER       ((volatile INT32U *) (P_USBD_BASE + 0x0194))
#define P_USBD_ISOIBUFPOINTER       ((volatile INT32U *) (P_USBD_BASE + 0x0198))
#define P_USBD_INTBUFPOINTER        ((volatile INT32U *) (P_USBD_BASE + 0x012C))
#define P_USBD_EP0RTR               ((volatile INT32U *) (P_USBD_BASE + 0x0118))
#define P_USBD_EP0RR                ((volatile INT32U *) (P_USBD_BASE + 0x011C))
#define P_USBD_EP0VR                ((volatile INT32U *) (P_USBD_BASE + 0x0120))
#define P_USBD_EP0IR                ((volatile INT32U *) (P_USBD_BASE + 0x0124))
#define P_USBD_EP0LR                ((volatile INT32U *) (P_USBD_BASE + 0x0128))
#define P_USBD_EPSTALL              ((volatile INT32U *) (P_USBD_BASE + 0x0150))
#define P_USBD_EPSTALLBITCLEAR      ((volatile INT32U *) (P_USBD_BASE + 0x01BC))
#define P_USBD_ISOODATA             ((volatile INT32U *) (P_USBD_BASE + 0x0138))
#define P_USBD_ISOIDATA             ((volatile INT32U *) (P_USBD_BASE + 0x013C))
#define P_USBD_ISOEVENT	            ((volatile INT32U *) (P_USBD_BASE + 0x0170))
#define P_USBD_ISOINTEN	            ((volatile INT32U *) (P_USBD_BASE + 0x0174))
#define P_USBD_ISOINT	            ((volatile INT32U *) (P_USBD_BASE + 0x0178))
#define P_USBD_ISOOWRTCOUNT	        ((volatile INT32U *) (P_USBD_BASE + 0x017C))
#define P_USBD_IOUTDATA	            ((volatile INT32U *) (P_USBD_BASE + 0x0180))
#define P_USBD_IOUTEVENT            ((volatile INT32U *) (P_USBD_BASE + 0x0184))
#define P_USBD_IOUTINTEN            ((volatile INT32U *) (P_USBD_BASE + 0x0188))
#define P_USBD_IOUTINT              ((volatile INT32U *) (P_USBD_BASE + 0x018C))
#define P_USBD_IOUTWRTCOUNT         ((volatile INT32U *) (P_USBD_BASE + 0x0190))
#define P_USBD_INTF	                ((volatile INT32U *) (P_USBD_BASE + 0x0130))
#define P_USBD_ALT	                ((volatile INT32U *) (P_USBD_BASE + 0x0134))
#define P_USBD_CALT	                ((volatile INT32U *) (P_USBD_BASE + 0x0154))
#define P_USBD_MAXALT	            ((volatile INT32U *) (P_USBD_BASE + 0x0158))
#define P_USBD_MAXINT	            ((volatile INT32U *) (P_USBD_BASE + 0x016C))
#define P_USBD_DMAWRTCOUNT	        ((volatile INT32U *) (P_USBD_BASE + 0x0140))
#define P_USBD_DMAACK	            ((volatile INT32U *) (P_USBD_BASE + 0x0148))
#define P_USBD_DMASIZECNT	        ((volatile INT32U *) (P_USBD_BASE + 0x01C0))
#define P_USBD_BULKDMASIZE	        ((volatile INT32U *) (P_USBD_BASE + 0x01C8))
#define P_USBD_BULKDMA_STARTADDR	((volatile INT32U *) (P_USBD_BASE + 0x01D0))
#define P_USBD_ISOODMA_STARTADDR    ((volatile INT32U *) (P_USBD_BASE + 0x01D8))
#define P_USBD_ISOIDMA_STARTADDR    ((volatile INT32U *) (P_USBD_BASE + 0x01E0))
#define P_USBD_VISOIDMA_STARTADDR   ((volatile INT32U *) (P_USBD_BASE + 0x01E8))
#define P_USBD_ISOIDMA_WRCOUNT      ((volatile INT32U *) (P_USBD_BASE + 0x01EC))

// USBD20 new register
#if  MCU_VERSION  >= GPL326XX
#define P_USBD20_ISO                ((volatile INT32U *) (P_USBD_BASE + 0x0168))
#define	P_USBD_EPDIS				((volatile INT32U *) (P_USBD_BASE + 0x019C))
#define	P_USBD_PHYCONFIG			((volatile INT32U *) (P_USBD_BASE + 0x01A0))
#define	P_USBD_CONFIG1				((volatile INT32U *) (P_USBD_BASE + 0x01A4))
#define	P_USBD_PINGINTEN			((volatile INT32U *) (P_USBD_BASE + 0x01A8))
#define	P_USBD_PINGINT				((volatile INT32U *) (P_USBD_BASE + 0x01AC))
#define	P_USBD_VIDIDATA				((volatile INT32U *) (P_USBD_BASE + 0x01B0))
#define	P_USBD_VIDIBUFPTR			((volatile INT32U *) (P_USBD_BASE + 0x01B4))
#define	P_USBD_VIDIDMASTADDR		((volatile INT32U *) (P_USBD_BASE + 0x01E8))
#define	P_USBD_PROBE				((volatile INT32U *) (P_USBD_BASE + 0x01FC))
#define P_USB20_I2CDR				((volatile INT32U *) (P_USBD_BASE + 0x01C4))
#define P_USB20_I2CCR				((volatile INT32U *) (P_USBD_BASE + 0x01CC))
#endif
/******************************************************************************
 * USB HOST
 ******************************************************************************/
#define	P_USBH_CONFIG				((volatile INT32U *) (0xC0070000))
#define	P_USBH_TIMECONFIG			((volatile INT32U *) (0xC0070004))
#define	P_USBH_DATA					((volatile INT32U *) (0xC0070008))
#define	P_USBH_TRANSFER				((volatile INT32U *) (0xC007000C))
#define	P_USBH_DVEADDR				((volatile INT32U *) (0xC0070010))
#define	P_USBH_DEVICEEP				((volatile INT32U *) (0xC0070014))
#define	P_USBH_TXCOUNT				((volatile INT32U *) (0xC0070018))
#define	P_USBH_RXCOUNT				((volatile INT32U *) (0xC007001C))
#define	P_USBH_FIFO_INPOINTER		((volatile INT32U *) (0xC0070020))
#define	P_USBH_FIFO_OUTPOINTER		((volatile INT32U *) (0xC0070024))
#define	P_USBH_AUTOIN_BYTECOUNT		((volatile INT32U *) (0xC0070028))
#define	P_USBH_AUTOOUT_BYTECOUNT	((volatile INT32U *) (0xC007002C))
#define	P_USBH_AUTOTRANS			((volatile INT32U *) (0xC0070030))
#define	P_USBH_STATUS				((volatile INT32U *) (0xC0070034))
#define	P_USBH_INT					((volatile INT32U *) (0xC0070038))
#define	P_USBH_INTEN				((volatile INT32U *) (0xC007003C))
#define	P_USBH_STORAGE_RST			((volatile INT32U *) (0xC0070040))
#define	P_USBH_SOFT_RST	 			((volatile INT32U *) (0xC0070044))
#define	P_USBH_SOF_TIMER			((volatile INT32U *) (0xC0070048))
#define	P_USBH_FRAME_NUM			((volatile INT32U *) (0xC007004C))
#define	P_USBH_OTG_CONFIG			((volatile INT32U *) (0xC0070050))
#define	P_USBH_VBUS_SET				((volatile INT32U *) (0xC0070054))
#define	P_USBH_VBUS_STATUS			((volatile INT32U *) (0xC0070058))
#define	P_USBH_INACK_COUNT			((volatile INT32U *) (0xC007005C))
#define	P_USBH_OUTACK_COUNT			((volatile INT32U *) (0xC0070060))
#define	P_USBH_RSTACK_COUNT			((volatile INT32U *) (0xC0070064))
#define	P_USBH_STORAGE				((volatile INT32U *) (0xC0070068))
#define	P_USBH_DREADBACK			((volatile INT32U *) (0xC007006C))
#define	P_USBH_SOF_BOUNDARY			((volatile INT32U *) (0xC0070080))
#define	P_USBH_ISOCONFIG			((volatile INT32U *) (0xC0070084))

#if  MCU_VERSION  >= GPL326XX //usb2.0 Host

#define P_USBH20_HOST_DATA			(volatile unsigned int*)(0xC00E0800)
#define P_USBH20_HOSTXFR			(volatile unsigned int*)(0xC00E0804)
#define P_USBH20_TXCOUNT			(volatile unsigned int*)(0xC00E0808)
#define P_USBH20_RXCOUNT			(volatile unsigned int*)(0xC00E0810)
#define P_USBH20_FIFOINIDX			(volatile unsigned int*)(0xC00E0818)
#define P_USBH20_FIFOOUTIDX			(volatile unsigned int*)(0xC00E0820)
#define P_USBH20_STORAGE0_RST		(volatile unsigned int*)(0xC00E0AC0)
#define P_USBH20_STORAGE1_RST		(volatile unsigned int*)(0xC00E0AC8)
#define P_USBH20_RXSTATUS			(volatile unsigned int*)(0xC00E0828)
#define P_USBH20_HOSTFIFOCLR		(volatile unsigned int*)(0xC00E0838)
#define P_USBH20_SOFT_RST			(volatile unsigned int*)(0xC00E083C)
#define P_USBH20_DELAYTIME			(volatile unsigned int*)(0xC00E0840)
#define P_USBH20_LINESTATE			(volatile unsigned int*)(0xC00E0844)
#define P_USBH20_SOFTIMER_EN		(volatile unsigned int*)(0xC00E0848)
#define P_USBH20_SOFTIMER			(volatile unsigned int*)(0xC00E0850)
#define P_USBH20_FRAME_NO			(volatile unsigned int*)(0xC00E0858)
#define P_USBH20_SOF_CNT			(volatile unsigned int*)(0xC00E0860)
#define P_USBH20_SOF_CNTHS			(volatile unsigned int*)(0xC00E0868)
#define P_USBH20_CNCTCNT0			(volatile unsigned int*)(0xC00E0870)
#define P_USBH20_CNCTCNT2			(volatile unsigned int*)(0xC00E0878)
#define P_USBH20_HSFS_CNC			(volatile unsigned int*)(0xC00E087C)
#define P_USBH20_DEVICE_ADDR		(volatile unsigned int*)(0xC00E0880)
#define P_USBH20_DEVICE_POINT		(volatile unsigned int*)(0xC00E0884)
#define P_USBH20_DMAINACKSIZE		(volatile unsigned int*)(0xC00E0888)
#define P_USBH20_DMAOUTACKSIZE		(volatile unsigned int*)(0xC00E0890)
#define P_USBH20_DMAAUTO_EN			(volatile unsigned int*)(0xC00E0898)
#define P_USBH20_INACKCNT			(volatile unsigned int*)(0xC00E089C)
#define P_USBH20_OUTACKCNT			(volatile unsigned int*)(0xC00E08A4)
#define P_USBH20_CLRACKCNT			(volatile unsigned int*)(0xC00E08AC)
#define P_USBH20_SOFMASKCNT			(volatile unsigned int*)(0xC00E08B0)
#define P_USBH20_SOFMASKCNTHS		(volatile unsigned int*)(0xC00E08B8)
#define P_USBH20_DELAY_EN			(volatile unsigned int*)(0xC00E08C0)
#define P_USBH20_TEST_EN			(volatile unsigned int*)(0xC00E08C4)
//#define P_USBH20_DebugEnable		(volatile unsigned int*)(0xC00E08C8)
#define P_USBH20_HOST_EN			(volatile unsigned int*)(0xC00E08CC)
//#define P_USBH20_DebugCnt			(volatile unsigned int*)(0xC00E08D0)
#define P_USBH20_FORCE_EN			(volatile unsigned int*)(0xC00E08D8)
#define P_USBH20_SPEEDMODE			(volatile unsigned int*)(0xC00E0A08)
#define P_USBH20_INT				(volatile unsigned int*)(0xC00E0B00)
#define P_USBH20_INT_EN				(volatile unsigned int*)(0xC00E0B40)
#define P_USBH20_DMAREQ				(volatile unsigned int*)(0xC00E0C00)
#define P_USBH20_RXSTATE			(volatile unsigned int*)(0xC00E0C44)
#define	P_USBH20_ISOCONFIG			(volatile unsigned int*)(0xC00E08C4)

#endif

/******************************************************************************
 * Session: SPI SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: SPI SFR
 ******************************************************************************/

/******************************************************************************
 * SPI: 0xC0080000
 ******************************************************************************/
#define SPI0_BASE	0xC0080000

#if MCU_VERSION == GPL326XXB || MCU_VERSION == GP326XXXA
#define SPI1_BASE	0xC0150000
#else
#define SPI1_BASE	0xC0090000
#endif

#define P_SPI0_CTRL       		    ((volatile INT32U *) SPI0_BASE + 0x00)
#define P_SPI0_TX_DATA       		((volatile INT32U *) SPI0_BASE + 0x08)
#define P_SPI0_RX_DATA       		((volatile INT32U *) SPI0_BASE + 0x10)

#define R_SPI0_CTRL          		(*((volatile INT32U *) SPI0_BASE + 0x00))
#define R_SPI0_TX_STATUS     		(*((volatile INT32U *) SPI0_BASE + 0x04))
#define R_SPI0_TX_DATA       		(*((volatile INT32U *) SPI0_BASE + 0x08))
#define R_SPI0_RX_STATUS     		(*((volatile INT32U *) SPI0_BASE + 0x0C))
#define R_SPI0_RX_DATA       		(*((volatile INT32U *) SPI0_BASE + 0x10))
#define R_SPI0_MISC          		(*((volatile INT32U *) SPI0_BASE + 0x14))

#define P_SPI1_CTRL       		    ((volatile INT32U *) SPI1_BASE + 0x00)
#define P_SPI1_TX_DATA       		((volatile INT32U *) SPI1_BASE + 0x08)
#define P_SPI1_RX_DATA       		((volatile INT32U *) SPI1_BASE + 0x10)

#define R_SPI1_CTRL          		(*((volatile INT32U *) SPI1_BASE + 0x00))
#define R_SPI1_TX_STATUS     		(*((volatile INT32U *) SPI1_BASE + 0x04))
#define R_SPI1_TX_DATA       		(*((volatile INT32U *) SPI1_BASE + 0x08))
#define R_SPI1_RX_STATUS     		(*((volatile INT32U *) SPI1_BASE + 0x0C))
#define R_SPI1_RX_DATA       		(*((volatile INT32U *) SPI1_BASE + 0x10))
#define R_SPI1_MISC          		(*((volatile INT32U *) SPI1_BASE + 0x14))

/******************************************************************************
 * Session: SDC SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: SDC SFR
 ******************************************************************************/

/******************************************************************************
 * SDC: 0xC00A0000
 ******************************************************************************/
#define	P_SDC_BASE					0xC00A0000
#define	P_SDC_DATA_TX				((volatile INT32U *) (P_SDC_BASE + 0x0000))
#define	P_SDC_DATA_RX				((volatile INT32U *) (P_SDC_BASE + 0x0004))

#define	R_SDC_DATA_TX				(*((volatile INT32U *) (P_SDC_BASE + 0x0000)))
#define	R_SDC_DATA_RX				(*((volatile INT32U *) (P_SDC_BASE + 0x0004)))
#define	R_SDC_CMMAND				(*((volatile INT32U *) (P_SDC_BASE + 0x0008)))
#define	R_SDC_ARGUMENT				(*((volatile INT32U *) (P_SDC_BASE + 0x000C)))
#define	R_SDC_RESPONSE				(*((volatile INT32U *) (P_SDC_BASE + 0x0010)))
#define	R_SDC_STATUS				(*((volatile INT32U *) (P_SDC_BASE + 0x0014)))
#define	R_SDC_CTRL				    (*((volatile INT32U *) (P_SDC_BASE + 0x0018)))
#define	R_SDC_INTEN					(*((volatile INT32U *) (P_SDC_BASE + 0x001C)))

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB)	&& (MCU_VERSION < GPL327XX)
/******************************************************************************
 * SDC1: 0xC00B000
 ******************************************************************************/
#define	P_SDC1_BASE					0xC00B0000
#define	P_SDC1_DATA_TX				((volatile INT32U *) (P_SDC1_BASE + 0x0000))
#define	P_SDC1_DATA_RX				((volatile INT32U *) (P_SDC1_BASE + 0x0004))

#define	R_SDC1_DATA_TX				(*((volatile INT32U *) (P_SDC1_BASE + 0x0000)))
#define	R_SDC1_DATA_RX				(*((volatile INT32U *) (P_SDC1_BASE + 0x0004)))
#define	R_SDC1_CMMAND				(*((volatile INT32U *) (P_SDC1_BASE + 0x0008)))
#define	R_SDC1_ARGUMENT				(*((volatile INT32U *) (P_SDC1_BASE + 0x000C)))
#define	R_SDC1_RESPONSE				(*((volatile INT32U *) (P_SDC1_BASE + 0x0010)))
#define	R_SDC1_STATUS				(*((volatile INT32U *) (P_SDC1_BASE + 0x0014)))
#define	R_SDC1_CTRL				    (*((volatile INT32U *) (P_SDC1_BASE + 0x0018)))
#define	R_SDC1_INTEN				(*((volatile INT32U *) (P_SDC1_BASE + 0x001C)))

#endif
/******************************************************************************
 * Session: ADC SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: ADC SFR
 ******************************************************************************/

/******************************************************************************
 * ADC: 0xC00C0000
 ******************************************************************************/
#define P_ADC_ASADC_DATA       			((volatile INT32U *) 0xC00C0010)

#define R_ADC_SETUP       			    (*((volatile INT32U *) 0xC00C0000))
#define R_ADC_MADC_CTRL       			(*((volatile INT32U *) 0xC00C0004))
#define R_ADC_MADC_DATA       			(*((volatile INT32U *) 0xC00C0008))
#define R_ADC_ASADC_CTRL       			(*((volatile INT32U *) 0xC00C000C))
#define R_ADC_ASADC_DATA       			(*((volatile INT32U *) 0xC00C0010))
#define R_ADC_TP_CTRL        			(*((volatile INT32U *) 0xC00C0014))
#define R_ADC_USELINEIN        	    	(*((volatile INT32U *) 0xC00C0018))
#define R_ADC_SH_WAIT       			(*((volatile INT32U *) 0xC00C001C))

/******************************************************************************
 * Session: DAC SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: DAC SFR
 ******************************************************************************/

/******************************************************************************
 * DAC: 0xC00D0000
 ******************************************************************************/
#define P_DAC_CHA_DATA       			((volatile INT32U *) 0xC00D0004)
#define P_DAC_CHB_DATA       			((volatile INT32U *) 0xC00D0024)

#define R_DAC_CHA_CTRL                  (*((volatile INT32U *) 0xC00D0000))
#define R_DAC_CHA_DATA                  (*((volatile INT32U *) 0xC00D0004))
#define R_DAC_CHA_FIFO                  (*((volatile INT32U *) 0xC00D0008))
#define R_DAC_CHB_CTRL                  (*((volatile INT32U *) 0xC00D0020))
#define R_DAC_CHB_DATA                  (*((volatile INT32U *) 0xC00D0024))
#define R_DAC_CHB_FIFO                  (*((volatile INT32U *) 0xC00D0028))
#define R_DAC_PGA                       (*((volatile INT32U *) 0xC00D002C))
#define R_DAC_IISEN                     (*((volatile INT32U *) 0xC00D003C))

/******************************************************************************
 * Session: CF SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: CF SFR
 ******************************************************************************/

/******************************************************************************
 * CF: 0xC00E0000
 ******************************************************************************/
#define P_CFC_DMADATA				((volatile INT32U *) 0xC00E0018)

#define R_CFC_CTRL					(*((volatile INT32U *) 0xC00E0000))
#define R_CFC_ADDR					(*((volatile INT32U *) 0xC00E0004))
#define R_CFC_CMD					(*((volatile INT32U *) 0xC00E0008))
#define R_CFC_CYCLE_SEL				(*((volatile INT32U *) 0xC00E000C))
#define R_CFC_STATUS				(*((volatile INT32U *) 0xC00E0010))
#define R_CFC_DMACTRL				(*((volatile INT32U *) 0xC00E0014))
#define R_CFC_DMADATA				(*((volatile INT32U *) 0xC00E0018))
#define R_CFC_DMACNT				(*((volatile INT32U *) 0xC00E001C))
#define R_CFC_INTEN					(*((volatile INT32U *) 0xC00E0020))


/******************************************************************************
 * Session: MSC SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: MSC SFR
 ******************************************************************************/

/******************************************************************************
 * MSC: 0xC00F0000
 ******************************************************************************/
#define MSCBASE	   					0xC00F0000
#define	P_MSC_COMMAND				((volatile INT32U *) (MSCBASE+0x00000000))
#define	P_MSC_CTRL			    	((volatile INT32U *) (MSCBASE+0x00000004))
#define	P_MSC_STATUS				((volatile INT32U *) (MSCBASE+0x00000008))
#define	P_MSC_GETINT    	    	((volatile INT32U *) (MSCBASE+0x0000000C))
#define	P_MSC_DATATX    	    	((volatile INT32U *) (MSCBASE+0x00000010))
#define	P_MSC_DATARX    	    	((volatile INT32U *) (MSCBASE+0x00000014))
#define	P_MSC_SETRWADR   	    	((volatile INT32U *) (MSCBASE+0x00000018))
#define	P_MSC_READREG   	    	((volatile INT32U *) (MSCBASE+0x0000001C))
#define	P_MSC_WRITEREG   	    	((volatile INT32U *) (MSCBASE+0x00000020))
#define	P_MSC_DATACNT   	    	((volatile INT32U *) (MSCBASE+0x00000024))
#define	P_MSC_DATAADR   	    	((volatile INT32U *) (MSCBASE+0x00000028))
/******************************************************************************
 * Session: NAND SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: NAND SFR
 ******************************************************************************/

/******************************************************************************
 * NAND: 0xD0900000
 ******************************************************************************/
#define R_NF_DMA_CTRL          		(*((volatile INT32U *) 0xD0900000))
#define R_NF_DMA_ADDR   		    (*((volatile INT32U *) 0xD0900004))
#define R_NF_DMA_LEN       		    (*((volatile INT32U *) 0xD0900008))
#define R_NF_CTRL             		(*((volatile INT32U *) 0xD0900140))
#define R_NF_CMD           		    (*((volatile INT32U *) 0xD0900144))
#define R_NF_ADDRL              	(*((volatile INT32U *) 0xD0900148))
#define R_NF_ADDRH           		(*((volatile INT32U *) 0xD090014C))
#define R_NF_DATA           		(*((volatile INT32U *) 0xD0900150))
#define R_NF_ADDR_CTRL             	(*((volatile INT32U *) 0xD0900154))
#define R_NF_RESERVE_REG            (*((volatile INT32U *) 0xD09001DC)) 

#define R_NF_ECC_CTRL          		(*((volatile INT32U *) 0xD090015C))
#define R_NF_ECC_LPRL_LB       		(*((volatile INT32U *) 0xD0900160))
#define R_NF_ECC_LPRH_LB       		(*((volatile INT32U *) 0xD0900164))
#define R_NF_ECC_CPR_LB        		(*((volatile INT32U *) 0xD0900168))
#define R_NF_ECC_LPR_CKL_LB        	(*((volatile INT32U *) 0xD090016C))
#define R_NF_ECC_LPR_CKH_LB        	(*((volatile INT32U *) 0xD0900170))
#define R_NF_ECC_CPCKR_LB			(*((volatile INT32U *) 0xD0900174))
#define R_NF_ECC_ERR0_LB            (*((volatile INT32U *) 0xD0900178))
#define R_NF_ECC_ERR1_LB            (*((volatile INT32U *) 0xD090017C))
#define R_NF_ECC_LPRL_HB            (*((volatile INT32U *) 0xD0900120))
#define R_NF_ECC_LPRH_HB            (*((volatile INT32U *) 0xD0900124))
#define R_NF_ECC_CPR_HB             (*((volatile INT32U *) 0xD0900128))
#define R_NF_ECC_LPR_CKL_HB         (*((volatile INT32U *) 0xD090012C))
#define R_NF_ECC_LPR_CKH_HB         (*((volatile INT32U *) 0xD0900130))
#define R_NF_ECC_CPCKR_HB           (*((volatile INT32U *) 0xD0900134))
#define R_NF_ECC_ERR0_HB            (*((volatile INT32U *) 0xD0900138))
#define R_NF_ECC_ERR1_HB            (*((volatile INT32U *) 0xD090013C))
#define R_NF_CHECKSUM0_LB           (*((volatile INT32U *) 0xD09000C0))
#define R_NF_CHECKSUM1_LB 		    (*((volatile INT32U *) 0xD09000C4))
#define R_NF_CHECKSUM0_HB           (*((volatile INT32U *) 0xD09000C8))
#define R_NF_CHECKSUM1_HB           (*((volatile INT32U *) 0xD09000CC))

#define R_NF_BCH_CTRL               (*((volatile INT32U *) 0xD0900158))
#define R_NF_BCH_ERROR              (*((volatile INT32U *) 0xD0900160))
#define R_NF_BCH_PARITY0            (*((volatile INT32U *) 0xD0900164))
#define R_NF_BCH_PARITY1            (*((volatile INT32U *) 0xD0900168))
#define R_NF_BCH_PARITY2            (*((volatile INT32U *) 0xD090016C))
#define R_NF_BCH_PARITY3            (*((volatile INT32U *) 0xD0900170))
#define R_NF_BCH_PARITY4            (*((volatile INT32U *) 0xD0900174))
#define R_NF_BCH_PARITY5            (*((volatile INT32U *) 0xD0900178))
#define R_NF_BCH_PARITY6            (*((volatile INT32U *) 0xD090017C))
#define R_NF_BCH_PARITY7            (*((volatile INT32U *) 0xD0900180))
#define R_NF_BCH_PARITY8            (*((volatile INT32U *) 0xD0900184))
#define R_NF_BCH_PARITY9            (*((volatile INT32U *) 0xD0900188))

#define R_NF_SHARE_DELAY            (*((volatile INT32U *) 0xD02000A4))
#define R_NF_SHARE_BYTES            (*((volatile INT32U *) 0xD0900018))


#define R_BCH_FIND_ERR_BASE         (*((volatile INT32U *) 0xC0100000))
#define R_BCH_FIND_ERR_CTRL         (*((volatile INT32U *) 0xC0100000))
#define R_BCH_STATUS_REG1           (*((volatile INT32U *) 0xC0100004))
#define P_BCH_LOCATION_VAL0         ((volatile INT32U *) 0xC0100020)
#define R_BCH_LOCATION_VAL0         (*((volatile INT32U *) 0xC0100020))
#define R_BCH_LOCATION_VAL1         (*((volatile INT32U *) 0xC0100024))
#define R_BCH_LOCATION_VAL2         (*((volatile INT32U *) 0xC0100028))
#define R_BCH_LOCATION_VAL3         (*((volatile INT32U *) 0xC010002C))
#define R_BCH_LOCATION_VAL4         (*((volatile INT32U *) 0xC0100030))
#define R_BCH_LOCATION_VAL5         (*((volatile INT32U *) 0xC0100034))
#define R_BCH_LOCATION_VAL6         (*((volatile INT32U *) 0xC0100038))
#define R_BCH_LOCATION_VAL7         (*((volatile INT32U *) 0xC010003C))
#define R_BCH_LOCATION_VAL8         (*((volatile INT32U *) 0xC0100040))
#define R_BCH_LOCATION_VAL9         (*((volatile INT32U *) 0xC0100044))
#define R_BCH_LOCATION_VAL10        (*((volatile INT32U *) 0xC0100048))
#define R_BCH_LOCATION_VAL11        (*((volatile INT32U *) 0xC010004C))
#define R_NF_BCH_FIND_ERR_PARITY0   (*((volatile INT32U *) 0xC0100050))
#define R_NF_BCH_FIND_ERR_PARITY1   (*((volatile INT32U *) 0xC0100054))
#define R_NF_BCH_FIND_ERR_PARITY2   (*((volatile INT32U *) 0xC0100058))
#define R_NF_BCH_FIND_ERR_PARITY3   (*((volatile INT32U *) 0xC010005c))
#define R_NF_BCH_FIND_ERR_PARITY4   (*((volatile INT32U *) 0xC0100060))
#define R_NF_BCH_FIND_ERR_PARITY5   (*((volatile INT32U *) 0xC0100064))
#define R_NF_BCH_FIND_ERR_PARITY6   (*((volatile INT32U *) 0xC0100068))
#define R_NF_BCH_FIND_ERR_PARITY7   (*((volatile INT32U *) 0xC010006c))
#define R_NF_BCH_FIND_ERR_PARITY8   (*((volatile INT32U *) 0xC0100070))
#define R_NF_BCH_FIND_ERR_PARITY9   (*((volatile INT32U *) 0xC0100074))


/******************************************************************************
 * NAND: BCH-60bit
 ******************************************************************************/
//#define R_NF_DMA_CTRL          		(*((volatile INT32U *) 0xD0900000))
//#define R_NF_DMA_ADDR   		    (*((volatile INT32U *) 0xD0900004))
//#define R_NF_DMA_LEN       		    (*((volatile INT32U *) 0xD0900008))
//#define R_NF_SHARE_BYTES            (*((volatile INT32U *) 0xD0900018))
//#define R_NF_CTRL             		(*((volatile INT32U *) 0xD0900140))
//#define R_NF_CMD           		    (*((volatile INT32U *) 0xD0900144))
//#define R_NF_ADDRL              	(*((volatile INT32U *) 0xD0900148))
//#define R_NF_ADDRH           		(*((volatile INT32U *) 0xD090014C))
//#define R_NF_DATA           		(*((volatile INT32U *) 0xD0900150))
//#define R_NF_ADDR_CTRL             	(*((volatile INT32U *) 0xD0900154))
//#define R_NF_BCH_CTRL             	(*((volatile INT32U *) 0xD0900158))

//#define R_NF_SHARE_DELAY            (*((volatile INT32U *) 0xD02000A4))

// RANDOMIZE
#define	R_NF_RANDOM_CTRL	        (*((volatile INT32U *) 0xD09001CC))
#define	R_NF_LFSR_COE	            (*((volatile INT32U *) 0xD09001D0))
#define	R_NF_SEEDGEN	            (*((volatile INT32U *) 0xD09001D4))
#define	R_NF_PAGEADDR	            (*((volatile INT32U *) 0xD09001D8))
#define	R_NF_PRE_NUM0	            (*((volatile INT32U *) 0xD09001E0))
#define	R_NF_PRE_NUM1	            (*((volatile INT32U *) 0xD09001E4))
#define	R_NF_PRE_NUM2	            (*((volatile INT32U *) 0xD09001E8))
#define	R_NF_PRE_NUM3	            (*((volatile INT32U *) 0xD09001EC))
#define	R_NF_PRE_NUM4	            (*((volatile INT32U *) 0xD09001F0))
#define	R_NF_PRE_NUM5	            (*((volatile INT32U *) 0xD09001F4))
#define	R_NF_PRE_NUM6	            (*((volatile INT32U *) 0xD09001F8))
#define	R_NF_PRE_NUM7	            (*((volatile INT32U *) 0xD09001FC))

#define	R_NF_CS_CTRL	            (*((volatile INT32U *) 0xD09001DC))


#define BCH_S332_BASE				0xD0800000  // For GP17 New Mapping
#define P_BCH_CFG           		(*((volatile INT32U *) 0xD0800000))
#define P_BCH_DATA_PTR         		(*((volatile INT32U *) 0xD0800004))
#define P_BCH_PARITY_PTR       		(*((volatile INT32U *) 0xD0800008))
#define P_BCH_INT_STATUS       		(*((volatile INT32U *) 0xD080000C))
#define P_BCH_SOFT_RST       		(*((volatile INT32U *) 0xD0800010))
#define P_BCH_INT_MASK       		(*((volatile INT32U *) 0xD0800014))
#define P_BCH_REPORT_STATUS     	(*((volatile INT32U *) 0xD0800018))
#define P_BCH_SEC_ERR_REPORT     	(*((volatile INT32U *) 0xD080001C))
#define P_BCH_SEC_FAIL_REPORT     	(*((volatile INT32U *) 0xD0800020))
#define P_BCH_MAX_ERR_BITS      	(*((volatile INT32U *) 0xD0800024))
#define P_BCH_SHIFT_ADDR        	(*((volatile INT32U *) 0xD0800028))
#define R_BCH_USER_DATA_0          	(*((volatile INT32U *) 0xD0800030))
#define R_BCH_USER_DATA_1          	(*((volatile INT32U *) 0xD0800034))
#define R_BCH_USER_DATA_2          	(*((volatile INT32U *) 0xD0800038))
#define R_BCH_USER_DATA_3          	(*((volatile INT32U *) 0xD080003C))
#define R_BCH_USER_DATA_4          	(*((volatile INT32U *) 0xD0800040))
#define R_BCH_USER_DATA_5          	(*((volatile INT32U *) 0xD0800044))
#define R_BCH_USER_DATA_6          	(*((volatile INT32U *) 0xD0800048))
#define R_BCH_USER_DATA_7          	(*((volatile INT32U *) 0xD080004C))

/******************************************************************************
 * Session: System control SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: System control SFR
 ******************************************************************************/

/******************************************************************************
 * System control: 0xD0000000
 ******************************************************************************/
#define R_SYSTEM_BODY_ID         		(*((volatile INT32U *) 0xD0000000))
#define R_SYSTEM_MIPI_CTRL         		(*((volatile INT32U *) 0xD0000008))
#define R_SYSTEM_CTRL_NEW          		(*((volatile INT32U *) 0xD0000008))
#define R_SYSTEM_CTRL           		(*((volatile INT32U *) 0xD000000C))
#define R_SYSTEM_CLK_EN0  	      		(*((volatile INT32U *) 0xD0000010))
#define R_SYSTEM_CLK_EN1         		(*((volatile INT32U *) 0xD0000014))
#define R_SYSTEM_RESET_FLAG        		(*((volatile INT32U *) 0xD0000018))
#define R_SYSTEM_CLK_CTRL               (*((volatile INT32U *) 0xD000001C))
#define R_SYSTEM_LVR_CTRL               (*((volatile INT32U *) 0xD0000020))

#define R_SYSTEM_WATCHDOG_CTRL          (*((volatile INT32U *) 0xD0000028))
#define R_SYSTEM_WATCHDOG_CLEAR         (*((volatile INT32U *) 0xD000002C))
#define R_SYSTEM_WAIT  				    (*((volatile INT32U *) 0xD0000030))
#define R_SYSTEM_HALT  				    (*((volatile INT32U *) 0xD0000034))
#define R_SYSTEM_SLEEP  				(*((volatile INT32U *) 0xD0000038))
#define R_SYSTEM_POWEP_STATE       		(*((volatile INT32U *) 0xD000003C))
#define R_SYSTEM_PLLEN              	(*((volatile INT32U *) 0xD000005C))
#define R_SYSTEM_PLL_WAIT_CLK           (*((volatile INT32U *) 0xD0000060))



/******************************************************************************
 * Session: Interrupt control SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Interrupt control SFR
 ******************************************************************************/

/******************************************************************************
 * Interrupt control: 0xD0100000
 ******************************************************************************/

#define R_INT_IRQFLAG				(*((volatile INT32U *) 0xD0100000))
#define R_INT_FIQFLAG				(*((volatile INT32U *) 0xD0100004))
#define R_INT_I_PMST				(*((volatile INT32U *) 0xD0100008))
#define R_INT_I_PSLV0				(*((volatile INT32U *) 0xD0100010))
#define R_INT_I_PSLV1				(*((volatile INT32U *) 0xD0100014))
#define R_INT_I_PSLV2				(*((volatile INT32U *) 0xD0100018))
#define R_INT_I_PSLV3				(*((volatile INT32U *) 0xD010001C))
#define R_INT_KECON					(*((volatile INT32U *) 0xD0100020))
#define R_INT_IRQNUM				(*((volatile INT32U *) 0xD0100028))
#define R_INT_FIQNUM				(*((volatile INT32U *) 0xD010002C))
#define R_INT_IRQMASK				(*((volatile INT32U *) 0xD0100030))
#define R_INT_FIQMASK				(*((volatile INT32U *) 0xD0100034))
#define R_INT_GMASK					(*((volatile INT32U *) 0xD0100038))


/******************************************************************************
 * Session: Memory control SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Memory control SFR
 ******************************************************************************/

/******************************************************************************
 * Memory control: 0xD0200000 and D09001F0
 ******************************************************************************/
#define R_MEM_CS0_CTRL				(*((volatile INT32U *) 0xD0200000))
#define R_MEM_CS1_CTRL				(*((volatile INT32U *) 0xD0200004))
#define R_MEM_CS2_CTRL				(*((volatile INT32U *) 0xD0200008))
#define R_MEM_CS3_CTRL				(*((volatile INT32U *) 0xD020000C))
#define R_MEM_SDRAM_CTRL0			(*((volatile INT32U *) 0xD0200040))
#define R_MEM_SDRAM_CTRL1			(*((volatile INT32U *) 0xD0200044))
#define R_MEM_SDRAM_TIMING			(*((volatile INT32U *) 0xD0200048))
#define R_MEM_SDRAM_CBRCYC			(*((volatile INT32U *) 0xD020004C))
#define R_MEM_SDRAM_MISC			(*((volatile INT32U *) 0xD0200050))
#define R_MEM_SDRAM_STATUS			(*((volatile INT32U *) 0xD0200054))
#define R_MEM_NOR_FLASH_ADDR		(*((volatile INT32U *) 0xD0200024))
#define R_MEM_IO_CTRL               (*((volatile INT32U *) 0xD0200060))  /* Dominant add, 04/14/2008 */
#define R_MEM_M2_BUS_PRIORITY		(*((volatile INT32U *) 0xD0200080))
#define R_MEM_M3_BUS_PRIORITY		(*((volatile INT32U *) 0xD0200084))
#define R_MEM_M4_BUS_PRIORITY		(*((volatile INT32U *) 0xD0200088))
#define R_MEM_M5_BUS_PRIORITY		(*((volatile INT32U *) 0xD020008C))
#define R_MEM_M6_BUS_PRIORITY		(*((volatile INT32U *) 0xD0200090))
#define R_MEM_M7_BUS_PRIORITY		(*((volatile INT32U *) 0xD0200094))
#define R_MEM_M8_BUS_PRIORITY		(*((volatile INT32U *) 0xD0200098))
#define R_MEM_M9_BUS_PRIORITY		(*((volatile INT32U *) 0xD020009C))
#define R_MEM_M10_BUS_PRIORITY		(*((volatile INT32U *) 0xD02000A0))
#define R_MEM_M11_BUS_PRIORITY		(*((volatile INT32U *) 0xD02000A4))

#define R_MEM_BUS_MONITOR_EN		(*((volatile INT32U *) 0xD09001F0))
#define R_MEM_BUS_MONITOR_PERIOD	(*((volatile INT32U *) 0xD09001F4))
#define R_MEM_SDRAM_IDLE_RATE		(*((volatile INT32U *) 0xD09001F8))

#define R_MEM_M0_UTILIZATION		(*((volatile INT32U *) 0xD0900100))
#define R_MEM_M1_UTILIZATION		(*((volatile INT32U *) 0xD0900104))
#define R_MEM_M2_UTILIZATION		(*((volatile INT32U *) 0xD0900108))
#define R_MEM_M3_UTILIZATION		(*((volatile INT32U *) 0xD090010C))
#define R_MEM_M4_UTILIZATION		(*((volatile INT32U *) 0xD0900110))
#define R_MEM_M5_UTILIZATION		(*((volatile INT32U *) 0xD0900114))
#define R_MEM_M6_UTILIZATION		(*((volatile INT32U *) 0xD0900118))
#define R_MEM_M7_UTILIZATION		(*((volatile INT32U *) 0xD090011C))
#define R_MEM_M8_UTILIZATION		(*((volatile INT32U *) 0xD0900120))
#define R_MEM_M9_UTILIZATION		(*((volatile INT32U *) 0xD0900124))
#define R_MEM_M10_UTILIZATION		(*((volatile INT32U *) 0xD0900128))
#define R_MEM_M11_UTILIZATION		(*((volatile INT32U *) 0xD090012C))

#define R_MEM_M0_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900130))
#define R_MEM_M1_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900134))
#define R_MEM_M2_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900138))
#define R_MEM_M3_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD090013C))
#define R_MEM_M4_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900140))
#define R_MEM_M5_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900144))
#define R_MEM_M6_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900148))
#define R_MEM_M7_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD090014C))
#define R_MEM_M8_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900150))
#define R_MEM_M9_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900154))
#define R_MEM_M10_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD0900158))
#define R_MEM_M11_UTILIZATION_BYTE	(*((volatile INT32U *) 0xD090015C))



/******************************************************************************
 * Session: DMA controller SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: DMA controller SFR
 ******************************************************************************/

/******************************************************************************
 * DMA controller: 0xD0300000
 ******************************************************************************/
#define	R_DMA0_CTRL					(*((volatile INT32U *) 0xD0300000))
#define	R_DMA0_SRC_ADDR				(*((volatile INT32U *) 0xD0300004))
#define	R_DMA0_TAR_ADDR				(*((volatile INT32U *) 0xD0300008))
#define	R_DMA0_TX_COUNT				(*((volatile INT32U *) 0xD030000C))
#define	R_DMA0_SPRITE_SIZE			(*((volatile INT32U *) 0xD0300010))
#define	R_DMA0_TRANSPARENT			(*((volatile INT32U *) 0xD0300014))
#define	R_DMA0_MISC					(*((volatile INT32U *) 0xD0300018))

#define	R_DMA1_CTRL					(*((volatile INT32U *) 0xD0300040))
#define	R_DMA1_SRC_ADDR				(*((volatile INT32U *) 0xD0300044))
#define	R_DMA1_TAR_ADDR				(*((volatile INT32U *) 0xD0300048))
#define	R_DMA1_TX_COUNT				(*((volatile INT32U *) 0xD030004C))
#define	R_DMA1_SPRITE_SIZE			(*((volatile INT32U *) 0xD0300050))
#define	R_DMA1_TRANSPARENT			(*((volatile INT32U *) 0xD0300054))
#define	R_DMA1_MISC					(*((volatile INT32U *) 0xD0300058))

#define	R_DMA2_CTRL					(*((volatile INT32U *) 0xD0300080))
#define	R_DMA2_SRC_ADDR				(*((volatile INT32U *) 0xD0300084))
#define	R_DMA2_TAR_ADDR				(*((volatile INT32U *) 0xD0300088))
#define	R_DMA2_TX_COUNT				(*((volatile INT32U *) 0xD030008C))
#define	R_DMA2_SPRITE_SIZE			(*((volatile INT32U *) 0xD0300090))
#define	R_DMA2_TRANSPARENT			(*((volatile INT32U *) 0xD0300094))
#define	R_DMA2_MISC					(*((volatile INT32U *) 0xD0300098))

#define	R_DMA3_CTRL					(*((volatile INT32U *) 0xD03000C0))
#define	R_DMA3_SRC_ADDR				(*((volatile INT32U *) 0xD03000C4))
#define	R_DMA3_TAR_ADDR				(*((volatile INT32U *) 0xD03000C8))
#define	R_DMA3_TX_COUNT				(*((volatile INT32U *) 0xD03000CC))
#define	R_DMA3_SPRITE_SIZE			(*((volatile INT32U *) 0xD03000D0))
#define	R_DMA3_TRANSPARENT			(*((volatile INT32U *) 0xD03000D4))
#define	R_DMA3_MISC					(*((volatile INT32U *) 0xD03000D8))

#define	R_DMA_LINE_LEN				(*((volatile INT32U *) 0xD03001F0))
#define	R_DMA_DEVICE				(*((volatile INT32U *) 0xD03001F4))
#define	R_DMA_CEMODE				(*((volatile INT32U *) 0xD03001F8))
#define	R_DMA_INT					(*((volatile INT32U *) 0xD03001FC))

/******************************************************************************
 * Session: PPU/TFT/STN/TVE/CSI controller SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: PPU/TFT/STN/TVE/CSI controller SFR
 ******************************************************************************/

/******************************************************************************
 * PICTURE PROCESS UNIT(PPU) CONTROL REGISTERS
 ******************************************************************************/
#define R_PPU_TEXT3_X_POSITION    		(*((volatile INT32U *) 0xD0500000))	// TEXT3 X position register
#define R_PPU_TEXT3_Y_POSITION    		(*((volatile INT32U *) 0xD0500004)) // TEXT3 Y position register
#define R_PPU_TEXT3_X_OFFSET      		(*((volatile INT32U *) 0xD0500008)) // TEXT3 X offset register
#define R_PPU_TEXT3_Y_OFFSET      		(*((volatile INT32U *) 0xD050000C)) // TEXT3 Y offset register
#define R_PPU_TEXT3_ATTRIBUTE     		(*((volatile INT32U *) 0xD0500010)) // TEXT3 attribute register
#define R_PPU_TEXT3_CTRL       	    	(*((volatile INT32U *) 0xD0500014)) // TEXT3 control register
#define R_PPU_TEXT3_N_PTR         		(*((volatile INT32U *) 0xD0500018)) // TEXT3 number array pointer register
#define P_PPU_TEXT3_N_PTR         		((volatile INT32U *) 0xD0500018)    // TEXT3 number array pointer register
#define R_PPU_TEXT3_A_PTR         		(*((volatile INT32U *) 0xD050001C)) // TEXT3 attribute array pointer register
#define P_PPU_TEXT3_A_PTR         		((volatile INT32U *) 0xD050001C)    // TEXT3 attribute array pointer register

#define R_PPU_TEXT4_X_POSITION    		(*((volatile INT32U *) 0xD0500020)) // TEXT4 X position register
#define R_PPU_TEXT4_Y_POSITION    		(*((volatile INT32U *) 0xD0500024)) // TEXT4 Y position register
#define R_PPU_TEXT4_X_OFFSET      		(*((volatile INT32U *) 0xD0500028)) // TEXT4 X offset register
#define R_PPU_TEXT4_Y_OFFSET      		(*((volatile INT32U *) 0xD050002C)) // TEXT4 Y offset register
#define R_PPU_TEXT4_ATTRIBUTE     		(*((volatile INT32U *) 0xD0500030)) // TEXT4 attribute register
#define R_PPU_TEXT4_CTRL      		 	(*((volatile INT32U *) 0xD0500034)) // TEXT4 control register
#define R_PPU_TEXT4_N_PTR         		(*((volatile INT32U *) 0xD0500038)) // TEXT4 number array pointer register
#define P_PPU_TEXT4_N_PTR         		((volatile INT32U *) 0xD0500038)    // TEXT4 number array pointer register
#define R_PPU_TEXT4_A_PTR         		(*((volatile INT32U *) 0xD050003C)) // TEXT4 attribute array pointer register
#define P_PPU_TEXT4_A_PTR         		((volatile INT32U *) 0xD050003C)    // TEXT4 attribute array pointer register

#define R_PPU_TEXT1_X_POSITION    		(*((volatile INT32U *) 0xD0500040)) // TEXT1 X position register
#define R_PPU_TEXT1_Y_POSITION    		(*((volatile INT32U *) 0xD0500044)) // TEXT1 Y position register
#define R_PPU_TEXT1_X_OFFSET      		(*((volatile INT32U *) 0xD0500340)) // TEXT1 X offset register
#define R_PPU_TEXT1_Y_OFFSET      		(*((volatile INT32U *) 0xD0500344)) // TEXT1 Y offset register
#define R_PPU_TEXT1_ATTRIBUTE     		(*((volatile INT32U *) 0xD0500048)) // TEXT1 attribute register
#define R_PPU_TEXT1_CTRL       	    	(*((volatile INT32U *) 0xD050004C)) // TEXT1 control register
#define R_PPU_TEXT1_N_PTR         		(*((volatile INT32U *) 0xD0500050)) // TEXT1 number array pointer register
#define P_PPU_TEXT1_N_PTR         		((volatile INT32U *) 0xD0500050)    // TEXT1 number array pointer register
#define R_PPU_TEXT1_A_PTR         		(*((volatile INT32U *) 0xD0500054)) // TEXT1 attribute array pointer register
#define P_PPU_TEXT1_A_PTR         		((volatile INT32U *) 0xD0500054)    // TEXT1 attribute array pointer register

#define R_PPU_TEXT2_X_POSITION    		(*((volatile INT32U *) 0xD0500058)) // TEXT2 X position register
#define R_PPU_TEXT2_Y_POSITION    		(*((volatile INT32U *) 0xD050005C)) // TEXT2 Y position register
#define R_PPU_TEXT2_X_OFFSET      		(*((volatile INT32U *) 0xD0500350)) // TEXT2 X offset register
#define R_PPU_TEXT2_Y_OFFSET      		(*((volatile INT32U *) 0xD0500354)) // TEXT2 Y offset register
#define R_PPU_TEXT2_ATTRIBUTE     		(*((volatile INT32U *) 0xD0500060)) // TEXT2 attribute register
#define R_PPU_TEXT2_CTRL       		    (*((volatile INT32U *) 0xD0500064)) // TEXT2 control register
#define R_PPU_TEXT2_N_PTR         		(*((volatile INT32U *) 0xD0500068)) // TEXT2 number array pointer register
#define P_PPU_TEXT2_N_PTR         		((volatile INT32U *) 0xD0500068)    // TEXT2 number array pointer register
#define R_PPU_TEXT2_A_PTR         		(*((volatile INT32U *) 0xD050006C)) // TEXT2 attribute array pointer register
#define P_PPU_TEXT2_A_PTR         		((volatile INT32U *) 0xD050006C)    // TEXT2 attribute array pointer register

#define R_PPU_VCOMP_VALUE       		(*((volatile INT32U *) 0xD0500070)) // Vertical compression value register
#define R_PPU_VCOMP_OFFSET      		(*((volatile INT32U *) 0xD0500074)) // Vertical compression offset register
#define R_PPU_VCOMP_STEP        		(*((volatile INT32U *) 0xD0500078)) // Vertical compression step register


#define R_PPU_TEXT1_SEGMENT       		(*((volatile INT32U *) 0xD0500080)) // TEXT Layer 1 Segment Address Register
#define P_PPU_TEXT1_SEGMENT       		((volatile INT32U *) 0xD0500080)    // TEXT Layer 1 Segment Address Register
#define R_PPU_TEXT2_SEGMENT       		(*((volatile INT32U *) 0xD0500084)) // TEXT Layer 2 Segment Address Register
#define P_PPU_TEXT2_SEGMENT       		((volatile INT32U *) 0xD0500084)    // TEXT Layer 2 Segment Address Register
#define R_PPU_SPRITE_SEGMENT        	(*((volatile INT32U *) 0xD0500088)) // Sprite Segment Address Register
#define P_PPU_SPRITE_SEGMENT        	((volatile INT32U *) 0xD0500088)    // Sprite Segment Address Register
#define R_PPU_TEXT3_SEGMENT       		(*((volatile INT32U *) 0xD050008C)) // TEXT Layer 3 Segment Address Register
#define P_PPU_TEXT3_SEGMENT       		((volatile INT32U *) 0xD050008C)    // TEXT Layer 3 Segment Address Register
#define R_PPU_TEXT4_SEGMENT       		(*((volatile INT32U *) 0xD0500090)) // TEXT Layer 4 Segment Address Register
#define P_PPU_TEXT4_SEGMENT       		((volatile INT32U *) 0xD0500090)    // TEXT Layer 4 Segment Address Register

#define R_PPU_TEXT1_COSINE        		(*((volatile INT32U *) 0xD0500348)) // TEXT Layer 1 Cosine Register
#define P_PPU_TEXT1_COSINE        		((volatile INT32U *) 0xD0500348)    // TEXT Layer 1 Cosine Register
#define R_PPU_TEXT1_SINE          		(*((volatile INT32U *) 0xD050034C)) // TEXT Layer 1 Sine Register
#define P_PPU_TEXT1_SINE          		((volatile INT32U *) 0xD050034C)    // TEXT Layer 1 Sine Register
#define R_PPU_TEXT2_COSINE        		(*((volatile INT32U *) 0xD0500358)) // TEXT Layer 2 Cosine Register
#define P_PPU_TEXT2_COSINE        		((volatile INT32U *) 0xD0500358)    // TEXT Layer 2 Cosine Register
#define R_PPU_TEXT2_SINE          		(*((volatile INT32U *) 0xD050035C)) // TEXT Layer 2 Sine Register
#define P_PPU_TEXT2_SINE          		((volatile INT32U *) 0xD050035C)    // TEXT Layer 2 Sine Register
#define R_PPU_TEXT4_COSINE        		(*((volatile INT32U *) 0xD05000A0)) // TEXT Layer 4 Cosine Register
#define P_PPU_TEXT4_COSINE        		((volatile INT32U *) 0xD05000A0)    // TEXT Layer 4 Cosine Register
#define R_PPU_TEXT4_SINE          		(*((volatile INT32U *) 0xD05000A4)) // TEXT Layer 4 Sine Register
#define P_PPU_TEXT4_SINE          		((volatile INT32U *) 0xD05000A4)    // TEXT Layer 4 Sine Register

#define R_PPU_BLENDING          		(*((volatile INT32U *) 0xD05000A8)) // TEXT layers blending control register
#define R_PPU_FADE_CTRL         		(*((volatile INT32U *) 0xD05000C0)) // Fade effect control register
#define R_PPU_IRQTMV            		(*((volatile INT32U *) 0xD05000D8)) // Vertical hit IRQ control register
#define R_PPU_IRQTMH            		(*((volatile INT32U *) 0xD05000DC)) // Horizontal hit IRQ control register
#define R_PPU_LINE_COUNTER      		(*((volatile INT32U *) 0xD05000E0)) // TV line counter register
#define R_PPU_LIGHTPEN_CTRL     		(*((volatile INT32U *) 0xD05000E4)) // Light pen control register
#define R_PPU_PALETTE_CTRL   		    (*((volatile INT32U *) 0xD05000E8)) // Palette control register
#define R_PPU_LPHPOSITION       		(*((volatile INT32U *) 0xD05000F8)) // Ligth pen horizontal position register
#define R_PPU_LPVPOSITION       		(*((volatile INT32U *) 0xD05000FC)) // Ligth pen vertical position register

#define R_PPU_Y25D_COMPRESS        		(*((volatile INT32U *) 0xD0500104)) // Y compress parameter under 2.5D mode
#define R_PPU_SPRITE_CTRL        		(*((volatile INT32U *) 0xD0500108)) // Sprite control register

#define R_PPU_WINDOW0_X         		(*((volatile INT32U *) 0xD0500120)) // Window 1 X control register
#define R_PPU_WINDOW0_Y         		(*((volatile INT32U *) 0xD0500124)) // Window 1 Y control register
#define R_PPU_WINDOW1_X         		(*((volatile INT32U *) 0xD0500128)) // Window 2 X control register
#define R_PPU_WINDOW1_Y         		(*((volatile INT32U *) 0xD050012C)) // Window 2 Y control register
#define R_PPU_WINDOW2_X         		(*((volatile INT32U *) 0xD0500130)) // Window 3 X control register
#define R_PPU_WINDOW2_Y         		(*((volatile INT32U *) 0xD0500134)) // Window 3 Y control register
#define R_PPU_WINDOW3_X         		(*((volatile INT32U *) 0xD0500138)) // Window 4 X control register
#define R_PPU_WINDOW3_Y         		(*((volatile INT32U *) 0xD050013C)) // Window 5 Y control register

#define R_PPU_IRQ_EN        			(*((volatile INT32U *) 0xD0500188)) // PPU IRQ enable register
#define R_PPU_IRQ_STATUS    			(*((volatile INT32U *) 0xD050018C)) // PPU IRQ status register

#define R_PPU_SPRITE_DMA_SOURCE      	(*((volatile INT32U *) 0xD05001C0)) // PPU DMA source address register
#define R_PPU_SPRITE_DMA_TARGET      	(*((volatile INT32U *) 0xD05001C4)) // PPU DMA target address register
#define R_PPU_SPRITE_DMA_NUMBER      	(*((volatile INT32U *) 0xD05001C8)) // PPU DMA transfer number register
#define R_PPU_HB_CTRL					(*((volatile INT32U *) 0xD05001CC)) // Horizontal blank control
#define R_PPU_HB_GO						(*((volatile INT32U *) 0xD05001D0))	// Horizatal blank enable


#define R_TV_FBI_ADDR        			(*((volatile INT32U *) 0xD05001E0)) // Frame buffer address for TV
#define R_TFT_FBI_ADDR       			(*((volatile INT32U *) 0xD050033C)) // Frame buffer address for TFT-LCD
#define R_PPU_FBO_ADDR          		(*((volatile INT32U *) 0xD05001E8)) // Frame buffer address for PPU Output
#define R_PPU_FB_GO         			(*((volatile INT32U *) 0xD05001F0))
#define R_PPU_BLD_COLOR         		(*((volatile INT32U *) 0xD05001F4)) // Transparent color in RGB565 mode register
#define R_PPU_MISC	         			(*((volatile INT32U *) 0xD05001F8))	// PPU MISC Control
#define R_PPU_ENABLE        			(*((volatile INT32U *) 0xD05001FC)) // PPU enable register
#define R_FREE_SIZE        			    (*((volatile INT32U *) 0xD050036C)) // PPU Free Size Mode register

// Sprite virtual 3D
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

#define R_PPU_EXTENDSPRITE_CONTROL		(*((volatile INT32U *) 0xD0500334))	// Extend sprite control
#define R_PPU_EXTENDSPRITE_ADDR			(*((volatile INT32U *) 0xD0500338))	// Extend sprite start address
#define R_PPU_RGB_OFFSET				(*((volatile INT32U *) 0xD050037C))	// Special effect RGB offset

#define P_PPU_TEXT_H_OFFSET0      		((volatile INT32U *) 0xD0500400)    // Horizontal movement control RAM

#define P_PPU_TEXT_HCMP_VALUE0       	((volatile INT32U *) 0xD0500800)    // Horizontal compression control RAM

#define R_PPU_TEXT3_COS0          		(*((volatile INT32U *) 0xD0500800)) // TEXT3 2.5D cosine value register
#define P_PPU_TEXT3_COS0          		((volatile INT32U *) 0xD0500800)    // TEXT3 2.5D cosine array pointer
#define R_PPU_TEXT3_SIN0          		(*((volatile INT32U *) 0xD0500804)) // TEXT3 2.5D sine value register
#define P_PPU_TEXT3_SIN0          		((volatile INT32U *) 0xD0500804)    // TEXT3 2.5D sine array pointer

#define P_PPU_PALETTE_RAM0      		((volatile INT32U *) 0xD0501000)    // Palette RAM0 base
#define P_PPU_PALETTE_RAM1      		((volatile INT32U *) 0xD0501400)    // Palette RAM1 base
#define P_PPU_PALETTE_RAM2      		((volatile INT32U *) 0xD0501800)    // Palette RAM2 base
#define P_PPU_PALETTE_RAM3      		((volatile INT32U *) 0xD0501C00)    // Palette RAM3 base

#define P_PPU_SPRITE_ATTRIBUTE_BASE		((volatile INT32U *) 0xD0502000)    // Sprite attribute RAM base
#define P_PPU_SPRITE_EXTERN_ATTRIBUTE_BASE		((volatile INT32U *) 0xD0506000)    // Sprite exterd attribute RAM base 

/******************************************************************************
 * TV CONTROL REGISTERS
 ******************************************************************************/
#define R_TV_CTRL        		    (*((volatile INT32U *) 0xD05000F0))	// TV Control Register
#define R_TV_CTRL2        		  (*((volatile INT32U *) 0xD05000F4))	// TV Control2 Register
#define R_TV_SATURATION     		(*((volatile INT32U *) 0xD0500200)) // TV Saturation Control Register
#define R_TV_HUE            		(*((volatile INT32U *) 0xD0500204)) // TV Hue Control Register
#define R_TV_BRIGHTNESS     		(*((volatile INT32U *) 0xD0500208)) // TV Brightness Control Register
#define R_TV_SHARPNESS      		(*((volatile INT32U *) 0xD050020C)) // TV Sharpness Control Register
#define R_TV_Y_GAIN         		(*((volatile INT32U *) 0xD0500210)) // TV Y Gain Control Register
#define R_TV_Y_DELAY        		(*((volatile INT32U *) 0xD0500214)) // TV Y Delay Control Register
#define R_TV_V_POSITION     		(*((volatile INT32U *) 0xD0500218)) // TV Vertical Position Control Register
#define R_TV_H_POSITION     		(*((volatile INT32U *) 0xD050021C)) // TV Horizontal Position Control Register
#define R_TV_VIDEODAC       		(*((volatile INT32U *) 0xD0500220)) // TV Video DAC Control Register

/******************************************************************************
 * TFT CONTROL REGISTERS
 ******************************************************************************/
#define R_TFT_CTRL       	    	(*((volatile INT32U *) 0xD0500140))	// TFT Control Register
#define R_TFT_V_PERIOD      		(*((volatile INT32U *) 0xD0500144)) // TFT Vertical Period Control Register
#define R_TFT_VS_WIDTH      		(*((volatile INT32U *) 0xD0500148)) // TFT VSYNC Width Control Register
#define R_TFT_V_START       		(*((volatile INT32U *) 0xD050014C)) // TFT Vertical Start Position Control Register
#define R_TFT_V_END         		(*((volatile INT32U *) 0xD0500150)) // TFT Vertical End Position Control Register
#define R_TFT_H_PERIOD      		(*((volatile INT32U *) 0xD0500154)) // TFT Horizontal Period Control Register
#define R_TFT_HS_WIDTH      		(*((volatile INT32U *) 0xD0500158)) // TFT HSYNC Width Control Register
#define R_TFT_H_START       		(*((volatile INT32U *) 0xD050015C)) // TFT Horizontal Start Position Control Register
#define R_TFT_H_END         		(*((volatile INT32U *) 0xD0500160)) // TFT Horizontal End Position Control Register
#define R_TFT_LINE_RGB_ORDER 		(*((volatile INT32U *) 0xD0500164)) // TFT Line RGB Order Control Register
#define R_TFT_STATUS        		(*((volatile INT32U *) 0xD0500168)) // TFT Status Register
#define R_TFT_MEM_BUFF_WR      		(*((volatile INT32U *) 0xD050016C))
#define R_TFT_MEM_BUFF_RD      		(*((volatile INT32U *) 0xD0500170))
#define R_TFT_TE_CTRL        		(*((volatile INT32U *) 0xD0500180))
#define R_TFT_TE_HS_WIDTH        	(*((volatile INT32U *) 0xD0500184))
#define R_TFT_INT_EN        		(*((volatile INT32U *) 0xD0500188))
#define R_TFT_INT_CLR        		(*((volatile INT32U *) 0xD050018C))
#define R_TFT_VS_START       		(*((volatile INT32U *) 0xD05001B0))
#define R_TFT_VS_END        		(*((volatile INT32U *) 0xD05001B4))
#define R_TFT_HS_START        		(*((volatile INT32U *) 0xD05001B8))
#define R_TFT_HS_END        		(*((volatile INT32U *) 0xD05001BC))

#define R_TFT_TS_CKV                (*((volatile INT32U *) 0xD05003C0))
#define R_TFT_TW_CKV                (*((volatile INT32U *) 0xD05003C4))
#define R_TFT_TS_MISC               (*((volatile INT32U *) 0xD05003C8))
#define R_TFT_TS_POL                (*((volatile INT32U *) 0xD05003CC))
#define R_TFT_TS_STV                (*((volatile INT32U *) 0xD05003D0))
#define R_TFT_TW_STV                (*((volatile INT32U *) 0xD05003D4))
#define R_TFT_TS_STH                (*((volatile INT32U *) 0xD05003D8))
#define R_TFT_TW_STH                (*((volatile INT32U *) 0xD05003DC))
#define R_TFT_TS_OEV                (*((volatile INT32U *) 0xD05003E0))
#define R_TFT_TW_OEV                (*((volatile INT32U *) 0xD05003E4))
#define R_TFT_TS_LD                 (*((volatile INT32U *) 0xD05003E8))
#define R_TFT_TW_LD                 (*((volatile INT32U *) 0xD05003EC))
#define R_TFT_TAB0                  (*((volatile INT32U *) 0xD05003F0))
#define R_TFT_TAB1                  (*((volatile INT32U *) 0xD05003F4))
#define R_TFT_TAB2                  (*((volatile INT32U *) 0xD05003F8))
#define R_TFT_TAB3                  (*((volatile INT32U *) 0xD05003FC))

#define R_TFT_CLIP_V_START       	(*((volatile INT32U *) 0xD05003B0))
#define R_TFT_CLIP_V_END        	(*((volatile INT32U *) 0xD05003B4))
#define R_TFT_CLIP_H_START        	(*((volatile INT32U *) 0xD05003B8))
#define R_TFT_CLIP_H_END        	(*((volatile INT32U *) 0xD05003BC))

#define P_TFT_COLOR_MAP_BASE		((volatile INT32U *) 0xD0507000)    // Color mapping RAM base

/*****************************************************************************
* STN CONTROL REGISTERS
******************************************************************************/
#define R_STN_CTRL0				(*((volatile INT32U *) 0xD050017C))	// STN Control Register 0
#define R_STN_SEG				(*((volatile INT32U *) 0xD0500200))	// STN Segment Register
#define R_STN_COM				(*((volatile INT32U *) 0xD0500204))	// STN Column Register
#define R_STN_PIC_COM			(*((volatile INT32U *) 0xD0500208))	// STN Picture Column Register
#define R_STN_CPWAIT			(*((volatile INT32U *) 0xD050020C))	// STN CP Wait Register
#define R_STN_CTRL1				(*((volatile INT32U *) 0xD0500210))	// STN Control Register 1
#define R_STN_GTG_SEG			(*((volatile INT32U *) 0xD0500214))	// STN Global Timing Generator Segment Register
#define R_STN_GTG_COM			(*((volatile INT32U *) 0xD0500218))	// STN Global Timing Generator Column Register
#define R_STN_SEG_CLIP			(*((volatile INT32U *) 0xD050021C))	// STN Clipping Start Segment Register
#define R_STN_COM_CLIP			(*((volatile INT32U *) 0xD0500144))	// STN Clipping Start Column Register

/******************************************************************************
 * CMOS SENSOR INTERFACE (CSI) CONTROL REGISTERS
 ******************************************************************************/
#define P_CSI_MD_FBADDR    			((volatile INT32U *) 0xD0500254)    // CSI Motion Detect Buffer Start Address Register
#define P_CSI_MD_FBADDRH			((volatile INT32U *) 0xD0500298)
#define P_CSI_TG_FBSADDR	   		((volatile INT32U *) 0xD0500278)    // CSI Frame Buffer Start Address Register

#define R_CSI_TG_CTRL0 		  		(*((volatile INT32U *) 0xD0500240))	// CSI Timing Generator Control Register 1
#define R_CSI_TG_CTRL1   			(*((volatile INT32U *) 0xD0500244)) // CSI Timing Generator Control Register 2
#define R_CSI_TG_HLSTART   			(*((volatile INT32U *) 0xD0500248)) // CSI Horizontal Latch Start Register
#define R_CSI_TG_HEND      			(*((volatile INT32U *) 0xD050024C)) // CSI Horizontal End Register
#define R_CSI_TG_VL0START   		(*((volatile INT32U *) 0xD0500250)) // CSI Field 0 Vertical Start Register
#define R_CSI_TG_VEND   	   		(*((volatile INT32U *) 0xD0500258)) // CSI Vertical End Register
#define R_CSI_TG_HSTART	    		(*((volatile INT32U *) 0xD050025C)) // CSI Horizontal Start Register
#define R_CSI_MD_RGBL	     		(*((volatile INT32U *) 0xD0500260)) // CSI Motion Detect RGB/YUV Lo-Word Register
#define R_CSI_SEN_CTRL	     		(*((volatile INT32U *) 0xD0500264)) // CSI Attribute Control Register
#define R_CSI_TG_BSUPPER	   		(*((volatile INT32U *) 0xD0500268)) // CSI Blue Screen Upper Limit Control Register
#define R_CSI_TG_BSLOWER   			(*((volatile INT32U *) 0xD050026C)) // CSI Blue Screen Lower Limit Control Register
#define R_CSI_MD_RGBH	     		(*((volatile INT32U *) 0xD0500270)) // CSI Motion Detect RGB/YUV Hi-Word Register
#define R_CSI_MD_CTRL	    		(*((volatile INT32U *) 0xD0500274)) // CSI Motion Detect Control Register
#define R_CSI_TG_VL1START   		(*((volatile INT32U *) 0xD0500280)) // CSI Field 1 Vertical Start Register
#define R_CSI_TG_HWIDTH	    		(*((volatile INT32U *) 0xD0500284)) // CSI Horizontal Width Control Register
#define R_CSI_TG_VHEIGHT 	   		(*((volatile INT32U *) 0xD0500288)) // CSI Vertical Width Control Register
#define R_CSI_TG_CUTSTART   		(*((volatile INT32U *) 0xD050028C)) // CSI Cut Region Start Address Register
#define R_CSI_TG_CUTSIZE   			(*((volatile INT32U *) 0xD0500290)) // CSI Cut Size Register
#define R_CSI_TG_VSTART    			(*((volatile INT32U *) 0xD0500294)) // CSI Vertical Start Register
#define R_CSI_TG_HRATIO    			(*((volatile INT32U *) 0xD050029C)) // CSI Horizontal Compress Ratio Control Register
#define R_CSI_TG_VRATIO    			(*((volatile INT32U *) 0xD05002A0)) // CSI Vertical Compress Ratio Control Register
#define R_CSI_MD_HPOS 				(*((volatile INT32U *) 0xD05002A4)) // CSI Motion Detect Horizontal Hit Position Register
#define R_CSI_MD_VPOS 				(*((volatile INT32U *) 0xD05002A8)) // CSI Motion Detect Vertical Hit Position Register


/******************************************************************************
 * RAMDOM CONTROL REGISTERS
 ******************************************************************************/
#define	R_RANDOM0					(*((volatile INT32U *) 0xD0500380))
#define	R_RANDOM1					(*((volatile INT32U *) 0xD0500384))


/******************************************************************************
 * DEFLICKER INTERFACE CONTROL REGISTERS
 ******************************************************************************/
#define	R_DEFLICKER_CTRL			(*((volatile INT32U *) 0xD0800000)) // De-flicker control register
#define	R_DEFLICKER_INPTRL			(*((volatile INT32U *) 0xD0800004)) // De-flicker input address register
#define	R_DEFLICKER_OUTPTRL			(*((volatile INT32U *) 0xD0800008)) // De-flicker output address register
#define	R_DEFLICKER_PARA			(*((volatile INT32U *) 0xD080000C)) // De-flicker parameter register
#define	R_DEFLICKER_INT				(*((volatile INT32U *) 0xD080007C)) // De-flicker interrupt status register
/******************************************************************************
 * GTE CONTROL REGISTERS
 ******************************************************************************/
#define	R_GTE0_ACT_M4X4				(*((volatile INT32U *) 0xF6800000))
#define	R_GTE0_ACT_M3X3				(*((volatile INT32U *) 0xF6800004))
#define	R_GTE0_ACT_INNER			(*((volatile INT32U *) 0xF6800008))
#define	R_GTE0_ACT_OUTER			(*((volatile INT32U *) 0xF680000C))
#define	R_GTE1_ACT_M4X4				(*((volatile INT32U *) 0xF6800010))
#define	R_GTE1_ACT_M3X3				(*((volatile INT32U *) 0xF6800014))
#define	R_GTE1_ACT_INNER			(*((volatile INT32U *) 0xF6800018))
#define	R_GTE1_ACT_OUTER			(*((volatile INT32U *) 0xF680001C))

#define	R_GTE_A0					(*((volatile INT32U *) 0xF6004000))
#define	R_GTE_A1					(*((volatile INT32U *) 0xF6004004))
#define	R_GTE_A2					(*((volatile INT32U *) 0xF6004008))
#define	R_GTE_A3					(*((volatile INT32U *) 0xF600400C))
#define	R_GTE_A4					(*((volatile INT32U *) 0xF6004010))
#define	R_GTE_A5					(*((volatile INT32U *) 0xF6004014))
#define	R_GTE_A6					(*((volatile INT32U *) 0xF6004018))
#define	R_GTE_A7					(*((volatile INT32U *) 0xF600401C))
#define	R_GTE_A8					(*((volatile INT32U *) 0xF6004020))
#define	R_GTE_A9					(*((volatile INT32U *) 0xF6004024))
#define	R_GTE_AA					(*((volatile INT32U *) 0xF6004028))
#define	R_GTE_AB					(*((volatile INT32U *) 0xF600402C))
#define	R_GTE_AC					(*((volatile INT32U *) 0xF6004030))
#define	R_GTE_AD					(*((volatile INT32U *) 0xF6004034))
#define	R_GTE_AE					(*((volatile INT32U *) 0xF6004038))
#define	R_GTE_AF					(*((volatile INT32U *) 0xF600403C))
#define	R_GTE0_XI					(*((volatile INT32U *) 0xF6004040))
#define	R_GTE0_YI					(*((volatile INT32U *) 0xF6004044))
#define	R_GTE0_ZI					(*((volatile INT32U *) 0xF6004048))
#define	R_GTE0_WI					(*((volatile INT32U *) 0xF600404C))
#define	R_GTE1_XI					(*((volatile INT32U *) 0xF6004050))
#define	R_GTE1_YI					(*((volatile INT32U *) 0xF6004054))
#define	R_GTE1_ZI					(*((volatile INT32U *) 0xF6004058))
#define	R_GTE1_WI					(*((volatile INT32U *) 0xF600405C))
#define	R_GTE0_XO					(*((volatile INT32U *) 0xF6004060))
#define	R_GTE0_YO					(*((volatile INT32U *) 0xF6004064))
#define	R_GTE0_ZO					(*((volatile INT32U *) 0xF6004068))
#define	R_GTE0_WO					(*((volatile INT32U *) 0xF600406C))
#define	R_GTE1_XO					(*((volatile INT32U *) 0xF6004070))
#define	R_GTE1_YO					(*((volatile INT32U *) 0xF6004074))
#define	R_GTE1_ZO					(*((volatile INT32U *) 0xF6004078))
#define	R_GTE1_WO					(*((volatile INT32U *) 0xF600407C))

#define R_GTE_MODE					(*((volatile INT32U *) 0xF6004080))
#define R_GTE_FORMAT				(*((volatile INT32U *) 0xF6004084))
#define R_GTE0_OF					(*((volatile INT32U *) 0xF600408C))
#define R_GTE1_OF					(*((volatile INT32U *) 0xF600409C))
#define R_GTE_DIVA					(*((volatile INT32U *) 0xF60040A0))
#define R_GTE_DIVB					(*((volatile INT32U *) 0xF60040A4))
#define R_GTE_DIVOF					(*((volatile INT32U *) 0xF60040A8))
#define R_GTE_DIVO					(*((volatile INT32U *) 0xF60040AC))
#define R_GTE_DIVR					(*((volatile INT32U *) 0xF60040B0))



/******************************************************************************
 * Session: Scaler SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Scaler SFR
 ******************************************************************************/

/******************************************************************************
 * Scaler: 0xD0600000
 ******************************************************************************/
#define R_SCALER_CTRL				(*((volatile INT32U *) 0xD0600000))
#define R_SCALER_OUT_BOND_COLOR	    (*((volatile INT32U *) 0xD0600004))
#define R_SCALER_OUT_WIDTH			(*((volatile INT32U *) 0xD0600008))
#define R_SCALER_OUT_HEIGHT			(*((volatile INT32U *) 0xD060000C))
#define R_SCALER_X_FACTOR			(*((volatile INT32U *) 0xD0600010))
#define R_SCALER_Y_FACTOR			(*((volatile INT32U *) 0xD0600014))
#define R_SCALER_X_START			(*((volatile INT32U *) 0xD0600018))
#define R_SCALER_Y_START			(*((volatile INT32U *) 0xD060001C))
#define R_SCALER_IN_WIDTH			(*((volatile INT32U *) 0xD0600020))
#define R_SCALER_IN_HEIGHT			(*((volatile INT32U *) 0xD0600024))
#define R_SCALER_IN_Y_ADDR			(*((volatile INT32U *) 0xD0600028))
#define R_SCALER_IN_U_ADDR			(*((volatile INT32U *) 0xD060002C))
#define R_SCALER_IN_V_ADDR			(*((volatile INT32U *) 0xD0600030))
#define R_SCALER_OUT_Y_ADDR			(*((volatile INT32U *) 0xD0600034))
#define R_SCALER_OUT_U_ADDR			(*((volatile INT32U *) 0xD0600038))
#define R_SCALER_OUT_V_ADDR			(*((volatile INT32U *) 0xD060003C))
#define R_SCALER_CURRENT_LINE		(*((volatile INT32U *) 0xD0600040))
#define R_SCALER_A11                (*((volatile INT32U *) 0xD0600044))
#define R_SCALER_A12                (*((volatile INT32U *) 0xD0600048))
#define R_SCALER_A13                (*((volatile INT32U *) 0xD060004C))
#define R_SCALER_A21                (*((volatile INT32U *) 0xD0600050))
#define R_SCALER_A22                (*((volatile INT32U *) 0xD0600054))
#define R_SCALER_A23                (*((volatile INT32U *) 0xD0600058))
#define R_SCALER_A31                (*((volatile INT32U *) 0xD060005C))
#define R_SCALER_A32                (*((volatile INT32U *) 0xD0600060))
#define R_SCALER_A33                (*((volatile INT32U *) 0xD0600064))
#define R_SCALER_IN_VISIBLE_WIDTH	(*((volatile INT32U *) 0xD0600068))
#define R_SCALER_IN_VISIBLE_HEIGHT	(*((volatile INT32U *) 0xD060006C))
#define R_SCALER_OUT_X_OFFSET		(*((volatile INT32U *) 0xD0600070))
#define R_SCALER_LINE_BUFFER_ADDR	(*((volatile INT32U *) 0xD0600074))
#define R_SCALER_INT_FLAG			(*((volatile INT32U *) 0xD060007C))
#define R_SCALER_POST               (*((volatile INT32U *) 0xD0600080))

#define R_SCALER_MAXY				(*((volatile INT32U *) 0xD0600084))
#define R_SCALER_MINY				(*((volatile INT32U *) 0xD0600088))
#define R_SCALER_SUMY				(*((volatile INT32U *) 0xD060008C))
#define R_SCALER_MAXU				(*((volatile INT32U *) 0xD0600090))
#define R_SCALER_MINU				(*((volatile INT32U *) 0xD0600094))
#define R_SCALER_SUMU				(*((volatile INT32U *) 0xD0600098))
#define R_SCALER_MAXV				(*((volatile INT32U *) 0xD060009C))
#define R_SCALER_MINV				(*((volatile INT32U *) 0xD06000A0))
#define R_SCALER_SUMV				(*((volatile INT32U *) 0xD06000A4))

#define R_SCALER_Y_HIS0				(*((volatile INT32U *) 0xD06000C0))
#define R_SCALER_Y_HIS1				(*((volatile INT32U *) 0xD06000C4))
#define R_SCALER_Y_HIS2				(*((volatile INT32U *) 0xD06000C8))
#define R_SCALER_Y_HIS3				(*((volatile INT32U *) 0xD06000CC))
#define R_SCALER_Y_HIS4				(*((volatile INT32U *) 0xD06000D0))
#define R_SCALER_Y_HIS5				(*((volatile INT32U *) 0xD06000D4))
#define R_SCALER_Y_HIS6				(*((volatile INT32U *) 0xD06000D8))
#define R_SCALER_Y_HIS7				(*((volatile INT32U *) 0xD06000DC))
#define R_SCALER_Y_HIS8				(*((volatile INT32U *) 0xD06000E0))
#define R_SCALER_Y_HIS9				(*((volatile INT32U *) 0xD06000E4))
#define R_SCALER_Y_HIS10			(*((volatile INT32U *) 0xD06000E8))
#define R_SCALER_Y_HIS11			(*((volatile INT32U *) 0xD06000EC))
#define R_SCALER_Y_HIS12			(*((volatile INT32U *) 0xD06000F0))
#define R_SCALER_Y_HIS13			(*((volatile INT32U *) 0xD06000F4))
#define R_SCALER_Y_HIS14			(*((volatile INT32U *) 0xD06000F8))
#define R_SCALER_Y_HIS15			(*((volatile INT32U *) 0xD06000FC))

#define R_SCALER_STATUS_WIRTE1		(*((volatile INT32U *) 0xD0600100))
#define R_SCALER_STATUS_READ1		(*((volatile INT32U *) 0xD0600104))
#define R_SCALER_STATUS_READ2		(*((volatile INT32U *) 0xD0600108))

#define P_SCALER_Y_GAMMA_0          ((volatile INT32U *) 0xD0600400)
#define P_SCALER_Y_HISTOGRAM_0      ((volatile INT32U *) 0xD0600800)

/******************************************************************************
 * Session: Scaler 2 SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Scaler SFR
 ******************************************************************************/

/******************************************************************************
 * Scaler: 0xD1000000
 ******************************************************************************/

#define R_SCALER2_CTRL				(*((volatile INT32U *) 0xD1000000))
#define R_SCALER2_OUT_BOND_COLOR	    (*((volatile INT32U *) 0xD1000004))
#define R_SCALER2_OUT_WIDTH			(*((volatile INT32U *) 0xD1000008))
#define R_SCALER2_OUT_HEIGHT			(*((volatile INT32U *) 0xD100000C))
#define R_SCALER2_X_FACTOR			(*((volatile INT32U *) 0xD1000010))
#define R_SCALER2_Y_FACTOR			(*((volatile INT32U *) 0xD1000014))
#define R_SCALER2_X_START			(*((volatile INT32U *) 0xD1000018))
#define R_SCALER2_Y_START			(*((volatile INT32U *) 0xD100001C))
#define R_SCALER2_IN_WIDTH			(*((volatile INT32U *) 0xD1000020))
#define R_SCALER2_IN_HEIGHT			(*((volatile INT32U *) 0xD1000024))
#define R_SCALER2_IN_Y_ADDR			(*((volatile INT32U *) 0xD1000028))
#define R_SCALER2_OUT_Y_ADDR			(*((volatile INT32U *) 0xD1000034))
#define R_SCALER2_CURRENT_LINE		(*((volatile INT32U *) 0xD1000040))
#define R_SCALER2_IN_VISIBLE_WIDTH	(*((volatile INT32U *) 0xD1000068))
#define R_SCALER2_IN_VISIBLE_HEIGHT	(*((volatile INT32U *) 0xD100006C))
#define R_SCALER2_OUT_X_OFFSET		(*((volatile INT32U *) 0xD1000070))
#define R_SCALER2_LINE_BUFFER_ADDR	(*((volatile INT32U *) 0xD1000074))
#define R_SCALER2_INT_FLAG			(*((volatile INT32U *) 0xD100007C))
#define R_SCALER2_POST               (*((volatile INT32U *) 0xD1000080))

#define R_SCALER2_STATUS_WIRTE1		(*((volatile INT32U *) 0xD1000100))
#define R_SCALER2_STATUS_READ1		(*((volatile INT32U *) 0xD1000104))
#define R_SCALER2_STATUS_READ2		(*((volatile INT32U *) 0xD1000108))

/******************************************************************************
 * Session: JPEG SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: JPEG SFR
 ******************************************************************************/

/******************************************************************************
 * JPEG: 0xD0700000
 ******************************************************************************/
#define P_JPG_QUANT_LUM				((volatile INT32U *) 0xD0700000)
#define P_JPG_QUANT_CHROM			((volatile INT32U *) 0xD0700100)

#define R_JPG_QUANT_VALUE_ONE		(*((volatile INT32U *) 0xD0701204))
#define R_JPG_QUANT_SCALER			(*((volatile INT32U *) 0xD070120C))
#define R_JPG_JFIF					(*((volatile INT32U *) 0xD0701210))
#define R_JPG_TRUNCATE				(*((volatile INT32U *) 0xD0701214))
#define R_JPG_VLC_STUFF_BITS		(*((volatile INT32U *) 0xD0701218))
#define R_JPG_JFIF_FLAG				(*((volatile INT32U *) 0xD070121C))
#define R_JPG_RESTART_MCU_NUM		(*((volatile INT32U *) 0xD0701220))
#define R_JPG_PROGRESSIVE			(*((volatile INT32U *) 0xD0701238))
#define R_JPG_HSIZE					(*((volatile INT32U *) 0xD07012C4))
#define R_JPG_VSIZE					(*((volatile INT32U *) 0xD07012CC))
#define R_JPG_EXTENDED_HSIZE		(*((volatile INT32U *) 0xD07012D8))
#define R_JPG_EXTENDED_VSIZE		(*((volatile INT32U *) 0xD07012E0))
#define R_JPG_SCALER_IMAGE_SIZE		(*((volatile INT32U *) 0xD0702380))		// This register is used to notify scaler the image size when bypass scaler mode is used in JPEG engine
#define R_JPG_Y_FRAME_ADDR			(*((volatile INT32U *) 0xD0702380))
#define R_JPG_U_FRAME_ADDR			(*((volatile INT32U *) 0xD0702384))
#define R_JPG_V_FRAME_ADDR			(*((volatile INT32U *) 0xD0702388))
#define R_JPG_READ_CTRL				(*((volatile INT32U *) 0xD070238C))
#define R_JPG_CLIP_START_X			(*((volatile INT32U *) 0xD0702390))
#define R_JPG_CLIP_START_Y			(*((volatile INT32U *) 0xD0702394))
#define R_JPG_CLIP_WIDTH			(*((volatile INT32U *) 0xD0702398))
#define R_JPG_CLIP_HEIGHT			(*((volatile INT32U *) 0xD070239C))
#define R_JPG_BS_OFFSET				(*((volatile INT32U *) 0xD07023A0))
#define R_JPG_RESTART_MARKER_CNT	(*((volatile INT32U *) 0xD07023A4))
#define R_JPG_CTRL					(*((volatile INT32U *) 0xD07023C0))
#define R_JPG_VLC_TOTAL_LEN			(*((volatile INT32U *) 0xD07023C4))
#define R_JPG_VLC_ADDR				(*((volatile INT32U *) 0xD07023CC))
#define R_JPG_RING_FIFO				(*((volatile INT32U *) 0xD07023D4))
#define R_JPG_IMAGE_SIZE			(*((volatile INT32U *) 0xD07023D8))
#define R_JPG_RESET					(*((volatile INT32U *) 0xD07023DC))
#define R_JPG_INT_CTRL				(*((volatile INT32U *) 0xD07023E0))
#define R_JPG_INT_FLAG				(*((volatile INT32U *) 0xD07023E4))
#define R_JPG_ENCODE_VLC_CNT		(*((volatile INT32U *) 0xD07023F4))

#define P_JPG_DC_LUM_MINCODE		((volatile INT32U *) 0xD0703000)
#define P_JPG_DC_LUM_VALPTR			((volatile INT32U *) 0xD0703040)
#define P_JPG_DC_LUM_HUFFVAL		((volatile INT32U *) 0xD0703080)

#define P_JPG_DC_CHROM_MINCODE		((volatile INT32U *) 0xD0703100)
#define P_JPG_DC_CHROM_VALPTR		((volatile INT32U *) 0xD0703140)
#define P_JPG_DC_CHROM_HUFFVAL		((volatile INT32U *) 0xD0703180)

#define P_JPG_AC_LUM_MINCODE		((volatile INT32U *) 0xD0703200)
#define P_JPG_AC_LUM_VALPTR			((volatile INT32U *) 0xD0703240)

#define P_JPG_AC_CHROM_MINCODE		((volatile INT32U *) 0xD0703280)
#define P_JPG_AC_CHROM_VALPTR		((volatile INT32U *) 0xD07032C0)

#define P_JPG_AC_LUM_HUFFVAL		((volatile INT32U *) 0xD0704000)
#define P_JPG_AC_CHROM_HUFFVAL		((volatile INT32U *) 0xD0704400)

/******************************************************************************
 * Session: Cache controller SFR
 * Layer: Driver Layer 1
 * Date: 2008/01/25
 * Note: Cache controller SFR
 ******************************************************************************/

/******************************************************************************
 * Cache controller: 0xFF000000
 ******************************************************************************/
#define P_CACHE_VALID_LOCK_TAG		((volatile INT32U *) 0xF7000000)
#define R_CACHE_CTRL				(*((volatile INT32U *) 0xFF000000))
#define R_CACHE_CFG					(*((volatile INT32U *) 0xFF000004))
#define R_CACHE_INVALID_LINE		(*((volatile INT32U *) 0xFF000008))
#define R_CACHE_LOCKDOWN			(*((volatile INT32U *) 0xFF00000C))
#define R_CACHE_DRAIN_WRITE_BUFFER	(*((volatile INT32U *) 0xFF000010))
#define R_CACHE_DRAIN_LINE			(*((volatile INT32U *) 0xFF000014))
#define R_CACHE_INVALID_BANK		(*((volatile INT32U *) 0xFF000018))
#define R_CACHE_INVALID_RANGE_SEZE	(*((volatile INT32U *) 0xFF00001C))
#define R_CACHE_INVALID_RANGE_ADDR	(*((volatile INT32U *) 0xFF000020))
#define R_CACHE_ACCESS_COUNT		(*((volatile INT32U *) 0xFF000040))
#define R_CACHE_HIT_COUNT			(*((volatile INT32U *) 0xFF000044))


/******************************************************************************
 * Session: SPU control SFR
 * Layer: Driver Layer 1
 * Date: 2008/08/08
 * Note: SPU control SFR
 ******************************************************************************/

/******************************************************************************
 * SPU control: 0xD0400000
 ******************************************************************************/
#define P_SPU_BASE_ADDR				0xD0400000
#define P_SPU_CH_EN					((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E00))	// channel 0~15 enable
#define P_SPU_MAIN_VOLUME			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E04))	// main volume
#define P_SPU_CH_FIQ_EN				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E08))	// channel 0~15 FIQ enable
#define P_SPU_CH_FIQ_STATUS			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E0C))	// channel 0~15 FIQ status
#define P_SPU_BEAT_BASE_COUNTER		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E10))	// beat base counter
#define P_SPU_BEAT_COUNTER			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E14))	// beat counter
#define P_SPU_ENV_CLK_CH0_3			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E18))	// channel 0~3 envelope interval selection
#define P_SPU_ENV_CLK_CH4_7			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E1C))	// channel 4~7 envelope interval selection
#define P_SPU_ENV_CLK_CH8_11		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E20))	// channel 8~11 envelope interval selection
#define P_SPU_ENV_CLK_CH12_15		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E24))	// channel 12~15 envelope interval selection
#define P_SPU_ENV_RAMP_DOWN			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E28))	// channel 0~15 envelope fast ramp down
#define P_SPU_CH_STOP_STATUS		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E2C))	// channel 0~15 stop channel status
#define P_SPU_CH_ZC_ENABLE			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E30))	// channel 0~15 zero crossing enable
#define P_SPU_CONTROL_FLAG			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E34))	// SPU control flags
#define P_SPU_COMPRESSOR_CTRL		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E38))	// compressor control
#define P_SPU_CH_STATUS				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E3C))	// channel 0~15 status
#define P_SPU_WAVE_IN_LEFT			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E40))	// left channel mixer input
#define P_SPU_WAVE_IN_RIGHT			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E44))	// right channel mixer input
#define P_SPU_WAVE_OUT_LEFT			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E48))	// wave output left of 32 channel + software channel
#define P_SPU_WAVE_OUT_RIGHT		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E4C))	// wave output right of 32 channel + software channel
#define P_SPU_CH_REPEAT_EN			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E50))	// channel 0~15 repeat enable control
#define P_SPU_CH_ENV_MODE			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E54))	// channel 0~15 envelope mode
#define P_SPU_CH_TONE_RELEASE		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E58))	// channel 0~15 tone release control
#define P_SPU_CH_IRQ_STATUS			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E5C))	// channel 0~15 envelope IRQ status
#define P_SPU_CH_PITCH_BEND_EN		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E60))	// channel 0~15 pitch bend enable
#define P_SPU_ATTACK_RELEASE_TIME	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E68))	// attack/release time control
#define P_SPU_BENK_ADDR				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E7C))	// wave table's bank address

#define P_SPU_CH_EN_HIGH			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E80))	// channel 16~31 enable
#define P_SPU_CH_FIQ_EN_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E88))	// channel 16~31 FIQ enable
#define P_SPU_CH_FIQ_STATUS_HIGH	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E8C))	// channel 16~31 FIQ status
#define P_SPU_POST_WAVE_CTRL		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E94))	// post wave counter and control
#define P_SPU_ENV_CLK_CH16_19		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E98))	// channel 16~19 envelope interval selection
#define P_SPU_ENV_CLK_CH20_23		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0E9C))	// channel 20~23 envelope interval selection
#define P_SPU_ENV_CLK_CH24_27		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EA0))	// channel 24~27 envelope interval selection
#define P_SPU_ENV_CLK_CH28_31		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EA4))	// channel 28~31 envelope interval selection
#define P_SPU_ENV_RAMP_DOWN_HIGH	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EA8))	// channel 16~31 envelope fast ramp down
#define P_SPU_CH_ZC_ENABLE_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EB0))	// channel 16~31 zero crossing enable
#define P_SPU_CH_STOP_STATUS_HIGH	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EAC))	// channel 16~31 stop channel status
#define P_SPU_CH_STATUS_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EBC))	// channel 16~31 status
#define P_SPU_POST_WAVE_OUT_LEFT	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EC8))	// wave output left of 32 channel
#define P_SPU_POST_WAVE_OUT_RIGHT	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0ECC))	// wave output right of 32 channel
#define P_SPU_CH_REPEAT_EN_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0ED0))	// channel 16~31 repeat enable control
#define P_SPU_CH_ENV_MODE_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0ED4))	// channel 16~31 envelope mode
#define P_SPU_CH_TONE_RELEASE_HIGH	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0ED8))	// channel 16~31 tone release control
#define P_SPU_CH_IRQ_STATUS_HIGH	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EDC))	// channel 16~31 envelope IRQ status
#define P_SPU_CH_PITCH_BEND_EN_HIGH	((volatile INT32U *) (P_SPU_BASE_ADDR + 0x0EE0))	// channel 16~31 pitch bend enable

/******************************************************************************
 * SPU channel 0 internal SRAM
 ******************************************************************************/
#define P_SPU_SRAM_BASE				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1000))
#define P_SPU_WAVE_ADDR_LOW			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1000))
#define P_SPU_MODE					((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1004))
#define P_SPU_LOOP_ADDR				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1008))
#define P_SPU_PAN_VELOCITY			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x100C))
#define P_SPU_ENVELOPE_0			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1010))
#define P_SPU_ENVELOPE_DATA			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1014))
#define P_SPU_ENVELOPE_1			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1018))
#define P_SPU_ENV_ADDR_HIGH			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x101C))
#define P_SPU_ENV_ADDR_LOW			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1020))
#define P_SPU_WAVE_DATA_0			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1024))
#define P_SPU_LOOP_CTRL				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1028))
#define P_SPU_WAVE_DATA				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x102C))
#define P_SPU_ADPCM_SEL				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1034))
#define P_SPU_WL_ADDR_HIGH			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1038))
#define P_SPU_WAVE_ADDR_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x103C))

#define P_SPU_PHASE_HIGH			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1800))
#define P_SPU_PHASE_ACC_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1804))
#define P_SPU_TARGET_PHASE_HIGH		((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1808))
#define P_SPU_RAMP_DOWN_CLK			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x180C))
#define P_SPU_PHASE					((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1810))
#define P_SPU_PHASE_ACC				((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1814))
#define P_SPU_TARGET_PHASE			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x1818))
#define P_SPU_PHASE_CTRL			((volatile INT32U *) (P_SPU_BASE_ADDR + 0x181C))

#define P_SPU_DMADATAL				((volatile INT32U *)(0xC0110000))
#define P_SPU_DMADATAR				((volatile INT32U *)(0xC0110004))


//#if SUPPORT_AI_FM_MODE == CUSTOM_ON
//Daniel 07/03
/******************************************************************************
 * I2C control 0xC00B0000
 ******************************************************************************/
#define	P_I2C_Ctrl				(*((volatile INT32U *) 0xC00B0000))
#define P_I2C_Ctrl_Flag			(*((volatile INT32U *) 0xC00B0004))
#define P_I2C_DEV_ID			(*((volatile INT32U *) 0xC00B0008))
#define P_I2C_ADDR				(*((volatile INT32U *) 0xC00B000C))
#define P_I2C_AD_NUM			(*((volatile INT32U *) 0xC00B0010))
#define P_I2C_RDATA				(*((volatile INT32U *) 0xC00B0014))

//#endif 		//#if SUPPORT_AI_FM_MODE == CUSTOM_ON

/******************************************************************************
 * Analog TFT CONTROL REGISTERS
 ******************************************************************************/
#define R_ANALOG_TFT_CTRL_L       	    	(*((volatile INT32U *) 0xC0130080))
#define R_ANALOG_TFT_CTRL_H       	    	(*((volatile INT32U *) 0xC0130084))
#define R_ANALOG_TFT_CTRL2_L       	    	(*((volatile INT32U *) 0xC0130088))
#define R_ANALOG_TFT_CTRL2_H       	    	(*((volatile INT32U *) 0xC013008C))
#define R_ANALOG_TFT_TS_STH_L       	    (*((volatile INT32U *) 0xC0130090))
#define R_ANALOG_TFT_TS_STH_H       	    (*((volatile INT32U *) 0xC0130094))
#define R_ANALOG_TFT_H_START_L       	    (*((volatile INT32U *) 0xC0130098))
#define R_ANALOG_TFT_H_START_H       	    (*((volatile INT32U *) 0xC013009C))
#define R_ANALOG_TFT_V_START       	        (*((volatile INT32U *) 0xC01300A0))
#define R_ANALOG_TFT_TW_STH         	    (*((volatile INT32U *) 0xC01300A4))
#define R_ANALOG_TFT_T_HS2OEH       	    (*((volatile INT32U *) 0xC01300A8))
#define R_ANALOG_TFT_TW_OEH         	    (*((volatile INT32U *) 0xC01300AC))
#define R_ANALOG_TFT_T_HS2CKV       	    (*((volatile INT32U *) 0xC01300B0))
#define R_ANALOG_TFT_TW_CKV       	        (*((volatile INT32U *) 0xC01300B4))
#define R_ANALOG_TFT_T_HS2OEV       	    (*((volatile INT32U *) 0xC01300B8))
#define R_ANALOG_TFT_TW_OEV         	    (*((volatile INT32U *) 0xC01300BC))
#define R_ANALOG_TFT_TS_STV         	    (*((volatile INT32U *) 0xC01300C0))
#define R_ANALOG_TFT_CLK_DLY          	    (*((volatile INT32U *) 0xC01300C4))
#define R_ANALOG_TFT_AG            	        (*((volatile INT32U *) 0xC01300C8))
#define R_ANALOG_TFT_TS_POL         	    (*((volatile INT32U *) 0xC01300CC))
#define R_ANALOG_TFT_AG_H_START            	(*((volatile INT32U *) 0xC0130100))
#define R_ANALOG_TFT_AG_V_START            	(*((volatile INT32U *) 0xC0130104))

/******************************************************************************
 *Sensor FIFO Mode for GPL326xx
 ******************************************************************************/  
#define R_TGR_IRQ_EN				(*((volatile INT32U *) 0xD050023C)) 
#define R_TGR_IRQ_STATUS			(*((volatile INT32U *) 0xD0500238))
#define P_CSI_TG_FBSADDR_B	   		((volatile INT32U *) 0xD05002AC) 

/******************************************************************************
 *IR RX for GPL326xx
 ******************************************************************************/
#define R_IR_RX_CTRL          		(*((volatile INT32U *) 0xC0140000))
#define R_IR_RX_DATA    		    (*((volatile INT32U *) 0xC0140004))
#define R_IR_RX_PWR       		    (*((volatile INT32U *) 0xC0140008))

/******************************************************************************
 *MIC for GPL326xx
 ******************************************************************************/
#define P_MIC_ASADC_DATA       		((volatile INT32U *) 0xC00C0050)
#define R_MIC_SETUP					(*((volatile INT32U *) 0xC00C0040))
#define R_MIC_READY					(*((volatile INT32U *) 0xC00C0044))
#define R_MIC_ASADC_CTRL			(*((volatile INT32U *) 0xC00C004C))
#define R_MIC_ASADC_DATA			(*((volatile INT32U *) 0xC00C0050))
#define R_MIC_SH_WAIT				(*((volatile INT32U *) 0xC00C005C))
#define R_MIC_PGA_GAIN       		(*((volatile INT32U *) 0xC00C006C))

/******************************************************************************
 *Digital AGC for GPL326xxB
 ******************************************************************************/
#define R_DAGC_CTRL					(*((volatile INT32U *) 0xC00C0070))
#define R_DAGC_TIME					(*((volatile INT32U *) 0xC00C0074))
#define R_DAGC_ENABLE				(*((volatile INT32U *) 0xC00C0078))
#define R_DAGC_STATUS				(*((volatile INT32U *) 0xC00C007C))

/******************************************************************************
 * MPEG4 control: 0xD0C00000
 ******************************************************************************/
/////////////////////////////////////////////////////////////////////////////////
// MPEG Engine Register Definition 
/////////////////////////////////////////////////////////////////////////////////
#define R_MP4_MJWIDTH_L	        (*((volatile INT32U *) 0xD0C00000))
#define R_MP4_MJWIDTH_H  		(*((volatile INT32U *) 0xD0C00004))
#define R_MP4_MJHEIGHT_L        (*((volatile INT32U *) 0xD0C00008))
#define R_MP4_MJHEIGHT_H        (*((volatile INT32U *) 0xD0C0000C))

#define R_MP4_MJHOFFSET_L       (*((volatile INT32U *) 0xD0C00010))
#define R_MP4_MJHOFFSET_H       (*((volatile INT32U *) 0xD0C00014))	
#define R_MP4_MJVOFFSET_L	    (*((volatile INT32U *) 0xD0C00018))
#define R_MP4_MJVOFFSET_H       (*((volatile INT32U *) 0xD0C0001C))

#define R_MP4_VLCOFFADDRL	    (*((volatile INT32U *) 0xD0C00020))
#define R_MP4_VLCOFFADDRM	    (*((volatile INT32U *) 0xD0C00024))
#define R_MP4_VLCOFFADDRH	    (*((volatile INT32U *) 0xD0C00028))
	                              
#define R_MP4_TMBOFFADDRL		(*((volatile INT32U *) 0xD0C00040))   
#define R_MP4_TMBOFFADDRM	    (*((volatile INT32U *) 0xD0C00044))	
#define R_MP4_TMBOFFADDR	    (*((volatile INT32U *) 0xD0C00048))	
#define R_MP4_YUVSEL		    (*((volatile INT32U *) 0xD0C0004C))

#define R_MP4_CTRL		    	(*((volatile INT32U *) 0xD0C00050))		
#define R_MP4_SETTING		    (*((volatile INT32U *) 0xD0C00054))		
#define R_MP4_HSFACTOR		    (*((volatile INT32U *) 0xD0C00058))	
#define R_MP4_VSFACTOR		    (*((volatile INT32U *) 0xD0C0005C))	
	
#define R_MP4_GOP_VALUE		    (*((volatile INT32U *) 0xD0C00060))		
#define R_MP4_SET_EN			(*((volatile INT32U *) 0xD0C00064))		
#define R_MP4_VARDTHRL		    (*((volatile INT32U *) 0xD0C00068))	
#define R_MP4_VARDTHRM		    (*((volatile INT32U *) 0xD0C0006C))	

#define R_MP4_INIAVGACT		    (*((volatile INT32U *) 0xD0C00070))	
#define R_MP4_AVGACTWEIL	    (*((volatile INT32U *) 0xD0C00078))	
#define R_MP4_AVGACTWEIM	    (*((volatile INT32U *) 0xD0C0007C))	

#define R_MP4_RESET			    (*((volatile INT32U *) 0xD0C00080))		
#define R_MP4_BUF_CTRL	   		(*((volatile INT32U *) 0xD0C00084))		
#define R_MP4_IP_FRAME          (*((volatile INT32U *) 0xD0C00088))
#define R_MP4_TBL		   		(*((volatile INT32U *) 0xD0C0008C))
		
#define R_MP4_SOFCTRL	   		(*((volatile INT32U *) 0xD0C000A0))			
#define R_MP4_GOPCNTMONITOR	   	(*((volatile INT32U *) 0xD0C000A4))    	
#define R_MP4_AVGACT	   		(*((volatile INT32U *) 0xD0C000A8))			
	
#define R_MP4_HSFINI			(*((volatile INT32U *) 0xD0C000C0)) 			
#define R_MP4_VSFINI	   		(*((volatile INT32U *) 0xD0C000C4))	
#define R_MP4_YUV_MODE	   		(*((volatile INT32U *) 0xD0C000C8))
#define R_MP4_MCMEHSFL	   		(*((volatile INT32U *) 0xD0C000CC))	

#define R_MP4_INT_MASK		   	(*((volatile INT32U *) 0xD0C000D0))			
#define R_MP4_INT_STATUS	   	(*((volatile INT32U *) 0xD0C000D4))
#define R_MP4_DISGATED 			(*((volatile INT32U *) 0xD0C000D8))

#define R_MP4_VLC_A_SIZE_L	   	(*((volatile INT32U *) 0xD0C000DC)) 		
#define R_MP4_VLC_A_SIZE_M	   	(*((volatile INT32U *) 0xD0C000E0)) 		
#define R_MP4_VLC_A_SIZE_H	   	(*((volatile INT32U *) 0xD0C000E4)) 		
#define R_MP4_VLC_B_SIZE_L	   	(*((volatile INT32U *) 0xD0C000E8)) 		
#define R_MP4_VLC_B_SIZE_M	   	(*((volatile INT32U *) 0xD0C000EC)) 
#define R_MP4_VLC_B_SIZE_H	   	(*((volatile INT32U *) 0xD0C000F0)) 

#define R_MP4_DEC_WIDTH_L	   	(*((volatile INT32U *) 0xD0C00100)) 					
#define R_MP4_DEC_WIDTH_H	   	(*((volatile INT32U *) 0xD0C00104)) 					
#define R_MP4_DEC_HEIGHT_L   	(*((volatile INT32U *) 0xD0C00108)) 				
#define R_MP4_DEC_HEIGHT_H	   	(*((volatile INT32U *) 0xD0C0010C)) 
				
#define R_MP4_DVLCOFFADDRL 		(*((volatile INT32U *) 0xD0C00110)) 		 	
#define R_MP4_DVLCOFFADDRM 		(*((volatile INT32U *) 0xD0C00114)) 		 	
#define R_MP4_DVLCOFFADDRH 		(*((volatile INT32U *) 0xD0C00118)) 
#define R_MP4_LASTMEMADDRL  	(*((volatile INT32U *) 0xD0C0011C))		
#define R_MP4_LASTMEMADDRM  	(*((volatile INT32U *) 0xD0C00120))	
#define R_MP4_LASTMEMADDRH 		(*((volatile INT32U *) 0xD0C00124))  
#define R_MP4_LASTLENGTH  		(*((volatile INT32U *) 0xD0C00128)) 
 	
#define R_MP4_MESRASMCE_SEL  	(*((volatile INT32U *) 0xD0C0012C))	
#define R_MP4_MCSRASMCE_SEL  	(*((volatile INT32U *) 0xD0C00130))		
#define R_MP4_ACCEN			  	(*((volatile INT32U *) 0xD0C00134))	 

/////////////////////////////////////////////////////////////////////////////////
// Texture Engine Register Definition
/////////////////////////////////////////////////////////////////////////////////
// reg0100 ~ reg013f for Qtable1
// reg0140 ~ reg017f for Qtable2
#define R_MP4_QTABLE1_STR  		(*((volatile INT32U *) 0xD0C00400))		
#define R_MP4_QTABLE2_STR  		(*((volatile INT32U *) 0xD0C00500)) 	
#define R_MP4_QTBLSET  			(*((volatile INT32U *) 0xD0C00604))  
#define R_MP4_QSRAMEN  			(*((volatile INT32U *) 0xD0C00608)) 
#define R_MP4_ENTHUMB  			(*((volatile INT32U *) 0xD0C0060C))  
#define R_MP4_JFIF	  			(*((volatile INT32U *) 0xD0C00610))	
#define R_MP4_TRUNTRES 			(*((volatile INT32U *) 0xD0C00614))
#define R_MP4_VLCBIT  			(*((volatile INT32U *) 0xD0C00618))
#define R_MP4_DECMPFF 			(*((volatile INT32U *) 0xD0C0061C))
#define R_MP4_RESTARTMCUL		(*((volatile INT32U *) 0xD0C00620))
#define R_MP4_RESTARTMCU		(*((volatile INT32U *) 0xD0C00624))

#define R_MP4_I_FRAME_QSCALE    (*((volatile INT32U *) 0xD0C00640))   		
#define R_MP4_P_FRAME_QSCALE	(*((volatile INT32U *) 0xD0C00644))
#define R_MP4_MATCHCNT			(*((volatile INT32U *) 0xD0C00648))	
#define R_MP4_MATCHCODE0		(*((volatile INT32U *) 0xD0C0064C))	
#define R_MP4_MATCHCODE1		(*((volatile INT32U *) 0xD0C00650))	
#define R_MP4_MATCHCODE2		(*((volatile INT32U *) 0xD0C00654))	
#define R_MP4_MATCHCODE3		(*((volatile INT32U *) 0xD0C00658))	
#define R_MP4_OFFSET			(*((volatile INT32U *) 0xD0C0065C))

#define R_MP4_VOPTimeIncMode	(*((volatile INT32U *) 0xD0C00660))
#define R_MP4_MS_CNT			(*((volatile INT32U *) 0xD0C00664))		
#define R_MP4_MSPlusCntEn		(*((volatile INT32U *) 0xD0C00668))	
#define R_MP4_VOPTimeIncRes0	(*((volatile INT32U *) 0xD0C00680))
#define R_MP4_VOPTimeIncRes1	(*((volatile INT32U *) 0xD0C00684))
#define R_MP4_VOPTimeInc0		(*((volatile INT32U *) 0xD0C00688))	
#define R_MP4_VOPTimeInc1		(*((volatile INT32U *) 0xD0C0068C))
	
#define R_MP4_VOP_TIME_INC_LEN	(*((volatile INT32U *) 0xD0C00690))

#define R_MP4_REACT0			(*((volatile INT32U *) 0xD0C00698))			
#define R_MP4_REACT1			(*((volatile INT32U *) 0xD0C0069C))			
#define R_MP4_D0_IFRAME0		(*((volatile INT32U *) 0xD0C006A0))		
#define R_MP4_D0_IFRAME1		(*((volatile INT32U *) 0xD0C006A4))		
#define R_MP4_PreIframeMBBytes0	(*((volatile INT32U *) 0xD0C006A8))
#define R_MP4_PreIframeMBBytes1	(*((volatile INT32U *) 0xD0C006AC))
#define R_MP4_PrePframeMBBytes0	(*((volatile INT32U *) 0xD0C006B0))
#define R_MP4_PrePframeMBBytes1	(*((volatile INT32U *) 0xD0C006B4))
#define R_MP4_QScaleUpperB		(*((volatile INT32U *) 0xD0C006B8))	
#define R_MP4_QScaleLowerB		(*((volatile INT32U *) 0xD0C006BC))	
#define R_MP4_D0_Pframe0		(*((volatile INT32U *) 0xD0C006C0))	
#define R_MP4_D0_Pframe1		(*((volatile INT32U *) 0xD0C006C4))	

#define R_VLCSIZE_LOW           (*((volatile INT32U *) 0xD0C006C8))
#define R_VLCSIZE_MID           (*((volatile INT32U *) 0xD0C006CC))
#define R_VLCSIZE_HIGH			(*((volatile INT32U *) 0xD0C006D0))

#define R_MP4_QSUM0			    (*((volatile INT32U *) 0xD0C006D4))		
#define R_MP4_QSUM1			    (*((volatile INT32U *) 0xD0C006D8))	
#define R_MP4_REACT2			(*((volatile INT32U *) 0xD0C006DC))
#define R_MP4_D0_IFRAME2		(*((volatile INT32U *) 0xD0C006E0))	
#define R_MP4_D0_Pframe2		(*((volatile INT32U *) 0xD0C006E4))	
#define R_MP4_MVLCBIT			(*((volatile INT32U *) 0xD0C006E8)) 
#define R_MP4_TIMEBASE_ERR		(*((volatile INT32U *) 0xD0C006EC))
#define R_MP4_ResetVedioConMode (*((volatile INT32U *) 0xD0C006F0))
		
#define R_MP4_SRAM_CS_N			(*((volatile INT32U *) 0xD0C006F8))   

#define R_MP4_SRAM_CS			(*((volatile INT32U *) 0xD0C00700))    	
#define R_MP4_CPURDQ_CSSEL		(*((volatile INT32U *) 0xD0C00704))	
#define R_MP4_DATAPartition		(*((volatile INT32U *) 0xD0C00708))	
#define R_MP4_ReSyncSolve		(*((volatile INT32U *) 0xD0C0070C))	
#define R_MP4_VLDSize0			(*((volatile INT32U *) 0xD0C00710)) 
#define R_MP4_VLDSize1			(*((volatile INT32U *) 0xD0C00714)) 
#define R_MP4_VLDSize2			(*((volatile INT32U *) 0xD0C00718))

#define R_MP4_SRAM_SEL_PAGE		(*((volatile INT32U *) 0xD0C00780))	
#define R_MP4_TSelect			(*((volatile INT32U *) 0xD0C00784))

#define R_MP4_BlockEnd			(*((volatile INT32U *) 0xD0C00790)) 
#define R_MP4_DeHuffmanEn		(*((volatile INT32U *) 0xD0C00798))	
#define R_MP4_ZZRdy			    (*((volatile INT32U *) 0xD0C0079C))
#define R_MP4_SoftDecode0		(*((volatile INT32U *) 0xD0C007A0))	
#define R_MP4_SoftDecode1		(*((volatile INT32U *) 0xD0C007A4))	
#define R_MP4_VideoRstMode		(*((volatile INT32U *) 0xD0C007A8))	
#define R_MP4_NACTEN			(*((volatile INT32U *) 0xD0C007AC))

#define R_MP4_IMCUCnt0			(*((volatile INT32U *) 0xD0C007B0))
#define R_MP4_IMCUCnt1			(*((volatile INT32U *) 0xD0C007B4))
#define R_MP4_PMCUCNT0			(*((volatile INT32U *) 0xD0C007B8))
#define R_MP4_PMCUCNT1			(*((volatile INT32U *) 0xD0C007BC))
#define R_MP4_SkipMCUCnt0		(*((volatile INT32U *) 0xD0C007C0))
#define R_MP4_SkipMCUCnt1		(*((volatile INT32U *) 0xD0C007C4))	

#define R_MP4_H263_CTRL			(*((volatile INT32U *) 0xD0C007C8))
#define R_MP4_H263_STRUCT		(*((volatile INT32U *) 0xD0C007CC))

#define R_MP4_HmvSum0		    (*((volatile INT32U *) 0xD0C007D0))
#define R_MP4_HmvSum1		    (*((volatile INT32U *) 0xD0C007D4)) 
#define R_MP4_VmvSum0		    (*((volatile INT32U *) 0xD0C007D8))
#define R_MP4_VmvSum1		    (*((volatile INT32U *) 0xD0C007DC))   

#define R_MP4_TSRAM_CTRL		(*((volatile INT32U *) 0xD0C007E0)) 

#define R_MP4_GatedClock		(*((volatile INT32U *) 0xD0C007E4)) 	
#define R_MP4_MBHold		    (*((volatile INT32U *) 0xD0C007E8))
#define R_MP4_ROIMBXOffsetLSB	(*((volatile INT32U *) 0xD0C007EC))	

#define R_MP4_ROIMBXDestLSB		(*((volatile INT32U *) 0xD0C007F0))  
#define R_MP4_ROIMBYOffsetLSB	(*((volatile INT32U *) 0xD0C007F4))	 
#define R_MP4_ROIMBYDestLSB		(*((volatile INT32U *) 0xD0C007F8))   
#define R_MP4_ROIMBY_MSB		(*((volatile INT32U *) 0xD0C007FC))      

// reg0200 ~ reg023f for HuffmanTable
#define P_MP4_Huffman_Str		((volatile INT32U *) 0xD0C00800)		
// reg0200 ~ reg023f for HuffmanTable
//////////////////////////////////////////////////////////////////////////////
#define P_MP4_YDCW_Str			((volatile INT32U *) 0xD0C00800)		
#define P_MP4_YDCS_Str			((volatile INT32U *) 0xD0C00860)		
#define P_MP4_YDCV_Str			((volatile INT32U *) 0xD0C00880)		                              
#define P_MP4_CDCW_Str			((volatile INT32U *) 0xD0C008C0)		
#define P_MP4_CDCS_Str			((volatile INT32U *) 0xD0C00920)		
#define P_MP4_CDCV_Str			((volatile INT32U *) 0xD0C00940)		                             
#define P_MP4_YACW_Str			((volatile INT32U *) 0xD0C00980)		
#define P_MP4_YACS_Str			((volatile INT32U *) 0xD0C009E0)		                              
#define P_MP4_CACW_Str			((volatile INT32U *) 0xD0C00A40)		
#define P_MP4_CACS_Str			((volatile INT32U *) 0xD0C00AA0)

#define R_MP4_RAW_BASE			(*((volatile INT32U *) 0xD0C00C00)) 
#define R_MP4_RREFCON_BASE		(*((volatile INT32U *) 0xD0C00C04)) 	
#define R_MP4_WREFCON_BASE		(*((volatile INT32U *) 0xD0C00C08)) 
#define R_MP4_VLC_BASE			(*((volatile INT32U *) 0xD0C00C0C)) 
#define R_THUMBNAIL_BASE		(*((volatile INT32U *) 0xD0C00C10)) 
#define R_FRAME_SIZE			(*((volatile INT32U *) 0xD0C00C14)) 
#define R_THUMBNAIL_SIZE		(*((volatile INT32U *) 0xD0C00C18)) 
#define R_DP0_BASE				(*((volatile INT32U *) 0xD0C00C1C)) 
#define R_MP4_SUSPEND			(*((volatile INT32U *) 0xD0C00C20))
#define R_MP4_BIST_FAIL			(*((volatile INT32U *) 0xD0C00C24)) 
#define R_MP4_BUSY				(*((volatile INT32U *) 0xD0C00C28))
#define R_MP4_IRAM_BASE			(*((volatile INT32U *) 0xD0C00C2C)) 	
#define R_MP4_YUV_MODE_SET		(*((volatile INT32U *) 0xD0C00C30))

#define R_MP4_DEB_CTRL			(*((volatile INT32U *) 0xD0C00E00))
#define R_MP4_QUANT_CTRL		(*((volatile INT32U *) 0xD0C00E04))
#define R_MP4_DEB_ISRAM_ADDR	(*((volatile INT32U *) 0xD0C00E08))
#define R_MP4_DEB_FRAME_ADDR	(*((volatile INT32U *) 0xD0C00E0C))
#define R_MP4_DEB_BUSY			(*((volatile INT32U *) 0xD0C00E10))
#define R_MP4_DEB_BIST_FAIL		(*((volatile INT32U *) 0xD0C00E14))

/******************************************************************************
 * MIPI control: 0xD0E0000
 ******************************************************************************/
#define R_MIPI_GLB_CSR				(*((volatile INT32U *) 0xD0E00000))
#define R_MIPI_PHY_RST				(*((volatile INT32U *) 0xD0E00004))
#define R_MIPI_RC_CTRL				(*((volatile INT32U *) 0xD0E00008))
#define R_MIPI_ECC_ORDER			(*((volatile INT32U *) 0xD0E0000C))

#define R_MIPI_CCIR601_TIMING		(*((volatile INT32U *) 0xD0E00010))
#define R_MIPI_IMG_SIZE				(*((volatile INT32U *) 0xD0E00014))

#define R_MIPI_DATA_FMT				(*((volatile INT32U *) 0xD0E00020))
#define R_MIPI_PAYLOAD_HEADER		(*((volatile INT32U *) 0xD0E00024))
#define R_MIPI_CTRL_STATE			(*((volatile INT32U *) 0xD0E00028))
#define R_MIPI_VLK_CHECK			(*((volatile INT32U *) 0xD0E0002C))

#define R_MIPI_HEADER_DATA			(*((volatile INT32U *) 0xD0E00030))
#define R_MIPI_HEADER_DT_VLD		(*((volatile INT32U *) 0xD0E00034))

#define R_MIPI_INT_ENABLE			(*((volatile INT32U *) 0xD0E00040))
#define R_MIPI_INT_SOURCE			(*((volatile INT32U *) 0xD0E00080))
#define R_MIPI_INT_STATUS			(*((volatile INT32U *) 0xD0E000C0))

#endif 		// __DRV_L1_SFR_H__
