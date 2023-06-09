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
#include "dbguart.h"

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
	dbguart_Init();
	dbguart_SendString("hello from tinyTimer\n initialize ");

	leds_Init();
	dbguart_SendChar('.');
	pwr_Init();
	dbguart_SendChar('.');
	pwr_UseMode(PWR_POWER_DOWN);
	dbguart_SendChar('.');
		
	leds_Init();
	dbguart_SendChar('.');
	buttons_Init();
	dbguart_SendChar('.');
	rtc_Init();
	dbguart_SendChar('.');
	rtc_StartModule();
	dbguart_SendChar('.');
	ttimer_Init();
	dbguart_SendChar('.');

	dbguart_SendString(" done\n");

	sei();

	/* testing
	leds_ShowRunning(BUTTON1);
	//leds_ShowPause(BUTTON1);
	//ttimer_ProcessEvents(BUTTON1, TT_EV_BUTTON_1s_LONG_PRESSED);
	ttimer_ProcessEvents(BUTTON2, TT_EV_BUTTON_1s_LONG_PRESSED);
	ttimer_ProcessEvents(BUTTON3, TT_EV_BUTTON_1s_LONG_PRESSED);
	ttimer_ProcessEvents(BUTTON4, TT_EV_BUTTON_1s_LONG_PRESSED);
	*/
    while (1) 
    {
		if(local_events & EV_BUTTON) {
			button_ProcessEvent();
		}

		if(local_events & EV_RTC) {
			rtc_ProcessEvent();
		}

		// synv events from background and/or go to sleep
		while(1) {
			cli();
			// sync from background
			local_events = global_events;
			global_events = 0;
			
			if(local_events) {
				sei();
				break;
			}
			pwr_Sleep();
		}
    }
}

