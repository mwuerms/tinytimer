/**
 * Martin Egli
 * 2023-03-22
 * module power
 */

#include <stdint.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "io.h"
#include "pwr.h"
#include "dbguart.h"

#define PWR_MAX_USE_CNT   100
static uint8_t pwr_use_cnts[4];

void pwr_Init(void) {
    DBG3_DIR_OUT();
    DBG3_PIN_CLR();
    pwr_use_cnts[0] = 0;  // PWR_RUN = 0,
    pwr_use_cnts[1] = 0;  // PWR_IDLE,
    pwr_use_cnts[2] = 0;  // PWR_STANDBY
    pwr_use_cnts[3] = 0;  // PWR_POWER_DOWN
    return;
}

void pwr_UseMode(uint8_t mode) {
    if(pwr_use_cnts[mode] < (PWR_MAX_USE_CNT-1)) {
        pwr_use_cnts[mode]++;
    }
}

void pwr_ReleaseMode(uint8_t mode) {
    if(pwr_use_cnts[mode]) {
        pwr_use_cnts[mode]--;
    }
}

void pwr_Sleep(void) {
    DBG3_PIN_SET();

    dbguart_SendString("PWR ");
    /*dbguart_SendChar(0);
    dbguart_SendChar(':');
    dbguart_SendChar(pwr_use_cnts[0]);
    dbguart_SendChar(1);
    dbguart_SendChar(':');
    dbguart_SendChar(pwr_use_cnts[1]);
    dbguart_SendChar(2);
    dbguart_SendChar(':');
    dbguart_SendChar(pwr_use_cnts[2]);
    dbguart_SendChar(3);
    dbguart_SendChar(':');
    dbguart_SendChar(pwr_use_cnts[3]);*/

    if(pwr_use_cnts[0]) {
        // PWR_RUN
        sei();
        dbguart_SendChar('R');
        DBG3_PIN_CLR();
        return;
    }

	cli();
    if(pwr_use_cnts[1]) {
        // PWR_IDLE
        SLPCTRL.CTRLA = SLEEP_MODE_IDLE | SLPCTRL_SEN_bm;
        dbguart_SendChar('I');
    }
    else if(pwr_use_cnts[2]) {
        // PWR_STANDBY
        SLPCTRL.CTRLA = SLEEP_MODE_STANDBY | SLPCTRL_SEN_bm;
        dbguart_SendChar('S');
    }
    else { // if(pwr_use_cnts[3]) {
        // PWR_POWER_DOWN
		SLPCTRL.CTRLA = SLEEP_MODE_PWR_DOWN | SLPCTRL_SEN_bm;
        dbguart_SendChar('P');
    }
	
	//sleep_bod_disable();
    sei();
    dbguart_SendChar('!');
    sleep_cpu();
    //sleep_disable();
	SLPCTRL.CTRLA = 0x00;
    sei();
    DBG3_PIN_CLR();
}
