
#ifndef _spi_ST_H
#define _spi_ST_H
//-----------------------------------------------------------------------------

//#include "stm32f10x.h"
#include "main.h"

//-----------------------------------------------------------------------------
#define SPI_WAIT ((uint32_t)0x5000)

#define SPI_INIT (1<<0) //SPI проинициализирован.

//-----------------------------------------------------------------------------
void     Spi1Init(void);
uint8_t	 Spi1TxRxByte(uint8_t byte);
uint16_t Spi1TxRx2Byte(uint16_t data);
uint32_t Spi1Rx3Byte(void);

void     Spi2Init    (void);
uint8_t  Spi2GetStat (void);
uint16_t Spi2SendData(uint16_t data);
uint8_t  Spi2TxRxByte(uint8_t byte);
//-----------------------------------------------------------------------------
#endif /*_spi_ST_H*/


