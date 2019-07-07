#ifndef __ELEVATOR_H__
#define __ELEVATOR_H__

#include "estruturas-elevador.h"
#include "estruturas-uart.h"
#include "cmsis_os2.h"
#include "decoding.h"
#include "encoding.h"
#include <math.h>

extern osThreadId_t threadElevatorIds[NUMBER_OF_ELEVATORS];

int avaliaSubida(StructElevador elevador);
int avaliaDescida(StructElevador elevador);
void threadElevator(void *arg);

#endif
