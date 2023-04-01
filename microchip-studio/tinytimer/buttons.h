/**
 * Martin Egli
 * 2023-03-21
 * module buttons
 */

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <stdint.h>
#include "ttimer.h"

/*enum {
    BUTTON1 = 0,
    BUTTON2,
    BUTTON3,
    BUTTON4
};*/
#define BUTTON1  TTIMER1
#define BUTTON2  TTIMER2
#define BUTTON3  TTIMER3
#define BUTTON4  TTIMER4
#define NB_BUTTONS NB_TTIMER

void buttons_Init(void);
void button_ProcessEvent(void);

#endif
