
# Introduction to 64 Bit intel assembly language programming for linux - Ray Seyfarth


## Chapter 3

Process regions:
* Stack: highest address allowed in logical address (48-1 bits set to 1, 131 TB)
         restricted to 16 MB. Grows with page faults, which adds page
* heap:
* data: initialized data. just above text
* bss: data staticall allocated, not stored in file, initilized to 0
* text: from lowest address (0x400000)


## Chapter 4

CR3 points to PML4 (Page Map Level 4)
Virtual address:
* 16 bits unused
* 9 bits PML index (address = 8 bytes => 512 address in 4KiB page, 512 = 2^9 => 9 bits)
* 9 bits P
* 9 bits
* 12 page offset (2^12 = 4096 B = 4KiB = page size)

PML: array of 512 addresses pointers

## Chapter 5

16 64-bit GPRs, 16 128/256-bit FPRs
RIP: 64-bit instruction pointer
Rflags: flags register

8088 registers:
* ax: accumulator
* bx: base register (for array access)
* cx: count register (for string operations)
* dx: data register
* si: source index
* di: destination index
* bp: base pointer (for function frames)
* sp: stack pointer

x86-64:
* GPRs: rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8-r15
* access: rax for 64 bits, eax for 32 bits, ax for 16 bits; b/w/dr8

mov     rax, 100    ; move immediate value (constant) to rax

a   dq  44
mov rax, [a]    ; load constant from memory
mov [a], rax    ; store from register to memory


## Chapter 6

neg     rax         ; 2's complement
add     [a], rax    ; [a] = [a] + rax
sub     rax, [a]    ; rax = rax - [a] 
imul    r8,  [x]    ; rax = rax * [x]
imul    rbx, [x], 100 ; rxb = 100 * x

conditional move: cmovz (zero), cmovnz (not zero), cmovl (<0), cmovle (<= 0), cmovg (>0), cmovge (>=0)

Move value to register if used more than once, otherwise operate with memory values

# Chapter 7

C not:
* ! : logical not, results in true/false
* ~ : bitwise not, negates each bit

bitwise operations: not, and, or, xor, shr/shl

Extract bits 23-51 from quad-word
```
mov     rax, [sample]
shr     rax, 23 
and     rax, 0x1fffffff
mov     [field], rax
```

Fill bits 32-51 in quad-word with bits from 'field'
```
mov     rax, [sample]
ror     rax, 23
shr     rax, 29
shl     rax, 29
or      rax, [field]
rol     rax, 23
mov     [sample], rax
```

## Chapter 8

conditional jump: jz (=0), jnz (!=0), jg (>0), jge, jle, jc, jnc

equivalent of `if(a<b) {temp = a; a = b; b = temp;}`
```
mov     rax, [a]
mov     rbx, [b]
cmp     rax, rxb
jge     inorder
mov     [temp], rax
mov     [a], rbx
mov     [b], rax
inorder:
```

Other branching instructions:

loop    label   ; decrements rcx and branches until rcx=0
rep             ; repeats insturction number defined in rcx

String instructions
* rax: hold value
* rsi: source index
* rdi: destination index

Move bytes from location to another
```
lea     rsi, [source]   ; rsi: source index
lea     rdi, [destination]  ; rdi: destination index
mov     rcx, 10000      ; rcx: number of times to repeat
rep     movsb
```

Store value in location
```
mov     eax, 1      ; eax: value to store
mov     ecx, 1000   ; rcx: number of times to repeat
rep     stosd       ; repeate stosb instruction ecx times
```

scan, cmp, ...


## Chapter 9

Stack: 
* supposed to start at highest possible value: 0x7ffffffff
* stack randomization: starts below highest value to avoid attacks
* `push`: rsp=rsp-8, place value at rsp

Call a function: call, ret

Function call protocol for Linux: System V Application Binary Interface
* 6 integer parameters passed in rdi, rsi, rdx, rex, r8 and r9
* 8 floating-point parameters passed in xmm0-xmm7
* rax for return integer, xmm0 for return floating
* stack pointer on 16 byte boundary -> rsp in hex should finish in 0

Stack frame with 32 bytes (aligned to 16) for local variable:
```
push rbp
mov rbp, rsp
sub rsp, 32
```

Return and undo stack frame (no need for leaf function):
```
leave
ret
```

Local stack variables: use equ instead of just rsp
```
x   equ     0
y   equ     8
    mov     [rsp+x], r8
    mov     [rsp+y], r9
```



