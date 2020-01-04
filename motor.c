
#include <xc.h>
#include "gpio.h"
#include "qei.h"
#include "uart.h"
#include "pwm.h"
#include "motor.h"
#include <stdio.h>

// TODO: arbitrary value for now, to be measured and set correctly
#define MOTOR_MAX_SPEED     52     // POSCNT/10ms 
#define MAX_PMW_DC          100     // can be changed if Motor derived with higher voltage than specified
#define CNTR_P              (52 / MOTOR_MAX_SPEED)

/* Motor speed PI controller
 * current_speed : current motor speed in num of POSCNT per 10ms
 * desired_speed : desired motor speed in num of POSCNT per 10ms
 * 
 * return: PWM DC percentage 0(stationary) : 100(full speed) 
 */
static uint16_t motor_PI_control(uint16_t cur_dc, uint16_t current_speed, uint16_t desired_speed) {
    int16_t error = desired_speed - current_speed;
    static int16_t prev_error1 = 0;
    static int16_t prev_error2 = 0;
    static int16_t prev_error3 = 0;
    
    /* P part */
    cur_dc += CNTR_P * error ;
    
    /* TODO: I part */
    
    
    // update previous errors
    prev_error3 = prev_error2;
    prev_error2 = prev_error1;
    prev_error1 = error;
    
    return cur_dc;
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
    float velo = 5.50;
    //enum MOVEMENT dir;
    char mystring[40];
    
    velo = calc_velocity(100);
    sprintf(mystring, "%d\n\r", velo );
    mySendString(mystring);
    
    // increase DC 
    dc = (dc+10) % 100;
    
    motor_perform( FORWARD, dc );
    //motor_perform( BACKWARD, dc );
}

// calc the Motor max speed in POSCNT/10ms unit
void motor_calc_max_speed( void ) {
    static uint16_t first_call = 1;
    int16_t velocity;
    char mystring[20];
    
    if( first_call == 1 ) {
        // drive motor with high speed
        motor_perform( FORWARD, 100 );
        first_call = 0;
    }
    
    velocity = calc_velocity_in_poscnt();
    sprintf( mystring, "v = %d\n\r", velocity );
    mySendString( mystring );
}
 
void test_motor_PI_control( uint16_t desired_speed ) {
    static uint16_t curr_dc = 0;
    char mystring[40];
    int16_t current_speed;
    
    current_speed = calc_velocity_in_poscnt();
        
    if( current_speed - desired_speed != 0 )
    {
        sprintf( mystring, "dc = %d, v = %d\n\r", curr_dc, current_speed );
        mySendString( mystring );

        // calc DC 
        curr_dc = motor_PI_control( curr_dc, current_speed, desired_speed);
    }
    motor_perform( FORWARD, curr_dc );
    //motor_perform( BACKWARD, curr_dc );
}
