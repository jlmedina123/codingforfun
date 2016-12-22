
http://www.coranac.com/tonc/text/asm.htm


Conditions -> stored in PSR
* Z: result of operation 0
* N: result of operation negative (most significant bit set)
* C: Carry bit set (most significant bit set)
* V: overflow
```
// Asm version of for(A=0; A != 16; A++)
    mov     A, #0
for_start:
    add     A, #1
    cmp     A, #16  // Compare A to 16
    bne for_start   // Branch to beginning of loop if A isn't 16
```

Line formats
* directives: `.xxx`
* functions
    * preceded by directives for alignment, code section, instruction set, `.global`, label
    * thumb functions also preceded by `.thumb_func`, `.code 16`, `.thumb`
    * calling function: bl, arguments r0-r3
    * returning from function: bx

Registers
* r0-r3: argument/scratch
* r4-r11: variable
* r12: intra-procedure-call
* r13: SP
* r14: LR
* r15: PC

Instructions
* data operations (arithmetic, etc)
* memory operations (store, load)
* branches

Barrel shift:
* operations: lsl, lsr, asr, ror
* applications: `x*9 = x + (x<<3)`
* shift by value costs 0 cycles, shift by register 1 cycle

Immediate values:
* ARM instruction 32 bits, 4 bits condition, ... -> 12 bits for immediate value
* bigger numbers 

    
