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
typedef uint64_t DWord;

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
    // Free the words of b, then b itself
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

    // If res is larger than a and b, we need to set the upper unused bits of res to 0
    for(i = 0; i < (res->nwords - a->nwords); i++) {
        res->words[i] = 0;
    }

    // Since the bits are right-aligned, then if res is larger than a and b, the word of res will be different to a and b
    // offset is set to the difference between the sizes, to account for this
    int offset = res->nwords - a->nwords;
    // Iterate over the words
    // Since the words are 32-bit integers, we can take a normal AND of each word in a and b, and set the word in res to the result
    for(i = 0; i < a->nwords; i++) {
        res->words[i + offset] = a->words[i] & b->words[i];
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

    // If res is larger than a and b, we need to set the upper unused bits of res to 0
    for(i = 0; i < (res->nwords - a->nwords); i++) {
        res->words[i] = 0;
    }

    // Since the bits are right-aligned, then if res is larger than a and b, the word of res will be different to a and b
    // offset is set to the difference between the sizes, to account for this
    int offset = res->nwords - a->nwords;
    // Iterate over the words
    // Since the words are 32-bit integers, we can take a normal OR of each word in a and b, and set the word in res to the result
    for(i = 0; i < a->nwords; i++) {
        res->words[i + offset] = a->words[i] | b->words[i];
    }
}

// form bit-wise negation of Bits a,b
// store result in res Bits
void invertBits(Bits a, Bits res)
{
    // Ensure the result Bits is of equal length or larger than a, to contain result
    assert(res->nwords >= a->nwords);

    int i = 0;

    // If res is larger than a, we need to set the upper unused bits of res to 0
    for(i = 0; i < (res->nwords - a->nwords); i++) {
        res->words[i] = 0;
    }

    // Since the bits are right-aligned, then if res is larger than a, the word of res will be different to a
    // offset is set to the difference between the sizes, to account for this
    int offset = res->nwords - a->nwords;
    // Iterate over the words
    // Since the words are 32-bit integers, we can take a normal NOT of each word in a, and set the word in res to the result
    for(i = 0; i < a->nwords; i++) {
        res->words[i + offset] = ~a->words[i];
    }
}

// left shift Bits
void leftShiftBits(Bits b, int shift, Bits res)
{
    /*
       To shift bits right and left, we need to shift each word in b by the required shift distance, then
       save the bits that were pushed off the end, and place them on the end of the next word

       We do this by copying the 32-bit word to a 64-bit dword, and performing the shift;
       since a 64-bit value can handle any shift up to 32 bits while still preserving the
       contents of the original 32-bit integer, we can save the overflow

       For example: Take a 32-bit integer:
       10111001110101101000101100111111
       If we shift this left by 3 bits, the result is:
       11001110101101000101100111111000
       And the leading 3 bits are lost.
       However, if we use a 64-bit integer:
       0000000000000000000000000000000010111001110101101000101100111111
       ^63                             ^31                            ^0
       And shift it by 3 bits:
       0000000000000000000000000000010111001110101101000101100111111000
       ^63                             ^31                            ^0
       The three leading bits are preserved in the upper 32 bits of the dword.

       These bits can then be masked with 0xFFFFFFFF00000000 and shifted 32 bits to the right,
       result in two 32-bit integers, the original value shifted:
       11001110101101000101100111111000
       and the 'carry' from the shift:
       00000000000000000000000000000101

       After the next word is shifted by three bits, the carry can be OR'd with the shifted value
       of the next word, resulting in a correctly shifted value
       */

    assert(shift < BITS_PER_WORD); // We can't handle shifts greater than 32 bits
    assert(shift >= 0); // We don't want to shift backwards

    // 64-bit shifting variable, in which we will perform the actual bit shifting
    DWord shifter = 0;
    // Carry variable
    Word carry = 0;

    // Iterate from the LSB to the MSB, shifting each word
    for(int i = b->nwords - 1; i >= 0; i--) {
        // Get the current word
        shifter = b->words[i];
        // Shift it back by the required number of bits
        shifter <<= shift;
        // Store least significant 32 bits of the shifted variable, combined with the carried
        // values from the last shift
        res->words[i] = (Word)shifter | carry;
        // Take the most significant 32 bits of the shift, and store them as the carry
        carry = (shifter & (0xFFFFFFFF00000000)) >> BITS_PER_WORD;
    }
}

// right shift Bits
void rightShiftBits(Bits b, int shift, Bits res)
{
    // See leftShiftBits for information

    assert(shift < BITS_PER_WORD); // We can't handle double shifts yet
    assert(shift >= 0); // We don't want to shift backwards

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
    // TODO: Don't make this assumption?
    assert(from->nwords == to->nwords);

    // Copy words from from to to
    for(int i = 0; i < from->nwords; i++) {
        to->words[i] = from->words[i];
    }
}

// assign a bit-string (sequence of 0's and 1's) to Bits
// if the bit-string is longer than the size of Bits, truncate higher-order bits
void setBitsFromString(Bits b, char *bitseq)
{
    int curWord;
    int curBit;
    int curChar;

    // Set b to 0 before we start to use it, so any old values don't interfere with the new ones
    for(int i = 0; i < b->nwords; i++) {
        b->words[i] = 0;
    }

    // Iterate over bitseq, setting bits in b
    // curWord: Current word in b->words
    // curBit: Current bit in the current word (left-aligned)
    // curChar: Current char in bitseq (left-aligned)
    curWord = b->nwords - 1;
    curBit = BITS_PER_WORD - 1;
    // Iterate over bitseq, starting and the end and moving towards the start
    // Terminate when we either each the start of bitseq, or when we run out of space in b
    for(curChar = (int)strlen(bitseq) - 1; curChar >= 0 && curWord >= 0; curChar--) {
        // Test if the current character is 1
        if(bitseq[curChar] == '1') {
            // If it is, prepare a bitmask; curBit is the left-aligned bit index,
            // so make it right-aligned to get the index of the bit for a shift
            int bitShift = (BITS_PER_WORD - 1) - curBit;
            // Then make a bitmask, and set the appropriate bit in the current word to 1
            b->words[curWord] |= 1 << bitShift;
        }

        // Decrement curBit
        curBit -= 1;
        // If we've reached the end of the word, then reset curBit to 31 and decrement curWord
        if(curBit < 0) {
            curBit = BITS_PER_WORD - 1;
            curWord -= 1;
        }
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
            // the C spec states that !0 == 1 and !(non-0 integer) == 0
            // So if done twice, 0 will become 0 and any non-0 integer will be 1
            // There's probably a better way but I didn't feel like researching it,
            // so came up with this on the spot
            // Or you could just use an if statement, but I also like this way
            printf("%u", !!(b->words[i] & mask));
        }
    }
}
