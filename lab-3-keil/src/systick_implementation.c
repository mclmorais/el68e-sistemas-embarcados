#include "systick_implementation.h"

uint32_t sysTickPeriod;
void (*interruptCallback)();

void ConfigSysTick(uint32_t systemClock)
{
	sysTickPeriod = ((systemClock - 80) / 10);

	SysTickEnable();
	SysTickPeriodSet(sysTickPeriod);
	SysTickIntEnable();
}
