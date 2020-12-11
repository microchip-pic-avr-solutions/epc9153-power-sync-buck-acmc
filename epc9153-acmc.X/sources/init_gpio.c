/*
 * File:   init_gpio.c
 * Author: M91406.LA
 *
 * Created on November 2, 2019, 2:19 PM
 */


#include <xc.h>
#include <stdint.h>
#include "globals.h"

volatile uint16_t ConfigureGPIOs(void) {

    //Set I/O pins to low level as default
    LATA = 0;
    LATB = (1 << 8);
    //all inputs are for ADC or the enable line, 1=input, 0=output
    TRISA = 0b111;
    TRISB = (1 << 3) | (1 << 4 ) | (1 << 9);//(1 << 0) | (1 << 4) | (1 << 9) | (1 << 7);


    //set analog pins to free others to be digital
    ANSELA = 0xF;
    ANSELB = (1 << 2) | (1 << 3) | (1 << 4);
    
    CNPDBbits.CNPDB14 = 1;  // Enable intern pull down register (PWM1H)
    CNPDBbits.CNPDB15 = 1;  // Enable intern pull down register (PWM1L)
    
//    CNPDBbits.CNPDB12 = 1;  // Enable intern pull down register (PWM2H)
//    CNPDBbits.CNPDB13 = 1;  // Enable intern pull down register (PWM2L)    
    
    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
#ifdef DEBUG_MODE    
    __builtin_write_RPCON(0x0000); // unlock PPS
//
    RPOR4bits.RP40R = 0x0001;    //RB8->UART1:U1TX
////    RPOR5bits.RP42R = 0x0024;    //RB10->PWM:PWMEA
////    RPOR5bits.RP43R = 0x0025;    //RB11->PWM:PWMEB    
//    RPOR2bits.RP36R = 0x0018;    //RB4->CMP2:CMP2
//    RPOR5bits.RP42R = 0x0018;    //RB10->CMP2:CMP2
    RPOR5bits.RP42R = 0x0017;    //RB10->CMP1:CMP1
    RPINR18bits.U1RXR = 0x0029;    //RB9->UART1:U1RX
//
    __builtin_write_RPCON(0x0800); // lock PPS
#endif    

    /* disable unused peripherals */
#ifndef DEBUG_MODE 
    PMD1 = 0x04F8;
    PMD2 = 0x1F;
    PMD3 = 0xAA;
    PMD4bits.REFOMD = 1;
    PMD6 = 0xF01;
    PMD7 = 0x408;
    PMD8 = 0x193E;
#endif    

    return(1);
}
