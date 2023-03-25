/**
 * Martin Egli
 * 2023-03-21
 * main loop
 */

// in io.h #include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#include "project.h"
#include "leds.h"
#include "buttons.h"
#include "pwr.h"
#include "rtc.h"
#include "ttimer.h"

volatile uint8_t global_events;

void clock_init(void) {
	CCP = CCP_IOREG_gc;     //unlock
	CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc; // 20 MHz
	CLKCTRL.MCLKCTRLB = 0;  //no prescale, PEN = 0 -> no prescaler
	// no lock CLKCTRL.MCLKLOCK = 0; // no lock
	// no RUNSTANDY CLKCTRL.OSC20MCTRLA = 0;
}

void cpuint_Init(void) {
	//	FUSE.OSCCFG = FUSE_FREQSEL0_bm
	//SIGROW.DEVICEID0

}

int main(void)
{
	uint8_t local_events = 0;
	global_events = 0;

	clock_init();
	cpuint_Init();

	leds_Init();
	leds_Off(0);
	leds_Off(1);
	leds_Off(2);
	leds_Off(3);
	//leds_ShowAlarm(0);
	leds_ShowRunning(0);
	leds_ShowRunning(1);
	leds_ShowRunning(2);
	leds_ShowRunning(3);
	sei();
	while(1);
	/*
	PORTA.DIR = _BV(3);
	while(1) {
		PORTA.OUTSET = _BV(3);
		for(volatile uint32_t xx = 0; xx < 100000; xx++);
		PORTA.OUTCLR = _BV(3);
		for(volatile uint32_t xx = 0; xx < 100000; xx++);
	}*/
/*
	pwr_Init();
	pwr_ClaimMode(PWR_RUN);
	rtc_Init();
	
	leds_Init();
	buttons_Init();
	ttimer_Init();
	
	sei();
	cli();

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
			pwr_Sleep();
		}
    }*/
}

