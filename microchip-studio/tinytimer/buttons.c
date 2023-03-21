/**
 * Martin Egli
 * 2023-03-21
 * module timer
 */

#include "io.h"
#include "buttons.h"
#include <avr/interrupt.h>

volatile uint8_t systick_ms;
ISR(PORTA_PORT_vect){
	return;
}
ISR(PORTB_PORT_vect){
	return;
}

void buttons_Init(void) {
	BUTTON1_PORT.DIRCLR = BUTTON1_PIN_BV;
	BUTTON2_PORT.DIRCLR = BUTTON2_PIN_BV;
	BUTTON3_PORT.DIRCLR = BUTTON3_PIN_BV;
	BUTTON4_PORT.DIRCLR = BUTTON4_PIN_BV;
}
