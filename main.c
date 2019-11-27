
#include <xc.h> 
#include "timer.h"
#include "pwm.h"
#include "gpio.h"
#include "qei.h"
#include "uart.h"

// configuration bits, can be configured using GUI: window -> target memory views -> configuration bits 
// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = ON                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Source (HS Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = OFF           // Peripheral Pin Select Configuration (Allow Multiple Re-configurations)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS1            // Watchdog Timer Postscaler (1:1)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR1             // POR Timer Value (Disabled)
#pragma config ALTI2C = OFF             // Alternate I2C  pins off
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

#define IN_SIMULATION_MODE  0

int main(void) {
    uint16_t curr_dc = 10;
    
    /*** oscillator setup --------------------------------------------------
    Here we are using PPL for 16MHz to generate 80MHz clock.
    FCY = 0.5 * (16MHz*20/(2*2)) = 40 MIPS
    ---------------------------------------------------------------------***/
    PLLFBD = 18; //set PPL to M=20 (18+2)
    CLKDIVbits.PLLPRE = 0; //N1 = input/2
    CLKDIVbits.PLLPOST = 0; //N2 = output/2
    if (IN_SIMULATION_MODE != 1)
    {
        while (OSCCONbits.LOCK != 1); //Wait for PPL to lock
    }
    
    initIO();
    timer1_setup( 100 );        // 100 ms timer
    pwm2_setup( 50, curr_dc );   // DC 10% of 50ms period
    
    timer1_start();
    pwc2_run();
    
    while(1){
        
        wait_ms( 1000 ); // wait 1 second

        //timer1_stop();
        //timer1_setup( 200 );
        //timer1_start();
        //reset_current_time();

        curr_dc = ( curr_dc + 10 ) % 100;
        pwc2_change_dc( curr_dc );   // change DC by +10%
    }

    //timer1_stop();
    //pwc2_stop();
    //while (1);
    return 0;
}