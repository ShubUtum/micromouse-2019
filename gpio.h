/* 
 * File:   gpio.h
 * Author: a2-lenz
 *
 * Created on April 5, 2019, 11:31 AM
 */

#ifndef GPIO_H
#define	GPIO_H

#include "xc.h"

#define LED1  LATBbits.LATB8
#define LED2  LATBbits.LATB14
#define SW1   PORTBbits.RB7
    

void initIO();
#endif	/* GPIO_H */

