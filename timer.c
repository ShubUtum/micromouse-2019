
#include <xc.h>
#include <stdio.h>
#include "timer.h"
#include "gpio.h"
#include "uart.h"
#include "pwm.h"
#include "pushButton.h"
#include "motor.h"
#include "mouse.h"

static void timer_10ms_tick_actions( void );
static void timer_50ms_tick_actions( void );
static void timer_100ms_tick_actions( void );


static uint16_t _current_time;
static uint16_t _period_ms;

uint16_t timer1_setup( uint16_t period_ms ) {
    //_id = 1;
    _period_ms = period_ms;
     
    T1CON = 0;            // Timer reset
    T1CONbits.TGATE = 0;  // Gated time accumulation disabled
    T1CONbits.TCS   = 0;  // internal clk
    TMR1 = 0;
    /*
     * prescaller  1 tick   1m            max period 2^16 ticks
     * 256         6.4us    156.25 ticks  419 ms
     * 64          1.6us    625 ticks     104 ms
     */
    if( period_ms < 105 ) {
        T1CONbits.TCKPS = 0b10; // 1:64 prescale
        PR1 = period_ms * 625 - 1;  // set Timer 1 period
    }
    else if( period_ms < 420 ) {
        T1CONbits.TCKPS = 0b11; // 1:256 prescale
        PR1 = period_ms * 156.25 - 1;  // set Timer 1 period
    }
    else {
        PR1 = 0;
        _period_ms = 0;
        IEC0bits.T1IE = 0;      // Disable Timer1 interrupt
        return 1;               // error
    }
    IFS0bits.T1IF = 0;      // Reset Timer1 interrupt flag
    IPC0bits.T1IP = 4;      // Timer1 Interrupt priority level=4
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupt
    T1CONbits.TON = 0; // leave timer disabled initially
    
    return 0;   // no errors
}

void timer1_start( void ) {
    _current_time = 0;
    T1CONbits.TON   = 1;  // start timer
}

void timer1_stop( void ) {
    _current_time = 0;
    T1CONbits.TON   = 0;  // stop timer
}

uint16_t get_current_time( void ) {
    return _current_time;
}

void reset_current_time( void ) {
    _current_time = 0;
}

void wait_ms( uint16_t wait_duration_ms ) {
    _current_time = 0;
    
    while( _current_time < wait_duration_ms );
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt( void ) {
    static int counter = 0;
    
    IFS0bits.T1IF = 0; // reset Timer 1 interrupt flag
    _current_time += _period_ms;
    
    //RED_LED = ~RED_LED; // toggle RED led
    
    counter++;
    /*****************************************************************************
     * DONOT ADD ANY CODE IN THIS FUNCTION !!!
     * CREATE A NEW FUNCTION AND CALL IT FROM ONE OF THE timer_xxms_tick_actions
     * ***************************************************************************/
    timer_10ms_tick_actions();
    
    if( counter %2 == 0  ) {
        // 20 ms tick
        // add timer_20ms_tick_actions() if there is any 20ms periodic actions required
    }
    if( counter %5 == 0  ) {
        // 50 ms tick
        timer_50ms_tick_actions();
    }
    if( counter %10 == 0 ) {
        // 100 ms tick
        timer_100ms_tick_actions();
    }
    
    // reset counter 
    if( counter == 1000 ) counter = 0;
}

static void timer_10ms_tick_actions( void ) {
    // pwm2_sin_modulation();
    //GREEN_LED = ~GREEN_LED; // toggle GREEN led
    
    //motor_calc_max_speed(); // Motor max speed calibration
    //test_motor_PI_control( 40 );
    calc_motors_speed();
    check_mouse_move();
}

static void timer_50ms_tick_actions( void ) {
    check_push_button(50);
}

static void timer_100ms_tick_actions( void ) {
    //GREEN_LED = ~GREEN_LED; // toggle GREEN led
    //PWM_RED_LED = ~PWM_RED_LED; // toggle RED led
    RED_LED = ~RED_LED; // toggle RED led
    //GREEN_LED = ~GREEN_LED; // toggle GREEN led

    //BACK_LED = ~BACK_LED;
    //FRONT_LED = ~FRONT_LED;
    //test_motor();
    
    //test_motor_PI_control( 40 );
    //send_A2Z();
}
