# nodes.s ... linked list testing

    .data
N:
    .word 100
heap:
    .space 800    # 100 Node slots
free:
    .space 100    # assume all set to zero

    .text
    .globl main
# {{{ main
main:
    # prologue
    add  $sp, $sp, -4
    sw   $fp, ($sp)
    la   $fp, ($sp)
    add  $sp, $sp, -4
    sw   $ra, ($sp)
    add  $sp, $sp, -4
    sw   $s0, ($sp)
    # function body
    move $s0, $0        # Node *L = NULL
    li   $a1, 1
    jal  append
    move $s0, $v0      # L = append(L, 1)
    move $a0, $s0
    li   $a1, 3
    jal  showList
    jal  append
    move $s0, $v0      # L = append(L, 3)
    move $a0, $s0
    li   $a1, 5
    jal  showList
    jal  append
    move $s0, $v0      # L = append(L, 5)
    move $a0, $s0
    li   $a1, 7
    jal  showList
    jal  append
    move $s0, $v0      # L = append(L, 7)
    move $a0, $s0
    jal  showList
    # epilogue
    lw   $s0, ($sp)
    add  $sp, $sp, 4
    lw   $ra, ($sp)
    add  $sp, $sp, 4
    lw   $fp, ($sp)
    add  $sp, $sp, 4
    jr   $ra
# }}}

# {{{ append
append:
    # prologue
    add  $sp, $sp, -4
    sw   $fp, ($sp)
    la   $fp, ($sp)
    add  $sp, $sp, -4
    sw   $ra, ($sp)
    # ... add more if needed
    # function body
#...
    # epilogue
    # ... add more if needed
    lw   $ra, ($sp)
    add  $sp, $sp, 4
    lw   $fp, ($sp)
    add  $sp, $sp, 4
    jr   $ra
# }}}

# {{{ showList
showList:
    # prologue
    add  $sp, $sp, -4
    sw   $fp, ($sp)
    la   $fp, ($sp)
    add  $sp, $sp, -4
    sw   $ra, ($sp)
    # ... add more if needed
    # function body
#...
    # epilogue
    # ... add more if needed
    lw   $ra, ($sp)
    add  $sp, $sp, 4
    lw   $fp, ($sp)
    add  $sp, $sp, 4
    jr   $ra
# }}}

# {{{ newNode
newNode:
    # prologue
    add  $sp, $sp, -4
    sw   $fp, ($sp)
    la   $fp, ($sp)
    add  $sp, $sp, -4
    sw   $ra, ($sp)
    # function body

    li $t0, 0   # i
    li $t1, 1   # 1
    lw $t2, N   # N
nN_loop:
    beq $t0, $t2, nN_no_nodes

    lw $t3, free($t0)
    beq $t3, $t1, nN_return_node

    addi $t0, 1
    j nN_loop

nN_no_nodes:
    move $v0, $0
    j nN_end

nN_return_node:
    sw $0, free($t0)
    la $v0, heap($t0)
    j nN_end

    # epilogue
nN_end:
    lw   $ra, ($sp)
    add  $sp, $sp, 4
    lw   $fp, ($sp)
    add  $sp, $sp, 4
    jr   $ra
# }}}

# {{{ freeNode
freeNode:
    # prologue
    add  $sp, $sp, -4
    sw   $fp, ($sp)
    la   $fp, ($sp)
    add  $sp, $sp, -4
    sw   $ra, ($sp)
    # ... add more if needed
    # function body
#...
    # epilogue
    # ... add more if needed
    lw   $ra, ($sp)
    add  $sp, $sp, 4
    lw   $fp, ($sp)
    add  $sp, $sp, 4
    jr   $ra
# }}}
