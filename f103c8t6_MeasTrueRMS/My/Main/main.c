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

//	if(Blink(INTERVAL_100_mS)) LedPC13On();
//	else					   LedPC13Off();

	LedPC13Toggel();

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
//Измерение ~U: F=50Гц, Uамп = 1В, смещенеи 1,6В.
#define AC_MEAS_OFFSET	(ADC_REF / 2)

volatile uint16_t AcRmsMeas_r    = 0;
volatile uint16_t AcMeasOffset_r = 0;

//************************************************************
void AC_MeasLoop(void){

	static uint32_t ac_meas_acc     = 0; //накопитель измерений.
	static uint16_t ac_meas_counter = 0; //счетчик измерений.
	       int32_t  ac_meas         = 0;
	//-----------------------------
	ac_meas  = (Adc_GetRegDR(ADC1) * ADC_QUANT) / 10000;//Получили значение АЦП.
	ac_meas -= AcMeasOffset_r;//Вычитаем смещение.
	ac_meas *= ac_meas;       //Возведение в квадрат.
	ac_meas_acc += ac_meas;   //Накапливаем сумму измерений.

	if(++ac_meas_counter >= 20)
		{
			ac_meas_acc /= 20;
			AcRmsMeas_r  = (uint16_t)sqrtf((float)ac_meas_acc);//Извлекаем квадратный корень.
			ac_meas_counter = 0;
			ac_meas_acc     = 0;
		}
}
//************************************************************
uint16_t AC_GetMeas(void){

	return AcRmsMeas_r;
}
//************************************************************
void AC_MeasOffset(void){

	uint32_t avrMeas = 0;
	//-----------------------------
	for(uint32_t i = 0; i < 64; i++)
		{
			avrMeas += Adc_GetRegDR(ADC1);
		}
	avrMeas >>= 6;
	AcMeasOffset_r = (avrMeas * ADC_QUANT) / 10000;//Получили значение АЦП.
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	uint16_t dsRes = 0;
	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	Adc_Init();
	SysTick_Init();

	__enable_irq();
	//***********************************************
	//Инициализация OLED SSD1306
	I2C_Init(SSD1306_I2C);//I2C_Int_Init(SSD1306_I2C);
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//DS18B20
	DS18B20_Init(DS18B20_Resolution_12_bit);
	DS18B20_StartConvertTemperature();
	//***********************************************

	//***********************************************
	msDelay(500);

	AC_MeasOffset();
	//************************************************************************************
	while(1)
		{
			msDelay(100);
			//***********************************************
			Led_Blink();			      //Мигание светодиодами.
			Temperature_Get(&dsRes);//DS18B20.
			IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));//Инкримент счетчика секунд.
			Time_Calculation(secCounter);//Преобразование времени
			//***********************************************
			//Работа с дисплеем 128x64.
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
			Lcd_Print("RegularADC =");
			//Lcd_BinToDec(Adc_GetMeas(0), 4, LCD_CHAR_SIZE_NORM);

			//uint16_t volts = (Adc_GetRegDR(ADC1) * ADC_QUANT) / 10000;
			//Lcd_BinToDec(Average(volts, 16), 4, LCD_CHAR_SIZE_NORM);
			Lcd_BinToDec(AC_GetMeas(), 4, LCD_CHAR_SIZE_NORM);

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
	if(++msCountForDS18B20 >= 2000)
		{
			msCountForDS18B20 = 0;
			FlagsStr.DS18B20  = 1;
		}
	//-----------------------------
	//Измерение ~U: F=50Гц, Uамп = 1В, смещенеи 1,6В.
	AC_MeasLoop();
	//-----------------------------
	msDelay_Loop();
	Blink_Loop();
	Encoder()->Loop();
}
//*******************************************************************************************
//******************************************************************************************
