

# Logical shift (replace upper bits with zeros)
MOV     R0, R1, LSL #2  ; R0 <- R1 << IR(0:7)
MOV     R0, R1, LSR #2  ; R0 <- R1 >> IR(0:7)
MOV     R0, R1, LSL R2  ; R0 <- R1 << R2(0:7)

# Arithmetic shift (maintains value of most significant bit, sign is maintained)
MOV     R0, R1, ASR #2  ; R0 <- R1 << IR(sign maintained)
# LSL = ASL, so ASL doesnt exist

MOV     R0, R1, ROR #2

# addressing
```
LDR     R0, [R1]        ; no offset
LDR     R0, [R1, #4]    ; immediate offset
LDR     R0, [R1, R2]    ; register offset: R0 <- Mem[R1+R2]
LDR     R0, [R1, R2, LSL #2]    ; scaled register offset
LDR     R0, [R1, #4]!   ; immediate pre-indexed. R1 written back R1 <- R1+IR
LDR     R0, [R1, R2]!   ; register pre-indexed. R1 <- R1+R2, R0 <- Mem[R1]
LDR     R0, [R1], #4    ; immediate post-indexed. R0 <- Mem[R1], R1 <- R1+IR
```


Move 16 bits from value to result

```
        TTL     move16  ; title directive
        AREA    Program, CODE, READONLY ; assemble new code or data area 
                                        ; Program: name of the area
                                        ; CODE: contains machine instructions
                                        ; READONLY: area should not be written to (default)
        ENTRY               ; directive tells assembler where first instruction 
Main
        LDRB    R1, Value   ; Load (LDR) unsigned byte (B)
        STR     R1, Result  ; Store (STR) word (default)
        SWI     &11         ; software interrupt function number 17 (&11), exit() for demo system
                            ; & is DCD (allocate word aligned to 4-byte boundary) ???
Value   DCW     &C123       ; allocates a halfword (W) aligned to two-byte boundary, and defines intial
                            ; runtime content of the memory
        ALIGN               ; needed so next allocation is aligned
Result  DCW     0

        END     
```

Invert (one's compliment) of value

```
        TTL     invert
        AREA    Program, CODE, READONLY
        ENTRY







