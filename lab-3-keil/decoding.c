#include "decoding.h"
#include "estruturas-uart.h"

osMessageQueueId_t messageQueueElevatorIds[NUMBER_OF_ELEVATORS];
osThreadId_t threadDecoderId;

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
