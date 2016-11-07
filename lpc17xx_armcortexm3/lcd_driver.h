#include "lcd.h"
#include "misc.h"
#include "systick.h"
 
 
void send(int byte)
{
    //systick_tenmsdelay(1);
    E |= Ebit;
    DB = (byte >> 4); // most significant bits
     
    systick_tenmsdelay(1);
    E &= ~Ebit;
    systick_tenmsdelay(1);
    E |= Ebit;
     
    DB = (byte); // least significant bits 
        
    systick_tenmsdelay(1);
    E &= ~Ebit;
    systick_tenmsdelay(1);
    E |= Ebit;
    systick_tenmsdelay(1);
}
 
 
void sendCommand(int byte) {
    RS &= ~RSbit; // 0 for commands
    send(byte);
}
 
void sendData(int byte) {
    RS |= RSbit; // 1 for data
    send(byte);
}
 
void myhexprintf(uint8_t numb)
{
    uint8_t a = numb / 0x10;
    uint8_t b = numb % 0x10;
    uint8_t arr[3] = {a, b, 0x0};
    myprintf((char *)numb);
 }
  
void myprintf(char *p)
{
    clear();
    int row = 0;
    int column = 0;
    while (*p != '\0')
    {
        if (*p == '\n')
        {
            row++;
            column = 0;
        }
        else
            printchar(*p, row, column);
         
        column++;
        if (column > 15) // max 16 columns (0-15)
        {
            column = 0;
            row++;
        }
        if (row > 1) // max 2 rows (0-1)
        {
            //column = 0;
            //row = 0;
            return;
        }
        p++;
    }
}    
         
void printchar(char data, int row, int column) {
    int address = 0x80 + (row * 0x40) + column;
    sendCommand(address);
    sendData(data);
}
 
void clear(void) 
{
    sendCommand(0x01);
}
 
void lcdInit(void)
{
    systick_tenmsdelay(50);
     
    E |= Ebit;
    RS &= ~RSbit; // 0 for commands
     
    systick_tenmsdelay(1);
 
    int i;
    for (i = 0 ; i < 3; i++) {
        sendCommand(0x3); // set 4 bits (0x28 cant be sent over 4 data bus)
        systick_tenmsdelay(1);
    }
    sendCommand(0x2);  //cursor home?
     
    systick_tenmsdelay(1);
    sendCommand(0x28); /*function set: 1
                         DL = 0 4bit
                         N = 1 2 lines
                         F = 0 5x8 dots */
    //systick_msdelay(1);
    sendCommand(0x0C); /* display control 1
                          D = 1  display on
                          C = 0 cursor off
                          B = 0 cursor blink off */
                           
    //systick_msdelay(1);
    sendCommand(0x6); /* Entry mode set 1
                          ID = 1 increment cursor position
                          S = 0   no display shift */
    systick_tenmsdelay(1);
    sendCommand(0x01); /* clear display */
     
    systick_tenmsdelay(10);
 
}
