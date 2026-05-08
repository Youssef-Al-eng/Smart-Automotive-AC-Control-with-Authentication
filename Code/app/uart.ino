/************************************************************************************
 * File Name    : uart.c
 * Description  : UART driver for AVR (TX/RX, baud, send/receive).
 * Author       : Abdalah Mohamed 
 ************************************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

// ---------------- UART DRIVER ----------------

void Uart_SetBaudRate(unsigned short BuadRate) {
    unsigned short UBBR = ((F_CPU / 16) / BuadRate) - 1;   /* Calculate UBRR value */
    UBRR0L = (UBBR & 0xFF);                                /* Load low byte */
    UBRR0H = ((UBBR >> 8) & 0xFF);                         /* Load high byte */
}

void Uart_Init(void) {
    Uart_SetBaudRate(9600);                                /* Set baud = 9600 */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);                  /* Enable TX & RX */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);                /* 8-bit data, 1 stop */
}

void Uart_SendChar(unsigned char DataByte) {
    while (!(UCSR0A & (1 << UDRE0)));                      /* Wait for empty buffer */
    UDR0 = DataByte;                                       /* Load data to transmit */
}

unsigned char Uart_IsDataAvailable(void) {
    return (UCSR0A & (1 << RXC0));                         /* Return 1 if data received */
}

unsigned char Uart_ReadData(void) {
    while (!(UCSR0A & (1 << RXC0)));                       /* Wait for available data */
    return UDR0;                                           /* Return received byte */
}

void Uart_SendString(char DataString[], unsigned char Size) {
    for (int i = 0; i < Size && DataString[i]; i++) {
        Uart_SendChar(DataString[i]);                      /* Send each char */
    }
}
