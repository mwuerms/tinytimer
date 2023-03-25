/**
 * Martin Egli
 * 2023-03-21
 * module leds
 */

#ifndef _LEDS_H_
#define _LEDS_H_

#include <stdint.h>

#define NB_LEDS NB_TTIMER
void leds_Init(void);

enum {
    LED1 = 0,
    LED2,
    LED3,
    LED4
};

void leds_On(uint8_t led_nb);
void leds_Off(uint8_t led_nb);
void leds_ShowRunning(uint8_t led_nb);
void leds_ShowPause(uint8_t led_nb);
void leds_ShowAlarm(uint8_t led_nb);

#endif
