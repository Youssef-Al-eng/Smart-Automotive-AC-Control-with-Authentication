/************************************************************************************
 * File Name    : LM35.h
 * Description  : LM35 temperature sensor driver header
 * Author       : Youssef Alaa
 ************************************************************************************/

#ifndef LM35_H_
#define LM35_H_

#include <avr/io.h>
#include "std_types.h"
#include "macros.h"

void LM35_Init(void);
float_32 LM35_Read(void);  // Returns temperature in Celsius

#endif

// PF0