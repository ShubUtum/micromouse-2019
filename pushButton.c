/**
 * 
 * 
	a	b	   c    
R---|	|------+ 1 click
P   |---|

	a				e
R---|				|---- long press
P   |---------------|

	a	b	a	 d
R---|	|---|	 |--- 2 click
P   |---|   |----|

constants:
----------
DC_gap 		= t(b-c) = 350 ms
long_press 	= t(a-e) = 1000 ms
 * 
 */

#include <xc.h>
#include "uart.h"
#include "gpio.h"
#include "fsm.h"

#define DOUBLE_CLICK_MAX_GAP    350     // ms
#define LONG_PRESS              1000    //ms

static void single_click( void ){
    LOG( "Single click\n\r" );

    fsm_handle_event( SINGLE_CLICK_EVENT );
}

static void double_click( void ){
    LOG( "Double click\n\r" );

    fsm_handle_event( DOUBLE_CLICK_EVENT );
}

static void long_click( void ){
    LOG( "Long click\n\r" );

    fsm_handle_event( LONG_CLICK_EVENT );
}

void check_push_button( uint16_t time_tick ) {
    /*
        ----INPUT-------|OUTPUT
       Old	New	PT	RT	|?
       R	R		Y	|(c)if RT > DC_gap : "1 clk" & stop R Timer
       R	P	N		|(a)start P timer & stop R Timer
       P	P	Y		|
       P	R	Y	N	|stop P timer, 
                        |(e)if PT > 1s : "long press", 
                        |(d)elseif(track 2clk) : "2 clk", 
                        |(b)else : "1clk_or_2clk" & start R timer
    */    
    static uint16_t _SW1_last = 0;
    static uint16_t _press_time = 0;
    static uint16_t _release_time = 0;
    static uint16_t _check_double_click = 0;   // set to 1 after 1st click, used to check double click

    if( SW1 == 1 && _SW1_last == 0 ) {   // BUTTON PRESS
        _release_time = 0;
    }
    else if( SW1 == 0 && _SW1_last == 1 ) {   // BUTTON RELEASE
        if( _press_time >= LONG_PRESS ) {
            long_click();
        }
        else {  // normal press, can be 1st or 2nd click
            if( _check_double_click == 1 ) {   // 2nd button release
                double_click();
                _check_double_click = 0;
            }
            else {                      // 1st button release
                _check_double_click = 1; 
            }   
        }
        _press_time = 0;
    }
    else if( SW1 == 1 && _SW1_last == 1 ) {   // PRESS TIME
        _press_time += time_tick;
    }
    else if( SW1 == 0 && _SW1_last == 0 ) {   // RELEASE TIME
        _release_time += time_tick;
        
        if( _check_double_click == 1 && _release_time >= DOUBLE_CLICK_MAX_GAP ) {
            single_click();
            _check_double_click = 0;
        }
    }
    
    _SW1_last = SW1;
}
