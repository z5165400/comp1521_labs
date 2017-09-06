# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by Andrew Walls, August 2017

    .data
main_ret_save: .space 4
ascii_dot: .asciiz "."
ascii_hash: .asciiz "#"
ascii_nl: .asciiz "\n"


    .text
    .globl main
main:
    sw   $ra, main_ret_save

# Your main program code goes here

end_main:
    lw   $ra, main_ret_save
    jr   $ra


# The other functions go here

# {{{ neighbours
neighbours:
    addi $sp, -4
    sw $fp, ($sp)
    move $fp, $sp

    # i: $a0, j: $a1
    li $t0, 0   # nn
    li $t1, -1  # x
    li $t2, -1  # y
    lw $t3, N   # N
    li $t4, 1   # 1 (terminator)

    addi $t3, -1

nb_loop:
    move $t5, $a0                       # t5 = i
    move $t6, $a1                       # t6 = j
    add $t5, $t1                        # t5 = i + x
    add $t6, $t2                        # t6 = j + y
    blt $t5, $0, nb_inner_loop_end      # if i+x < 0 continue
    bgt $t5, $t3, nb_inner_loop_end     # if i+x > N-1 continue
    blt $t6, $0, nb_inner_loop_end      # if j+y < 0 continue
    bgt $t6, $t3, nb_inner_loop_end     # if j+y > N-1 continue

    beqz $t1, nb_inner_loop_end     # if x == 0 continue
    beqz $t2, nb_inner_loop_end     # if y == 0 continue

    mul $t5, $t3        # i+x * N to get row
    add $t6, $t5, $t6   # (i+x * N) + i+j to get address

    lw $t6, N($t5)      # get value
    bne $t6, $t4, nb_inner_loop_end

    addi $t0, 1

nb_inner_loop_end:
    addi $t2, 1
    ble $t2, $t4, nb_loop

nb_loop_end:
    addi $t1, 1
    li $t2, -1
    ble $t1, $t4, nb_loop

    move $v0, $t0

    lw $fp, ($sp)
    addi $sp, 4
    jr $ra
# }}}

# {{{ copyBackAndShow
copyBackAndShow:
    addi $sp, -4
    sw $fp, ($sp)
    move $fp, $sp
    addi $sp, -4
    sw $ra, ($sp)

    li $t0, 0   # i
    li $t1, 0   # j
    lw $t2, N   # N

cBAS_loop:
    move $t3, $t0           # i
    mul $t3, $t3, $t2       # [i]
    add $t3, $t3, $t1       # [i][j]
    lw $t4, newBoard($t3)
    sw $t4, board($t3)

    li $v0, 4   # print string
    bnez $t4, cBAS_put_char

    la $a0, ascii_dot
    syscall
    j ascii_inner_loop_end

cBAS_put_hash:
    la $a0, ascii_hash
    syscall

cBAS_inner_loop_end:
    addi $t1, 1
    blt $t1, $t2, cBAS_loop

cBAS_loop_end:
    li $v0, 4           # print string
    la $a0, ascii_nl    # newline
    syscall

    addi $t0, 1
    li $t1, 0
    blt $t0, $t2, cBAS_loop

cBAS_end:
    lw $ra, ($sp)
    addi $sp, 4
    lw $fp, ($sp)
    addi $sp, 4
    jr $ra
# }}}
