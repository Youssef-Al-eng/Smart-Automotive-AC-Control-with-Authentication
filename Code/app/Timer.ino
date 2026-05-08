/************************************************************************************
 * Module: Timer
 * Description: 1ms system timer using Timer0
 * Author: Abdalah Mohamed 
 ************************************************************************************/
#include "Timer.h"
volatile uint32_t g_millis_counter = 0;
/* Timer0 Compare Match ISR (1ms tick) */
ISR(TIMER0_COMPA_vect)  {g_millis_counter++;}
/* Initialize Timer0 for 1ms tick */
void Timer_Init(void)
{    TCCR0A = (1 << WGM01);               /* CTC mode */
    TCCR0B = (1 << CS01) | (1 << CS00);  /* Prescaler 64 */
    OCR0A = 249;                          /* Compare value for 1ms */
    TIMSK0 |= (1 << OCIE0A);             /* Enable compare interrupt */
    sei();                                /* Enable global interrupts */
}

/* Get elapsed milliseconds */
uint32_t Timer_GetMillis(void)
{
    uint32_t millis_val;
    cli();                                /* Temporarily disable interrupts */
    millis_val = g_millis_counter;
    sei();                                /* Re-enable interrupts */
    return millis_val;
}
