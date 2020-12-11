/*
 * File:   init_cmp.c
 * Author: YZ
 *
 * Created on December 18, 2019
 */
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"

volatile uint16_t ConfigureComparators(void) {

    // Disable the CMP module before the initialization
    DACCTRL1Lbits.DACON = 0;
    
	// Comparator Register settings
	DACCTRL1L = 0x37; //CLKDIV 1:4; DACON disabled; DACSIDL disabled; FCLKDIV 1:8; CLKSEL AFVCO/2 = 249 MHz; 
	DACCTRL2L = 0x55; //TMODTIME 85; 
	DACCTRL2H = 0x8A; //SSTIME 138; 
    
    // DAC for IREF
	DAC2CONH = 0x00; //TMCB 0; 
	DAC2CONL = 0x8200; //CMPPOL Non Inverted; HYSPOL Rising Edge; HYSSEL None; DACEN enabled; FLTREN disabled; CBE disabled; IRQM Interrupts are disabled; INSEL CMP1A; DACOEN enabled; 

	DAC2DATL = 0x00; //DACDATL 0; 
	DAC2DATH = ISENSEREF; //DACDATH 205 is minimum value 20;//37; 
    
    //Hysteretic 
    DAC1DATL = VOUTADC(11.9); //DACDATL 0;
    DAC1DATH = VOUTADC(12.0);//1416;//VOUTADC(10);
//    DAC1CONHbits.TMCB = 0xFF;
//    SLP1CONLbits.HCFSEL = 1; // 1 = PWM1H
//    SLP1CONHbits.HME = 1; // Hysteretic Mode
    DAC1CONLbits.HYSSEL = 0b00;  // no hysteresis
    DAC1CONLbits.HYSPOL = 0;    // Hysteresis is applied to the falling edge of the comparator output
    DAC1CONLbits.INSEL = 0; // CMP1A input
    DAC1CONLbits.FLTREN = 1;
//    DAC1CONLbits.CMPPOL = 1;
    DAC1CONLbits.DACEN = 1; // Enable DAC module    

    // Enable
    DACCTRL1Lbits.DACON = 1;

    return (1);
}
