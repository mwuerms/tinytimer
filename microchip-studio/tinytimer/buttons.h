/**
 * Martin Egli
 * 2023-03-21
 * module buttons
 */

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <stdint.h>
#include "ttimer.h"

enum {
    BUTTON1 = 0,
    BUTTON2,
    BUTTON3,
    BUTTON4
};
#define NB_BUTTONS NB_TTIMER

void buttons_Init(void);
void button_ProcessEvent(void);

#endif
