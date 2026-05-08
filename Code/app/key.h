/************************************************************************************
 * File Name    : key.h
 * Description  : 4x3 keypad driver header file.
 * Author       : Amr Mohamed 
 ************************************************************************************/
#ifndef KEY_H_
#define KEY_H_
#include <avr/io.h>
#include "std_types.h"
#include "macros.h"

/* Rows → PL3, PL2, PL1, PL0 (Arduino pins 46–49) */
#define KEYPAD_ROW_PORT   PORTL
#define KEYPAD_ROW_PIN    PINL
#define KEYPAD_ROW_DIR    DDRL
/* Columns → PB3, PB2, PB1 (Arduino pins 50–52) */
#define KEYPAD_COL_PORT   PORTB
#define KEYPAD_COL_PIN    PINB
#define KEYPAD_COL_DIR    DDRB
#define ROW_NUM 4               /* Number of rows */
#define COL_NUM 3               /* Number of columns */

/* ---------------- Function Prototypes ---------------- */

void KEYPAD_Init(void);         /* Initialize keypad pins */
char KEYPAD_GetKey(void);       /* Read pressed key */

#endif /* KEY_H_ */