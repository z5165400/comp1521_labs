// ADT for Bit-strings
// COMP1521 17s2 Week02 Lab Exercise
// Written by John Shepherd, July 2017
// Modified by Andrew Walls, July 2017

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Bits.h"

// assumes that an unsigned int is 32 bits
#define BITS_PER_WORD 32

// A bit-string is an array of unsigned ints (each a 32-bit Word)
// The number of bits (hence Words) is determined at creation time
// Words are indexed from right-to-left
// words[0] contains the most significant bits
// words[nwords-1] contains the least significant bits
// Within each Word, bits are indexed right-to-left
// Bit position 0 in the Word is the least significant bit
// Bit position 31 in the Word is the most significant bit

typedef unsigned int Word;

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
   new->words = calloc(nwords, sizeof(Word));
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
        res->words[i] = ~a->words[i - offset];
    }
}

// left shift Bits
void leftShiftBits(Bits b, int shift, Bits res)
{
   // challenge problem
}

// right shift Bits
void rightShiftBits(Bits b, int shift, Bits res)
{
   // challenge problem
}

// copy value from one Bits object to another
void setBitsFromBits(Bits from, Bits to)
{
   // TODO
}

// assign a bit-string (sequence of 0's and 1's) to Bits
// if the bit-string is longer than the size of Bits, truncate higher-order bits
void setBitsFromString(Bits b, char *bitseq)
{
    int i;
    int j = b->nwords - 1;
    int k = (int)strlen(bitseq);

    // Reinitialise b to 0, in case it's already been used
    memset(b->words, 0, sizeof(unsigned int) * b->nwords);

    // Loop from the end of the string towards the start, terminating
    // when either the end of the string or the end of the Bits array is
    // reached
    // i is already initalised
    for(i = k; i >= 0 && j >= 0; i--) {
        // Prepare an 32-bit integer to use as a mask
        unsigned int newBit = 0;
        // Set LSB of newBit if the character is 1
        if(bitseq[i] == '1') newBit = 1;
        // Prepare bitmask, shifting newBit to correct position, and apply
        b->words[j] |= newBit << ((BITS_PER_WORD - 1) - (i % BITS_PER_WORD));
        // Decrement j to move to a new word at the end of each
        if(i != k && i % BITS_PER_WORD == 0) j--;
    }
    //printf("\n");
}

// display a Bits value as sequence of 0's and 1's
void showBits(Bits b)
{
    // Iterate over the words
    for(int i = 0; i < b->nwords; i++) {
        // Iterate from the Most Significant Bit to the Least Significant Bit
        for(int j = BITS_PER_WORD - 1; j >= 0; j--) {
            // Create a mask to select the bit we want
            unsigned int mask = 1 << j;
            // b->words[i] & mask: Use the mask retrieve the value at the bit we want
            // !!(expression): Fairly hack-ish way to force an integer to be 1 or 0;
            // the C spec states that !0 == 0 and !(non-0 integer) == 1
            // So if done twice, 0 will become 0 and any non-0 integer will be 1
            // There's probably a better way but I didn't feel like researching it,
            // so came up with this on the spot
            printf("%u", !!(b->words[i] & mask));
        }
    }
    printf("\n");
}
