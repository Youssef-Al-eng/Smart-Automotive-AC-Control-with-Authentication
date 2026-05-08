/************************************************************************************
 * File Name    : LDR.c
 * Description  : LDR sensor driver
 * Author       : Amr Ahmed
 ************************************************************************************/

#include "LDR.h"

void LDR_Init(void)
{
    ADMUX = (1<<REFS0); // AVCC
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint_16 LDR_Read(void)
{
    ADMUX = (ADMUX & 0xF0) | 1; // ADC1 / PF1
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC));
    return ADC;
}

//PF1