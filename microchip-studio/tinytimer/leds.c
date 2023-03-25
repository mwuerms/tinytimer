/**
 * Martin Egli
 * 2023-03-21
 * module timer
 */
#include "io.h"
#include "leds.h"
#include "ttimer.h"
#include <avr/interrupt.h>

static const uint8_t sin_lookup[] = {0, 9, 37, 79, 128, 176, 218, 246, 255, 246, 218, 176, 128, 79, 37, 9};
#define SIN_LOOKUP_SIZE	(sizeof(sin_lookup)/sizeof(sin_lookup[0]))
//#define SIN_LOOKUP_MAX	(255)
static const uint8_t glow_sin_lookup[] = {0, 8, 16, 24, 32, 40, 47, 54, 60, 65, 70, 75, 78, 81, 83, 84, 85, 84, 83, 81, 78, 75, 70, 65, 60, 54, 47, 40, 32, 24, 16, 8};
#define GLOW_SIN_LOOKUP_SIZE	(sizeof(glow_sin_lookup)/sizeof(glow_sin_lookup[0]))

typedef enum {
	LED_ST_OFF = 0,
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

#define LED_RUNNING_DELAY_MAX	(4)
#define LED_ALARM_DELAY_MAX	(2)

static led_ctrl_t led_ctrl[NB_TTIMER];

inline void leds_Set(uint8_t ttimer_nb) {
	switch(ttimer_nb) {
		case 0:	LED1_PORT.OUTSET = LED1_PIN_BV;	break;
		case 1:	LED2_PORT.OUTSET = LED2_PIN_BV;	break;
		case 2:	LED3_PORT.OUTSET = LED3_PIN_BV;	break;
		case 3:	LED4_PORT.OUTSET = LED4_PIN_BV;	break;
	}
}

inline void leds_Clr(uint8_t ttimer_nb) {
	switch(ttimer_nb) {
		case 0:	LED1_PORT.OUTCLR = LED1_PIN_BV;	break;
		case 1:	LED2_PORT.OUTCLR = LED2_PIN_BV;	break;
		case 2:	LED3_PORT.OUTCLR = LED3_PIN_BV;	break;
		case 3:	LED4_PORT.OUTCLR = LED4_PIN_BV;	break;
	}
}

inline void leds_Tgl(uint8_t ttimer_nb) {
	switch(ttimer_nb) {
		case 0:	LED1_PORT.OUTTGL = LED1_PIN_BV;	break;
		case 1:	LED2_PORT.OUTTGL = LED2_PIN_BV;	break;
		case 2:	LED3_PORT.OUTTGL = LED3_PIN_BV;	break;
		case 3:	LED4_PORT.OUTTGL = LED4_PIN_BV;	break;
	}
}

void leds_Display(uint8_t ttimer_nb) {
	switch(led_ctrl[ttimer_nb].state) {
		case LED_ST_RUNNING:
			led_ctrl[ttimer_nb].cnt++;
			if(led_ctrl[ttimer_nb].cnt < glow_sin_lookup[led_ctrl[ttimer_nb].step]) {
				leds_Set(ttimer_nb);
			}
			else {
				leds_Clr(ttimer_nb);	
			}
			if(led_ctrl[ttimer_nb].cnt == 0) {
				led_ctrl[ttimer_nb].delay++;
				if(led_ctrl[ttimer_nb].delay == LED_RUNNING_DELAY_MAX) {
					led_ctrl[ttimer_nb].delay = 0;
					// ++ step after delay
					led_ctrl[ttimer_nb].step++;
					if(led_ctrl[ttimer_nb].step >= GLOW_SIN_LOOKUP_SIZE)
						led_ctrl[ttimer_nb].step = 0;
				}
			}
			break;
		case LED_ST_PAUSE:
			// for a start, code pause later
			leds_Clr(ttimer_nb);
			break;
		case LED_ST_ALARM:
			led_ctrl[ttimer_nb].cnt++;
			if(led_ctrl[ttimer_nb].cnt < sin_lookup[led_ctrl[ttimer_nb].step]) {
				leds_Set(ttimer_nb);
			}
			else {
				leds_Clr(ttimer_nb);	
			}
			if(led_ctrl[ttimer_nb].cnt == 0) {
				led_ctrl[ttimer_nb].delay++;
				if(led_ctrl[ttimer_nb].delay == LED_ALARM_DELAY_MAX) {
					led_ctrl[ttimer_nb].delay = 0;
					led_ctrl[ttimer_nb].step++;
					if(led_ctrl[ttimer_nb].step >= SIN_LOOKUP_SIZE)
						led_ctrl[ttimer_nb].step = 0;
				}
			}
			break;
		case LED_ST_OFF:
		default:
			leds_Clr(ttimer_nb);
			break;
	}
}

static volatile uint8_t tcb0_cnt = 0;
static volatile uint8_t tcb0_step = 0;
static volatile uint8_t tcb0_delay = 0;
ISR(TCB0_INT_vect) {
	TCB0.INTFLAGS = TCB_CAPT_bm; //irq clear
	uint8_t n;
	// init vars
	for(n = 0; n < NB_TTIMER; n++) {
		leds_Display(n);
	}
	/* tested sinus glow: OK
	if(tcb0_cnt < glow_sin_lookup[tcb0_step]) {
		leds_Set(0);
	}
	else {
		leds_Clr(0);
	}
	tcb0_cnt++;
	if(tcb0_cnt == 0) {
		tcb0_delay++;
		if(tcb0_delay == 8) {
			// 2: alarm
			// 4: 1 s
			// 8: 2 s
			// 16: 4 s (glow)
			tcb0_delay = 0;
			tcb0_step++;
			if(tcb0_step == GLOW_SIN_LOOKUP_SIZE) {
				tcb0_step = 0;
			}
		}
	}*/
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

void leds_Off(uint8_t ttimer_nb) {
	led_ctrl[ttimer_nb].state = LED_ST_OFF;
	leds_Clr(ttimer_nb);
	return;
}

void leds_ShowRunning(uint8_t ttimer_nb) {
	led_ctrl[ttimer_nb].state = LED_ST_RUNNING;
	led_ctrl[ttimer_nb].step = 0;
	led_ctrl[ttimer_nb].cnt = 0;
	led_ctrl[ttimer_nb].delay = 0;
	leds_Set(ttimer_nb);
	return;
}

void leds_ShowPause(uint8_t ttimer_nb) {
	led_ctrl[ttimer_nb].state = LED_ST_PAUSE;
	return;
}

void leds_ShowAlarm(uint8_t ttimer_nb) {
	led_ctrl[ttimer_nb].state = LED_ST_ALARM;
	led_ctrl[ttimer_nb].step = 0;
	led_ctrl[ttimer_nb].cnt = 0;
	return;
}

