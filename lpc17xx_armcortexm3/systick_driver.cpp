#include "systick.h"
 
volatile unsigned long systick_mscounter = 0;
volatile unsigned long systick_seccounter = 0;
 
void SysTickInit(void)
{
    //flashOnboardLED(1,1000000);
    systick_mscounter = 0;
    systick_seccounter = 0;
     
    ST_CTRL &= ~bit0; // disable systick for setup
    ST_RELOAD = 0xF423F; //999,999 in hex, to get 10ms ticks (100 MHz clock)   
    ST_CURRENT = 0; // clearing current by writing to it
    ST_CTRL |= bit2; // use processor clock
    ST_CTRL |= bit1; // enable IRQ interrupt
    ST_CTRL |= bit0; // enable systick
    //flashOnboardLED(2,1000000);
 
    //NVIC_EnableIRQ(SysTick_IRQn); // Systick is an exception, this would hault the CPU
 
    //flashOnboardLED(3,1000000);
 }
  
 extern "C" void SysTick_Handler()
 {
     unsigned long flag = ST_CTRL; // clearing flag (bit16) by reading register
     systick_mscounter++;
     if (systick_mscounter % 100 == 0)
         systick_seccounter++;
 }
  
  
 void systick_tenmsdelay(int ms) /* 1 give 10 msecs */
 {
     unsigned long current_ms = systick_mscounter;
     while (systick_mscounter - current_ms < ms);
 }
  
  void systick_secdelay(int secs)
 {
     unsigned long current_secs = systick_seccounter;
     while (systick_seccounter - current_secs < secs);
 }
   
      
     
