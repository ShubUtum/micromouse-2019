/* 
 * File:   pwm.c
 * Author: Khaled
 *
 * Created on November 3, 2019, 9:07 PM
 */

#include <xc.h>
#include "pwm.h"


static uint16_t _duration_ms;
static uint16_t _dc_perc;

uint16_t pwm2_setup( uint16_t duration_ms, uint16_t dc_perc )
{
    _duration_ms   = duration_ms;
    _dc_perc       = dc_perc;
    
    P2TCONbits.PTEN = 0;    // PWM Time Base Timer is off
    /*
    * prescaller  1 tick   1m            max period 2^15 ticks  dc 1%
    * 64          1.6us    625 ticks     52.4 ms                12.5 * duration
    * 16          0.4us    2500 ticks    13.1 ms                50  * duration
    * 4           0.1us    10000 ticks   3.28 ms                200 * duration
    * 1           25ns     40000 ticks   0.8 ms                 800 * duration
    */
    if( duration_ms <= 13 ) {
        P2TCONbits.PTCKPS   = 0b10;   // 1:16 prescale
        P2TPERbits.PTPER    = duration_ms * 2500 - 1;
        P2DC1               = dc_perc * duration_ms * 50 - 1;
    } 
    else if( duration_ms <= 52 ) {
        P2TCONbits.PTCKPS   = 0b11;   // 1:64 prescale
        P2TPERbits.PTPER    = duration_ms * 625 - 1;
        P2DC1                = dc_perc * duration_ms * 12.5 - 1;
    }else {
        /* not supported, return error */
        _duration_ms   = 0xFFFF;   // invalid value
        _dc_perc       = 0xFFFF;   // invalid value
        return 1;
    }
    P2TCONbits.PTMOD    = 0b00;  // free running mode
    P2TCONbits.PTSIDL   = 0;     // PWM time base runs in CPU Idle mode
    P2TCONbits.PTOPS    = 0b0000;
    /* P2TMR ***********************************************************************/
    P2TMRbits.PTDIR     = 0;    // count UP
    P2TMRbits.PTMR      = 0x0000;    // reset Time based count value
    /* PWM2CON 1/2 *****************************************************************/
    PWM2CON1bits.PEN1H  = 1;    // enable PWM2 output High pin
    PWM2CON1bits.PEN1L  = 0;    // disable PWM2 output Low pin
    PWM2CON2bits.IUE    = 1;    // enable immediate DC update
    
    P2TCONbits.PTEN     = 1;    // PWM Time Base Timer is on        
    return 0;
}

uint16_t pwc2_change_dc( uint16_t dc_perc ) {
    if( _duration_ms <= 13 ) {
        P2DC1   = dc_perc * _duration_ms * 50 - 1;
    } 
    else if( _duration_ms <= 52 ) {
        P2DC1   = dc_perc * _duration_ms * 12.5 - 1;
    }else {
        /* not supported, return error */
        _duration_ms   = 0xFFFF;   // invalid value
        _dc_perc       = 0xFFFF;   // invalid value
        return 1;
    }
    return 0;
}

void pwc2_run( void ) {
    P2TCONbits.PTEN     = 1;    // PWM Time Base Timer is on        
}

void pwc2_stop( void ) {
    P2TCONbits.PTEN     = 0;    // PWM Time Base Timer is off
}