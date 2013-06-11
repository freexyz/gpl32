#ifndef __drv_l1_SPI_H__
#define __drv_l1_SPI_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"


#define SPI_CONTROL_ENA    (1 << 15)  /* SPI enable */
#define SPI_CONTROL_LBM    (1 << 13)  /* loop back mode selection: SPIRX = SPITX */
#define SPI_CONTROL_RST    (1 << 11)  /* SPI software reset */
#define SPI_CONTROL_SLVE   (1 <<  8)  /* SPI Slave Mode */
#define SPI_CONTROL_CPHA   (1 <<  5)  /* SPI Clock Phase */
#define SPI_CONTROL_CPOL   (1 <<  4)  /* SPI Clock Polarity */
#define SPI_CONTROL_CLK    (7 <<  0) /* SPI master mode clock selection */

#define SPI_TX_STATUS_IF    (1 << 15)  /* SPI transmit interrupt flag */
#define SPI_RX_STATUS_IF    (1 << 15)  /* SPI receive interrupt flag */
#define SPI_TX_STATUS_IEN   (1 << 14)  /* SPI transmit interrupt enable */
#define SPI_RX_STATUS_IEN   (1 << 14)  /* SPI receive interrupt enable */

#define SPI_TX_STATUS_LEVEL  (0xF << 4)  /* SPI transmit FIFO interrupt level */
#define SPI_RX_STATUS_LEVEL  (0xF << 4)  /* SPI receiver FIFO interrupt level */
#define SPI_RX_STATUS_RXFLAG (7 << 0)

#define SPI_MISC_SMART     (1 <<  8)  /* SPI FIFO smart mode */
#define SPI_MISC_RFF       (1 <<  3)  /* Rcecive FIFO is not empty */
#define SPI_MISC_RNE       (1 <<  2)  /* Rcecive FIFO is not empty */
#define SPI_MISC_TNF       (1 <<  1)  /* Transmit FIFO is not full */


#define SPI_TIMEOUT_VALUE  10000

/* SPI error message */
#define SPI_TIMEOUT        -1
#define SPI_BUSY           -2

typedef enum
{
    SPI_MODE_MST,                     
    SPI_MODE_SLAVE                     
} SPI_MODE_TYPE;

typedef enum
{
    SPI_DISABLED,                 
    SPI_ENABLED                     
} SPI_FLAG;

typedef struct 
{
	volatile INT32U	CTRL;
	volatile INT32U	TX_STATUS;
	volatile INT32U	TX_DATA;
	volatile INT32U	RX_STATUS;
	volatile INT32U	RX_DATA;
	volatile INT32U	MISC;
}SPI_SFR;

typedef struct
{
	INT8U	spi_num;
	INT8U 	*tx_buf;
	INT8U 	*rx_buf;
	BOOLEAN cs_internal;
	INT32U  len;
	INT32U  rcv_len;
	INT32U  tx_count;
	INT32U  rx_count;
	INT8U 	cs_gpio_pin;
	INT8U   cs_active;
	INT8U   tx_int_level;
	INT8U   rx_int_level;
	INT32S	(*spi_txrx)(INT8U spi_num);
	SPI_SFR *spi_sfr;
} SPI_MASTER;

extern void spi_init(INT8U spi_num);
extern void spi_lbm_set(INT8U spi_num, INT8S status);
extern INT32S spi_clk_set(INT8U spi_num, INT8S spi_clk);
extern INT32S spi_transceive(INT8U spi_num, INT8U *tx_data, INT32U tx_len, INT8U *rx_data, INT32U rx_len);
extern INT32S spi_disable(INT8U spi_num);
extern INT32S spi_pha_pol_set(INT8U spi_num, INT8U pha_pol);
extern void spi_cs_by_internal_set(INT8U spi_num,INT8U pin_num, INT8U active);
extern void spi_cs_by_external_set(INT8U spi_num);
extern INT32S spi_transceive_cpu(INT8U spi_num, INT8U *tx_data, INT32U tx_len, INT8U *rx_data, INT32U rx_len);
extern INT32S spi_clk_rate_set(INT8U spi_num, INT32U clk_rate);

#endif 		// __drv_l1_SPI_H__