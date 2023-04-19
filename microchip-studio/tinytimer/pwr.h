/**
 * Martin Egli
 * 2023-03-21
 * define io
 */

#ifndef _PWR_H_
#define _PWM_H_

#include <stdint.h>

typedef enum {
    PWR_RUN = 0,
    PWR_IDLE,
    PWR_STANDBY,
    PWR_POWER_DOWN
} pwr_modes_t;

void pwr_Init(void);
void pwr_Sleep(void);
void pwr_UseMode(uint8_t mode);
void pwr_ReleaseMode(uint8_t mode);

#endif
