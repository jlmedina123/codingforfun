.data

liststr:    .asciz "%d, "
maxstr:     .asciz "\nMax %d\n"
table:
            .word 23, 43, 41, 54, 31
endtable:
length:     .word 5

.text
.global main

/* r0 to r12   "r0" to "r12"  
   r13 or sp   "r13" or "sp" 
   r14 or lr   "r14" or "lr"
   r15 or pc   "r15" or "pc"
*/
main:
    push    {ip, lr}

    ldr     r2, =table  @ table pointer
    mov     r4, #0      @ current max
    ldr     r5, =length
    ldr     r5, [r5]    @ counter
    cmp     r5, #0      
    beq     done        @ if table empty, exit

loop:
    @ ldr     r0, =liststr
    @ ldr     r1, [r2]
    @ bl      printf      @ printf messes up the registers...
    ldr     r3, [r2]
    cmp     r3, r4
    bls     nextloop    @ if number less than max, jump
    mov     r4, r3      @ new max
nextloop:
    add     r2, r2, #4  @ increment table pointer
    subs    r5, r5, #1  @ decrement counter, and update condition flags
    bne     loop        @ loop again if counter not zero
done:
    ldr     r0, =maxstr
    mov     r1, r4
    bl      printf

    pop     {ip, pc}
