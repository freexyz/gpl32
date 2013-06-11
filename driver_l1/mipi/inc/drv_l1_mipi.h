#ifndef __drv_l1_MIPI_H__
#define __drv_l1_MIPI_H__

/****************************************************
*		include file								*
****************************************************/
#include "driver_l1.h"
#include "drv_l1_sfr.h"

/****************************************************
*		constant 									*
****************************************************/
#ifndef ENABLE
#define ENABLE 		1
#endif
#ifndef DISABLE
#define DISABLE		0
#endif

/* mipi clock sample edge*/
#define MIPI_DATA_TO_CSI		0
#define MIPI_DATA_TO_OID		1

/* mipi clock sample edge*/
#define D_PHY_SAMPLE_POS		0x0
#define D_PHY_SAMPLE_NEG		0x1

/* mipi lane */
#define MIPI_1_LANE				0x0
#define MIPI_2_LANE				0x1

/* mipi seperate clock source */
#define MIPI_D_PHY_CLK			0x0
#define MIPI_SPEREATE_CLK		0x1

/* mipi ecc order*/
#define MIPI_ECC_ORDER0			0x0
#define MIPI_ECC_ORDER1			0x1
#define MIPI_ECC_ORDER2			0x2
#define MIPI_ECC_ORDER3			0x3

/* mipi check method*/
#define MIPI_CHECK_LP_00		0x0
#define MIPI_CHECK_HS_SEQ		0x1

/* mipi format */
#define MIPI_YUV422				0x0
#define MIPI_RGB888				0x1
#define MIPI_YUV565				0x2
#define MIPI_RAW8				0x3
#define MIPI_RAW10				0x4
#define MIPI_RAW12				0x5
#define MIPI_GENERIC_8_BIT		0x6
#define MIPI_USER_DEFINE_BYTE	0x7

/* interrupt flag */
#define MIPI_LANE0_SOT_SYNC_ERR	0x01
#define MIPI_HD_1BIT_ERR 		0x02
#define MIPI_HD_NBIT_ERR		0x04
#define MIPI_DATA_CRC_ERR		0x08
#define MIPI_LANE1_SOT_SYNC_ERR	0x10
#define MIPI_CCIR_SOF 			0x20

// interrupt index
#define SOT_SYNC_ERR0_IDX		0
#define HD_ERR_1BIT_IDX			1
#define HD_ERR_IDX				2
#define CRC_ERR_IDX				3
#define SOT_SYNC_ERR1_IDX		4
#define SOF_FLAG_IDX			5


/****************************************************
*		data type 									*
*****************************************************/
#ifndef __MIPI_CONFIG__
#define __MIPI_CONFIG__
typedef struct mipi_config_s 
{
	/* image size */
	INT16U width;
	INT16U height;
	
	/* global configure */
	INT8U low_power_en;		/* 0:disable, 1:enable */
	INT8U byte_clk_edge;	/* 0:posedge, 1:negedge */
	
	/* format */
	INT8U data_from_mmr;	/* 0:disable, 1:enable */
	INT8U mipi_format;		/* MIPI_YUV422, MIPI_YUV565... */
	
	/* ecc */
	INT8U ecc_check_en;		/* 0:disable, 1:enable */
	INT8U ecc_order;		/* 0~3 */
	INT8U data_mask_time;	/* unit is ns */
	INT8U check_hs_seq;		/* 0:disable, 1:enable */
}mipi_config_t;
#endif


/****************************************************
*		function
*****************************************************/
extern void mipi_init(INT8U mipi_to_csi);
extern void mipi_uninit(void);
extern INT32S mipi_user_isr_register(INT8U index, void (*user_isr)(void));
extern INT32S mipi_user_isr_unregister(INT8U index);
extern INT32S mipi_set_irq_enable(INT32U enable, INT32U index);
extern void mipi_set_global_cfg(INT8U low_power_en, INT8U byte_clk_edge, INT8U sel_pix_clk);
extern void mipi_set_ecc(INT8U ecc_order, INT8U ecc_check_en, INT8U da_mask_cnt, INT8U check_hs_seq);
extern void mipi_set_ccir601_if(INT8U h_back_porch, INT8U h_front_porch, INT8U blanking_line_en);
extern void mipi_set_image_size(INT16U h_size, INT16U v_size);
extern void mipi_set_data_fmt(INT8U data_from_mmr, INT8U data_type_mmr);
extern void mipi_set_parameter(mipi_config_t *para);
#endif