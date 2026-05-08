/************************************************************************************
 * File Name    : display.c
 * Description  : display functions for showing temperature, limits, 
 *                and custom messages on the LCD.
 * Author       : Amr Mohamed 
 ************************************************************************************/

#include "Display.h"
#include <stdio.h>
#include <string.h>

/* Global Variables Definition */
float last_temp = 25.0f;
float low_limit  = 20.0f;
float high_limit = 30.0f;
char buffer[32];

void update_temp_display(void) {
    LCD_ClearLine(0);
    snprintf(buffer, sizeof(buffer), "Temp: %.1fC", last_temp);
    LCD_String(buffer);
}

void update_limits_display(void) {
    LCD_ClearLine(1);
    if(last_temp <= 20) LCD_String("Speed: 0      ");
    else if(last_temp <= 25) LCD_String("Speed: 64     ");
    else if(last_temp <= 30) LCD_String("Speed: 128    ");
    else if(last_temp <= 40) LCD_String("Speed: 192    ");
    else LCD_String("Motor STOP    ");
}

void display_message(const char* msg, uint8_t line) {
    LCD_ClearLine(line);
    LCD_String(msg);
}