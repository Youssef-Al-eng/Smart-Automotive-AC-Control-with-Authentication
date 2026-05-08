
/************************************************************************************
 * File Name    : LDR.h
 * Description  : LDR sensor driver header
 * Author       : Amr Ahmed
 ************************************************************************************/
// LDR.h
#ifndef LDR_H_
#define LDR_H_

#include <avr/io.h>
#include "std_types.h"
#include "macros.h"

void LDR_Init(void);
uint_16 LDR_Read(void); // ADC1

#endif
