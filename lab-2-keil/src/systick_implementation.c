#include "systick_implementation.h"

uint32_t sysTickPeriod;
void (*interruptCallback)();

void ConfigSysTick(uint32_t systemClock)
{
	// TODO: Generalizar para qualquer system clock
	sysTickPeriod = systemClock / 2;


	SysTickEnable();
	SysTickPeriodSet(sysTickPeriod);
	SysTickIntEnable();
}

void SetFasterSysTick(bool faster)
{
	SysTickPeriodSet(faster ? sysTickPeriod / 1000 : sysTickPeriod);
}
