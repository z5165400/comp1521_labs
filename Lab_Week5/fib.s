# MIPS assembler to compute Fibonacci numbers

    .data
msg1:
    .asciiz "n = "
msg2:
    .asciiz "fib(n) = "
msg3:
    .asciiz "n must be > 0\n"

    .text

# int main(void)
# {
#     int n;
#     printf("n = ");
#     scanf("%d", &n);
#     if (n >= 1)
#        printf("fib(n) = %d\n", fib(n));
#     else {
#        printf("n must be > 0\n");
#        exit(1);
#     }
#     return 0;
# }

    .globl main
main:
    # prologue
    addi $sp, $sp, -4
    sw   $fp, ($sp)
    move $fp, $sp
    addi $sp, $sp, -4
    sw   $ra, ($sp)

    # function body
    la   $a0, msg1       # printf("n = ");
    li   $v0, 4
    syscall

    li   $v0, 5          # scanf("%d", &n);
    syscall
    move $a0, $v0

    li $t0, 1
    # if n >= 1, continue at main_continue
    bge $a0, $t0, main_continue
    # or print an error message and exit
    la $a0, msg3
    li $v0, 4
    syscall

    # exit with error code 1
    li $v0, 1
    j main_end

main_continue:
    jal  fib             # $s0 = fib(n);
    nop
    move $s0, $v0

    la   $a0, msg2       # printf((fib(n) = ");
    li   $v0, 4
    syscall

    move $a0, $s0        # printf("%d", $s0);
    li   $v0, 1
    syscall

    li   $a0, '\n'       # printf("\n");
    li   $v0, 11
    syscall

    # epilogue
main_end:
    lw   $ra, ($sp)
    addi $sp, $sp, 4
    lw   $fp, ($sp)
    addi $sp, $sp, 4
    jr   $ra


# int fib(int n)
# {
#     if (n < 1)
#        return 0;
#     else if (n == 1)
#        return 1;
#     else
#        return fib(n-1) + fib(n-2);
# }

fib:
    # prologue
    addi $sp, $sp, -4
    sw $fp, ($sp)
    move $fp, $sp

    addi $sp, $sp, -12
    sw $ra, 8($sp)
    sw $s0, 4($sp)
    sw $s1, 0($sp)

    li $t0, 1

    # n > 1
    bgt $a0, $t0, fib_gt1
    # n == 1
    beq $a0, $t0, fib_eq1
    # n < 1
    blt $a0, $t0, fib_lt1


fib_lt1:
    move $v0, $0
    j fib_end

fib_eq1:
    li $v0, 1
    j fib_end

fib_gt1:
    # store a0
    move $s0, $a0
    # a0 = n - 1
    addi $a0, $a0, -1
    # v0 = fib(n - 1)
    jal fib
    # store fib(n - 1)
    move $s1, $v0

    # a0 = n - 2
    addi $a0, $s0, -2
    # v0 = fib(n - 2)
    jal fib

    # v0 = fib(n - 2) + fib(n - 1)
    add $v0, $v0, $s1

    # epilogue
fib_end:
    lw $s1, 0($sp)
    lw $s0, 4($sp)
    lw $ra, 8($sp)
    lw $fp, 12($sp)
    addi $sp, $sp, 16
    jr   $ra
