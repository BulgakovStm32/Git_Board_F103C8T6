/*
 *  st7735.h
 *
 *  Created on:
 *      Author:
 */

#ifndef __ST7735_H__
#define __ST7735_H__
//*******************************************************************************************
//*******************************************************************************************

#include "fonts_16bit.h"
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

extern const uint16_t testImg[];
//*******************************************************************************************
//*******************************************************************************************
#define HW_SPI

#define ST7735_SPI				SPI1

//************************************
//SCK - SPI SCK
#define ST7735_SCK_GPIO_PORT	GPIOB
#define ST7735_SCK_GPIO_PIN		13
#define ST7735_SCK_Low			GPIO_PIN_Low(ST7735_SCK_GPIO_PORT, ST7735_SCK_GPIO_PIN)
#define ST7735_SCK_High			GPIO_PIN_High(ST7735_SCK_GPIO_PORT, ST7735_SCK_GPIO_PIN)

//SDA - SPI MOSI
#define ST7735_SDA_GPIO_PORT	GPIOB
#define ST7735_SDA_GPIO_PIN		15
#define ST7735_SDA_Low			GPIO_PIN_Low(ST7735_SDA_GPIO_PORT, ST7735_SDA_GPIO_PIN)
#define ST7735_SDA_High			GPIO_PIN_High(ST7735_SDA_GPIO_PORT, ST7735_SDA_GPIO_PIN)

//RST (RES) - сигнал сброса
#define ST7735_RST_GPIO_PORT	GPIOA
#define ST7735_RST_GPIO_PIN		4
#define ST7735_RST_Low			GPIO_PIN_Low(ST7735_RST_GPIO_PORT, ST7735_RST_GPIO_PIN)
#define ST7735_RST_High			GPIO_PIN_High(ST7735_RST_GPIO_PORT, ST7735_RST_GPIO_PIN)

//RS (DC) - сигнал выбора передачи данных или команд
#define ST7735_RS_GPIO_PORT		GPIOA
#define ST7735_RS_GPIO_PIN		3
#define ST7735_RS_Low			GPIO_PIN_Low(ST7735_RS_GPIO_PORT, ST7735_RS_GPIO_PIN)
#define ST7735_RS_High			GPIO_PIN_High(ST7735_RS_GPIO_PORT, ST7735_RS_GPIO_PIN)

//CS - Chip Select
#define ST7735_CS_GPIO_PORT		GPIOA
#define ST7735_CS_GPIO_PIN		2
#define ST7735_CS_Low			GPIO_PIN_Low(ST7735_CS_GPIO_PORT, ST7735_CS_GPIO_PIN)
#define ST7735_CS_High			GPIO_PIN_High(ST7735_CS_GPIO_PORT, ST7735_CS_GPIO_PIN)

#define ST7735_SELECT			ST7735_CS_Low
#define ST7735_UNSELECT			ST7735_CS_High

//LED - подсветка
#define ST7735_LED_GPIO_PORT	GPIOA
#define ST7735_LED_GPIO_PIN		1
#define ST7735_LED_Low			GPIO_PIN_Low(ST7735_LED_GPIO_PORT, ST7735_LED_GPIO_PIN)
#define ST7735_LED_High			GPIO_PIN_High(ST7735_LED_GPIO_PORT, ST7735_LED_GPIO_PIN)

#define	ST7735_Delay_mS(delay)		DELAY_mS(delay)

//************************************
#define ST7735_CMD 	0x00	//LCD_C     0x00
#define ST7735_DATA	0x01	//LCD_D     0x01

#define ST7735_WIDTH  		128	//кол-во пикселей по ширине (по Х)
#define ST7735_HEIGHT 		160	//кол-во пикселей по высоте (по Y)

//#define LCD_LEN   	(uint16_t)((LCD_X * LCD_Y) / 8)

//************************************
#define FONT_SIZE_1 	(uint8_t)0x01
#define FONT_SIZE_2 	(uint8_t)0x02
#define FONT_SIZE_3 	(uint8_t)0x03

// some flags for initR() :(
#define INITR_GREENTAB 		0x00
#define INITR_REDTAB 		0x01
#define INITR_BLACKTAB 		0x02
#define INITR_18GREENTAB 	INITR_GREENTAB
#define INITR_18REDTAB 		INITR_REDTAB
#define INITR_18BLACKTAB 	INITR_BLACKTAB
#define INITR_144GREENTAB 	0x01
#define INITR_MINI160x80 	0x04
#define INITR_HALLOWING 	0x05

// Some register settings
#define ST7735_MADCTL_BGR 	0x08
#define ST7735_MADCTL_MH 	0x04

#define ST7735_FRMCTR1 	0xB1
#define ST7735_FRMCTR2 	0xB2
#define ST7735_FRMCTR3 	0xB3
#define ST7735_INVCTR 	0xB4
#define ST7735_DISSET5 	0xB6

#define ST7735_PWCTR1 	0xC0
#define ST7735_PWCTR2 	0xC1
#define ST7735_PWCTR3 	0xC2
#define ST7735_PWCTR4 	0xC3
#define ST7735_PWCTR5 	0xC4
#define ST7735_VMCTR1 	0xC5

#define ST7735_PWCTR6 	0xFC

#define ST7735_GMCTRP1	0xE0
#define ST7735_GMCTRN1 	0xE1

// Some ready-made 16-bit ('565') color settings:
//#define ST7735_BLACK 	ST77XX_BLACK
//#define ST7735_WHITE 	ST77XX_WHITE
//#define ST7735_RED 		ST77XX_RED
//#define ST7735_GREEN 	ST77XX_GREEN
//#define ST7735_BLUE 	ST77XX_BLUE
//#define ST7735_CYAN 	ST77XX_CYAN
//#define ST7735_MAGENTA	ST77XX_MAGENTA
//#define ST7735_YELLOW 	ST77XX_YELLOW
//#define ST7735_ORANGE 	ST77XX_ORANGE

#define ST_CMD_DELAY 		0x80 // special signifier for command lists

#define ST77XX_NOP 			0x00
#define ST77XX_SWRESET 		0x01
#define ST77XX_RDDID 		0x04
#define ST77XX_RDDST 		0x09

#define ST77XX_SLPIN 		0x10
#define ST77XX_SLPOUT 		0x11
#define ST77XX_PTLON 		0x12
#define ST77XX_NORON 		0x13

#define ST77XX_INVOFF 		0x20
#define ST77XX_INVON 		0x21
#define ST77XX_DISPOFF 		0x28
#define ST77XX_DISPON 		0x29
#define ST77XX_CASET 		0x2A
#define ST77XX_RASET 		0x2B
#define ST77XX_RAMWR 		0x2C
#define ST77XX_RAMRD 		0x2E

#define ST77XX_PTLAR 		0x30
#define ST77XX_TEOFF 		0x34
#define ST77XX_TEON 		0x35
#define ST77XX_MADCTL 		0x36
#define ST77XX_COLMOD 		0x3A

#define ST77XX_MADCTL_MY 	0x80
#define ST77XX_MADCTL_MX 	0x40
#define ST77XX_MADCTL_MV 	0x20
#define ST77XX_MADCTL_ML 	0x10
#define ST77XX_MADCTL_RGB 	0x00

#define ST77XX_RDID1 		0xDA
#define ST77XX_RDID2 		0xDB
#define ST77XX_RDID3 		0xDC
#define ST77XX_RDID4 		0xDD

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_COLOR_BLACK 		0x0000
#define ST77XX_COLOR_WHITE 		0xFFFF
#define ST77XX_COLOR_RED 		0xF800
#define ST77XX_COLOR_GREEN 		0x07E0
#define ST77XX_COLOR_BLUE 		0x001F
#define ST77XX_COLOR_CYAN 		0x07FF
#define ST77XX_COLOR_MAGENTA 	0xF81F
#define ST77XX_COLOR_YELLOW 	0xFFE0
#define ST77XX_COLOR_ORANGE		0xFC00

//************************************
//Макрос заливки всего экрана
#define st7735_clear(color) 			st7735_Fill(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color)

//Макрос для рисования прямоугольников, с заданием начальной точки, ширины и высоты
#define st7735_rectangle(x0, y0, w, h, c)	st7735_Fill(x0, y0, x0+w, y0+h, c)

//Макрос рисования точки
#define st7735_point(x,y,c)			st7735_Fill(x, y, x, y, c)

//Макрос рисования вертикальной линии
#define st7735_line_vertical(x,y,l,d,c) st7735_Fill(x, y, x+d-1, y+l-1, c)

//Макрос рисования горизонтальной линии
#define st7735_line_horizont(x,y,l,d,c)	st7735_Fill(x, y, x+l-1, y+d-1, c)

//*******************************************************************************************
//*******************************************************************************************
void st7735_Init(uint16_t color);

void st7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

void st7735_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

void st7735_PrintChar  (uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void st7735_PrintString(uint16_t x, uint16_t y, char* str, FontDef font, uint16_t color, uint16_t bgcolor);

void st7735_BinToDec(uint16_t x, uint16_t y, uint32_t var, uint32_t numDigit, FontDef font, uint16_t color, uint16_t bgcolor);
void st7735_BinToDec2(uint16_t x, uint16_t y, uint32_t num, FontDef font, uint16_t color, uint16_t bgcolor);

void st7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void st7735_DrawCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint16_t color);
void st7735_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

//*******************************************************************************************
//*******************************************************************************************
#endif      // __ST7735_H__



