#ifndef MOTOR_H
#define	MOTOR_H

#include "qei.h"

#define MOTOR_MAX_SPEED     52     // POSCNT/10ms

void init_left_motor( void );
void init_right_motor( void );

void calc_motors_speed( void );
void get_motors_moved_dist( int* LM_dist, int* RM_dist );

void left_motor_perform( int desired_speed );
void right_motor_perform( int desired_speed );

void test_motor( void );
void motor_calc_max_speed( void );
void test_motor_PI_control( uint16_t desired_speed );

#endif	/* MOTOR_H */

