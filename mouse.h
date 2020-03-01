/* 
 * File:   mouse.h
 * Author: Khaled
 *
 * Created on February 25, 2020, 2:38 AM
 */

#ifndef MOUSE2_H
#define MOUSE2_H


typedef void ( *mouse_action )( void );

void mouse_init( void );
void mouse_move_fw( void );
void mouse_move_bw( void );
void mouse_rotate_left( void );
void mouse_rotate_right( void );

void check_mouse_move( void );

void demo1_rect( void );
void demo2_stair( void );

#endif   /* MOUSE2_H */

