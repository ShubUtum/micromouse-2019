
#include "timer.h"
#include "gpio.h"


void initTimer1(unsigned int period) 
{
    T1CON = 0; // ensure Timer 1 is in reset state
    T1CONbits.TGATE = 0; // gated time accumulation disabled
    T1CONbits.TCKPS = 0b10; // FCY divide by 64: tick = 1.6us
    T1CONbits.TCS = 0; // select internal FCY clock source
    TMR1 = 0;
    PR1 = period-1; // set Timer 1 period register ()
    IFS0bits.T1IF = 0; // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4; // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1; // enable Timer 1 interrupt
    T1CONbits.TON = 0; // leave timer disabled initially
    
}

void startTimer1(void) {
    T1CONbits.TON = 1; //
   
  
}

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
        static  int count=0;


        IFS0bits.T1IF = 0; // reset Timer 1 interrupt flag


        LED1=~LED1;
        if (count<9)
        {
            count++;
        }
        else
        {
            //LED1=~LED1;
            count=0;
        }

    
}

