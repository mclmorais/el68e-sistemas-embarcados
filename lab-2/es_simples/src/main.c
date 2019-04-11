#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

uint32_t counter = 1;
uint8_t LED_D1 = 0;
uint8_t LED_D2 = 0;
uint8_t LED_D3 = 0;
uint8_t LED_D4 = 0;

void SysTick_Handler((void *funcaoqualquer)
{

  counter++;
  // if (++counter > 8)
  //   counter = 1;

  funcaoqualquer();
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LED_D1);
  LED_D1 ^= GPIO_PIN_1;

  if (counter % 2 == 0)
  {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, LED_D2);
    LED_D2 ^= GPIO_PIN_0; // Tro
  }
  if (counter % 4 == 0)
  {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, LED_D3);
    LED_D3 ^= GPIO_PIN_4; // Tro
  }
  if (counter % 8 == 0)
  {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, LED_D4);
    LED_D4 ^= GPIO_PIN_0; // Tro
  }
} // SysTick_Handler

void main(void)
{
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN |
                                              SYSCTL_USE_PLL |
                                              SYSCTL_CFG_VCO_480),
                                             24000000); // PLL em 24MHz

  SysTickEnable();
  SysTickPeriodSet(2400000); // f = 5Hz

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // Habilita GPIO N (LED D1 = PN1, LED D2 = PN0)
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    ; // Aguarda final da habilita��o

  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); // LEDs D1 e D2 como sa�da
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);       // LEDs D1 e D2 apagados
  GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilita GPIO F (LED D3 = PF4, LED D4 = PF0)
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    ; // Aguarda final da habilita��o

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); // LEDs D3 e D4 como sa�da
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);       // LEDs D3 e D4 apagados
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0, push-button SW2 = PJ1)
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    ; // Aguarda final da habilita��o

  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1); // push-buttons SW1 e SW2 como entrada
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  SysTickIntEnable();

  while (1)
  {

    
    initTimer()
    //    if(counter % 2 == 0)
    //    {
    //      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, LED_D2);
    //      LED_D2 ^= GPIO_PIN_0; // Tro
    //    }
    //    if(counter % 4 == 0)
    //    {
    //      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, LED_D3);
    //      LED_D3 ^= GPIO_PIN_0; // Tro
    //    }
    //    if(counter % 8 == 0)
    //    {
    //      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, LED_D4);
    //      LED_D4 ^= GPIO_PIN_4; // Tro
    //    }
  } // while
} // main