/************************************************************************************
 * File Name    : macros.h
 * Description  : Common bit manipulation macros.
 * Author       : Youssef Alaa
 ************************************************************************************/

#ifndef MACROS_H_
#define MACROS_H_

#define SET_BIT(REG, BIT)       ((REG) |= (1U << (BIT)))        /* Set bit */
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(1U << (BIT)))       /* Clear bit */
#define TOGGLE_BIT(REG, BIT)    ((REG) ^= (1U << (BIT)))        /* Toggle bit */

#define BIT_IS_SET(REG, BIT)    ((REG) & (1U << (BIT)))         /* Check if bit = 1 */
#define BIT_IS_CLEAR(REG, BIT)  (!((REG) & (1U << (BIT))))      /* Check if bit = 0 */

#define ROR(REG, NUM)           ((REG) = ((REG) >> (NUM)) | ((REG) << (8 - (NUM))))  /* Rotate right */
#define ROL(REG, NUM)           ((REG) = ((REG) << (NUM)) | ((REG) >> (8 - (NUM))))  /* Rotate left */

#endif /* MACROS_H_ */
