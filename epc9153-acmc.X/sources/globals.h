
#ifndef SYSTEM_GLOBALS_H
#define SYSTEM_GLOBALS_H

/******************************************************************************/
/* System Level #include's                                                    */
/******************************************************************************/
#include "user.h"
#include "fault_handler.h"
#include "npnz16b.h"
#include "v_loop.h"
#include "i_loop.h"
#include "v_loop_only.h"

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

extern volatile uint16_t vin_adc, vout_adc, iout_adc, v_ref, i_ref, duty_out, v_cfly, v_cfly_ref, deltaD_offset, i_loop2_out;
extern volatile uint16_t i_trigger, v_trigger;
extern volatile uint16_t temperature_adc;
extern volatile int16_t vcfly[2];
extern volatile int16_t v_loop_POSB_load, v_loop_POSB_noload;

extern volatile int16_t e_cfly, sum_e_cfly, e_neg, e_pos, e20_n1, e20_n2;
extern volatile uint16_t v_loop_error, v_loop_only_error;
extern volatile uint16_t max_counter, max_hc, max_iout;
extern volatile int16_t deltaDQ2;
//volatile int16_t maxdeltaDpos, maxdeltaDneg;
extern volatile int16_t systemstate, systemstateprevious, controlstate;
extern volatile int16_t statechanged;
extern volatile int16_t newadcavailable;
extern volatile int16_t numberofphases;
extern volatile int16_t mscounter;
extern volatile int16_t deltaD;
extern volatile int16_t rx_count;
extern volatile int16_t tx_count, low_iout_count;
extern volatile int16_t pwm1_count, hc_count;
extern volatile int16_t counter_inactive, counter_force, counter_pd, counter_pd_max;
extern volatile uint8_t command_int8;
extern volatile int16_t deltaDmax;
extern volatile uint16_t cs_calib_offset;
extern volatile cNPNZ16b_t v_loop, i_loop, v_loop_only;

extern volatile FAULT_OBJECT_t fault_ocp;
extern volatile FAULT_OBJECT_t fault_regulation;
extern volatile FAULT_OBJECT_t fault_temperature;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/




#endif // end of SYSTEM_GLOBALS_H
