/************************************************************************************
 * File Name    : DC_Motor.c
 * Description  : DC motor driver for controlling direction and speed using PWM.
 * Author       : Abdalah Mohamed  
 ************************************************************************************/
#include "DC_Motor.h"
void motor_init(void) {
    MOTOR_DDR |= (1 << MOTOR_PIN1) | (1 << MOTOR_PIN2);  /* Set motor pins as output */
    motor_stop();                                        /* Ensure motor is stopped */
}

/* Run motor forward at specified speed */
void motor_forward(uint8_t speed) {
    SET_BIT(MOTOR_PORT, MOTOR_PIN1);                     /* Set forward pin HIGH */
    CLEAR_BIT(MOTOR_PORT, MOTOR_PIN2);                   /* Set backward pin LOW */
    pwm_set_duty(speed);                                 /* Apply PWM speed */
}

/* Run motor backward at specified speed */
void motor_backward(uint8_t speed) {
    CLEAR_BIT(MOTOR_PORT, MOTOR_PIN1);                   /* Set forward pin LOW */
    SET_BIT(MOTOR_PORT, MOTOR_PIN2);                     /* Set backward pin HIGH */
    pwm_set_duty(speed);                                
}

/* Stop motor */
void motor_stop(void) {
    CLEAR_BIT(MOTOR_PORT, MOTOR_PIN1);                   /* Set both pins LOW */
    CLEAR_BIT(MOTOR_PORT, MOTOR_PIN2);
    pwm_set_duty(0);                                    
}
