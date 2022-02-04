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
//---------------------------
DS18B20_t Sensor_1;
DS18B20_t Sensor_2;
DS18B20_t Sensor_3;

DS2782_t  DS2782;

Encoder_t Encoder;

static uint8_t  I2cTxBuf[32] = {0};
static uint8_t  I2cRxBuf[32] = {0};
static uint32_t I2cNacCount  = 0;
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

	DS2782_GetI2cAddress(&DS2782); //получение адреса на шине I2C
	DS2782_GetID(&DS2782);         //получение Unique ID (factory option)
	DS2782_GetTemperature(&DS2782);//получение температуры.
	DS2782_GetVoltage(&DS2782);    //получение напряжения на АКБ.
	DS2782_GetCurrent(&DS2782);    //получения тока потребления от АКБ.
}
//************************************************************
void Task_Lcd_DS2782(void){

	//Вывод времени.
	Time_Display(1, 2);

	//Вывод адреса на шине I2C
	Lcd_SetCursor(1, 3);
	Lcd_Print("DS2782_I2C_ADDR:");
	Lcd_Print("0x");
	Lcd_u8ToHex(DS2782.I2C_Address);

	//Вывод Unique ID (factory option)
	Lcd_SetCursor(1, 4);
	Lcd_Print("DS2782_ID:");
	Lcd_Print("0x");
	Lcd_u8ToHex(DS2782.ID);

	//Вывод температуры.
	Lcd_SetCursor(1, 5);
	Lcd_Print("Bat_T=");
	Lcd_BinToDec(DS2782.Temperature/10, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(DS2782.Temperature%10, 1, LCD_CHAR_SIZE_NORM);
	Lcd_Print(" C");

	//Вывод напряжения на АКБ.
	Lcd_SetCursor(1, 6);
	Lcd_Print("Bat_U=");
	Lcd_BinToDec(DS2782.Voltage/100, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('.');
	Lcd_BinToDec(DS2782.Voltage%100, 2, LCD_CHAR_SIZE_NORM);
	Lcd_Chr('V');

	//Вывод тока потребления от АКБ.
	int16_t temp = DS2782.Current;

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

	//Енкодер.
//	static uint16_t tempReg = 0;
//	Encoder_IncDecParam(&Encoder, &tempReg, 5, 0, 100);
//	TIM3->CCR1 = tempReg; //Задаем коэф-т заполнения.
//
//	Lcd_SetCursor(1, 5);
//	Lcd_Print("Encoder=");
//	Lcd_BinToDec(tempReg, 4, LCD_CHAR_SIZE_NORM);
//
//	//Вывод темперетуры DS18B20.
//	Sensor_1.SENSOR_NUMBER    = 1;
//	Sensor_1.TEMPERATURE_SIGN = I2cRxBuf[0];
//	Sensor_1.TEMPERATURE  	  = (uint32_t)(I2cRxBuf[1] << 8);
//	Sensor_1.TEMPERATURE 	 |= (uint32_t)I2cRxBuf[2];
//
//	Sensor_2.SENSOR_NUMBER    = 2;
//	Sensor_2.TEMPERATURE_SIGN = I2cRxBuf[3];
//	Sensor_2.TEMPERATURE      = (uint32_t)(I2cRxBuf[4] << 8);
//	Sensor_2.TEMPERATURE 	 |= (uint32_t)I2cRxBuf[5];
//
//	Temperature_Display(&Sensor_1, 1, 3);
//	Temperature_Display(&Sensor_2, 1, 4);
//	//Temperature_Display(&Sensor_3, 1, 5);

	Lcd_SetCursor(9, 1);
	Lcd_Print("I2CErr=");
	Lcd_BinToDec(I2cNacCount, 4, LCD_CHAR_SIZE_NORM);
}
//************************************************************
void Task_LcdUpdate(void){

//	if(Led_Blink(RTOS_GetTickCount(), 1000, 50)) LedPC13On();
//	else										 LedPC13Off();

	RTOS_SetTask(Task_Temperature_Display, 0, 0);
	//RTOS_SetTask(Task_Lcd_DS2782, 0, 0);

	LedPC13On();

	Lcd_Update(); //вывод сделан для SSD1306
	//Lcd_ClearVideoBuffer();

	LedPC13Off();
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

	//Вывод Unique ID (factory option)
	Txt_Print("DS2782_ID:");
	Txt_Print("0x");
	Txt_u8ToHex(DS2782.ID);
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
	Txt_Print("Bat_I=");
	if(DS2782.Current < 0)Txt_Chr('-');
	else                  Txt_Chr(' ');

	Txt_Chr('\n');
	Txt_Print("******************\n");
	//--------------------------------
	DMA1Ch4StartTx(Txt_Buf()->buf, Txt_Buf()->bufIndex);
	Txt_Buf()->bufIndex = 0;
}
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
//Запросы для отлаживания STM32 I2C в режиме Slave.
#define STM32_SLAVE_I2C		  I2C1
#define STM32_SLAVE_I2C_ADDR (0x05 << 1)

//************************************************************
void Task_STM32_Master_Write(void){

	I2cTxBuf[0]++;
	I2cTxBuf[1] = I2cTxBuf[0] + 1;
	I2cTxBuf[2] = I2cTxBuf[1] + 1;

	if(I2C_StartAndSendDeviceAddr(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR | I2C_MODE_WRITE) == 0)
	{
		I2C_SendData(STM32_SLAVE_I2C, I2cTxBuf, 3);
	}
}
//************************************************************
void Task_STM32_Master_Read(void){

	if(I2C_StartAndSendDeviceAddr(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR | I2C_MODE_READ) == 0)
	{
		I2C_ReadData(STM32_SLAVE_I2C, I2cRxBuf, 6);
	}
	else
	{
		I2cNacCount++;
		for(uint16_t i=0; i<6; i++)
		{
			*(I2cRxBuf+i) = 0;
		}
	}
}
//************************************************************
void Task_STM32_I2C_DMA_Send(void){

	//LedPC13Toggel();
	//--------------------------------
	static uint8_t sendBuf[32] = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE};

//	/* Enable the peripheral clock DMA1 */
//	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
//
//	DMA1_Channel6->CCR &= ~DMA_CCR_EN;	   // Channel disable
//
//	/* DMA1 Channel6 I2C1_TX config */
//	DMA1_Channel6->CPAR  = (uint32_t)&(I2C1->DR);// Peripheral address.
//	DMA1_Channel6->CMAR  = (uint32_t)sendBuf;    // Memory address.
//	DMA1_Channel6->CNDTR = 5; 		   		     // Data size.
//
//	DMA1_Channel6->CCR = (3 << DMA_CCR_PL_Pos)   | // PL[1:0]: Channel priority level - 11: Very high.
//						 (0 << DMA_CCR_PSIZE_Pos)| // PSIZE[1:0]: Peripheral size - 00: 8-bits.
//						 (0 << DMA_CCR_MSIZE_Pos)| // MSIZE[1:0]: Memory size     - 00: 8-bits.
//						 DMA_CCR_MINC |			   // MINC: Memory increment mode - Memory increment mode enabled.
//						 DMA_CCR_DIR  |            // DIR:  Data transfer direction: 1 - Read from memory.
//						 //DMA_CCR_CIRC | 		   // CIRC: Circular mode
//						 //DMA_CCR_TEIE | 		   // TEIE: Transfer error interrupt enable
//						 //DMA_CCR_HTIE | 		   // HTIE: Half transfer interrupt enable
//						 DMA_CCR_TCIE;// | 		   // TCIE: Transfer complete interrupt enable
//						 //DMA_CCR_EN;			   // EN: Channel enable
//
//	NVIC_SetPriority(DMA1_Channel6_IRQn, 0);// Set priority
//	NVIC_EnableIRQ(DMA1_Channel6_IRQn);     // Enable DMA1_Channel6_IRQn
//	//DMA1_Channel6->CCR |= DMA_CCR_EN;       // Channel enable

//	if(I2C_StartAndSendDeviceAddr(STM32_SLAVE_I2C, SSD1306_I2C_ADDR | I2C_MODE_WRITE)) return;
//	I2C_SendByte(STM32_SLAVE_I2C, 0x01);
//
//	//I2C1->CR2 |= I2C_CR2_LAST;  	  //DMA Last Transfer
//	I2C1->CR2 |= I2C_CR2_DMAEN; 	  //DMAEN(DMA requests enable)
//	DMA1_Channel6->CCR |= DMA_CCR_EN; // Channel enable

	LedPC13On();

	I2C_DMA_Write(STM32_SLAVE_I2C, SSD1306_I2C_ADDR, 0, sendBuf, 5);

	LedPC13Off();
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
//	Sensor_1.GPIO_PORT     = GPIOA;
//	Sensor_1.GPIO_PIN      = 3;
//	Sensor_1.SENSOR_NUMBER = 1;
//	Sensor_1.RESOLUTION    = DS18B20_Resolution_12_bit;
//	TemperatureSens_GpioInit(&Sensor_1);
//	TemperatureSens_SetResolution(&Sensor_1);
//	TemperatureSens_StartConvertTemperature(&Sensor_1);

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
	//Инициализация Энкодера.
	Encoder.GPIO_PORT_A 	 = GPIOA;
	Encoder.GPIO_PIN_A   	 = 0;
	Encoder.GPIO_PORT_B 	 = GPIOA;
	Encoder.GPIO_PIN_B  	 = 1;
	Encoder.GPIO_PORT_BUTTON = GPIOA;
	Encoder.GPIO_PIN_BUTTON  = 7;
	Encoder_Init(&Encoder);
	//***********************************************
	//Инициализация	ШИМ
//	TIM3_InitForPWM();
	//***********************************************
	//Ини-я OLED SSD1306
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//Ини-я DS2782.
	//DS2782_Init(DS2782_I2C);

	//I2C_Init(STM32_SLAVE_I2C, 0);
	//***********************************************
	//Отладка I2C по прерываниям.
//	static uint8_t i2cBuf[3] = {1, 2, 3};
//	I2C_IT_Init(I2C2, 0);
//	I2C_IT_StartTx(I2C1, SSD1306_I2C_ADDR, 0x55, i2cBuf, 3);

	//Отладка I2C+DMA.
	I2C_DMA_Init(I2C1, I2C1_TX_DMAChannel, 0);

	//***********************************************
	//Ини-я диспетчера.
	RTOS_Init();
	RTOS_SetTask(Task_LcdUpdate, 		  0, 50);
	//RTOS_SetTask(Task_STM32_Master_Read,  0, 500);
	//RTOS_SetTask(Task_STM32_Master_Write, 0, 500);
	//RTOS_SetTask(Task_STM32_I2C_DMA_Send, 0, 500);

	//RTOS_SetTask(Task_Temperature_Read, 0, 1000);
	//RTOS_SetTask(Task_GPS, 			0, 500);
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
	Encoder_ScanLoop(&Encoder);
}
//*******************************************************************************************
//******************************************************************************************
