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
void Led_Blink1(uint32_t millis){

	static uint32_t millisOld = 0;
	static uint32_t flag      = 0;
	//-------------------
	if((millis - millisOld) >= (flag ? 900 : 100 ))
	{
		millisOld = millis;
		flag = !flag;
		LedPC13Toggel();
	}
}
//************************************************************
//void Temperature_Read(void){
//
//		   uint32_t millis    = Scheduler_GetTickCount();
//	static uint32_t millisOld = 0;
//	//-------------------
//	if(millis - millisOld >= 1000)
//	{
//		millisOld = millis;
//
//		TemperatureSens_ReadTemperature(&Sensor_1);
//		TemperatureSens_ReadTemperature(&Sensor_2);
//	}
//}
//************************************************************
void Temperature_Display(DS18B20_t *sensor, uint8_t cursor_x, uint8_t cursor_y){

	uint32_t temperature = sensor->TEMPERATURE;
	//-------------------
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_Print("Sens");
	Lcd_BinToDec(sensor->SENSOR_NUMBER, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('=');

	if(TemperatureSens_Sign(sensor) & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
	else                    								Lcd_Chr('+');

	Lcd_BinToDec(temperature/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(temperature%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" C (PinA");
	Lcd_BinToDec(sensor->GPIO_PIN, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Chr(')');

	Lcd_SetCursor(1, 6);
	Lcd_Print("_Str_");

	Lcd_SetCursor(1, 7);
	Lcd_Print("_Str_");

	Lcd_SetCursor(1, 8);
	Lcd_Print("_Str_");
}
//*******************************************************************************************
//*******************************************************************************************
#define RADIUS 15.0
#define X_0	   64
#define Y_0	   0
#define RADIAN	((2*M_PI)/120.0)
//************************************************************
void Task_UartSendData(void){

//	static uint8_t txBuf[] = {"_TERMOMETR_\r"};
	//-----------------------------
//	//BinToDecWithDot(timeStamp, txBuf);
//	txBuf[7] = '\t';
//
//	//BinToDecWithoutDot(encodTicks, txBuf+8);
//	txBuf[14] = '\t';
//
//	//BinToDecWithDot(angle, txBuf+15);
//	txBuf[22] = '\t';
//
//	//BinToDecWithDot(speed, txBuf+23);
//	txBuf[30] = '\r';



	//DMA1Ch4StartTx(txBuf, sizeof(txBuf)-1);
	UartTextBuf()[127] = '\r';

	DMA1Ch4StartTx(UartTextBuf(), 128);
	//-----------------------------
	//Scheduler_SetTimerTask(Task_UartSendData, 1000);
}
//************************************************************
void Task_Temperature_Read(void){

	TemperatureSens_ReadTemperature(&Sensor_1);
	TemperatureSens_ReadTemperature(&Sensor_2);
	TemperatureSens_ReadTemperature(&Sensor_3);
	//-----------------------------
	//Scheduler_SetTimerTask(Task_Temperature_Read, 1000);
}
//************************************************************
void Task_Lcd(void){

	static uint32_t x1 = 0;
	static uint32_t y1 = 0;
	static uint32_t secCounter = 0;
	//-----------------------------
	Led_Blink1(RTOS_GetTickCount());					     //Мигание светодиодами.
	IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));//Инкримент счетчика секунд.
	Time_Calculation(secCounter);						     //Преобразование времени
	//-----------------------------
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_THERMOMETER_");

	//Вывод времени.
	Lcd_SetCursor(1, 2);
	Lcd_Print("Time: ");
	Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(Time.min, 2, LCD_CHAR_SIZE_NORM); //минуты
	Lcd_Chr(':');
	Lcd_BinToDec(Time.sec, 2, LCD_CHAR_SIZE_NORM); //секунды

	//Вывод темперетуры DS18B20.
	Temperature_Display(&Sensor_1, 1, 3);
	Temperature_Display(&Sensor_2, 1, 4);
	Temperature_Display(&Sensor_3, 1, 5);

	//Рисование графики.
	float rad_temp = Time.sec * RADIAN;

	x1 = (uint32_t)(X_0 + RADIUS * (float)cos(rad_temp));
	y1 = (uint32_t)(Y_0 + RADIUS * (float)sin(rad_temp));

	Lcd_Line(X_0, Y_0, x1, y1, PIXEL_ON);
	//-----------------------------
	//Scheduler_SetTask(Task_Lcd);
}
//************************************************************
void Task_LcdUpdate(void){

	static uint32_t fps      = 0;
	static uint32_t fps_temp = 0;

	//-----------------------------
	RTOS_SetTask(Task_Lcd, 0, 0);
	//-----------------------------
	//Счетчик кадров в секунду
	if(!Blink(INTERVAL_1000_mS)) fps_temp++;
	if(Blink(INTERVAL_1000_mS) && fps_temp != 0)
	{
		fps = fps_temp;
		fps_temp = 0;
	}
	Lcd_SetCursor(16, 1);
	Lcd_Print("FPS=");
	Lcd_BinToDec(fps, 2, LCD_CHAR_SIZE_NORM);
	//-----------------------------
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

	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	microDelay_Init();
	Uart1Init(USART1_BRR);

	__enable_irq();
	msDelay(500);
	//***********************************************
	//Ини-я OLED SSD1306
	//I2C1_Init();
	//I2C1_DMAInit();
	SSD1306_Init(SSD1306_I2C);
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
//	//Ини-я диспетчера.
//	Scheduler_Init();
//
//	//Постановка задач в очередь.
//	Scheduler_SetTask(Task_UartSendData);
//	Scheduler_SetTask(Task_Temperature_Read);
//	Scheduler_SetTask(Task_Lcd);
//	Scheduler_SetTask(Task_LcdUpdate);

	RTOS_Init();

	RTOS_SetTask(Task_Temperature_Read, 0, 1000);
	//RTOS_SetTask(Task_Lcd, 0, 5);
	RTOS_SetTask(Task_LcdUpdate, 0, 20);
	//***********************************************
	msDelay(500);
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
	//-----------------------------
	//Измерение ~U: F=50Гц, Uамп = 1В, смещенеи 1,6В.
	//AC_MeasLoop();
}
//*******************************************************************************************
//******************************************************************************************
