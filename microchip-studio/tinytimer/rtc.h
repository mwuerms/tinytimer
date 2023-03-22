/**
 * Martin Egli
 * 2023-03-21
 * module tinytimer
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>
#include <stdbool.h>

#include "rtc.h"

void rtc_Init(void);

void rtc_StartModule(void);
void rtc_StopModule(void);

void rtc_StartSingleTimeout(uint8_t ttimer_nb, uint16_t duration_s);
void rtc_StopTimeout(uint8_t ttimer_nb);
void rtc_Pause(uint8_t ttimer_nb);
void rtc_Resume(uint8_t ttimer_nb);
#endif // _RTC_H_
