/*
 *
 *
 *    Created on:
 *        Author:
 *  Редактировал: Беляев А.А. 17.10.2021
 *
 * За основу взят планировщик задач с сайта ChipEnable.ru
 * http://chipenable.ru/index.php/programming-avr/item/110-planirovschik.html
 */

#include "rtos.h"

//*******************************************************************************************
//*******************************************************************************************

volatile static Task_t   TaskArray[MAX_TASKS];// очередь задач
volatile static uint32_t ArrayTail = 0;		  // "хвост" очереди
volatile static uint32_t TickCount = 0;		  //

//*******************************************************************************************
//*******************************************************************************************
//Глобальное запрещение прерываний.
__INLINE static void _disableInterrupt(void){

	__disable_irq();
}
//**********************************************************
//Глобальное разрешение рерываний.
__INLINE static void _enableInterrupt(void){

	__enable_irq();
}
//*******************************************************************************************
//*******************************************************************************************
/* Инициализация РТОС.
 *
 */
void RTOS_Init(void){

//   TCCR0        |= (1<<CS01)|(1<<CS00);         // прескалер - 64
//   TIFR         = (1<<TOV0);                   // очищаем флаг прерывания таймера Т0
//   TIMSK        |= (1<<TOIE0);                  // разрешаем прерывание по переполнению
//   TIMER_COUNTER = TIMER_START;                 // загружаем начальное зн. в счетный регистр

	ArrayTail = 0;//"хвост" в 0
}
//**********************************************************
/* Добавление задачи в список
 *
 */
void RTOS_SetTask(void(*taskFunc)(void), uint16_t taskDelay, uint16_t taskPeriod){
   
	volatile Task_t *task = &TaskArray[0];
	//-----------------------------
	if(!taskFunc) return;

	for(uint32_t i = 0; i < ArrayTail; i++)  // поиск задачи в текущем списке
	{
		if(task->pFunc == taskFunc)// если нашли, то обновляем переменные
		{
			_disableInterrupt();//Глобальное запрещение прерываний.
			task->Delay  = taskDelay;
			task->Period = taskPeriod;
			task->Run    = 0;
			_enableInterrupt();//Глобальное разрешение рерываний.
			return;
		}
		task++;
	}
	//Если такой задачи в списке нет и есть место,то добавляем в конец массива
	if(ArrayTail < MAX_TASKS)
	{
		_disableInterrupt();//Глобальное запрещение прерываний.
		task 		 = &TaskArray[ArrayTail];
		task->pFunc  = taskFunc;
		task->Delay  = taskDelay;
		task->Period = taskPeriod;
		task->Run    = 0;
		ArrayTail++;	   // увеличиваем "хвост"
		_enableInterrupt();//Глобальное разрешение рерываний.
	}
}
//**********************************************************
/* Удаление задачи из списка
 *
 */
void RTOS_DeleteTask(void(*taskFunc)(void)){

	for(uint32_t i=0; i < ArrayTail; i++) //проходим по списку задач
	{
		if(TaskArray[i].pFunc == taskFunc) // если задача в списке найдена
		{
			_disableInterrupt();  //Глобальное запрещение прерываний.
			if(i != (ArrayTail-1))//переносим последнюю задачу на место удаляемой
			{
				TaskArray[i] = TaskArray[ArrayTail - 1];
			}
			ArrayTail--;	   //уменьшаем указатель "хвоста"
			_enableInterrupt();//Глобальное разрешение рерываний.
			return;
		}
	}
}
//**********************************************************
/* Диспетчер РТОС, вызывается в main
 *
 */
void RTOS_DispatchLoop(void){

	void(*function)(void);
	volatile Task_t *task = &TaskArray[0];
	//-----------------------------
	//проходим по списку задач
	for(uint32_t i=0; i< ArrayTail; i++)
	{
		//если флаг на выполнение взведен,
		if(task->Run == 1)
		{
			function = task->pFunc;// запоминаем задачу, т.к. во время выполнения может измениться индекс
			if(task->Period == 0) RTOS_DeleteTask(task->pFunc);// если период равен 0
			else
			{
				task->Run = 0; 								  //иначе снимаем флаг запуска
				if(!task->Delay) task->Delay = task->Period-1;//если задача не изменила задержку задаем ее
															  //задача для себя может сделать паузу
			}
			//выполняем задачу
			(*function)();
		}
		task++;
	}
}
//**********************************************************
/* Таймерная служба РТОС (прерывание аппаратного таймера)
 *
 */
//ISR(RTOS_ISR)
//{
//   u08 i;
//
//   TIMER_COUNTER = TIMER_START;                       // задаем начальное значение таймера
//
//   for (i=0; i<arrayTail; i++)                        // проходим по списку задач
//   {
//      if  (TaskArray[i].delay == 0)                   // если время до выполнения истекло
//           TaskArray[i].run = 1;                      // взводим флаг запуска,
//      else TaskArray[i].delay--;                      // иначе уменьшаем время
//   }
//}

void RTOS_TimerServiceLoop(void){

	volatile Task_t *task = &TaskArray[0];
	//-----------------------------
	TickCount++;
	for(uint32_t i=0; i < ArrayTail; i++)//проходим по списку задач
	{
	    if	(task->Delay == 0) task->Run = 1;//если время до выполнения истекло взводим флаг запуска,
	    else task->Delay--;                  //иначе уменьшаем время
	    task++;
	}
}
//**********************************************************
/* Значение счетчика тиков RTOS.
 *
 */
uint32_t RTOS_GetTickCount(void){

	return TickCount;
}
//*******************************************************************************************
//*******************************************************************************************



