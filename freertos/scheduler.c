#include "LPC17xx.h"
#include "core_cm3.h"
#include "bit_manip.h"
 
#define NUMB_TASKS 2
#define HW32_REG(ADDRESS) (*((volatile unsigned long *)(ADDRESS)))
#define stop_cpu __breakpoint(0)
 
 
 
volatile uint32_t systick_count = 0;
 
long long task0_stack[1024]; //8 bytes * 1024
long long task1_stack[1024];
long long task2_stack[1024];
long long task3_stack[1024];
 
uint32_t curr_task = 0;
uint32_t next_task = 1;
uint32_t PSP_array[4];
 
 
void task0(void);
void task1(void);
 
void PendSV_Handler(void) {
    __asm volatile(
    "MRS R0, PSP\n"
    "STMDB   R0!,{R4-R11}\n"
    "LDR R1,=__cpp(&curr_task)\n" // load curr_task into R1
    "LDR R2,[R1]\n"
    "LDR R3,=__cpp(&PSP_array)\n"
    "STR R0,[R3, R2, LSL #2]\n" // store register into memory
 
    "LDR R4,=__cpp(&next_task)\n"
    "LDR R4,[R4]\n"
    "STR R4,[R1]\n"
    "LDR R0,[R3, r4, LSL #2]\n"
    "LDMIA R0!,{R4-R11}\n"
    "MSR PSP, R0\n"
    "BX LR\n"
    "ALIGN 4\n"
            );
}
 
void SysTick_Handler(void) {
    systick_count++;
    if (curr_task == NUMB_TASKS)
        next_task=0;
    else
        next_task=curr_task+1;
 
 
    if (curr_task != next_task)
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
 
}
 
int init_sys(void)
{
    SCB->CCR |= SCB_CCR_STKALIGN_Msk; // enable double word stack alignment
 
    PSP_array[0] = ((unsigned int)task0_stack) + (sizeof task0_stack) - 16*4; // top of stack
    HW32_REG((PSP_array[0] + (14<<2))) = (unsigned long)task0; // initial Program Counter
    HW32_REG((PSP_array[0] + (15<<2))) = 0x01000000;           // initial PSR
 
    PSP_array[1] = ((unsigned int)task1_stack) + (sizeof task1_stack) - 16*4;
    HW32_REG((PSP_array[1] + (14<<2))) = (unsigned long)task1;
    HW32_REG((PSP_array[1] + (15<<2))) = 0x01000000;
 
    curr_task = 0;
    __set_PSP((PSP_array[curr_task] + 16*4)); // set PSP to top of stack
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    SysTick_Config(168000);
 
    __set_CONTROL(0x3);
 
    __ISB();
 
    task0();
 
    /*while(1) {
        stop_cpu;
    }; */
}
 
int main()
{
    init_sys();
    while(1);
    return -1;
}
 
 
void task0(void) {
    uint32_t count = 0x00000000;
    while(1)
        if (count == 0xFFFFFFFF)
            count = 0x00000000;
        else
            count++;
}
 
 
void task1(void) {
    uint32_t count = 0x00000000;
    while(1)
        if (count == 0xFFFFFFFF)
            count = 0x00000000;
        else
            count++;
}
