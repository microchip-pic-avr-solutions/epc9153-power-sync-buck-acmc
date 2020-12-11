#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "globals.h"

volatile uint16_t ConfigureFaultHandlers(void) {
    
    fault_ocp.source_obj = &iout_adc;   // Set pointer to variable to monitor
    fault_ocp.ref_obj = NULL;      // Clear pointer to "compare against" variable 
    fault_ocp.bit_mask = 0xFFFF;  // Compare all bits of SOURCE (no bit filter)
    
    fault_ocp.status.bits.type = FLTCMP_GREATER_THAN; // Select Compare-Type
    fault_ocp.counter = 0;        // Clear fault event counter
    fault_ocp.trip_level = IOUTADC(ILIMITOCP)+ISENSEOFFSET;   // Set fault trip level
    fault_ocp.tripcnt_max = 350;    // Set counter level at which a FAULT condition will be tripped
    fault_ocp.reset_level = IOUTADC(IOUTNORM)+ISENSEOFFSET;   // Set fault reset level
    fault_ocp.rstcnt_max = 300;     // Set counter level at which a FAULT condition will be cleared
    
    // User-function declarations
    fault_ocp.trip_response = &ocp_trip_response; // Set pointer to user-function which should be called when a FAULT is tripped
    fault_ocp.reset_response = &ocp_reset_response; // Set pointer to user-function which should be called when a FAULT is cleared
    
    fault_ocp.status.bits.enabled = true; // Enable fault checks
    
    // Regulation error
    
    fault_regulation.source_obj = &vout_adc;   // Set pointer to variable to monitor
    fault_regulation.ref_obj = &v_ref;      // Clear pointer to "compare against" variable 
    fault_regulation.bit_mask = 0xFFFF;  // Compare all bits of SOURCE (no bit filter)
    
    fault_regulation.status.bits.type = FLTCMP_GREATER_THAN; // Select Compare-Type
    fault_regulation.counter = 0;        // Clear fault event counter
    fault_regulation.trip_level = VOUTADC(3.5);   // Set fault trip level
    fault_regulation.tripcnt_max = 1;    // Set counter level at which a FAULT condition will be tripped
    fault_regulation.reset_level = VOUTADC(0.5);   // Set fault reset level
    fault_regulation.rstcnt_max = 500;     // Set counter level at which a FAULT condition will be cleared
    
    // User-function declarations
    fault_regulation.trip_response = &ocp_trip_response; // Set pointer to user-function which should be called when a FAULT is tripped
    fault_regulation.reset_response = &ocp_reset_response; // Set pointer to user-function which should be called when a FAULT is cleared
    
    fault_regulation.status.bits.enabled = false; // dont't enable fault checks yet (enable in main state machine) 
    
    // Over temperature error
    
    fault_temperature.source_obj = &temperature_adc;   // Set pointer to variable to monitor
    fault_temperature.ref_obj = NULL;      // Clear pointer to "compare against" variable 
    fault_temperature.bit_mask = 0xFFFF;  // Compare all bits of SOURCE (no bit filter)
    
    fault_temperature.status.bits.type = FLTCMP_LESS_THAN; // Select Compare-Type
    fault_temperature.counter = 0;        // Clear fault event counter
    fault_temperature.trip_level = TEMPERATURESD;   // Set fault trip level
    fault_temperature.tripcnt_max = 50;    // Set counter level at which a FAULT condition will be tripped
    fault_temperature.reset_level = TEMPERATUREON;   // Set fault reset level
    fault_temperature.rstcnt_max = 300;     // Set counter level at which a FAULT condition will be cleared
    
    // User-function declarations
    fault_temperature.trip_response = &ocp_trip_response; // Set pointer to user-function which should be called when a FAULT is tripped
    fault_temperature.reset_response = &ocp_reset_response; // Set pointer to user-function which should be called when a FAULT is cleared
    
    fault_temperature.status.bits.enabled = true; // Enable fault checks    
    
    return (1);
}


