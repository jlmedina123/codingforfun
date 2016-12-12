https://0xax.gitbooks.io/linux-insides/content/Booting/

#x86 architecture

## Registers

* GPRs (16-bit mode)
	* AX: Accumulator register. Used in arithmetic operations.
	* CX: Counter register. Used in shift/rotate instructions and loops.
	* DX: Data register. Used in arithmetic operations and I/O operations.
	* BX: Base register. Used as a pointer to data (located in segment register DS, when in segmented mode).
	* Stack Pointer register (SP). Pointer to the top of the stack.
	* Stack Base Pointer register (BP). Used to point to the base of the stack.
	* Source Index register (SI). Used as a pointer to a source in stream operations.
	* Destination Index register (DI). Used as a pointer to a destination in stream operations.
* GPRs addressing:
	* 16-bit mode: as written above
	* 32-bit mode: E+above
	* 64-bit mode: R+above
	* 8-bit LSB: AL, CL, DL, BL
	* 8-bit second half: AH, CH, DH, BH  
	
* 6 Segment Registers are:
	* Stack Segment (SS). Pointer to the stack.
	* Code Segment (CS). Pointer to the code.
	* Data Segment (DS). Pointer to the data.
	* Extra Segment (ES). Pointer to extra data ('E' stands for 'Extra').
	* F Segment (FS). Pointer to more extra data ('F' comes after 'E').
	* G Segment (GS). Pointer to still more extra data ('G' comes after 'F'). 

* Indexes and pointers
	* ES:EDI EDI DI Destination index register
	* DS:ESI EDI SI Source index register
	* SS:EBP EBP BP Stack Base pointer register
	* SS:ESP ESP SP Stack pointer register
	* CS:EIP EIP IP Index Pointer
	
Linux doesnt use segments (but paging), so all Segment registers point to same place
	
* EFLAGS Register (32 bits)

## Operation modes

* Real-time
	* mimics behavior of 8086
	* 8086 CPU 16-bit (64KB), but memory 20-bit addresses (1MB) -> use memory segmentation
	* memory segmentation: memory divided in 64KB segments. 
	* physical address (CS:IP) = segment selector (base addr) * 16 (or <<4)+ offset
	
#Kernel Boot Process

## Bootloader to kernel

* power, pgood, reset deasserted. 
* reset vector:
	* CPU (x86) registers get default values loaded: IP 0xfff0, CS selector 0xf000, CS base 0xffff0000 -> starting address = 4GB
	* memory location that CPU uses to fetch first instruction after reset -> jmp that points to BIOS entry point
* BIOS: 
	* in ROM, hardwire to entry point
	* initializes and checks hardware, and finds bootable device
* Bootloader
	* boot sector code 
