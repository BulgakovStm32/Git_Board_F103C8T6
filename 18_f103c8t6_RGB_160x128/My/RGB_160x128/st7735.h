#ifndef __ST7735_H__
#define __ST7735_H__
#include <stdint.h>
#include <stdbool.h>


// Port B
#define RST  4	//GPIO_Pin_4
#define BL   5	//GPIO_Pin_5
#define DC   3//6	//GPIO_Pin_6
// Port A
#define CS   4	//GPIO_Pin_4
#define CLK  5	//GPIO_Pin_5
#define DIN  6	//GPIO_Pin_7

#define LCD_C     0x00
#define LCD_D     0x01

#define LCD_X     84
#define LCD_Y     48
#define LCD_LEN   (uint16_t)((LCD_X * LCD_Y) / 8)

#define chip_select_enable()	GPIO_PIN_Low(GPIOA,CS)	//gpio_reset(GPIOA,CS)
#define chip_select_disable()	GPIO_PIN_High(GPIOA,CS)	//gpio_set(GPIOA,CS)

#define FONT_SIZE_1 (uint8_t)0x01
#define FONT_SIZE_2 (uint8_t)0x02
#define FONT_SIZE_3 (uint8_t)0x03


// some flags for initR() :(
#define INITR_GREENTAB 0x00
#define INITR_REDTAB 0x01
#define INITR_BLACKTAB 0x02
#define INITR_18GREENTAB INITR_GREENTAB
#define INITR_18REDTAB INITR_REDTAB
#define INITR_18BLACKTAB INITR_BLACKTAB
#define INITR_144GREENTAB 0x01
#define INITR_MINI160x80 0x04
#define INITR_HALLOWING 0x05

// Some register settings
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH 0x04

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Some ready-made 16-bit ('565') color settings:
#define ST7735_BLACK ST77XX_BLACK
#define ST7735_WHITE ST77XX_WHITE
#define ST7735_RED ST77XX_RED
#define ST7735_GREEN ST77XX_GREEN
#define ST7735_BLUE ST77XX_BLUE
#define ST7735_CYAN ST77XX_CYAN
#define ST7735_MAGENTA ST77XX_MAGENTA
#define ST7735_YELLOW ST77XX_YELLOW
#define ST7735_ORANGE ST77XX_ORANGE


#define ST_CMD_DELAY 0x80 // special signifier for command lists

#define ST77XX_NOP 0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID 0x04
#define ST77XX_RDDST 0x09

#define ST77XX_SLPIN 0x10
#define ST77XX_SLPOUT 0x11
#define ST77XX_PTLON 0x12
#define ST77XX_NORON 0x13

#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_RAMRD 0x2E

#define ST77XX_PTLAR 0x30
#define ST77XX_TEOFF 0x34
#define ST77XX_TEON 0x35
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1 0xDA
#define ST77XX_RDID2 0xDB
#define ST77XX_RDID3 0xDC
#define ST77XX_RDID4 0xDD

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

#define st7735_clear(x) st7735_fill(0,0x7f,0,0x9f,x)
#define st7735_rectangle(a,b,c,d,e) st7735_fill(a,a+b-1,c,c+d-1,e)
#define st7735_point(x,y,c) st7735_fill(x,x,y,y,c)
#define st7735_line_vertical(x,y,l,d,c)  st7735_fill(x,x+d-1,y,y+l-1,c)
#define st7735_line_horizont(x,y,l,d,c)  st7735_fill(x,x+l-1,y,y+d-1,c)

// functions
void st7735_init(uint16_t color);
void st7735_fill(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1, uint16_t color);
//void pcd8544_fill_fb(uint8_t value);
//void pcd8544_display_fb();
//void st7735_off();

#endif      // __ST7735_H__
