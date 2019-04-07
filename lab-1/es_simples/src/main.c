#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#define TIME_BASE_MAX 961538

void readGPIO();
void menu(void);

uint32_t timeBaseMax = TIME_BASE_MAX;

void main(void)
{
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN |
                                              SYSCTL_USE_PLL |
                                              SYSCTL_CFG_VCO_480),
                                             24000000); // PLL em 24MHz

  // Ativa porta N de GPIO e seta pinos 0 e 1 como saída e entrada
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    ;

  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
  GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_1);

  // Ativa UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    ;

  // Ativa pinos da porta A para utilização da UART
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    ;
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);

  UARTStdioConfig(0, 115200, ui32SysClock);

  UARTEchoSet(false);

  UARTprintf("Laboratorio 1 - Frequencimetro\n");

  uint32_t timeBaseCounter = 0;

  uint32_t frequencyCounter = 0;
  uint8_t readyForNextReading = true;

  char receivedCharacter;

  while (1)
  {
    // Pino N0 é ligado enquanto contagem de pulsos está sendo realizada.
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x01);
    for (timeBaseCounter = 0; timeBaseCounter < timeBaseMax; timeBaseCounter++)
    {
      bool isPinHigh = (GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) & GPIO_PIN_1) == GPIO_PIN_1;

      // Conta um pulso se este foi o primeiro LOW recebido após o pino estar em HIGH.
      // Após medir, impede que pulsos em LOW sejam contabilizados até um pulso em HIGH ser adquirido novamente.
      // (impede que várias contagem sejam feitas no mesmo pulso LOW caso faquisição >> fmedida)
      if (readyForNextReading && !isPinHigh)
      {
        frequencyCounter++;
        readyForNextReading = false;
      }
      else if (isPinHigh)
      {
        readyForNextReading = true;
      }
    }
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);

    UARTprintf("Frequencia: %i Hz\n", frequencyCounter);
    frequencyCounter = 0;

    uint8_t bytesAvailable = UARTRxBytesAvail();
    if (bytesAvailable > 0)
      receivedCharacter = UARTgetc();

    if (receivedCharacter == 'C')
      menu();

    // UARTprintf(receivedString);
    receivedCharacter = 'Z';

  } // while
} // main

void menu()
{
  UARTprintf("Digite a constante de tempo desejada:\n");
  char stringifiedNumber[11];
  uint32_t decodedConstant = 0;
  UARTgets(stringifiedNumber, 11);

  //Transforma string numérica recebida em um unsigned integer
  decodedConstant += (stringifiedNumber[9] - 48);
  for (uint8_t i = 0; i < 9; i++)
  {
    int powerOfTen = 1;
    for (uint8_t j = 0; j < (9 - i); j++)
      powerOfTen *= 10;
    decodedConstant += (stringifiedNumber[i] - 48) * powerOfTen;
  }

  if (decodedConstant == 0)
    decodedConstant = TIME_BASE_MAX;

  UARTprintf("Constante escolhida: %u\n", decodedConstant);

  timeBaseMax = decodedConstant;
  
  return;
}