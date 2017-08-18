// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by Andrew Walls

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct _bit_fields {
    unsigned int a : 4,
                 b : 8,
                 c : 20;
};

int main(void)
{
    struct _bit_fields x;

    printf("%lu\n",sizeof(x));

    x.a = 0xF;
    x.b = 0x00;
    x.c = 0xAAAAA;

    printf("x.a = 0x%01x, x.b = 0x%02x, x.c = 0x%05x\n", x.a, x.b, x.c);
    printf("x as an unsigned int, represented as hex: %#010x\n", *((unsigned int *)(&x)));

    return 0;
}
