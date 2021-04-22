
#include "Encoder.h"

//-----------------------------------------------------------------------------
static void Encoder_Loop(void);
static void Encoder_Turn(uint16_t *parameter, uint16_t min, uint16_t max);

static Encoder_t EncoderStr = {.ButtonIsPress = 0,
							   .Loop          = &Encoder_Loop,
							   .Rotation      = &Encoder_Turn};

static uint8_t EncoderStateReg = ENCODER_NO_TURN;//Регистр состияния энкодера.
//-----------------------------------------------------------------------------
static void Encoder_Turn(uint16_t *parameter, uint16_t min, uint16_t max){

	//--------------------
	switch(EncoderStateReg){
		//-----------
		//поворот вправо.
		case ENCODER_TURN_RIGHT:
			if ((*parameter) < max)(*parameter)++;    //Проверка на  максимум.
			else                   (*parameter) = min;//Закольцовывание редактирования параметра.
			EncoderStateReg = ENCODER_NO_TURN;
		break;
		//-----------
		//поворот влево.		
		case ENCODER_TURN_LEFT:
			if ((*parameter) > min)(*parameter)--;    //Проверка на минимум.
			else                   (*parameter) = max;//Закольцовывание редактирования параметра.
			EncoderStateReg = ENCODER_NO_TURN;
		break;
		//-----------
		default:
		break;	
		//-----------
	}
	//--------------------	
}
//-----------------------------------------------------------------------------
/*Когда энкодер не вращают на обоих его выводах(A и В) единицы(внутряння подтяжка МК).Это состояние 3 энкодера - начальное состояние энкодера.
*Так-же начальным состоянием может быть  состояние энкодера когда А и В ровны 0.
*Когда вращаю вправо, то после состояния 3 энкодер передет в состояние 1 (А=1, В=0).
*Когда вращают влево, то после состояния 3 энкодер прердет в состояние 2 (А=0, В=1). */
static void Encoder_Loop(void){
	
	static uint8_t  cycle   = 0;
	static uint8_t  msCount = 0;
	static uint16_t but[3]  = {0, 0, 0};
	//--------------------
	//Обработка вращения энкодера.
	switch(EncoderStateReg){
		//-----------
		//нет вращения энекодера.
		case ENCODER_NO_TURN:
			if(!(EncoderPORT->IDR & EncoderPinA) && !(EncoderPORT->IDR & EncoderPinB))
				{
					EncoderStateReg = ENCODER_TURN;
				}
		break;
		//-----------
		//произошло вращение энкодера.		
		case ENCODER_TURN:
			if( !(EncoderPORT->IDR & EncoderPinA) && (EncoderPORT->IDR & EncoderPinB))
				{
					EncoderStateReg = ENCODER_TURN_RIGHT;//щелчок вправо.
				}     
		  if( (EncoderPORT->IDR & EncoderPinA) && !(EncoderPORT->IDR & EncoderPinB))
				{
			  	  	EncoderStateReg = ENCODER_TURN_LEFT;//щелчок влево.
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
		  msCount = 0;//сброс счетчика
		  //-----------
			if(cycle < 3)
				{
					but[cycle] = EncoderPORT->IDR & EncoderButtonPin;//Считывание состояние вывода.
					cycle++;
				} 
			else
				{
					cycle = 0;
					//Мажоритарное определение состояния выводов.
					if(~((but[0] & but[1]) | (but[1] & but[2]) | (but[0] & but[2])) & EncoderButtonPin) 
						{
							 EncoderStr.ButtonIsPress = 1;
						}
					else EncoderStr.ButtonIsPress = 0;
				}
			//-----------
		}
	//--------------------
}
//-----------------------------------------------------------------------------
void Encoder_Init(void){
	
//	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
//	//--------------------
//	EncoderPORT->CRH &= ~(GPIO_CRH_CNF12  | GPIO_CRH_CNF11  | GPIO_CRH_CNF10);	 //
//	EncoderPORT->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_MODE11 | GPIO_CRH_MODE10);	 //
//	//Input pull-up
//    EncoderPORT->CRH |= (GPIO_CRH_CNF12_1 | GPIO_CRH_CNF11_1 | GPIO_CRH_CNF10_1);//
//	EncoderPORT->ODR |= (GPIO_ODR_ODR12   | GPIO_ODR_ODR11   | GPIO_ODR_ODR10);	 //


	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	//--------------------
	GPIOB->CRH &= ~(GPIO_CRH_CNF12  | GPIO_CRH_CNF11  | GPIO_CRH_CNF10);	 //
	GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_MODE11 | GPIO_CRH_MODE10);	 //
	//Input pull-up
	GPIOB->CRH |= (GPIO_CRH_CNF12_1 | GPIO_CRH_CNF11_1 | GPIO_CRH_CNF10_1);//
	GPIOB->ODR |= (GPIO_ODR_ODR12   | GPIO_ODR_ODR11   | GPIO_ODR_ODR10);	 //
}
//-----------------------------------------------------------------------------
Encoder_t* Encoder(void){

	return &EncoderStr;
}
//-----------------------------------------------------------------------------




