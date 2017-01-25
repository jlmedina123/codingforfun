/* http://www.ethernut.de/en/documents/arm-inline-asm.html
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    unsigned int y = 0x0000FFFF;
    unsigned int x = 0;
    printf("x: 0x%x, y: 0x%x\n", x, y);

    // asm(code                              : output operands   : input operands : clobbered registers);
    //asm("mov %[result], %[value], ror #1" : [result] "=r" (y) : [value] "r" (x));

    // operand: [symbolic name unique in asm statement] "constraint string" (C expression)
    // before gcc 3.1:
    // asm("mov %0, $1, rot#1" : "=r" (result) :"r" (value));
    // operands: r=register, f=floating register, I=immediate, 
    //           '='=write-only (output), +=read-write output (but inputs data too), &=register for output only
    asm("mov %[result], %[value], ror #1" /* rotate bits by 1 */
        : [result] "=r" (y)               /* put result in y */
        : [value] "r" (x)                 /* value to rotate from x */
        :                                 /* no clobbers */
    );
    printf("x: 0x%x, y: 0x%x\n", x, y);

    // add volatile to avoid compiler optimizations (eg: the next asm statement would be removed by 
    // compiler without volatile
    asm volatile(" mov r0, r0");


    /* https://www.heyrick.co.uk/armwiki/The_Status_register
       CSPR/SPSR registers bits:
       31 30 29 28 27   24   19 … 16   9 8 7 6 5 4 … 0
        N  Z  C  V  Q —  J — GE[3:0] — E A I F T M[4:0]
        I (bit7): set to disable IRQ interrupts
        F (bit 6): set to disable FIQ 
        
        Reading writing to register
          mrs: copy cpsr bits into register
          msr: copy register into cpsr bits
        
        Register sections:
          cpsr_c: control bits (0-7)
          cpsr_x: extension bits (8-15)
          cpsr_s: status bits (16-23)
          cpsr_f: flag bits (24-31)

        Eg: set V flag (bit 28)
          MSR   CPSR_f, #0x10000000
        Or also
          CMP R0, #1<<31
          CMNVC R0, #1<<31

        Eg: set mode
          mrs   r0, cpsr
          bic   r0, r0, #0x1F
          orr   r0, r0, #newmode
          msr   cpsr_c, r0
     */
    int c = 2;
    int b = 3;
    asm volatile("mrs r12, cpsr\n\t"        /* copy CPSR into register */
                 "orr r12, r12, #0xC0\n\t"  /* 0xC0 = 1100 0000b => set interrupt flags bit 6 and 7 */
                 "msr cpsr_c, r12\n\t"      /* copy register to control CPSR flags */
                 : : : "r12", "cc", "memory"); /* clobber registers: inform compiler of use of r12, condition register,
                                                  memory locations (store all cached values and retain sequence) */
    c *= b; /* critical section shared with IRQ */
    asm volatile("mrs r12, cpsr\n\t"
                 "bic r12, r12, #0xC0\n\t" /* clear interrupt flags -> enable interrupts */
                 "msr cpsr_c, r12"
                 : : : "r12", "cc", "memory");


   return 0;
}

/* shifts:
        ASR: arithmetic shift right. Right shift extend sign, sign copied into new bits
        LSL: logical shift left. zeros in new bits
        LSR: logical shift right. zeros in new bits
        ROR: rotate right. bit coming out from right are put into left
*/
unsigned long ByteSwap(unsigned long val)
{
    asm volatile (
        "eor     r3, %1, %1, ror #16\n\t"
        "bic     r3, r3, #0x00FF0000\n\t"
        "mov     %0, %1, ror #8\n\t"
        "eor     %0, %0, r3, lsr #8"
        : "=r" (val)    /* output operands: write only. %0 */
        : "0"(val)      /* input operands: %1 */
        : "r3", "cc"    /* cobblered registers: r3 is used, condition register is used */
    );
    return val;
}
