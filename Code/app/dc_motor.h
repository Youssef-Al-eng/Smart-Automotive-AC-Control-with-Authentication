/************************************************************************************
 * File Name    : DC_Motor.h
 * Description  : DC motor driver interface for controlling direction and speed.
 * Author       : Abdalah Mohamed  
 ************************************************************************************/
#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include <avr/io.h>
#include <stdint.h>
#include "PWM.h"

#define MOTOR_PIN1 PD2  // motor input 1 (INT1)
#define MOTOR_PIN2 PD3  // motor input 2 (INT2)
#define MOTOR_PORT PORTD  
#define MOTOR_DDR  DDRD

void motor_init(void);
void motor_forward(uint8_t speed);
void motor_backward(uint8_t speed);
void motor_stop(void);

#endif

//int1 >> PD2
//int2 >> PD3