/*----------------------------------------------------------------------------
* CMSIS-RTOS 'main' function template
*---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "RTE_Components.h"
#include "system_TM4C129.h"
#include "cmsis_os2.h"
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "estruturas-uart.h"

#define NUMBER_OF_ELEVATORS 3

osThreadId_t threadEncoderId, threadDecoderId, threadElevatorIds[NUMBER_OF_ELEVATORS];

osMessageQueueId_t messageQueueElevatorIds[NUMBER_OF_ELEVATORS];

void decodificar(uint8_t* message)
{
	EventoEntrada evento;

	switch (message[1])
	{
	case 'F':
		evento.tipo = PORTAS;
		evento.dados[0] = FECHADAS;
		break;
	case 'A':
		evento.tipo = PORTAS;
		evento.dados[0] = ABERTAS;
		break;
	case 'I':
		evento.tipo = BOTAO_INTERNO;
		evento.dados[0] = message[2] - 0x61;
		break;
	case 'E':
		evento.tipo = BOTAO_EXTERNO;
		evento.dados[0] = message[4] == 's' ? SUBIDA : DESCIDA;
		evento.dados[1] = (message[2] - '0') * 10 + (message[3] - '0');
		break;
	default:
		if(message[1] == '1' && (message[2] >= '0' && message[2] <= '9'))
		{
			evento.tipo = PASSOU_POR_ANDAR;
			evento.dados[0] = 10 + (message[2] - '0');
		}
		else if (message[1] >= '0' && message[1] <= '9')
		{
			evento.tipo = PASSOU_POR_ANDAR;
			evento.dados[0] = message[1] - '0';
		}
		break;
	}

	// Checa o primeiro caractere da mensagem para decidir o elevador
	switch (message[0])
	{
	case 'e':
		osMessageQueuePut (messageQueueElevatorIds[0], &evento, 0, NULL);
		break;
	case 'c':
		osMessageQueuePut (messageQueueElevatorIds[1], &evento, 0, NULL);
		break;
	case 'd':
		osMessageQueuePut (messageQueueElevatorIds[2], &evento, 0, NULL);
	default:
		break;
	}
}

void UART0_Handler(void)
{
	uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Status);
	osThreadFlagsSet(threadDecoderId, 0x0001);
}

void sendString(char string[])
{
	for(uint8_t i = 0; i < strlen(string); i++)
	{
		UARTCharPut(UART0_BASE, string[i]);
	}
	UARTCharPut(UART0_BASE, '\r');
}

void threadEncoder(void *arg)
{
	osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
	sendString("er");
	osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
	sendString("ef");
	osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
	sendString("es");
	while(true)
	{
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		sendString("ep");
	}
}

void threadDecoder(void *arg)
{
	uint8_t buffer[20];
	uint8_t i = 0;
	uint8_t receivedChar = 0;
	while(true)
	{
		//osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		if(UARTCharsAvail(UART0_BASE))
		{
			receivedChar = UARTCharGetNonBlocking(UART0_BASE);
			if(receivedChar == LF)
			{
				decodificar(buffer);
				i = 0;
				for(uint8_t j = 0; j < 20; j++)
					buffer[j] = 0;
			}
			else
			{
				buffer[i] = receivedChar;
				i = (i + 1) % 10;
			}
		}
	}
}

void threadElevator(void *arg)
{
	uint32_t elevatorNumber = ((uint32_t)arg);
	int x = 0;
	osStatus_t status;
	EventoEntrada eventoRecebido;
	while(true)
	{
		status = osMessageQueueGet (messageQueueElevatorIds[elevatorNumber], &eventoRecebido, NULL, NULL);
		if (status == osOK)
		{
			x = elevatorNumber;
		}
	}

}

/*----------------------------------------------------------------------------
* Application main thread
*---------------------------------------------------------------------------*/
void app_main (void *argument)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
	UARTConfigSetExpClk(UART0_BASE, SystemCoreClock, 115200, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));
	UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
	UARTIntDisable(UART0_BASE, 0xFFFFFFFF);
	UARTIntEnable(UART0_BASE, UART_INT_RX);
	IntEnable(INT_UART0);
	UARTEnable(UART0_BASE);

	threadEncoderId = osThreadNew(threadEncoder, NULL, NULL);
	threadDecoderId = osThreadNew(threadDecoder, NULL, NULL);

	for(uint8_t i = 0; i < NUMBER_OF_ELEVATORS; i++)
	{
		threadElevatorIds[i] = osThreadNew(threadElevator, (void*) i, NULL);
		messageQueueElevatorIds[i] = osMessageQueueNew(10, sizeof(EventoEntrada), NULL);
	}

	while(true)
	{
		osDelay(osWaitForever);
	}
}

int main (void)
{
	SystemCoreClockUpdate();
	osKernelInitialize();
	osThreadNew(app_main, NULL, NULL);
	if(osKernelGetState() == osKernelReady)
	{
		osKernelStart();
	}
	while(true);
}
