/************************************************************************************
 * File Name    : key.c
 * Description  : 4x3 keypad driver for reading pressed keys.
 * Author       : Amr Mohamed 
 ************************************************************************************/
#include "key.h"
#include <util/delay.h>
/* Keypad layout: rows x columns */
static const char KEYPAD_Map[ROW_NUM][COL_NUM] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

/* init keypad pins */
void KEYPAD_Init(void)
{
    KEYPAD_ROW_DIR |= 0x0F;                                     /* Set all row pins as outputs */
    KEYPAD_COL_DIR &= ~((1<<PB1)|(1<<PB2)|(1<<PB3));            /* Set column pins as inputs */
    KEYPAD_COL_PORT |= ((1<<PB1)|(1<<PB2)|(1<<PB3));            /* Enable pull-up resistors on columns */
    KEYPAD_ROW_PORT |= 0x0F;                                   
}

/* get key currently pressed */
char KEYPAD_GetKey(void)
{
    for (uint8_t row = 0; row < ROW_NUM; row++)
    {
        KEYPAD_ROW_PORT |= 0x0F;                                /* Reset all rows to HIGH */
        CLEAR_BIT(KEYPAD_ROW_PORT, (3 - row));                  /* Activate one row at a time */
        _delay_us(5);                                         
        
        if (BIT_IS_CLEAR(KEYPAD_COL_PIN, PB3)) {                /* Check first column */
            _delay_ms(20);                                      /* Debounce delay */
            while (BIT_IS_CLEAR(KEYPAD_COL_PIN, PB3));          /* Wait until key is released */
            return KEYPAD_Map[row][0];                         
        }

        if (BIT_IS_CLEAR(KEYPAD_COL_PIN, PB2)) {                /* Check second column */
            _delay_ms(20);
            while (BIT_IS_CLEAR(KEYPAD_COL_PIN, PB2));
            return KEYPAD_Map[row][1];
        }

        if (BIT_IS_CLEAR(KEYPAD_COL_PIN, PB1)) {                /* Check third column */
            _delay_ms(20);
            while (BIT_IS_CLEAR(KEYPAD_COL_PIN, PB1));
            return KEYPAD_Map[row][2];
        }
    }

    return 0;                                                   /* retyrn 0 if no key is pressed */
}
