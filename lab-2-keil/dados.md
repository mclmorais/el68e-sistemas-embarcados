# Timer
* Timer 0 
  * Pino Capture Compare PWM: ```T0CCP0```
  * Assignment: ```PD0 PA0 PL4```

# Edge Count

 **Timer use**: Individual  
 **Count direction**: Up or Down  
 **Counter Size**: 16-bit
 **Prescaler Size**: 8-bit

 > In Edge-Count mode, the timer is configured as a 24-bit up- or down-counter including the optional
prescaler with the upper count value stored in the **GPTM Timer n Prescale (GPTMTnPR)** register
and the lower bits in the **GPTMTnR** register. 

> In this mode, the timer is capable of capturing three types of events: **rising edge**, **falling edge**, or **both**.  

> When counting down in Input Edge-Count Mode, the timer stops after the programmed number of
edge events has been detected. To re-enable the timer, ensure that the TnEN bit is cleared and
repeat steps 4 through 8.

# Prescaler
> Note that when
counting down in one-shot or periodic modes, the prescaler acts as a true prescaler and contains
the least-significant bits of the count.

>When counting up in one-shot or periodic modes, **the prescaler
acts as a timer extension and holds the most-significant bits of the count**. In input edge count, input
edge time and PWM mode, the prescaler always acts as a timer extension, regardless of the count
direction.

# Valor Máximo
> For rising-edge detection, the input signal must be High for at least two clock periods
following the rising edge. Similarly, for falling-edge detection, the input signal must be Low
for at least two clock periods following the falling edge. Based on this criteria, the maximum
input frequency for edge detection is 1/4 of the frequency.

* 1/4 * 120MHz = __30MHz__

# Registradores
* GPTM Timer n Prescale (GPTMTnPR)

* GPTM Timer n Match Register (GPTMTnMATCHR)

* GPTM Timer n Prescale Match (GPTMTnPMR)

* GPTM Interrupt Mask (GPTMIMR) - _993_

* GPTM Control (GPTMCTL)

* In down-count mode, the GPTMTnMATCHR and GPTMTnPMR registers are configured so
that the difference between the value in the GPTMTnILR and GPTMTnPR registers and the
GPTMTnMATCHRand GPTMTnPMRregisters equals thenumber of edge events thatmust
be counted.
* In up-count mode, the timer counts from 0x0 to the value in the GPTMTnMATCHR and
GPTMTnPMR registers. Note that when executing an up-count, the value of the GPTMTnPR
and GPTMTnILR must be greater than the value of GPTMTnPMR and GPTMTnMATCHR.

# Links

[Understanding Input Edge-Time Mode: Detect missing edges when counter/timer overflows](https://e2e.ti.com/support/microcontrollers/other/f/908/t/682038)

[Timer in edge count mode](http://e2e.ti.com/support/microcontrollers/other/f/908/t/354038?Timer-in-edge-count-mode)