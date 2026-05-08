/************************************************************************************
 * File Name    : Buttons.h
 * Description  : Header file for reading push buttons.
 * Author       : Abdalah Mohamed 
 ************************************************************************************/

#ifndef BUTTONS_H
#define BUTTONS_H

#include "std_types.h"
#include <avr/io.h>

#define BTN_DDR  DDRC
#define BTN_PIN  PINC
#define BTN_PORT PORTC

#define BTN_UP_PIN     PC3
#define BTN_DOWN_PIN   PC2
#define BTN_SELECT_PIN PC1
#define BTN_RESET_PIN  PC0

void buttons_init(void);
uint8_t read_button(void);

#endif
