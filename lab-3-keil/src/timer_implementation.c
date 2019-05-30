#include "timer_implementation.h"

uint32_t freqCarry = 0;
uint32_t freqMeasure = 0;
uint32_t timerCount = 0;

void ConfigTimerCounter(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP);
	TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
//	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFF);
	TimerMatchSet(TIMER0_BASE, TIMER_A, 0xFFFF);
	TimerPrescaleMatchSet(TIMER0_BASE, TIMER_A, 0x00);
	TIMER0_TAILR_R = 0xFFFF;
	TIMER0_TAPR_R = 0xFF;
	TIMER0_IMR_R |= 0x02;
	NVIC_PRI4_R &= ~0xE0000000;
	NVIC_EN0_R |= 0x80000;
//	TimerIntRegister(TIMER0_BASE, TIMER_A, Time0A_Handler);
//	TimerIntEnable(TIMER0_BASE, TIMER_CAPA_MATCH);
	TimerEnable(TIMER0_BASE, TIMER_A);
}
