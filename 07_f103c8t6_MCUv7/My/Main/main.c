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
Time_t	  Time;

DS18B20_t Sensor_1;
DS18B20_t Sensor_2;
DS18B20_t Sensor_3;

DS2782_t  DS2782;

Encoder_t Encoder;

I2C_DMA_t I2cDma;

//static uint8_t  I2CTxBuf[32] = {0};
//static uint8_t  I2CRxBuf[32] = {0};

static uint32_t ButtonPressCount = 0;
static int16_t	PIDcontrol = 0;

static uint32_t MCUv7_EncoderVal = 0;
static uint32_t MCUv7_msVal		 = 0;
//*******************************************************************************************
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
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//Запросы для отлаживания STM32 I2C в режиме Slave.
#define STM32_SLAVE_I2C		  			I2C1
#define STM32_SLAVE_I2C_ADDR 			(0x05 << 1)
#define STM32_SLAVE_I2C_NUM_BYTE_READ	9

//static uint8_t  I2CTxBuf[32] = {0};
//static uint8_t  I2CRxBuf[32] = {0};
//************************************************************
void Task_STM32_Master_Write(void){

//	I2CTxBuf[0]++;
//	I2CTxBuf[1] = I2CTxBuf[0] + 1;
//	I2CTxBuf[2] = I2CTxBuf[1] + 1;

//	if(I2C_StartAndSendDeviceAddr(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR | I2C_MODE_WRITE) == I2C_OK)
//	{
//		I2C_SendData(STM32_SLAVE_I2C, I2CTxBuf, 3);
//	}

	//Запись данных
//	if(I2C_DMA_Write(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR, 0xCA, I2CTxBuf, 3) != I2C_DMA_BUSY)
//	{
//		for(uint16_t i = 0; i < 3; i++) *(I2CTxBuf+i) = 0;//Очистка буфера.
//		I2C_DMA_Init(STM32_SLAVE_I2C, I2C_GPIO_NOREMAP);
//	}
}
//************************************************************
void Task_STM32_Master_Read(void){

	//Складываем приняты данные.
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

	//Чтение данных
//	if(I2C_DMA_Read(STM32_SLAVE_I2C, STM32_SLAVE_I2C_ADDR, I2CRxBuf, STM32_SLAVE_I2C_NUM_BYTE_READ) == I2C_DMA_NAC)
//	{
//		for(uint16_t i = 0; i < STM32_SLAVE_I2C_NUM_BYTE_READ; i++) *(I2CRxBuf+i) = 0;//Очистка буфера.
//		I2C_DMA_Init(STM32_SLAVE_I2C, I2C_GPIO_NOREMAP);
//	}
}
//*******************************************************************************************
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
	Lcd_Print("Dbg_MCUv7");
	//Вывод ошибок обvена по I2C.
	Lcd_SetCursor(11, 1);
	Lcd_Print("I2CNAC=");
	Lcd_BinToDec(I2C_GetNacCount(STM32_SLAVE_I2C), 4, LCD_CHAR_SIZE_NORM);
	//Вывод времени.
	Time_Display(1, 2);

	//Значение энкодера MCUv7.
//	Lcd_SetCursor(1, 3);
//	Lcd_Print("MCUEn= ");
//	Lcd_BinToDec(1234567890, 10, LCD_CHAR_SIZE_NORM);


	//Енкодер.
//	static uint16_t tempReg = 0;
//	Encoder_IncDecParam(&Encoder, &tempReg, 5, 0, 100);
//	TIM3->CCR1 = tempReg; //Задаем коэф-т заполнения.

//	Lcd_SetCursor(1, 6);
//	Lcd_Print("Encoder=");
//	Lcd_BinToDec(tempReg, 4, LCD_CHAR_SIZE_NORM);

	//PID
//	Lcd_SetCursor(1, 8);
//	Lcd_Print("PID_Out=");
//	if(PIDcontrol < 0)
//	{
//		PIDcontrol = (PIDcontrol ^ 0xFFFF) + 1;//Уберем знак.
//		Lcd_Chr('-');
//	}
//	else Lcd_Chr(' ');
//	Lcd_BinToDec((uint16_t)PIDcontrol, 4, LCD_CHAR_SIZE_NORM);

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

	Temperature_Display(&Sensor_1, 1, 4);
	Temperature_Display(&Sensor_2, 1, 5);
	Temperature_Display(&Sensor_3, 1, 6);

	//Кнопка энкодера.
	IncrementOnEachPass(&ButtonPressCount, Encoder.BUTTON_STATE);
	Lcd_SetCursor(1, 8);
	Lcd_Print("Button=");
	Lcd_BinToDec((uint16_t)ButtonPressCount, 4, LCD_CHAR_SIZE_NORM);
}
//************************************************************
void Task_UartSend(void){

	//--------------------------------
	Txt_Chr('\f');

	Txt_Print("******************\n");

	Txt_Print("_MCUv7_(+BT)\n");

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
//*******************************************************************************************
/*!brief P, I and D parameter values
 * The K_P, K_I and K_D values (P, I and D gains)
 * need to be modified to adapt to the application at hand
 */
#define K_P 0.60
#define K_I 0.40
#define K_D	0.00
PID_t 		PID;
//************************************************************
void Task_PID(void){

	static int16_t outVal = 0;
	//-----------------------------

	PIDcontrol = PID_Controller(81, outVal, &PID);
	outVal     = PIDcontrol;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
#define MCUv7_I2C		I2C1
#define MCUv7_I2C_ADDR	(0x05<<1)

uint8_t  mcuI2cRxBuf[32];
uint8_t  mcuI2cTxBuf[32];
uint32_t rxSize;
//************************************************************
void Task_ReadResponseFromMCU(void){

	//Чтение ответа на команду от MCUv7 с помощью DMA.
	I2cDma.slaveAddr = MCUv7_I2C_ADDR;
	I2cDma.rxBufSize = rxSize;
	if(I2C_DMA_Read(&I2cDma) == I2C_DMA_NAC)//Если ошибка при чтении ответа
	{
		for(uint32_t i = 0; i < rxSize; i++) *(I2cDma.RxBuf+i) = 0;//Очистка буфера.
		I2C_DMA_Init(&I2cDma);
	}
}
//************************************************************
void Task_RequestFromMCUv7(void){

	static uint32_t cyclCount = cmdArduinoMicroTS;
		   uint32_t txSize;
//		   uint32_t rxSize;
	//-----------------------------
	//Индикация передачи
	//LedPC13Toggel();

	switch(cyclCount){
		//------------------
//		case(0):
//			mcuI2cTxBuf[0] = 0x00;
//			mcuI2cTxBuf[1] = 0x01;
//			mcuI2cTxBuf[2] = 0xCE;
//			txSize = 3;
//			rxSize = 16;
//
//			cyclCount++;
//		break;
		//------------------
		case(cmdArduinoMicroTS):
			mcuI2cTxBuf[0] = cmdArduinoMicroTS;
			mcuI2cTxBuf[1] = 0x01;
			mcuI2cTxBuf[2] = 0xCE;
			txSize = 3;		//сколько байт передаем
			rxSize = 6;		//сколько байт вычитываем(Count+Cmd+Data(uint32))

			cyclCount = cmdGetEncoderPosition;
		break;
		//------------------
		case(cmdGetEncoderPosition):
			mcuI2cTxBuf[0] = cmdGetEncoderPosition;
			mcuI2cTxBuf[1] = 0x01;
			mcuI2cTxBuf[2] = 0xCE;
			txSize = 3;
			rxSize = 15;

			cyclCount= cmdGetTemperature;
		break;
		//------------------
		case(cmdGetTemperature):
			mcuI2cTxBuf[0] = cmdGetTemperature; //
			mcuI2cTxBuf[1] = 0x02; 				//кол-во байтов в запросе
			mcuI2cTxBuf[2] = 0x01; 				//sensor_number
			txSize = 3;
			rxSize = 6;

			cyclCount++;
		break;
		//------------------
		case(cmdGetTemperature+1):
			mcuI2cTxBuf[0] = cmdGetTemperature; //
			mcuI2cTxBuf[1] = 0x02; 				//кол-во байтов в запросе
			mcuI2cTxBuf[2] = 0x02; 				//sensor_number
			txSize = 3;
			rxSize = 6;

			cyclCount = cmdArduinoMicroTS;
		break;
		//------------------
		default:
			cyclCount = 0;
		break;
		//------------------
	}
	//Перадача команды в MCUv7
	if(I2C_StartAndSendDeviceAddr(MCUv7_I2C, MCUv7_I2C_ADDR|I2C_MODE_WRITE) != I2C_OK) return;
	I2C_SendData(MCUv7_I2C, mcuI2cTxBuf, txSize);

	RTOS_SetTask(Task_ReadResponseFromMCU, 1, 0);
//	//Чтение ответа на команду от MCUv7 с помощью DMA.
//	I2cDma.slaveAddr = MCUv7_I2C_ADDR;
//	I2cDma.rxBufSize = rxSize;
//	if(I2C_DMA_Read(&I2cDma) == I2C_DMA_NAC)//Если ошибка при чтении ответа
//	{
//		for(uint32_t i = 0; i < rxSize; i++) *(I2cDma.RxBuf+i) = 0;//Очистка буфера.
//		I2C_DMA_Init(&I2cDma);
//	}
}
//************************************************************
void I2cRxParsing(void){

	MCU_Response_t *resp = (MCU_Response_t *)I2cDma.RxBuf;
	//-----------------------------
	LedPC13Toggel();//индикация приема ответа.

	switch(resp->CmdCode){
		//------------------
		case(cmdArduinoMicroTS):
			MCUv7_msVal	= *(uint32_t*)&resp->Payload[0];
		break;
		//------------------
		case(cmdGetEncoderPosition):
			MCUv7_EncoderVal = *(uint32_t *)&resp->Payload[2];
		break;
		//------------------
		case(cmdGetTemperature):
			//Складываем принятые данные.
			if(resp->Payload[3] == 1)// Датчик 1
			{
				Sensor_1.SENSOR_NUMBER    = 1;
				Sensor_1.TEMPERATURE_SIGN = resp->Payload[0];
				Sensor_1.TEMPERATURE  	  = (uint32_t)((resp->Payload[1] << 8) | resp->Payload[2]);
			}
			else if(resp->Payload[3] == 2)// Датчик 2
			{
				Sensor_2.SENSOR_NUMBER    = 2;
				Sensor_2.TEMPERATURE_SIGN = resp->Payload[0];
				Sensor_2.TEMPERATURE  	  = (uint32_t)((resp->Payload[1] << 8) | resp->Payload[2]);
			}
		break;
		//------------------
		default:

		break;
		//------------------
	}
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
void Task_LcdUpdate(void){

	TIME_Calculation(&Time, MCUv7_msVal);//RTOS_GetTickCount());
	//Выбор сраницы отображения Енкодером.
	static uint32_t encoder = 0;
	Encoder_IncDecParam(&Encoder, &encoder, 1, 0, 2);
	switch(encoder){
		//--------------------
		case 0:
			//RTOS_SetTask(Task_STM32_Master_Read,   5,  0);
			//RTOS_SetTask(Task_STM32_Master_Write,  10, 0);
			RTOS_SetTask(Task_RequestFromMCUv7,     5, 0);
			RTOS_SetTask(Task_Temperature_Display, 10, 0);
		break;
		//--------------------
		case 1:
			RTOS_SetTask(Task_DS2782,	  	  5,  0);
			RTOS_SetTask(Task_DS2782_Display, 10, 0);
		break;
		//--------------------
		default:
			Lcd_ClearVideoBuffer();
			Lcd_SetCursor(1, 1);
			Lcd_Print(" EMPTY PAGE ");
		break;
		//--------------------
	}
	//Обновление изображения на экране.
	//Очистка видеобуфера производится на каждой странице.
	Lcd_Update(); //вывод сделан для SSD1306
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	//***********************************************
	//Drivers.
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	MICRO_DELAY_Init();
	USART_Init(USART1, USART1_BRR);

	MICRO_DELAY(100000);//Эта задержка нужна для стабилизации напряжения патания.
					    //Без задержки LCD-дисплей не работает.
	//***********************************************
	//Инициализация PID.
	PID_Init(K_P * SCALING_INT16_FACTOR,
			 K_I * SCALING_INT16_FACTOR,
			 K_D * SCALING_INT16_FACTOR,
			 &PID);
	//***********************************************
	//Ини-я DS2782.
//	DS2782_Init(DS2782_I2C, I2C_GPIO_NOREMAP);
	//***********************************************
	//Ини-я OLED SSD1306
	SSD1306_Init(SSD1306_I2C, SSD1306_128x64, I2C_GPIO_NOREMAP);
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
	//Отладка I2C по прерываниям.
//	static uint8_t i2cBuf[3] = {1, 2, 3};
//	I2C_IT_Init(I2C2, 0);
//	I2C_IT_StartTx(I2C1, SSD1306_I2C_ADDR, 0x55, i2cBuf, 3);

	//Отладка I2C+DMA.
	//I2C_DMA_Init(I2C1, I2C_GPIO_NOREMAP);

	//Вопрос - Нужно ли делать работу с ДМА через контекст(т.е. через структуру)??
	//Не везде этот способу удобен. НУжно подумать.
	I2cDma.i2c 			 = I2C1;
	I2cDma.i2cGpioRemap  = I2C_GPIO_NOREMAP;
	I2cDma.i2cRxCallback = I2cRxParsing;
	I2C_DMA_Init(&I2cDma);
	//***********************************************
	//Ини-я диспетчера.
	RTOS_Init();
	RTOS_SetTask(Task_LcdUpdate, 0, 25);
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

	RTOS_TimerServiceLoop();
	msDelay_Loop();
	Blink_Loop();
	Encoder_ScanLoop(&Encoder);
}
//*******************************************************************************************
//******************************************************************************************
