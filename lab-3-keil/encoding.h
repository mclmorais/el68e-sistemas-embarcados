#ifndef __ENCODING_H__
#define __ENCODING_H__
#include "estruturas-uart.h"
#include "cmsis_os2.h"
#include "stdbool.h"
#include "string_utils.h"

extern osMessageQueueId_t messageQueueOutputId;
extern osThreadId_t threadEncoderId;

void threadEncoder(void *arg);

#endif
