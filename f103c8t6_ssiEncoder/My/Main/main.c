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
volatile uint32_t sysTick      = 0;
volatile uint32_t uSecTick     = 0;
volatile uint32_t EncoderTicks = 0;
volatile uint32_t EncoderTicksVirt = 0;
volatile uint32_t EncoderTicksTemp = 0;
volatile float Angle           = 0.0;
volatile float OldAngle        = 0.0;
volatile float DeltaAngle      = 0.0;
volatile float RPM             = 0.0;

uint8_t txBuf[64] = {0,};
//*******************************************************************************************
//*******************************************************************************************
void Led_Blink(void){

	if(Blink(INTERVAL_50_mS)) Led_PC13_On();
	else					  Led_PC13_Off();
//	Led_PC13_Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
//Работа с энкодером AMM3617.Энкодер выдает 17-тибитный код Грея.
#define ENCODER_TIMEOUT 	(30 / 10)			      //деление на 10 так как uSecTick = 10 мкСек.
//#define ENCODER_NUM_STEP 	131072 					  //количество шагов энкодера для 17 бит
#define ENCODER_NUM_STEP    65536					  //количество шагов энкодера для 16 бит
//#define ENCODER_NUM_STEP    32768					  //количество шагов энкодера для 15 бит
#define ENCODER_QUANT  		(360.0 / ENCODER_NUM_STEP)//количество градусов в одном наге энкодера.

#define _RPM						60
#define NUM_MILLISECOND_PER_SECOND  1000
#define TIMEOUT_FOR_CALC_SPEED_MS	100
#define QUANT_FOR_100mS            ((_RPM * (NUM_MILLISECOND_PER_SECOND/TIMEOUT_FOR_CALC_SPEED_MS) * 1000) / 360.0)
#define QUANT_FOR_40uS             (float)(5555555.55)
//************************************************************
//************************************************************
//Преобразование кода Грея в бинарный код.
//uint32_t GrayToBin(uint32_t grayCode){
//
//	grayCode ^= grayCode >> 1;
//	grayCode ^= grayCode >> 2;
//	grayCode ^= grayCode >> 4;
//	grayCode ^= grayCode >> 8;
//	grayCode ^= grayCode >> 16;
//	return grayCode;
//}
//------------------------------------------------------------
//Получение значение поворота энкодера.
//uint32_t Encoder_GetTicks(void){
//
//	//Чтение и выравнивание данных из энкодера.
//	//uint32_t encoderVal = (Spi1Rx3Byte() >> 6) & 0x0001FFFF; //Разрешения энкодера 17 бит.
//	//uint32_t encoderVal = (Spi1Rx3Byte() >> 7) & 0x0000FFFF; //Разрешения энкодера 16 бит.
//	//uint32_t encoderVal = (Spi1Rx3Byte() >> 8) & 0x00007FFF; //Разрешения энкодера 15 бит.
//	//return GrayToBin((Spi1Rx3Byte() >> 7) & 0x0000FFFF);//Преобразование кода Грея в двоичный код.
//
//
//	uint32_t grayCode = (Spi1Rx3Byte() >> 7) & 0x0000FFFF;//Разрешения энкодера 16 бит.
//
//	grayCode ^= grayCode >> 1;
//	grayCode ^= grayCode >> 2;
//	grayCode ^= grayCode >> 4;
//	grayCode ^= grayCode >> 8;
//	grayCode ^= grayCode >> 16;
//
//	return grayCode;
//}
//*******************************************************************************************
//*******************************************************************************************
void BinToDecWithoutDot(uint32_t var, uint8_t* buf){

	*(buf+0) = (uint8_t)(var / 100000) + '0';
	var %= 100000;

	*(buf+1) = (uint8_t)(var / 10000) + '0';
	var %= 10000;

	*(buf+2) = (uint8_t)(var / 1000) + '0';
	var %= 1000;

	//*(buf+3) = ',';

	*(buf+3) = (uint8_t)(var / 100) + '0';
	var %= 100;

	*(buf+4) = (uint8_t)(var / 10) + '0';
	*(buf+5) = (uint8_t)(var % 10) + '0';
}
//------------------------------------------------------------
void BinToDecWithDot(uint32_t var, uint8_t* buf){

	*(buf+0) = (uint8_t)(var / 100000) + '0';
	var %= 100000;

	*(buf+1) = (uint8_t)(var / 10000) + '0';
	var %= 10000;

	*(buf+2) = (uint8_t)(var / 1000) + '0';
	var %= 1000;

	*(buf+3) = ',';

	*(buf+4) = (uint8_t)(var / 100) + '0';
	var %= 100;

	*(buf+5) = (uint8_t)(var / 10) + '0';
	*(buf+6) = (uint8_t)(var % 10) + '0';
}
//------------------------------------------------------------
void BuildingAndSendTextBuffer(uint32_t timeStamp, uint32_t encodTicks, uint32_t angle, uint32_t speed){

	BinToDecWithDot(timeStamp, txBuf);
	txBuf[7] = '\t';

	BinToDecWithoutDot(encodTicks, txBuf+8);
	txBuf[14] = '\t';

	BinToDecWithDot(angle, txBuf+15);
	txBuf[22] = '\t';

	BinToDecWithDot(speed, txBuf+23);
	txBuf[30] = '\r';

	DMA1Ch4StartTx(txBuf, 31);
}
//*******************************************************************************************
//*******************************************************************************************
int main(void){

	uint32_t olduSecTicks = 0;
	volatile uint32_t q   = 0;
	//***********************************************
	Sys_Init();
	Gpio_Init();
	SysTick_Init();
	TIM4_Init();          //TIM4 настривается для периодической генерации прерывания.
	Spi1Init();           //Инициализация SPI для работы с энкодером.
	Uart1Init(USART1_BRR);

	__enable_irq();
	//************************************************************************************
	while(1)
		{
			//***********************************************
			//Чтение данных из энкодера каждые 30мкСек.
			if((uSecTick - olduSecTicks) >= ENCODER_TIMEOUT)
				{
				 	olduSecTicks = uSecTick;
				 	//Led_PC13_Toggel();//Отладка
				 	//Чтение заначения энкодера.
				 	__disable_irq();
				 	//EncoderTicks = Encoder_GetTicks();
				 	EncoderTicks  = (Spi1Rx3Byte()>>7) & 0x0000FFFF;//Разрешения энкодера 16 бит.
				 	//Преобразование кода Грея в бинарный код.
				 	EncoderTicks ^= EncoderTicks >> 1;
				 	EncoderTicks ^= EncoderTicks >> 2;
				 	EncoderTicks ^= EncoderTicks >> 4;
				 	EncoderTicks ^= EncoderTicks >> 8;
				 	EncoderTicks ^= EncoderTicks >> 16;
					//Эмуляция квадратурного(инкриментного) энкодера.
				 	//Преобразование в код Грея двух младших разрядов.
				 	//q = (EncoderTicks >> 2) & 3u;//ssi 17бит - инкриментный энкодера 8192 тика
				 	q = (EncoderTicks >> 1) & 3u;//ssi 16бит - инкриментный энкодера 8192 тика
					//     if (q == 2) q = 3;
					//else if (q == 3) q = 2;
				 	if(q > 1) q ^= 1;
					//Ногодрыг
					((q >> 1u) & 1u) ? EncAOn() : EncAOff();
					 (q &  1u) ?       EncBOn() : EncBOff();
					__enable_irq();
					//Расчет значений для будущего расчета скорости.
					Angle = ENCODER_QUANT * EncoderTicks;  //расчет угла поворота вала энкодера.
					if(OldAngle > Angle) OldAngle -= 360.0;//Это нужно для корректного расчета скорости при переходе от 359 к 0 градусов.
					DeltaAngle = Angle - OldAngle;         //приращение угла
					RPM = DeltaAngle * QUANT_FOR_100mS;    //Расчет скорости вращения.
				}
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

	static uint32_t mSecCount = 0;
	//--------------------------
	//sysTick++;
	if(++sysTick > 999999) sysTick = 0;
	//Led_PC13_Toggel(); //Для отладки.
	//--------------------------
	if(++mSecCount >= TIMEOUT_FOR_CALC_SPEED_MS)
		{
			mSecCount = 0;
			Led_PC13_Toggel(); //Ииндикация обмена
			//Расчет скорости вращения.
			//RPM = DeltaAngle * QUANT_FOR_100mS;
			OldAngle = Angle;
			//Передаем данные
			BuildingAndSendTextBuffer(sysTick,
									  EncoderTicks,
									  (uint32_t)(Angle*1000),
									  (uint32_t)RPM);
		}
	//--------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание TIM4. Каждые 10 микросекунды.
void TIM4_IRQHandler(void){

	//--------------------------
	TIM4->SR &= ~TIM_SR_UIF;//Сброс флага прерывания.
	//--------------------------
	uSecTick++;
	//Led_PC13_Toggel();
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание TIM1.
void TIM1_UP_IRQHandler(void){

	//--------------------------
	TIM1->SR &= ~TIM_SR_UIF;//Сброс флага прерывания.
	//--------------------------
	//Led_PC13_Toggel();
}
//*******************************************************************************************
//*******************************************************************************************































