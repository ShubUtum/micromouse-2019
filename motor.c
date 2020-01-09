
#include <xc.h>
#include <stdio.h>
#include "gpio.h"
#include "qei.h"
#include "uart.h"
#include "pwm.h"
#include "pid.h"
#include "motor.h"


#define MOTOR_MAX_SPEED     52     // POSCNT/10ms

#define GET_LM_SPEED       qei1_get_poscnt_speed()
#define GET_RM_SPEED       qei2_get_poscnt_speed()
/************************************************************************
 ************************************************************************/
pid_params* init_motor_left( uint16_t max_speed )
{
   // base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
   init_QEI_1(16, 33, 4, 0);
   init_pwm1( 1, 0 );   // 1KHz PWM (1ms period)

   pid_params *pid_motor_l = init_pid( 0.5,                       // kp
                                       0.1,                       // ki
                                       -max_speed, max_speed,     // Motor speed range (PID ip range)
                                       -100, 100                  // PWM DC range (PID op range)
                                       );

   LOG("Left Motor ready\n\r");

   return pid_motor_l;
}

/************************************************************************
 ************************************************************************/
pid_params* init_motor_right( uint16_t max_speed )
{
   // base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
   init_QEI_2(16, 33, 4, 0);
   init_pwm2( 1, 0 );   // 1KHz PWM (1ms period)

   pid_params *pid_motor_r = init_pid( 0.5,                       // kp
                                       0.1,                       // ki
                                       -max_speed, max_speed,     // Motor speed range (PID ip range)
                                       -100, 100                  // PWM DC range (PID op range)
                                       );

   LOG("Right Motor ready\n\r");

   return pid_motor_r;
}


//motor control
static void motor_perform(enum MOVEMENT direction, int speed){
    //H bridge control
    if (!isPWMStart(2))
        pwm2_run();
    
    switch(direction){
        case LEFT: break;
        case RIGHT: break;
        case FORWARD: 
            CTRLH_INPUT1 = 1;
            CTRLH_INPUT2 = 0;
            pwm2_change_dc(speed);
            break;
        case BACKWARD:
            CTRLH_INPUT1 = 0;
            CTRLH_INPUT2 = 1;
            pwm2_change_dc(speed);
            break;
        default:
            CTRLH_INPUT1 = 1;
            CTRLH_INPUT2 = 1;
            return;
    }
}

void test_motor( void ){
    static int dc = 0;
    int velo = 0;
    //enum MOVEMENT dir;

    velo = GET_LM_SPEED;
    LOG( "%d\n\r", velo );

    // increase DC 
    dc = (dc+10) % 100;
    
    motor_perform( FORWARD, dc );
    //motor_perform( BACKWARD, dc );
}

// calc the Motor max speed in POSCNT/10ms unit
void motor_calc_max_speed( void ) {
    static pid_params* LM_pid = NULL;
    static pid_params* RM_pid = NULL;

    if( LM_pid == NULL ) {    // first_call
       LM_pid = init_motor_left( MOTOR_MAX_SPEED );
        // drive motor with high speed
        motor_perform( FORWARD, 100 );
    }
    if( RM_pid == NULL ) {    // first_call
       RM_pid = init_motor_right( MOTOR_MAX_SPEED );
        // drive motor with high speed
        //TODO: Right Motor motor_perform( FORWARD, 100 );
    }
    
    LOG( "v = %d\n\r", GET_LM_SPEED );
}
 
void test_motor_PI_control( uint16_t desired_speed ) {
    uint16_t motor_pwm_dc = 0;
    static pid_params* LM_pid = NULL;
    static uint16_t time_idx = 0;
    int16_t current_speed;

    if( LM_pid == NULL ) {    // first_call
       LM_pid = init_motor_left( MOTOR_MAX_SPEED );
    }


    current_speed = GET_LM_SPEED;
        
    // calc DC
    motor_pwm_dc = pid_control( LM_pid, current_speed, desired_speed);

    motor_perform( FORWARD, motor_pwm_dc );
    //motor_perform( BACKWARD, motor_pwm_dc );

    if( time_idx < 200 ) {  // 200 = 2 Seconds
       LOG( "%d: v = %2d, dc = %3d\n\r", time_idx, current_speed, motor_pwm_dc );
       time_idx++;
    }
}
