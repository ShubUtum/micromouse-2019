
#include <xc.h>
#include <stdio.h>
#include "gpio.h"
#include "uart.h"
#include "motor.h"
#include "mouse.h"
#include "fsm.h"
#include "coordinate.h"
#include "mazeHandler.h"

#define MOTOR_SPEED     (MOTOR_MAX_SPEED-20)

#define CELL_POSCNT_DIST        2000     // one cell (16 cm) distance in POSCNT unit
#define ROTATE_POSCNT_DIST      2000      // distance of each wheel in POSCNT unit when mouse rotate

int               desired_LM_speed;
int               desired_RM_speed;
int               desired_poscnt_dist;
enum MOVEMENT     curr_move;

mouse_action actionsBuffer[10] = { NULL };

static void mouse_move_done( void );
static uint16_t get_mouse_moved_dist( void );

void mouse_init( void ) {
    init_left_motor();
    init_right_motor();
    
    // init sensors

    desired_LM_speed     = 0;
    desired_RM_speed     = 0;
    desired_poscnt_dist  = 0;
    curr_move   = BRAKE;
}

void mouse_brake( void ) {
    LOG( "-- Brake --\n\r" );
    desired_LM_speed     = 0;
    desired_RM_speed     = 0;
    desired_poscnt_dist  = 0;
    curr_move   = BRAKE;

    BACK_LED = 0;
    FRONT_LED = 0;

    left_motor_perform( desired_LM_speed );
    right_motor_perform( desired_RM_speed );
}

void mouse_move_fw( void ) {
    LOG( "-- FW --\n\r" );
    desired_LM_speed     = MOTOR_SPEED;
    desired_RM_speed     = MOTOR_SPEED;
    desired_poscnt_dist  = CELL_POSCNT_DIST;
    curr_move   = FORWARD;
    
    BACK_LED = 0;
    FRONT_LED = 1;

    left_motor_perform( desired_LM_speed );
    right_motor_perform( desired_RM_speed );
    
    switch (currentDirection)
	{
	case NORTH: currentMazeLocation.y++; break;
	case EAST: currentMazeLocation.x++; break;
	case SOUTH: currentMazeLocation.y--; break;
	case WEST: currentMazeLocation.x--; break;
	}
}

void mouse_move_bw( void ) {
    LOG( "-- BW --\n\r" );
    desired_LM_speed     = -MOTOR_SPEED;
    desired_RM_speed     = -MOTOR_SPEED;
    desired_poscnt_dist  = CELL_POSCNT_DIST;
    curr_move   = BACKWARD;
    
    BACK_LED = 1;
    FRONT_LED = 0;

    left_motor_perform( desired_LM_speed );
    right_motor_perform( desired_RM_speed );
    
    switch (currentDirection)
	{
	case NORTH: currentMazeLocation.y--; break;
	case EAST: currentMazeLocation.x--; break;
	case SOUTH: currentMazeLocation.y++; break;
	case WEST: currentMazeLocation.x++; break;
	}
}

void mouse_rotate_left( void ) {
    desired_LM_speed     = -MOTOR_SPEED;
    desired_RM_speed     = MOTOR_SPEED;
    desired_poscnt_dist  = ROTATE_POSCNT_DIST;
    curr_move   = LEFT;
    LOG( "-- RL --, LM=%d, RM=%d\n\r", desired_LM_speed, desired_RM_speed );
    
    BACK_LED = 1;
    FRONT_LED = 1;

    left_motor_perform( desired_LM_speed );
    right_motor_perform( desired_RM_speed );
    
    currentDirection = ((currentDirection >> 1) | (currentDirection << (4 - 1))) & 0b1111;
}

void mouse_rotate_right( void ) {
    desired_LM_speed     = MOTOR_SPEED;
    desired_RM_speed     = -MOTOR_SPEED;
    desired_poscnt_dist  = ROTATE_POSCNT_DIST;
    curr_move   = RIGHT;
    LOG( "-- RR --, LM=%d, RM=%d\n\r", desired_LM_speed, desired_RM_speed );

    BACK_LED = 1;
    FRONT_LED = 1;
    
    left_motor_perform( desired_LM_speed );
    right_motor_perform( desired_RM_speed );
    
    currentDirection = ((currentDirection << 1) | (currentDirection >> (4 - 1))) & 0b1111;
}

void check_mouse_move( void ) {
    
   if( curr_move != BRAKE ) {

      desired_poscnt_dist -= get_mouse_moved_dist();

      if( desired_poscnt_dist <= 0 ) {
         // movement done
         desired_poscnt_dist = 0;

         mouse_move_done();
      }
      else {
         // TODO: adjust speed based on the sensors readings
         left_motor_perform( desired_LM_speed );
         right_motor_perform( desired_RM_speed );
      }
   }
}

// return positive moved distance regardless of the direction
static uint16_t get_mouse_moved_dist( void ) {
    int   LM_dist, RM_dist;

    get_motors_moved_dist( &LM_dist, &RM_dist );

    switch( curr_move ){
        case LEFT:
            LOG( "RL:\t(%d,\t%d)\n\r", LM_dist, RM_dist );
            return( RM_dist );
            break;

        case RIGHT:
            LOG( "RR:\t(%d,\t%d)\n\r", LM_dist, RM_dist );
            return( LM_dist );
            break;

        case FORWARD:
           LOG( "FW:\t(%d,\t%d)\n\r", LM_dist, RM_dist );
            return( (LM_dist + RM_dist) >> 1 );
            break;

        case BACKWARD:
           LOG( "BW:\t(%d,\t%d)\n\r", LM_dist, RM_dist );
            return( (-1*LM_dist - RM_dist) >> 1 );
            break;

        default:
            return( 0 );
    }
}

static void mouse_move_done( void ) {

   LOG( "--- Move Done ---\n\r" );
   fsm_handle_event( MOUSE_MOVE_DONE_EVENT );
}

void demo1_rect( void ) {
   const int steps_cnt = 2;
   static int curr_step = 0;

   mouse_action procedure[] = {
      mouse_move_fw,
//      mouse_rotate_left
      mouse_rotate_right
   };

   if( curr_step < steps_cnt )
      (*procedure[ curr_step++ ])();
   else  mouse_brake();

   //if( curr_step == steps_cnt ) curr_step = 0;
}

void demo2_stair( void ) {
   const int steps_cnt = 7;
   static int curr_step = 0;

   mouse_action procedure[] = {
      mouse_move_fw,
      mouse_rotate_left,
      mouse_move_fw,
      mouse_rotate_right,
      mouse_move_fw,
      mouse_rotate_right,
      mouse_rotate_right
   };

   (*procedure[ curr_step++ ])();

   if( curr_step == steps_cnt ) curr_step = 0;
}

void perform_actions() {
	static int curr_step = 0;
	
    (*actionsBuffer[ curr_step++ ])();

   if( actionsBuffer[curr_step] == NULL ) curr_step = 0;
}
