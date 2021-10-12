
#include "Encoder.h"

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
void Encoder_Init(Encoder_t *encoder){

	//Конфигурация выводов: Input with pull-up.
	_encoder_GpioInit(encoder->GPIO_PORT_A,      encoder->GPIO_PIN_A);
	_encoder_GpioInit(encoder->GPIO_PORT_B,      encoder->GPIO_PIN_B);
	_encoder_GpioInit(encoder->GPIO_PORT_BUTTON, encoder->GPIO_PIN_BUTTON);
}
//**********************************************************
/**
 * @brief: Фу-я опроса состояния энкодера. Вызывается каждую 1мСек.
 * @In_param:
 * @Out_param:
 */
void Encoder_ScanLoop(Encoder_t *encoder){

		   uint32_t pinA      = (1 << encoder->GPIO_PIN_A);
		   uint32_t pinB      = (1 << encoder->GPIO_PIN_B);
		   uint32_t pinButton = (1 << encoder->GPIO_PIN_BUTTON);
	static uint8_t  cycle     = 0;
	static uint8_t  msCount   = 0;
	static uint16_t but[3]    = {0,};
	//---------------------
	//Обработка вращения энкодера.
	switch(encoder->ENCODER_STATE){
		//-----------
		//нет вращения энекодера.
		case ENCODER_NO_TURN:
			if(!(encoder->GPIO_PORT_A->IDR & pinA) && !(encoder->GPIO_PORT_B->IDR & pinB))
			{
				encoder->ENCODER_STATE = ENCODER_TURN;
			}
		break;
		//-----------
		//произошло вращение энкодера.
		case ENCODER_TURN:
			//щелчок вправо.
			if(!(encoder->GPIO_PORT_A->IDR & pinA) && (encoder->GPIO_PORT_B->IDR & pinB))
			{
				encoder->ENCODER_STATE = ENCODER_TURN_RIGHT;
			}
			//щелчок влево.
			if((encoder->GPIO_PORT_A->IDR & pinA) && !(encoder->GPIO_PORT_B->IDR & pinB))
			{
				encoder->ENCODER_STATE = ENCODER_TURN_LEFT;
			}
		break;
		//-----------
		default:
		break;
		//-----------
	}
	//--------------------
	//Опрос кнопки энкодера.
	if(++msCount >= ENCODER_BUTTON_TIMEOUT)
	{
		msCount = 0;
		//-----------
		if(++cycle <= 3) but[cycle] = encoder->GPIO_PORT_BUTTON->IDR & pinButton;//Считывание состояние вывода.
		else
		{
			cycle = 0;
			//Мажоритарное определение состояния выводов.
			if(~((but[0] & but[1]) | (but[1] & but[2]) | (but[0] & but[2])) & pinButton)
			{
				encoder->BUTTON_STATE = 1;
			}
			else encoder->BUTTON_STATE = 0;
		}
		//-----------
	}
	//--------------------
}
//**********************************************************
/**
 * @brief: Фу-я инкрименте/декримента переменной при каждом шелчке энкодера.
 * @In_param: parameter - адрес переменной значение которой хотим  изменять;
 *            min - минимальное значение до которого будет уменьшаться значение переменной;
 *            max - максимальное значение до которого будет увеличиваться значение переменной.
 * @Out_param:
 */
void Encoder_IncDecParam(Encoder_t *encoder, uint16_t *parameter, uint16_t min, uint16_t max){

	//--------------------
	switch(encoder->ENCODER_STATE){
		//-----------
		//поворот вправо.
		case ENCODER_TURN_RIGHT:
			if ((*parameter) < max)(*parameter)++;    //Проверка на  максимум.
			else                   (*parameter) = min;//Закольцовывание редактирования параметра.
			encoder->ENCODER_STATE = ENCODER_NO_TURN;
		break;
		//-----------
		//поворот влево.
		case ENCODER_TURN_LEFT:
			if ((*parameter) > min)(*parameter)--;    //Проверка на минимум.
			else                   (*parameter) = max;//Закольцовывание редактирования параметра.
			encoder->ENCODER_STATE = ENCODER_NO_TURN;
		break;
		//-----------
		default:
		break;
		//-----------
	}
	//--------------------
}
//*******************************************************************************************
//*******************************************************************************************




