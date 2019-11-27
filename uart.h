/* 
 * File:   timer.h
 * Author: gaoyingqiang
 *
 * Created on November 19, 2019, 13:02 PM
 */

#ifndef UART_H
#define	UART_H
#include "xc.h"
#include "uart.h"

uint16_t configUART2( uint16_t baud_rate, uint16_t fcy);
void send_A2Z( void );
void send_char( char c);
void mySendString( char * myString);
void adjust_LED4( void );
#endif	/* UART_H */
        
        
