/*
 * pid.c
 *
 *  Created on: Dec 19, 2019
 *      Author: khaled
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pid.h"
#include "uart.h"


/************************************************************************
 * kp:      Proportional coeff    x100 unit to avoid floating operations
 * ki:      Integral coeff        x100 unit to avoid floating operations
 * min_ip:  desired control param min value        (Motor velocity min value)
 * max_ip:  desired control param max value        (Motor velocity max value)
 * min_op:  PID Controller output signal min value     (PWM DC min value -100)
 * max_op:  PID Controller output signal max value     (PWM DC max value 100)
 ************************************************************************/
void init_pid( pid_params *pid, float kp, float ki, int16_t min_ip, int16_t max_ip, int16_t min_op, int16_t max_op ) {

   memset( pid, 0, sizeof(*pid) );

   pid->kf              = (max_op * 15) / max_ip;     // forward coeff: x100 unit to avoid floating operations e.g. kf 2 will be represented as 200
   pid->kp              = (uint16_t) (kp*100);         // x100 and change to int e.g kp 1.23 will be represented as 123
   pid->ki              = (uint16_t) (ki*100);         // same3 as kp
   pid->i_error         = 0;
   pid->i_error_limit   = max_ip /*- min_ip*/;             // limit I accumulated error to the max input value range e.g motor speed range
   pid->min_op          = min_op;
   pid->max_op          = max_op;
   
   LOG("kf=%d, kp=%d, ki=%d, i_error_limit=%d\n\r", pid->kf, pid->kp, pid->ki, pid->i_error_limit);
}


/************************************************************************
 * Motor speed PI controller
 * current_val : current motor speed in num of POSCNT per 10ms
 * desired_val : desired motor speed in num of POSCNT per 10ms
 *
 * return: PWM DC percentage 100(full speed) : 0(stationary) : -100(full speed in reverse direction)
 ************************************************************************/
int16_t pid_control( pid_params *pid, uint16_t current_val, uint16_t desired_val ) {
   int16_t op;
   int16_t tempOp;
   int16_t error = desired_val - current_val;

   pid->i_error += error;
   if( pid->i_error > pid->i_error_limit )
      pid->i_error = pid->i_error_limit;

   /* calculate the output control signal value */
    op =  pid->kf * desired_val +      // F: forward amp
          pid->kp * error +            // P: proportional amp
          pid->ki * pid->i_error;      // I: integral amp

    
    op = op*0.01;
    //tempOp = op/100;
    //op = tempOp;
    
    /* limit the output signal to the output signal range */
    if( op > pid->max_op )    op = pid->max_op;
    if( op < pid->min_op )    op = pid->min_op;
    
    //LOG("error= %d, i_error= %d, DC= %d \n\r", error, pid->i_error, op);
    
    return op;
 }
