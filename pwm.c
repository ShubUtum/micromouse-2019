/* 
 * File:   pwm.c
 * Author: Khaled
 *
 * Created on November 3, 2019, 9:07 PM
 */

#include <xc.h>
#include <math.h>
#include "pwm.h"
#include "gpio.h"


static uint16_t _duration1_ms;
static uint16_t _dc1_perc;
static uint16_t _duration2_ms;
static uint16_t _dc2_perc;

uint16_t pwm1_setup( uint16_t duration_ms, uint16_t dc_perc )
{
    _duration1_ms   = duration_ms;
    _dc1_perc       = dc_perc;
    
    P1TCONbits.PTEN = 0;    // PWM Time Base Timer is off
    /*
    * prescaller  1 tick   1m            max period 2^15 ticks  dc 1%
    * 64          1.6us    625 ticks     52.4 ms                12.5 * duration
    * 16          0.4us    2500 ticks    13.1 ms                50  * duration
    * 4           0.1us    10000 ticks   3.28 ms                200 * duration
    * 1           25ns     40000 ticks   0.8 ms                 800 * duration
    */
    if( duration_ms == 0 || duration_ms > 52 ) {
        /* not supported, return error */
        _duration1_ms   = 0xFFFF;   // invalid value
        _dc1_perc       = 0xFFFF;   // invalid value
        error();
        return 1;
    }
    else if( duration_ms <= 13 ) {
        P1TCONbits.PTCKPS   = 0b10;   // 1:16 prescale
        P1TPERbits.PTPER    = duration_ms * 2500 - 1;
        P1DC1               = dc_perc * duration_ms * 50;
    } 
    else if( duration_ms <= 52 ) {
        P1TCONbits.PTCKPS   = 0b11;   // 1:64 prescale
        P1TPERbits.PTPER    = duration_ms * 625 - 1;
        P1DC1                = dc_perc * duration_ms * 12.5;
    }
    P1TCONbits.PTMOD    = 0b00;  // free running mode
    P1TCONbits.PTSIDL   = 0;     // PWM time base runs in CPU Idle mode
    P1TCONbits.PTOPS    = 0b0000;
    /* P1TMR ***********************************************************************/
    P1TMRbits.PTDIR     = 0;    // count UP
    P1TMRbits.PTMR      = 0x0000;    // reset Time based count value
    /* PWM1CON 1/2 *****************************************************************/
    PWM1CON1bits.PEN1H  = 1;    // enable  PWM1 output High pin
    PWM1CON1bits.PEN1L  = 0;    // disable PWM1 output Low pin
    PWM1CON1bits.PEN2H  = 0;    // disable PWM1 output High pin
    PWM1CON1bits.PEN2L  = 0;    // disable PWM1 output Low pin
    PWM1CON1bits.PEN3H  = 0;    // disable PWM1 output High pin
    PWM1CON1bits.PEN3L  = 0;    // disable PWM1 output Low pin
    PWM1CON2bits.IUE    = 1;    // enable immediate DC update
    
    P1TCONbits.PTEN     = 1;    // PWM Time Base Timer is on        
    return 0;
}

uint16_t pwm1_change_dc( uint16_t dc_perc ) {
    if( dc_perc > 100 ) dc_perc = 100;
    
    if( _duration1_ms <= 13 ) {
        P1DC1   = dc_perc * _duration1_ms * 50;
    } 
    else if( _duration1_ms <= 52 ) {
        P1DC1   = dc_perc * _duration1_ms * 12.5;
    }else {
        /* not supported, return error */
        _duration1_ms   = 0xFFFF;   // invalid value
        _dc1_perc       = 0xFFFF;   // invalid value
        return 1;
    }
    return 0;
}

void pwm1_run( void ) {
    P1TCONbits.PTEN     = 1;    // PWM Time Base Timer is on        
}

void pwm1_stop( void ) {
    P1TCONbits.PTEN     = 0;    // PWM Time Base Timer is off
}


uint16_t pwm2_setup( uint16_t duration_ms, uint16_t dc_perc )
{
    _duration2_ms   = duration_ms;
    _dc2_perc       = dc_perc;
    
    P2TCONbits.PTEN = 0;    // PWM Time Base Timer is off
    /*
    * prescaller  1 tick   1m            max period 2^15 ticks  dc 1%
    * 64          1.6us    625 ticks     52.4 ms                12.5 * duration
    * 16          0.4us    2500 ticks    13.1 ms                50  * duration
    * 4           0.1us    10000 ticks   3.28 ms                200 * duration
    * 1           25ns     40000 ticks   0.8 ms                 800 * duration
    */
    if( duration_ms == 0 || duration_ms > 52 ) {
        /* not supported, return error */
        _duration2_ms   = 0xFFFF;   // invalid value
        _dc2_perc       = 0xFFFF;   // invalid value
        error();
        return 1;
    }
    else if( duration_ms <= 13 ) {
        P2TCONbits.PTCKPS   = 0b10;   // 1:16 prescale
        P2TPERbits.PTPER    = duration_ms * 2500 - 1;
        P2DC1               = dc_perc * duration_ms * 50;
    } 
    else if( duration_ms <= 52 ) {
        P2TCONbits.PTCKPS   = 0b11;   // 1:64 prescale
        P2TPERbits.PTPER    = duration_ms * 625 - 1;
        P2DC1                = dc_perc * duration_ms * 12.5;
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

uint16_t pwm2_change_dc( uint16_t dc_perc ) {
    if( dc_perc > 100 ) dc_perc = 100;
    
    if( _duration2_ms <= 13 ) {
        P2DC1   = dc_perc * _duration2_ms * 50;
    } 
    else if( _duration2_ms <= 52 ) {
        P2DC1   = dc_perc * _duration2_ms * 12.5;
    }else {
        /* not supported, return error */
        _duration2_ms   = 0xFFFF;   // invalid value
        _dc2_perc       = 0xFFFF;   // invalid value
        return 1;
    }
    return 0;
}

void pwm2_run( void ) {
    P2TCONbits.PTEN     = 1;    // PWM Time Base Timer is on        
}

void pwm2_stop( void ) {
    P2TCONbits.PTEN     = 0;    // PWM Time Base Timer is off
}


int isPWMStart(int channel){
    if (channel == 1)
        return P1TCONbits.PTEN;
    else if (channel == 2)
        return P2TCONbits.PTEN;
    else
        return 0;
}

void pwm2_sin_modulation( void ) {
    static float t = 0.0;
    int dc;

    t += 0.01;      // 10ms
    
    //if(t > 2.0) t = 0;
    //dc = (uint16_t)  (t*0.1 * 50);
        
    dc=(int) ((sin(2*3.14*2*t)+1.0)*50);    // sin: -1:1 , +1: 0:2, *50: 0:100 = dc range
    
    pwm2_change_dc( dc +1);   
}
