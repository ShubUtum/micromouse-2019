/*
 * File:   qei.h
 * Author: Sergey
 *
 * Created on November 25, 2019, 2:46 PM
 */

#ifndef QEI_H
#define	QEI_H


#include <xc.h>

void init_QEI(uint16_t base_resolution, uint16_t gearing_ratio, uint16_t edge_gain, uint16_t wheel_circumference);
float calc_velocity(uint16_t deltaTime);
int16_t calc_velocity_in_poscnt( void );
#endif	/* PWM_H */
