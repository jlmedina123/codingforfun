.data

format: .asciz  "0x%x%x + 0x%x%x = 0x%x%x\n"
a:      .word   0x11111111, 0x11111111
b:      .word   0x22222222, 0x22222222

.text
.global main

main:
        push    {ip, lr}

        @ load values from memory
        ldr     r0, =a
        ldr     r1, [r0]
        ldr     r2, [r0, #4]
        ldr     r0, =b
        ldr     r3, [r0]
        ldr     r4, [r0, #4]
        adds    r5, r1, r3      @ add and set Carry flag
        adc     r6, r2, r4      @ add including Carry if set
        @ print
        ldr     r0, =format
        bl      printf

        mov     r0, #0
        pop {ip, pc}
