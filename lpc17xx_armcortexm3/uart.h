#ifndef uart_h_
#define uart_h_
 
#include "LPC17xx.h"
#include "misc.h"
 
void UART0Init(void);
unsigned char UART0InChar(void);
void UART0OutChar(unsigned char data);
 
#endif
