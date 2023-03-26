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

typedef union {
    uint32_t value;
    struct {
        uint16_t l;
        uint16_t h;
    };
} uint16_32_t;

static uint16_32_t rtc_systime; // in 1/1024 seconds
static inline void rtc_UpdateSystime(void) {
    // save SR
    rtc_systime.l = rtc_GetCNT();
    // restore SR
}

typedef struct {
    uint16_32_t starttime; // in 1/1024 seconds
    uint16_32_t duration; // in 1/1024 seconds
    uint16_32_t compare; // in 1/1024 seconds
    uint16_t timeout_s;
    uint8_t status;     // use rtc_state_t
    uint8_t led_nb;
    uint8_t event;
} rtc_timer_t;

static rtc_timer_t rtc_timers[NB_RTC_TIMER];
static volatile uint8_t rtc_timer_process_list[NB_RTC_TIMER+1]; // store timer_nb to reference in rtc_timers
#define RTC_PROCESS_TIMER_LIST_SIZE (sizeof(rtc_timer_process_list)/sizeof(rtc_timer_process_list[0]))
#define RTC_PROCESS_TIMER_UNUSED 0xFF

static inline void rtc_UpdateDurationOfRTCTimer(uint8_t timer_nb, uint32_t now) {
    rtc_timers[timer_nb].starttime.value = rtc_systime.value;   // now
    rtc_timers[timer_nb].duration.value = rtc_timers[timer_nb].compare.value - rtc_timers[timer_nb].starttime.value;  // duration = time left = end - now
}

// insert timer_nb at rtc_timer_process_list[pos]
// copy all elements from pos 1 up
// ignore RTC_PROCESS_TIMER_UNUSED for purpose of simplicity
static void rtc_InsertTimerInTimerProcessListAtPosition(uint8_t timer_nb, uint8_t pos) {
    uint8_t n, temp;
    for(n = pos; n < RTC_PROCESS_TIMER_LIST_SIZE; n++) {
        temp = rtc_timer_process_list[n];
        rtc_timer_process_list[n] = timer_nb;
        timer_nb = temp;
    }
}

// sort in by calculated duration?, smallest/next at the front
static uint8_t rtc_SortTimerInTimerProcessListByDuration(uint8_t timer_nb) {
    uint8_t n;
    rtc_UpdateSystime();
    // check if there is any space left in rtc_timer_process_list
    // update duration in rtc_timer_process_list
    for(n = 0; n < RTC_PROCESS_TIMER_LIST_SIZE; n++) {
        if(rtc_timer_process_list[n] == RTC_PROCESS_TIMER_UNUSED) {
            // found end of list
            break;
        }
        rtc_UpdateDurationOfRTCTimer(rtc_timer_process_list[n], rtc_systime.value);
    }
    // sort new timer in by its duration.value
    for(n = 0; n < RTC_PROCESS_TIMER_LIST_SIZE; n++) {
        if(rtc_timer_process_list[n] == RTC_PROCESS_TIMER_UNUSED) {
            // this place in unused, use this one
            rtc_timer_process_list[n] = timer_nb;
            // done
            break;
        }
        if(rtc_timers[timer_nb].duration.value < rtc_timers[rtc_timer_process_list[n]].duration.value) {
            // insert timer_nb at this point
            rtc_InsertTimerInTimerProcessListAtPosition(timer_nb, n);
            // done
            break;
        }
    }
    return 0;
}

#define rtc_PeekTimerProcessList() rtc_timer_process_list[0];

// pop 1st, copy all others from back ot front, [0] = front/head, fill in UNUSED at the back
static uint8_t rtc_RemoveHeadFromTimerProcessList(void) {
    uint8_t ans, n;
    ans = rtc_timer_process_list[0];
    for(n = 0; n < (RTC_PROCESS_TIMER_LIST_SIZE-1); n++) {
        rtc_timer_process_list[n] = rtc_timer_process_list[n+1];
    }
    rtc_timer_process_list[n] = RTC_PROCESS_TIMER_UNUSED;
    return ans;
}

static uint8_t rtc_RemoveGivenTimerNbFromTimerProcessList(uint8_t timer_nb) {
    uint8_t ans, n, pos;
    // find pos of timer_nb
    pos = RTC_PROCESS_TIMER_LIST_SIZE;
    for(n = 0; n < RTC_PROCESS_TIMER_LIST_SIZE; n++) {
        if(rtc_timer_process_list[n] == timer_nb) {
            // found, save in pos
            pos = n;
        }
    }
    if(pos == RTC_PROCESS_TIMER_LIST_SIZE) {
        // not found, so do not remove
        return 0;
    }
    ans = rtc_timer_process_list[pos];
    for(n = pos; n < (RTC_PROCESS_TIMER_LIST_SIZE-1); n++) {
        rtc_timer_process_list[n] = rtc_timer_process_list[n+1];
    }
    rtc_timer_process_list[n] = RTC_PROCESS_TIMER_UNUSED;
    return ans;
}

static void rtc_InitTimerProcessList(void) {
    uint8_t n;
    for(n = 0; n < RTC_PROCESS_TIMER_LIST_SIZE; n++) {
        rtc_timer_process_list[n] = RTC_PROCESS_TIMER_UNUSED;
    }
}

ISR(RTC_CNT_vect) {
    if(RTC.INTFLAGS & 0x01) {
        // OVF
        RTC.INTFLAGS = 1;   // clr
    }
    if(RTC.INTFLAGS & 0x02) {
        // CMP
        RTC.INTFLAGS = 2;   // clr
    }
}

void rtc_Init(void) {
	uint8_t n;
    for(n = 0; n < NB_TTIMER; n++) {
        rtc_timers[n].status = RTC_ST_OFF;
    }
    // The PIT will continue to operate in any sleep mode.
    pwr_ClaimMode(PWR_POWER_DOWN);
}

void rtc_StartModule(void) {
    RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_RTCEN_bm;	// RUN in standby, prescaler = 1
	RTC.CLKSEL = RTC_CLKSEL0_bm;	// 1024 Hz
    RTC.CNT = 0;
    RTC.PER = 0xFFFF; // maximum -> free running
    RTC.CMP = 32678/2*3;
    RTC.INTFLAGS = RTC_INTFLAGS;
    RTC.INTCTRL = 3;
    RTC.CTRLA |= RTC_RTCEN_bm; // run
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

void rtc_StartSingleTimeout(uint8_t timer_nb, uint16_t duration_s, uint8_t event) {
    rtc_timers[timer_nb].led_nb = timer_nb;
    rtc_timers[timer_nb].timeout_s = duration_s;
    rtc_timers[timer_nb].event = event;

    rtc_UpdateSystime();
    rtc_timers[timer_nb].starttime.value = rtc_systime.value;   // now
    rtc_timers[timer_nb].duration.value = (uint32_t)duration_s * 1024;
    rtc_timers[timer_nb].compare.value = rtc_timers[timer_nb].starttime.value + rtc_timers[timer_nb].duration.value;
    rtc_timers[timer_nb].status = RTC_ST_RUNNING;

    rtc_SortTimerInTimerProcessListByDuration(timer_nb);
    return;
}

void rtc_StopTimeout(uint8_t timer_nb) {
    rtc_timers[timer_nb].status = RTC_ST_OFF;
    rtc_RemoveGivenTimerNbFromTimerProcessList(timer_nb);
}

void rtc_Pause(uint8_t timer_nb) {
    rtc_timers[timer_nb].status = RTC_ST_PAUSE;
    rtc_UpdateSystime();
    rtc_UpdateDurationOfRTCTimer(timer_nb, rtc_systime.value);
    rtc_RemoveGivenTimerNbFromTimerProcessList(timer_nb);
}

void rtc_Resume(uint8_t timer_nb) {
    if(rtc_timers[timer_nb].status != RTC_ST_PAUSE) {
        // error, cannot resume
        return;
    }
    rtc_UpdateSystime();
    rtc_timers[timer_nb].starttime.value = rtc_systime.value;   // now
    rtc_timers[timer_nb].compare.value = rtc_timers[timer_nb].starttime.value + rtc_timers[timer_nb].duration.value;
    rtc_timers[timer_nb].status = RTC_ST_RUNNING;
    rtc_SortTimerInTimerProcessListByDuration(timer_nb);
}
