#include "elevator.h"

osThreadId_t threadElevatorIds[NUMBER_OF_ELEVATORS];

int avaliaSubida(StructElevador elevador)
{
	int andar_externo = -1;
	int andar_interno = -1;
	if(elevador.estadoAnterior != PRONTO || elevador.estadoAtual == SUBINDO)
	{
		if(((elevador.pendentesSubida >> (elevador.andarAtual + 1)) << (elevador.andarAtual + 1)) > (0xFFFF >> (15 - elevador.andarAtual)))
		{
			uint16_t aux_externo = ((elevador.pendentesSubida >> (elevador.andarAtual + 1)) << (elevador.andarAtual + 1));
			aux_externo &= -aux_externo;
			andar_externo = log2(aux_externo);
		}
	}
	else if(elevador.pendentesSubida != 0)
	{
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
	if(andar_externo == -1 && andar_interno == -1)
		return -1;
	else if(andar_externo != -1 && andar_interno == -1)
		return andar_externo;
	else if(andar_externo == -1 && andar_interno != -1)
		return andar_interno;
	else return (andar_externo > andar_interno ? andar_interno : andar_externo);
}

int avaliaDescida(StructElevador elevador)
{
	int andar_externo = -1;
	int andar_interno = -1;
	if(elevador.estadoAnterior != PRONTO || elevador.estadoAtual == DESCENDO)
	{
		if(((elevador.pendentesDescida << (16 - elevador.andarAtual)) >> (16 - elevador.andarAtual)) < (0xFFFF >> (15 - elevador.andarAtual)) && elevador.pendentesDescida != 0)
		{
			uint16_t aux_externo = ((elevador.pendentesDescida << (16 - elevador.andarAtual)) >> (16 - elevador.andarAtual));
			aux_externo = 1 << (int)log2(aux_externo);
			andar_externo = log2(aux_externo);
		}
	}
	else if(elevador.pendentesDescida != 0)
	{
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
	if(andar_externo == -1 && andar_interno == -1)
		return -1;
	else if(andar_externo != -1 && andar_interno == -1)
		return andar_externo;
	else if(andar_externo == -1 && andar_interno != -1)
		return andar_interno;
	else return (andar_externo > andar_interno ? andar_externo : andar_interno);
}

void threadElevator(void *arg)
{
	uint8_t elevatorNumber = ((uint32_t)arg);
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
