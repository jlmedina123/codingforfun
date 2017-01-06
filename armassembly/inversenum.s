/* $ arm-linux-gnueabi-gcc -static hellow.s -o hellow
 * $ qemu-arm hellow
 */
.syntax unified   // switch to a nicer syntax 
.global main      // declare main as exportable (public). link to stdlib 

# main function
main:
        push {ip, lr}      // push return address (lr) and ip on the stack
        ldr r3, #5 
        mvn r0, r3
        //str r3, result
        //ldr r0, result
        bl printf          // branch local - a function call, printf()
        mov r0, #0         // return 0
        pop {ip, pc}       // pop the values from stack into ip and pc registers

/* Loading immediate constants into register with GNU
mov r0, #74     // decimal
mov r0, #0x4A   // hex
mov r0, #0112   // octal (leading zero)
mov r0, #0b1001010 // binary
mov r0, #'J'    // ascii
.set constnum, 74  // with a symbol
mov r0, #constnum
*/
