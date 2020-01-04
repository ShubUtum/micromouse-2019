/* 
 * File:   gpio.h
 * Author: a2-lenz
 *
 * Created on April 5, 2019, 11:31 AM
 */

#ifndef GPIO_H
#define	GPIO_H

#include "xc.h"

#define RED_LED     LATBbits.LATB6
#define GREEN_LED   LATBbits.LATB15
#define SW1         PORTAbits.RA4

#define CTRLH_INPUT1 LATBbits.LATB4
#define CTRLH_INPUT2 LATBbits.LATB5
    

void initIO();
void error();

#endif	/* GPIO_H */

