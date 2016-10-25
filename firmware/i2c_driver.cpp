#include "i2c.h"
#include "LPC17xx.h"
#include "misc.h"
#include "lcd.h"
#include "onboardleds.h"
 
void i2c1_init()
{
    //myprintf("Starting I2C init");
     
    i2c1_busy = 0;
     
    // page 428  user manual
     
    //1. Power: In the PCONP register (Table 46), set bit PCI2C0/1/2.
    LPC_SC->PCONP |= bit7; //bit7 for PCI2C0
     
    //2. Clock: In PCLKSEL0 select PCLK_I2C1
    LPC_SC->PCLKSEL1 &= ~(bit6 & bit7); // PCLK = CCLK/8
     
    //3. Pins: Select I2C0, I2C1, or I2C2 pins through the PINSEL registers.
    LPC_PINCON->PINSEL1 |= (bit6 | bit7); // selecting P0.19 as SDA1
    LPC_PINCON->PINSEL1 |= (bit8 | bit9); // selecting P0.20 as SCL1
     
    fdiv = SystemCoreClock / MODE_400kbps;
    // duty cycle high half word
    LPC_I2C1->I2SCLL = fdiv >> 1; // fdiv / 2
    // duty cycle low half word
    LPC_I2C1->I2SCLH = fdiv - (fdiv >> 1); // compensate for odd dividers
 
    //4. Interrupts are enabled in the NVIC
    NVIC_SetPriority(I2C1_IRQn, 1);
    NVIC_EnableIRQ(I2C1_IRQn);
     
    // Enable master only functions
    LPC_I2C1->I2CONSET = 0x40;
}
 
uint32_t i2c1_receive(uint8_t address, uint8_t* buffer, uint32_t length)
{
    if (i2c1_busy == 1)
    {
        myprintf("I2C1 ERROR: tried to receive while bus busy");
        return 0;
    }
    i2c1_busy = 1;
     
    // setup pointers
    slave_address = ((address << 1) | 0x01); //????
    buf = buffer;
    buf_len = length;
    num_transferred = 0;
  
    // trigger a start condition
    LPC_I2C1->I2CONSET = STA;
  
    // wait for completion
    while (i2c1_busy);
  
    // get how many bytes were transferred
    myprintf("I2C1 INFO: transfer completed");
    return num_transferred;
}
  
uint32_t i2c1_send(uint8_t address, uint8_t* buffer, uint32_t length) {
    // check software FSM
    flashOnboardLED(1,1000000);
     
    if (i2c1_busy)
    {  
        myprintf("I2C1 ERROR: tried to send while bus busy");
        return 0; // program should just die
    }
     
    // set to status to 'busy'
    i2c1_busy = 1;
     
    // setup pointers
    slave_address = address;//<<1;
    buf = buffer;
    buf_len = length;
    num_transferred = 0;
  
    // trigger a start condition
    flashOnboardLED(2,1000000);
    LPC_I2C1->I2CONSET = STA;
    flashOnboardLED(3,1000000);
     
    // wait for completion
    while (i2c1_busy);
    flashOnboardLED(4,1000000);
     
    // get how many bytes were transferred
    myprintf("send");
    return num_transferred;
}
 
extern "C" void I2C1_IRQHandler(void) {
    flashOnboardLED(1,1000000);
    // get reason for interrupt
    uint8_t status = LPC_I2C1->I2STAT;
  
    // ignore data nack when control is true
    if ((status == 0x30) && (ignore_data_nack))
            status = 0x28;
  
  
    switch(status) {
  
    // Int: start condition has been transmitted
    // Do:  send SLA+R or SLA+W
    case 0x08:
        LPC_I2C1->I2DAT = slave_address; // formatted by send or receive
        LPC_I2C1->I2CONCLR = STA | SI;
        break;
  
    // Int: SLA+W has been transmitted, ACK received
    // Do:  send first byte of buffer if available
    case 0x18:
        if (num_transferred < buf_len) {             
            LPC_I2C1->I2DAT = buf[0];
            LPC_I2C1->I2CONCLR = STO | STA | SI;
        }
        else {
            LPC_I2C1->I2CONCLR = STA | SI;
            LPC_I2C1->I2CONSET = STO;
        }
        break;
  
    // Int: SLA+W has been transmitted, NACK received
    // Do:  stop!
    case 0x20:
        LPC_I2C1->I2CONCLR = STA | SI;
        LPC_I2C1->I2CONSET = STO;
        num_transferred = 0xFFFFFFFF;
        i2c1_busy = 0;
        break;
  
    // Int: data byte has been transmitted, ACK received
    // Do:  load next byte if available, else stop
    case 0x28:
        num_transferred++;
        if (num_transferred < buf_len) {
            LPC_I2C1->I2DAT = buf[num_transferred];
            LPC_I2C1->I2CONCLR = STO | STA | SI;
        }
        else {
            LPC_I2C1->I2CONCLR = STA | SI;
            LPC_I2C1->I2CONSET = STO;
            i2c1_busy = 0;
        }
        break;
  
    // Int: data byte has been transmitted, NACK received
    // Do:  stop!
    case 0x30:
        LPC_I2C1->I2CONCLR = STA | SI;
        LPC_I2C1->I2CONSET = STO;
        i2c1_busy = 0;
        break;
  
    // Int: arbitration lost in SLA+R/W or Data bytes
    // Do:  release bus
    case 0x38:
        LPC_I2C1->I2CONCLR = STO | STA | SI;
        i2c1_busy = 0;
        break;
  
    // Int: SLA+R has been transmitted, ACK received
    // Do:  determine if byte is to be received
    case 0x40:
        if (num_transferred < buf_len) {             
            LPC_I2C1->I2CONCLR = STO | STA | SI;
            LPC_I2C1->I2CONSET = AA;
        }
        else {
            LPC_I2C1->I2CONCLR = AA | STO | STA | SI;
        }
        break;
  
    // Int: SLA+R has been transmitted, NACK received
    // Do:  stop!
    case 0x48:
        LPC_I2C1->I2CONCLR = STA | SI;
        LPC_I2C1->I2CONSET = STO;
        num_transferred = 0xFFFFFFFF;
        i2c1_busy = 0;
        break;
  
    // Int: data byte has been received, ACK has been returned
    // Do:  read byte, determine if another byte is needed
    case 0x50:
        buf[num_transferred] = LPC_I2C1->I2DAT;
        num_transferred++;
        if (num_transferred < buf_len) {
            LPC_I2C1->I2CONCLR = STO | STA | SI;
            LPC_I2C1->I2CONSET = AA;
        }
        else {
            LPC_I2C1->I2CONCLR = AA | STO | STA | SI;
        }
        break;
  
    // Int: data byte has been received, NACK has been returned
    // Do:  transfer is done, stop.
    case 0x58:
        LPC_I2C1->I2CONCLR = STA | SI;
        LPC_I2C1->I2CONSET = STO;
        i2c1_busy = 0;
        break;
  
    // something went wrong, trap error
    default:
        while (1); 
        break;
  
    }
}
