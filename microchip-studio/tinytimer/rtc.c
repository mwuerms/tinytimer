/**
 * Martin Egli
 * 2023-03-21
 * module tiny timer
 */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include "ttimer.h"
#include "pwr.h"
#include "leds.h"

typedef enum {
    RTC_ST_OFF = 0,
    RTC_ST_RUNNING,
    RTC_ST_PAUSE
} rtc_state_t;

typedef struct {
    uint8_t status;     // use rtc_state_t
    uint16_t timeout_s;
} rtc_ctrl_t;

static rtc_ctrl_t rtc_ctrl[NB_TTIMER];

// use PIT to generate a 1 s interrupt, this is accurate enough for this application
// use CNT to have a 1 ms timing
ISR(RTC_CNT_vect) {
    if(RTC.INTFLAGS & 0x01) {
        leds_On(LED1);
        leds_Off(LED1);
        leds_On(LED1);
        leds_Off(LED1);
    }
    if(RTC.INTFLAGS & 0x02) {
        leds_On(LED1);
        leds_Off(LED1);
        leds_On(LED1);
        leds_Off(LED1);
        leds_On(LED1);
        leds_Off(LED1);
        leds_On(LED1);
        leds_Off(LED1);
    }
    RTC.INTFLAGS = 3;
}
/*ISR(RTC_PIT_vect) {
    // testing
    leds_On(LED1);
    RTC.PITINTFLAGS = RTC_PI_bm; // clr
    _NOP();
	leds_Off(LED1);
}*/

void rtc_Init(void) {
	uint8_t n;
    for(n = 0; n < NB_TTIMER; n++) {
        rtc_ctrl[n].status = RTC_ST_OFF;
        rtc_ctrl[n].timeout_s = 0;
    }
    // The PIT will continue to operate in any sleep mode.
    pwr_ClaimMode(PWR_POWER_DOWN);
}

void rtc_StartModule(void) {
    leds_Off(LED1);
    leds_On(LED1);
    leds_Off(LED1);
    leds_On(LED1);
    RTC.CTRLA = 1; // run
    RTC.CTRLA = RTC_RUNSTDBY_bm;	// RUN in standby, prescaler = 1
	RTC.CLKSEL = 0; // 32 kHz RTC_CLKSEL0_bm;	// 1024 Hz
    RTC.CNT = 0;
    RTC.PER = 0xFFFF; // maximum -> free running
    RTC.CMP = 32678/2*3;
    RTC.INTFLAGS = 3;
    RTC.INTCTRL = 3;
    RTC.CTRLA |= 1; // run
    //RTC.PITCTRLA = (RTC_PERIOD3_bm | RTC_PERIOD0_bm | RTC_PITEN_bm);     // CYC1024 = 9 = 0b1001
    //RTC.PITINTFLAGS = RTC_PI_bm; // clr
	//RTC.PITINTCTRL = RTC_PI_bm; // enable INT
    //RTC.CTRLA |= RTC_RTCEN_bm;
}

void rtc_StopModule(void) {
    RTC.CTRLA = 0; // stop
    RTC.PITINTCTRL = 0;
}

uint16_t rtc_GetCNT(void) {
    uint16_t reg1, reg2;
    // synchronisation
    do {
        reg1 = RTC.CNT;
        reg2 = RTC.CNT;
        if(reg1 == reg2) {
            break;
        }
    } while(1);
    return reg1;
}




void rtc_StartSingleTimeout(uint8_t ttimer_nb, uint16_t duration_s) {
    return;
}

void rtc_StopTimeout(uint8_t ttimer_nb) {
    return;
}

void rtc_Pause(uint8_t ttimer_nb) {
    return;
}

void rtc_Resume(uint8_t ttimer_nb) {
    return;
}
