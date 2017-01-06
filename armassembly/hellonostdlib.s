/* 
 * compile with gnu gcc arm cross compiler
 * $ arm-linux-gnueabi-gcc hw.S -o hw -nostdlib
 * $ qemu-arm hw
 * from https://www.ibm.com/developerworks/library/l-arm-toolchain/
 */
.text
.align 2
.global _start

/* r0 to r6 for input arguments, r7 for system call number
 *  ssize_t sys_write(unsigned int fd, const char * buf, size_t count)
 *         r7                 r0      r1                r2
 */

_start:
    adr     r1, msg         @ Address
    mov     r0, #1          @ STDOUT
    mov     r2, #18         @ Length
    mov     r7, #4          @ sys_write sys call
    svc     0x00000000      @ call supervisor (linux kernel)
                            @ same as 'swi #0'
         
/* int sys_exit(int status)
 *     r7       r0
 */     
    mov     r0, #0          @ Return code
    mov     r7, #1          @ sys_exit sys call
    svc     0x00000000

.align 2
    
msg:
    .asciz "Hello GNU GCC ARM\n"
