#ifndef MOTOR_H
#define	MOTOR_H

#include "pid.h"

enum MOVEMENT{LEFT, RIGHT, FORWARD, BACKWARD, BRAKE, STOP};

pid_params* init_motor_left( uint16_t max_speed );
pid_params* init_motor_right( uint16_t max_speed );
void test_motor( void );
void motor_calc_max_speed( void );
void test_motor_PI_control( uint16_t desired_speed );

#endif	/* MOTOR_H */

