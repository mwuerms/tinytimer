/**
 * Martin Egli
 * 2023-03-21
 * module tiny timer
 */

#include <util/atomic.h>
#include "ttimer.h"
#include "rtc.h"
#include "leds.h"

#define min_in_s(m)	(m * 60)

static const uint16_t timeout_sec_const[NB_TTIMER] = {
	min_in_s(3),
	min_in_s(5),
	min_in_s(10),
	min_in_s(25),
};

static const uint16_t alarm_duration = 10;

// states of ttimer
typedef enum {
    TT_ST_OFF = 0,
    TT_ST_RUNNING,
    TT_ST_PAUSE,
    TT_ST_ALARM
} ttimer_state_t;

typedef struct {
    uint8_t state;
    uint16_t timeout_sec;
} ttimer_ctrl_t;

static ttimer_ctrl_t ttimer_ctrl[NB_TTIMER];

void ttimer_Init(void) {
	uint8_t n;
	// init vars
	for(n = 0; n < NB_TTIMER; n++) {
		ttimer_ctrl[n].state = TT_ST_OFF;
		ttimer_ctrl[n].timeout_sec = timeout_sec_const[n];
	}
	
}

void ttimer_ProcessEvents(uint8_t ttimer_nb, uint8_t events) {
	/*if(ttimer_IsNumberValid(ttimer_nb) == false) {
		return;
	}*/
	switch(ttimer_ctrl[ttimer_nb].state) {
		case TT_ST_OFF:
			if(events & (TT_EV_BUTTON_SHORT_PRESSED | TT_EV_BUTTON_1s_LONG_PRESSED)) {
				// starting
				ttimer_ctrl[ttimer_nb].state = TT_ST_RUNNING;
				// duration and stuff ttimer_ctrl[ttimer_nb].
				rtc_StartSingleTimeout(ttimer_nb, ttimer_ctrl[ttimer_nb].timeout_sec);
				leds_ShowRunning(ttimer_nb);
			}
			else if(events & TT_EV_BUTTON_2s_LONG_PRESSED) {
				// stop
				ttimer_ctrl[ttimer_nb].state = TT_ST_OFF;
				rtc_StopTimeout(ttimer_nb);
				leds_Off(ttimer_nb);
			}
			break;

		case TT_ST_RUNNING:
			if(events & TT_EV_TIMEOUT) {
				// alarm
				ttimer_ctrl[ttimer_nb].state = TT_ST_ALARM;
				// add timeout to clear alarm
				rtc_StartSingleTimeout(ttimer_nb, alarm_duration);
				leds_ShowAlarm(ttimer_nb);
			}
			else if(events & TT_EV_BUTTON_2s_LONG_PRESSED) {
				// stop
				ttimer_ctrl[ttimer_nb].state = TT_ST_OFF;
				rtc_StopTimeout(ttimer_nb);
				leds_Off(ttimer_nb);
			}
			else if(events & TT_EV_BUTTON_SHORT_PRESSED) {
				// pause here
				ttimer_ctrl[ttimer_nb].state = TT_ST_PAUSE;
				rtc_Pause(ttimer_nb);
				leds_ShowPause(ttimer_nb);
			}
			break;
		case TT_ST_PAUSE:
			if(events & TT_EV_BUTTON_2s_LONG_PRESSED) {
				// stop
				ttimer_ctrl[ttimer_nb].state = TT_ST_OFF;
				rtc_StopTimeout(ttimer_nb);
				leds_Off(ttimer_nb);
			}
			else if(events & TT_EV_BUTTON_SHORT_PRESSED) {
				// resume
				ttimer_ctrl[ttimer_nb].state = TT_ST_RUNNING;
				rtc_Resume(ttimer_nb);
				leds_ShowRunning(ttimer_nb);
			}
			break;
		case TT_ST_ALARM:
			if(events & (TT_EV_TIMEOUT | TT_EV_BUTTON_SHORT_PRESSED)) {
				// stop
				ttimer_ctrl[ttimer_nb].state = TT_ST_OFF;
				leds_Off(ttimer_nb);
			}
			break;
	}
	return;
}
