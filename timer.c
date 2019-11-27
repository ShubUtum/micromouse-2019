
#include <xc.h>
#include "timer.h"
#include "gpio.h"
#include "uart.h"

static uint16_t _current_time;
static uint16_t _period_ms;
//uint8_t _id;

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

void interrupt_init( void ) {
    IEC1bits.U2RXIE = 0;  // receive interrupt request not enabled
    IFS1bits.U2RXIF = 0; // receive interrupt request has occurred
    IEC1bits.U2TXIE = 0; // transmit interrupt interrupt request enabled
}

void set_receive_priority( void) {
    IFS1bits.U2RXIF=1; // interrupt request occured
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt( void ) {
    IFS0bits.T1IF = 0; // reset Timer 1 interrupt flag
    
    _current_time += _period_ms;
    
    LED2 = ~LED2; // toggle led 2 (RB14)
    
    send_A2Z();
    
    char *mystring = "dispic33fJ64MC804";
    mySendString(mystring);
    
}
