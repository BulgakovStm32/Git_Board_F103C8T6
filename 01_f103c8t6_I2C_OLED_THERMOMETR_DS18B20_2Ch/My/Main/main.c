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
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}Time_t;

static Time_t Time;
//---------------------------
DS18B20_t Sensor_1;
DS18B20_t Sensor_2;
DS18B20_t Sensor_3;
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
void Temperature_Display(DS18B20_t *sensor, uint8_t cursor_x, uint8_t cursor_y){

	uint32_t temperature = sensor->TEMPERATURE;
	//-------------------
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_PrintBig("Sens");
	Lcd_BinToDec(sensor->SENSOR_NUMBER, 1, LCD_CHAR_SIZE_BIG);
	Lcd_PrintBig("= ");
	if(TemperatureSens_Sign(sensor) & DS18B20_SIGN_NEGATIVE)Lcd_ChrBig('-');
	else                    								Lcd_ChrBig('+');
	Lcd_BinToDec(temperature/10, 2, LCD_CHAR_SIZE_BIG);
	Lcd_ChrBig('.');
	Lcd_BinToDec(temperature%10, 1, LCD_CHAR_SIZE_BIG);
	Lcd_Print("o ");
	Lcd_ChrBig('C');
}
//************************************************************
void Temperature_TxtDisplay(DS18B20_t *sensor){

	uint32_t temperature = sensor->TEMPERATURE;
	//-------------------
	Txt_Print("Sens");
	Txt_BinToDec(sensor->SENSOR_NUMBER, 1);
	Txt_Print("= ");
	if(TemperatureSens_Sign(sensor) & DS18B20_SIGN_NEGATIVE)Txt_Chr('-');
	else                    								Txt_Chr('+');
	Txt_BinToDec(temperature/10, 2);
	Txt_Chr('.');
	Txt_BinToDec(temperature%10, 1);
	Txt_Print(" C\n");
}
//*******************************************************************************************
//*******************************************************************************************
void Task_Temperature_Read(void){

	TemperatureSens_ReadTemperature(&Sensor_1);
	TemperatureSens_ReadTemperature(&Sensor_2);
	TemperatureSens_ReadTemperature(&Sensor_3);
	//-----------------------------
	//Scheduler_SetTimerTask(Task_Temperature_Read, 1000);
}
//************************************************************
void Task_Lcd(void){

	static uint32_t secCounter = 0;
	//-----------------------------
	if(Led_Blink(RTOS_GetTickCount(), 1000, 50)) LedPC13On();
	else										 LedPC13Off();


	//Мигание светодиодами.
	IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));//Инкримент счетчика секунд.
	Time_Calculation(secCounter);						     //Преобразование времени
	//-----------------------------
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_THERMOMETER_(+BT)");

	//Вывод времени.
	Lcd_SetCursor(1, 2);
	Lcd_Print("Time: ");
	Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(Time.min,  2, LCD_CHAR_SIZE_NORM); //минуты
	Lcd_Chr(':');
	Lcd_BinToDec(Time.sec,  2, LCD_CHAR_SIZE_NORM); //секунды

	//Вывод темперетуры DS18B20.
	Temperature_Display(&Sensor_1, 1, 3);
	Temperature_Display(&Sensor_2, 1, 5);
	Temperature_Display(&Sensor_3, 1, 7);
	//-----------------------------
	//Scheduler_SetTask(Task_Lcd);
}
//************************************************************
void Task_LcdUpdate(void){

	RTOS_SetTask(Task_Lcd, 0, 0);
	Lcd_Update(); //вывод сделан для SSD1306
	Lcd_ClearVideoBuffer();
	//-----------------------------
	//Scheduler_SetTask(Task_LcdUpdate);
	//Scheduler_SetTimerTask(Task_LcdUpdate, 1000);
	//LedPC13Toggel();
}
//************************************************************
void Task_UartSend(void){

	Txt_Chr('\f');

	Txt_Print("_THERMOMETER_(+BT)\n");

	Txt_Print("Time: ");
	Txt_BinToDec(Time.hour, 2);//часы
	Txt_Chr(':');
	Txt_BinToDec(Time.min, 2); //минуты
	Txt_Chr(':');
	Txt_BinToDec(Time.sec, 2); //секунды
	Txt_Chr('\n');

	//Вывод темперетуры DS18B20.
	Temperature_TxtDisplay(&Sensor_1);
	Temperature_TxtDisplay(&Sensor_2);
	Temperature_TxtDisplay(&Sensor_3);
	//Txt_Chr('\n');

	DMA1Ch4StartTx(Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	microDelay_Init();
	Uart1Init(USART1_BRR);
	//***********************************************
	//Ини-я DS18B20
	Sensor_1.GPIO_PORT     = GPIOA;
	Sensor_1.GPIO_PIN      = 2;
	Sensor_1.SENSOR_NUMBER = 1;
	Sensor_1.RESOLUTION    = DS18B20_Resolution_12_bit;
	TemperatureSens_GpioInit(&Sensor_1);
	TemperatureSens_SetResolution(&Sensor_1);
	TemperatureSens_StartConvertTemperature(&Sensor_1);

	Sensor_2.GPIO_PORT     = GPIOA;
	Sensor_2.GPIO_PIN      = 1;
	Sensor_2.SENSOR_NUMBER = 2;
	Sensor_2.RESOLUTION    = DS18B20_Resolution_12_bit;
	TemperatureSens_GpioInit(&Sensor_2);
	TemperatureSens_SetResolution(&Sensor_2);
	TemperatureSens_StartConvertTemperature(&Sensor_2);

	Sensor_3.GPIO_PORT     = GPIOA;
	Sensor_3.GPIO_PIN      = 0;
	Sensor_3.SENSOR_NUMBER = 3;
	Sensor_3.RESOLUTION    = DS18B20_Resolution_12_bit;
	TemperatureSens_GpioInit(&Sensor_3);
	TemperatureSens_SetResolution(&Sensor_3);
	TemperatureSens_StartConvertTemperature(&Sensor_3);
	//***********************************************
	//Ини-я OLED SSD1306
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//Ини-я диспетчера.
//	Scheduler_Init();
//
//	//Постановка задач в очередь.
//	Scheduler_SetTask(Task_UartSendData);
//	Scheduler_SetTask(Task_Temperature_Read);
//	Scheduler_SetTask(Task_Lcd);
//	Scheduler_SetTask(Task_LcdUpdate);

	RTOS_Init();

	RTOS_SetTask(Task_Temperature_Read, 0, 1000);
	RTOS_SetTask(Task_LcdUpdate, 0, 20);
	RTOS_SetTask(Task_UartSend, 0, 1000);
	//***********************************************
	microDelay(100000);
	__enable_irq();
	//************************************************************************************
	while(1)
	{
		//Scheduler_Loop();
		RTOS_DispatchLoop();
		//__WFI();//Sleep
	}
	//************************************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_Handler(void){

	//Scheduler_TimerServiceLoop();
	RTOS_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
}
//*******************************************************************************************
//******************************************************************************************
