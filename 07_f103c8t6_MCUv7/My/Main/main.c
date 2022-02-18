/*
 * 	main.c
 *
 *  Created on: 19 октября 2021 года.
 *  Autho     : Беляев А.А.
 *
 *	Описание        :
 *  Датчики         :
 *  Вывод информации:
 *
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

static Time_t   Time;
static uint32_t mScounter = 0;
//---------------------------
DS18B20_t Sensor_1;
DS18B20_t Sensor_2;
DS18B20_t Sensor_3;
DS2782_t  DS2782;
Encoder_t Encoder;

static uint8_t  I2CTxBuf[32] = {0};
static uint8_t  I2CRxBuf[32] = {0};

static uint32_t ButtonPressCount = 0;
//*******************************************************************************************
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

	count /= 1000;
	Time.hour = (uint8_t)((count / 3600) % 24);
	Time.min  = (uint8_t)((count / 60) % 60);
	Time.sec  = (uint8_t)(count % 60);
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
	Lcd_BinToDec(Time.min,  2, LCD_CHAR_SIZE_NORM);//минуты
	//Lcd_Chr(':');
	if(Time.sec & 1) Lcd_Chr(':');//Мигание разделительным знаком
	else			 Lcd_Chr(' ');
	Lcd_BinToDec(Time.sec,  2, LCD_CHAR_SIZE_NORM);//секунды
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Вывод значения встроенного АЦП.
//#define VREF      2480UL 				  	//Опроное напряжение в мв. Измеряется внешним вольтметром как можно точнее.
//#define VDD		  3254UL 				  	//Напряжение питания в мв. Измеряется внешним вольтметром как можно точнее.
//#define ADC_RES	  4096UL 				    //Количество квантов АЦП. 2^12 = 4096.
//#define ADC_QUANT ((VDD * 10000) / ADC_RES) //Вес кванта АЦП.
//#define K_RESIST_DIVIDE 2
//
//#define ADC_CH_VREF  9    //канал АЦП, к которому подключен внешний ИОН.
//#define ADC_CH_MEAS  8    //канал АЦП, которым измеряем напряжениа на АКБ.
//
//typedef struct{
//	uint32_t Bat_V;
//	uint32_t Vdd_V;
//	uint32_t Vref_V;
//}AdcMeas_t;
//
//AdcMeas_t	AdcMeas;
////----------------------------------------------
//void Task_AdcMeas(void){
//
//	AdcMeas.Bat_V  = ((Adc_GetMeas(ADC_CH_MEAS) * ADC_QUANT) / 10000) * K_RESIST_DIVIDE;//Измерение напряжения АКБ.
//	AdcMeas.Vref_V = Adc_GetMeas(ADC_CH_VREF);	                   //Измерение напряжения внешнего ИОН.
//	AdcMeas.Vdd_V  = (VREF * ADC_RES) / Adc_GetMeas(ADC_CH_VREF);  //Расчет напряжения питания через внешний ИОН.
//}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Работа с микросхемой DS2782.
void Task_DS2782(void){

	DS2782_GetI2cAddress(&DS2782); 		//получение адреса на шине I2C
	//DS2782_GetID(&DS2782);         		//получение Unique ID (factory option)
	DS2782_GetTemperature(&DS2782);		//получение температуры.
 	DS2782_GetVoltage(&DS2782);    		//получение напряжения на АКБ.
	DS2782_GetCurrent(&DS2782);    		//получения тока потребления от АКБ.
	DS2782_GetAverageCurrent(&DS2782);	//получения усредненного за 28 сек. тока потребления от АКБ.
	DS2782_GetAccumulatedCurrent(&DS2782);
}
//************************************************************
void Task_DS2782_Display(void){

	uint32_t temp = 0;

	Lcd_ClearVideoBuffer();

	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_DS2782_");
	//Вывод времени.
	Time_Display(1, 2);
	//Вывод ошибок обvена по I2C.
	Lcd_SetCursor(10, 1);
	Lcd_Print("I2CNAC=");
	Lcd_BinToDec(I2C_GetNacCount(DS2782_I2C), 4, LCD_CHAR_SIZE_NORM);

	//Вывод адреса на шине I2C
	Lcd_SetCursor(1, 3);
	Lcd_Print("DS2782_I2C_ADDR:");
	Lcd_Print("0x");
	Lcd_u8ToHex(DS2782.I2C_Address);

	//Вывод температуры.
	Lcd_SetCursor(1, 4);
	Lcd_Print("Bat_T   = ");
	temp = DS2782.Temperature;
	Lcd_BinToDec(temp/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(temp%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" C");

	//Вывод напряжения на АКБ.
	Lcd_SetCursor(1, 5);
	Lcd_Print("Bat_U   = ");
	temp = DS2782.Voltage;
	Lcd_BinToDec(temp/100, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(temp%100, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('V');
	//----------------------------------------------
	//Вывод тока потребления от АКБ.
	int16_t currentTemp = DS2782.Current;

	Lcd_SetCursor(1, 6);
	Lcd_Print("Bat_I   =");

	if(currentTemp < 0)
	{
		currentTemp = (currentTemp ^ 0xFFFF) + 1;//Уберем знак.
		Lcd_Chr('-');
	}
	else Lcd_Chr(' ');

	Lcd_BinToDec(currentTemp, 4, LCD_CHAR_SIZE_NORM);
	Lcd_Print("mA");

	//----------------------------------------------
	//Вывод усредненного за 28сек. тока.
	currentTemp = DS2782.AverageCurrent;

	Lcd_SetCursor(1, 7);
	Lcd_Print("Bat_Iavr=");

	if(currentTemp < 0)
	{
		currentTemp = (currentTemp ^ 0xFFFF) + 1;//Уберем знак.
		Lcd_Chr('-');
	}
	else Lcd_Chr(' ');

	Lcd_BinToDec(currentTemp, 4, LCD_CHAR_SIZE_NORM);
	Lcd_Print("mA");
	//----------------------------------------------

	Lcd_SetCursor(1, 8);
	Lcd_Print("Bat_Iacc=");
	Lcd_BinToDec(DS2782.AccumulatedCurrent, 5, LCD_CHAR_SIZE_NORM);
	Lcd_Print("uAh");

	//----------------------------------------------
	//Вывод значения встроенного АЦП.

	//Измерение напряжения АКБ.
//	Lcd_SetCursor(14, 1);
//	Lcd_Print("BAT:");
//	Lcd_BinToDec(AdcMeas.Bat_V, 4, LCD_CHAR_SIZE_NORM);
//if(I2C_DMA_State() == I2C_DMA_READY)
//	//Измерение напряжения внешнего ИОН.
//	Lcd_SetCursor(1, 8);
//	Lcd_Print("ADC=");
//	Lcd_BinToDec(Adc_GetMeas(ADC_CH_VREF), 5, LCD_CHAR_SIZE_NORM);
//
//	//Расчет напряжения питания через внешний ИОН.
//	Lcd_Print(" Vdd=");
//	Lcd_BinToDec(AdcMeas.Vdd_V, 5, LCD_CHAR_SIZE_NORM);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Работа с GPS L96-M33
#define GPS_I2C				I2C1
#define GPS_I2C_ADDR		(0x10 << 1)
#define GPS_I2C_RX_BUF_SIZE	256

static uint8_t GpsRxBuf[GPS_I2C_RX_BUF_SIZE] = {0,};

//************************************************************
void Task_GPS(void){

	I2C_Read(GPS_I2C, GPS_I2C_ADDR, 0xAE, GpsRxBuf, 1 );
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Запросы для отлаживания STM32 I2C в режиме Slave.
#define STM32_SLAVE_I2C		  			I2C1
#define STM32_SLAVE_I2C_ADDR 			(0x05 << 1)
#define STM32_SLAVE_I2C_NUM_BYTE_READ	9

//************************************************************
void Task_STM32_Master_Write(void){

	I2CTxBuf[0]++;
	I2CTxBuf[1] = I2CTxBuf[0] + 1;
	I2CTxBuf[2] = I2CTxBuf[1] + 1;

	if(I2C_StartAndSendDeviceAddr(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR | I2C_MODE_WRITE) == I2C_OK)
	{
		I2C_SendData(STM32_SLAVE_I2C, I2CTxBuf, 3);
	}
}
//************************************************************
void Task_STM32_Master_Read(void){

//	LedPC13On();

	//Складываем приняты данные.
	Sensor_1.SENSOR_NUMBER    = 1;
	Sensor_1.TEMPERATURE_SIGN = I2CRxBuf[0];
	Sensor_1.TEMPERATURE  	  = (uint32_t)((I2CRxBuf[1] << 8) | I2CRxBuf[2]);

	Sensor_2.SENSOR_NUMBER    = 2;
	Sensor_2.TEMPERATURE_SIGN = I2CRxBuf[3];
	Sensor_2.TEMPERATURE      = (uint32_t)((I2CRxBuf[4] << 8) | I2CRxBuf[5]);

	Sensor_3.SENSOR_NUMBER    = 3;
	Sensor_3.TEMPERATURE_SIGN = I2CRxBuf[6];
	Sensor_3.TEMPERATURE      = (uint32_t)((I2CRxBuf[7] << 8) | I2CRxBuf[8]);

	//Чтение данных
	if(I2C_DMA_Read(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR, I2CRxBuf, STM32_SLAVE_I2C_NUM_BYTE_READ) == I2C_DMA_NAC)
	{
		for(uint16_t i = 0; i < STM32_SLAVE_I2C_NUM_BYTE_READ; i++) *(I2CRxBuf+i) = 0;//Очистка буфера.
		I2C_DMA_Init(STM32_SLAVE_I2C, I2C_GPIO_NOREMAP);
	}

//	LedPC13Off();
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//void Task_Temperature_Read(void){
//
//	TemperatureSens_ReadTemperature(&Sensor_1);
//	TemperatureSens_ReadTemperature(&Sensor_2);
//	TemperatureSens_ReadTemperature(&Sensor_3);
//}
//************************************************************
void Task_Temperature_Display(void){

	Lcd_ClearVideoBuffer();

	//Шапка
	Lcd_SetCursor(1, 1);
	Lcd_Print("_MCUv7_");
	//Вывод времени.
	Time_Display(1, 2);
	//Вывод ошибок обvена по I2C.
	Lcd_SetCursor(10, 1);
	Lcd_Print("I2CErr=");
	Lcd_BinToDec(I2C_GetNacCount(STM32_SLAVE_I2C), 4, LCD_CHAR_SIZE_NORM);

	//Енкодер.
//	static uint16_t tempReg = 0;
//	Encoder_IncDecParam(&Encoder, &tempReg, 5, 0, 100);
//	TIM3->CCR1 = tempReg; //Задаем коэф-т заполнения.

//	Lcd_SetCursor(1, 6);
//	Lcd_Print("Encoder=");
//	Lcd_BinToDec(tempReg, 4, LCD_CHAR_SIZE_NORM);

	//Кнопка энкодера.
	IncrementOnEachPass(&ButtonPressCount, Encoder.BUTTON_STATE);
	Lcd_SetCursor(1, 7);
	Lcd_Print("Button=");
	Lcd_BinToDec((uint16_t)ButtonPressCount, 4, LCD_CHAR_SIZE_NORM);


//	//Вывод темперетуры DS18B20.
//	Sensor_1.SENSOR_NUMBER    = 1;
//	Sensor_1.TEMPERATURE_SIGN = I2CRxBuf[0];
//	Sensor_1.TEMPERATURE  	  = (uint32_t)((I2CRxBuf[1] << 8) | I2CRxBuf[2]);
//
//	Sensor_2.SENSOR_NUMBER    = 2;
//	Sensor_2.TEMPERATURE_SIGN = I2CRxBuf[3];
//	Sensor_2.TEMPERATURE      = (uint32_t)((I2CRxBuf[4] << 8) | I2CRxBuf[5]);
//
//	Sensor_3.SENSOR_NUMBER    = 3;
//	Sensor_3.TEMPERATURE_SIGN = I2CRxBuf[6];
//	Sensor_3.TEMPERATURE      = (uint32_t)((I2CRxBuf[7] << 8) | I2CRxBuf[8]);

	Temperature_Display(&Sensor_1, 1, 3);
	Temperature_Display(&Sensor_2, 1, 4);
	Temperature_Display(&Sensor_3, 1, 5);
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_LcdUpdate(void){

	Time_Calculation(mScounter);
	//Выбор сраниц отображения Енкодером.
	static uint16_t encoder = 0;
	Encoder_IncDecParam(&Encoder, &encoder, 1, 0, 2);
	switch(encoder){
		//--------------------
		case 0:
			//RTOS_SetTask(Task_STM32_Master_Read,   5,  0);
			RTOS_SetTask(Task_Temperature_Display, 5, 0);
		break;
		//--------------------
		case 1:
			//RTOS_SetTask(Task_DS2782,	  	  5,  0);
			RTOS_SetTask(Task_DS2782_Display, 5, 0);
		break;
		//--------------------
		default:
			Lcd_ClearVideoBuffer();
			Lcd_SetCursor(1, 1);
			Lcd_Print(" EMPTY PAGE ");
		break;
		//--------------------
	}

	RTOS_SetTask(Task_STM32_Master_Read, 10, 0);
	RTOS_SetTask(Task_DS2782,	  	     15, 0);
	//Обновление изображения на экране.
	//Очистка видеобуфера производится на каждой странице.
	Lcd_Update(); //вывод сделан для SSD1306
}
//************************************************************
void Task_UartSend(void){

	//--------------------------------
	Txt_Chr('\f');

	Txt_Print("******************\n");

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

	//--------------------------------
	//Вывод данных DS2782.
	//Вывод адреса на шине I2C
	Txt_Chr('\n');
	Txt_Print("DS2782_I2C_ADDR:");
	Txt_Print("0x");
	Txt_u8ToHex(DS2782.I2C_Address);
	Txt_Chr('\n');

	//Вывод температуры.
	Txt_Print("Bat_T=");
	Txt_BinToDec(DS2782.Temperature/10, 2);
	Txt_Chr('.');
	Txt_BinToDec(DS2782.Temperature%10, 1);
	Txt_Print(" C");
	Txt_Chr('\n');

	//Вывод напряжения на АКБ.
	Txt_Print("Bat_U=");
	Txt_BinToDec(DS2782.Voltage/100, 2);
	Txt_Chr('.');
	Txt_BinToDec(DS2782.Voltage%100, 2);
	Txt_Chr('V');
	Txt_Chr('\n');

	//Вывод тока потребления от АКБ.
	Txt_Print("Bat_I  =");
	if(DS2782.Current < 0)
	{
		DS2782.Current = (DS2782.Current ^ 0xFFFF) + 1;//Уберем знак.
		Txt_Chr('-');
	}
	else Txt_Chr(' ');
	Txt_BinToDec(DS2782.Current, 4);
	Txt_Print("mA");
	Txt_Chr('\n');
	Txt_Chr('\n');

	//Количество нажатий на кнопку.
	Txt_Print("ButtonPress=");
	Txt_BinToDec(ButtonPressCount, 4);

	Txt_Chr('\n');
	Txt_Print("******************\n");
	//--------------------------------
	DMA1Ch4StartTx(Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	microDelay_Init();
	USART_Init(USART1, USART1_BRR);
	//Adc_Init();

	microDelay(100000);//Эта задержка нужна для стабилизации напряжения патания.
					   //Без задержки LCD-дисплей не работает.
	//***********************************************
	//Инициализация Энкодера.
	Encoder.GPIO_PORT_A 	 = GPIOB;
	Encoder.GPIO_PIN_A   	 = 10;
	Encoder.GPIO_PORT_B 	 = GPIOB;
	Encoder.GPIO_PIN_B  	 = 11;
	Encoder.GPIO_PORT_BUTTON = GPIOB;
	Encoder.GPIO_PIN_BUTTON  = 1;
	Encoder_Init(&Encoder);
	//***********************************************
	//Инициализация	ШИМ
	//TIM3_InitForPWM();
	//***********************************************
	//Ини-я DS2782.
	DS2782_Init(DS2782_I2C, I2C_GPIO_NOREMAP);
	//***********************************************
	//Ини-я OLED SSD1306
	SSD1306_Init(SSD1306_I2C, SSD1306_128x64, I2C_GPIO_NOREMAP);
	//Lcd_ClearVideoBuffer();
	//***********************************************
	//Отладка I2C по прерываниям.
//	static uint8_t i2cBuf[3] = {1, 2, 3};
//	I2C_IT_Init(I2C2, 0);
//	I2C_IT_StartTx(I2C1, SSD1306_I2C_ADDR, 0x55, i2cBuf, 3);

	//Отладка I2C+DMA.
	I2C_DMA_Init(I2C1, I2C_GPIO_NOREMAP);
	//***********************************************
	//Ини-я диспетчера.
	RTOS_Init();
	RTOS_SetTask(Task_LcdUpdate, 		  0, 20);
	//RTOS_SetTask(Task_STM32_Master_Read,  0, 500);
	//RTOS_SetTask(Task_STM32_Master_Write, 0, 500);

	//RTOS_SetTask(Task_Temperature_Read, 0, 1000);
	//RTOS_SetTask(Task_GPS, 			0, 500);
	RTOS_SetTask(Task_UartSend, 		0, 1000);
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

	mScounter++;

	RTOS_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
	Encoder_ScanLoop(&Encoder);
}
//*******************************************************************************************
//******************************************************************************************
