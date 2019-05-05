#if !defined(__GPIO_IMPLEMENTATION_H)
#define __GPIO_IMPLEMENTATION_H

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

void ConfigGPIOAsCounter(void);

#endif // __GPIO_IMPLEMENTATION_H
