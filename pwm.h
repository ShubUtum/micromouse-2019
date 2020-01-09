/* 
 * File:   pwm.h
 * Author: Khaled
 *
 * Created on November 3, 2019, 9:07 PM
 */

#ifndef PWM_H
#define	PWM_H


uint16_t init_pwm1( uint16_t duration_ms, uint16_t dc_perc );
uint16_t init_pwm2( uint16_t duration_ms, uint16_t dc_perc );

uint16_t pwm1_change_dc( uint16_t dc_perc );
uint16_t pwm2_change_dc( uint16_t dc_perc );

void pwm1_run( void );
void pwm2_run( void );

void pwm1_stop( void );
void pwm2_stop( void );

void pwm2_sin_modulation( void );
int isPWMStart(int channel);

#endif	/* PWM_H */

