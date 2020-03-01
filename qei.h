/*
 * File:   qei.h
 * Author: Sergey
 *
 * Created on November 25, 2019, 2:46 PM
 */

#ifndef QEI_H
#define	QEI_H


#include <xc.h>

typedef struct qei_params_s {

   long longCNT;
   long prev_count;

   uint16_t base_resolution;
   uint16_t gearing_ratio;
   uint16_t edge_gain;
   uint16_t wheel_circumference;
} qei_params;


void init_QEI_1(uint16_t base_resolution, uint16_t gearing_ratio, uint16_t edge_gain, uint16_t wheel_circumference);
void init_QEI_2(uint16_t base_resolution, uint16_t gearing_ratio, uint16_t edge_gain, uint16_t wheel_circumference);

int16_t qei1_get_poscnt( void );
int16_t qei2_get_poscnt( void );

#endif	/* PWM_H */
