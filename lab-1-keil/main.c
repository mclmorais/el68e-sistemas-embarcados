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

// Medida máxima para BPL 24 MHz: 1,455MHz
// Medida máxima para BPL 120MHz: 7,300MHz

#define MAX_COUNT 2999950 //2999951

#define CLOCK 24000000

#if CLOCK == 120000000
	#define TIME_BASE_MAX MAX_COUNT * 5
#else
	#define TIME_BASE_MAX MAX_COUNT
#endif

extern uint32_t frequencyMeasure(int32_t);

bool khzScale = false;

int main(void)
{
	uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
	                                            SYSCTL_OSC_MAIN |
	                                            SYSCTL_USE_PLL |
	                                            SYSCTL_CFG_VCO_480),
	                                           CLOCK); // PLL em 24MHz

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

	uint32_t frequencyCounter = 0;
	UARTprintf("Laboratorio 1 - Frequencimetro\n");
	while (1)
	{
		while((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) & GPIO_PIN_1) == GPIO_PIN_1)
		{
		}
		while((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) & GPIO_PIN_1) != GPIO_PIN_1)
		{
		}
		frequencyCounter = frequencyMeasure(khzScale ? TIME_BASE_MAX / 10 : TIME_BASE_MAX);

		UARTprintf("Frequencia: %i ", frequencyCounter / (khzScale ? 200 : 2));
		UARTprintf(khzScale ? "KHz\n" : "Hz\n");

		uint8_t bytesAvailable = UARTRxBytesAvail();
		if (bytesAvailable > 0)
		{
			uint8_t receivedCharacter = UARTgetc();

			if (receivedCharacter == 'k')
			{
				khzScale = true;
			}
			else if (receivedCharacter == 'h')
			{
				khzScale = false;
			}
			else
			{
				UARTFlushRx();
			}
		}
	}         // while
}         // main
