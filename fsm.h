/*
 * fsm.h
 *
 *  Created on: Mar 1, 2020
 *      Author: Khaled
 */

#ifndef FSM_H_
#define FSM_H_

typedef enum {
   IDLE_STATE,
   SEARCH_STATE,
   NAVIGATE_STATE,

   STATES_CNT
} fsm_state;


typedef enum {
   SINGLE_CLICK_EVENT,
   DOUBLE_CLICK_EVENT,
   LONG_CLICK_EVENT,
   MOUSE_MOVE_DONE_EVENT,

   EVENTS_CNT
} fsm_event;

typedef fsm_state ( *fsm_event_handle )( void );


void fsm_init( void );
void fsm_handle_event( fsm_event event );

#endif /* FSM_H_ */
