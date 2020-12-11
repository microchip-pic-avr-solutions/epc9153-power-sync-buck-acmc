/*
 * File:   power_control.c
 * Author: M91406.LA
 *
 * Created on November 2, 2019, 3:52 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"

#define CS_CALIB_STEPS 8U // current calibration steps
typedef struct {
    volatile uint16_t cs_calib_cnt;
    volatile uint16_t cs_calib_offset;
}CS_CALIBRATION_t;
    
volatile CS_CALIBRATION_t calib_cs1;
volatile bool cs_calib_complete;

void CurrentSenseCalibration(void);

volatile uint16_t PowerControlStateMachine(void) {
    
    static uint16_t delay=0;
    volatile uint16_t fres = 0;
//    static uint16_t delayTest;
    
    if (systemstate != systemstateprevious) {
        statechanged = 1;
        systemstateprevious = systemstate;
    }
    
    
    switch (systemstate) {
        case off:
            if (statechanged == 1) {
                statechanged = 0; //allow for delay for on/off events
                mscounter = 0;
            }
//            LATB |= (1 << 5);                
            PWMDISABLED;
            PWMHOLD;

            v_ref = 0;
            i_ref = 0;//IOUTADC(3);
            e_cfly = 0;
            sum_e_cfly = 0;
            max_counter = 0;
            counter_inactive = 0;
            counter_pd = 0;
            controlstate = normal;
            cs_calib_offset = ISENSEOFFSET;
            cs_calib_complete = false;
            calib_cs1.cs_calib_cnt = 0;
            calib_cs1.cs_calib_offset = 0;
//            e_pos = 0;
//            e_neg = 0;
//            e20_n1 = 0;
//            e20_n2 = 0;
            deltaD_offset = 4;
//            DACCTRL1Lbits.DACON = 0; // disable current comparator
    

            if ( (mscounter > 500) && (vin_adc > VINADC(UVLO_ON)) && (vin_adc < VINADC(OVLO_IN)) ) //&& 
                 //   (vin_adc > VINADC(UVLO_ON)) && 
                //    (vin_adc < VINADC(OVLO_ON))  &&
                  //  (temperature_adc < TEMPERATUREON) 
            {
                systemstate = outputdischarged;
                
            }
            break;
            
        case outputdischarged:
            statechanged = 0;
            
            // input or output over voltage
            if ( (vin_adc > VINADC(OVLO_IN)) || (vout_adc > VOUTADC(OVLO_OUT)) ) {
                systemstate = latchoff;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;
            } 
            
            // input under voltage 
            if (vin_adc < VINADC(UVLO_OFF)) {
                systemstate = off;
                break;
            } 
                       
            if (vout_adc < VOUTADC(VOUTKEEPOFFATSTARTUP)) {
                if (ISNS_NEED_CALIBRATION) {
                    systemstate = current_sns_cal;     
                } else {
                    systemstate = softstart_duty_ramp;
                    while (PG1STATbits.UPDATE == 1);
                    PWMRELEASE;
                    PG1IOCONH |= 0x000C; // enable PWM
                    v_ref = vout_adc;// + RAMPRATE;
    //                i_loop.MaxOutput = DUTYRAMPRATE;
    //                e_cfly = 0;
    //                sum_e_cfly = 0;       
    //                v_loop.status.bits.enable = 1;
    //                i_loop.status.bits.enable = 1;
                    duty_out = DUTYRAMPRATE;
    //                v_loop_only.status.bits.enable = 1;    
    //                v_loop_only_Precharge(&v_loop_only , RAMPRATE , 200 );
    //                v_loop_Precharge(&v_loop , v_ref , IOUTADC(0.15) );
    //                i_loop_Precharge(&i_loop , 0 , 0 );  
                    delay = 0;                                        
                }
            }
            break;

        case current_sns_cal:
            statechanged = 0;
            
            // input or output over voltage
            if ( (vin_adc > VINADC(OVLO_IN)) || (vout_adc > VOUTADC(OVLO_OUT)) ) {
                systemstate = latchoff;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;
            } 
            
//            // input under voltage 
//            if (vin_adc < VINADC(UVLO_OFF)) {
//                systemstate = off;
//                break;
//            } 
            
            CurrentSenseCalibration();
                       
            if (cs_calib_complete) {
//                i_loop.InputOffset = cs_calib_offset;
                fault_ocp.trip_level = IOUTADC(ILIMITOCP)+cs_calib_offset;   // Set fault trip level
                systemstate = softstart_duty_ramp;
                while (PG1STATbits.UPDATE == 1);
                PWMRELEASE;
                PG1IOCONH |= 0x000C; // enable PWM
                v_ref = vout_adc;// + RAMPRATE;
//                i_loop.MaxOutput = DUTYRAMPRATE;
//                e_cfly = 0;
//                sum_e_cfly = 0;       
//                v_loop.status.bits.enable = 1;
//                i_loop.status.bits.enable = 1;
                duty_out = DUTYRAMPRATE;
//                v_loop_only.status.bits.enable = 1;    
//                v_loop_only_Precharge(&v_loop_only , RAMPRATE , 200 );
//                v_loop_Precharge(&v_loop , v_ref , IOUTADC(0.15) );
//                i_loop_Precharge(&i_loop , 0 , 0 );  
                delay = 0;                                
            }
//                if (ENABLEINPUT == 0) {
//                    systemstate = off;
//                }
            break;
            


        case softstart_duty_ramp:
            // input or output over voltage
            if ( (vin_adc > VINADC(OVLO_IN)) || (vout_adc > VOUTADC(OVLO_OUT)) ) {
                systemstate = latchoff;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;
            }       
            
            // output short circuit
            if ( (vout_adc < VOUTADC(VOUTSCPLIMIT)) && (iout_adc >= (IOUTADC(ILIMITACTUALAMPS) + ISENSEREF)) ) {
                systemstate = latchoff_sc;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;                
            }
            
            if (vin_adc < VINADC(UVLO_OFF)) {
                systemstate = off;
                while(PG1STATbits.UPDATE);
                PWMDISABLED;
                PWMHOLD;
                v_ref = 0;
                i_ref = 0;
                duty_out = 0;
                v_loop.status.bits.enable = 0;
                i_loop.status.bits.enable = 0;      
                fault_regulation.status.bits.enabled = false; // Enable fault checks                    
                break;
            }               
//            LATC &= ~(1 << 2);
            if (statechanged == 1) {
                statechanged = 0;
//                    Iout_PID.controlReference = IOUTADC(ILIMITACTUALAMPS);
            }
            if (newadcavailable == 1) {
                newadcavailable = 0;
                newadcavailable = 0;
                duty_out += DUTYRAMPRATE;
            }
//            if (v_ref > VOUTADC(5)) {
//                CMP2CONbits.CMPON = 1;
//            }
//            if (duty_out > DUTYRAMPTH) {
//                systemstate = latchoff_sc;
//                while(PG1STATbits.UPDATE);
//                //disable PWM
//                PG1IOCONHbits.PENH = 0; 
//                PG1IOCONHbits.PENL = 0;                  
//                break;                     
//            }
            if ( (vout_adc > VOUTADC(4)) || (duty_out > DUTYRAMPTH) )  {
                v_ref = vout_adc + RAMPRATE;
                systemstate = softstart;
                v_loop_Precharge(&v_loop , 0 , iout_adc-ISENSEOFFSET );
                i_loop_Precharge(&i_loop , 0 , duty_out );
                v_loop.status.bits.enable = 1;
                i_loop.status.bits.enable = 1;      
                fault_regulation.status.bits.enabled = true; // Enable fault checks    
//                LATB &= ~(1 << 5);
//                LATB |= (1 << 3);
                delay = 0;
            }
//                if (ENABLEINPUT == 0)//|| (vin_adc < VINADC(UVLO_OFF))
//                       // || (vin_adc > VINADC(OVLO_OFF))) 
//                {
//                    systemstate = off;
//                }
            break;
            
        case softstart:
            // input or output over voltage
            if ( (vin_adc > VINADC(OVLO_IN)) || (vout_adc > VOUTADC(OVLO_OUT)) ) {
                systemstate = latchoff;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;
            }       
            
            // output short circuit
            if ( (vout_adc < VOUTADC(VOUTSCPLIMIT)) && (iout_adc >= (IOUTADC(ILIMITACTUALAMPS) + ISENSEREF)) ) {
                systemstate = latchoff_sc;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;                
            }
            
            if (vin_adc < VINADC(UVLO_OFF)) {
                systemstate = off;
                while(PG1STATbits.UPDATE);
                PWMDISABLED;
                PWMHOLD;
                v_ref = 0;
                i_ref = 0;
                duty_out = 0;
                v_loop.status.bits.enable = 0;
                i_loop.status.bits.enable = 0;      
                fault_regulation.status.bits.enabled = false; // Enable fault checks                    
                break;
            }               
            
            fres = fault_check(&fault_ocp);
            if (fres == 10) {
                systemstate = latchoff_ocp;
                break;
            }
//            fres = fault_check(&fault_regulation);
//            if (fres == 10) {
//                systemstate = latchoff_ocp;
//                break;
//            }               
//            LATC &= ~(1 << 2);
            if (statechanged == 1) {
                statechanged = 0;
//                    Iout_PID.controlReference = IOUTADC(ILIMITACTUALAMPS);
            }
            if (newadcavailable == 1) {
                newadcavailable = 0;
                newadcavailable = 0;
                v_ref += RAMPRATE;
//                if ( vout_adc < VOUTADC(4) ) {
//                    duty_out += DUTYRAMPRATE;
////                    i_loop.MaxOutput += DUTYRAMPRATE;
//                } else {
//                    i_loop.MaxOutput = DUTYMAX;
//                }
            }
//            if (v_ref > VOUTADC(5)) {
//                CMP2CONbits.CMPON = 1;
//            }
            if (v_ref >= VOUTADC(VOUTACTUALVOLTS)) {
                v_ref = VOUTADC(VOUTACTUALVOLTS);
                systemstate = running;
//                LATB &= ~(1 << 5);
//                LATB |= (1 << 3);
                delay = 0;
            }
//                if (ENABLEINPUT == 0)//|| (vin_adc < VINADC(UVLO_OFF))
//                       // || (vin_adc > VINADC(OVLO_OFF))) 
//                {
//                    systemstate = off;
//                }
            break;            

        case running:
//                LATC = 1 << 7;
            statechanged = 0;
            // input or output over voltage
            if ( (vin_adc > VINADC(OVLO_IN)) || (vout_adc > VOUTADC(OVLO_OUT)) ) {
                systemstate = latchoff;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;
            }        

            if ( iout_adc >= (IOUTADC(ILIMITACTUALAMPS)+ISENSEREF) ) {
                systemstate = latchoff_sc;
                while(PG1STATbits.UPDATE);
                //disable PWM
                PG1IOCONHbits.PENH = 0; 
                PG1IOCONHbits.PENL = 0;                  
                break;                
            }
            
            fres = fault_check(&fault_ocp);
            if (fres == 10) {
                systemstate = latchoff_ocp;
                break;
            }
            fres = fault_check(&fault_regulation);
            if (fres == 10) {
                systemstate = latchoff_ocp;
                break;
            }    
            
//            fres = fault_check(&fault_temperature);
//            if (fres == 10) {
//                systemstate = latchoff_ocp;
//                break;
//            }               
                        
            if (vin_adc < VINADC(UVLO_OFF)) {
                systemstate = off;
                while(PG1STATbits.UPDATE);
                PWMDISABLED;
                PWMHOLD;
                v_ref = 0;
                i_ref = 0;
                duty_out = 0;
                v_loop.status.bits.enable = 0;
                i_loop.status.bits.enable = 0;      
                fault_regulation.status.bits.enabled = false; // Enable fault checks                    
                break;
            }               
            
            if (controlstate == lowpower) {
                counter_inactive++;
                counter_pd_max = counter_pd;
                counter_pd = 0;
            }
//                if (ENABLEINPUT == 0)//|| (vin_adc < VINADC(UVLO_OFF)) 
//                      //  || (vin_adc > VINADC(OVLO_OFF))) 
//                {
//                    systemstate = off;
//                }
//            if (newadcavailable == 1) {
////                    LATC = 1 << 7;
//                newadcavailable = 0;
//                newadcavailable = 0;
//
//            }
            break;
            
        case latchoff:
            
            break;
            
        case latchoff_sc:
            
            break;
            
        case latchoff_ocp:
            
            break;
            
        case undervoltage:
            if (  (vin_adc > VINADC(UVLO_ON)) && 
                  (vin_adc < VINADC(OVLO_IN)) && 
                  (vout_adc < VOUTADC(VOUTKEEPOFFATSTARTUP)) ) //&& 

            {
                systemstate = softstart_duty_ramp;
                while (PG1STATbits.UPDATE == 1);
                PWMRELEASE;
                PG1IOCONH |= 0x000C; // enable PWM
                v_ref = vout_adc;// + RAMPRATE;
                duty_out = DUTYRAMPRATE;
                delay = 0;       
                
            }
            break;

        case error:
            break;
    }
    
    
    return (1);
}

inline void CurrentSenseCalibration(void) {
    // Current Calibration Procedure
//    if ((buck.mode != BUCK_STATE_STANDBY) || 
//        (buck.status.bits.cs_calib_complete) || 
//        (!buck.status.bits.adc_active)
//       )
//    { return; }
        
        
    if (++calib_cs1.cs_calib_cnt < CS_CALIB_STEPS)
    {
        calib_cs1.cs_calib_offset += iout_adc; // Read ADC offset value
    }
    else
    {
        calib_cs1.cs_calib_offset += iout_adc; // Read ADC offset value

        cs_calib_offset = (calib_cs1.cs_calib_offset >> 3);             // Divide accumulated ADC samples (calculate average)
        cs_calib_complete = true;
    }

    return;
}
