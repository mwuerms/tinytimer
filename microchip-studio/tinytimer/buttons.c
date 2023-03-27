/**
 * Martin Egli
 * 2023-03-21
 * module timer
 */

#include <avr/interrupt.h>
#include "io.h"
#include "buttons.h"
#include "rtc.h"

static inline uint8_t button_GetIN(uint8_t button_nb) {
	switch(button_nb) {
		case BUTTON1: 
			if(BUTTON1_PORT.IN & BUTTON1_PIN_BV)
				return 1;
			break;
		case BUTTON2: 
			if(BUTTON2_PORT.IN & BUTTON2_PIN_BV)
				return 1;
			break;
		case BUTTON3: 
			if(BUTTON3_PORT.IN & BUTTON3_PIN_BV)
				return 1;
			break;
		case BUTTON4: 
			if(BUTTON4_PORT.IN & BUTTON4_PIN_BV)
				return 1;
			break;
	}
	return 0;
}

static uint16_t button_rtc_cnt_pressed[NB_BUTTONS];
//static uint16_t button_rtc_cnt_released[NB_BUTTONS];

#define RTC_CNT_100ms	(uint16_t)(0.1*1024)  // 0.1s * 1024
#define RTC_CNT_1s		(1*1024) // 1.0s * 1024
#define RTC_CNT_2s		(2*1024) // 2.0s * 1024
static void button_ProcessInt(uint8_t button_nb) {
	uint8_t  button_value = button_GetIN(button_nb);
	uint16_t rtc_cnt_value = rtc_GetCNT();
	if(button_value) {
		// rising edge: released
		if(rtc_cnt_value > button_rtc_cnt_pressed[button_nb]) {
			rtc_cnt_value -= button_rtc_cnt_pressed[button_nb];
		}
		else {
			button_rtc_cnt_pressed[button_nb] -= rtc_cnt_value;
			rtc_cnt_value = button_rtc_cnt_pressed[button_nb];
		}
		if(rtc_cnt_value < RTC_CNT_100ms) {
			// too short, nothing to do
			return;
		}
		else if(rtc_cnt_value < RTC_CNT_1s) {
			ttimer_ProcessEvents(button_nb, TT_EV_BUTTON_SHORT_PRESSED);
		}
		else if(rtc_cnt_value < RTC_CNT_2s) {
			ttimer_ProcessEvents(button_nb, TT_EV_BUTTON_1s_LONG_PRESSED);
		}
		else {
			ttimer_ProcessEvents(button_nb, TT_EV_BUTTON_2s_LONG_PRESSED);
		}
	}
	else {
		// falling edge: pressed
		button_rtc_cnt_pressed[button_nb] = rtc_cnt_value; // save to evaluate released
	}
}


ISR(PORTA_PORT_vect) {
	if(BUTTON1_PORT.INTFLAGS & BUTTON1_PIN_BV) {
		BUTTON1_PORT.INTFLAGS |= BUTTON1_PIN_BV; // clr
		button_ProcessInt(BUTTON1);
	}
	if(BUTTON2_PORT.INTFLAGS & BUTTON2_PIN_BV) {
		BUTTON2_PORT.INTFLAGS |= BUTTON2_PIN_BV; // clr
		button_ProcessInt(BUTTON2);
	}
}

ISR(PORTB_PORT_vect) {
	if(BUTTON3_PORT.INTFLAGS & BUTTON3_PIN_BV) {
		BUTTON3_PORT.INTFLAGS |= BUTTON3_PIN_BV; // clr
		button_ProcessInt(BUTTON3);
	}
	if(BUTTON4_PORT.INTFLAGS & BUTTON4_PIN_BV) {
		BUTTON4_PORT.INTFLAGS |= BUTTON4_PIN_BV; // clr
		button_ProcessInt(BUTTON4);
	}
}

/* all buttons
 * input
 * pullup
 * interrupt on both edges
 * button pressed =  low,  falling edge
 * button released = high, rising edge
 */
#define PORT_INT_DISABLE	0
#define PORT_INT_BOTH_EDGES	1
void buttons_Init(void) {
	BUTTON1_PORT.DIRCLR = BUTTON1_PIN_BV;	// input
	BUTTON1_PORT.BUTTON1_PIN_CTRL = PORT_PULLUPEN_bm | PORT_INT_BOTH_EDGES;	// pullup, int on both edges
	
	BUTTON2_PORT.DIRCLR = BUTTON2_PIN_BV;
	BUTTON2_PORT.BUTTON2_PIN_CTRL = PORT_PULLUPEN_bm | PORT_INT_BOTH_EDGES;

	BUTTON3_PORT.DIRCLR = BUTTON3_PIN_BV;
	BUTTON3_PORT.BUTTON3_PIN_CTRL = PORT_PULLUPEN_bm | PORT_INT_BOTH_EDGES;

	BUTTON4_PORT.DIRCLR = BUTTON4_PIN_BV;
	BUTTON4_PORT.BUTTON4_PIN_CTRL = PORT_PULLUPEN_bm | PORT_INT_BOTH_EDGES;
}
