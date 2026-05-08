/************************************************************************************
 * File Name    : MQ2.h
 * Description  : MQ2 gas sensor driver interface.
 * Author       : Youssef Alaa
 ************************************************************************************/

#ifndef MQ2_H_
#define MQ2_H_

#include <avr/io.h>
#include "std_types.h"
#include "macros.h"

void MQ2_Init(void);
uint_16 MQ2_Read(void);

#endif
