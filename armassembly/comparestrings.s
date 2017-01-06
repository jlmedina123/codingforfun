@ compare two null terminated strings

.data
string1:        .asciz "string1"  @ asciz inserts nul character at end
string2:        .asciz "string"
strsame:        .asciz "Strings are the same"
strdifferent:   .asciz "Strings are different"
newline:        .asciz "\n"

.text
.global main

main:
        push    {ip, lr}

        @ print strings
        ldr     r0, =string1
        bl      printf
        @ mov   r0, '\n'        @ this wouldn't work, printf expects address
        ldr     r0, =newline
        bl      printf
        ldr     r0, =string2
        bl      printf
        ldr     r0, =newline
        bl      printf

        @ compare strings
        ldr     r1, =string1
        ldr     r2, =string2
compareloop:
        cmp     r1, #0
        beq     str1end
        cmp     r2, #0
        beq     different
        ldr     r3, [r1]
        ldr     r4, [r2]
        cmp     r3, r4
        bne     different
        add     r1, r1, #4
        add     r2, r2, #4
        b       compareloop

str1end:
        cmp     r2, #0
        beq     equal
        b       different

equal:
        ldr     r0, =strsame
        bl      printf
        b       done

different:
        ldr     r0, =strdifferent
        bl      printf
        b       done

done:
        ldr     r0, =newline
        bl      printf
        mov     r0, #0
        pop     {ip, pc}

