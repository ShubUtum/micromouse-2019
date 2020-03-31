
#include <xc.h>
#include <stdio.h>
#include "gpio.h"
#include "qei.h"
#include "uart.h"
#include "pwm.h"
#include "pid.h"
#include "motor.h"


#define GET_LM_SPEED       qei1_get_poscnt()
#define GET_RM_SPEED       qei2_get_poscnt()

#define MOTOR_MAX_PWM_DC    (100*6/9) // 100% * 6v/9v

int         LM_curr_speed;  // LM moved POSCNT/10ms
int         RM_curr_speed;  // RM moved POSCNT/10ms
pid_params  LM_pid;
pid_params  RM_pid;
/************************************************************************
 ************************************************************************/
void init_left_motor( void )
{   
   // base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
   init_QEI_1(16, 33, 4, 0);
   init_pwm2( 1, 0 );   // 1KHz PWM (1ms period)

   init_pid( &LM_pid,
             0.7,                                   // kp
             0.05,                                  // ki
             -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED,     // Motor speed range (PID ip range)
             -MOTOR_MAX_PWM_DC, MOTOR_MAX_PWM_DC    // PWM DC range (PID op range)
           );

   LM_curr_speed = GET_LM_SPEED;
   //LOG("Left Motor ready\n\r");
}

/************************************************************************
 ************************************************************************/
void init_right_motor( void )
{
   // base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
   init_QEI_2(16, 33, 4, 0);
   init_pwm1( 1, 0 );   // 1KHz PWM (1ms period)

   init_pid( &RM_pid,
             0.7,                                   // kp
             0.05,                                  // ki
             -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED,     // Motor speed range (PID ip range)
             -MOTOR_MAX_PWM_DC, MOTOR_MAX_PWM_DC    // PWM DC range (PID op range)
           );

   RM_curr_speed = GET_RM_SPEED;
   //LOG("Right Motor ready\n\r");
}

void calc_motors_speed( void ) {

   LM_curr_speed = GET_LM_SPEED;
   RM_curr_speed = GET_RM_SPEED;
}

void get_motors_moved_dist( int* LM_dist, int* RM_dist ) {

   *LM_dist = LM_curr_speed;
   *RM_dist = RM_curr_speed;
}

void left_motor_perform( int desired_speed ){
    int pwm_dc;
    
    //if( desired_speed > MOTOR_MAX_SPEED ) desired_speed = MOTOR_MAX_SPEED;
    
    pwm_dc = pid_control( &LM_pid, LM_curr_speed, desired_speed );

    //H bridge control
    if( desired_speed == 0 || pwm_dc == 0 ) {         // brake
        LM_DIR1 = 0;
        LM_DIR2 = 0;
        pwm2_change_dc( 0 );
    }else if( pwm_dc > 0 ) {    // Forward
        LM_DIR1 = 1;
        LM_DIR2 = 0;
        pwm2_change_dc( pwm_dc );
    }
    else {                      // Backward
        LM_DIR1 = 0;
        LM_DIR2 = 1;
        //pwm_dc = -1*pwm_dc;
        //LOG( "pwm2 DC = %d\n\r", -pwm_dc );
        pwm2_change_dc( (-pwm_dc) );
    }
}

void right_motor_perform( int desired_speed ){    
    int pwm_dc;
    
    //if( desired_speed > MOTOR_MAX_SPEED ) desired_speed = MOTOR_MAX_SPEED;
    
    pwm_dc = pid_control( &RM_pid, RM_curr_speed, desired_speed );

    //H bridge control
    if( desired_speed == 0 || pwm_dc == 0 ) {         // brake
        RM_DIR1 = 0;
        RM_DIR2 = 0;
        pwm1_change_dc( 0 );
    }else if( pwm_dc > 0 ) { // Forward
        RM_DIR1 = 1;
        RM_DIR2 = 0;
        pwm1_change_dc( pwm_dc );
    }
    else {                  // Backward
        RM_DIR1 = 0;
        RM_DIR2 = 1;
        pwm1_change_dc( -pwm_dc );
    }
}

// calc the Motor max speed in POSCNT/10ms unit
void motor_calc_max_speed( void ) {
    static uint16_t time_idx = 0;

    if( time_idx == 0 ) {    // first_call
       init_left_motor();
       init_right_motor();
       // drive motor with high speed
       LM_DIR1 = 1; LM_DIR2 = 0; pwm2_change_dc( MOTOR_MAX_PWM_DC );
       RM_DIR1 = 1; RM_DIR2 = 0; pwm1_change_dc( MOTOR_MAX_PWM_DC );
    }
    
    if( time_idx < 200 ) {  // 200 = 2 Seconds
        LOG( "%d: LM_v = %d\t\t RM_v = %d\n\r", time_idx, LM_curr_speed, RM_curr_speed );
        time_idx++;
    }
}
 
void test_motor_PI_control( uint16_t desired_speed ) {
    uint16_t motor_pwm_dc = 0;
    static uint16_t time_idx = 0;
    int16_t current_speed;

    if( time_idx == 0 ) {    // first_call
       init_left_motor();
       init_right_motor();
    }
    
    if( time_idx < 200 ) {  // 200 = 2 Seconds
        current_speed = LM_curr_speed;
        motor_pwm_dc = pid_control( &LM_pid, current_speed, desired_speed);
        LM_DIR1 = 1; LM_DIR2 = 0; pwm2_change_dc( motor_pwm_dc );
        LOG( "%d: LM = %d, %d\t-\t", time_idx, current_speed, motor_pwm_dc );
        
        current_speed = RM_curr_speed;
        motor_pwm_dc = pid_control( &RM_pid, current_speed, desired_speed);
        RM_DIR1 = 1; RM_DIR2 = 0; pwm1_change_dc( motor_pwm_dc );
        LOG( "RM = %d, %d\n\r", current_speed, motor_pwm_dc );
       
       time_idx++;
    }
}

void test_motor( void ){
    static int speed = MOTOR_MAX_SPEED;
    LOG( "LM_V=%d \t\t RM_V=%d\n\r", LM_curr_speed, RM_curr_speed );

    left_motor_perform( speed );
    right_motor_perform( (-1*speed) );

    // increase DC 
    //speed = (speed+10) % MOTOR_MAX_SPEED;
}
