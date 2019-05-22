// Keil ARM Compiler
#include <stdbool.h>
#include <stdint.h>
// CMSIS-Core
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
// Driverlib
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
// Implementações
#include "timer_implementation.h"
#include "gpio_implementation.h"
#include "systick_implementation.h"
#include "uart_implementation.h"

bool khzScale = false;

void SysTick_Handler(void)
{
	if(++timerCount >= (khzScale ? 1 : 10))
	{
		freqMeasure = (TimerValueGet(TIMER0_BASE, TIMER_A) + 0xFFFF * freqCarry) / (khzScale ? 100 : 1);
		//	freqMeasure -= (freqMeasure / 1000000) + 1;
		TIMER0_TAV_R = 0;
		freqCarry = 0;
		flagUART++;
		timerCount = 0;
	}
}

void Time0A_Handler(void)
{
	TimerIntDisable(TIMER0_BASE, TIMER_CAPA_MATCH);
	freqCarry++;
	TimerIntClear(TIMER0_BASE, TIMER_CAPA_MATCH);
	TimerIntEnable(TIMER0_BASE, TIMER_CAPA_MATCH);
}

void inputCallback(bool khzChosen)
{
	khzScale = khzChosen;
}

int main(void)
{
	uint32_t ui32SysClock = SysCtlClockFreqSet(
		(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
		120000000); // PLL em 24MHz

	ConfigSysTick(ui32SysClock);

	ConfigGPIOCounter();
	ConfigTimerCounter();

	ConfigGPIOUART();
	ConfigUART(ui32SysClock);

	while (1)
	{
		PrintFrequency(freqMeasure, khzScale);
		HandleUARTInput(inputCallback);
	} // while
} // main
