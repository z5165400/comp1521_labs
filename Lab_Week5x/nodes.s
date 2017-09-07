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

append:
   # prologue
   add  $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   add  $sp, $sp, -4
   sw   $ra, ($sp)
   # ... add more if needed
   # function body
...
   # epilogue
   # ... add more if needed
   lw   $ra, ($sp)
   add  $sp, $sp, 4
   lw   $fp, ($sp)
   add  $sp, $sp, 4
   jr   $ra

showList:
   # prologue
   add  $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   add  $sp, $sp, -4
   sw   $ra, ($sp)
   # ... add more if needed
   # function body
...
   # epilogue
   # ... add more if needed
   lw   $ra, ($sp)
   add  $sp, $sp, 4
   lw   $fp, ($sp)
   add  $sp, $sp, 4
   jr   $ra

newNode:
   # prologue
   add  $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   add  $sp, $sp, -4
   sw   $ra, ($sp)
   # ... add more if needed
   # function body
...
   # epilogue
   # ... add more if needed
   lw   $ra, ($sp)
   add  $sp, $sp, 4
   lw   $fp, ($sp)
   add  $sp, $sp, 4
   jr   $ra

freeNode:
   # prologue
   add  $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   add  $sp, $sp, -4
   sw   $ra, ($sp)
   # ... add more if needed
   # function body
...
   # epilogue
   # ... add more if needed
   lw   $ra, ($sp)
   add  $sp, $sp, 4
   lw   $fp, ($sp)
   add  $sp, $sp, 4
   jr   $ra
