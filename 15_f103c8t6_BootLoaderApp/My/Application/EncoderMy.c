/*
 * Encoder.c
 *
 *  Created on:
 *      Author:
 */
//*******************************************************************************************
//*******************************************************************************************

#include "EncoderMy.h"

//*******************************************************************************************
//*******************************************************************************************
static void _encoder_GpioInit(GPIO_TypeDef *gpio, uint32_t pin){
	
	uint32_t pinTemp = 1 << pin;
	//Включение тактирования портов.
		 if(gpio == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	else if(gpio == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	else if(gpio == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	else return;
	//Конфигурация выводов: Input with pull-up.
	if(pin <= 7)
	{
		pin = pin * 4;
		gpio->CRL &= ~(0x03 << pin);//GPIO_CRL_MODEx - 00:Input mode (reset state)
		//CNFy[1:0]: 10 - Input with pull-up / pull-down.
		gpio->CRL &= ~(0x03 << (pin + 2));
		gpio->CRL |=  (0x02 << (pin + 2));
	}
	else
	{
		pin = (pin - 8) * 4;
		gpio->CRH &= ~(0x03 << pin);//GPIO_CRL_MODEx - 00:Input mode (reset state)
		//CNFy[1:0]: 10 - Input with pull-up / pull-down.
		gpio->CRH &= ~(0x03 << (pin + 2));
		gpio->CRH |=  (0x02 << (pin + 2));
	}
	gpio->ODR |= pinTemp; //pull-up.
}
//*******************************************************************************************
//*******************************************************************************************
/**
 * @brief: Фу-я инициализации выводов STM32 для работы энкодера.
 * @In_param:
 * @Out_param:
 */
void ENCODER_Init(Encoder_t *encoder){

	//Конфигурация выводов: Input with pull-up.
	_encoder_GpioInit(encoder->GpioPort_A,      encoder->GpioPin_A);
	_encoder_GpioInit(encoder->GpioPort_B,      encoder->GpioPin_B);
	_encoder_GpioInit(encoder->GpioPort_BUTTON, encoder->GpioPin_BUTTON);
	encoder->encoderState = ENCODER_STATE_NO_TURN;
	encoder->buttonState  = ENCODER_BUT_RELEASED;
}
//**********************************************************
/**
 * @brief: Фу-я опроса состояния энкодера Вариант 2. Вызывается каждую 1мСек.
 * @In_param:
 * @Out_param:
 */
void ENCODER_ScanLoop(Encoder_t *encoder){

	uint32_t pinA         = (1 << encoder->GpioPin_A);
	uint32_t pinB         = (1 << encoder->GpioPin_B);
	uint32_t pinButton    = (1 << encoder->GpioPin_BUTTON);
	uint8_t  currentState =  0;
	//--------------------
	//Определение состояния энкодера.
	static uint8_t oldState = 0; //хранит последовательность состояний энкодера

	//проверяем состояние выводов микроконтроллера
	if(encoder->GpioPort_A->IDR & pinA) currentState |= 1<<0;
	if(encoder->GpioPort_B->IDR & pinB) currentState |= 1<<1;

	//если равно предыдущему, то выходим
	if(currentState != (oldState & 0b00000011))
	{
		//если не равно, то сдвигаем и сохраняем
		oldState = (oldState << 2) | currentState;
		//сравниваем получившуюся последовательность
		if(oldState == 0b11100001) encoder->encoderState = ENCODER_STATE_TURN_RIGHT;
		if(oldState == 0b11010010) encoder->encoderState = ENCODER_STATE_TURN_LEFT;
	}
	//--------------------
	//Опрос кнопки энкодера.
	static uint8_t oldStateButton = 0; //хранит последовательность состояний кнопки.
	currentState = 0;

	//проверяем состояние выводов микроконтроллера
	if(encoder->GpioPort_BUTTON->IDR & pinButton) currentState |= 1<<0;

	//если равно предыдущему, то выходим
	//if(currentState == (oldStateButton & 0b00000001)) return;

	//если не равно, то сдвигаем и сохраняем
	oldStateButton = (oldStateButton << 1) | currentState;

	//сравниваем получившуюся последовательность
	if(oldStateButton == 0b00000000) encoder->buttonState = ENCODER_BUT_PRESSED;
	if(oldStateButton == 0b11111111) encoder->buttonState = ENCODER_BUT_RELEASED;
	//--------------------
}
//**********************************************************
/**
 * @brief: Фу-я инкрименте/декримента переменной при каждом шелчке энкодера.
 * @In_param: parameter - адрес переменной значение которой хотим  изменять;
 * 			  step - шаг изменения значения переменной;
 *            min  - минимальное значение до которого будет уменьшаться значение переменной;
 *            max  - максимальное значение до которого будет увеличиваться значение переменной.
 * @Out_param:
 */
void ENCODER_IncDecParam(Encoder_t *encoder, uint32_t *parameter, uint32_t step, uint32_t min, uint32_t max){

	//--------------------
	switch(encoder->encoderState){
		//-----------
		//поворот вправо.
		case ENCODER_STATE_TURN_RIGHT:
			if ((*parameter) < max)(*parameter)+= step;//Проверка на  максимум.
			else                   (*parameter) = min; //Закольцовывание редактирования параметра.
			encoder->encoderState = ENCODER_STATE_NO_TURN;
		break;
		//-----------
		//поворот влево.
		case ENCODER_STATE_TURN_LEFT:
			if ((*parameter) > min)(*parameter)-= step;//Проверка на минимум.
			else                   (*parameter) = max; //Закольцовывание редактирования параметра.
			encoder->encoderState = ENCODER_STATE_NO_TURN;
		break;
		//-----------
		default:
		break;
		//-----------
	}
	//--------------------
}
//**********************************************************
EncoderButtonState_t ENCODER_GetButton(Encoder_t *encoder){

	return encoder->buttonState;
}
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************














