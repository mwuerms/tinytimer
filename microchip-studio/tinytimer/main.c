/*
 * tinytimer.c
 *
 * Created: 21.03.2023 19:57:59
 * Author : Martin
 */ 

// in io.h #include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
//#include <avr/sleep.h>

#include "project.h"
#include "leds.h"
#include "buttons.h"
#include "timer.h"

volatile uint8_t global_events;

void clock_init(void) {
	CCP = CCP_IOREG_gc;     //unlock
	CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc; // 20 MHz
	CLKCTRL.MCLKCTRLB = 0;  //no prescale, PEN = 0 -> no prescaler
	// no lock CLKCTRL.MCLKLOCK = 0; // no lock
	// no RUNSTANDY CLKCTRL.OSC20MCTRLA = 0;
}

void sleep_mode(void) {
	sei();
}

int main(void)
{
	uint8_t local_events = 0;
	global_events = 0;

	clock_init();
	leds_Init();
	buttons_Init();
	timer_Init();
	
	sei();
	cli();
	
    /* Replace with your application code */
    while (1) 
    {
		if(local_events) {
			_NOP();
		}
		while(1) {
			cli();
			local_events = global_events;
			
			if(local_events) {
				break;
			}
			sleep_mode();
		}
    }
}

