/*
 * File:   qei.c
 * Author: Sergey
 *
 * Created on November 25, 2019, 2:46 PM
 */

#include <xc.h>
#include "qei.h"
#include "uart.h"
#include <math.h>
#include <dsp.h>
#include <stdio.h>

qei_params qei1;
qei_params qei2;



static int16_t qei_get_poscnt_speed( qei_params *qei );



// base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
void init_QEI_1(uint16_t base_resolution, uint16_t gearing_ratio, uint16_t edge_gain, uint16_t wheel_circumference) {

   /* init QEI 1 config parameters */
   qei1.longCNT             = 0;
   qei1.prev_count          = 0;
   qei1.base_resolution     = base_resolution;
   qei1.gearing_ratio       = gearing_ratio;
   qei1.edge_gain           = edge_gain;
   qei1.wheel_circumference = wheel_circumference;

     // Configure QEI pins as digital inputs
    //ADPCFGbits.PCFG5 = 1; // QEB on pin 7 shared with AN5
    //ADPCFGbits.PCFG4 = 1; // QEA on pin 6 shared with AN4

    // Configure QEI module
    QEI1CONbits.QEIM = 0;        // Disable QEI Module
    QEI1CONbits.CNTERR = 0;      // Clear any count errors
    QEI1CONbits.QEISIDL = 1;     // Discontinue operation when in idle mode/sleep
    QEI1CONbits.SWPAB = 0;       // QEA and QEB not swapped
    QEI1CONbits.PCDOUT = 0;      // Disable counter direction pin (normal I/O operation)
    QEI1CONbits.POSRES = 0;      // index pulse does not reset POSCNT
    DFLT1CONbits.CEID = 1;       // Count error interrupts disabled
    DFLT1CONbits.QEOUT = 0;      // disable digital filters
    QEI1CONbits.UPDN_SRC = 0;
    QEI1CONbits.TQCS = 0;        // internal clock source (Tcy)
    QEI1CONbits.QEIM = 0b111;    // x4 edge gain and reset POSCNT when == MAXCNT

    // set initial counter value and maximum range
    MAX1CNT = 0xffff;    // set the highest possible time out
    POS1CNT = 0x7fff;    // set POS1CNT into middle of range
    qei1.prev_count = POS1CNT;

    // Configure Interrupt controller
    IFS3bits.QEI1IF = 0;        // clear interrupt flag
    IEC3bits.QEI1IE = 0;        // enable QEI interrupt
    IPC14bits.QEI1IP = 5;       // set QEI interrupt priority
}

// base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
void init_QEI_2(uint16_t base_resolution, uint16_t gearing_ratio, uint16_t edge_gain, uint16_t wheel_circumference) {

   /* init QEI 2 config parameters */
   qei2.longCNT             = 0;
   qei2.prev_count          = 0;
   qei2.base_resolution     = base_resolution;
   qei2.gearing_ratio       = gearing_ratio;
   qei2.edge_gain           = edge_gain;
   qei2.wheel_circumference = wheel_circumference;

    // Configure QEI module
    QEI2CONbits.QEIM = 0;        // Disable QEI Module
    QEI2CONbits.CNTERR = 0;      // Clear any count errors
    QEI2CONbits.QEISIDL = 1;     // Discontinue operation when in idle mode/sleep
    QEI2CONbits.SWPAB = 0;       // QEA and QEB not swapped
    QEI2CONbits.PCDOUT = 0;      // Disable counter direction pin (normal I/O operation)
    QEI2CONbits.POSRES = 0;      // index pulse does not reset POSCNT
    DFLT2CONbits.CEID = 1;       // Count error interrupts disabled
    DFLT2CONbits.QEOUT = 0;      // disable digital filters
    QEI2CONbits.UPDN_SRC = 0;
    QEI2CONbits.TQCS = 0;        // internal clock source (Tcy)
    QEI2CONbits.QEIM = 0b111;    // x4 edge gain and reset POSCNT when == MAXCNT

    // set initial counter value and maximum range
    MAX2CNT = 0xffff;    // set the highest possible time out
    POS2CNT = 0x7fff;    // set POS1CNT into middle of range
    qei2.prev_count = POS2CNT;

    // Configure Interrupt controller
    IFS4bits.QEI2IF = 0;        // clear interrupt flag
    IEC4bits.QEI2IE = 0;        // enable QEI interrupt
    IPC18bits.QEI2IP = 5;       // set QEI interrupt priority
}

// QEI1 interrupt service routine
void __attribute__((interrupt, auto_psv)) _QEI1Interrupt(void)
{
    IFS3bits.QEI1IF = 0; // clear interrupt flag
    
    if(POS1CNT < 32768) {
       qei1.longCNT += 0x10000; // over-run condition caused interrupt
    }
    else {
       qei1.longCNT -= 0x10000; // under-run condition caused interrupt
    }
}

// QEI2 interrupt service routine
void __attribute__((interrupt, auto_psv)) _QEI2Interrupt(void)
{
    IFS4bits.QEI2IF = 0; // clear interrupt flag

    if(POS2CNT < 32768) {
       qei2.longCNT += 0x10000; // over-run condition caused interrupt
    }
    else {
       qei2.longCNT -= 0x10000; // under-run condition caused interrupt
    }
}

float qei_calc_velocity(uint16_t deltaTime) {
    float f = 1.0f/(deltaTime);
    long currentCount = qei1.longCNT + POS1CNT;
    float v = (currentCount - qei1.prev_count) * f;
    qei1.prev_count = currentCount;
    v = (2*PI / (qei1.base_resolution * qei1.gearing_ratio * qei1.edge_gain)) * v;
    if (qei1.wheel_circumference == 0) {
        return v;
    }
    else {
        return qei1.wheel_circumference * v;
    }
}

// return POS1CNT change since last call(POS1CNT/time_unit). can be a negative or positive value based on the Motor direction
static int16_t qei_get_poscnt_speed( qei_params *qei ) {

    uint32_t currentCount = qei->longCNT + POS1CNT;

    int16_t v = currentCount - qei->prev_count;

    qei->prev_count = currentCount;

    return v;
}

int16_t qei1_get_poscnt_speed( void ) {
   return qei_get_poscnt_speed( &qei1 );
}
int16_t qei2_get_poscnt_speed( void ) {
   return qei_get_poscnt_speed( &qei2 );
}

