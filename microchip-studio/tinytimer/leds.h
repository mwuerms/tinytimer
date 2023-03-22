/**
 * Martin Egli
 * 2023-03-21
 * module leds
 */

#ifndef _LEDS_H_
#define _LEDS_H_

#include <stdint.h>
void leds_Init(void);

void leds_Off(uint8_t ttimer_nb);
void leds_ShowRunning(uint8_t ttimer_nb);
void leds_ShowPause(uint8_t ttimer_nb);
void leds_ShowAlarm(uint8_t ttimer_nb);

#endif
