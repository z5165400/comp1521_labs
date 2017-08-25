# COMP1521 Lab 04 ... Simple MIPS assembler
# Modified by Andrew Walls

### Global data

    .data
msg1:
    .asciiz "n: "
msg2:
    .asciiz "n! = "
eol:
    .asciiz "\n"

### main() function

    .data
    .align 2
main_ret_save:
    .word 4
main_fac_save:
    .word 4

    .text
    .globl main

main:
    sw  $ra, main_ret_save

    li  $v0, 4      # print_string
    la  $a0, msg1
    syscall

    li  $v0, 5      # read_int
    syscall
    # $v0 contains int

    move $a0, $v0
    jal fac
    move $s0, $v0

    li  $v0, 4      # print_string
    la  $a0, msg2
    syscall

    li  $v0, 1       # print_int
    move $a0, $s0
    syscall

    li  $v0, 4      # print_string
    la  $a0, eol
    syscall

    lw  $ra, main_ret_save
    jr  $ra           # return

### fac() function

    .data
    .align 2
fac_ret_save:
    .space 4

    .text
    .globl fac

fac:
    sw  $ra, fac_ret_save

    # i
    li  $t0, 1
    # f
    li  $t1, 1

loop:
    bge $t0, $a0, endwhile
    addi $t0, 1
    mul $t1, $t1, $t0
    j loop

endwhile:
    move $v0, $t1

    lw  $ra, fac_ret_save
    jr  $ra            # return ($v0)
