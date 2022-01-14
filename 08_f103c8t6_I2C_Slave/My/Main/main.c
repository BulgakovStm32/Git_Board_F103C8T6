/*
 * 	main.c
 *
 *  Created on:
 *  Autho     : Беляев А.А.
 *
 *	Описание  : Работа I2C в режиме Slave

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
	Lcd_Print("Sens");
	Lcd_BinToDec(sensor->SENSOR_NUMBER, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print("= ");
	if(TemperatureSens_Sign(sensor) & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
	else                    								Lcd_Chr('+');
	Lcd_BinToDec(temperature/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(temperature%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print("o ");
	Lcd_Chr('C');
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
void Task_Temperature_Read(void){

	TemperatureSens_ReadTemperature(&Sensor_1);
	TemperatureSens_ReadTemperature(&Sensor_2);
	TemperatureSens_ReadTemperature(&Sensor_3);
}
//************************************************************
void Task_Temperature_Display(void){

	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_MCUv7_");

	//Вывод времени.
	Time_Display(1, 2);

	//Вывод темперетуры DS18B20.
	Temperature_Display(&Sensor_1, 1, 3);
	//Temperature_Display(&Sensor_2, 1, 4);
	//Temperature_Display(&Sensor_3, 1, 5);
}
//************************************************************
void Task_LcdUpdate(void){

	if(Led_Blink(RTOS_GetTickCount(), 1000, 50)) LedPC13On();
	else										 LedPC13Off();

	RTOS_SetTask(Task_Temperature_Display, 0, 0);

	Lcd_Update(); //вывод сделан для SSD1306
	Lcd_ClearVideoBuffer();
}
//*******************************************************************************************
//*******************************************************************************************
//Запросы для отлаживания STM32 I2C в режиме Slave.
#define STM32_SLAVE_I2C		  I2C1
#define STM32_SLAVE_I2C_ADDR (0x05)

static uint8_t txBuf[32] = {0};
static uint8_t rxBuf[32] = {0};
//************************************************************
void Task_STM32_Slave_Write(void){

	txBuf[0]++;
	txBuf[1] = txBuf[0] + 1;
	txBuf[2] = txBuf[1] + 1;
	txBuf[3] = txBuf[2] + 1;
}
//************************************************************
void Task_STM32_Slave_Read(void){

	LedPC13Toggel();
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
	//Uart1Init(USART1_BRR);
	//Adc_Init();

	microDelay(100000);//Эта задержка нужна для стабилизации напряжения патания.
					   //Без задержки LCD-дисплей не работает.
	//***********************************************
	//Ини-я DS18B20
	Sensor_1.GPIO_PORT     = GPIOA;
	Sensor_1.GPIO_PIN      = 3;
	Sensor_1.SENSOR_NUMBER = 1;
	Sensor_1.RESOLUTION    = DS18B20_Resolution_12_bit;
	TemperatureSens_GpioInit(&Sensor_1);
	TemperatureSens_SetResolution(&Sensor_1);
	TemperatureSens_StartConvertTemperature(&Sensor_1);

//	Sensor_2.GPIO_PORT     = GPIOA;
//	Sensor_2.GPIO_PIN      = 1;
//	Sensor_2.SENSOR_NUMBER = 2;
//	Sensor_2.RESOLUTION    = DS18B20_Resolution_12_bit;
//	TemperatureSens_GpioInit(&Sensor_2);
//	TemperatureSens_SetResolution(&Sensor_2);
//	TemperatureSens_StartConvertTemperature(&Sensor_2);
//
//	Sensor_3.GPIO_PORT     = GPIOA;
//	Sensor_3.GPIO_PIN      = 0;
//	Sensor_3.SENSOR_NUMBER = 3;
//	Sensor_3.RESOLUTION    = DS18B20_Resolution_12_bit;
//	TemperatureSens_GpioInit(&Sensor_3);
//	TemperatureSens_SetResolution(&Sensor_3);
//	TemperatureSens_StartConvertTemperature(&Sensor_3);
	//***********************************************
	//Инициализация	ШИМ
	//TIM3_InitForPWM();
	//***********************************************
	//Отладка I2C по прерываниям.
//	static uint8_t i2cBuf[3] = {1, 2, 3};
//	I2C_IT_Init(I2C1, 0);
//	I2C_IT_StartTx(I2C1, SSD1306_I2C_ADDR, 0x55, i2cBuf, 3);

	//Отладка I2C Slave
	//I2C_Slave_Init(I2C1, 0x05, 0);
	I2C_IT_Slave_Init(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR, 0);
	//I2C_IT_Slave_StartRx(rxBuf, 3);
	I2C_IT_Slave_StartTx(txBuf, 32);
	//***********************************************
	//Ини-я диспетчера.
	RTOS_Init();
	//RTOS_SetTask(Task_Temperature_Read, 0, 1000);
	RTOS_SetTask(Task_STM32_Slave_Write,0, 500);
	//RTOS_SetTask(Task_STM32_Slave_Read, 0, 500);

	//RTOS_SetTask(Task_LcdUpdate, 		0, 5);
	//RTOS_SetTask(Task_UartSend, 		0, 1000);
	//RTOS_SetTask(Task_DS2782, 		0, 250);
	//RTOS_SetTask(Task_AdcMeas, 		0, 250);
	//***********************************************
	__enable_irq();
	//**************************************************************
	while(1)
	{
		RTOS_DispatchLoop();
		//__WFI();//Sleep
	}
	//**************************************************************
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
//******************************************************************************************
