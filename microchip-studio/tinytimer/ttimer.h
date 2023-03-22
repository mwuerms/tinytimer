/**
 * Martin Egli
 * 2023-03-21
 * module tinytimer
 */

#ifndef _TTIMER_H_
#define _TTIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/sfr_defs.h>

#define NB_TTIMER   4   // in use: 0 ... (NB_TTIMER-1)
#define ttimer_IsNumberValid(nb) (nb <= (NB_TTIMER-1)) ? true : false

void ttimer_Init(void);

#define TT_EV_BUTTON_SHORT_PRESSED      _BV(0)
#define TT_EV_BUTTON_1s_LONG_PRESSED    _BV(1)
#define TT_EV_BUTTON_2s_LONG_PRESSED    _BV(2)
#define TT_EV_TIMEOUT                   _BV(3)
void ttimer_ProcessEvents(uint8_t ttimer_nb, uint8_t events);

#endif // _TTIMER_H_
