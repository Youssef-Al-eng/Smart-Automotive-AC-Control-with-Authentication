/***********************************************************************************
 * File Name    : Buttons.c
 * Description  : Functions to initialize and read push buttons.
 * Author       : Abdalah Mohamed 
 ************************************************************************************/

#include "Buttons.h"

void buttons_init(void)
{
    // Set buttons as input
    BTN_DDR &= ~((1<<BTN_UP_PIN) | (1<<BTN_DOWN_PIN) | (1<<BTN_SELECT_PIN) | (1<<BTN_RESET_PIN));
    // Enable pull-ups
    BTN_PORT |= ((1<<BTN_UP_PIN) | (1<<BTN_DOWN_PIN) | (1<<BTN_SELECT_PIN) | (1<<BTN_RESET_PIN));
}

uint8_t read_button(void)
{
    if (!(BTN_PIN & (1<<BTN_UP_PIN))) return 1;      // UP
    if (!(BTN_PIN & (1<<BTN_DOWN_PIN))) return 2;    // DOWN
    if (!(BTN_PIN & (1<<BTN_SELECT_PIN))) return 3;  // SELECT
    if (!(BTN_PIN & (1<<BTN_RESET_PIN))) return 4;   // RESET
    return 0;                                        // NONE
}
