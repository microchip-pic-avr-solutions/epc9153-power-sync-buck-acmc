/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */

#include <xc.h>
#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */

#include "globals.h"


void __attribute__ ( ( __interrupt__ , auto_psv ) ) _ADCAN1Interrupt(void)
{
//    uint16_t cmp_channel;
    LATB |= (1 << 1);  // pin 14, bottom pin on the left side

    vout_adc = ADCBUF0;
    vin_adc = ADCBUF9;

    newadcavailable = 1;//means that some ADC has triggered interrupt since its measurement is available
    iout_adc = ADCBUF1;
    
    switch(controlstate) {

        case normal:  // normal PWM, usually high current
            

//            if ( ADCMP3CONbits.STAT ) { // low current
//            if ( iout_adc < (cs_calib_offset + 75) ) {
//                low_iout_count++;
////                cmp_channel = ADCMP3CONbits.CHNL;
//                if ( (low_iout_count > 240) && ( DAC1CONLbits.CMPSTAT ) ) { // Vout is good for at least 40 PWM cycles
////                if ( DAC1CONLbits.CMPSTAT ) { // Vout is good
//                    PG1FFPCIHbits.BPEN = 0; // enable FF PCI
//                    controlstate = lowpower;  // enter low power 
////                    while (PG1STATbits.UPDATE == 1);
////                    PG1TRIGB = (DUTYBURST >> 1)+ADC_TRIG_OFFSET;
////                    PG1DC = DUTYBURST;
//                    hc_count = 0;
//                    counter_inactive = 0;
//                    counter_pd_max = 0;
//                    counter_pd = 0;
//                    _ADCAN1IF = 0;
//                    LATB &= ~(1 << 1);
//                    return; // exit interrupt 
//                }
//            } else {
//                low_iout_count = 0;
//            }
            v_loop_Update(&v_loop);
            i_loop_Update(&i_loop);            
            while (PG1STATbits.UPDATE == 1);
            PG1TRIGB = i_trigger;
            PG1DC = duty_out;            
            break;
            
        case lowpower: // low power
            if ( vout_adc > VOUTADC(20.4) ) {
                    controlstate = normal;
                    low_iout_count = 0;
                    counter_pd_max = 0;
                    counter_pd = 0;

                    v_loop_Update(&v_loop);
                    i_loop_Update(&i_loop);            
                    while (PG1STATbits.UPDATE == 1);
                    PG1TRIGB = i_trigger;
                    PG1DC = duty_out;                       
                    PG1FFPCIHbits.BPEN = 1; // disable FF PCI                
            } else
//            if ( !DAC1CONLbits.CMPSTAT ) { // Vout is not good (low)
            if ( counter_pd_max > 6 ) {
//            if ( (iout_adc > 2640) || (counter_pd_max> 11) ) { // 2640
                if ( iout_adc > ( cs_calib_offset + 270) ) {
                    controlstate = normal;
                    low_iout_count = 0;
                    counter_pd_max = 0;
                    counter_pd = 0;

                    v_loop_Update(&v_loop);
                    i_loop_Update(&i_loop);            
                    while (PG1STATbits.UPDATE == 1);
                    PG1TRIGB = i_trigger;
                    PG1DC = duty_out;                       
                    PG1FFPCIHbits.BPEN = 1; // disable FF PCI
                }
            } 
            else { // still low current
 
                if (!(DAC1CONLbits.CMPSTAT)) { // voltage falls below 20 V
                    if ( counter_pd > 20 ) { // voltage falls below 20 V for too long
                         controlstate = normal;
                         low_iout_count = 0;
                         counter_pd_max = 0;
                         counter_pd = 0;

                         v_loop_Update(&v_loop);
                         i_loop_Update(&i_loop);            
                         while (PG1STATbits.UPDATE == 1);
                         PG1TRIGB = i_trigger;
                         PG1DC = duty_out;                       
                         PG1FFPCIHbits.BPEN = 1; // disable FF PCI                    
                     } else {               
                        counter_inactive = 0;
                        counter_pd++;
                     }
                } else {
                    
                    if (counter_inactive > 69) {
//                        controlstate = forcecharge;
//                        while (PG1STATbits.UPDATE == 1);
//                        PG1TRIGB = (DUTYFORCE >> 1)+ADC_TRIG_OFFSET;
//                        PG1DC = DUTYFORCE;    
                        PG1FFPCIHbits.BPEN = 1; // disable FF PCI, start switching
                        counter_force = 0;
                        counter_pd = 0;
                        counter_pd_max = 0;
                        _PWM1IF = 0;
                        _ADCAN1IE = 0;
                        _PWM1IE = 1; // enable PWM1 interrupt

                    }
                }
            }
            break;
            
//        case forcecharge: // force charge
//            if ( iout_adc > 2640 ) { //2640
//                controlstate = normal;
//                low_iout_count = 0;
//                v_loop_Update(&v_loop);
//                i_loop_Update(&i_loop);            
//                while (PG1STATbits.UPDATE == 1);
//                PG1TRIGB = i_trigger;
//                PG1DC = duty_out;       
//            } else {
//                PG1FFPCIHbits.BPEN = 0; // enable FF PCI
//                controlstate = lowpower;  // enter low power 
////                while (PG1STATbits.UPDATE == 1);
////                PG1TRIGB = (DUTYBURST >> 1)+ADC_TRIG_OFFSET;
////                PG1DC = DUTYBURST;
//                hc_count = 0;
//                counter_inactive = 0;
//              
//            }
//            break;
    }
    
    temperature_adc = ADCBUF19;
#ifdef DEBUG_MODE    
        max_counter++;
        if (max_counter==0xFFFF) {
//            max_hc = 0;
            max_iout = 0;
        } else {
            if (iout_adc>max_iout)
                { max_iout = iout_adc; }
//            if (hc_count>max_hc)
//                { max_hc = hc_count;}
        }    
#endif       

//    LATB &= ~(1 << 1);    
    _ADCAN1IF = 0;
}

void __attribute__((interrupt, auto_psv)) _PWM1Interrupt(void) {
//    if (PG1FFPCIHbits.BPEN == 1) {
    counter_pd = 0;
    counter_pd_max = 0;
    counter_force++;
    if (counter_force>1) {
        PG1FFPCIHbits.BPEN = 0; // enable FF PCI
        counter_inactive = 0;
        _PWM1IE = 0;
        _ADCAN1IE = 1;
    }
//    pwm1_count++;
//    if (pwm1_count == 1) {
//        LATB |= (1 << 15);   // turn on Q2
//    } else if (pwm1_count == 3) {
//        PG1IOCONH |= 0x000C; // enable PWM
//        _PWM1IE = 0;  // disable PWM2 interrupt        
//    }
    _PWM1IF = 0;
}

void __attribute__ ( ( __interrupt__ , auto_psv ) ) _ADCMP0Interrupt(void)
{
    systemstate = latchoff_sc;
    while(PG1STATbits.UPDATE);
    //disable PWM
    PG1IOCONHbits.PENH = 0; 
    PG1IOCONHbits.PENL = 0;       
    
    _ADCAN1IE = 0;   // disable interrupts
    _ADCMP0IE = 0;
    _ADCMP0IF = 0;
}