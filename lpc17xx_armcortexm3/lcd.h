#ifndef lcd_h_
#define lcd_h
 
#include "gpio.h"
#include "systick.h" //for delay
 
#define RS myFIO0PIN //P0.24 - DIP16
#define RSbit bit24
 
#define E  myFIO0PIN //P0.23 - DIP15
#define Ebit bit23
 
/* D4: P2.0 DIP26  lsb  -  D4
   D5: P2.1 DIP25       -  D5
   D6: P2.2 DIP24       -  D6
   D7: P2.3 DIP23  msb  -  D7
 */
#define DB myFIO2PIN //P2.0 - P2.3
 
 
void send(int byte);
void sendCommand(int byte);
void sendData(int byte);
void myprintf(char *p);
void myhexprintf(uint8_t numb);
void printchar(char data, int row, int column);
void lcdInit(void);
void clear(void);
 
 
#endif
