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
	
	pwr_Init();
	pwr_ClaimMode(PWR_RUN);
		
	leds_Init();
	buttons_Init();
	rtc_Init();
	rtc_StartModule();
	ttimer_Init();
	
	leds_On(LED1);
	//leds_Off(LED1);
	leds_Off(LED2);
	leds_Off(LED3);
	leds_Off(LED4);
	//leds_ShowAlarm(LED1);
	//leds_ShowRunning(LED1);
	leds_ShowRunning(LED2);
	leds_ShowRunning(LED3);
	leds_ShowRunning(LED4);
	
	leds_Off(LED1);

	sei();

	while((CLKCTRL.MCLKSTATUS & 0x20) == 0);
	leds_On(LED1);
	
    while (1) 
    {
		if(local_events & 0x01) {
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
    }
}

