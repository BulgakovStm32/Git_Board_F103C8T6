/*
 *	st7735.c
 *
 *  Created on:
 *      Author:
 */
//*******************************************************************************************
//*******************************************************************************************

#include "st7735.h"
//#include "main.h"
//#include "stm32f10x_gpio.h"
//#include "stm32f10x_spi.h"
//#include "uart.h"

//*******************************************************************************************
//*******************************************************************************************
//volatile uint8_t fb[LCD_LEN];          // screen buffer
//bool isPower = false;

void st7735_send(uint8_t dc, uint8_t data);

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void st7735_init(uint16_t color){

	//Инициализация портов
	STM32_GPIO_InitForOutputPushPull(ST7735_SCK_GPIO_PORT, ST7735_SCK_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_SDA_GPIO_PORT, ST7735_SDA_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_RST_GPIO_PORT, ST7735_RST_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_RS_GPIO_PORT,  ST7735_RS_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_CS_GPIO_PORT,  ST7735_CS_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_LED_GPIO_PORT, ST7735_LED_GPIO_PIN);

	//-------------------
	//chip_select_enable();
	ST7735_CS_Low;

    // hardware reset
	ST7735_RST_Low;		//GPIO_PIN_Low(GPIOB, RST);
	ST7735_Delay_mS(10);

	ST7735_RST_High;	//GPIO_PIN_High(GPIOB, RST);
	ST7735_Delay_mS(10);

	//-------------------
    // init routine

    st7735_send(ST7735_CMD, ST77XX_SWRESET);
    DELAY_mS(150);	//delay_ms(150);
    st7735_send(ST7735_CMD, ST77XX_SLPOUT);
    DELAY_mS(150);	//delay_ms(150);

    st7735_send(ST7735_CMD, ST7735_FRMCTR1);
    st7735_send(ST7735_DATA, 0x01);
    st7735_send(ST7735_DATA, 0x2C);
    st7735_send(ST7735_DATA, 0x2D);

    st7735_send(ST7735_CMD, ST7735_FRMCTR2);
    st7735_send(ST7735_DATA, 0x01);
    st7735_send(ST7735_DATA, 0x2C);
    st7735_send(ST7735_DATA, 0x2D);

    st7735_send(ST7735_CMD, ST7735_FRMCTR3);
    st7735_send(ST7735_DATA, 0x01);
    st7735_send(ST7735_DATA, 0x2C);
    st7735_send(ST7735_DATA, 0x2D);
    st7735_send(ST7735_DATA, 0x01);
    st7735_send(ST7735_DATA, 0x2C);
    st7735_send(ST7735_DATA, 0x2D);

    st7735_send(ST7735_CMD, ST7735_INVCTR);
    st7735_send(ST7735_DATA, 0x07);

    st7735_send(ST7735_CMD, ST7735_PWCTR1);
    st7735_send(ST7735_DATA, 0xA2);
    st7735_send(ST7735_DATA, 0x02);
    st7735_send(ST7735_DATA, 0x84);

    st7735_send(ST7735_CMD, ST7735_PWCTR2);
    st7735_send(ST7735_DATA, 0xC5);

    st7735_send(ST7735_CMD, ST7735_PWCTR3);
    st7735_send(ST7735_DATA, 0x0A);
    st7735_send(ST7735_DATA, 0x00);

    st7735_send(ST7735_CMD, ST7735_PWCTR4);
    st7735_send(ST7735_DATA, 0x8A);
    st7735_send(ST7735_DATA, 0x2A);

    st7735_send(ST7735_CMD, ST7735_PWCTR5);
    st7735_send(ST7735_DATA, 0x8A);
    st7735_send(ST7735_DATA, 0xEE);

    st7735_send(ST7735_CMD, ST7735_VMCTR1);
    st7735_send(ST7735_DATA, 0x0E);

    st7735_send(ST7735_CMD, ST77XX_INVOFF);

    st7735_send(ST7735_CMD, ST77XX_MADCTL);
    st7735_send(ST7735_DATA, 0xC0);

    st7735_send(ST7735_CMD, ST77XX_COLMOD);
    st7735_send(ST7735_DATA, 0x05);

    st7735_send(ST7735_CMD, ST7735_GMCTRP1);
    st7735_send(ST7735_DATA, 0x02);
    st7735_send(ST7735_DATA, 0x1C);
    st7735_send(ST7735_DATA, 0x07);
    st7735_send(ST7735_DATA, 0x12);
    st7735_send(ST7735_DATA, 0x37);
    st7735_send(ST7735_DATA, 0x32);
    st7735_send(ST7735_DATA, 0x29);
    st7735_send(ST7735_DATA, 0x2D);
    st7735_send(ST7735_DATA, 0x29);
    st7735_send(ST7735_DATA, 0x25);
    st7735_send(ST7735_DATA, 0x2B);
    st7735_send(ST7735_DATA, 0x39);
    st7735_send(ST7735_DATA, 0x00);
    st7735_send(ST7735_DATA, 0x01);
    st7735_send(ST7735_DATA, 0x03);
    st7735_send(ST7735_DATA, 0x10);

    st7735_send(ST7735_CMD, ST7735_GMCTRN1);
    st7735_send(ST7735_DATA, 0x03);
    st7735_send(ST7735_DATA, 0x1D);
    st7735_send(ST7735_DATA, 0x07);
    st7735_send(ST7735_DATA, 0x06);
    st7735_send(ST7735_DATA, 0x2E);
    st7735_send(ST7735_DATA, 0x2C);
    st7735_send(ST7735_DATA, 0x29);
    st7735_send(ST7735_DATA, 0x2D);
    st7735_send(ST7735_DATA, 0x2E);
    st7735_send(ST7735_DATA, 0x2E);
    st7735_send(ST7735_DATA, 0x37);
    st7735_send(ST7735_DATA, 0x3F);
    st7735_send(ST7735_DATA, 0x00);
    st7735_send(ST7735_DATA, 0x00);
    st7735_send(ST7735_DATA, 0x02);
    st7735_send(ST7735_DATA, 0x10);

    st7735_send(ST7735_CMD, ST77XX_NORON);
    ST7735_Delay_mS(10);	//delay_ms(10);

    st7735_send(ST7735_CMD, ST77XX_DISPON);
    ST7735_Delay_mS(100);	//delay_ms(100);

    //chip_select_disable();
    ST7735_CS_High;

    st7735_clear(color);
}
//***********************************************************
void st7735_send(uint8_t dc, uint8_t data)
{

   if (dc == ST7735_DATA) ST7735_RS_High;	//gpio_set(GPIOB,DC);	//данные
   else					  ST7735_RS_Low; 	//gpio_reset(GPIOB,DC);	//команда

#ifdef HW_SPI
   SPI1->DR=data;
   while (!(SPI1->SR & SPI_I2S_FLAG_TXE) || (SPI1->SR & SPI_I2S_FLAG_BSY));
#else

   for (uint8_t i=0; i<8; i++)
   {
      if (data & 0x80) ST7735_SDA_High;	//gpio_set(GPIOA,DIN);
      else			   ST7735_SDA_Low; 	//gpio_reset(GPIOA,DIN);

      data = (data << 1);

      // Set Clock Signal
      ST7735_SCK_High; 	//gpio_set(GPIOA,CLK);
      ST7735_SCK_Low;	//gpio_reset(GPIOA,CLK);
   }
#endif
}
//***********************************************************
void st7735_fill(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1, uint16_t color){

   //chip_select_enable();
   ST7735_CS_Low;

   st7735_send(ST7735_CMD,  ST77XX_CASET);
   st7735_send(ST7735_DATA, 0);
   st7735_send(ST7735_DATA, x0);
   st7735_send(ST7735_DATA, 0);
   st7735_send(ST7735_DATA, x1);

   st7735_send(ST7735_CMD,  ST77XX_RASET);
   st7735_send(ST7735_DATA, 0);
   st7735_send(ST7735_DATA, y0);
   st7735_send(ST7735_DATA, 0);
   st7735_send(ST7735_DATA, y1);

   st7735_send(ST7735_CMD,  ST77XX_RAMWR);

   uint16_t len=(uint16_t)((uint16_t)(x1-x0+1)*(uint16_t)(y1-y0+1));

#ifdef HW_SPI

   gpio_set(GPIOB,DC);
   SPI1->CR1 &= CR1_SPE_Reset;      // disable SPI for setup
   SPI1->CR1  = (SPI_Mode_Master|SPI_DataSize_16b|SPI_NSS_Soft|SPI_BaudRatePrescaler_2);
   SPI1->CR1 |= CR1_SPE_Set;        // enable SPI
   for (uint16_t i=0; i < len; i++)
   {
       while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
       SPI1->DR=color;
   }
   while (!(SPI1->SR & SPI_I2S_FLAG_TXE) || (SPI1->SR & SPI_I2S_FLAG_BSY));

   chip_select_disable();

   SPI1->CR1 &= CR1_SPE_Reset;      // disable SPI for setup
   SPI1->CR1  = (SPI_Mode_Master|SPI_DataSize_8b|SPI_NSS_Soft|SPI_BaudRatePrescaler_4);
   SPI1->CR1 |= CR1_SPE_Set;        // enable SPI
#else
   uint8_t c1 = color >> 8;
   uint8_t c2 = (uint8_t)color;

   ST7735_RS_High;	//gpio_set(GPIOB,DC);

   for (uint16_t i=0; i < len; i++)
   {
      st7735_send(ST7735_DATA, c1);
      st7735_send(ST7735_DATA, c2);
   }

   //chip_select_disable();
   ST7735_CS_High;
#endif
}
//***********************************************************


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

