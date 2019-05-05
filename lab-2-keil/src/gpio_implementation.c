#include "gpio_implementation.h"

void ConfigGPIOCounter(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
	GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinConfigure(GPIO_PD0_T0CCP0);
}

void ConfigGPIOUART(void)
{
	// Ativa pinos da porta A para utilização da UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
}
