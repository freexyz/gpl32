#include "drv_l1_tools.h"
#include "drv_l1_sfr.h"


// DMA INT register
#define	C_DMA0_INT_PEND				0x00000001
#define	C_DMA1_INT_PEND				0x00000002
#define	C_DMA2_INT_PEND				0x00000004
#define	C_DMA3_INT_PEND				0x00000008
#define	C_DMA0_TIMEOUT				0x00000010
#define	C_DMA1_TIMEOUT				0x00000020
#define	C_DMA2_TIMEOUT				0x00000040
#define	C_DMA3_TIMEOUT				0x00000080
#define	C_DMA0_BUSY					0x00000100
#define	C_DMA1_BUSY					0x00000200
#define	C_DMA2_BUSY					0x00000400
#define	C_DMA3_BUSY					0x00000800


// DMA Miscellaneuous register
#define C_DMA_MISC_STATE_MASK		0x00000007
#define C_DMA_MISC_TIMEOUT_SHIFT	4
#define C_DMA_MISC_TIMEOUT_MAX		255
#define C_DMA_MISC_TIMEOUT_MASK		0x00000FF0
#define C_DMA_MISC_TRANSPARENT_EN	0x00001000
#define C_DMA_MISC_ERROR_WRITE		0x00004000
#define C_DMA_MISC_DMA_REQUEST		0x00008000

// DMA Control register
#define	C_DMA_CTRL_ENABLE			0x00000001
#define	C_DMA_CTRL_BUSY				0x00000002
#define	C_DMA_CTRL_SOFTWARE			0x00000000
#define	C_DMA_CTRL_EXTERNAL			0x00000004
#define	C_DMA_CTRL_NORMAL_INT		0x00000008
#define	C_DMA_CTRL_DEST_INCREASE	0x00000000
#define	C_DMA_CTRL_DEST_DECREASE	0x00000010
#define	C_DMA_CTRL_SRC_INCREASE		0x00000000
#define	C_DMA_CTRL_SRC_DECREASE		0x00000020
#define	C_DMA_CTRL_DEST_FIX			0x00000040
#define	C_DMA_CTRL_SRC_FIX			0x00000080
#define	C_DMA_CTRL_INT				0x00000100
#define	C_DMA_CTRL_RESET			0x00000200
#define	C_DMA_CTRL_M2M				0x00000000
#define	C_DMA_CTRL_M2IO				0x00000400
#define	C_DMA_CTRL_IO2M				0x00000800
#define	C_DMA_CTRL_8BIT				0x00000000
#define	C_DMA_CTRL_16BIT			0x00001000
#define	C_DMA_CTRL_32BIT			0x00002000
#define	C_DMA_CTRL_SINGLE_TRANS		0x00000000
#define	C_DMA_CTRL_DEMAND_TRANS		0x00004000
#define	C_DMA_CTRL_DBF				0x00008000
#define	C_DMA_CTRL_SINGLE_ACCESS	0x00000000
#define	C_DMA_CTRL_BURST4_ACCESS	0x00010000
#define	C_DMA_CTRL_BURST8_ACCESS	0x00020000


INT32S mem_transfer_dma(INT32U src, INT32U dest, INT32U len);


INT32S mem_transfer_dma(INT32U src, INT32U dest, INT32U len)
{

	#if (defined _DRV_L1_DMA) && (_DRV_L1_DMA == 1)
    	DMA_STRUCT dma_struct;
    	INT8S done;
    	INT32S result;
    	if (OSRunning==TRUE)
        {
    		done = C_DMA_STATUS_WAITING;
    		dma_struct.s_addr = (INT32U) src;
    		dma_struct.t_addr = (INT32U) dest;
    		dma_struct.width = DMA_DATA_WIDTH_4BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
    		dma_struct.count = (INT32U)(len/4); 
    		dma_struct.notify = &done;
    		dma_struct.timeout = 0;
    		result = dma_transfer(&dma_struct);
    		if (result) {
    			while(1);
    		}

			while (1)
			{
				if (done != C_DMA_STATUS_WAITING)
				{
					break;
				}
			}
			return STATUS_OK;
    	}
    	else
    	{
    		#if _DRV_L1_CACHE==1
    		cache_invalid_range((INT32U) src, len);
    		#endif 	    		
    		gp_memcpy((INT8S *)dest,(INT8S *)src,(INT32U)len);
    	}	
		return STATUS_OK;
	#else
		R_DMA0_CTRL	= C_DMA_CTRL_RESET; /* reset dma */
		R_DMA_INT = R_DMA_INT; /* clear interrupt flag */
		
		R_DMA0_MISC &= ~(C_DMA_MISC_TIMEOUT_MASK); /* set time-out value */
		R_DMA0_MISC |= 0xFF << C_DMA_MISC_TIMEOUT_SHIFT; /* 1 sec */
		
		/* set DMA 0 for transfering data */
		R_DMA0_SRC_ADDR = (INT32U)src;
		R_DMA0_TAR_ADDR = (INT32U)dest;
		R_DMA0_TX_COUNT = len>>1;
		R_DMA0_CTRL     = C_DMA_CTRL_SINGLE_ACCESS|C_DMA_CTRL_SINGLE_TRANS|C_DMA_CTRL_SRC_INCREASE|C_DMA_CTRL_DEST_INCREASE 
		                 |C_DMA_CTRL_SOFTWARE|C_DMA_CTRL_16BIT|C_DMA_CTRL_INT|C_DMA_CTRL_NORMAL_INT|C_DMA_CTRL_ENABLE;
		
		
		while((R_DMA_INT&C_DMA0_INT_PEND) == 0);
		if (R_DMA_INT & C_DMA0_TIMEOUT) {
			return C_DMA0_TIMEOUT;
		}
		R_DMA_INT = C_DMA0_INT_PEND;
			
		return STATUS_OK;
    #endif

}

#if 0
INT32U DMAmmCopy(INT32U wSourceAddr, INT32U wTargeAddr, INT16U Count) //count is byte num
{
//	INT32S i;
	DMA_STRUCT dma_struct;
	INT8S done;
	INT32S result;
	if (OSRunning==TRUE)
    {
		done = C_DMA_STATUS_WAITING;
		dma_struct.s_addr = (INT32U) wSourceAddr;
		dma_struct.t_addr = (INT32U) wTargeAddr;
		dma_struct.width = DMA_DATA_WIDTH_4BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
		dma_struct.count = (INT32U)(Count/4); 
		dma_struct.notify = &done;
		dma_struct.timeout = 0;
		result = dma_transfer(&dma_struct);
		if (result) {
			while(1);
		}
		#if 0	
			i = 0xFFFFFF;
			while (i > 0) {
				if (done != C_DMA_STATUS_WAITING) {
					break;
				}
				i--;
			}
			if (i == 0) {
				return;
			}
			#else
			while (1)
			 {
				if (done != C_DMA_STATUS_WAITING)
				{
					break;
				}
			}
			return 0;
		#endif
	}
	else
	{
		#if _DRV_L1_CACHE==1
		cache_invalid_range((INT32U) wSourceAddr, Count);
		#endif 	
		
		gp_memcpy((INT8S *)wTargeAddr,(INT8S *)wSourceAddr,(INT32U)Count);
	}	
	return 0;
}
#endif

