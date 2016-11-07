#include <stdio.h>      // printf() / scanf()
#include "FreeRTOS.h"
 
#include "utilities.h"  // delay_ms()
#include "task.h"
#include "LPC17xx.h"
#include "LED_Display.hpp"
#include "io.hpp" // LD
 
#define NUMB_TASKS 1
#define TASK_STACK_SIZE 1024*10
#define TASK_PRIORITY 5
#define MAX_CONSECUTIVE 5
 
#define THRESHOLD_HUMIDITY 10 // watering will start if humidity less than this in %
#define MAX_VALUE 0x8ff // output from sensor in water (0x8e1)
 
void task1(void *);
void pump_on();
void pump_off();
void init_sensor();
void init_gpio();
 
 
int main(void)
{
    xTaskCreate(task1, "task1", 1024, NULL, 4, NULL);
    vTaskStartScheduler();
}
 
void init_sensor() {
    LPC_SC->PCONP |= 1 << 12; // power up adc converter
    LPC_ADC->ADCR = (1<<3) | (2 << 8) | (1 << 21); // selec ad0.3 | clk | operational
    LPC_PINCON->PINSEL1 |= (1<<20); // selec AD0.3 for P0.26
    LPC_PINCON->PINSEL1 &= ~(1<<21);
}
 
void init_gpio() {
    // P2.0
    LPC_PINCON->PINSEL4 &= ~(1<<0|1<<1);
    LPC_GPIO2->FIODIR |= (1<<0);
    LPC_PINCON->PINMODE_OD2 |= (1<<0); // open-drain
}
 
void pump_on() {
    LPC_GPIO2->FIOSET = 1<<0;
}
 
void pump_off() {
    LPC_GPIO2->FIOCLR = 1<<0;
}
 
void task1(void *args)
{
    unsigned int value;
    unsigned int humidity;
    unsigned int times_consecutive = 0;
 
    init_sensor();
    init_gpio();
 
    vTaskDelay(5000/portTICK_PERIOD_MS);
    printf("Starting conversion monitoring...\n");
    while(1)
    {
            // start a sample
            LPC_ADC->ADCR |= 1 << 24;
            LPC_ADC->ADCR &= ~(1<<25);
            LPC_ADC->ADCR &= ~(1<<26);
            while((LPC_ADC->ADDR3 & (1 << 31)) == 0);
 
            value = (LPC_ADC->ADDR3 >> 4) & 0xfff;
            humidity = ((value*100)/MAX_VALUE);
 
            // display humidity percentage in LED display
            LD.setNumber((char)humidity);
            printf("Humidity: %u% (0x%x)\n", humidity, value);
 
            // water plant if needed
            if (humidity < THRESHOLD_HUMIDITY && times_consecutive < MAX_CONSECUTIVE) {
                printf("Watering plant for 5 secs\n");
                times_consecutive++;
                pump_on();
                vTaskDelay(5000/portTICK_PERIOD_MS);
                printf("Stopping for 10 secs\n");
                LD.setRightDigit('-');
                LD.setLeftDigit('-');
                pump_off();
                vTaskDelay(10000/portTICK_PERIOD_MS);
            } else {
                times_consecutive = 0;
                printf("Taking 10 sec break\n");
                vTaskDelay(10000/portTICK_PERIOD_MS);
            }
      }
}
