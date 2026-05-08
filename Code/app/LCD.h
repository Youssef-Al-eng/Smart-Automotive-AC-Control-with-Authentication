/************************************************************************************
 * Module: LCD
 * File Name: LCD.h
 * Description: Header file for 16x2 LCD driver (4-bit mode)
 * Author: Youssef Alaa
 ************************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include "std_types.h"
#include "macros.h"

/*******************************************************************************
 * Definitions                                  *
 *******************************************************************************/

/* LCD Data pins (Upper Nibble of PORTA: PA4 - PA7) */
#define LCD_DATA_PORT   PORTC
#define LCD_DATA_DIR    DDRC

/* LCD Control pins (PH5, PH6) */
#define LCD_CTRL_PORT   PORTH
#define LCD_CTRL_DIR    DDRH
#define LCD_RS          PH5
#define LCD_EN          PH6

/* LCD Commands */
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_HOME            0x02
#define LCD_CMD_ENTRY_MODE      0x06
#define LCD_CMD_DISPLAY_ON      0x0C
#define LCD_CMD_FUNCTION_SET    0x28

/*******************************************************************************
 * Function Prototypes                              *
 *******************************************************************************/

/* Initialize LCD */
void LCD_Init(void);

/* Send command to LCD */
void LCD_Command(uint8_t cmnd);

/* Display a single character */
void LCD_Char(uint8_t data);

/* Print a full string */
void LCD_String(const char *str);

/* Display text at a specific row and column */
void LCD_SetCursor(uint8_t row, uint8_t col);

/* Clear Screen */
void LCD_Clear(void);

/* Clear one line (0 or 1) */
void LCD_ClearLine(uint8_t row);

#endif /* LCD_H_ */