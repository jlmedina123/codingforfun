/*PLL0 initialization routine for LPC17xx controller
 *
 * LPC17xx controllers have three clock sources:
 * Main oscillator: external (on-board)
 * Internal RC oscillator: used after power-up/reset
 * RTC oscillator: 1 Hz low-power oscillator for RTC
 *
 * There are also two PLLs that can be used:
 * PLL0:
 * Used by CPU and peripherals
 * CLCSRCSEL register: selects clock
 *
 * PLL1:
 * provides 48 MHz to USB subsystem
 * Disabled and powered off after power-up/reset
 *
 * After power-up, the CPU is running on the internal RC oscillator. I wrote a routine in C to switch to PLL0 at 100MHz. The routine basically follows the user manual and implements all the steps given in the manua
 */

#include "pll0.h"
#include "LPC17xx.h"
#include "misc.h"
 
void PLL0OneFeedSequence(void)
{
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
}
 
void PLL0SetupSequence(void)
{
    // 1. Disconnect PLL0 with one feed sequence if PLL0 is already connected.
    LPC_SC->PLL0CON &= ~bit1; 
    PLL0OneFeedSequence();   
     
    // 2. Disable PLL0 with one feed sequence.
    LPC_SC->PLL0CON |= bit0;
    PLL0OneFeedSequence();
     
    // 3. Change the CPU Clock Divider setting to speed up operation without PLL0, if desired.
    LPC_SC->CCLKCFG = 0; // divide frequency by 1
     
    // 4. Write to the Clock Source Selection Control register to change the clock source if needed.
    LPC_SC->CLKSRCSEL = 0x10; // 10 for main clock
     
    // 5. Write to the PLL0CFG and make it effective with one feed sequence. The PLL0CFG can only be updated when PLL0 is disabled.
    LPC_SC->PLL0CFG = 0x0E; // for 100 MHz (from example 1 in specs)
     
    // 6. Enable PLL0 with one feed sequence.
    LPC_SC->PLL0CON |= bit0; //setting bit 0 to enable
    PLL0OneFeedSequence();
      
    // 7. Change the CPU Clock Divider setting for the operation with PLL0. It is critical to do this before connecting PLL0.
    LPC_SC->CCLKCFG = myPLL_output_divider; 
 
 
    // 8. Wait for PLL0 to achieve lock by monitoring the PLOCK0 bit in the PLL0STAT register,
    //    or using the PLOCK0 interrupt, or wait for a fixed time when the input clock to PLL0 is
    //    slow (i.e. 32 kHz). The value of PLOCK0 may not be stable when the PLL reference
    //    frequency (FREF, the frequency of REFCLK, which is equal to the PLL input
    //    frequency divided by the pre-divider value) is less than 100 kHz or greater than
    //    20 MHz. In these cases, the PLL may be assumed to be stable after a start-up time
    //    has passed. This time is 500 μs when FREF is greater than 400 kHz and 200 / FREF
    //    seconds when FREF is less than 400 kHz.
 
    int i = 0;
    while((LPC_SC->PLL0STAT & bit26) != bit26)
        i++;
         
     // 9. Connect PLL0 with one feed sequence.
     LPC_SC->PLL0CON |= bit1; //setting bit 1 to connect     
     PLL0OneFeedSequence();
     SystemCoreClockUpdate(); //defined in system_LPC17xx.c
 }
