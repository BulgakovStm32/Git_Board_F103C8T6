/*
 * Encoder.h
 *
 *  Created on:
 *      Author:
 */

#ifndef _Encoder_H
#define _Encoder_H
//*******************************************************************************************
//*******************************************************************************************

//#include "stm32f10x.h"
//#include "gpio_ST.h"

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
//Порт, к которому подключен энкодер.
#define EncoderPORT 	  GPIOC
#define EncoderPinA  	  GPIO_IDR_IDR12  //вывод А энкодера.
#define EncoderPinB  	  GPIO_IDR_IDR11  //вывод В энкодера.
#define EncoderButtonPin  GPIO_IDR_IDR10  //кнопка энкодера.
//**********************************
#define ENCODER_NO_TURN     	0
#define ENCODER_TURN        	1
#define ENCODER_TURN_RIGHT  	2
#define ENCODER_TURN_LEFT		3

#define ENCODER_BUTTON_TIMEOUT	10
//**********************************
//Состояние кнопки энкодера.


//**********************************
typedef struct{
	GPIO_TypeDef *GPIO_PORT_A;
	uint32_t	  GPIO_PIN_A;

	GPIO_TypeDef *GPIO_PORT_B;
	uint32_t	  GPIO_PIN_B;

	GPIO_TypeDef *GPIO_PORT_BUTTON;
	uint32_t	  GPIO_PIN_BUTTON;
	//------------
	uint8_t		  ENCODER_STATE;
	uint8_t		  BUTTON_STATE;
	uint32_t	  ENCODER_NUMBER;
}Encoder_t;
//**********************************

//typedef struct{
//	uint8_t ButtonIsPress :1;
//	//--------
//	void (*Loop)();
//	void (*Rotation)(uint16_t *parameter, uint16_t min, uint16_t max);
//}Encoder_t;
//*******************************************************************************************
//*******************************************************************************************
void Encoder_Init(Encoder_t *encoder);
void Encoder_ScanLoop(Encoder_t *encoder);
void Encoder_IncDecParam(Encoder_t *encoder, uint16_t *parameter, uint16_t min, uint16_t max);
//*******************************************************************************************
//*******************************************************************************************
#endif //_Encoder_H


