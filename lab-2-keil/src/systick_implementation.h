#if !defined(__SYSTICK_IMPLEMENTATION_H_)
#define __SYSTICK_IMPLEMENTATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

void ConfigSysTick(uint32_t systemClock);

#endif // __SYSTICK_IMPLEMENTATION_H_
