/**
 * Martin Egli
 * 2023-03-21
 * project definitions
 */

#ifndef _PROJECT_TINYTIMER_H_
#define _PROJECT_TINYTIMER_H_

#include <stdint.h>
#include <avr/cpufunc.h>

extern volatile uint8_t global_events;
#define EV_WAKEUP   _BV(0)
#define EV_BUTTON   _BV(1)
#define EV_RTC      _BV(2)

#endif
