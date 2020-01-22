
#include "gpio.h"
#include "uart.h"
uint16_t error_flag = 0;

void initIO() {
    int i;
    /***********************************************************************
     *  PIN#     name        I/O   mappable?  function                                          Bread board change?
     ***********************************************************************
     *   17     PWM2H1(RB8)   op     N        PWM2 Left Motor Speed    (RED LED)
     *   25     PWM1H1(RB14)  op     N        PWM1 Right Motor Speed
     * 
     *   2       AN0(RA0)     ip     N        ADC ch 1
     *   3       AN1(RA1)     ip     N        ADC ch 2
     *   4       AN2(RB0)     ip     N        ADC ch 3
     * 
     *   23      RB12         op     Y        UART2 TX
     *   24      RB13         ip     Y        UART2 RX
     * 
     *   6       RB2          ip     Y        QEI1 A - Left Motor encoder ch A
     *   7       RB3          ip     Y        QEI1 B - Left Motor encoder ch B
     *   21      RB10         ip     Y        QEI2 A - Right Motor encoder ch A
     *   22      RB11         ip     Y        QEI2 B - Right Motor encoder ch B
     * 
     *   11      RB4          op     Y        GP - Left Motor H bridge dir control 1                  Yes
     *   14      RB5          op     Y        GP - Left Motor H bridge dir control 2                  Yes
     *   15      RB6          op     Y        GP - Right Motor H bridge dir control 1
     *   16      RB7          op     Y        GP - Right Motor H bridge dir control 2
     * 
     *   26      RB15         op     Y        GP - output(GREEN LED)                                  Yes
     *   12      RA4          ip     Y        Switch                                                  Yes
     * 
     *   5       AN3(RB1)                     FREE
     *   18      RB9                          FREE
     ***********************************************************************/
    // set up analog pins
    AD1CON1bits.ADON = 0; // disable ADC1 module
    AD1PCFGL=0xFFFF; //all pins are digital
   
    // set digital port direction
    // inputs
    //all pins are inputs by default
    TRISAbits.TRISA4  =1; //Switch input
    //TRISBbits.TRISB13 =1; //UART2 RX
    //TRISBbits.TRISB2  =1; //QEI1 A
    //TRISBbits.TRISB3  =1; //QEI1 B

    //outputs
    TRISBbits.TRISB8  =0; //PWM2H1 & RED LED output
    TRISBbits.TRISB15 =0; //GREEN LED output
    TRISBbits.TRISB6  =0; //RED LED
    TRISBbits.TRISB8  =0; //PWM_RED LED
    TRISBbits.TRISB4  =0; //H bridge dir control 1
    TRISBbits.TRISB5  =0; //H bridge dir control 2
    //TRISBbits.TRISB12 =0; //UART2 TX
    

    // set up remappable pins
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)
    // inputs
     // input pin mapping example below:, here you choose the register with the device and assign an remappable pin
    RPINR14bits.QEA1R = 2; // QEI1 A to pin RB2
    RPINR14bits.QEB1R = 3; // QEI1 B to pin RB3
    RPINR16bits.QEA2R = 10; //QEI2 A to pin RB10
    RPINR16bits.QEB2R = 11; //QEI2 B to pin RB11
    
    RPINR19bits.U2RXR = 13; // input from uart to pin RB13
    //RPINR18bits.U1RXR = 1 ; //U1RX register is pin RB1 / RP1
    
    // outputs
    //output pin mapping example below, here you choose the pin register and then map it to a device 
    //RPOR6bits.RP12R = 0b00111; // pin RB12 to SPI1 miso (the cod 0b00111 idetifies SPI miso, table in datasheet)  
    //RPOR1bits.RP2R = 0b00011; // pin RP2 is connected to UART1 TX, for coding see table 11-2 in datasheet
    //end of set up of remappable pins
    
    RPOR6bits.RP12R = 0b00101;  // Map RB12 to U2TX UART2 Transmit
    
    RPOR4bits.RP8R  = 0; //default pin output for PWM2H1 (RED LED)
    RPOR7bits.RP14R = 0; //default pin output for PWM1H1 
    
    RPOR2bits.RP4R  = 0; //default pin output for H bridge dir control 
    RPOR2bits.RP5R  = 0; //default pin output for H bridge dir control 
    RPOR3bits.RP6R  = 0; //default pin output for H bridge dir control 
    RPOR3bits.RP7R  = 0; //default pin output for H bridge dir control 
    
    RPOR7bits.RP15R = 0; //default pin output for GREEN LED //0 is default
    
    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)


    for (i = 0; i < 30000; i++); // short delay
}

void error() {
    LOG("\n\r\n\rxxxxxxxxxxxx\n\r ERROR \n\rxxxxxxxxxxxx\n\r");
    RED_LED     = 1;
    GREEN_LED   = 1;
    error_flag = 1;
}

