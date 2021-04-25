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
int main(void){

	uint16_t dsRes = 0;
//	uint16_t BmiT  = 0;
	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	Adc_Init();
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
			//***********************************************
			//Работа с АЦП.
			//Регулярный канал АЦП.
			Lcd_String(1, 5);
			Lcd_Print("RegularADC=");
			//Lcd_BinToDec(Adc_GetMeas(0), 4, LCD_CHAR_SIZE_NORM);
			Lcd_BinToDec(Adc_GetRegDR(ADC1), 4, LCD_CHAR_SIZE_NORM);


			//Инжектированный канал АЦП.
			Lcd_String(1, 6);
			Lcd_Print("InjectedADC=");
			Lcd_BinToDec(Adc_GetRegJDRx(ADC1, 4), 4, LCD_CHAR_SIZE_NORM);
			//***********************************************
			Lcd_Update();//вывод сделан для SSD1306
			Lcd_Clear();
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
//******************************************************************************************
