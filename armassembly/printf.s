@ reference:
@ http://cs.smith.edu/dftwiki/index.php/Tutorial:_Assembly_Language_with_the_Raspberry_Pi

.syntax unified

@ --- Data Section ----
.data
number:  .word 0x11223344 @ reserves for 4-bytes items, and set initial value
         .align
format:  .asciz "-%x-\n"  @ assembles strings into consecutive address


@ --- Code Section ---
.text
.global main
main:
    @ Stack the return address (lr) and dummy register (ip) to
    @ keep the stack 8-byte aligned.
    push    {ip, lr}

    @ Load the argument and perform the call. This is like 'printf("...")' in C.
    ldr     r0, =format
    ldr     r1, =number  @ load the address of number
    ldr     r1, [r1]     @ load the contents of that address
    bl      printf

    @ Exit from 'main'. This is like 'return 0' in C.
    mov     r0, #0    @ Return 0.

    @ Pop the dummy ip to reverse our alignment fix, and pop the original lr
    @ value directly into pc — the Program Counter — to return.
    pop     {ip, pc}
