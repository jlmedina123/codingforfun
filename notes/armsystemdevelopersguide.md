
# Chapter 1

RISC:
* instructions: reduced number, can execute in single cycle, fixed lenght instructions
* pipelines: instruction processing broken down in pipeline stages, processed in parallel
* large number of GPRs
* load-store arch

ARM: 
* low power, high density code
* All peripherals memory mapped

ARM different from pure RISC:
* not every instruction takes one cycle
* inline barrel shifter: more complex instructions
* thumb 16-but instruction set: 16 or 32 bit instructions



# Chapter 2

Implementations
* Von Neumann: data and instructions in same bus
* Harvard: data nad instructions separate buses

Dataflow:
* register file, barrel shifter, MAC, ALU, address register, incremementer, ...

Registers (register file):
* r0-r12 GPRs
* r13 SP, r14 LR, r15 PC
* cpsr, spsr

Modes:
* 6 privileged
* 1 nonprivileged

Banked registers:
* r8 to r15

Core states:
* ARM (arm instruction set), thumb (thumb 16-bit instruction set), jazelle (java bytecodes)
* cpsr reflects state of processor

Interrupt masks: cpsr bits 6-7 mask IRQs and FIQs

Condition flags
* updated by comparisons and results of ALU operations with S instruction (eg SUBS
* flags: saturation, overflow, carry, zero, negative

Pipeline:
* three stages: fetch, decode, execute
* longer pipeline -> simpler stages -> higher frequency, but more latency
* ARM9 pipeline: fetch, decode, execute, memory, write
* ARM10 pipeline: fetch, issue, decode, execute, memory, write

Vector table: 
* each entry has a form of branch instruction pointing to start of routine
* routines: reset vector, undefined unstruction vector, IRQ vector, ...

Hardware core extensions
* cache: two levels:
    * L1: data and instructions unified (von neumann)
    * L2: dat and cached separated (harvard)



# Chapter 3 ARM instruction set

```
# move and barrel shift
MOV r7, r5         ; r7 <- r5
MOV r7, r5, LSL #2 ; r7 <- r5<<2 (=r5*4), logical shift left free...

# arithmetic
SUB r0, r1, r2     ; r0 <- r1 - r2
ADD r0, r1, r1, LSL #1  ; r0 <- r1 + r1<<1
ADDEQ r0, r1, r2   ; r0 <- r1 + r2 if zero flag set

# logical
BIC r0, r1, r2     ; r0 <- r1 & ~r2, logical bit clear

# comparison
CMP r0, r9         ; if r0 == r9, set z flag in cpsr register

# load-store
LDR r0, [r1, #10]  ; r0 <- mem[r1 + 10]
LDR r0, [r1, r2]   ; r0 <- mem[r1 + r2]   : preindex
LDR r0, [r1], r2   ; r1 <- r1 + r2, r0 <- mem[r1]  : postindex

# stack: ascending, descending, full (sp to last used), empty (sp to first unused)
STMFD sp!, {r1, r4}  ; push full descending
```

# Chapter 4 thumb instruction set

Only r0-r7

...

# Chapter 5 Efficient C

32-bit registers:
* Load always loads 32-bits: 
    * casting loaded char to int does not cost extra instruction (same for int to char)
    * load for 8 or 16 bits extended before written to register
        * unsigned values are zero extended
        * signed values are signed extended
* ARMv5 added instructions for load 64 bits
* char is unsigned ->  char i >= 0 is infinite loop, armcc gives warning (unsigned comparison with 0)

## C types
* avoid char/short local variables -> take 32-bit in stack, plus extra instruction for checking range
* use smallest type for array and global variables
* avoid casts in expressions, casts in load/store are free
* avoid char/short for function arguments or returns (avoids extra casts)
* use unsigned for divisions

## C looping
* use unsigned loop counter with condition i != 0
* unroll loop (repeat loop body and reduce loop iterations)

## Register allocation
* Up to 12 variables -> allocated in registers
* More variables -> allocated on stack -> spilled variables -> slow

## Function calls
* limit arguments to 4: first four arguments in registers (r0-r3), subsequent in stack
* if more than 4 arguments -> use pointer to struct

## Pointer aliasing
* avoid subexpressions that access memory, use new local variable for that
* avoid taking address of local variable

## Others...
* for structs, order elements in increasing size (char to long)
* avoid bit-fields, use defines instead

```
typedef struct {
    unsigned int sA : 1;
    unsigned int sB : 1;
    unsigned int sC : 1;
} stages_v1;
stages_v1 *stage;
stage->sA;
...

// instead
typedef unsigned long stages_v1;
#define sA (1uL << 0)
#define sB (1uL << 1)
#define sC (1uL << 2)
staves_v1 *stage;
stage & sA;
...

```

* if data endianess makes difference, use char * to access it
* avoid division (and modulus) in circular loops:

```
offset = (offset + increment) % buffer_size;
// instead:
offset += increment;
if (offset >= buffer_size) offset -= buffer_size;
```

* avoid floating point (usually it's C library, not hardware), use fixed-point instead
* use inline functions for small functions


# Chapter 6: Assembly code for optimizations


## instruction scheduling: avoid process stall due to pipeline hazard

* 5-stage pipeline: fetch, decode, ALU, ls1, ls2


* register allocation: minimize memory accesses
* condition execution

