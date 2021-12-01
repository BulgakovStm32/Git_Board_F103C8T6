/*
 * main.c
 *
 *  Created on: Oct 9, 2021
 *      Author: belyaev
 *
 *  Описание проекта: Измерение частоты и вывод на OLED.f103c8t6_I2C_OLED128x32_FreqCounter
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

uint32_t EncoderReg = 0;
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
		//LedPC13Toggel();
	}
}
//************************************************************
void Temperature_Read(void){

		   uint32_t millis    = Scheduler_GetTickCount();
	static uint32_t millisOld = 0;
	//-------------------
	if(millis - millisOld >= 1000)
	{
		millisOld = millis;

		TemperatureSens_ReadTemperature(&Sensor_1);
		TemperatureSens_ReadTemperature(&Sensor_2);
	}
}
//************************************************************
void Temperature_Display(DS18B20_t *sensor, uint8_t cursor_x, uint8_t cursor_y){

	uint32_t temperature = sensor->TEMPERATURE;
	//-------------------
	Lcd_SetCursor(cursor_x, cursor_y);
	Lcd_Print("Sens_");
	Lcd_BinToDec(sensor->SENSOR_NUMBER, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('=');

	if(TemperatureSens_Sign(sensor) & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
	else                    								Lcd_Chr('+');

	Lcd_BinToDec(temperature/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(temperature%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" C");
}
//*******************************************************************************************
//*******************************************************************************************
#define RADIUS 30.0
#define X_0	   64
#define Y_0	   0
#define RADIAN	((2*M_PI)/120.0)

//************************************************************
void Task_Lcd(void){

	static uint32_t secCounter = 0;
	//-----------------------------
	Led_Blink1(Scheduler_GetTickCount());                    //Мигание светодиодами.
	IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));//Инкримент счетчика секунд.
	Time_Calculation(secCounter);						     //Преобразование времени
	//-----------------------------
	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_FREQ_COUNTER_");

	//Вывод времени.
	Lcd_SetCursor(1, 2);
	Lcd_Print("Time: ");
	Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
	Lcd_Chr(':');
	Lcd_BinToDec(Time.min, 2, LCD_CHAR_SIZE_NORM); //минуты
	Lcd_Chr(':');
	Lcd_BinToDec(Time.sec, 2, LCD_CHAR_SIZE_NORM); //секунды

	//Вывод значения счетного регистра TIM3
	EncoderReg = (TIM3_GetCounter() >> 1);//На каждый щелчок энкодера таймер меняется на 2.
										  //по этому делим на 2.

	Lcd_SetCursor(1, 4);
	Lcd_Print("TIM3_CNT= ");
	Lcd_BinToDec(EncoderReg, 4, LCD_CHAR_SIZE_NORM);

	//Вывод регистра STATUS
//	Lcd_SetCursor(1, 3);
//	Lcd_Print("reg_STATUS = 0x");
//	Lcd_u8ToHex(NRF24L01.STATUS_Reg);



	//Рисование графики.
//	static uint32_t x1 = 0;
//	static uint32_t y1 = 0;
//	float rad_temp = Time.sec * RADIAN;
//	x1 = (uint32_t)(X_0 + RADIUS * (float)cos(rad_temp));
//	y1 = (uint32_t)(Y_0 + RADIUS * (float)sin(rad_temp));
//	Lcd_Line(X_0, Y_0, x1, y1, PIXEL_ON);
	//-----------------------------
	Scheduler_SetTask(Task_Lcd);
	//Scheduler_SetTimerTask(Task_Lcd, 50);
}
//************************************************************
void Task_LcdUpdate(void){

	static uint32_t fps      = 0;
	static uint32_t fps_temp = 0;
	//-----------------------------
	//Счетчик кадров в секундe - не отлажен!!!
	if(!Blink(INTERVAL_1000_mS)) fps_temp++;
	if( Blink(INTERVAL_1000_mS) && fps_temp != 0)
	{
		fps = fps_temp >> 3;//делим не 8 т.к. вывод на дисплей производится за 8 проходов.
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
	//Scheduler_SetTimerTask(Task_LcdUpdate, 10);
}
//************************************************************
void Task_Spi(void){

	SPI_TxRxByte(SPI1, 0xFF);
	LedPC13Toggel();
	//-----------------------------
	Scheduler_SetTimerTask(Task_Spi, 100);
}
//************************************************************
void Task_Uart(void){

//	Txt_Chr('\f');//Очистка окна терминала

	Txt_Print("Time: ");
	Txt_BinToDec(Time.hour, 2);//часы
	Txt_Chr(':');
	Txt_BinToDec(Time.min, 2); //минуты
	Txt_Chr(':');
	Txt_BinToDec(Time.sec, 2); //секунды
	Txt_Chr('\n');


	//Lcd_SetCursor(cursor_x, cursor_y);
	Txt_Print("Sens_");
	Txt_BinToDec(0, 1);
	Txt_Chr('=');

	if(TemperatureSens_Sign(0) & DS18B20_SIGN_NEGATIVE)Txt_Chr('-');
	else                    						   Txt_Chr('+');

	Txt_BinToDec(25, 2);
	Txt_Chr('.');
	Txt_BinToDec(1, 1);
	Txt_Print(" C");
	Txt_Chr('\n');

	Txt_Print("TIM3_CNT= ");
	Txt_BinToDec(EncoderReg, 4);
	Txt_Chr('\n');
	Txt_Chr('\n');

	DMA1Ch4StartTx(Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
	//-----------------------------
	Scheduler_SetTimerTask(Task_Uart, 50);
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	//SPI_Init(SPI1);
	Uart1Init(USART1_BRR);
	microDelay_Init();

	TIM4_Init();
	TIM4_SetFreq(5200 * 2);

	TIM3_InitForEncoder();
	TIM3_SetCounter(81);

	__enable_irq();
	msDelay(500);
	//***********************************************
	//Ини-я OLED SSD1306
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//Ини-я DS18B20

//	Sensor_1.GPIO_PORT     = GPIOA;
//	Sensor_1.GPIO_PIN      = 3;
//	Sensor_1.SENSOR_NUMBER = 1;
//	Sensor_1.RESOLUTION    = DS18B20_Resolution_12_bit;
//	TemperatureSens_GpioInit(&Sensor_1);
//	TemperatureSens_SetResolution(&Sensor_1);
//	TemperatureSens_StartConvertTemperature(&Sensor_1);
//
//	Sensor_2.GPIO_PORT     = GPIOB;
//	Sensor_2.GPIO_PIN      = 15;
//	Sensor_2.SENSOR_NUMBER = 2;
//	Sensor_2.RESOLUTION    = DS18B20_Resolution_12_bit;
//	TemperatureSens_GpioInit(&Sensor_2);
//	TemperatureSens_SetResolution(&Sensor_2);
//	TemperatureSens_StartConvertTemperature(&Sensor_2);
	//***********************************************
	//Инициализация ETR

	//Тактирование TIM2 и GPIOA, PA0 input-float
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	//базовые настройки таймера ни кто не отменял
	TIM2->PSC = 1 - 1;
	TIM2->CNT = 0;
	TIM2->ARR = 60000 - 1;
	TIM2->CR1 &= ~TIM_CR1_DIR;//считаем вверх

	TIM2->SMCR &= ~TIM_SMCR_ETF; //настраиваем фильтр
	TIM2->SMCR &= ~TIM_SMCR_ETPS;//предделитель 0
	TIM2->SMCR &= ~TIM_SMCR_ETP; //возрастающий
	TIM2->SMCR |= TIM_SMCR_ECE;  //внешнее тактирование

	//***********************************************
	//Ини-я диспетчера.
	Scheduler_Init();

	//Постановка задач в очередь.
	Scheduler_SetTask(Task_Lcd);
	Scheduler_SetTask(Task_LcdUpdate);
	Scheduler_SetTask(Task_Spi);
	Scheduler_SetTask(Task_Uart);
	//***********************************************
	//msDelay(500);
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

	//LedPC13Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание TIM4.
void TIM4_IRQHandler(void){

	TIM4->SR  &= ~TIM_SR_UIF; //Сброс флага прерывания.
	//TIM4->CR1 &= ~TIM_CR1_CEN;//Counter dasable.

	//LedPC13Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
