#include "systick_implementation.h"

uint32_t sysTickPeriod;
void (*interruptCallback)();

void ConfigSysTick(uint32_t systemClock)
{
	// TODO: Generalizar para qualquer system clock
	sysTickPeriod = systemClock / 1000;


	SysTickEnable();
	SysTickPeriodSet(sysTickPeriod);
	SysTickIntEnable();
}
