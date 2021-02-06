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

	if(Blink(INTERVAL_100_mS)) LedPC13On();
	else					   LedPC13Off();

//	if(Blink(INTERVAL_250_mS)) LedPA6_On();
//	else					   LedPA6_Off();

//	if(Blink(INTERVAL_500_mS)) LedPA7_On();
//	else					   LedPA7_Off();
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
//Работа с BMI160
#define BMI160_ADDR	(0x69 << 1)

//---------------------------
typedef struct{
	uint8_t	Chip_ID;
	uint8_t Err_Reg;
	uint8_t PMU_Status;
	uint8_t Data[20];
	uint8_t Sensor_Time[3];
	uint8_t Status;
	uint8_t INT_Status[4];
	uint8_t Temperature[2];
}BMI160_t;
static BMI160_t BMI160Str;
//---------------------------
uint16_t GyrX = 0;
uint16_t GyrY = 0;
uint16_t GyrZ = 0;

uint16_t AvrCount = 0;
uint32_t AccXavr  = 0;
uint32_t AccYavr  = 0;
uint32_t AccZavr  = 0;
//************************************************************
void BMI160_Init(void){

	uint8_t  BMI160Cmd = 0;
	//-------------------
	//Soft Reset
	//	BMI160Cmd = 0xb6;
	//	I2C_Write(I2C2, BMI160_ADDR, 0x7E, &BMI160Cmd, 1);
	//	msDelay(10);
	//Чтение ID
	I2C_Read(I2C2, BMI160_ADDR, 0x00, &BMI160Str.Chip_ID, 1);
	//ACC_RANGE.
	BMI160Cmd = 0b00000011; //2g
	I2C_Write(I2C2, BMI160_ADDR, 0x41, &BMI160Cmd, 1);
	//ACC_CONF.
	BMI160Cmd  = 0b00101000;//acc_us = 0, acc_bwp = 5
	//BMI160Cmd |= 0b00000001;//acc_odr = 25/32
	I2C_Write(I2C2, BMI160_ADDR, 0x40, &BMI160Cmd, 1);
	//Включение акселерометра(с ним же включается датчик температуры).
	BMI160Cmd = 0b00010001;
	I2C_Write(I2C2, BMI160_ADDR, 0x7E, &BMI160Cmd, 1);
}
//************************************************************
uint16_t BMI160_GetTemperature(void){

	uint16_t bmiT = 0;
	uint32_t temp = 0;
	//-------------------
	//Чтение температуры.
	I2C_Read(I2C2, BMI160_ADDR, 0x20, (uint8_t*)&bmiT, 2);
	temp = bmiT + 148000;//это смещение в 23 градуса Цельсия. 23 гр.Цельсия = 296 Кельвинов.
					     //148000 - это 296 Кельвинов / 0,002. 0,002 это LBS датчика температуры.
	temp *= 20;          //это LBS * 10000.
	temp -= 136575; 	 //1 гр.Цельсия = 273,15 Кельвинов. 136575 это 273,15 / LBS.
	temp /= 10000;
	return (uint16_t)temp;

//	I2C_Read(I2C2, BMI160_ADDR, 0x20, BMI160Str.Temperature, 2);
//	uint32_t Temperature = ((BMI160Str.Temperature[1] << 8) | BMI160Str.Temperature[0]) & 0x7FFF;
//	Temperature += 148000; //это смещение в 23 градуса Цельсия. 23 гр.Цельсия = 296 Кельвинов.
//						   //148000 - это 296 Кельвинов / 0,002. 0,002 это LBS датчика температуры.
//	Temperature *= 20;     //это LBS * 10000.
//	Temperature -= 136575; //1 гр.Цельсия = 273,15 Кельвинов. 136575 это 273,15 / LBS.
//	Temperature /= 10000;
}
//*******************************************************************************************
//*******************************************************************************************
//Работа с MPU6050.
#define MPU6050_ADDR	(0x68 << 1)

//---------------------------
typedef struct{

	uint8_t  Chip_ID;
	uint16_t Acc[3];

}MPU6050_t;
static MPU6050_t MPU6050Str;
//---------------------------

//************************************************************
void MPU6050_Init(void){

	uint8_t cmd = 0;
	//-------------------
	//Чтение ID чипа.
	I2C_Read(I2C2, MPU6050_ADDR, 0x75, &MPU6050Str.Chip_ID, 1);

	//Датчик тактируется от встроенного 8Мгц осциллятора
	//MPU6050_Write(0x6B, 0x00); // Register_PWR_M1 = 0, Disable sleep mode
	cmd = 0x00;
	I2C_Write(I2C2, MPU6050_ADDR, 0x6B, &cmd, 1);
	//Выполнить очистку встроенных регистров датчика
	//MPU6050_Write(0x6A, 0x01); // Register_UsCtrl = 1
	cmd = 0x01;
	I2C_Write(I2C2, MPU6050_ADDR, 0x6A, &cmd, 1);

	msDelay(500);
}
//************************************************************
uint8_t MPU6050_ReadID(void){

	uint8_t temp;
	//-------------------
	I2C_Read(I2C2, MPU6050_ADDR, 0x75, &temp, 1);
	return temp;
}
//************************************************************
void MPU6050_ReadAcc(uint8_t *pBuf){

//	*(pBuf+0) = 1;
//	*(pBuf+1) = 2;
//	*(pBuf+2) = 3;

	//Прочитаем X
	I2C_Read(I2C2, MPU6050_ADDR, 0x3B, (pBuf+1), 1);
	I2C_Read(I2C2, MPU6050_ADDR, 0x3C, (pBuf+0), 1);
	//Прочитаем Y
	I2C_Read(I2C2, MPU6050_ADDR, 0x3D, (uint8_t*)pBuf+3, 1);
	I2C_Read(I2C2, MPU6050_ADDR, 0x3E, (uint8_t*)pBuf+2, 1);
	//Прочитаем Z
	I2C_Read(I2C2, MPU6050_ADDR, 0x3F, (uint8_t*)pBuf+5, 1);
	I2C_Read(I2C2, MPU6050_ADDR, 0x40, (uint8_t*)pBuf+4, 1);
//	//Прочитаем Температуру
//	I2C_Read(I2C2, MPU6050_ADDR, 0x42, pBuf+0, 1);
//	I2C_Read(I2C2, MPU6050_ADDR, 0x41, pBuf+1, 1);
}
//*******************************************************************************************
//*******************************************************************************************
//Работа с платой RTC.

#define DS1307_ADDR	(0b1101000 << 1)

//---------------------------
#pragma pack(push, 1)//размер выравнивания в 1 байт
typedef struct{
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;
	uint8_t Day;
	uint8_t Date;
	uint8_t Month;
	uint8_t Year;
	uint8_t Control;
	uint8_t RAM[56];
}DS1307_t;
#pragma pack(pop)//вернули предыдущую настройку.
//---------------------------
static DS1307_t	DS1307Str;
//************************************************************

//*******************************************************************************************
//*******************************************************************************************
//Работа с ШИМ.

uint8_t  pwm_values[24 * 3 + 1];

//************************************************************
void set_led_color(uint8_t numLed, uint8_t red, uint8_t green, uint8_t blue){

	uint8_t i;
	//----------------
	for(i = 0; i < 8; i++)
		pwm_values[24 * numLed + i] = ((red   >> (7 - i)) & 1) ? 36 : 12;

	for(i = 0; i < 8; i++)
		pwm_values[24 * numLed + 8  + i] = ((green >> (7 - i)) & 1) ? 36 : 12;

	for(i = 0; i < 8; i++)
		pwm_values[24 * numLed + 16 + i] = ((blue  >> (7 - i)) & 1) ? 36 : 12;

	pwm_values[sizeof(pwm_values)/sizeof(pwm_values[0]) - 1] = 0;
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	uint16_t dsRes = 0;
//	uint16_t BmiT  = 0;
	//-----------------------------
	//Drivers.
	Sys_Init();
	Gpio_Init();

	//TIM3_InitForPWM();
	//TIM1_InitForCapture();
	//TIM1->CCR2 = 10;

	//Настройка DMA для работы с таймером.
	//DMA1_ChX_Init(DMA1_Channel4, pwm_value, (sizeof(pwm_value) / sizeof(pwm_value[0])) );

	//Настройка DMA для работы с I2C. Передача буфера в I2С через DMA.

	//***********************************************
	//Инициализация планировщик.
//	Scheduler_Init();
//	Scheduler_SetTask(Task_LedsBlink);
//	Scheduler_SetTask(Task_GetTemperature);
//	Scheduler_SetTask(Task_Display);
//	Scheduler_SetTask(Task_LcdUpdate);

	SysTick_Init();
	__enable_irq();


	I2C_Init(SSD1306_I2C);//I2C_Int_Init(SSD1306_I2C);
	//***********************************************

//	__disable_irq();
	msDelay(1000);
	//***********************************************
	//Энкодер.
//	EncoderInit();

	//***********************************************
	//LCD Lm6063. - Работает.
//	Lcd_Init();
//	Lcd_String(1, 1);
//	Lcd_Print("Hello World!!!");
//	Lcd_Update();
	//***********************************************
	//LCD 16x2 I2C - Работает.
//	I2C_Init(LCD_I2C);
//	lcd_init();
//	lcd_set_cursor(LCD_STRING_1, 1);
//	lcd_print_string("Hello IMUv1.0!!!");
	//***********************************************
	//LCD 128x32 TIC32 - Работает.
//	Lcd_TIC32_Init();    //
//	Lcd_TIC32_ClearRAM();//Очистка ОЗУ драйвера
	//***********************************************
	//OLED SSD1306
	SSD1306_Init(SSD1306_I2C);
	//***********************************************
	//DS18B20
	DS18B20_Init(DS18B20_Resolution_12_bit);
	DS18B20_StartConvertTemperature();
	//***********************************************
	//Акселерометр BMI160.
	//BMI160_Init();
	//Акселерометр MPU6050.
	//MPU6050_Init();
	//***********************************************
	//датчик температуры и влажности AHT10.
	AHT10_SoftReset();
	//***********************************************
	//Работа с платой RTC.
	//Запуск часов.
	uint8_t DS1307_Cmd = 0;
	I2C_Write(I2C2, DS1307_ADDR, 0x00, &DS1307_Cmd, 1);
	//***********************************************
	//Переход в режим Sleep. - не отлажено!!!
//	DBGMCU->CR |=  DBGMCU_CR_DBG_SLEEP;    //Отладка в режимах пониженного энергопотребления
//	SCB->SCR   &= ~SCB_SCR_SLEEPDEEP_Msk;  //для M3 запрешаем sleepdeep
//	SCB->SCR   |=  SCB_SCR_SLEEPONEXIT_Msk;//после выхода их прерывания переход в Sleep

//	float cosAlfa = cosf(32 / 32);
	//************************************************************************************
	while(1)
		{
			//msDelay(5);
			//***********************************************
			//Обучение ШИМ.

//			if(Blink(INTERVAL_500_mS))
//				{
//					set_led_color(0, 0x1f, 0x1f, 0x1f);
//					set_led_color(1, 0x1f, 0x1f, 0x1f);
//					set_led_color(2, 0x1f, 0x1f, 0x1f);
//					DMA1_ChX_Init(pwm_values, sizeof(pwm_values)/sizeof(pwm_values[0]));
//				}
//			else
//				{
//					set_led_color(0, 0x1f, 0   , 0   );
//					set_led_color(1, 0   , 0x1f, 0   );
//					set_led_color(2, 0   , 0   , 0x1f);
//					DMA1_ChX_Init(pwm_values, sizeof(pwm_values)/sizeof(pwm_values[0]));
//				}


//			set_led_color(0, 0x1f, 0x1f, 0x1f);
//			set_led_color(1, 0x1f, 0x1f, 0x1f);
//			set_led_color(2, 0x1f, 0x1f, 0x1f);
//			DMA1_ChX_Init(pwm_values, sizeof(pwm_values)/sizeof(pwm_values[0]));
//			msDelay(500);
//
//			set_led_color(0, 0x1f, 0   , 0   );
//			set_led_color(1, 0   , 0x1f, 0   );
//			set_led_color(2, 0   , 0   , 0x1f);
//			DMA1_ChX_Init(pwm_values, sizeof(pwm_values)/sizeof(pwm_values[0]));
//			msDelay(500);
//
//			set_led_color(1, 0x1f, 0   , 0   );
//			set_led_color(2, 0   , 0x1f, 0   );
//			set_led_color(0, 0   , 0   , 0x1f);
//			DMA1_ChX_Init(pwm_values, sizeof(pwm_values)/sizeof(pwm_values[0]));
//			msDelay(500);
//
//			set_led_color(2, 0x1f, 0   , 0   );
//			set_led_color(0, 0   , 0x1f, 0   );
//			set_led_color(1, 0   , 0   , 0x1f);
//			DMA1_ChX_Init(pwm_values, sizeof(pwm_values)/sizeof(pwm_values[0]));
//			msDelay(500);


			//TIM3->CCR1 = 10;

			//TIM3->CCR1++;
			//if(TIM3->CCR1 >= 100) TIM3->CCR1 = 0;
			//***********************************************
			//Таймер TIM1 в режиме захвата.
			//TIM1->CCR1 = 10;



			//***********************************************
			//Работа с платой RTC (микросхема DS1307).
			//Чтение регистров времени
			I2C_Read(I2C2, DS1307_ADDR, 0x00, (uint8_t*)&DS1307Str, 8);
			//Вывод времени.
			Lcd_String(1, 2);
			Lcd_Print("DS1307  : ");
			//часы
			Lcd_BinToDec(DS1307Str.Hours, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr(':');
			//минуты
			uint8_t min = ((DS1307Str.Minutes >> 4) * 10) + (DS1307Str.Minutes & 0x0F);
			Lcd_BinToDec(min , 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr(':');
			//секунды
			uint8_t sec = ((DS1307Str.Seconds >> 4) * 10) + (DS1307Str.Seconds & 0x0F);
			Lcd_BinToDec(sec , 2, LCD_CHAR_SIZE_NORM);
			//***********************************************
			//Планировщик.
//			Scheduler();
			//***********************************************
			//Работа с энкодером.
//			Encoder()->Rotation(&count, 0, 999);
//			if(Encoder()->ButtonIsPress)
//				{
//					count = 0;
//					lcd_clear();
//				}
			//***********************************************
			//Мигание светодиодами.
			Led_Blink();
			//DS18B20
			Temperature_Get(&dsRes);
			//***********************************************
			//LCD 128x64 - Работает.

			IncrementOnEachPass(&secCounter, Blink(INTERVAL_500_mS));
			Time_Calculation(secCounter);

			Lcd_String(1, 1);
			Lcd_Print("IMU v1.0 Test");
			//Вывод времени.
//			Lcd_String(1, 2);
//			Lcd_Print("Time:");
//			Lcd_BinToDec(Time.hour, 2, LCD_CHAR_SIZE_NORM);//часы
//			Lcd_Chr(':');
//			Lcd_BinToDec(Time.min, 2, LCD_CHAR_SIZE_NORM); //минуты
//			Lcd_Chr(':');
//			Lcd_BinToDec(Time.sec, 2, LCD_CHAR_SIZE_NORM); //секунды

			//Вывод темперетуры DS18B20.
			Lcd_String(1, 3);
			Lcd_Print("DS18B20 =");
			if(DS18B20_GetTemperatureSign() & DS18B20_SIGN_NEGATIVE)Lcd_Chr('-');
			else                    								Lcd_Chr('+');
			Lcd_BinToDec(dsRes/10, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('.');
			Lcd_BinToDec(dsRes%10, 1, LCD_CHAR_SIZE_NORM);
			Lcd_Print(" C");

			//Вывод информации BMI160.
//			Lcd_String(1, 4);
//			Lcd_Print("BmiID= ");
//			Lcd_u8ToHex(BMI160Str.Chip_ID);
//
//			BmiT = BMI160_GetTemperature();
//
//			Lcd_String(1, 5);
//			Lcd_Print("BmiT = ");
//			Lcd_BinToDec(BmiT/10, 2, LCD_CHAR_SIZE_NORM);
//			Lcd_Chr('.');
//			Lcd_BinToDec(BmiT%10, 1, LCD_CHAR_SIZE_NORM);
//			Lcd_Print(" C");


//			GyrX = *(uint16_t*)&BMI160Str.Data[0];
//			GyrY = *(uint16_t*)&BMI160Str.Data[2];
//			GyrZ = *(uint16_t*)&BMI160Str.Data[4];

//			Lcd_String(1, 6);
//			Lcd_Print("BmiAccZ= ");
//			Lcd_BinToDec(GyrZ, 5, LCD_CHAR_SIZE_NORM);
//
//			Lcd_String(1, 7);
//			Lcd_Print("BmiAccY= ");
//			Lcd_BinToDec(GyrY, 5, LCD_CHAR_SIZE_NORM);
//
//			Lcd_String(1, 8);
//			Lcd_Print("BmiAccX= ");
//			Lcd_BinToDec(GyrX, 5, LCD_CHAR_SIZE_NORM);

			//Вывод информации MPU6050.
			//Вывод ID
//			Lcd_String(1, 5);
//			Lcd_Print("MPU_ID= ");
//			Lcd_u8ToHex(MPU6050Str.Chip_ID);

//			Lcd_String(1, 6);
//			Lcd_Print("MPUAccX= ");
//			Lcd_BinToDec(MPU6050Str.Acc[0], 5, LCD_CHAR_SIZE_NORM);
//
//			Lcd_String(1, 7);
//			Lcd_Print("MPUAccY= ");
//			Lcd_BinToDec(MPU6050Str.Acc[1], 5, LCD_CHAR_SIZE_NORM);
//
//			Lcd_String(1, 8);
//			Lcd_Print("MPUAccZ= ");
//			Lcd_BinToDec(MPU6050Str.Acc[2], 5, LCD_CHAR_SIZE_NORM);


			Lcd_String(21, 1);
			if(Blink(INTERVAL_50_mS))Lcd_Chr(':');
			else                     Lcd_Chr(' ');
//			//Мигабщая окружность.
//			if(Blink(INTERVAL_500_mS))Lcd_Circle(100, 16, 16, PIXEL_ON);
//			else				      Lcd_Circle(100, 16, 16, PIXEL_OFF);
//			//Мигающий квадрат в правом нижмнем углу.
//			if(Blink(INTERVAL_250_mS))SSD1306_DrawFilledRectangle(118, 54, 10, 10, SSD1306_COLOR_WHITE);
//			else					  SSD1306_DrawFilledRectangle(118, 54, 10, 10, SSD1306_COLOR_BLACK);
//			//Мигающий треугольник.
//			if(Blink(INTERVAL_50_mS))SSD1306_DrawTriangle(118, 50, 123, 40, 128, 50, SSD1306_COLOR_WHITE);
//			else					 SSD1306_DrawTriangle(118, 50, 123, 40, 128, 50, SSD1306_COLOR_BLACK);
			//Рисование линии в зависимости от положение платы BMI160.
			//SSD1306_DrawLine(90, 32, 120, ((GyrX/100))+32, SSD1306_COLOR_WHITE);
			//Прогрессбар.
			//Lcd_Bar(38, 118, 27, 118, 100);

			Lcd_Update();//вывод сделан для SSD1306
			Lcd_Clear();
			//***********************************************
			//LCD 16x2 I2C - Работает.
			//Мигающий разделитель.
//			lcd_set_cursor(LCD_STRING_2, 1);
//			if(Blink(INTERVAL_500_mS))lcd_print_ch(':');
//			else                      lcd_print_ch(' ');
//
//			lcd_set_cursor(LCD_STRING_2, 2);
//			if(Blink(INTERVAL_250_mS))lcd_print_ch('@');
//			else                      lcd_print_ch('#');
//
//			lcd_set_cursor(LCD_STRING_2, 5);
//			lcd_print_string("temp=");
//			lcd_BinToDec((dsRes / 10), 2);
//			lcd_print_string(".");
//			lcd_BinToDec((dsRes % 10), 1);

			//***********************************************
			//Акселерометр BMI160.

			//Чтение данных акселерометра.
			//I2C_Read(I2C2, BMI160_ADDR, 0x12, BMI160Str.Data, 6);
			//Фильтрация измерений акселерометра.
//			GyrX = *(uint16_t*)&BMI160Str.Data[0];
//			GyrX = Average(*(uint16_t*)&BMI160Str.Data[0], 10);//Усреднение
//			GyrX = expRunningAverage(*(uint16_t*)&BMI160Str.Data[0]);
//			GyrX = runMiddleArifmOptim(*(uint16_t*)&BMI160Str.Data[0]);
//			GyrY = Filter(*(uint16_t*)&BMI160Str.Data[0]);     //ФНЧ
//			GyrZ = FilterSMA(*(uint16_t*)&BMI160Str.Data[0]);  //скользящее среднее
			//***********************************************
			//Акселерометр MPU6050.

			//Чтение данных акселерометра.
			//MPU6050_ReadAcc((uint8_t*)MPU6050Str.Acc);
			//Фильтрация измерений акселерометра.
			//MPU6050Str.Acc[0] = Filter(MPU6050Str.Acc[0]);     //ФНЧ
			//MPU6050Str.Acc[0] = Average(MPU6050Str.Acc[0], 32);//Усреднение
			//MPU6050Str.Acc[0] = expRunningAverage(MPU6050Str.Acc[0]);

//			MPU6050Str.Acc[0] = runMiddleArifmOptim(MPU6050Str.Acc[0] & 0x7FFF);
//			MPU6050Str.Acc[1] = Filter(MPU6050Str.Acc[1]);//ФНЧ
//			MPU6050Str.Acc[2] = expRunningAverage(MPU6050Str.Acc[2]);
			//***********************************************
			//датчик температуры и влажности AHT10.
			if(tic(Blink(INTERVAL_250_mS))) AHT10_ReadData();

			//Вывод темперартуры.
			Lcd_String(1, 4);
			Lcd_Print("AHT10_T =");
			if(AHT10_GetTemperatureSign() == AHT10_SIGN_NEGATIVE)Lcd_Chr('-');
			else                    							 Lcd_Chr('+');

			Lcd_BinToDec(AHT10()->Temperature / 10, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('.');
			Lcd_BinToDec(AHT10()->Temperature % 10, 1, LCD_CHAR_SIZE_NORM);
			Lcd_Print(" C");
			//Вывод влажности.
			AHT10()->Humidity = Filter_EMA(AHT10()->Humidity);
			Lcd_String(1, 5);
			Lcd_Print("AHT10_H = ");
			Lcd_BinToDec(AHT10()->Humidity / 100, 2, LCD_CHAR_SIZE_NORM);
			Lcd_Chr('.');
			Lcd_BinToDec(AHT10()->Humidity % 10, 1, LCD_CHAR_SIZE_NORM);
			Lcd_Print(" %");
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
	if(++msCountForDS18B20 >= 1000)
		{
			msCountForDS18B20 = 0;
			FlagsStr.DS18B20  = 1;
		}
	//-----------------------------
//	Scheduler_TimerService();
	msDelay_Loop();
	Blink_Loop();
	Encoder()->Loop();
}
//*******************************************************************************************
//*******************************************************************************************
