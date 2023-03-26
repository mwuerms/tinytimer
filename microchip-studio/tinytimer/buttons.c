/**
 * Martin Egli
 * 2023-03-21
 * module timer
 */

#include "io.h"
#include "buttons.h"
#include <avr/interrupt.h>

static void button_ProcessInt(uint8_t button_nb) {
	if(BUTTON4_PORT.IN & BUTTON4_PIN_BV) {
			// rising edge
		}
		else {
			// falling edge
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
