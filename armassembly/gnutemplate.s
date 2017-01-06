@ http://www.cse.unsw.edu.au/~cs3221/labs/assembler-intro.pdf

.text
_start:     
.global _start
.global main
            b main
main:
            ; code here
            mov     pc, lr
            .end


