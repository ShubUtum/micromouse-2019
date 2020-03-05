/* 
 * File:   gpio.h
 * Author: a2-lenz
 *
 * Created on April 5, 2019, 11:31 AM
 */

#ifndef GPIO_H
#define	GPIO_H

#include "xc.h"
#include <stdio.h>

#define IN_BREAD_BOARD_MODE  0


#if IN_BREAD_BOARD_MODE

#define PWM_RED_LED LATBbits.LATB8
#define RED_LED     LATBbits.LATB6
#define GREEN_LED   LATBbits.LATB15
#define SW1         PORTAbits.RA4

#define CTRLH_INPUT1 LATBbits.LATB4
#define CTRLH_INPUT2 LATBbits.LATB5
#else

#define BACK_LED     LATBbits.LATB4
#define FRONT_LED    LATAbits.LATA8

#define RED_LED     LATBbits.LATB15
#define GREEN_LED   LATCbits.LATC7

#define SW1         PORTBbits.RB9

#define LM_DIR1 LATCbits.LATC8
#define LM_DIR2 LATCbits.LATC9
#define RM_DIR1 LATCbits.LATC1
#define RM_DIR2 LATCbits.LATC0

typedef enum {
   ALL_OFF  = 0x00,
   RED      = 0x01,  // BIT 0
   GREEN    = 0x02,  // BIT 1
   BLUE     = 0x04   // BIT 2
} rgb_color;

#endif    

void initIO();

void error();

void RGB( uint16_t color);

#endif	/* GPIO_H */

