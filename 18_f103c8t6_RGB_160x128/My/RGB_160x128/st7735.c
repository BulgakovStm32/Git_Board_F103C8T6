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

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
static void _st7735_SendByte(uint8_t dc, uint8_t data){

   if (dc == ST7735_DATA) ST7735_RS_High;	//gpio_set(GPIOB,DC);	//данные
   else					  ST7735_RS_Low; 	//gpio_reset(GPIOB,DC);	//команда

#ifdef HW_SPI

   ST7735_SPI->DR = data;
   while (!(ST7735_SPI->SR & SPI_SR_TXE) || (ST7735_SPI->SR & SPI_SR_BSY));

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
static void _st7735_SendCmd(uint8_t cmd){

   	ST7735_RS_Low; 	//gpio_reset(GPIOB,DC);	//команда

#ifdef HW_SPI

   ST7735_SPI->DR = cmd;
   while (!(ST7735_SPI->SR & SPI_SR_TXE) || (ST7735_SPI->SR & SPI_SR_BSY));

#else

   for (uint8_t i=0; i<8; i++)
   {
      if (cmd & 0x80) ST7735_SDA_High;	//gpio_set(GPIOA,DIN);
      else			   ST7735_SDA_Low; 	//gpio_reset(GPIOA,DIN);

      cmd = (cmd << 1);

      // Set Clock Signal
      ST7735_SCK_High; 	//gpio_set(GPIOA,CLK);
      ST7735_SCK_Low;	//gpio_reset(GPIOA,CLK);
   }

#endif

}
//***********************************************************
void _st7735_SendDataBuf(uint8_t *buff, size_t buff_size) {

	ST7735_RS_High;	//gpio_set(GPIOB,DC);	//данные

	while(buff_size--)
	{
		//_st7735_SendByte(ST7735_DATA, *buff++);

		while (!(ST7735_SPI->SR & SPI_SR_TXE));
		ST7735_SPI->DR = *buff++;
	}
}
//***********************************************************


//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void st7735_Init(uint16_t color){

	//Инициализация портов
	STM32_GPIO_InitForOutputPushPull(ST7735_RST_GPIO_PORT, ST7735_RST_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_RS_GPIO_PORT,  ST7735_RS_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_CS_GPIO_PORT,  ST7735_CS_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_LED_GPIO_PORT, ST7735_LED_GPIO_PIN);

#ifdef HW_SPI
	//Использование аппаратного модуля SPI.
	SPI_Init(ST7735_SPI);

#else
	//Програмная реализация SPI
	STM32_GPIO_InitForOutputPushPull(ST7735_SCK_GPIO_PORT, ST7735_SCK_GPIO_PIN);
	STM32_GPIO_InitForOutputPushPull(ST7735_SDA_GPIO_PORT, ST7735_SDA_GPIO_PIN);
#endif

	//-------------------
	//chip_select_enable
	ST7735_SELECT;

    // hardware reset
	ST7735_RST_Low;		//GPIO_PIN_Low(GPIOB, RST);
	ST7735_Delay_mS(10);

	ST7735_RST_High;	//GPIO_PIN_High(GPIOB, RST);
	ST7735_Delay_mS(10);

	//-------------------
    // init routine

	_st7735_SendByte(ST7735_CMD, ST77XX_SWRESET);//Software Reset
    DELAY_mS(150);	//delay_ms(150);

    _st7735_SendByte(ST7735_CMD, ST77XX_SLPOUT);//Sleep out & booster on
    DELAY_mS(150);	//delay_ms(150);


    //Frame Rate Control (In normal mode/ Full colors)
    _st7735_SendByte(ST7735_CMD, ST7735_FRMCTR1);
    _st7735_SendByte(ST7735_DATA, 0x01);
    _st7735_SendByte(ST7735_DATA, 0x2C);
    _st7735_SendByte(ST7735_DATA, 0x2D);

    //Frame Rate Control (In Idle mode/ 8-colors)
    _st7735_SendByte(ST7735_CMD, ST7735_FRMCTR2);
    _st7735_SendByte(ST7735_DATA, 0x01);
    _st7735_SendByte(ST7735_DATA, 0x2C);
    _st7735_SendByte(ST7735_DATA, 0x2D);

    //Frame Rate Control (In Partial mode/ full colors)
    _st7735_SendByte(ST7735_CMD, ST7735_FRMCTR3);
    _st7735_SendByte(ST7735_DATA, 0x01);
    _st7735_SendByte(ST7735_DATA, 0x2C);
    _st7735_SendByte(ST7735_DATA, 0x2D);
    _st7735_SendByte(ST7735_DATA, 0x01);
    _st7735_SendByte(ST7735_DATA, 0x2C);
    _st7735_SendByte(ST7735_DATA, 0x2D);

    //Display Inversion Control
    _st7735_SendByte(ST7735_CMD, ST7735_INVCTR);
    _st7735_SendByte(ST7735_DATA, 0x07);

    //Power Control 1
    _st7735_SendByte(ST7735_CMD, ST7735_PWCTR1);
    _st7735_SendByte(ST7735_DATA, 0xA2);
    _st7735_SendByte(ST7735_DATA, 0x02);
    _st7735_SendByte(ST7735_DATA, 0x84);

    //Power Control 2
    _st7735_SendByte(ST7735_CMD, ST7735_PWCTR2);
    _st7735_SendByte(ST7735_DATA, 0xC5);

    //Power Control 3 (in Normal mode/ Full colors)
    _st7735_SendByte(ST7735_CMD, ST7735_PWCTR3);
    _st7735_SendByte(ST7735_DATA, 0x0A);
    _st7735_SendByte(ST7735_DATA, 0x00);

    //Power Control 4 (in Idle mode/ 8-colors)
    _st7735_SendByte(ST7735_CMD, ST7735_PWCTR4);
    _st7735_SendByte(ST7735_DATA, 0x8A);
    _st7735_SendByte(ST7735_DATA, 0x2A);

    //Power Control 5 (in Partial mode/ full-colors)
    _st7735_SendByte(ST7735_CMD, ST7735_PWCTR5);
    _st7735_SendByte(ST7735_DATA, 0x8A);
    _st7735_SendByte(ST7735_DATA, 0xEE);

    //VCOM Control 1
    _st7735_SendByte(ST7735_CMD, ST7735_VMCTR1);
    _st7735_SendByte(ST7735_DATA, 0x0E);

    //Инвертирование цветов
    _st7735_SendByte(ST7735_CMD, ST77XX_INVOFF);	//Display inversion off
    //_st7735_SendByte(ST7735_CMD, ST77XX_INVON); 	//Display inversion on

    //Memory data access control
    _st7735_SendByte(ST7735_CMD, ST77XX_MADCTL);
    _st7735_SendByte(ST7735_DATA, 0xC0);

    //Interface Pixel Format
    _st7735_SendByte(ST7735_CMD, ST77XX_COLMOD);
    _st7735_SendByte(ST7735_DATA, 0x05);

    //Gamma (‘+’polarity) Correction Characteristics Setting
    _st7735_SendByte(ST7735_CMD, ST7735_GMCTRP1);
    _st7735_SendByte(ST7735_DATA, 0x02);
    _st7735_SendByte(ST7735_DATA, 0x1C);
    _st7735_SendByte(ST7735_DATA, 0x07);
    _st7735_SendByte(ST7735_DATA, 0x12);
    _st7735_SendByte(ST7735_DATA, 0x37);
    _st7735_SendByte(ST7735_DATA, 0x32);
    _st7735_SendByte(ST7735_DATA, 0x29);
    _st7735_SendByte(ST7735_DATA, 0x2D);
    _st7735_SendByte(ST7735_DATA, 0x29);
    _st7735_SendByte(ST7735_DATA, 0x25);
    _st7735_SendByte(ST7735_DATA, 0x2B);
    _st7735_SendByte(ST7735_DATA, 0x39);
    _st7735_SendByte(ST7735_DATA, 0x00);
    _st7735_SendByte(ST7735_DATA, 0x01);
    _st7735_SendByte(ST7735_DATA, 0x03);
    _st7735_SendByte(ST7735_DATA, 0x10);

    //Gamma ‘-’polarity Correction Characteristics Setting
    _st7735_SendByte(ST7735_CMD, ST7735_GMCTRN1);
    _st7735_SendByte(ST7735_DATA, 0x03);
    _st7735_SendByte(ST7735_DATA, 0x1D);
    _st7735_SendByte(ST7735_DATA, 0x07);
    _st7735_SendByte(ST7735_DATA, 0x06);
    _st7735_SendByte(ST7735_DATA, 0x2E);
    _st7735_SendByte(ST7735_DATA, 0x2C);
    _st7735_SendByte(ST7735_DATA, 0x29);
    _st7735_SendByte(ST7735_DATA, 0x2D);
    _st7735_SendByte(ST7735_DATA, 0x2E);
    _st7735_SendByte(ST7735_DATA, 0x2E);
    _st7735_SendByte(ST7735_DATA, 0x37);
    _st7735_SendByte(ST7735_DATA, 0x3F);
    _st7735_SendByte(ST7735_DATA, 0x00);
    _st7735_SendByte(ST7735_DATA, 0x00);
    _st7735_SendByte(ST7735_DATA, 0x02);
    _st7735_SendByte(ST7735_DATA, 0x10);

    //Normal Display Mode On
    _st7735_SendByte(ST7735_CMD, ST77XX_NORON);
    ST7735_Delay_mS(10);

    //Display On
    _st7735_SendByte(ST7735_CMD, ST77XX_DISPON);
    ST7735_Delay_mS(100);

    //chip_select_disable
    ST7735_UNSELECT;

    st7735_clear(color);
}
//***********************************************************
void st7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    // column address set
	_st7735_SendCmd(ST77XX_CASET);
    uint8_t data[] = { 	0x00,
    					x0 + ST7735_XSTART,
						0x00,
						x1 + ST7735_XSTART };
    _st7735_SendDataBuf(data, sizeof(data));

    // row address set
    _st7735_SendCmd(ST77XX_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    _st7735_SendDataBuf(data, sizeof(data));

    // write to RAM
    _st7735_SendCmd(ST77XX_RAMWR);
}
//***********************************************************
void st7735_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color){

	ST7735_SELECT;

	st7735_SetAddressWindow(x0, y0, x1, y1);

   //количество пикселей заданной области
   uint16_t len = (uint16_t)((uint16_t)(x1-x0+1) * (uint16_t)(y1-y0+1));

#ifdef HW_SPI

   ST7735_RS_High; //gpio_set(GPIOB,DC);//Передаем данные

   //Перенастройка SPI на передачу 16 битов.
   ST7735_SPI->CR1 &= ~SPI_CR1_SPE; 			//CR1_SPE_Reset;      // disable SPI for setup

   ST7735_SPI->CR1 |= (SPI_CR1_MSTR |			//SPI_Mode_Master 	|
		   	   	   	   SPI_CR1_DFF 	|    		//SPI_DataSize_16b |
					   SPI_CR1_SSM  |	   		//SPI_NSS_Soft		|
					   (0 << SPI_CR1_BR_Pos))|	//SPI_BaudRatePrescaler_2);
					   SPI_CR1_SPE;				//CR1_SPE_Set; 		// enable SPI

   //Передача данных
   for (uint16_t i = 0; i < len; i++)
   {
       while (!(ST7735_SPI->SR & SPI_SR_TXE));
       ST7735_SPI->DR = color;
   }

   //Ждем окончания передачи последнего байта данных.
   while (!(ST7735_SPI->SR & SPI_SR_TXE) || (ST7735_SPI->SR & SPI_SR_BSY));

   ST7735_UNSELECT;

   //Перенастройка SPI на передачу 8 битов.
   ST7735_SPI->CR1 &= ~SPI_CR1_SPE;				// disable SPI for setup

   ST7735_SPI->CR1   = 0;
   ST7735_SPI->CR1  |= (SPI_CR1_MSTR |			//SPI_Mode_Master 	|
  		   	   	   	    //SPI_CR1_DFF 	|    	//SPI_DataSize_8b |
  					    SPI_CR1_SSM  |	   		//SPI_NSS_Soft		|
						SPI_CR1_SSI  | 			//обеспечить высокий уровень программного NSS
  					    (0 << SPI_CR1_BR_Pos))|	//SPI_BaudRatePrescaler_2);
  					    SPI_CR1_SPE;		    //CR1_SPE_Set; 		// enable SPI

#else

   uint8_t c1 = color >> 8;
   uint8_t c2 = (uint8_t)color;

   ST7735_RS_High;	//gpio_set(GPIOB,DC);

   for (uint16_t i = 0; i < len; i++)
   {
		_st7735_SendByte(ST7735_DATA, c1);
		_st7735_SendByte(ST7735_DATA, c2);
   }

   //chip select disable
   ST7735_UNSELECT;
#endif
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void st7735_PrintChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {

	uint32_t i, b, j;
	uint8_t data[2];

	st7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];

        for(j = 0; j < font.width; j++)
        {
            if((b << j) & 0x8000)
            {
                data[0] = color >> 8,
                data[1] = color & 0xFF;
                _st7735_SendDataBuf(data, sizeof(data));
            }
            else
            {
            	data[0] = bgcolor >> 8;
            	data[1] = bgcolor & 0xFF;
                _st7735_SendDataBuf(data, sizeof(data));
            }
        }
    }
}
//***********************************************************
void st7735_PrintString(uint16_t x, uint16_t y, char* str, FontDef font, uint16_t color, uint16_t bgcolor) {

	//chip select enable
	ST7735_SELECT;

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

        st7735_PrintChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    //chip select disable
    ST7735_UNSELECT;
}
//***********************************************************
void st7735_BinToDec(uint16_t x, uint16_t y, uint32_t var, uint32_t numDigit, FontDef font, uint16_t color, uint16_t bgcolor){

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

	ST7735_SELECT;//chip_select_enable

	for(uint8_t i = 0; i < numDigit; i++)
	{
		var = '0' + decArray[(numDigit - 1) - i];

		st7735_PrintChar(pos_x, y, (char)var, font, color, bgcolor);

		pos_x = pos_x + font.width;
	}

	ST7735_UNSELECT; //chip_select_disable
}
//***********************************************************
void st7735_BinToDec2(uint16_t x, uint16_t y, uint32_t num, FontDef font, uint16_t color, uint16_t bgcolor){

	#define NUM_LEN 10

	uint8_t n[NUM_LEN];
	uint8_t *s = n + (NUM_LEN - 1);
	//uint8_t len = 0;
	//--------------------
	*s = 0;	// EOL
	do
	{

		*(--s) = ('0' + num%10);
		num = num / 10;

		//len++;
		//if(len > (NUM_LEN-1)) break;

	} while (num > 0);
	//--------------------
	//Вывод на дисплей
	st7735_PrintString(x, y, (char*)s, font, color, bgcolor);
}
//***********************************************************
void st7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {

//    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
//    if((x + w - 1) >= ST7735_WIDTH)  return;
//    if((y + h - 1) >= ST7735_HEIGHT) return;

    ST7735_SELECT;

//    st7735_SetAddressWindow(x, y, x+w-1, y+h-1);
//    _st7735_SendDataBuf((uint8_t*)data, sizeof(uint16_t)*w*h);


    st7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    //количество пикселей заданной области
    uint16_t len = w * h;

    ST7735_RS_High;	//gpio_set(GPIOB,DC);	//данные

    //Перенастройка SPI на передачу 16 битов.
    ST7735_SPI->CR1 &= ~SPI_CR1_SPE; 			//CR1_SPE_Reset;      // disable SPI for setup

    ST7735_SPI->CR1 |= (SPI_CR1_MSTR |			//SPI_Mode_Master 	|
 		   	   	   	   SPI_CR1_DFF 	|    		//SPI_DataSize_16b |
 					   SPI_CR1_SSM  |	   		//SPI_NSS_Soft		|
 					   (0 << SPI_CR1_BR_Pos))|	//SPI_BaudRatePrescaler_2);
 					   SPI_CR1_SPE;				//CR1_SPE_Set; 		// enable SPI

    //Передача данных
    for (uint16_t i = 0; i < len; i++)
    {
        while (!(ST7735_SPI->SR & SPI_SR_TXE));
        ST7735_SPI->DR = *data;
        data++;
    }

    //Ждем окончания передачи последнего байта данных.
    while (!(ST7735_SPI->SR & SPI_SR_TXE) || (ST7735_SPI->SR & SPI_SR_BSY));

    ST7735_UNSELECT;

    //Перенастройка SPI на передачу 8 битов.
    ST7735_SPI->CR1 &= ~SPI_CR1_SPE;				// disable SPI for setup

    ST7735_SPI->CR1   = 0;
    ST7735_SPI->CR1  |= (SPI_CR1_MSTR |			//SPI_Mode_Master 	|
   		   	   	   	    //SPI_CR1_DFF 	|    	//SPI_DataSize_8b |
   					    SPI_CR1_SSM  |	   		//SPI_NSS_Soft		|
 						SPI_CR1_SSI  | 			//обеспечить высокий уровень программного NSS
   					    (0 << SPI_CR1_BR_Pos))|	//SPI_BaudRatePrescaler_2);
   					    SPI_CR1_SPE;		    //CR1_SPE_Set; 		// enable SPI

    //ST7735_UNSELECT;
}
//***********************************************************
//рисуем круг по координатам с радиусом - по Брезенхейму
void st7735_DrawCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint16_t color){

	signed char xc = 0;
	signed char yc = 0;
	signed char p  = 0;
	//--------------------
	//if(center_x > LCD_X_RES || center_y > LCD_Y_RES) return;
	//--------------------
	yc = radius;
	p  = 3 - (radius<<1);
	while(xc <= yc)
	{
		st7735_point(center_x + xc, center_y + yc, color);
		st7735_point(center_x + xc, center_y - yc, color);
		st7735_point(center_x - xc, center_y + yc, color);
		st7735_point(center_x - xc, center_y - yc, color);
		st7735_point(center_x + yc, center_y + xc, color);
		st7735_point(center_x + yc, center_y - xc, color);
		st7735_point(center_x - yc, center_y + xc, color);
		st7735_point(center_x - yc, center_y - xc, color);

		if(p < 0) p +=  (xc++ << 2) + 6;
		else      p += ((xc++ - yc--)<<2) + 10;
	}
}
//***********************************************************
//Draws a line between two points on the display - по Брезенхейму
void st7735_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

	int16_t dy       = 0;
	int16_t dx       = 0;
	int16_t stepx    = 0;
	int16_t stepy    = 0;
	int16_t fraction = 0;
	//--------------------
	//если передали в функцию некорректные данные - выходим.
	//if(x1>LCD_X_RES || x2>LCD_X_RES || y1>LCD_Y_RES || y2>LCD_Y_RES) return;
	//--------------------
	//Перемещение начала координат в нижний левый угол экрана.
	//y1 = 63 - y1;
	//y2 = 63 - y2;
	//--------------------

	dy = y2 - y1;
	dx = x2 - x1;

	if(dy < 0)
	{
		dy    = -dy;
		stepy = -1;
	}
	else stepy = 1;

	if(dx < 0)
	{
		dx    = -dx;
		stepx = -1;
	}
	else stepx = 1;

	dy <<= 1;
	dx <<= 1;
	//Lcd_Pixel(x1,y1,mode);
	st7735_point(x1, y1, color);

	if(dx > dy)
	{
	  fraction = dy - (dx >> 1);
	  while(x1 != x2)
		{
		  if(fraction >= 0)
			{
			  y1 += stepy;
			  fraction -= dx;
			}
		  x1 += stepx;
		  fraction += dy;
		  //Lcd_Pixel(x1,y1,mode);
		  st7735_point(x1, y1, color);
		}
	}
	else
	{
	  fraction = dx - (dy >> 1);
	  while(y1 != y2)
		{
		  if(fraction >= 0)
			{
			  x1 += stepx;
			  fraction -= dy;
			}
		  y1 += stepy;
		  fraction += dx;
		  //Lcd_Pixel(x1,y1,mode);
		  st7735_point(x1, y1, color);
		}
	}
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

