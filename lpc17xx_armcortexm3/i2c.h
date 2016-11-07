#ifndef i2c_h_
#define i2c_h_
 
#define MODE_400kbps 400000
#define AA      (1 << 2)
#define SI      (1 << 3)
#define STO     (1 << 4)
#define STA     (1 << 5)
#define I2EN    (1 << 6)
#define ADDR     0x50   // 1010 000
 
#include "LPC17xx.h"  
 
 
void i2c1_init(void);
uint32_t i2c1_send(uint8_t address, uint8_t* buffer, uint32_t length);
uint32_t i2c1_receive(uint8_t address, uint8_t* buffer, uint32_t length);   
   
 
static volatile uint8_t* buf; // static not needed if members of class, but needed for global vbles
 
static volatile uint32_t i2c1_busy;
static volatile uint8_t slave_address;  
static volatile uint32_t buf_len; 
static volatile uint32_t num_transferred; 
static unsigned int fdiv;
static uint32_t ignore_data_nack = 1; 
 
#endif


