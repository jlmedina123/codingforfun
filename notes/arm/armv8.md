
A1

ARM is RISC
- large register file
- load/store
- simple addressing modes

armv8 supports:
- aarch64 execution, uses 64-bit registers
- aarch32 exection compatible with previous arm, uses 32-bit registers

Architecture profiles
- A: applicaiton profile, with VMSA based on MMU, a64
- R: real time with MPU, a32, t32
- M: microcontroller, t32, 

Execution states:
- aarch64
	- X0-30 64bit GP registers, X30=link register
	- 64bit PC, SP, ELRs
	- Exception levels EL0-3
	- 64-bit virtual addressing
	- Processor state PSTATE
- aarch32
	- 32bit GP registers

Register files
	- GP register file: 64-bit 32 registers
	- SIMD: 128-bit


