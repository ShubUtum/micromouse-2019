/* 
 * File:   pwm.h
 * Author: Khaled
 *
 * Created on November 3, 2019, 9:07 PM
 */

#ifndef PWM_H
#define	PWM_H


#include <xc.h>
#include "pwm.h"

enum MOVEMENT{LEFT, RIGHT, FORWARD, BACKWARD, BRAKE, STOP};

uint16_t pwm2_setup( uint16_t duration_ms, uint16_t dc_perc );
uint16_t pwm2_change_dc( uint16_t dc_perc );
void pwm2_run( void );
void pwm2_stop( void );

void motor_perform(enum MOVEMENT direction, int speed); 
int isPWMstart(int channel);


#endif	/* PWM_H */

