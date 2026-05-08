/************************************************************************************
 * File Name    : MQ2.c
 * Description  : MQ2 gas sensor ADC driver.
 * Author       : Youssef Alaa
 ************************************************************************************/
#include "MQ2.h"
void MQ2_Init(void){
    ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);  /* Enable ADC, prescaler 128 */
}

uint_16 MQ2_Read(void)
{
    ADMUX = (ADMUX & 0xF0) | 2;                          /* Select ADC2 (PF2) */
    ADCSRA |= (1 << ADSC);                               /* Start conversion */
    while (ADCSRA & (1 << ADSC));                        
    return ADC;                                    
}