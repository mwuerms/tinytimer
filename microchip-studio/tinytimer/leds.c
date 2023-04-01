/**
 * Martin Egli
 * 2023-03-21
 * module timer
 */

#include <util/atomic.h>
#include "io.h"
#include "leds.h"
#include "ttimer.h"
#include <avr/interrupt.h>

#define NB_ELEMENTS(ptr)	(sizeof(ptr)/sizeof(ptr[0]))

static const uint8_t leds_alarm_lookup[] = {0, 9, 37, 79, 128, 176, 218, 246, 255, 246, 218, 176, 128, 79, 37, 9};
#define LEDS_ALARM_LOOKUP_SIZE	NB_ELEMENTS(leds_alarm_lookup)
#define LED_ALARM_DELAY_MAX	(2)

static const uint8_t leds_running_lookup[] = {0, 1, 6, 15, 26, 39, 54, 70, 85, 100, 113, 124, 133, 138, 140, 138, 133, 124, 113, 100, 85, 70, 54, 39, 26, 15, 6, 1, 0, 0, 0, 0};
#define LEDS_RUNNING_LOOKUP_SIZE	NB_ELEMENTS(leds_running_lookup)
#define LED_RUNNING_DELAY_MAX	(4*2)

typedef enum {
	LED_ST_OFF = 0,
	LED_ST_ON,
	LED_ST_RUNNING,
	LED_ST_PAUSE,
	LED_ST_ALARM
} led_states_t;

typedef struct {
	uint8_t state;	// led_states_t
	uint8_t step;
	uint8_t cnt;
	uint8_t delay;
} led_ctrl_t;
static led_ctrl_t led_ctrl[NB_TTIMER];

inline void leds_Set(uint8_t led_nb) {
	switch(led_nb) {
		case 0:	LED1_PORT.OUTSET = LED1_PIN_BV;	break;
		case 1:	LED2_PORT.OUTSET = LED2_PIN_BV;	break;
		case 2:	LED3_PORT.OUTSET = LED3_PIN_BV;	break;
		case 3:	LED4_PORT.OUTSET = LED4_PIN_BV;	break;
	}
}

inline void leds_Clr(uint8_t led_nb) {
	switch(led_nb) {
		case 0:	LED1_PORT.OUTCLR = LED1_PIN_BV;	break;
		case 1:	LED2_PORT.OUTCLR = LED2_PIN_BV;	break;
		case 2:	LED3_PORT.OUTCLR = LED3_PIN_BV;	break;
		case 3:	LED4_PORT.OUTCLR = LED4_PIN_BV;	break;
	}
}

inline void leds_Tgl(uint8_t led_nb) {
	switch(led_nb) {
		case 0:	LED1_PORT.OUTTGL = LED1_PIN_BV;	break;
		case 1:	LED2_PORT.OUTTGL = LED2_PIN_BV;	break;
		case 2:	LED3_PORT.OUTTGL = LED3_PIN_BV;	break;
		case 3:	LED4_PORT.OUTTGL = LED4_PIN_BV;	break;
	}
}

void leds_Display(uint8_t led_nb) {
	switch(led_ctrl[led_nb].state) {
		case LED_ST_RUNNING:
			led_ctrl[led_nb].cnt++;
			if(led_ctrl[led_nb].cnt < leds_running_lookup[led_ctrl[led_nb].step]) {
				leds_Set(led_nb);
			}
			else {
				leds_Clr(led_nb);	
			}
			if(led_ctrl[led_nb].cnt == 128) {
				led_ctrl[led_nb].cnt = 0;
				led_ctrl[led_nb].delay++;
				if(led_ctrl[led_nb].delay == LED_RUNNING_DELAY_MAX) {
					led_ctrl[led_nb].delay = 0;
					// ++ step after delay
					led_ctrl[led_nb].step++;
					if(led_ctrl[led_nb].step == LEDS_RUNNING_LOOKUP_SIZE)
						led_ctrl[led_nb].step = 0;
				}
			}
			break;
		case LED_ST_PAUSE:
			// for a start, code pause later
			// blink 2x
			led_ctrl[led_nb].delay++;
			if(led_ctrl[led_nb].delay == 31) {
				led_ctrl[led_nb].delay = 0;
				// slow down, so 8192 Hz -> 256 Hz
				switch(led_ctrl[led_nb].step) {
					case 0:
						led_ctrl[led_nb].cnt++;
						if(led_ctrl[led_nb].cnt == 20) {
							led_ctrl[led_nb].cnt = 0;
							led_ctrl[led_nb].step = 1;
							leds_Clr(led_nb);
						}
						break;
					case 1:
						led_ctrl[led_nb].cnt++;
						if(led_ctrl[led_nb].cnt == 50) {
							led_ctrl[led_nb].cnt = 0;
							led_ctrl[led_nb].step = 2;
							leds_Set(led_nb);
						}
						break;
					case 2:
						led_ctrl[led_nb].cnt++;
						if(led_ctrl[led_nb].cnt == 20) {
							led_ctrl[led_nb].cnt = 0;
							led_ctrl[led_nb].step = 3;
							leds_Clr(led_nb);
						}
						break;
					case 3:
						led_ctrl[led_nb].cnt++;
						if(led_ctrl[led_nb].cnt == 250) {
							led_ctrl[led_nb].cnt = 0;
							led_ctrl[led_nb].step = 0;
							leds_Set(led_nb);
						}
						break;
					default:
						led_ctrl[led_nb].cnt = 0;
						led_ctrl[led_nb].step = 0;
						leds_Clr(led_nb);
				}
			}
			break;
		case LED_ST_ALARM:
			led_ctrl[led_nb].cnt++;
			if(led_ctrl[led_nb].cnt < leds_alarm_lookup[led_ctrl[led_nb].step]) {
				leds_Set(led_nb);
			}
			else {
				leds_Clr(led_nb);
			}
			if(led_ctrl[led_nb].cnt == 0) {
				led_ctrl[led_nb].delay++;
				if(led_ctrl[led_nb].delay == LED_ALARM_DELAY_MAX) {
					led_ctrl[led_nb].delay = 0;
					led_ctrl[led_nb].step++;
					if(led_ctrl[led_nb].step == LEDS_ALARM_LOOKUP_SIZE)
						led_ctrl[led_nb].step = 0;
				}
			}
			break;
		// nodthing to do
		case LED_ST_OFF:
		case LED_ST_ON:
		default:
			break;
	}
}

static volatile uint8_t leds_cnt;
ISR(TCB0_INT_vect) {
	TCB0.INTFLAGS = TCB_CAPT_bm; //irq clear
	
	for(leds_cnt = 0; leds_cnt < NB_LEDS; leds_cnt++) {
		leds_Display(leds_cnt);
	}
}

static uint8_t tcb0_ctrl;
static void tcb0_start(void) {
	if(tcb0_ctrl) {
		// already running
		return;
	}
	tcb0_ctrl = 1;
	if((FUSE.OSCCFG & FUSE_FREQSEL_gm) == 0x01) {
		// 16MHz (main): CLK_PER: 2.33MHz/2
		TCB0.CCMP = (16000000UL/6/2/(4096*2));
	}
	else {
		// 20MHz (main): CLK_PER: 3.3MHz/2
		TCB0.CCMP = (20000000UL/6/2/(4096*2));
	}
	TCB0.INTFLAGS = TCB_CAPT_bm; //irq clear
	TCB0.INTCTRL = TCB_CAPT_bm; //irq enable
	TCB0.CTRLB = TCB_CNTMODE_INT_gc;
	TCB0.CTRLA = TCB_ENABLE_bm; //enable, div1
}

void leds_Init(void) {
	uint8_t n;
	// init vars
	tcb0_ctrl = 0;
	for(n = 0; n < NB_TTIMER; n++) {
		led_ctrl[n].state = LED_ST_OFF;
		led_ctrl[n].step = 0;
		led_ctrl[n].cnt = 0;
		led_ctrl[n].delay = 0;
	}
	// init ios
	LED1_PORT.OUTCLR = LED1_PIN_BV;
	LED1_PORT.DIRSET = LED1_PIN_BV;
	LED2_PORT.OUTCLR = LED2_PIN_BV;
	LED2_PORT.DIRSET = LED2_PIN_BV;
	LED3_PORT.OUTCLR = LED3_PIN_BV;
	LED3_PORT.DIRSET = LED3_PIN_BV;
	LED4_PORT.OUTCLR = LED4_PIN_BV;
	LED4_PORT.DIRSET = LED4_PIN_BV;

	tcb0_start();
}

void leds_On(uint8_t led_nb) {
	led_ctrl[led_nb].state = LED_ST_ON;
	leds_Set(led_nb);
	return;
}

void leds_Off(uint8_t led_nb) {
	led_ctrl[led_nb].state = LED_ST_OFF;
	leds_Clr(led_nb);
	return;
}

void leds_ShowRunning(uint8_t led_nb) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		led_ctrl[led_nb].state = LED_ST_RUNNING;
		led_ctrl[led_nb].step = 0;
		led_ctrl[led_nb].cnt = 0;
		led_ctrl[led_nb].delay = 0;
		leds_Clr(led_nb);
	}
	return;
}

void leds_ShowPause(uint8_t led_nb) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		led_ctrl[led_nb].state = LED_ST_PAUSE;
		leds_Set(led_nb);
	}
	return;
}

void leds_ShowAlarm(uint8_t led_nb) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		led_ctrl[led_nb].state = LED_ST_ALARM;
		led_ctrl[led_nb].step = 0;
		led_ctrl[led_nb].cnt = 0;
		led_ctrl[led_nb].delay = 0;
		leds_Clr(led_nb);
	}
	return;
}
