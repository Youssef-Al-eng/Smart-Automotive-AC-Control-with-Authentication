/************************************************************************************
 * Module: LCD
 * File Name: LCD.c
 * Description: Source file for 16x2 LCD driver (4-bit mode)
 * Author: Youssef Alaa
 ************************************************************************************/

#include "LCD.h"
#include <util/delay.h>

/* Helper function to pulse the Enable pin (Internal use only) */
static void LCD_Pulse_Enable(void)
{
    SET_BIT(LCD_CTRL_PORT, LCD_EN);
    _delay_us(1);
    CLEAR_BIT(LCD_CTRL_PORT, LCD_EN);
    _delay_us(50);
}

void LCD_Init(void)
{
    /* 1. Configure Pins as Output */
    LCD_DATA_DIR |= 0xF0;           // Set PA4-PA7 as output
    SET_BIT(LCD_CTRL_DIR, LCD_RS);  // Set RS as output
    SET_BIT(LCD_CTRL_DIR, LCD_EN);  // Set EN as output

    _delay_ms(50); // Power-on delay

    /* 2. Initialization Sequence */
    CLEAR_BIT(LCD_CTRL_PORT, LCD_RS); // Command Mode

    // Sync sequence for 4-bit mode
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (0x30);
    LCD_Pulse_Enable();
    _delay_ms(5);

    LCD_Pulse_Enable();
    _delay_us(150);

    LCD_Pulse_Enable();
    
    // Switch to 4-bit mode
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (0x20);
    LCD_Pulse_Enable();

    /* 3. Configure LCD Settings */
    LCD_Command(LCD_CMD_FUNCTION_SET); 
    LCD_Command(LCD_CMD_DISPLAY_ON);   
    LCD_Command(LCD_CMD_CLEAR);        
    LCD_Command(LCD_CMD_ENTRY_MODE);   
}

void LCD_Command(uint8_t cmnd)
{
    CLEAR_BIT(LCD_CTRL_PORT, LCD_RS); // RS = 0 for Command

    // Send Upper Nibble
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd & 0xF0);
    LCD_Pulse_Enable();

    // Send Lower Nibble (shifted to match upper port bits)
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((cmnd << 4) & 0xF0);
    LCD_Pulse_Enable();

    _delay_ms(2);
}

void LCD_Char(uint8_t data)
{
    SET_BIT(LCD_CTRL_PORT, LCD_RS); // RS = 1 for Data

    // Send Upper Nibble
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data & 0xF0);
    LCD_Pulse_Enable();

    // Send Lower Nibble
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((data << 4) & 0xF0);
    LCD_Pulse_Enable();

    _delay_us(50);
}

void LCD_String(const char *str)
{
    while(*str)
    {
        LCD_Char((uint8_t)*str++);
    }
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr;
    
    // Calculate DDRAM address
    // Row 0 starts at 0x80, Row 1 starts at 0xC0
    if (row == 0)
    {
        addr = 0x80 + col;
    }
    else
    {
        addr = 0xC0 + col;
    }

    LCD_Command(addr);
}

void LCD_Clear(void)
{
    LCD_Command(LCD_CMD_CLEAR);
}

void LCD_ClearLine(uint8_t row)
{
    uint8_t i;
    LCD_SetCursor(row, 0);
    for(i = 0; i < 16; i++)
    {
        LCD_Char(' ');
    }
    LCD_SetCursor(row, 0);
}