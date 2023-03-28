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

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************
#define ENCODER_BUTTON_RELEASED	0
#define ENCODER_BUTTON_PRESSED	1
#define ENCODER_BUTTON_TIMEOUT	10
//**********************************
//Состояние энкодера.
typedef enum{
	ENCODER_NO_TURN = 0,
	ENCODER_TURN,
	ENCODER_TURN_RIGHT,
	ENCODER_TURN_LEFT
}EncoderState_t;
//**********************************
//Состояние кнопки энкодера.
typedef enum{
	RELEASED = 0,
	PRESSED,
	TOGGLE,
}EncoderButtonState_t;
//**********************************
typedef struct{
	GPIO_TypeDef 	*GpioPort_A;
	uint32_t	 	GpioPin_A;

	GPIO_TypeDef 	*GpioPort_B;
	uint32_t	  	GpioPin_B;

	GPIO_TypeDef 	*GpioPort_BUTTON;
	uint32_t	  	GpioPin_BUTTON;

	uint32_t	  				encoderNumber;
	__IO EncoderState_t	  	 	encoderState;
	__IO EncoderButtonState_t	buttonState;
}Encoder_t;
//*******************************************************************************************
//*******************************************************************************************
void 			  	 ENCODER_Init(Encoder_t *encoder);
void 				 ENCODER_ScanLoop(Encoder_t *encoder);
void 				 ENCODER_IncDecParam(Encoder_t *encoder, uint32_t *parameter, uint32_t step, uint32_t min, uint32_t max);
EncoderButtonState_t ENCODER_GetButton(Encoder_t *encoder);
//*******************************************************************************************
//*******************************************************************************************
#endif //_Encoder_H








