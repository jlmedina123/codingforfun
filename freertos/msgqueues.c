#include "stdio.h"
#include "FreeRTOS.h"
#include "io.hpp"
 
#define NUMB_TASKS 2
#define TASK_STACK_SIZE 1024*2
#define TASK_PRIORITY 5
 
void task1(void *);
void task2(void *);
 
QueueHandle_t queue;
 
int main(void)
{
    queue = xQueueCreate(4, sizeof(uint8_t));
 
    TaskHandle_t task_handlers[NUMB_TASKS];
    const char * const task_names[] = {"task1", "task2"};
    TaskFunction_t task_pointers[] = {task1, task2};
    int i;
 
    for (i = 0; i < NUMB_TASKS; i++) {
       if (xTaskCreate(task_pointers[i],
                       task_names[i],
                       TASK_STACK_SIZE,
                       NULL,
                       TASK_PRIORITY,
                       &task_handlers[i]) != pdPASS)
            printf("Failed to create task %s\n", task_names[i]);
       else
           printf("Task %s created\n", task_names[i]);
    }
 
 
    vTaskStartScheduler();
    return -1;
}
 
// switch monitor task
void task1(void *arg) {
    uint8_t switch_numb = 0;
    while (1) {
        if (SW.getSwitch(1)) {
            switch_numb = 1;
        } else if (SW.getSwitch(2)) {
            switch_numb = 2;
        } else if (SW.getSwitch(3)) {
            switch_numb = 3;
        } else if (SW.getSwitch(4)) {
            switch_numb = 4;
        } else {
            vTaskDelay(100);
            continue;
        }
 
        if(!xQueueSend(queue, &switch_numb, 100)) {
            printf("Can't send to queue %u\n", switch_numb);
        }
        vTaskDelay(300); // avoid button debouncing
    }
}
 
// switch handler
void task2(void *arg) {
    uint8_t switch_numb = 0;
    while(1) {
        if (xQueueReceive(queue, &switch_numb, portMAX_DELAY)) {
            switch (switch_numb) {
                case(1):
                        /* Display temperature */
                        printf("Temperature %.2f\n", TS.getFarenheit());
                        break;
                case(2):
                        /* Display light */
                        printf("Light %u\n", LS.getPercentValue());
                        break;
                case(3):
                        /* Display X-axis */
                        printf("X acceleration %d\n", AS.getX());
                        break;
                case(4):
                        /* infrared */
                        printf("Infrared %u\n", IS.getLastIRCode());
                        break;
                default:
                        printf("dont know what to do with this: %u\n", switch_numb);
            }
        } else {
            printf("Queue receive failed\n");
        }
    }
}
