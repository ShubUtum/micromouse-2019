
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h>

uint16_t timer1_setup( uint16_t period_ms );

void timer1_start( void );

void timer1_stop( void );

uint16_t get_current_time( void );

void reset_current_time( void );

void wait_ms( uint16_t wait_duration_ms );

#endif	/* TIMER_H */

