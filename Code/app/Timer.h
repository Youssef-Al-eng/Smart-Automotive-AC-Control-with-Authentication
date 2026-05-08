/************************************************************************************
 * File Name    : Timer.h
 * Description  : System tick timer using Timer0 (1ms resolution).
 * Author       : Abdalah Mohamed 
 ************************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_
#include "std_types.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Initialize Timer0 for 1ms system tick */
void Timer_Init(void);
/* Get current milliseconds (replacement for millis()) */
uint32_t Timer_GetMillis(void);

#endif 