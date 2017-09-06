# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by Andrew Walls, August 2017

    .data
    .align 2
main_ret_save: .space 4
ascii_dot: .asciiz "."
ascii_hash: .asciiz "#"
ascii_nl: .asciiz "\n"
msg_iters:
    .asciiz "# Iterations: "
msg_status_before:
    .asciiz "=== After iteration "
msg_status_after:
    .asciiz " ===\n"

    .text
    .globl main
main:
    sw   $ra, main_ret_save

    li $s0, 0       # maxiters
    li $s1, 1       # n
    li $s2, 0       # i
    li $s3, 0       # j
    lw $s4, N       # N

    li $v0, 4       # print "# Iterations:"
    la $a0, msg_iters
    syscall

    li $v0, 5       # read int
    syscall
    move $s0, $v0

main_nloop_start:
    li $s2, 0       # i = 0

main_iloop_start:
    li $s3, 0       # j = 0

main_jloop_start:
    # get number of neighbours
    move $a0, $s2
    move $a1, $s3
    jal neighbours

    li $t1, 1       # 1 (for comparisons)
    li $t2, 2       # 2 (again, comparisons)
    li $t3, 3       # 3 (see above)

    move $t0, $s3       # j
    mul $t0, $t0, $s4   # [j]
    add $t0, $t0, $s2   # [i][j]
    lb $t4, board($t0)  # board[i][j]; get value of current cell

    # The rather complicated if statement in the body of the loop
    # in the C code can be condensed to this surprisingly simple block:
    beq $v0, $t3, main_loop_set1    # If the current cell has 3 neighbours, it will always be alive
    bne $t4, $t1, main_loop_set0    # If not and it's dead, it'll stay dead
    beq $v0, $t2, main_loop_set1    # But if it's alive and has 2 neighbours, it'll stay alive
    # and for anything else it'll be dead, so fall through

main_loop_set0:
    # set the current cell to dead
    sb $0, newBoard($t0)
    j main_jloop_end

main_loop_set1:
    # set the current cell to alive
    sb $t1, newBoard($t0)
    j main_jloop_end

main_jloop_end:
    # increment j, repeat or fall through to end of the i loop
    addi $s3, 1
    blt $s3, $s4, main_jloop_start

main_iloop_end:
    # increment i, repeat or fall through to end of the n loop
    addi $s2, 1
    blt $s2, $s4, main_iloop_start

main_nloop_end:

    li $v0, 4                   # print "=== After iteration "
    la $a0, msg_status_before
    syscall
    li $v0, 1                   # print iteration
    move $a0, $s1
    syscall
    li $v0, 4
    la $a0, msg_status_after    # print " ==="
    syscall

    jal copyBackAndShow

    # increment n, repeat or fall through to end of main
    addi $s1, 1
    ble $s1, $s0, main_nloop_start

end_main:
    # return 0
    li   $v0, 0
    lw   $ra, main_ret_save
    jr   $ra

neighbours:
    # save and set the frame pointer
    # not that we actually need to do this, since we only use t# registers
    addi $sp, -4
    sw $fp, ($sp)
    move $fp, $sp

    # i: $a0, j: $a1
    li $t0, 0   # nn
    li $t1, -1  # x
    li $t2, -1  # y
    lw $t3, N   # N
    li $t4, 1   # 1 (terminator)

    addi $t7, $t3, -1 # N - 1

nb_loop:
    move $t5, $a0                       # t5 = i
    move $t6, $a1                       # t6 = j
    add $t5, $t5, $t1                   # t5 = i + x
    add $t6, $t6, $t2                   # t6 = j + y
    blt $t5, $0, nb_inner_loop_end      # if i+x < 0 continue
    bgt $t5, $t7, nb_inner_loop_end     # if i+x > N-1 continue
    blt $t6, $0, nb_inner_loop_end      # if j+y < 0 continue
    bgt $t6, $t7, nb_inner_loop_end     # if j+y > N-1 continue

    bnez $t1, nb_loop_xy_nz     # if x != 0, skip to board check
    bnez $t2, nb_loop_xy_nz     # if y != 0, skip to board check
    j nb_inner_loop_end         # if x == 0 && y == 0 continue

nb_loop_xy_nz:

    mul $t6, $t6, $t3       # [j+y]
    add $t5, $t5, $t6       # [i+x][j+y]

    lb $t6, board($t5)      # board[i+x][j+y]
    bne $t6, $t4, nb_inner_loop_end # skip to end of loop if cell is dead

    # otherwise increment the neighbour count
    addi $t0, 1

nb_inner_loop_end:
    # increment y, repeat or fall through to end of x loop
    addi $t2, 1
    ble $t2, $t4, nb_loop

nb_loop_end:
    # increment x, repeat or fall through to end of loop
    addi $t1, 1
    li $t2, -1
    ble $t1, $t4, nb_loop

    # store neighbour count in return register
    move $v0, $t0

    lw $fp, ($sp)
    addi $sp, 4
    jr $ra

# {{{ copyBackAndShow
copyBackAndShow:
    # store the frame pointer and return address, set frame pointer
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
    lb $t4, newBoard($t3)   # get newBoard[i][j]
    sb $t4, board($t3)      # set board[i][j]

    bnez $t4, cBAS_put_hash # if 1 (not 0) print a #

    li $v0, 4               # otherwise, fall through and print a .
    la $a0, ascii_dot
    syscall
    j cBAS_inner_loop_end

cBAS_put_hash:
    li $v0, 4               # print "."
    la $a0, ascii_hash
    syscall

cBAS_inner_loop_end:
    # increment j, repeat or fall through to end of i loop
    addi $t1, 1
    blt $t1, $t2, cBAS_loop

cBAS_loop_end:
    li $v0, 4           # print newline
    la $a0, ascii_nl
    syscall

    # increment i, repeat or fall through to cBAS_end
    addi $t0, 1
    li $t1, 0           # j = 0
    blt $t0, $t2, cBAS_loop

cBAS_end:
    # restore frame pointer and return address
    lw $ra, ($sp)
    addi $sp, 4
    lw $fp, ($sp)
    addi $sp, 4
    jr $ra
# }}}
