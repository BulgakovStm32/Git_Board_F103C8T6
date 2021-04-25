//-----------------------------------------------------------------------------

#include "adc_ST.h"

//-----------------------------------------------------------------------------
//Настройка АЦП. 
void Adc_Init(void){

	RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN | //Разрешить тактирование АЦП1.
				     RCC_APB2ENR_IOPCEN );//Разрешить тактирование порта PORTC
	//--------------------
//	GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 | //PC0 - аналоговый вход.
//					GPIO_CRL_CNF1 | GPIO_CRL_MODE1 | //PC1 - аналоговый вход.
//					GPIO_CRL_CNF2 | GPIO_CRL_MODE2 | //PC2 - аналоговый вход.
//					GPIO_CRL_CNF3 | GPIO_CRL_MODE3 | //PC3 - аналоговый вход.
//					GPIO_CRL_CNF4 | GPIO_CRL_MODE4 | //PC4 - аналоговый вход.
//					GPIO_CRL_CNF5 | GPIO_CRL_MODE5 );//PC5 - аналоговый вход.


	GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 | //PA0 - аналоговый вход.
					GPIO_CRL_CNF1 | GPIO_CRL_MODE1 | //PA1 - аналоговый вход.
					GPIO_CRL_CNF2 | GPIO_CRL_MODE2 );//PA2 - аналоговый вход.
	//--------------------	
	RCC->CFGR &= ~RCC_CFGR_ADCPRE;	   //
	RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV8;//предделитель 8 (72МГц/8 = 9МГц).
	//предочистка регистра.
	ADC1->CR1   = 0;
	ADC1->SQR2  = 0;
	ADC1->SQR3  = 0;
	//Настройка работы АЦП.
	//Время выборки канала. При тактировании АЦП 9МГц и при 28,5 циклов преобразование вемя преобразование получится ~3,16мкС.
	ADC1->SMPR2 = (ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | // время выборки канала 0 = 28,5 циклов
				   ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP1_1 | // время выборки канала 1 = 28,5 циклов
				   ADC_SMPR2_SMP2_0 | ADC_SMPR2_SMP2_1 );// время выборки канала 2 = 28,5 циклов
	//Выбор каналов.
	ADC1->SQR1 = ~ADC_SQR1_L; //1 регулярный канал.

	ADC1->CR2 =	(ADC_CR2_EXTSEL  | //выбрать источником запуска разряд SWSTART
				 ADC_CR2_EXTTRIG | //разр. внешний запуск регулярного канала
				 ADC_CR2_ADON);    //включить АЦП
	//Рабочий вариант самокалибровки АЦП.
	//Вычитать значение самокалибровки ненужно, АЦП это делает сам.
	ADC1->CR2 |= ADC_CR2_RSTCAL;         //Сброс калибровки
	while (ADC1->CR2 & ADC_CR2_RSTCAL){};//ждем окончания сброса
	ADC1->CR2 |= ADC_CR2_CAL;            //запуск калибровки
	while (ADC1->CR2 & ADC_CR2_CAL){};   //ждем окончания калибровки

	//--------------------
	ADC1->CR2 &= ~ADC_CR2_ADON;
	//Режим один регулярный канал, непрерывное преобразование.
	ADC1->CR2 |= ADC_CR2_CONT; //разрешение непрерывного режима

	//Инжект. каналы
	ADC1->JSQR |= ADC_JSQR_JSQ1_0; //канал 1 для первого инжект преобразования.
	ADC1->CR2  |= ADC_CR2_JEXTSEL; //источник запуска - JSWSTART
	ADC1->CR2  |= ADC_CR2_JEXTTRIG;//разрешение внешнего запуска для инжектированных
	ADC1->CR1  |= ADC_CR1_JAUTO;   //разрешение автоматического преобразования инжект. каналов

	ADC1->CR2 |= ADC_CR2_ADON;

	ADC1->CR2 |= ADC_CR2_SWSTART; //запуск переобразование регулярного группы
	ADC1->CR2 |= ADC_CR2_JSWSTART;//запуск переобразование инжектированной группы
	//--------------------
}
//-----------------------------------------------------------------------------
//Одно измерение АЦП.
uint16_t Adc_GetMeas(uint8_t adcChannel){
  
	ADC1->SQR3 = adcChannel;      	  //загрузить номер канала.
	ADC1->CR2 |= ADC_CR2_SWSTART;     //запуск преобразования в регулярном канале.
	while(!(ADC1->SR & ADC_SR_EOC)){};//дождаться окончания преобразования
	//Вычитать значение самокалибровки ненужно, АЦП это делает сам.
	return (uint16_t)((ADC1->DR * ADC_QUANT) / 10000);//(uint16_t)((ADC1->DR * 8068) / 10000);
}
//-----------------------------------------------------------------------------
uint16_t Adc_GetRegDR(ADC_TypeDef *adc){

	return adc->DR;
}
//-----------------------------------------------------------------------------
uint16_t Adc_GetRegJDRx(ADC_TypeDef *adc, uint8_t ch){

	if(ch == 1)return adc->JDR1;
	if(ch == 2)return adc->JDR2;
	if(ch == 3)return adc->JDR3;
	if(ch == 4)return adc->JDR4;
	return 0;
}
//-----------------------------------------------------------------------------



