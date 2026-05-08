/************************************************************************************
 * File Name    : PWM.h
 * Description  : PWM driver header for controlling motor speed
 * Author       : Youssef Alaa
 ************************************************************************************/

#ifndef PWM_H
#define PWM_H

#include "std_types.h"
#include <avr/io.h>

// FORCE Pin 44 (PL5) for Arduino Mega
#define PWM_DDR  DDRL
#define PWM_PORT PORTL
#define PWM_PIN  PL5 

void pwm_init(void);
void pwm_set_duty(uint8_t duty);

#endif