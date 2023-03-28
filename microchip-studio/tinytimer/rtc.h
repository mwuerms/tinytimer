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

#define NB_RTC_TIMER NB_TTIMER
void rtc_Init(void);

void rtc_StartModule(void);
void rtc_StopModule(void);

uint16_t rtc_GetCNT(void) ;

void rtc_StartSingleTimeout(uint8_t timer_nb, uint16_t duration_s);
void rtc_StopTimeout(uint8_t timer_nb);
void rtc_Pause(uint8_t timer_nb);
void rtc_Resume(uint8_t timer_nb);

void rtc_ProcessEvent(void);

#endif // _RTC_H_
