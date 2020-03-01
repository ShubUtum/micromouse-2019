/* 
 * File:   uart.h
 * Author: gaoyingqiang
 *
 * Created on November 19, 2019, 13:02 PM
 */

#ifndef UART_H
#define	UART_H
#include <stdio.h> // for sprintf()
#include "xc.h"


#define MAX_LOG_STR_LEN    150
extern char log_str[MAX_LOG_STR_LEN];

#define LOG( log_msg... )             \
   sprintf(log_str, log_msg );     \
   mySendString(log_str);          \

uint16_t configUART2( float baud_rate, uint16_t fcy );

void send_A2Z( void );
void send_char( char c );
void mySendString( char* stringinp );
void adjust_LED4( void );



#endif	/* UART_H */
