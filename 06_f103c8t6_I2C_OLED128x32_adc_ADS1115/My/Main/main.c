/*
 * 	main.c
 *
 *  Created on: 19 октября 2021 года.
 *  Autho     : Беляев А.А.
 *
 *	Описание        : Трехканальный измеритель температуры.
 *  Датчики         : DS18B20 - 3шт.
 *  Вывод информации: OLED дисплей SSD1306 128x64;
 *      			  UART1(PA9-U1TX, PA10-U1RX), скорость 9600.
 */
//*******************************************************************************************
//*******************************************************************************************
#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
typedef struct{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}Time_t;

static Time_t Time;
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
uint32_t Led_Blink(uint32_t millis, uint32_t period, uint32_t switch_on_time){

	static uint32_t millisOld = 0;
	static uint32_t flag      = 0;
	//-------------------
	if((millis - millisOld) >= (flag ? (period - switch_on_time) : switch_on_time ))
	{
		millisOld = millis;
		flag = !flag;
	}
	return flag;
}
//************************************************************
void Time_Display(uint8_t cursor_x, uint8_t cursor_y){

	//Вывод времени.
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_Print("Time: ");
	Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(Time.min,  2, LCD_CHAR_SIZE_NORM); //минуты
	Lcd_Chr(':');
	Lcd_BinToDec(Time.sec,  2, LCD_CHAR_SIZE_NORM); //секунды
}
//*******************************************************************************************
//*******************************************************************************************
//Работа с АЦП ADS1115.




//************************************************************
//Работа с микросхемой ADS1115.
void Task_ADS1115(void){

	ADS1115_SelectInput(0);
	uint16_t adcTemp = ADS1115_GetData();

}
//*******************************************************************************************
//*******************************************************************************************
//Вывод значения встроенного АЦП.
#define VREF      2480UL 				  	//Опроное напряжение в мв. Измеряется внешним вольтметром как можно точнее.
#define VDD		  3254UL 				  	//Напряжение питания в мв. Измеряется внешним вольтметром как можно точнее.
#define ADC_RES	  4096UL 				    //Количество квантов АЦП. 2^12 = 4096.
#define ADC_QUANT ((VDD * 10000) / ADC_RES) //Вес кванта АЦП.
#define K_RESIST_DIVIDE 2

#define ADC_CH_VREF  9    //канал АЦП, к которому подключен внешний ИОН.
#define ADC_CH_MEAS  8    //канал АЦП, которым измеряем напряжениа на АКБ.

typedef struct{
	uint32_t Bat_V;
	uint32_t Vdd_V;
	uint32_t Vref_V;
}AdcMeas_t;

AdcMeas_t	AdcMeas;
//----------------------------------------------
void Task_AdcMeas(void){

	AdcMeas.Bat_V  = ((Adc_GetMeas(ADC_CH_MEAS) * ADC_QUANT) / 10000) * K_RESIST_DIVIDE;//Измерение напряжения АКБ.
	AdcMeas.Vref_V = Adc_GetMeas(ADC_CH_VREF);	                   //Измерение напряжения внешнего ИОН.
	AdcMeas.Vdd_V  = (VREF * ADC_RES) / Adc_GetMeas(ADC_CH_VREF);  //Расчет напряжения питания через внешний ИОН.
}
//************************************************************
//Работа с микросхемой DS2782.
void Task_DS2782(void){

//	DS2782_GetI2cAddress(&DS2782); //получение адреса на шине I2C
//	DS2782_GetID(&DS2782);         //получение Unique ID (factory option)
//	DS2782_GetTemperature(&DS2782);//получение температуры.
//	DS2782_GetVoltage(&DS2782);    //получение напряжения на АКБ.
//	DS2782_GetCurrent(&DS2782);    //получения тока потребления от АКБ.
}
//************************************************************
void Task_Lcd_DS2782(void){

	//Вывод времени.
	Time_Display(1, 2);

	//Вывод адреса на шине I2C
	Lcd_SetCursor(1, 3);
	Lcd_Print("DS2782_I2C_ADDR:");
	Lcd_Print("0x");
//	Lcd_u8ToHex(DS2782.I2C_Address);

	//Вывод Unique ID (factory option)
	Lcd_SetCursor(1, 4);
	Lcd_Print("DS2782_ID:");
	Lcd_Print("0x");
//	Lcd_u8ToHex(DS2782.ID);

	//Вывод температуры.
	Lcd_SetCursor(1, 5);
	Lcd_Print("Bat_T=");
//	Lcd_BinToDec(DS2782.Temperature/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
//	Lcd_BinToDec(DS2782.Temperature%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" C");

	//Вывод напряжения на АКБ.
	Lcd_SetCursor(1, 6);
	Lcd_Print("Bat_U=");
//	Lcd_BinToDec(DS2782.Voltage/100, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
//	Lcd_BinToDec(DS2782.Voltage%100, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('V');

	//Вывод тока потребления от АКБ.
	int16_t temp = 0;//DS2782.Current;

	Lcd_SetCursor(1, 7);
	Lcd_Print("Bat_I=");

	if(temp < 0)
	{
		temp = (temp ^ 0xFFFF) + 1;//Уберем знак.
		Lcd_Chr('-');
	}
	else Lcd_Chr(' ');

	Lcd_BinToDec(temp, 4, LCD_CHAR_SIZE_NORM);
	Lcd_Print("mA");

	//----------------------------------------------
	//Вывод значения встроенного АЦП.

	//Измерение напряжения АКБ.
	Lcd_SetCursor(14, 1);
	Lcd_Print("BAT:");
	Lcd_BinToDec(AdcMeas.Bat_V, 4, LCD_CHAR_SIZE_NORM);

	//Измерение напряжения внешнего ИОН.
	Lcd_SetCursor(1, 8);
	Lcd_Print("ADC=");
	Lcd_BinToDec(Adc_GetMeas(ADC_CH_VREF), 5, LCD_CHAR_SIZE_NORM);

	//Расчет напряжения питания через внешний ИОН.
	Lcd_Print(" Vdd=");
	Lcd_BinToDec(AdcMeas.Vdd_V, 5, LCD_CHAR_SIZE_NORM);
}
//************************************************************
void Task_Lcd(void){

	//Мигание светодиодами.
//	if(Led_Blink(RTOS_GetTickCount(), 1000, 50)) LedPC13On();
//	else										 LedPC13Off();

	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_THERMOMETER_(+BT)");

	//Вывод времени.
	Time_Display(1, 2);

	//-----------------------------
	//Scheduler_SetTask(Task_Lcd);
}
//************************************************************
void Task_LcdUpdate(void){

	if(Led_Blink(RTOS_GetTickCount(), 1000, 50)) LedPC13On();
	else										 LedPC13Off();

	//RTOS_SetTask(Task_Lcd, 0, 0);
	RTOS_SetTask(Task_Lcd_DS2782, 0, 0);

	Lcd_Update(); //вывод сделан для SSD1306
	Lcd_ClearVideoBuffer();
	//-----------------------------
	//Scheduler_SetTask(Task_LcdUpdate);
	//Scheduler_SetTimerTask(Task_LcdUpdate, 1000);
	//LedPC13Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//***********************************************
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	microDelay_Init();
	//Uart1Init(USART1_BRR);
	//Adc_Init();

	microDelay(100000);//Эта задержка нужна для стабилизации напряжения патания.
					   //Без задержки LCD-дисплей не работает.
	//***********************************************
	//Ини-я дисплея.
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//Ини-я АЦП ADS1115.
	ADS1115_Init(ADS1115_I2C);
	//***********************************************
	//Ини-я диспетчера и постановка задач в очередь.
	RTOS_Init();
	RTOS_SetTask(Task_LcdUpdate, 0, 20);
	RTOS_SetTask(Task_ADS1115, 0, 1000);
	//***********************************************
	__enable_irq();
	//************************************************************************************
	while(1)
	{
		RTOS_DispatchLoop();
		//__WFI();//Sleep
	}
	//************************************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_Handler(void){

	static uint32_t secCounter = 0;
	IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));//Инкримент счетчика секунд.
	Time_Calculation(secCounter);						     //Преобразование времени

	RTOS_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
}
//*******************************************************************************************
//*******************************************************************************************
