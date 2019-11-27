/*
 * File:   uart.c
 * Author: gaoyingqiang
 *
 * Created on November 19, 2019, 10:44 AM
 */

#include <string.h> //for strlen()
#include "xc.h"
#include "uart.h"
#include <ctype.h>
#include "timer.h"
#include "gpio.h"
#include <stdlib.h> // for atoi()
#include "pwm.h"

uint16_t configUART2( uint16_t baud_rate, uint16_t fcy) //baud_rate in k, fcy in M
{
    float baud_rate_U2BRG;
    baud_rate_U2BRG = (fcy * 1000000 / (16 * (baud_rate * 1000)) ) - 1;
    baud_rate_U2BRG = (int) baud_rate_U2BRG;
    baud_rate_U2BRG = 16;
    U2MODEbits.UARTEN    = 0;     // disable UART
    U2BRG                = baud_rate_U2BRG;    // set baud rate to 57.6k bit/s
    U2MODEbits.LPBACK    = 0;     //disable loop-back mode
    U2MODEbits.WAKE      = 0;     // disable wake-up
    U2MODEbits.ABAUD     = 0;     // disable baud rate measurement 
    U2MODEbits.PDSEL     = 0b00;  // 8 bit data, no parity
    U2MODEbits.STSEL     = 0;     // one stop bit
    U2STAbits.URXISEL    = 0b00;  // interrupt when 1 character arrived
    U2MODEbits.UARTEN    = 1;     // enable UART
    U2STAbits.UTXEN      = 1;     // enable transmit
    
    return 0;
}

void send_A2Z( void )
{
    static uint16_t char_A2Z = 'A';
    U2TXREG = char_A2Z; 
    char_A2Z ++;
    if(char_A2Z > 'Z')
    {
        char_A2Z = 'A';
    }
   // wait_ms( 100 ); // wait 100ms
}

void send_char( char c ) // send one byte at a time
{
    U2TXREG = c;
    while(U2STAbits.TRMT == 0);
}


void mySendString(char * textString)
{
    int mystrlen = strlen(textString);
    int i;
    for(i=0;i<mystrlen;i++)
    {
        send_char(textString[i]);
    }
}
 

void adjust_LED4( void )
{ 
    char char_dc[5];
    int counter = 0;
    while(IFS1bits.U2RXIF==1) // receive interrupt triggered
    {
        if(U2RXREG != '<' && U2RXREG != '>') // copy the contents between <>
        {
            char_dc[counter] = U2RXREG;
            counter = counter + 1;
            IFS1bits.U2RXIF = 0;
        } 
    }
    
    int i = 0;
    int j = 0;
    char rplstr[10] = "OK:";
    int invalid_fc = 0;
    while(i<strlen(char_dc))
    {
        if(~isdigit(char_dc[i])) // if char_dc contains non-numbers
        {
            mySendString("ERROR!");
            i = i + 1;
            invalid_fc = 1;
            break;
        }
        else
        {
            rplstr[2+j] = char_dc[i];
            j = j+ 1;
        }
    }
    
    if(~invalid_fc)
    {
        char *reply = rplstr;
        mySendString(reply);
    }
   
    int int_dc = atoi(char_dc);
    if(int_dc > 0 && int_dc <= 100)
    {
        pwc2_change_dc( int_dc );
    }
    else
    {
        mySendString("ERROR!");
    }
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;     // clear Rx interrupt flag
    IPC7bits.U2RXIP = 0b001; // interrupt priority 1
    
    LED2 = ~LED2; // toggle LED2 when Rx Interrupt is called
    
    if(U2STAbits.OERR==1) // if Receive buffer has overflowed
    {
        U2STAbits.OERR = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
    while(U2STAbits.TRMT == 0) // Transmit Shift register is not empty
    {
        IFS1bits.U2TXIF = 0;     // clear Tx interrupt flag 
    }
}

