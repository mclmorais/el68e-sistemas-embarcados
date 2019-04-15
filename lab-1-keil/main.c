#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#define TIME_BASE_MAX 2698574

extern uint32_t frequencyMeasure(uint32_t);

bool khzScale = false;

int main(void)
{
	uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
	                                            SYSCTL_OSC_MAIN |
	                                            SYSCTL_USE_PLL |
	                                            SYSCTL_CFG_VCO_480),
	                                           24000000); // PLL em 24MHz

	// Inicialização de GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
	{
	}
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_1);

	// Inicialização da UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
	{
	}
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
	{
	}
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	UARTStdioConfig(0, 57600, ui32SysClock);
	UARTEchoSet(false);

	uint32_t frequencyCounter = 0 + 1;

	UARTprintf("Laboratorio 1 - Frequencimetro\n");
	while (1)
	{
		// Pino N0 é ligado enquanto contagem de pulsos está sendo realizada.
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);

		frequencyCounter = frequencyMeasure(TIME_BASE_MAX);

		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x01);

		UARTprintf("Frequencia: %i ", frequencyCounter / 2);
		UARTprintf(khzScale ? "KHz\n" : "Hz\n");
	}         // while
}         // main
