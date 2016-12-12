
# Chapter 1

## MU0 Simple process

Components:
* PC (program counter): holds address of current instruction
* ACC (accumulator): holds data value while it is worked
* ALU: performs operations on operands
* IR (instruction regi): hold current instruction
* instruction decode and control logic

Instruction set: 4 bits opcode + 12 bits address field
* LDA S : ACC = mem[S]
* JMP S : PC = S
* JGE S : if ACC>0 PC = S
...

Two components
* datapath: designed with RTL registers, multiplexers, etc
* control path: designed with FSM

Datapath design example
Control signals required to cause datapath to carry out full set of operations: reset, ex/ft, ACCz, ACC15, Asel, Bsel, ACCce, PCce, IRce, MEMrq, ALUfs,...
Control logic: decodes current instruction and generates control signals


Addressing modes
* immediate addressing: value in instruction
* absolute addressing: memory address of value in instruction
* indirect addressing: memory address of pointer to value
* register addressing: value in register, instruction has register number
* register indirect addr: register has memory addr of value
* base plus offset addr: instruction has register (with base addr) and offset to add
* base plus index: base register and index register
* base plus scaled index
* stack addressing: SP points to stack where data is pushed or popped

Control flow instructions (modify PC sequence)
* branch or jump
* subroutine call
* syscall
* exceptions

Techniques to speed up processors:
* pipelining
* cache memory
* super-scalar instructions (not used by ARM)

## Pipelines

Steps in instruction execution
1. fetch from memory
2. decode
3. access operands from register bank
4. perform operation with operands (ALU)
5. Access memory for data operand if necessary
6. Write result to register bank

Pipeline hazards
* read-after-write hazard: result from one instruction is operand to next one
* branch instructions -> solutions: speculative, delayed branch

## RISC

difference between RISC and CISC
* fixed instruction size
* load-store architecture: operands must be in registers
* large register bank

Simpler design allowed for:
* hardwired instruction decode logic (CISC uses microcode in ROM)
* pipelined execution (CISC now pipelines as well)
* single-cycle execution
but
* poor code density due to fixed-length instruction set -> uses more memory

Thumb instruction set: 16-bit instructions -> improves code density


# Chapter 2: ARM architecture

Features from RISC:
* load-store architecture
* fixed-lenght 32-bit instructions
* 3-address instruction formats

Features rejected
* register windows
* delayed branching -> remove atomicity of individual instruction
* single-cycle execution -> some instructions take more than one cycle

# Chapter 3: ARM assembly

Classes of instructions
* arithmetic
* bitwise logic
* register movement
* comparison 


TODO: look at hello world and other examples, learn
to code them...


# Chapter 4: ARM pipeline

## 3-stage pipeline (up to 1995)

Components (see diagram):
* register bank: stores processor state
* barrel shifter: shifts or rotates operands
* ALU: performs arithmetic and logic functions
* address register and incrementer: holds memory addresses
* data register and incrementer: holds data to/from memory
* instruction decoder

Stages:
* fetch
* decode
* execute

single instruction and data memory -> access memory every clock -> memory bandwith is performance bottleneck

## 5-stage pipeline

Improvements
* Uses separate instruction and data memories -> faster memory access
* less work per cycle -> can speed up clock


stages:
* fetch: instruction fetched from memory into instruction pipeline
* decode: instruction decoded, register operands read from register file
* execute: ALU result generated, or memory addr computed if load/store
* buffer/data: wait one cycle, or data memory accessed
* write-back: results written back to register file

Data dependencies (pipeline hazard)
* if instruction needs result from predecessor
* forwarding path -> stage passes result to dependent stage as soon as available

Pipeline stage organization (figure 4.4)


## ARM instruction execution

Data processing instructions
* two operands: one register, one register or immediate value
* second operand through barrel shifter
* operands combined in ALU
* result from ALU written back to destination register
* PC value in address register incremented and copied back to addr reg and r15
* next instruction bu one loaded

Data transfer (load/store) instructions
* base address is summed/substracted to offset (register or immediate value)
* address sent to addr register
* data transfer
* ALU computes auto-indexing modifications to base register if required

Branch instructions
* compute target address
* offset added to PC
* addr fetched, and return address copied to link register if required
* correct LR so it points to instruction following branch



## ARM implementation

* Clocking scheme
    * 2-phase non-overlapping clock -> allows use of level-sensity transparent latches
    * data passes through latches open in phase 1, and latches open in phase 2
    * non-overlapping phases -> no race conditions

* Datapath timing
    * Adder: ...

* Control logic components:
    * instruction decoder PLA (programmable logic array)
    * distributed secondary control
    * decentralized control units

## coprocessors

ARM supports extension of instruction set with the addition of hardware coprocessors


# Chapter 10: Memory hierarchy

Composite memory:
* fast cache: retains copies of instructions and data. Effectivness depends on spatil and temporal locality
* slow large main memory

Cache organizations:
* unified cache: single cache for instruction and data
* separate instruction and data caches -> load and store can execute in single clock cycle

Mapping
* direct mapped
* set associate: memory item can be placed anywhere, according to:
    * random
    * least recenttly used
    * round-robin

Memory management:
* segmentation: 
    * every memory access provides segment selector and offset
    * causes fragmentation of free memory because segments have variable size
* paging:
    * uses fixed-size pages of memory for both virtual and physical addresses
    * relations between virtual (logical) and physica addresses in page tables, held in main memory
    * single table would be too large -> use multiple levels of page tables
* demand paging: page marked as absent and exception generated when accessed. OS can write another page to swap and use it

* ARM supports only paging
