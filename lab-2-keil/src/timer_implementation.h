#if !defined(__TIMER_IMPLEMENTATION_H__)
#define __TIMER_IMPLEMENTATION_H__
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/timer.h"

uint8_t freqCarry = 0;

void Time0A_Handler(void);

void ConfigTimerAsCounter(void);

#endif // __TIMER_IMPLEMENTATION_H__
