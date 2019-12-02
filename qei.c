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

static long _longCNT = 0;

static uint16_t _base_resolution;
static uint16_t _gearing_ratio;
static uint16_t _edge_gain;
static uint16_t _wheel_circumference;

static long _prev_count;

// base_resolution = 16; gearing_ratio = 33; edge_gain = 4;
void init_QEI(uint16_t base_resolution, uint16_t gearing_ratio, uint16_t edge_gain, uint16_t wheel_circumference) {

    _base_resolution = base_resolution;
    _gearing_ratio = gearing_ratio;
    _edge_gain = edge_gain;
    _wheel_circumference = wheel_circumference;

     // Configure QEI pins as digital inputs
    //ADPCFGbits.PCFG5 = 1; // QEB on pin 7 shared with AN5
    //ADPCFGbits.PCFG4 = 1; // QEA on pin 6 shared with AN4

    // Configure QEI module
    QEICONbits.QEIM = 0;        // Disable QEI Module
    QEICONbits.CNTERR = 0;      // Clear any count errors
    QEICONbits.QEISIDL = 1;     // Discontinue operation when in idle mode/sleep
    QEICONbits.SWPAB = 0;       // QEA and QEB not swapped
    QEICONbits.PCDOUT = 0;      // Disable counter direction pin (normal I/O operation)
    QEICONbits.POSRES = 0;      // index pulse does not reset POSCNT
    DFLTCONbits.CEID = 1;       // Count error interrupts disabled
    DFLTCONbits.QEOUT = 0;      // disable digital filters
    QEICONbits.UPDN_SRC = 0;
    QEICONbits.TQCS = 0;        // internal clock source (Tcy)
    QEICONbits.QEIM = 0b111;    // x4 edge gain and reset POSCNT when == MAXCNT

    // set initial counter value and maximum range
    MAXCNT = 0xffff;    // set the highest possible time out
    POSCNT = 5;    // set POSCNT into middle of range
    _prev_count = POSCNT;

    // Configure Interrupt controller
    IFS3bits.QEI1IF = 0;        // clear interrupt flag
    IEC3bits.QEI1IE = 0;        // enable QEI interrupt
    IPC14bits.QEI1IP = 5;       // set QEI interrupt priority
}

// interrupt service routine
void __attribute__((interrupt, auto_psv)) _QEI1Interrupt(void)
{
    IFS3bits.QEI1IF = 0; // clear interrupt flag
    
    if(POSCNT < 32768) {
        _longCNT += 0x10000; // over-run condition caused interrupt
    }
    else {
        _longCNT -= 0x10000; // under-run condition caused interrupt
    }
}

float calc_velocity(uint16_t deltaTime) {
    float f = 1.0f/(deltaTime);
    long currentCount = _longCNT + POSCNT;
    float v = (currentCount - _prev_count) * f;
    _prev_count = currentCount;
    v = (2*PI / (_base_resolution * _gearing_ratio * _edge_gain)) * v;
    if (_wheel_circumference == 0) {
        return v;
    }
    else {
        return _wheel_circumference * v;
    }
}

