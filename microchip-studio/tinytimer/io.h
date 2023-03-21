/**
 * Martin Egli
 * 2023-03-21
 * define io
 */

#ifndef _IO_H_
#define _IO_H_

#include <avr/io.h>

// LED1: PA3
#define LED1_PORT	PORTA
#define LED1_PIN	3
#define LED1_PIN_BV	_BV(LED1_PIN)
// LED2: PA2
#define LED2_PORT	PORTA
#define LED2_PIN	2
#define LED2_PIN_BV	_BV(LED2_PIN)
// LED3: PB0
#define LED3_PORT	PORTB
#define LED3_PIN	0
#define LED3_PIN_BV	_BV(LED3_PIN)
// LED4: PB1
#define LED4_PORT	PORTB
#define LED4_PIN	1
#define LED4_PIN_BV	_BV(LED4_PIN)

// BUTTON1: PA5
#define BUTTON1_PORT	PORTA
#define BUTTON1_PIN		5
#define BUTTON1_PIN_BV	_BV(BUTTON1_PIN)
// BUTTON2: PA4
#define BUTTON2_PORT	PORTA
#define BUTTON2_PIN		4
#define BUTTON2_PIN_BV	_BV(BUTTON2_PIN)
// BUTTON3: PB2
#define BUTTON3_PORT	PORTB
#define BUTTON3_PIN		2
#define BUTTON3_PIN_BV	_BV(BUTTON2_PIN)
// BUTTON4: PB3
#define BUTTON4_PORT	PORTB
#define BUTTON4_PIN		3
#define BUTTON4_PIN_BV	_BV(BUTTON4_PIN)

#endif
