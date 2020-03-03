/*
 * fsm.c
 *
 *  Created on: Mar 1, 2020
 *      Author: Khaled
 */
#include <stddef.h>
#include "gpio.h"
#include "uart.h"
#include "fsm.h"
#include "mouse.h"

static fsm_state start_idle( void );
static fsm_state start_search( void );
static fsm_state continue_search( void );
static fsm_state start_navigate( void );
static fsm_state continue_navigate( void );

fsm_state curr_state = IDLE_STATE;

static fsm_event_handle  fsm_table[ STATES_CNT ][ EVENTS_CNT ] = {
    [ IDLE_STATE ] = {
                         [ SINGLE_CLICK_EVENT ]    = start_search,
                         [ DOUBLE_CLICK_EVENT ]    = start_navigate,
                         [ LONG_CLICK_EVENT   ]    = NULL,
                         [ MOUSE_MOVE_DONE_EVENT ] = NULL
    },
    [ SEARCH_STATE ] = {
                         [ SINGLE_CLICK_EVENT ]    = NULL,
                         [ DOUBLE_CLICK_EVENT ]    = start_navigate,
                         [ LONG_CLICK_EVENT   ]    = start_idle,
                         [ MOUSE_MOVE_DONE_EVENT ] = continue_search
    },
    [ NAVIGATE_STATE ] = {
                         [ SINGLE_CLICK_EVENT ]    = start_search,
                         [ DOUBLE_CLICK_EVENT ]    = NULL,
                         [ LONG_CLICK_EVENT   ]    = start_idle,
                         [ MOUSE_MOVE_DONE_EVENT ] = continue_navigate
    }
};

void fsm_init( void ) {

   curr_state = IDLE_STATE;
   start_idle();
}

void fsm_handle_event( fsm_event event ) {

   if( fsm_table[ curr_state ][ event ] != NULL ) {

      curr_state = (*fsm_table[ curr_state ][ event ])();
   }
}

static fsm_state start_idle( void ) {
   LOG( ">>>>>>> Move to IDLE state\n\r" );

   RGB( RED ); 
   return( IDLE_STATE );
}

static fsm_state start_search( void ) {
   LOG( ">>>>>>> Move to SEARCH state\n\r" );

   return( continue_search() );
}

static fsm_state continue_search( void ) {

   demo1_rect();

   RGB( GREEN );
   return( SEARCH_STATE );
}

static fsm_state start_navigate( void ) {
   LOG( ">>>>>>> Move to NAVIGATE state\n\r" );

   return( continue_navigate() );
}

static fsm_state continue_navigate( void ) {

   demo2_stair();

   RGB( BLUE );
   return( NAVIGATE_STATE );
}
