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
