/*
 * rtos.c
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
volatile static uint32_t arrayTail = 0;		  // "хвост" очереди
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

   arrayTail = 0;	// "хвост" в 0
}
//**********************************************************
/* Добавление задачи в список
 *
 */
void RTOS_SetTask(void(*taskFunc)(void), uint32_t taskDelay, uint32_t taskPeriod){
   
   if(!taskFunc) return;

   for(uint32_t i = 0; i < arrayTail; i++) // поиск задачи в текущем списке
   {
      if(TaskArray[i].pFunc == taskFunc)   // если нашли, то обновляем переменные
      {
		  _disableInterrupt();//Глобальное запрещение прерываний.

		 TaskArray[i].Delay  = taskDelay;
		 TaskArray[i].Period = taskPeriod;
		 TaskArray[i].Run    = 0;

		 _enableInterrupt();//Глобальное разрешение рерываний.
		 return;
      }
   }

   if(arrayTail < MAX_TASKS)	// если такой задачи в списке нет
   {                            // и есть место,то добавляем
	   _disableInterrupt();//Глобальное запрещение прерываний.

      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].Delay  = taskDelay;
      TaskArray[arrayTail].Period = taskPeriod;
      TaskArray[arrayTail].Run    = 0;

      arrayTail++;	     // увеличиваем "хвост"

      _enableInterrupt();//Глобальное разрешение рерываний.
   }
}
//**********************************************************
/* Удаление задачи из списка
 *
 */
void RTOS_DeleteTask(void(*taskFunc)(void)){

   for(uint32_t i=0; i<arrayTail; i++)   // проходим по списку задач
   {
      if(TaskArray[i].pFunc == taskFunc) // если задача в списке найдена
      {
    	 _disableInterrupt();//Глобальное запрещение прерываний.

         if(i != (arrayTail - 1))	// переносим последнюю задачу
         {                       	// на место удаляемой
            TaskArray[i] = TaskArray[arrayTail - 1];
         }
         arrayTail--;	// уменьшаем указатель "хвоста"

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

   for(uint32_t i=0; i<arrayTail; i++)                // проходим по списку задач
   {
      if (TaskArray[i].Run == 1)                      // если флаг на выполнение взведен,
      {                                               // запоминаем задачу, т.к. во
         function = TaskArray[i].pFunc;               // время выполнения может
                                                      // измениться индекс
         if(TaskArray[i].Period == 0)
         {                                            // если период равен 0
            RTOS_DeleteTask(TaskArray[i].pFunc);      // удаляем задачу из списка,
         }
         else
         {
            TaskArray[i].Run = 0;                     // иначе снимаем флаг запуска
            if(!TaskArray[i].Delay)                   // если задача не изменила задержку
            {                                         // задаем ее
               TaskArray[i].Delay = TaskArray[i].Period-1;
            }                                         // задача для себя может сделать паузу
         }
         (*function)();                               // выполняем задачу
      }
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

	TickCount++;
	for(uint32_t i=0; i<arrayTail; i++)                  // проходим по списку задач
	{
	    if(TaskArray[i].Delay == 0) TaskArray[i].Run = 1;// если время до выполнения истекло взводим флаг запуска,
	  else TaskArray[i].Delay--;                         // иначе уменьшаем время
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



