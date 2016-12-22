
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


# Chapter 4 thumb instruction set

Only r0-r7

...

# Chapter 5 Efficient C
```
