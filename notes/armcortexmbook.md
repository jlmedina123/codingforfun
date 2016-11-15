# Architecture

* access level: privileged, unprivileged (some memory regions are innaccesible, most NVIC registers innaccessibles)
* operation states: debug, thumb (running thumb instruction). no ARM state (doesnt support ARM instruction set)
* operation modes: handler (privileged access), thread (privileged or unprivileged, controlled by register CONTROL)

* Registers
	* Genral purpose registers: R0-R12
		* low registers R0-R7 only for 16-bit instructions
		* high registers R8-R12 for 32-bit instructions, and some 16-bit
	* SP R13: MSP, PSP (only thread mode). Last two bits zero => addr must be aligned
	* LR R14: holds return address. Program returns by loading LR into PC
	* PC R15: read returns addr plus 4, write causes branch operation 
	* Special registers: not memory mapped, access with instructions MRS and MSR
		* PSR: APSR, EPSR, IPSR
		* Exception interrupt masking: PRIMASK, FAULTMASK, BASEPRI
		* CONTROL: selects SP, and access level in thread mode 

* Memory regions
	* Program code
	* Data (SRAM)
	* Peripherals
	* Processor's internal control

* Exceptions
	* events that cause changes to program flow -> processor suspends task and executes handler
	* Each exception source has an exception number (240 interrupt inputs)
	* processed by NVIC: IRQs and NMI requests
		* programmable, regisers in SCS of memory map
		* nested: interrupts have priority
		* vector table: array of addresses with handlers. Location at VTOR register (0 after reset) 
	 	 

# Instruction Set

* based on Thumb-2: 16-bit and 32-bit instructions
* Addresing mode
    * immediate addressing: data in instruction. Eg MOV R2, #100
    * indexed addressing: uses register pointer. Eg LDR R1, =Count
    * PC-relative addressing: uses PC register as pointer
* Inserting data in instruction
    * DCB/W/D
    * .byte, .word, asciz
* Suffixes: S, Conditional execution (EQ, NE, LT, GT) if prev operation resulted in this

* Instructions
    * Moving data within processor: MOV, MRS, MSR
    * Memory access instructions: 
        * PC-related offset: LDR R0, =0x3   ; sets R0 to 0x3. Actually DCD 0x3, LDR R0, [PC, #offset] 
        * register offset: LDR R0, [R1, #0x3]  ; read at addr R1+0x3, store at R0 
        * write-back: LDR R0, [R1, #0x3]!   ; prev + update R1 to R1+0x3
        * post-index: LDR R0, [R1], #0x3    ; load mem addr R1 into  R0, then R1 to R1+0x3
    * Load/store multiple contiguous memory access
        * LDM/STM IA (increment addr after r/w) / DB (decrement addr before r/w)
             LDR   R4, =0x80    ; sets R4 to 0x80
             LDMIA R4, {R0-R3}  ; reads 4 words from mem addr 0x80 to 0x8F and store at R0-R3  

             LDR   R4, =0x8F    ; sets R4 to 0x8F
             LDMDB R4, {R0-R3}  ; reads 4 words from mem addr 0x8C, 0x88, 0x84, 0x80 and store at R0-R3

             LDR R8, =0x80
             STMIA R8!, {R0-R3} ; write 4 words from registers R0-R3 into memory 0x80-0x8F, 
                                  write back 0x90 to R8 after  

    * Stack: uses MSP or PS, depending on processor mode and CONTROL register
        * Eg: PUSH {R0, R2-44}
        * function return: 
            * PUSH {R4-R6, LR} ; save to beginning of subroutine
            * POP  {R4-R6, PC} ; pop from stack to R4-R6 and to PC directly (return addr). Addr
                                 in PC triggers a branch
	* Exclusive access: to build semaphores and mutexes
		* LDREXB
		* STREXB
	* Arithmetic operations
	* Logic ans shift
	* typecasting
	* bit-field instructions
	* program flow control
		* branch: instructions that cause branching
			* B
			* updating PC
	 * memory barriers: use when changing configuration
	 	* DMB
	 	* DSB         
                                                  
            

## ARM Cortex processor

* Registers
    * 




ARM Cortex-M Assembly
4 fields:
label: to identify position in memory of current instruction. unique name
opcode: instruction
operand: data
comment
Addressing mode:
immediate addressing: data in instruction. Eg MOV R2, #100
indexed addressing: uses register pointer. Eg LDR R1, =Count
PC-relative addressing: uses PC register as pointer
…
I/O ports
Initialize I/O port
1) Activate clock for port
2) set direction register
3) clear bits in alternate function register
4) enable port
Phase Lock Loop
allows software to adjust execution of speed: tradeoff between throughput and power
Initialization
1) set bypass PLL and clock divider
2) set crystal freq
3) activate PLL
4) configure and enable clock divide
5) wait for PLL to stabilize and bit to become high
6) connect PLL by clearing bypass PLL
SysTick Timer
to create time delays and periodic interrupts
Initialization
1) clear enable to turn it off
2) set reload
3) clear counter
4) write desired mode to control register
Interrupt synchronization
Interrupt: automatic transfer of software execution in response to async hardware event
Context switch as result of interrupt:
1) current instruction is finished
2) execution of current task is suspended
3) push 8 registers (R0-3, R12, LR PC, PSR) to stack
4) LR set to value indicating ISR is run (0xFFFFFF9), and how to return from interrupt
5) IPSR is set to interrupt number
6) PC is loaded with address of ISR
ISR: code executed for interrupts
polled interrupts: one ISR for all interrups
vectored interrupts: each source of interrupt its own ISR
Atomic operation: runs always until completion (disable interrupts)
Interthread communication
Binary semaphore: shared flag
mailbox: binary semaphore with data variable
FIFO:
NVIC (Nested Vector Interrupt Controller)
each exception has a 32-bit vector that points to memory location for ISR.
Vectors (addresses of ISRs) are stored in ROM from 0x0
Startup.s file defines names for interrupt vectors (ISR names)
Write ISR functions with ISR names from Startup.s
Active interrupt source: set priority and enable the interrupt
