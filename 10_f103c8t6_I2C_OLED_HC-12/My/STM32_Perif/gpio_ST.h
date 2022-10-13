
#ifndef _gpio_ST_H
#define _gpio_ST_H
//*******************************************************************************************
//*******************************************************************************************
//#include "stm32f10x.h"
//#include  "stm32f103xb.h"

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************

#define GPIO_POLLING_DELAY  10//50//10

#define PIN_HIGH	1
#define PIN_LOW		0

//*******************************************************************************************
// LED_PC13 - PC13
#define LED_PC13_On()     (GPIOC->BSRR = GPIO_BSRR_BS13)
#define LED_PC13_Off()    (GPIOC->BSRR = GPIO_BSRR_BR13)
#define LED_PC13_Toggel()  (GPIOC->ODR ^= GPIO_ODR_ODR13)

// HC12_SET - PA4
#define HC12_SET_GPIO_PORT	GPIOA
#define HC12_SET_GPIO_PIN	4
#define HC12_SET_High()   	(GPIOA->BSRR = GPIO_BSRR_BS4)
#define HC12_SET_Low()    	(GPIOA->BSRR = GPIO_BSRR_BR4)
#define HC12_SET_Toggel() 	(GPIOA->ODR ^= GPIO_ODR_ODR4)
//**********************************************************
// Макросы
#define GPIO_PIN_High(gpio, pin) 	(gpio->BSRR = pin)
#define GPIO_PIN_Low(gpio, pin)  	(gpio->BSRR = (pin<<15))
#define GPIO_PIN_Toggel(gpio, pin)	(gpio->ODR ^= pin)


#define GPIO_POLLING_DELAY  10//50//10

//*******************************************************************************************
//*******************************************************************************************
void GPIO_InitForOutputPushPull(GPIO_TypeDef *port, uint32_t pin);
void GPIO_InitForOutputOpenDrain(GPIO_TypeDef *port, uint32_t pin);
void GPIO_InitForInputPullUp(GPIO_TypeDef *port, uint32_t pin);


void     GPIO_Init     (void);
void     GPIO_CheckLoop(void);
uint32_t GPIO_GetPortState(GPIO_TypeDef *port);
uint32_t GPIO_GetPinState(GPIO_TypeDef *port, uint32_t pin);

//*******************************************************************************************
//*******************************************************************************************
#endif /*_gpio_ST_H*/


