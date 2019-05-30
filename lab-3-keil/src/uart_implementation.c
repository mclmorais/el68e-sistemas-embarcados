#include "uart_implementation.h"

uint8_t flagUART = 0;

void ConfigUART(uint32_t systemClock)
{
	// Ativa UART0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

	UARTStdioConfig(0, 57600, systemClock);

	UARTEchoSet(false);

	UARTprintf("Laboratorio 2 - Frequencimetro\n");

}

void PrintFrequency(uint32_t freqMeasure, bool khzScale)
{
	if(UARTTxBytesFree() == UART_TX_BUFFER_SIZE && flagUART > 0)
	{
		UARTprintf("Frequency: %i", freqMeasure);
		UARTprintf(khzScale ? "KHz\n" : "Hz\n");
		flagUART = 0;
	}
}

void HandleUARTInput(void (*callback)(bool))
{
	if (UARTRxBytesAvail() > 0)
	{
		uint8_t receivedCharacter = UARTgetc();

		if (receivedCharacter == 'k')
		{
			callback(true);
		}
		else if (receivedCharacter == 'h')
		{
			callback(false);
		}
		else
		{
			UARTFlushRx();
		}
	}
}
