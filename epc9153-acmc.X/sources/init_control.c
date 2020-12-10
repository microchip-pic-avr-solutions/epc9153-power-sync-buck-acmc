/*
 * File:   init_control.c
 * Author: M91406.LA
 *
 * Created on November 2, 2019, 3:15 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"


volatile uint16_t ConfigureControlLoops(void) {

    // Configuring the outer voltage loop controller
    
    
    
    v_loop.ptrSource = &vout_adc;
    v_loop.ptrTarget = &i_ref;
    v_loop.ptrControlReference = &v_ref;
    v_loop.MaxOutput = IOUTADC(ILIMITACTUALAMPS);//DUTYMAX;;
    v_loop.MinOutput = 0;
    v_loop.ptrDataProviderControlError = &v_loop_error;
    
//    v_loop_Reset(&v_loop);
    v_loop_Init(&v_loop);
//    v_loop_POSB_load = 1;
//    v_loop_POSB_noload = 2;
    
    // Configuring the inner current loop controller
    
    
    
    i_loop.ptrSource = &iout_adc;
    i_loop.ptrTarget = &duty_out;
    i_loop.ptrControlReference = &i_ref;
    i_loop.MaxOutput = DUTYMAX;
    i_loop.MinOutput = 0;
    i_loop.InputOffset = ISENSEOFFSET;
    i_loop.ptrADCTriggerARegister = &i_trigger;//&TRIG2;
    i_loop.ADCTriggerAOffset = ADC_TRIG_OFFSET;
//    i_loop.ptrADCTriggerBRegister = &v_trigger;//&TRIG2;
//    i_loop.ADCTriggerBOffset = 0;//((PWMPER >> 1) + ADC_TRIG_OFFSET);
    
//    i_loop_Reset(&i_loop);
    i_loop_Init(&i_loop);

    
    
    return (1);
}
