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

bool flag = false;
 
 void UART0_Handler(void){
	 uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);
	 UARTIntClear(UART0_BASE, ui32Status);
	 flag = 1;
 }
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
	UARTConfigSetExpClk(UART0_BASE, SystemCoreClock, 57600, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));
//	UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
	UARTIntDisable(UART0_BASE, 0xFFFFFFFF);
	UARTIntEnable(UART0_BASE, UART_INT_RX);
	IntEnable(INT_UART0);
	UARTEnable(UART0_BASE);
	UARTFIFODisable(UART0_BASE);
	char buffer[10] = "Init\n\0";
	uint8_t i = 0;
	while(buffer[i] != '\0'){
		UARTCharPut(UART0_BASE, buffer[i++]);
	}
	
  for (;;) {
		if(flag == true){
			uint8_t i = 0;
			while(UARTCharsAvail(UART0_BASE) && i<8){
				buffer[i++] = UARTCharGetNonBlocking(UART0_BASE);
			}
			buffer[i] = '\0';
			i = 0;
			while(buffer[i] != '\0'){
				UARTCharPut(UART0_BASE, buffer[i++]);
			}
			flag = false;
		}
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
  // ...
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
