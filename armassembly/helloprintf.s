/* $ arm-linux-gnueabi-gcc -static hellow.s -o hellow
 * $ qemu-arm hellow
 */
.syntax unified   // switch to a nicer syntax 
.global main      // declare main as exportable (public). link to stdlib 

# main function
main:
        push {ip, lr}      // push return address (lr) and ip on the stack
        ldr r0, =message   // put the address of the string into the r0 register  
        bl printf          // branch local - a function call, printf()
        mov r0, #0         // return 0
        pop {ip, pc}       // pop the values from stack into ip and pc registers

# our string to print, ASCII zero terminated
message:
        .asciz "Hello from printf\n"
