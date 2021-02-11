/*
 * main.c
 *
 *  Created on: Dec 18, 2020
 *      Author: belyaev
 */
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
typedef struct{
	uint16_t DS18B20 :1;

}Flags_t;

Flags_t	FlagsStr;
//---------------------------
typedef struct{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}Time_t;
static Time_t Time;
//---------------------------
static uint32_t secCounter  = 0;
//static uint16_t Temperature = 0;
//*******************************************************************************************
//*******************************************************************************************
void IncrementOnEachPass(uint32_t *var, uint16_t event){

		   uint16_t riseReg  = 0;
	static uint16_t oldState = 0;
	//-------------------
	riseReg  = (oldState ^ event) & event;
	oldState = event;
	if(riseReg) (*var)++;
}
//************************************************************
void Time_Calculation(uint32_t count){

	Time.hour =  count / 3600;
	Time.min  = (count % 3600) / 60;
	Time.sec  =  count % 60;
}
//************************************************************
void Led_Blink(void){

	if(Blink(INTERVAL_100_mS)) LedPC13On();
	else					   LedPC13Off();

}
//************************************************************
void Temperature_Get(uint16_t *pData){

	if(FlagsStr.DS18B20 == 1)
		{
			FlagsStr.DS18B20 = 0;
			*pData = DS18B20_ReadTemperature();
			DS18B20_StartConvertTemperature();
		}
}
//************************************************************
uint8_t tic(uint16_t event){

	  	   uint16_t riseReg  = 0;
	static uint16_t oldState = 0;
	//-------------------
	riseReg  = (oldState ^ event) & event;
	oldState = event;
	if(riseReg) return 1;
	else 		return 0;
}
//*******************************************************************************************
//*******************************************************************************************
//Работа с DS2782.

#define DS2782_ADDR (0b00110100 << 1)//(0x34 << 1)
#define DS2782_I2C	I2C1

//---------------------------
typedef struct{
	uint16_t hour;
	uint8_t  min;
	uint8_t  sec;
}DS2782_t;

static DS2782_t DS2782Str;
//---------------------------


//************************************************************
uint16_t DS2782_ReadADC(void){

	uint8_t regAddr   = 0x01;
	uint8_t rxBuf[16] = {0};
	//-------------------
	//Этот кусок кода работает. Обмен с DS2782 есть.
	I2C_StartAndSendDeviceAddr(DS2782_I2C, DS2782_ADDR | I2C_MODE_WRITE);
	I2C_SendData(DS2782_I2C, &regAddr, 1);
	I2C_Stop(DS2782_I2C);

	//-------------------
	I2C_Read(DS2782_I2C, DS2782_ADDR, 0x01, rxBuf, 1);



	//Не работает!!!
//	I2C_StartAndSendDeviceAddr(DS2782_I2C, DS2782_ADDR | I2C_MODE_WRITE);
//	I2C_SendData(DS2782_I2C, &regAddr, 1);
//
//	I2C_StartAndSendDeviceAddr(DS2782_I2C, DS2782_ADDR | I2C_MODE_READ);
//	I2C_ReadData(DS2782_I2C, rxBuf, 4);

//	I2C_Stop(DS2782_I2C);


	//Не работает!!!
//	I2C_Read(DS2782_I2C, DS2782_ADDR, 0x00, rxBuf, 3);

//	I2C_SendData(DS2782_I2C, &regAddr, 1);
//
//	//while(!(I2C2->SR1 & I2C_SR1_BTF)){};//Ждем окончания передачи
//
//	I2C_StartAndSendDeviceAddr(DS2782_I2C, DS2782_ADDR | I2C_MODE_READ);
//	I2C_ReadData(DS2782_I2C, rxBuf, 4);
	//I2C_Stop(I2C2);

	return rxBuf[0];
}
//************************************************************


//*******************************************************************************************
//*******************************************************************************************
int main(void){

	uint16_t dsRes = 0;
//	uint16_t BmiT  = 0;
	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	//***********************************************
	SysTick_Init();
	__enable_irq();


	I2C_Init(SSD1306_I2C);//I2C_Int_Init(SSD1306_I2C);
	//***********************************************
//	__disable_irq();
	msDelay(500);
	//***********************************************
	//OLED SSD1306
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//DS18B20
	DS18B20_Init(DS18B20_Resolution_12_bit);
	DS18B20_StartConvertTemperature();
	//***********************************************
	//DS2782.

	//***********************************************
	msDelay(500);
	//************************************************************************************
	while(1)
		{
			msDelay(10);
			//***********************************************
			//Мигание светодиодами.
			Led_Blink();
			//DS18B20.
			Temperature_Get(&dsRes);
			//Инкримент счетчика секунд.
			IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));
			//Преобразование времени
			Time_Calculation(secCounter);
			//***********************************************
			//LCD 128x64 - Работает.
			Lcd_String(1, 1);
			Lcd_Print("DS2782 Test");
			//Вывод времени.
			Lcd_String(1, 2);
			Lcd_Print("Time:");
			Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
			Lcd_Chr(':');
			Lcd_BinToDec(Time.min, 2, LCD_CHAR_SIZE_NORM); //минуты
			Lcd_Chr(':');
			Lcd_BinToDec(Time.sec, 2, LCD_CHAR_SIZE_NORM); //секунды
			//Вывод темперетуры DS18B20.
			Lcd_String(1, 3);
			Lcd_Print("DS18B20 =");
			if(DS18B20_GetTemperatureSign() & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
			else                    								Lcd_Chr('+');
			Lcd_BinToDec(dsRes/10, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('.');
			Lcd_BinToDec(dsRes%10, 1, LCD_CHAR_SIZE_NORM);
			Lcd_Print(" C");

			Lcd_String(21, 1);
			if(Blink(INTERVAL_50_mS))Lcd_Chr(':');
			else                     Lcd_Chr(' ');

			Lcd_Update();//вывод сделан для SSD1306
			Lcd_Clear();
			//***********************************************
			//Работа с микросхемой DS2782.
			DS2782_ReadADC();

			//***********************************************
			//датчик температуры и влажности AHT10.
			//if(tic(Blink(INTERVAL_250_mS))) AHT10_ReadData();

			//Вывод темперартуры.
//			Lcd_String(1, 4);
//			Lcd_Print("AHT10_T =");
//			if(AHT10_GetTemperatureSign() == AHT10_SIGN_NEGATIVE)Lcd_Chr('-');
//			else                    							 Lcd_Chr('+');
//
//			Lcd_BinToDec(AHT10()->Temperature / 10, 2, LCD_CHAR_SIZE_NORM);
//			Lcd_Chr('.');
//			Lcd_BinToDec(AHT10()->Temperature % 10, 1, LCD_CHAR_SIZE_NORM);
//			Lcd_Print(" C");
//			//Вывод влажности.
//			AHT10()->Humidity = Filter_EMA(AHT10()->Humidity);
//			Lcd_String(1, 5);
//			Lcd_Print("AHT10_H = ");
//			Lcd_BinToDec(AHT10()->Humidity / 100, 2, LCD_CHAR_SIZE_NORM);
//			Lcd_Chr('.');
//			Lcd_BinToDec(AHT10()->Humidity % 10, 1, LCD_CHAR_SIZE_NORM);
//			Lcd_Print(" %");
			//***********************************************
			/* Sleep */
			//__WFI();
		}
	//************************************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_Handler(void){

	static uint16_t msCountForDS18B20 = 0;
	//-----------------------------
	//Отсчет таймаута для датчика температуры.
	if(++msCountForDS18B20 >= 1000)
		{
			msCountForDS18B20 = 0;
			FlagsStr.DS18B20  = 1;
		}
	//-----------------------------
	msDelay_Loop();
	Blink_Loop();
	Encoder()->Loop();
}
//*******************************************************************************************
//*******************************************************************************************
