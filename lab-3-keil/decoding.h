#ifndef __DECODING_H__
#define __DECODING_H__

#include "cmsis_os2.h"
#include "elevator_constants.h"
#include "string_utils.h"
#include "../driverlib/uart.h"
#include <string.h>

extern osMessageQueueId_t messageQueueElevatorIds[NUMBER_OF_ELEVATORS];
extern osThreadId_t threadDecoderId;

void decode(uint8_t* message);
void threadDecoder(void *arg);

#endif
