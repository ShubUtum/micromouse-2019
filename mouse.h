/* 
 * File:   mouse.h
 * Author: Khaled
 *
 * Created on February 25, 2020, 2:38 AM
 */

#ifndef MOUSE2_H
#define MOUSE2_H

enum MOVEMENT{
    LEFT        = 0b00001000, 
    RIGHT       = 0b00000010,
    FORWARD     = 0b00000001, 
    BACK_TRACK  = 0b00010000,
    BACKWARD    = 0b00100000, 
    BRAKE       = 0b01000000, 
    STOP        = 0b10000000
};

typedef void ( *mouse_action )( void );

extern mouse_action actionsBuffer[10];

void mouse_init( void );
void mouse_brake( void );
void mouse_move_fw( void );
void mouse_move_bw( void );
void mouse_rotate_left( void );
void mouse_rotate_right( void );

void check_mouse_move( void );

void demo1_rect( void );
void demo2_stair( void );
void perform_actions( void );

#endif   /* MOUSE2_H */

