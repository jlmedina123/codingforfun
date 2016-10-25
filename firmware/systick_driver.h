#ifndef systick_h_
#define systick_h_
 
#include "misc.h"
#include "LPC17xx.h"
 
 
/* The processor has a 24-bit system timer, SysTick, that counts down from the reload value
to zero, reloads (wraps to) the value in the LOAD register on the next clock edge, then
counts down on subsequent clocks.
*/
 
#define ST_CTRL     (*((volatile unsigned long*)0xE000E010)) //page 783
#define ST_RELOAD   (*((volatile unsigned long*)0xE000E014))
        /* Value to load into the VAL register when the counter is
           enabled and when it reaches 0.
           The RELOAD value can be any value in the range 0x00000001-0x00FFFFFF.
         */
#define ST_CURRENT  (*((volatile unsigned long*)0xE000E018))
        /* Reads return the current value of the SysTick counter. */
 
void SysTickInit(void);
void systick_tenmsdelay(int ms);
void systick_secdelay(int sec);
  
  
#endif
