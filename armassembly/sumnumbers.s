.data

format:         .asciz "%d + %d = %d\n"
a:              .word   11
b:              .word   5
result:         .word   0

.text
.global main

main:
                push    {ip, lr}
                @ load values to sum
                ldr     r1, =a
                ldr     r1, [r1]
                ldr     r2, =b
                ldr     r2, [r2]
                @ sum numbers and store result
                add     r1, r1, r2
                ldr     r2, =result
                str     r1, [r2]
                @ print them
                ldr     r0, =format
                ldr     r1, =a
                ldr     r1, [r1]
                ldr     r2, =b
                ldr     r2, [r2]
                ldr     r3, =result
                ldr     r3, [r3]
                bl      printf

                pop {ip, pc}
