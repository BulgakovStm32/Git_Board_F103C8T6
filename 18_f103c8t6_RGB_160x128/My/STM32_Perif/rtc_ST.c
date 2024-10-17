
#include "rtc_ST.h"

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//function инициализация RTC                                                                
//argument none 
//result   1 - инициализация выполнена; 0 - часы уже были инициализированы 
void RTC_Init(void){
  
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN | 	//Power interface clock enable
					 RCC_APB1ENR_BKPEN);	//Backup interface clock enable
	//разрешить доступ к области резервных данных
	PWR->CR |= PWR_CR_DBP;					//Disable Backup Domain write protection
	//--------------------
	//если часы выключены - инициализировать их
//	if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN)
//	{
		RCC->BDCR |=  RCC_BDCR_BDRST;				//Backup domain software reset
		RCC->BDCR &= ~RCC_BDCR_BDRST;

		RCC->BDCR |= RCC_BDCR_LSEON;			 	//External Low Speed oscillator enable
		while (!(RCC->BDCR & RCC_BDCR_LSERDY)){};	//1:LSE oscillator ready

		RCC->BDCR |= RCC_BDCR_RTCSEL_LSE; 			//LSE oscillator clock used as RTC clock
		//RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;			//LSI oscillator clock used as RTC clock
		RCC->BDCR |= RCC_BDCR_RTCEN;				//RTC clock enable

		//--------------------
		RTC->CRL  |= RTC_CRL_CNF;	//разрешить конфигурирование регистров RTC

		//RTC prescaler. F_TR_CLK = F_RTC_CLK/(PRL[19:0]+1)
		RTC->PRLH  = 0;			//RTC prescaler load register high (RTC_PRLH)
		RTC->PRLL  = 32768 - 1;	//RTC prescaler load register low  (RTC_PRLL)

		RTC->CRL &= ~RTC_CRL_RSF;	//Registers Synchronized Flag
		while(!(RTC->CRL & RTC_CRL_RSF)){}

		//Настройка прерывания RTC.
		//RTC->CRH = RTC_CRH_SECIE;//прерывание от секундных импульсов
		//RTC->CRH = RTC_CRH_OWIE; //прерывание при переполнении счетного регистра


		while(!(RTC->CRL & RTC_CRL_RTOFF)){};	//
		RTC->CRL |= RTC_CRL_CNF;			 	//разрешить конфигурирование регистров RTC

		RTC->ALRH = 0;
		RTC->ALRL = 2;

		RTC->CNTH = 0;
		RTC->CNTL = 0;

		RTC->CRH  = RTC_CRH_ALRIE;			 	//прерывание при совпадении счетного и сигнального регистра

		RTC->CRL &= ~RTC_CRL_CNF;				//выйти из режима конфигурирования
		while(!(RTC->CRL & RTC_CRL_RTOFF)){};	//
//	}
	//--------------------
	//NVIC_EnableIRQ(RTC_IRQn);			//Разрешение прерывания от модуля RTC
	//NVIC_EnableIRQ(RTC_Alarm_IRQn);	//RTC Alarm through EXTI Line Interrupt
}
//-----------------------------------------------------------------------------
//function  читает счетчик RTC
//result    текущее значение счетного регистра
uint32_t RTC_GetCounter(void){
  
//  volatile uint32_t temp;

//  __disable_irq();
//  temp = (uint32_t)((RTC->CNTH << 16) | RTC->CNTL);
//  __enable_irq();

//  return  temp;
    return (uint32_t)((RTC->CNTH << 16) | RTC->CNTL);
}
//-----------------------------------------------------------------------------
//function  записывает новое значение в счетчик RTC
//argument  новое значение счетчика               
void RTC_SetCounter(uint32_t value){
  
  RTC->CRL |= RTC_CRL_CNF;    //включить режим конфигурирования
  RTC->CNTH = value>>16;      //записать новое значение счетного регистра
  RTC->CNTL = value;          //
  RTC->CRL &= ~RTC_CRL_CNF;   //выйти из режима конфигурирования
}
//-----------------------------------------------------------------------------
void RTC_IT_Handler(void){

	//LedPC13Toggel();
	//--------------------
	//причина прерывания - переполнение входного делителя (новая секунда)
	if(RTC->CRL & RTC_CRL_SECF)
	{
		RTC->CRL &= ~RTC_CRL_SECF;    //сбросить флаг (обязательно!!!)
		//выполняем какие-то действия
	}
	//--------------------
	//причина прерывания - совпадение счетного и сигнального регистра
	if(RTC->CRL & RTC_CRL_ALRF)
	{
		RTC->CRL &= ~RTC_CRL_ALRF;    //сбросить флаг (обязательно!!!)

		//LedPC13Toggel();

		while(!(RTC->CRL & RTC_CRL_RTOFF)){};

		RTC->CRL |= RTC_CRL_CNF;	//разрешить конфигурирование регистров RTC

		RTC->CNTH = 0;
		RTC->CNTL = 0;

		RTC->CRL &= ~RTC_CRL_CNF;	//выйти из режима конфигурирования

		while(!(RTC->CRL & RTC_CRL_RTOFF)){};

		//выполняем какие-то действия
	}
	//--------------------
	//причина прерывания - переполнение счетного регистра
	if(RTC->CRL & RTC_CRL_OWF)
	{
		RTC->CRL &= ~RTC_CRL_OWF;     //сбросить флаг (обязательно!!!)
		//выполняем какие-то действия
		//LED_ACT_Toggel();
	}
	//--------------------
}
//-----------------------------------------------------------------------------





