#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

#define TIME_BASE_MAX 1000000

void readGPIO();

void main(void){
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ     |
                                              SYSCTL_OSC_MAIN       |
                                              SYSCTL_USE_PLL        |
                                              SYSCTL_CFG_VCO_480    ),
                                              24000000); // PLL em 24MHz
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);          // Habilita GPIO N (LED D1 = PN1, LED D2 = PN0)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM));   // Aguarda final da habilitação
  
  // Configura LED 1 (N0) como saída para verificação da base de tempo
  GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);
  
  // Escreve 0 em N0
  GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 0);
  
  // Configura M0
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  
  uint32_t timeBaseCounter = 0;
  
  uint32_t frequencyCounter = 0;
  uint8_t readyForNextReading = false;
  
  while(1)
  {
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 1);
    
    for(timeBaseCounter = 0; timeBaseCounter < TIME_BASE_MAX; timeBaseCounter++)
    {
       if(readyForNextReading && readGPIO == 0)
       {
         frequencyCounter++;
         readyForNextReading = false;
       }
       else if (readGPIO == 1)
       {
         readyForNextReading = true;
       }
       
       
    }
    
  } // while
} // main