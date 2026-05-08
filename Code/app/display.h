/************************************************************************************
 * File Name    : display.h
 * Description  : Functions and variables for handling LCD display updates.
 * Author       : Amr Mohamed 
 ************************************************************************************/

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "LCD.h"

// Temperature variables (global)
extern float last_temp;
extern float low_limit;
extern float high_limit;

// Display buffer
extern char buffer[32];

// Display functions
void update_temp_display(void);
void update_limits_display(void);
void display_message(const char* msg, uint8_t line);

#endif
