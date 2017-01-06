.data

format: .asciz "%d + %d = %d\n"
a:      .word 11
b:      .word 15

.text
.global main

@ sumfunc: gets input in r1 r2, sums them
@ and returns value in r0
sumfunc:
        push    {ip, lr}
        add     r0, r1, r2
        pop     {ip, pc}

@ printfunc: input (total sum) in r1
printfunc:
        push    {ip, lr}
        mov     r3, r1
        ldr     r0, =format
        ldr     r1, =a
        ldr     r1, [r1]
        ldr     r2, =b
        ldr     r2, [r2]
        bl      printf
        pop     {ip, pc}

@ main
main:
        push    {ip, lr}
        @ load input arguments
        ldr     r1, =a
        ldr     r1, [r1]
        ldr     r2, =b
        ldr     r2, [r2]
        @ call sumfunc
        bl      sumfunc         @ branch with link, continues in next line after subroutine
        @ call printfunc
        mov     r1, r0
        bl      printfunc
        pop     {ip, pc}
