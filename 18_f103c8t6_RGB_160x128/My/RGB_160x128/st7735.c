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
void st7735_send(uint8_t dc, uint8_t data){

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
void st7735_WriteDataBuf(uint8_t *buff, size_t buff_size) {

	while(buff_size > 0)
	{
		st7735_send(ST7735_DATA, *buff);
		buff++;
		buff_size--;
	}
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

   //количество пикселей заданной области
   uint16_t len = (uint16_t)((uint16_t)(x1-x0+1) * (uint16_t)(y1-y0+1));

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
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void st7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    // column address set
    //ST7735_WriteCommand(ST7735_CASET);
	st7735_send(ST7735_CMD, ST77XX_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    st7735_WriteDataBuf(data, sizeof(data));

    // row address set
    //ST7735_WriteCommand(ST7735_RASET);
    st7735_send(ST7735_CMD, ST77XX_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    st7735_WriteDataBuf(data, sizeof(data));

    // write to RAM
    //ST7735_WriteCommand(ST7735_RAMWR);
    st7735_send(ST7735_CMD, ST77XX_RAMWR);
}
//***********************************************************
void st7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {

	uint32_t i, b, j;

	st7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                //ST7735_WriteData(data, sizeof(data));
                st7735_WriteDataBuf(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                //ST7735_WriteData(data, sizeof(data));
                st7735_WriteDataBuf(data, sizeof(data));
            }
        }
    }
}
//***********************************************************
void st7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {

	//chip_select_enable();
	ST7735_CS_Low;

    while(*str)
    {
        if(x + font.width >= ST7735_WIDTH)
        {
            x = 0;
            y += font.height;
            if(y + font.height >= ST7735_HEIGHT)
            {
                break;
            }

            if(*str == ' ')
            {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        st7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    //chip_select_disable();
    ST7735_CS_High;
}
//***********************************************************
uint32_t st7735_BinToDec(uint16_t x, uint16_t y, uint32_t var, uint32_t numDigit, FontDef font, uint16_t color, uint16_t bgcolor){

	#define DEC_ARR_SIZE 10

	uint8_t  decArray[DEC_ARR_SIZE];
	uint32_t div = 1000000000;
	uint16_t pos_x = x;
	//--------------------
	//Преобразование числа в строку.
	for(uint32_t i = DEC_ARR_SIZE; i > 0; i--)
	{
		decArray[i-1] = (uint8_t)(var/div);
		var %= div;
		div /= 10;
	}
	//--------------------
	//Вывод на дисплей

	ST7735_CS_Low;//chip_select_enable
	for(uint8_t i = 0; i < numDigit; i++)
	{
		var = 0x30 + decArray[(numDigit - 1) - i];

		st7735_WriteChar(pos_x, y, (char)var, font, color, bgcolor);

		pos_x = pos_x + font.width;
	}
	ST7735_CS_High; //chip_select_disable

	return numDigit+1;
}
//***********************************************************
void st7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {

    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH)  return;
    if((y + h - 1) >= ST7735_HEIGHT) return;

    //ST7735_Select();
    ST7735_CS_Low;

    st7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    st7735_WriteDataBuf((uint8_t*)data, sizeof(uint16_t)*w*h);

    //ST7735_Unselect();
    ST7735_CS_High;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

