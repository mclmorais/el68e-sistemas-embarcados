#if !defined(__GPIO_IMPLEMENTATION_H_)
#define __GPIO_IMPLEMENTATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

void ConfigGPIOCounter(void);
void ConfigGPIOUART(void);

#endif // __GPIO_IMPLEMENTATION_H_
