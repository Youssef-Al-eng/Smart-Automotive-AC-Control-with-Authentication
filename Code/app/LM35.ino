/************************************************************************************
 * File Name    : LM35.c
 * Description  : LM35 temperature sensor driver
 * Author       : Youssef Alaa
 ************************************************************************************/
#include "LM35.h"
#include <avr/io.h>
/* Initialize ADC for LM35 */
void LM35_Init(void)
{
    ADMUX = (1 << REFS0);                     /* AVCC reference */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | 
             (1 << ADPS1) | (1 << ADPS0);     /* Enable ADC, prescaler 128 */
}

/* Read ADC value from a channel */
static uint16_t LM35_ReadADC(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);                    /* Start conversion */
    while (ADCSRA & (1 << ADSC));            /* Wait until done */
    return ADC;
}

/* Read temperature in Celsius */
float LM35_Read(void)
{
    uint16_t adc_val = LM35_ReadADC(0);      /* PF0 */
    return (adc_val * 3.0 / 1023.0) * 100.0;
}
