/**
 * Martin Egli
 * 2023-03-22
 * module power
 */

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>
#include "pwr.h"

#define PWR_MAX_CLAIM_CNT   100
static uint8_t pwr_claim_cnts[4];

void pwr_Init(void) {
    pwr_claim_cnts[0] = 0;  // PWR_RUN = 0,
    pwr_claim_cnts[1] = 0;  // PWR_IDLE,
    pwr_claim_cnts[2] = 0;  // PWR_STANDBY
    pwr_claim_cnts[3] = 0;  // PWR_POWER_DOWN
    return;
}

void pwr_ClaimMode(uint8_t mode) {
    if(pwr_claim_cnts[mode] < (PWR_MAX_CLAIM_CNT-1)) {
        pwr_claim_cnts[mode]++;
    }
}

void pwr_ReleaseMode(uint8_t mode) {
    if(pwr_claim_cnts[mode]) {
        pwr_claim_cnts[mode]--;
    }
}

void pwr_Sleep(void) {
    if(pwr_claim_cnts[0]) {
        // PWR_RUN
        sei();
    }
    else if(pwr_claim_cnts[1]) {
        // PWR_IDLE
        _NOP();
    }
    else if(pwr_claim_cnts[2]) {
        // PWR_STANDBY
        _NOP();
    }
    else { // if(pwr_claim_cnts[3]) {
        // PWR_POWER_DOWN
        _NOP();
    }
}
