/************************************************************************************
 * File Name: std_types.h
 * Description: Standard data types and common definitions
 * Author: Youssef Alaa
 ************************************************************************************/

#ifndef STD_TYPES_H_
#define STD_TYPES_H_

#ifndef FALSE
#define FALSE       (0u)
#endif
#ifndef TRUE
#define TRUE        (1u)
#endif

#define LOGIC_HIGH  (1u)
#define LOGIC_LOW   (0u)
#define NULL_PTR    ((void*)0)

typedef unsigned char         uint8_t;       
typedef signed char           sint8_t;         
typedef unsigned short        uint_16;        
typedef signed short          sint_16;        
typedef unsigned long         uint_32;        
typedef signed long           sint_32;        
typedef unsigned long long    uint_64;         
typedef signed long long      sint_64;         
typedef float                 float_32;
typedef double                float_64;

#endif
