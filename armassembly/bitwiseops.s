.data

format: .asciz  "0x%x\n"
input:  .word   0x11223344

.text
.global main

loadinput:
        push    {ip, lr}
        ldr     r1, =input
        ldr     r1, [r1]
        ldr     r0, =format
        pop     {ip, pc}

main:
        push    {ip, lr}

        @ print original number
        bl      loadinput
        bl      printf

        @ print number shifted left one bit
        bl      loadinput
        mov     r1, r1, lsl #1
        bl      printf

        @ print second nibble of first byte
        bl      loadinput
        mov     r3, #0x11
        and     r1, r1, r3
        @mov    r1, r1, lsr #4
        bl      printf

        @ print one's compliment (inverse)
        bl      loadinput
        mvn     r1, r1
        bl      printf

        mov     r0, #0 
        pop     {ip, pc}
