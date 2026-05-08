/************************************************************************************
 * File Name    : PWM.c
 * Description  : PWM driver for controlling motor speed (Pin 44, Arduino Mega)
 * Author       : Youssef Alaa
 ************************************************************************************/

#include "PWM.h"
#include "macros.h"

/* Initialize PWM on Timer5, Channel C (PL5) */
void pwm_init(void)
{
    SET_BIT(PWM_DDR, PWM_PIN);             /* Set PWM pin as output */
    TCCR5A = (1 << WGM50) | (1 << COM5C1);/* Fast PWM 8-bit, enable output */
    TCCR5B = (1 << WGM52) | (1 << CS51) | (1 << CS50); /* Prescaler 64 */
    OCR5C = 0;                             /* Start at 0% duty */
}

/* Set PWM duty cycle (0-255) */
void pwm_set_duty(uint8_t duty)
{
    OCR5C = duty;                           /* Update duty cycle */
}
