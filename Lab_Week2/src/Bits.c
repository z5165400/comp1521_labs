// ADT for Bit-strings
// COMP1521 17s2 Week02 Lab Exercise
// Written by John Shepherd, July 2017
// Modified by Andrew Walls, July 2017

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "Bits.h"

// assumes that an uint32_t is 32 bits
#define BITS_PER_WORD 32

// A bit-string is an array of uint32_ts (each a 32-bit Word)
// The number of bits (hence Words) is determined at creation time
// Words are indexed from right-to-left
// words[0] contains the most significant bits
// words[nwords-1] contains the least significant bits
// Within each Word, bits are indexed right-to-left
// Bit position 0 in the Word is the least significant bit
// Bit position 31 in the Word is the most significant bit

typedef uint32_t Word;

struct BitsRep {
   int nwords;   // # of Words
   Word *words;  // array of Words
};

// make a new empty Bits with space for at least nbits
// rounds up to nearest multiple of BITS_PER_WORD
Bits makeBits(int nbits)
{
   Bits new;
   new = malloc(sizeof(struct BitsRep));
   assert(new != NULL);
   int  nwords;
   if (nbits%BITS_PER_WORD == 0)
      nwords = nbits/BITS_PER_WORD;
   else
      nwords = 1+nbits/BITS_PER_WORD;
   new->nwords = nwords;
   // calloc sets to all 0's
   new->words = calloc((uint32_t)nwords, sizeof(Word));
   assert(new->words != NULL);
   return new;
}

// release space used by Bits
void  freeBits(Bits b)
{
   assert(b != NULL && b->words != NULL);
   free(b->words);
   free(b);
}

// form bit-wise AND of two Bits a,b
// store result in res Bits
void andBits(Bits a, Bits b, Bits res)
{
    // Ensure the two AND Bits are of equal length
    assert(a->nwords == b->nwords);
    // Ensure the result Bits is equal length or larger, to contain result
    assert(res->nwords >= a->nwords);

    int i = 0;

    // i is already initialised
    // Zero out any overhanging bits of the result
    for(i = 0; i < (res->nwords - a->nwords); i++) {
        res->words[i] = 0;
    }

    // i is, again, already initialised, and contains the index of the
    // first word which a, b and res have in common
    // Which we'll store for later
    int offset = i;
    for(; i < res->nwords; i++) {
        // Set each word of res to the AND result of the a and b word at the same position
        res->words[i] = a->words[i - offset] & b->words[i - offset];
    }
}

// form bit-wise OR of two Bits a,b
// store result in res Bits
void orBits(Bits a, Bits b, Bits res)
{
    // Ensure the two OR Bits are of equal length
    assert(a->nwords == b->nwords);
    // Ensure the result Bits is equal length or larger, to contain result
    assert(res->nwords >= a->nwords);

    int i = 0;

    // i is already initialised
    // Zero out any overhanging bits of the result
    for(i = 0; i < (res->nwords - a->nwords); i++) {
        res->words[i] = 0;
    }

    // i is, again, already initialised, and contains the index of the
    // first word which a, b and res have in common
    // Which we'll store for later
    int offset = i;
    for(; i < res->nwords; i++) {
        // Set each word of res to the OR result of the a and b word at the same position
        res->words[i] = a->words[i - offset] | b->words[i - offset];
    }
}

// form bit-wise negation of Bits a,b
// store result in res Bits
void invertBits(Bits a, Bits res)
{
    // Ensure the result Bits is equal length or larger, to contain result
    assert(res->nwords >= a->nwords);

    int i = 0;

    // i is already initialised
    // Zero out any overhanging bits of the result
    for(i = 0; i < (res->nwords - a->nwords); i++) {
        res->words[i] = 0;
    }

    // i is, again, already initialised, and contains the index of the
    // first word which a, b and res have in common
    // Which we'll store for later
    int offset = i;
    for(; i < res->nwords; i++) {
        // Set each word of res to the NOT result of the a and b word at the same position
        res->words[i] = ~a->words[i - offset];
    }
}

// left shift Bits
void leftShiftBits(Bits b, int shift, Bits res)
{
    assert(shift < BITS_PER_WORD); // We can't handle double shifts yet; TODO

    // 64-bit shifting variable, in which we will perform the actual bit shifting
    uint64_t shifter = 0;
    // Carry variable
    uint32_t carry = 0;

    // Iterate from the LSB to the MSB, shifting each word
    for(int i = b->nwords - 1; i >= 0; i--) {
        // Get the current word
        shifter = b->words[i];
        // Shift it back by the required number of bits
        shifter <<= shift;
        // Store least significant 32 bits of the shifted variable, combined with the carried
        // values from the last shift
        res->words[i] = (uint32_t)shifter | carry;
        // Take the most significant 32 bits of the shift, and store them as the carry
        carry = (shifter & (0xFFFFFFFF00000000)) >> BITS_PER_WORD;
    }
}

// right shift Bits
void rightShiftBits(Bits b, int shift, Bits res)
{
     assert(shift < BITS_PER_WORD); // We can't handle double shifts yet; TODO

     // 64-bit shifting variable, in which we will perform the actual bit shifting
     uint64_t shifter = 0;
     // Carry variable
     uint32_t carry = 0;

     // Iterate from the MSB to the LSB, shifting each word
     for(int i = 0; i < b->nwords; i++) {
         // Get the current word
         shifter = (uint64_t)b->words[i] << (BITS_PER_WORD - 1);
         // Shift it back by the required number of bits
         shifter >>= shift;
         // Store most significant 32 bits of the shifted variable, combined with the carried
         // values from the last shift
         res->words[i] = (uint32_t)(shifter >> (BITS_PER_WORD - 1)) | carry;
         // Take the least significant 32 bits of the shift, and store them as the carry
         carry = (uint32_t)shifter;
     }
}

// copy value from one Bits object to another
void setBitsFromBits(Bits from, Bits to)
{
    // TODO: Don't make this assumption
    assert(from->nwords == to->nwords);

    // Counting down, because why not
    // This could also be done with a memcpy I guess?
    // I wonder if it would make a difference
    // TODO: Benchmarks
    for(int i = from->nwords - 1; i >= 0; i--) {
        to->words[i] = from->words[i];
    }
}

// assign a bit-string (sequence of 0's and 1's) to Bits
// if the bit-string is longer than the size of Bits, truncate higher-order bits
void setBitsFromString(Bits b, char *bitseq)
{
    // Set b to 0 before we start to use it, so any old values don't interfere with the new ones
    for(int i = 0; i < b->nwords; i++) {
        b->words[i] = 0;
    }

    // Iterate over bitseq, setting bits in b
    // We use j to keep track of the current bit in b, and i to keep track of the index of bitseq
    // i and j both start at the end, and work towards the start
    int j = b->nwords * BITS_PER_WORD - 1;
    for(int i = (int)strlen(bitseq) - 1; i>= 0 && j >= 0; i--, j--) {


        /*
            Explanation of below:

            If we're 37 bits to the left of the end of bitseq, which is 64 bits long:
            0000000001000001000101100010000000010000100000000011001000001001
            ^63                  ^42       ^32                            ^0
            then i will be (63 - 42) = 21, since i is from the left of bitseq
            And if b is also 64 bits long:
            00000000000000000000001000100000,  00010000100000000011001000001001
            ^63   b->words[0]    ^42       ^32 ^31        b->words[1]         ^0
            then j will be (63 - 42) = 21, since j is from the left of b

            If bitseq[i] == '1', which it does, then
            newBit = 1

            We then need to set bit 42 of b to 1. To do this, we need to select b->words[0];
            we do this by calculating (j / 32), which gives us 0
            Once we've selected b->words[0], newBit is used to create a bitmask
            A 1 is needed at bit 42, or bit (42 - 31 = 11) of b->words[0]
            To reliably get the bit within the word for the general case, we use j % 32 to get the remainder of a division with 32
            Essentially, j has two components: a + b, where b is a multiple of 32 and b is the remainder, and b is used
            to index into b->words, and a is used to selected a bit within a selected word
            This then gives us 21 in our example, which we subtract from 32 (32 - 21 = 11) to get the bit we need
            Then we have newBit << 11 to make our bitmask

            Then ou2 final step is b->words[0] = b->words[0] | newBit, which sets the 42nd bit of b to 1, the same as in bitseq
        */

        uint32_t newBit = 0;
        if(bitseq[i] == '1') newBit = 1;

        // The 1 here is required to get the right bitmask; I haven't looked into exactly why the rest of the expression isn't correct
        b->words[j / BITS_PER_WORD] |= newBit << (BITS_PER_WORD - (j % BITS_PER_WORD) - 1);
    }
}

// display a Bits value as sequence of 0's and 1's
void showBits(Bits b)
{
    // Iterate over the words
    for(int i = 0; i < b->nwords; i++) {
        // Iterate from the Most Significant Bit to the Least Significant Bit
        for(int j = BITS_PER_WORD - 1; j >= 0; j--) {
            // Create a mask to select the bit we want
            uint32_t mask = 1 << j;
            // b->words[i] & mask: Use the mask retrieve the value at the bit we want
            // !!(expression): Fairly hack-ish way to force an integer to be 1 or 0;
            // the C spec states that !0 == 0 and !(non-0 integer) == 1
            // So if done twice, 0 will become 0 and any non-0 integer will be 1
            // There's probably a better way but I didn't feel like researching it,
            // so came up with this on the spot
            printf("%u", !!(b->words[i] & mask));
        }
    }
}
