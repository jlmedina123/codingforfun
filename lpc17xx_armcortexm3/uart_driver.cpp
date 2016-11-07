#include "uart.h"
 
 
void UART0Init(void)
{
 
    /* The UART0/2/3 peripherals are configured using the following registers:
    1. Power: In the PCONP register (Table 46), set bits PCUART0/2/3.
    Remark: On reset, UART0 is enabled (PCUART0 = 1), and UART2/3 are disabled
    (PCUART2/3 = 0). */
    LPC_SC->PCONP |= bit3; /* set bit 3 for UART0 */
 
     
    /*2. Peripheral clock: In the PCLKSEL0 register (Table 40), select PCLK_UART0; in the
    PCLKSEL1 register (Table 41), select PCLK_UART2/3. */
    LPC_SC->PCLKSEL0 &= ~(bit6 | bit7); /*clear bits 6 and 7 for UART0 */
     
    /* 3. Baud rate: In register U0/2/3LCR (Table 279), set bit DLAB =1. This enables access
    to registers DLL (Table 273) and DLM (Table 274) for setting the baud rate. Also, if
    needed, set the fractional baud rate in the fractional divider register (Table 285).*/
    LPC_UART0->LCR |= bit7; /* Enable access to Divisor Latches. */
    LPC_UART0->LCR |= (bit0 & bit1); /* 8-bit character length */
    LPC_UART0->LCR &= ~bit2; /* 1 stop bit */
    LPC_UART0->LCR &= ~bit3; /* siable parity generation and checking */
    LPC_UART0->LCR &= ~bit6; /* disable break transmission */
    /* baud rate to auto ? */
     
    /*4. UART FIFO: Use bit FIFO enable (bit 0) in register U0/2/3FCR (Table 278) to enable
    FIFO. */
    LPC_UART0->FCR |= bit0; 
     
    /*5. Pins: Select UART pins through the PINSEL registers and pin modes through the
    PINMODE registers (Section 8.5).
    Remark: UART receive pins should not have pull-down resistors enabled.*/
    /* 5:4 01-> P0.2 = TXD0
       7:6 01-> P0.3 = RXD0
     */
    LPC_PINCON->PINSEL0 &= ~bit5;
    LPC_PINCON->PINSEL0 |= bit4;
    LPC_PINCON->PINSEL0 &= ~bit7;
    LPC_PINCON->PINSEL0 |= bit6; 
     
    /* PINMODE 5:4 bits, port 0 pin 2
               7:6 bits, port 0 pin 3
       10: neither pull-up nor pull-down
    */
    LPC_PINCON->PINMODE0 |= bit5;
    LPC_PINCON->PINMODE0 &= ~bit4;
    LPC_PINCON->PINMODE0 |= bit7;
    LPC_PINCON->PINMODE0 &= ~bit6;
     
    /*6. Interrupts: To enable UART interrupts set bit DLAB =0 in register U0/2/3LCR
    (Table 279). This enables access to U0/2/3IER (Table 275). Interrupts are enabled in
    the NVIC using the appropriate Interrupt Set Enable register. */
    LPC_UART0->LCR &= ~bit7; // disables divisor latch access bit 
     
    /*7. DMA: UART0/2/3 transmit and receive functions can operate with the GPDMA
    controller (see Table 543). */
     
}
     
     
unsigned char UART0InChar()
{
    /* UnLSR0 (RDR receive data ready) is set when the UnRBR holds an unread character
       and is cleared when the UARTn RBR FIFO is empty.
    */
    while((LPC_UART0->LSR & bit0) != 0); // while fifo is empty, keep waiting
    return ((unsigned char)(LPC_UART0->RBR & 0xFF)); // bits 8:0
}
 
void UART0OutChar(unsigned char c)
{
    while((LPC_UART0->LSR & bit6) != 0); // transmitter empty TEMT ... or bit5???
    LPC_UART0->THR = c; // write to transmit holding register bits 8:0
}
