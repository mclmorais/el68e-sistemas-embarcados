#include "encoding.h"

osMessageQueueId_t messageQueueOutputId;
osThreadId_t threadEncoderId;

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
