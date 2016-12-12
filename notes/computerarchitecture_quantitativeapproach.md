
# Chapter 1

performance techniques
* instruction-level parallelism: pipelining and mutiple instruction issue
* caches

Application paralelism
* instruction-level: pipelining
* vector architectures and GPU: single instruction to a collection of data in parallel
* thread-level parallelism: interaction amon parallel threads
* request-level: among large decouple tasks

Parallelism categories:
* SISD (single instruction stream, single data stream): uniprocessor
* SIMD (single instruction stream, multiple data stream): same instruction, multiple data items in parallel
* MISD: doesnt exist
* MISMD: each process its own instruction and its own data, task-level parallelism. More expensive than SIMD

ISA: x86, ARM, MIPS
* Operations: data transfer, arithmetic logical, control, floating point
* Control flow instructions: conditional branches, unconditional jumps, procedure calls, returns
* Encoding: fixed length, variable length

# Chapter 2: Memory Hierarchy Design

Locality: most programs dont access all code or data uniformly. Spatial and Temporal
Inclusion property: data from higher level is superset in lower level

Line = block = multiple words, moved due to spatial locality
Each line a tag with memory address
Where to place lines in cache:
* Set associative:
   * set: group of blocks in cache
   * block mapped to set, and placed anywhere in set
   * n-way set associative: n blocks in a set
* direct mapped cache: one block per set, block always placed in same set
* fully associate: just one set, block placed anywhere

Caching writes: can produce inconsistency
* write-through: cache updates itme in cache and writes update to main memory
* write-back: update copy only in cache. write to memory when its replaced

Causes for high miss rates:
* compulsory: first access
* capacity: cache cant fit all blocks needed
* conflict: for set associate when multiple blocks map to same set

Basic cache optimizations:
* larger block size: reduces compulsory misses, increases miss penalty
* bigger caches: more expensive
* higher associativity: reduces conflict miss, increases hit time
* multileve caches: fast cache but smaller for higher levels, slower but bigger for lower levels
* priority to read misses
* avoid translation virtual to physical. can use page offset, which is same in both addresses, to index cache -> no need for TLB

## Advanced cache performance optimization

Goals
* reduce hit time: 
    * first level cache, 
    * predict next block and preset multiplexer
* increase cache bandwith: 
    * pipelined cache, 
    * multibanked cache: divide cache into banks that support simultaneous access 
    * nonblocking caches: (continue supplying cache hits during a miss, hit under miss
* reduce miss penalty: 
    * critical word first: when requesting block, request miss word first
    * early restart: send miss word to processor as soon as it arrives, dont wait for rest of block
* reduce miss rate: 
    * compiler optimizations: 
	* loop interchange: go through memory within strides, dont jump from stride to stride
        * blocked algorithms: maximize access to data loaded in cache
    * hardware prefetching of instruction or data
* parallelism

techniques:

cache-hit process:
* addressing tag memory using index portion of addr
* compare read tag to addr
* set multiplexer to correct data item

## Memory technology and optimizations

Memory latency: 
* access time (time between read request and data arrive)
* cycle time: minimum time between unrelated requests

SRAM
* for cache memory
* dram requires data written back after a read, sram doesnt need refresh -> access time = cycle time
* 6 transistors for 1 bit
* minimal power to retain charge in standby mode

DRAM
* for main memory
* cost of address lines high -> multiplex them to reduce lines to half
* row access strobe (RAS) -> send half address
* column access strobe (CAS) -> other half
* 1 transistor for 1 bit
* reading destroys information -> restore it -> cycle time > access time
* bits refreshed periodically due to leaks, all bits in row refreshed simultaneously
* DIMMs (dual inline memory modules): 4-16 DRAMs, 8 bytes wide + ECC
* optimizations
    * timing signals
    * SDRAM (synchronous)
    * wider transfer: DDR3 16-bit busses
    * DDR: transfer data on risign and falling edge of DRAM clock signal
    * banks

Flash memory
* type of EEPROM, holds data without power
* must be erased before it is overwritten
* earsed in blocks
* limited number of cycles

## ARM cortex A8
* 2 instructions per clock, clock rate up to 1GHz
* two-level cache hierarchy
   * L1: pair of cache (I and D), 16KB or 32KB each, four-way set associative, way prediction and random replacement, virtually indexed and physically tagged
   * L2: optional, 8-way set associative, 128KB to 1MB, 1 to 4 banks (several transfers from memory concurrently), external bus 64 to 128 bits, physically indexed and tagged
* Memory management 
    * two TLBs (I and D), fully associated, 32 entries each with variable page size
    * replaced in TLB in round robin
    * TLB misses handled in hardware, which walks page table in memory
    * L2 miss penalty is 60 clock cycles
    * DDR SDRAM for main memory




# Appendix C: Pipelining


* Pipelining: multiple instructions are overlapped in execution
* Pipe stage: each step in pipeline completes a part of an instruction
* Processor cycle: time required to move instruction throughout pipe = slowest pipe stage

## RISC architecture:

* features
    * all operations on data apply to data in registers
    * only operations that affect memory are load and store
    * all instructions one size

* instruction classes
    * ALU instructions: add, substract, logical
    * load and store: base register and offset
    * branches and jumps: conditional transfer of control

### instruction without pipeline

clock cycles of RISC instruction
1. IF (instruction fetch cycle): fetch instruction from PC, add 4 to it
2. ID (instruction decode): decode instruction and read registers (in parallel)
3. EX (execution/effective address cycle): ALU operates on operands from previous cycle. Performs one of three:
    * Memory reference: base register + offset = effective addr
    * register-register instruction: performs operation from ALU opcode on values from register file
    * register-immediate instriction: performs operation from ALU opcode on first value read from register file
4. MEM (memory access): read if load, write effective addr if store
5. WB (write-back cycle): write result back to register file

Cycles take:
* branch instructions: 2 cycles
* store instructions: 4 cycles
* all others: 5 cycles

### Five-stage pipeline

* Each clock cycle = pipe stage
* Each instruction 5 cycles to complete, but each clock cycle 1 instruction completed
* Eliminating conflicts from overlapping instructions
    * separate instruction and data memories, with different caches
    * register file in two stages: reading ID (second half clock), writing WB (first half clock)
    * increase and store PC every clock -> conflict: branch doesnt change PC until ID phase
    * pipeline registers


* Pipeline hazards: prevent instruction from executing in its clock ccycle -> stalled pipeline
    * structural: resource conflicts -> use pipeline bubble
    * data: instruction depends on results of previous instruction -> use forwarding, pipeline interlock to preserve correct execution pattern
    * control: branches and instructions that change PC   

* Branch/control hazards:
    * branch changes PC -> branch taken
    * branch falls through -> branch not taken
    * dealing with branches: redo fetch of instruction following branch when branch detected -> stall IF cycle
    * methods:
        1. freeze or flush pipeline: hold or delete instructions after branch until branch destination is known
        2. treat every branch a not taken and continue. If taken, change fetched instruction for noop and restart fetch
        3. treat every branch as taken
        4. delayed branch

predicting branching
* static branch prediction: use profile information from earlier runs
* dynamic branch prediction
    * branch-prediction buffer or branch history table: 
        * memory address by branch instruction, bit says whether branch was release taken or not
        * 2-bit prediction to avoid predict incorrectly twice when unlikely happens: prediction must miss twice before it is changed

### Pipeline implementation

Unpipelined cycles:

1. Instruction fetch
    1.1 IR = mem[PC]: fetch memory at PC address into IR (instruction register)
    1.2 NPC = PC + 4: increment PC by 4 into NPC
2. Instruction decode
    2.1 A = reg[rs]: read GPRs into temporary register
    2.2 decoding in parallel
3. Execution: ALU operates on operands from previous cycle. One of four functions
    a. memory reference: adds operands to form effective addr
    b. register-register: ALUout = A func B
    c. register-immediate: ALUout = A op Imm
4. Memory access/branch completion: 
    a. Memory reference: Mem[ALUout] = B
    b. Branch: if (cond) PC = ALUout
5. Write-back cycle

Pipeline registers or pipeline latches: between stages, connect stages
* IF/ID
* ID/EX
* MEM/WB IR
* EX/MEM

Instruction issue: instruction moved (issued) from ID to EX
* Check hazards in ID phase -> if hazard exists, stall instruction
* Determine forwarding needed -> set controls
* Detect interlocks early

Branches
* make early decision -> both taken and untaken PCs calculated early -> requires additional adder


### Pipeline implementation issues

* Exceptions (interrupt, fault)
* restartable processor/pipeline: pipeline can save state, handle exception, and restart without affecting execution of program
* instruction guaranteed to complete -> commited -> at MEM stage for MIPS

* Floating point pipeline
    * allows longer latency for operations
    * changes compared to integer pipeline
        * EX cycle may be repeated multiple times
        * there may be multiple FP functional units: Eg
    * Example of functional units:
        * main integer unit for load-store, integer ALU operations, branches
        * FP and integer multiplier
        * FP adder for FP add, substract, conversion
        * FP and integer divider

 




# Chapter 3: Instruction level parallelism

ILP: overlap of instructions
Approaches: hardware and software statically at compilation time

