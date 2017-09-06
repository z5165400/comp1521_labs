# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by Andrew Walls, August 2017

    .data
main_ret_save: .space 4


    .text
    .globl main
main:
    sw   $ra, main_ret_save

# Your main program code goes here

end_main:
    lw   $ra, main_ret_save
    jr   $ra


# The other functions go here

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






