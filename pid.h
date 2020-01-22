/*
 * pid.h
 *
 *  Created on: Dec 19, 2019
 *      Author: kmohsen
 */

#ifndef PID_H_
#define PID_H_


typedef struct pid_params_s {
   int16_t kf;                  // forward coeff         x100 unit to avoid floating operations
   int16_t kp;                  // Proportional coeff    x100 unit to avoid floating operations
   int16_t ki;                  // Integral coeff        x100 unit to avoid floating operations
   int16_t i_error;             // I accumulated error
   int16_t i_error_limit;       // I accumulated error limit
   int16_t min_ip;              // desired control param min value        (Motor velocity min value)
   int16_t max_ip;              // desired control param max value        (Motor velocity max value)
   int16_t min_op;              // Controller output signal min value     (PWM DC min value -100)
   int16_t max_op;              // Controller output signal max value     (PWM DC max value 100)
} pid_params;


void init_pid( pid_params *pid, float kp, float ki, int16_t min_ip, int16_t max_ip, int16_t min_op, int16_t max_op );
int16_t pid_control( pid_params *pid, uint16_t current_val, uint16_t desired_val );


#endif /* PID_H_ */
