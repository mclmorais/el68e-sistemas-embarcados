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
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "utils/uartstdio.h"
// Implementações
#include "timer_implementation.h"

bool khzScale = false;
uint8_t flagUART = false;

uint32_t freqMeasure = 0;

void SysTick_Handler(void)
{
	freqMeasure = (TimerValueGet(TIMER0_BASE, TIMER_A) + 0x00FFFFFF * freqCarry);
	freqCarry = 0;
	TIMER0_TAV_R = 0;
	flagUART++;
}

void Time0A_Handler(void)
{
	freqCarry++;
	TimerIntClear(TIMER0_BASE, TIMER_CAPA_MATCH);
}

int main(void)
{
	uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
	                                            SYSCTL_OSC_MAIN |
	                                            SYSCTL_USE_PLL |
	                                            SYSCTL_CFG_VCO_480),
	                                           24000000); // PLL em 24MHz

	SysTickEnable();
	SysTickPeriodSet(ui32SysClock / 2);

	// Ativa UART0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

	// Ativa pinos da porta A para utilização da UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
	GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinConfigure(GPIO_PD0_T0CCP0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP);
	TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFF);
	TimerMatchSet(TIMER0_BASE, TIMER_A, 0xFFFF);
	TimerPrescaleMatchSet(TIMER0_BASE, TIMER_A, 0x00FF);
	TimerIntEnable(TIMER0_BASE, TIMER_CAPA_MATCH);
	TimerIntRegister(TIMER0_BASE, TIMER_A, Time0A_Handler);
	TimerEnable(TIMER0_BASE, TIMER_A);

	UARTStdioConfig(0, 57600, ui32SysClock);

	UARTEchoSet(false);

	SysTickIntEnable();

	UARTprintf("Laboratorio 2 - Frequencimetro\n");

	while (1)
	{
		if(UARTTxBytesFree() == UART_TX_BUFFER_SIZE && flagUART >= 2)
		{
			UARTprintf("Frequency: %i", freqMeasure);
			UARTprintf(khzScale ? "KHz\n" : "Hz\n");
			flagUART = 0;
		}
		uint8_t bytesAvailable = UARTRxBytesAvail();
		if (bytesAvailable > 0)
		{
			uint8_t receivedCharacter = UARTgetc();

			if (receivedCharacter == 'k')
			{
				khzScale = true;
				SysTickPeriodSet(ui32SysClock / 1000);
			}
			else if (receivedCharacter == 'h')
			{
				khzScale = false;
				SysTickPeriodSet(ui32SysClock);
			}
			else
			{
				UARTFlushRx();
			}
		}
	} // while
} // main
