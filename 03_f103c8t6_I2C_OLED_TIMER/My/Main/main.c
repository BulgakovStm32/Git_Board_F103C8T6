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
DS18B20_t	Sensor_1;
DS18B20_t	Sensor_2;
DS1307_t 	TimeAndCalendar;
Encoder_t	Encoder;

//*******************************************************************************************
//*******************************************************************************************
//void IncrementOnEachPass(uint32_t *var, uint16_t event){
//
//		   uint16_t riseReg  = 0;
//	static uint16_t oldState = 0;
//	//-------------------
//	riseReg  = (oldState ^ event) & event;
//	oldState = event;
//	if(riseReg) (*var)++;
//}
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
	Lcd_Print(" C(PinA");
	Lcd_BinToDec(sensor->GPIO_PIN, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Chr(')');
}
//*******************************************************************************************
//*******************************************************************************************
#define RADIUS 30.0
#define X_0	   64
#define Y_0	   0
#define RADIAN	((2*M_PI)/120.0)

//************************************************************
void Task_Temperature_Read(void){

	TemperatureSens_ReadTemperature(&Sensor_1);
	TemperatureSens_ReadTemperature(&Sensor_2);
	//-----------------------------
	Scheduler_SetTimerTask(Task_Temperature_Read, 1000);
}
//************************************************************
void Task_Lcd(void){

	static uint16_t counter = 0;
	//-----------------------------

	Led_Blink1(Scheduler_GetTickCount());//Мигание светодиодами.
	//-----------------------------
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_TIMER_");

	//Вывод времени RTC.
	DS1307_GetTimeAndCalendar(&TimeAndCalendar);

	Lcd_SetCursor(1, 2);
	Lcd_Print("Time:     ");
	Lcd_BinToDec(TimeAndCalendar.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(TimeAndCalendar.min, 2, LCD_CHAR_SIZE_NORM); //минуты
	Lcd_Chr(':');
	Lcd_BinToDec(TimeAndCalendar.sec, 2, LCD_CHAR_SIZE_NORM); //секунды

	Lcd_SetCursor(1, 3);
	Lcd_Print("Calendar: ");
	Lcd_BinToDec(TimeAndCalendar.date, 2, LCD_CHAR_SIZE_NORM);//день месяца
	Lcd_Chr('/');
	Lcd_BinToDec(TimeAndCalendar.month, 2, LCD_CHAR_SIZE_NORM);//месяц
	Lcd_Chr('/');
	Lcd_BinToDec(TimeAndCalendar.year, 4, LCD_CHAR_SIZE_NORM);//год

	//Работа с энкодером.
	Encoder_IncDecParam(&Encoder, &counter, 0, 100);

	Lcd_SetCursor(1, 4);
	Lcd_Print("Encoder: ");
	Lcd_BinToDec(counter, 4, LCD_CHAR_SIZE_NORM);

	//Вывод темперетуры DS18B20.
	Temperature_Display(&Sensor_1, 1, 5);
	Temperature_Display(&Sensor_2, 1, 6);

	//Рисование графики.
//	static uint32_t x1 = 0;
//	static uint32_t y1 = 0;
//	float rad_temp = Time.sec * RADIAN;
//
//	x1 = (uint32_t)(X_0 + RADIUS * (float)cos(rad_temp));
//	y1 = (uint32_t)(Y_0 + RADIUS * (float)sin(rad_temp));
//
//	Lcd_Line(X_0, Y_0, x1, y1, PIXEL_ON);
	//-----------------------------
	Scheduler_SetTask(Task_Lcd);
	//Scheduler_SetTimerTask(Task_Lcd, 1000);
}
//************************************************************
void Task_LcdUpdate(void){

	static uint32_t fps      = 0;
	static uint32_t fps_temp = 0;
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
	Lcd_Update();//вывод сделан для SSD1306
	Lcd_ClearVideoBuffer();
	//-----------------------------
	Scheduler_SetTask(Task_LcdUpdate);
	//Scheduler_SetTimerTask(Task_LcdUpdate, 1000);
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

	//TIM4_Init();
	//EXTI_Init();

	__enable_irq();
	msDelay(500);
	//***********************************************
	//Инициализация OLED SSD1306
	I2C1_Init();
	I2C1_DMAInit();
	SSD1306_Init(SSD1306_I2C);

	//uint8_t TxBuf[32] = {0,};
	//I2C1_Init();
	//I2C1_DMAInit();
	//I2C1_Write(SSD1306_I2C, SSD1306_I2C_ADDR, 0, TxBuf, 32);
	//***********************************************
	//Инициализация RTC DS1307.
	DS1307_Init(DS1307_I2C);

	//Настройки по умолчанию.
	if(!DS1307_GetStatusFlag(DS1307_STATUS_FLAG_CONFIG))
	{
		//DS1307_SetTimeZone(+8, 00);
		//DS1307_SetDayOfWeek(7);
		TimeAndCalendar.date  = 12;
		TimeAndCalendar.month = 12;
		TimeAndCalendar.year  = 2012;
		TimeAndCalendar.sec   = 12;
		TimeAndCalendar.min   = 12;
		TimeAndCalendar.hour  = 12;
		DS1307_SetTimeAndCalendar(&TimeAndCalendar);
	}
	//***********************************************
	//Инициализация Энкодера.
	Encoder.GPIO_PORT_A = GPIOA;
	Encoder.GPIO_PIN_A  = 0;

	Encoder.GPIO_PORT_B = GPIOA;
	Encoder.GPIO_PIN_B  = 1;

	Encoder.GPIO_PORT_BUTTON = GPIOA;
	Encoder.GPIO_PIN_BUTTON  = 7;

	Encoder.ENCODER_STATE = ENCODER_NO_TURN;

	Encoder_Init(&Encoder);
	//***********************************************
	//Инициализация DS18B20
	Sensor_1.GPIO_PORT     = GPIOA;
	Sensor_1.GPIO_PIN      = 3;
	Sensor_1.SENSOR_NUMBER = 1;
	Sensor_1.RESOLUTION    = DS18B20_Resolution_12_bit;
	TemperatureSens_GpioInit(&Sensor_1);
	TemperatureSens_SetResolution(&Sensor_1);
	TemperatureSens_StartConvertTemperature(&Sensor_1);

	Sensor_2.GPIO_PORT     = GPIOA;
	Sensor_2.GPIO_PIN      = 2;
	Sensor_2.SENSOR_NUMBER = 2;
	Sensor_2.RESOLUTION    = DS18B20_Resolution_12_bit;
	TemperatureSens_GpioInit(&Sensor_2);
	TemperatureSens_SetResolution(&Sensor_2);
	TemperatureSens_StartConvertTemperature(&Sensor_2);
	//***********************************************
	//Инициализация диспетчера.
	Scheduler_Init();

	//Постановка задач в очередь.
	Scheduler_SetTask(Task_Temperature_Read);
	Scheduler_SetTask(Task_Lcd);
	Scheduler_SetTask(Task_LcdUpdate);
	//***********************************************
	msDelay(500);
	//************************************************************************************
	while(1)
	{
		Scheduler_Loop();
		//__WFI();//Sleep
	}
	//************************************************************************************
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание каждую милисекунду.
void SysTick_Handler(void){

	Scheduler_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
	Encoder_ScanLoop(&Encoder);
	//-----------------------------
	//Измерение ~U: F=50Гц, Uамп = 1В, смещенеи 1,6В.
	//AC_MeasLoop();
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание TIM4.
void TIM4_IRQHandler(void){

	TIM4->SR  &= ~TIM_SR_UIF; //Сброс флага прерывания.
	//TIM4->CR1 &= ~TIM_CR1_CEN;//Counter dasable.

	//LedPC13Toggel();
	//Encoder_ScanLoop(&Encoder);
}
//*******************************************************************************************
//*******************************************************************************************
/**
  * @brief  This function handles EXTI0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void){

	if((EXTI->PR & 0x0001) != 0)  /* Check line 0 has triggered the IT */
	{
		EXTI->PR |= 0x0001; //Сброс бита в регистре EXTI_PR выполняется путем записи в соответствующий бит значения ‘1’.

		//LedPC13Toggel();
	}
//	else /* Should never occur */
//	{
//		//GPIOC->BSRR = (1<<8); /* Switch on orange led to report an error */
//	}
}
//*******************************************************************************************
//*******************************************************************************************




