#include <stdio.h>      // printf() / scanf()
#include "FreeRTOS.h"
 
#include "utilities.h"  // delay_ms()
#include "task.h"
#include "LPC17xx.h"
 
#define NUMB_TASKS 2
#define TASK_STACK_SIZE 1024
#define TASK_PRIORITY 5
 
void task1(void *);
void task2(void *);
void task3(void *);
void task4(void *);
 
int main(void)
{
   TaskHandle_t task_handlers[NUMB_TASKS];
   const char * const task_names[] = {"task1", "task2", "task3", "task4"};
   TaskFunction_t task_pointers[] = {task1, task2, task3, task4};
   int i;
 
   for (i = 0; i < NUMB_TASKS; i++) {
      if (xTaskCreate(task_pointers[i],
                          task_names[i],
                          (unsigned short)TASK_STACK_SIZE,
                          NULL,
                          (UBaseType_t)TASK_PRIORITY,
                          task_handlers) != pdPASS)
           printf("Failed to create task %s\n", task_names[i]);
      else
          printf("Task %s created\n", task_names[i]);
   }
   vTaskStartScheduler();
 
}
 
void task3(void *args) {
    return;
}
void task4(void *args) {
    return;
}
 
/* flash LED */
void task2(void *args)
{
    LPC_GPIO1->FIODIR |= (1 << 0); // set P1.0 as output
    while (1) {
        LPC_GPIO1->FIOSET = (1<<0);
        delay_ms(1000);
        LPC_GPIO1->FIOCLR = (1<<0);
        delay_ms(1000);
    }
}
/* turn on LED when button pressed */
void task1(void * args)
{
           // 1) Pin selection: make P1.9 as GPIO
           LPC_PINCON->PINSEL2 &= ~((1<<18)|(1<<19));
 
           // 2) direction
           LPC_GPIO1->FIODIR &= ~(1<<9); // set P1.9 as input
           LPC_GPIO1->FIODIR |= (1 << 0); // set P1.0 as output
 
           while (1) {
               //FIOPIN for reading 32 bits, FIOPIN1 to reading first 8 bits, ...
               if (LPC_GPIO1->FIOPIN & (1<<9)) {
                   LPC_GPIO1->FIOSET = (1<<0);
               } else {
                   LPC_GPIO1->FIOCLR = (1<<0);
               }
               delay_ms(100); // to avoid issues due to bounce button
          }
}
