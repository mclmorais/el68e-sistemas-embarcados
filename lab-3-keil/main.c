/*----------------------------------------------------------------------------
* CMSIS-RTOS 'main' function template
*---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include <stdio.h>
#include <string.h>
#include "RTE_Components.h"
#include "system_TM4C129.h"
#include "cmsis_os2.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include <string.h>
#include "estruturas-uart.h"
#include "estruturas-elevador.h"
#include <math.h>

#define NUMBER_OF_ELEVATORS 3

osThreadId_t threadEncoderId, threadDecoderId, threadElevatorIds[NUMBER_OF_ELEVATORS];

osMessageQueueId_t messageQueueElevatorIds[NUMBER_OF_ELEVATORS], messageQueueOutputId;

void decode(uint8_t* message)
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
	EventoSaida evento;
	uint8_t outputString[10];
	osStatus_t status;

	while(true)
	{
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		status = osMessageQueueGet(messageQueueOutputId, &evento, NULL, NULL);
		if (status == osOK)
		{

			switch (evento.numeroElevador)
			{
			case 0:
				outputString[0] = 'e';
				break;
			case 1:
				outputString[0] = 'c';
				break;
			case 2:
				outputString[0] = 'd';
				break;
			default:
				break;
			}

			if(evento.tipo == MOVIMENTO)
			{
				switch (evento.dados[0])
				{
				case INICIALIZA:
					outputString[1] = 'r';
					break;
				case ABRE_PORTAS:
					outputString[1] = 'a';
					break;
				case FECHA_PORTAS:
					outputString[1] = 'f';
					break;
				case SOBE:
					outputString[1] = 's';
					break;
				case DESCE:
					outputString[1] = 'd';
					break;
				case PARA:
					outputString[1] = 'p';
					break;
				default:
					break;
				}
				outputString[2] = CR;
			}
			else if (evento.tipo == LUZES)
			{
				switch (evento.dados[0])
				{
				case DESLIGA:
					outputString[1] = 'D';
					break;
				case LIGA:
					outputString[1] = 'L';
					break;
				default:
					break;
				}
				outputString[2] = evento.dados[1] + 'a';
			}

			sendString((char*)outputString);
		}
	}
}

void threadDecoder(void *arg)
{
	uint8_t buffer[20];
	uint8_t i = 0;
	uint8_t receivedChar = 0;
	while(true)
	{
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);

		do
		{
			receivedChar = UARTCharGet(UART0_BASE);
			buffer[i] = receivedChar;
			i = (i + 1) % 10;
		} while (receivedChar != '\n');

		while(UARTCharsAvail(UART0_BASE)) UARTCharGet(UART0_BASE);

		decode(buffer);

		i = 0;
		memset(buffer, 0, 20);
	}
}

int avaliaSubida(StructElevador elevador)
{
	int andar_externo = -1;
	int andar_interno = -1;
	if(elevador.estadoAnterior != PRONTO || elevador.estadoAtual == SUBINDO){
		if(((elevador.pendentesSubida >> (elevador.andarAtual + 1)) << (elevador.andarAtual + 1)) > (0xFFFF >> (15 - elevador.andarAtual)))
		{ 
			uint16_t aux_externo = ((elevador.pendentesSubida >> (elevador.andarAtual + 1)) << (elevador.andarAtual + 1));
			aux_externo &= -aux_externo;
			andar_externo = log2(aux_externo);
		}
	}
	else if(elevador.pendentesSubida != 0){
		uint16_t aux_externo = elevador.pendentesSubida;
		aux_externo &= -aux_externo;
		andar_externo = log2(aux_externo);
	}
	if(((elevador.pendentesInterno >> (elevador.andarAtual + 1)) << (elevador.andarAtual + 1)) > (0xFFFF >> (15 - elevador.andarAtual)))
	{
		uint16_t aux_interno = ((elevador.pendentesInterno >> (elevador.andarAtual + 1)) << (elevador.andarAtual + 1));
		aux_interno &= -aux_interno;
		andar_interno = log2(aux_interno);
	}
	if(andar_externo == -1 && andar_interno == -1) return -1;
	else if(andar_externo != -1 && andar_interno == -1) return andar_externo;
	else if(andar_externo == -1 && andar_interno != -1) return andar_interno;
	else return (andar_externo > andar_interno ? andar_interno : andar_externo);
}

int avaliaDescida(StructElevador elevador)
{
	int andar_externo = -1;
	int andar_interno = -1;
	if(elevador.estadoAnterior != PRONTO || elevador.estadoAtual == DESCENDO){
		if(((elevador.pendentesDescida << (16 - elevador.andarAtual)) >> (16 - elevador.andarAtual)) < (0xFFFF >> (15 - elevador.andarAtual)) && elevador.pendentesDescida != 0)
		{
			uint16_t aux_externo = ((elevador.pendentesDescida << (16 - elevador.andarAtual)) >> (16 - elevador.andarAtual));
			aux_externo = 1 << (int)log2(aux_externo);
			andar_externo = log2(aux_externo);
		}
	}
	else if(elevador.pendentesDescida != 0){
		uint16_t aux_externo = elevador.pendentesDescida;
		aux_externo &= -aux_externo;
		andar_externo = log2(aux_externo);
	}
	if(((elevador.pendentesInterno << (16 - elevador.andarAtual)) >> (16 - elevador.andarAtual)) < (0xFFFF >> (15 - elevador.andarAtual)) && elevador.pendentesInterno != 0)
	{
		uint16_t aux_interno = ((elevador.pendentesInterno << (16 - elevador.andarAtual)) >> (16 - elevador.andarAtual));
		aux_interno = 1 << (int)log2(aux_interno);
		andar_interno = log2(aux_interno);
	}
	if(andar_externo == -1 && andar_interno == -1) return -1;
	else if(andar_externo != -1 && andar_interno == -1) return andar_externo;
	else if(andar_externo == -1 && andar_interno != -1) return andar_interno;
	else return (andar_externo > andar_interno ? andar_externo : andar_interno);
}

void threadElevator(void *arg)
{
	uint8_t elevatorNumber = ((uint8_t)arg);
	StructElevador elevador; 
	elevador.estadoAtual = INICIALIZANDO;
	elevador.estadoAnterior = INICIALIZANDO;
	elevador.mensagemEnviada = false;
	elevador.pendentesSubida = 0;
	elevador.pendentesDescida = 0;
	elevador.pendentesInterno = 0;
	osStatus_t status;
	EventoEntrada eventoRecebido;
	EventoSaida eventoSaida;
	uint32_t tickInicial, tickFinal;
	while(true)
	{
		status = osMessageQueueGet(messageQueueElevatorIds[elevatorNumber], &eventoRecebido, NULL, NULL);
		if(status == osOK)
		{
			if(eventoRecebido.tipo == BOTAO_EXTERNO)
			{
				if(eventoRecebido.dados[1] != elevador.andarAtual && eventoRecebido.dados[1] != elevador.andarAlvo)
				{
					if(eventoRecebido.dados[0] == SUBIDA)
					{
						elevador.pendentesSubida |= (1 << eventoRecebido.dados[1]);
					}
					else if(eventoRecebido.dados[0] == DESCIDA)
					{
						elevador.pendentesDescida |= (1 << eventoRecebido.dados[1]);
					}
				}
			}
			else if(eventoRecebido.tipo == BOTAO_INTERNO)
			{
				if(eventoRecebido.dados[0] != elevador.andarAtual && eventoRecebido.dados[0] != elevador.andarAlvo)
				{
					elevador.pendentesInterno |= (1 << eventoRecebido.dados[0]);
					eventoSaida.tipo = LUZES;
					eventoSaida.numeroElevador = elevatorNumber;
					eventoSaida.dados[0] = LIGA;
					eventoSaida.dados[1] = eventoRecebido.dados[0];
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
			}
			else if(eventoRecebido.tipo == PASSOU_POR_ANDAR)
			{
				elevador.andarAtual = eventoRecebido.dados[0];
			}
		}
		
		switch(elevador.estadoAtual)
		{
			case INICIALIZANDO:
				if(elevador.mensagemEnviada == false)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = INICIALIZA;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = true;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				else if(eventoRecebido.tipo == PORTAS && eventoRecebido.dados[0] == ABERTAS)
				{
					elevador.estadoAnterior = PRONTO;
					elevador.estadoAtual = PRONTO;
					elevador.andarAtual = 0;
					elevador.andarAlvo = 0;
					elevador.pendentesSubida = 0;
					elevador.pendentesDescida = 0;
					elevador.pendentesInterno = 0;
					elevador.mensagemEnviada = false;
				}
				break;
			case PRONTO:
				if(avaliaSubida(elevador) == -1 && elevador.estadoAnterior == SUBINDO)
				{
					elevador.estadoAnterior = PRONTO;
				}
				else if(avaliaDescida(elevador) == -1 && elevador.estadoAnterior == DESCENDO)
				{
					elevador.estadoAnterior = PRONTO;
				}
				if(elevador.pendentesDescida != 0 | elevador.pendentesSubida != 0 | elevador.pendentesInterno != 0)
				{	
					if(avaliaSubida(elevador) != -1)
					{
						elevador.andarAlvo = avaliaSubida(elevador);
						elevador.estadoAtual = FECHANDO_PORTAS;
						elevador.mensagemEnviada = false;
					}
					else if(avaliaDescida(elevador) != -1)
					{
						elevador.andarAlvo = avaliaDescida(elevador);
						elevador.estadoAtual = FECHANDO_PORTAS;
						elevador.mensagemEnviada = false;
					}
				}
				else
				{
					tickFinal = osKernelGetTickCount();
					if((tickFinal - tickInicial) > 5000 && elevador.andarAtual != 0)
					{
						elevador.andarAlvo = 0;
						elevador.estadoAnterior = PRONTO;
						elevador.estadoAtual = FECHANDO_PORTAS;
						elevador.mensagemEnviada = false;
					}
				}
				break;
			case FECHANDO_PORTAS:
				if(elevador.mensagemEnviada == false)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = FECHA_PORTAS;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = true;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				else if(eventoRecebido.tipo == PORTAS && eventoRecebido.dados[0] == FECHADAS)
				{
					elevador.estadoAnterior = PRONTO;
					if(elevador.andarAlvo < elevador.andarAtual)
					{
						elevador.estadoAtual = DESCENDO;
					}
					else if(elevador.andarAlvo > elevador.andarAtual)
					{
						elevador.estadoAtual = SUBINDO;
					}
					elevador.mensagemEnviada = false;
				}
				break;
			case DESCENDO:
				if(elevador.mensagemEnviada == false)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = DESCE;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = true;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				else if(avaliaDescida(elevador) > elevador.andarAlvo && avaliaDescida(elevador) != -1)
				{
					elevador.andarAlvo = avaliaDescida(elevador);
				}
				else if(elevador.andarAtual == elevador.andarAlvo)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = PARA;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					osThreadFlagsSet(threadEncoderId, 0x0001);
					elevador.estadoAnterior = DESCENDO;
					elevador.estadoAtual = ABRINDO_PORTAS;
					elevador.pendentesSubida &= ~(1 << elevador.andarAlvo);
					elevador.pendentesDescida &= ~(1 << elevador.andarAlvo);
					elevador.pendentesInterno &= ~(1 << elevador.andarAlvo);
					eventoSaida.tipo = LUZES;
					eventoSaida.numeroElevador = elevatorNumber;
					eventoSaida.dados[0] = DESLIGA;
					eventoSaida.dados[1] = elevador.andarAtual;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = false;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				break;
			case SUBINDO:
				if(elevador.mensagemEnviada == false)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = SOBE;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = true;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				else if(avaliaSubida(elevador) < elevador.andarAlvo && avaliaSubida(elevador) != -1)
				{
					elevador.andarAlvo = avaliaSubida(elevador);
				}
				else if(elevador.andarAtual == elevador.andarAlvo)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = PARA;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					osThreadFlagsSet(threadEncoderId, 0x0001);
					elevador.estadoAnterior = SUBINDO;
					elevador.estadoAtual = ABRINDO_PORTAS;
					elevador.pendentesSubida &= ~(1 << elevador.andarAlvo);
					elevador.pendentesDescida &= ~(1 << elevador.andarAlvo);
					elevador.pendentesInterno &= ~(1 << elevador.andarAlvo);
					eventoSaida.tipo = LUZES;
					eventoSaida.numeroElevador = elevatorNumber;
					eventoSaida.dados[0] = DESLIGA;
					eventoSaida.dados[1] = elevador.andarAtual;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = false;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				break;
			case ABRINDO_PORTAS:
				if(elevador.mensagemEnviada == false)
				{
					eventoSaida.tipo = MOVIMENTO;
					eventoSaida.dados[0] = ABRE_PORTAS;
					eventoSaida.numeroElevador = elevatorNumber;
					osMessageQueuePut(messageQueueOutputId, &eventoSaida, 0, NULL);
					elevador.mensagemEnviada = true;
					osThreadFlagsSet(threadEncoderId, 0x0001);
				}
				else if(eventoRecebido.tipo == PORTAS && eventoRecebido.dados[0] == ABERTAS)
				{
					elevador.estadoAtual = AGUARDANDO;
					tickInicial = osKernelGetTickCount();
					elevador.mensagemEnviada = false;
				}
				break;
			case AGUARDANDO:
				tickFinal = osKernelGetTickCount();
				if((tickFinal - tickInicial) > 1000)
				{
					elevador.estadoAtual = PRONTO;
					tickInicial = osKernelGetTickCount();
				}
				break;
			default:
				break;
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
	osThreadSetPriority(threadEncoderId, osPriorityRealtime7);
	threadDecoderId = osThreadNew(threadDecoder, NULL, NULL);
	osThreadSetPriority(threadDecoderId, osPriorityRealtime7);

	for(uint8_t i = 0; i < NUMBER_OF_ELEVATORS; i++)
	{
		threadElevatorIds[i] = osThreadNew(threadElevator, (void*) i, NULL);
		messageQueueElevatorIds[i] = osMessageQueueNew(10, sizeof(EventoEntrada), NULL);
	}

	messageQueueOutputId = osMessageQueueNew(10, sizeof(EventoSaida), NULL);

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
