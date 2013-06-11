#include "drv_l1_mipi.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)                 //
//================================================================//
/****************************************************
*		constant 									*
****************************************************/
#define VIC_MIPI				VIC_MS

#define SOT_SYNC_ERR0_BIT		(1<<0)
#define HD_ERR_1BIT_BIT			(1<<1)
#define HD_ERR_BIT				(1<<2)
#define CRC_ERR_BIT				(1<<3)
#define SOT_SYNC_ERR1_BIT		(1<<4)
#define SOF_FLAG_BIT			(1<<5)

/****************************************************
*		varaible 									*
*****************************************************/
void (*mipi_user_isr[SOF_FLAG_IDX + 1])(void);

static void 
mipi_isr(
	void
)
{
	INT32U bit;
	INT32U IRQ_Status = R_MIPI_INT_STATUS;
	INT32U IRQ_Enable = R_MIPI_INT_ENABLE;
	
	IRQ_Status &= IRQ_Enable;
	R_MIPI_INT_SOURCE = IRQ_Status; 
	
	bit = (1 << SOT_SYNC_ERR0_IDX);
	if((IRQ_Status & bit) && (mipi_user_isr[SOT_SYNC_ERR0_IDX])) {
		mipi_user_isr[SOT_SYNC_ERR0_IDX]();
	}
	
	bit = (1 << HD_ERR_1BIT_IDX);
	if((IRQ_Status & bit) && (mipi_user_isr[HD_ERR_1BIT_IDX])) {
		mipi_user_isr[HD_ERR_1BIT_IDX]();
	}
	
	bit = (1 << HD_ERR_IDX);
	if((IRQ_Status & bit) && (mipi_user_isr[HD_ERR_IDX])) {
		mipi_user_isr[HD_ERR_IDX]();
	}
	
	bit = (1 << CRC_ERR_IDX);
	if((IRQ_Status & bit) && (mipi_user_isr[CRC_ERR_IDX])) {
		mipi_user_isr[CRC_ERR_IDX]();
	}
	
	bit = (1 << SOT_SYNC_ERR1_IDX);
	if((IRQ_Status & bit) && (mipi_user_isr[SOT_SYNC_ERR1_IDX])) {
		mipi_user_isr[SOT_SYNC_ERR1_IDX]();
	}
	
	bit = (1 << SOF_FLAG_IDX);
	if((IRQ_Status & bit) && (mipi_user_isr[SOF_FLAG_IDX])) {
		mipi_user_isr[SOF_FLAG_IDX]();
	}
}

void 
mipi_init(
	INT8U mipi_to_csi
)
{
	INT32U reg = R_SYSTEM_CTRL_NEW;
	
	R_MIPI_GLB_CSR = 0x01; 
	R_MIPI_INT_SOURCE = 0x3F; 
	R_MIPI_INT_ENABLE = 0; 
	
	if(mipi_to_csi == MIPI_DATA_TO_CSI) {
		reg |= (1 << 0);	//ppu csi
		reg &= ~(1 << 1); 		
	} else {
		reg &= ~(1 << 0);	
		reg |= (1 << 1); 	//oid		
	}
	R_SYSTEM_CTRL_NEW = reg;
	
	vic_irq_register(VIC_MIPI, mipi_isr);
	vic_irq_enable(VIC_MIPI);
}

void 
mipi_uninit(
	void
)
{
	R_MIPI_GLB_CSR = 0;
	R_MIPI_INT_SOURCE = 0x3F; 
	R_MIPI_INT_ENABLE = 0; 
	
	vic_irq_unregister(VIC_MIPI);
	vic_irq_disable(VIC_MIPI);
}

INT32S 
mipi_user_isr_register(
	INT8U index, 
	void (*user_isr)(void) 
)
{
	if(index > SOF_FLAG_IDX) {
		return -1;
	}
	
	if(user_isr == 0) {
		return -1;
	}
	
	mipi_user_isr[index] = user_isr;
	return 0;
}

INT32S 
mipi_user_isr_unregister(
	INT8U index
)
{
	if(index > SOF_FLAG_IDX) {
		return -1;
	}
	
	mipi_user_isr[index] = 0;
	return 0;
}

INT32S 
mipi_set_irq_enable(
	INT32U enable,
	INT32U index
)
{
	INT32U bit = (1 << index);
	
	if(index > SOF_FLAG_IDX) {
		return -1;
	}
	
	if(enable) {
		R_MIPI_INT_SOURCE |= bit & 0x3F;
		R_MIPI_INT_ENABLE |= bit & 0x3F; 
	} else {
		R_MIPI_INT_ENABLE &= ~bit & 0x3F; 	
		R_MIPI_INT_SOURCE = bit & 0x3F;
	}
	
	return 0;
}

void
mipi_set_global_cfg(
	INT8U low_power_en,
	INT8U byte_clk_edge, 
	INT8U sel_pix_clk
)
{
	INT32U reg = R_MIPI_GLB_CSR;
	
	if(low_power_en) {
		reg |= (1<<4);
	} else {
		reg &= ~(1<<4);
	}
	
	reg &= ~(1<<8);	//only support 1 lane
	
	if(sel_pix_clk) {
		//reg |= (1<<12); //not support MIPI_SPEREATE_CLK
	} else {
		reg &= ~(1<<12);
	}
	R_MIPI_GLB_CSR = reg;	
} 

void
mipi_set_ecc(
	INT8U ecc_order,
	INT8U ecc_check_en,
	INT8U da_mask_cnt,
	INT8U check_hs_seq
)
{
	INT32U reg = R_MIPI_ECC_ORDER;
	
	reg &= ~0x3;
	reg |= ecc_order & 0x03;
	
	if(ecc_check_en) {
		reg |= (1<<2);
	} else {
		reg &= ~(1<<2);
	}
	
	reg &= ~(0xFF << 8);
	reg |= ((INT32U)da_mask_cnt << 8);
	
	if(check_hs_seq) {
		reg |= (1<<16);
	} else {
		reg &= ~(1<<16);
	}
						
	R_MIPI_ECC_ORDER = reg;
}

void
mipi_set_ccir601_if(
	INT8U h_back_porch,
	INT8U h_front_porch,
	INT8U blanking_line_en
)
{
	INT32U reg = R_MIPI_CCIR601_TIMING;
	
	reg &= ~(0x0F << 0);
	reg |= (h_back_porch & 0x0F);
	
	reg &= ~(0x0F << 8);
	reg |= ((INT32U)(h_front_porch & 0x0F) << 8);
	
	if(blanking_line_en) {
		reg |= (1<<16);
	} else {
		reg &= ~(1<<16);
	}

	R_MIPI_CCIR601_TIMING = reg;
}

void
mipi_set_image_size(
	INT16U h_size,
	INT16U v_size
)
{
	INT32U format = (R_MIPI_DATA_FMT >> 4) & 0x07;
	
	if(h_size < 1) {
		h_size = 1;
	}
	
	switch(format)
	{
	case MIPI_YUV422:	
	case MIPI_YUV565:
	case MIPI_RAW10:
	case MIPI_RAW12:
		h_size *= 2;
		break;
		
	case MIPI_RGB888:
		h_size *= 3;
		break;

	case MIPI_RAW8:
	case MIPI_GENERIC_8_BIT:
		break;
	}
	
	h_size &= 0xFFFF;
	v_size &= 0xFFFF;
	R_MIPI_IMG_SIZE = h_size | ((INT32U)v_size << 16);
}

void
mipi_set_data_fmt(
	INT8U data_from_mmr,
	INT8U data_type_mmr
)
{
	INT32U reg = R_MIPI_DATA_FMT;

	if(data_from_mmr) {
		reg |= (1<<0);
	} else {
		reg &= ~(1<<0);
	}
	
	reg &= ~(0x07 << 4);
	reg |= (data_type_mmr & 0x07) << 4;

	R_MIPI_DATA_FMT = reg;
}

void 
mipi_set_parameter(
	mipi_config_t *para
)
{
	mipi_set_data_fmt(para->data_from_mmr, para->mipi_format);
	
	mipi_set_image_size(para->width, para->height);
	
	mipi_set_ccir601_if(0, 4, ENABLE);
	
	mipi_set_global_cfg(para->low_power_en,
						para->byte_clk_edge, 
						MIPI_D_PHY_CLK);
						
	mipi_set_ecc(para->ecc_order, 
				para->ecc_check_en, 
				para->data_mask_time, 
				para->check_hs_seq);
}
#endif