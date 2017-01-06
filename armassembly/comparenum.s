.data

format: .asciz "%d is greater than %d\n"
a:      .word 24
b:      .word 13

.text
.global main

/* Conditional branching: set condition flags in status register  
   with 'cmp' or if suffix 's' to instruction
        BEQ = branch if Z set, BNE = branch if Z clear
        BCS = branch if Carry set, BCC = branch if Carry clear
        BMI = branch if N set, BPL = branch if N clear
        BVS = branch if V set, BVC = branch if V clear
        BHI = branch if C set and Z clear, BLS = branch if C clear and Z set

    Compare Condition           Signed Unsigned
    greater than or equal       BGE    BCC
    greater than                BGT    BHI
    equal                       BEQ    BEQ
    not equal                   BNE    BNE
    less than or equal          BLS    BLS
    less than                   BLT    BCS

   Condition flags:
        Negative: set if signed value is negative, clear otherwise
        Zero: set if result is 0
        Carry: set if result is unsigned overflow, unsigned underflow, or value shifted out of shifter
        oVerflow: set if signed overflow
*/



main:
        push    {ip, lr}
        ldr     r0, =format
        ldr     r4, =a
        ldr     r4, [r4]
        ldr     r5, =b
        ldr     r5, [r5]
        cmp     r4, r5  @ updates condition flags
        mov     r1, r4  @ assuming 'a' greater
        mov     r2, r5
        bhi     print   @ if result wasnt zero and carry set
        mov     r2, r4  @ 'b' greater
        mov     r1, r5

print:
        bl      printf

        mov r0, #0
        pop     {ip, pc}
