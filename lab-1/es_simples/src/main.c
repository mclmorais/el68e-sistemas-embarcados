#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


#define TIME_BASE_MAX 961538 

void readGPIO();

void main(void){
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ     |
                                              SYSCTL_OSC_MAIN       |
                                              SYSCTL_USE_PLL        |
                                              SYSCTL_CFG_VCO_480    ),
                                              24000000); // PLL em 24MHz
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);          // Habilita GPIO N (LED D1 = PN1, LED D2 = PN0)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));   // Aguarda final da habilitação
  
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
  GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_1);  
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

  GPIOPinTypeGPIOInput(GPIO_PORTA_AHB_BASE, GPIO_PIN_2);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  
  UARTStdioConfig(0, 9600, 24000000);
  
  
  UARTprintf("Hello world!\n");
  
  uint32_t timeBaseCounter = 0;
  
  uint32_t frequencyCounter = 0;
  uint8_t readyForNextReading = false;
  
  uint8_t togglePN0 = 1;
  
  while(1)
  {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, togglePN0);
    togglePN0 = !togglePN0;
    
    for(timeBaseCounter = 0; timeBaseCounter < TIME_BASE_MAX; timeBaseCounter++)
    {
      if(readyForNextReading && (GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) & GPIO_PIN_1) == 0)
      {
        frequencyCounter++;
        readyForNextReading = false;
      }
      else if((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) & GPIO_PIN_1) == 1)
      {
        readyForNextReading = true;
      }
    }    

  } // while
} // main