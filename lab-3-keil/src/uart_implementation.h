#if !defined(__UART_IMPLEMENTATION_H_)
#define __UART_IMPLEMENTATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"

#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/gpio.h"

void ConfigUART(uint32_t systemClock);

void PrintFrequency(uint32_t freqMeasure, bool khzScale);
void HandleUARTInput(void (*callback)(bool));

extern uint8_t flagUART;

#endif // __UART_IMPLEMENTATION_H_
