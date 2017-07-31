#include "Bits.h"
#include "minunit.h"

mu_suite_start();

static Bits bits;
static Bits bits2;
static Bits bits3;

typedef unsigned int Word;

struct BitsRep {
   int nwords;   // # of Words
   Word *words;  // array of Words
};

static char *test_makeBits(void)
{
    unsigned int cmp[4] = {0};

    for (unsigned int i = 0; i <= 128; i += 16) {
        bits = makeBits((int)i);

        if(i % 32 == 0) {
            mu_assert(bits->nwords == i / 32, "nwords set incorrectly (was %d, should be %d)", bits->nwords, i / 32);
            mu_assert(memcmp(bits->words, cmp, i / 32) == 0, "words not initialised to 0 for %d bits", i);
        } else {
            mu_assert(bits->nwords == i / 32 + 1, "nwords set incorrectly (was %d, should be %d)", bits->nwords, i / 32 + 1);
            mu_assert(memcmp(bits->words, cmp, i / 32 + 1) == 0, "words not initialised to 0 for %d bits", i);
        }

        freeBits(bits);
    }

    return NULL;
}

static char *test_setBitsFromString(void)
{
    bits = makeBits(64);

    setBitsFromString(bits, "0000000000000000000000000000000000000000001001001001001001001001");
    mu_assert(bits->words[0] == 0, "Bits 0-31 do not form 32-bit integer with value 0 (had %#010x)", bits->words[0]);
    mu_assert(bits->words[1] == 0x00249249, "Bits 32-63 do not form 32-bit integer with hex 0x00249249 (had %#010x)", bits->words[1]);

    setBitsFromString(bits, "1111111111111111111111111111111111111111110111110111110111110111");
    mu_assert(bits->words[0] == 0xFFFFFFFF, "Bits 0-31 do not form 32-bit integer with hex 0xFFFFFFFF (had %#010x)", bits->words[0]);
    mu_assert(bits->words[1] == 0xFFDF7DF7, "Bits 32-63 do not form 32-bit integer with hex 0xFFDF7DF7 (had %#010x)", bits->words[1]);

    // TODO: More tests!

    freeBits(bits);
    return NULL;
}

static char *test_andBits(void)
{
    bits = makeBits(64);
    bits2 = makeBits(64);
    bits3 = makeBits(64);

    setBitsFromString(bits,  "0000000000000000000000000000000000000000001001001001001001001001");
    setBitsFromString(bits2, "1111111111111111111111111111111111111111110111110111110111110111");
    andBits(bits, bits2, bits3);

    mu_assert(bits3->words[0] == 0, "Bits 0-31 are not 0 after AND with inequals (had %#010x)", bits3->words[0]);
    mu_assert(bits3->words[1] == 0x41041, "Bits 32-64 are not 0 after AND with inequals (had %#010x)", bits3->words[1]);
    
    bits->words[0] = 0; bits->words[1] = 0;
    bits2->words[0] = 0; bits2->words[1] = 0;
    andBits(bits, bits2, bits3);

    mu_assert(bits3->words[0] == 0, "Bits 0-31 are not 0 after AND with 0s (had %#010x)", bits3->words[0]);
    mu_assert(bits3->words[1] == 0, "Bits 32-64 are not 0 after AND with 0s (had %#010x)", bits3->words[1]);

    bits->words[0] = bits->words[1] = 0xFFFFFFFF;
    bits2->words[0] = bits2->words[1] = 0xFFFFFFFF;
    andBits(bits, bits2, bits3);

    mu_assert(bits3->words[0] == 0xFFFFFFFF, "Bits 0-31 are not 1s after AND with 1s (had %#010x)", bits3->words[0]);
    mu_assert(bits3->words[1] == 0xFFFFFFFF, "Bits 32-64 are not 1s after AND with 1s (had %#010x)", bits3->words[1]);

    // TODO: Test overflows
    
    freeBits(bits);
    freeBits(bits2);
    freeBits(bits3);
    return NULL;
}

static char *test_orBits(void)
{
    bits = makeBits(64);
    bits2 = makeBits(64);
    bits3 = makeBits(64);

    setBitsFromString(bits,  "0000000000000000000000000000000000000000001001001001001001001001");
    setBitsFromString(bits2, "1111111111111111111111111111111111111111110111110111110111110111");
    orBits(bits, bits2, bits3);

    mu_assert(bits3->words[0] == 0xFFFFFFFF, "Bits 0-31 are not 1s after OR with inequals (had %#010x)", bits3->words[0]);
    mu_assert(bits3->words[1] == 0xFFFFFFFF, "Bits 32-64 are not 1s after OR with inequals (had %#010x)", bits3->words[1]);
    
    bits->words[0] = 0; bits->words[1] = 0;
    bits2->words[0] = 0; bits2->words[1] = 0;
    orBits(bits, bits2, bits3);

    mu_assert(bits3->words[0] == 0, "Bits 0-31 are not 0 after OR with 0s (had %#010x)", bits3->words[0]);
    mu_assert(bits3->words[1] == 0, "Bits 32-64 are not 0 after OR with 0s (had %#010x)", bits3->words[1]);

    bits->words[0] = bits->words[1] = 0xFFFFFFFF;
    bits2->words[0] = bits2->words[1] = 0xFFFFFFFF;
    orBits(bits, bits2, bits3);

    mu_assert(bits3->words[0] == 0xFFFFFFFF, "Bits 0-31 are not 1s after OR with 1s (had %#010x)", bits3->words[0]);
    mu_assert(bits3->words[1] == 0xFFFFFFFF, "Bits 32-64 are not 1s after OR with 1s (had %#010x)", bits3->words[1]);
    
    freeBits(bits);
    freeBits(bits2);
    freeBits(bits3);
    return NULL;
}

static char *test_invertBits(void)
{
    bits = makeBits(64);
    bits2 = makeBits(64);

    setBitsFromString(bits,  "0000000000000000000000000000000000000000001001001001001001001001");
    invertBits(bits, bits2);

    mu_assert(bits2->words[0] == 0xFFFFFFFF, "Bits 0-31 are not 1s after NOT with variants (had %#010x)", bits2->words[0]);
    mu_assert(bits2->words[1] == 0xFFDB6DB6, "Bits 32-64 are not 0xFFDB6DB6 after NOT with variants (had %#010x)", bits2->words[1]);
    
    bits->words[0] = 0; bits->words[1] = 0;
    invertBits(bits, bits2);

    mu_assert(bits2->words[0] == 0xFFFFFFFF, "Bits 0-31 are not 1s after NOT with 0s (had %#010x)", bits2->words[0]);
    mu_assert(bits2->words[1] == 0xFFFFFFFF, "Bits 32-64 are not 1s after NOT with 0s (had %#010x)", bits2->words[1]);

    bits->words[0] = bits->words[1] = 0xFFFFFFFF;
    invertBits(bits, bits2);

    mu_assert(bits2->words[0] == 0, "Bits 0-31 are not 0s after NOT with 1s (had %#010x)", bits2->words[0]);
    mu_assert(bits2->words[1] == 0, "Bits 32-64 are not 0s after NOT with 1s (had %#010x)", bits2->words[1]);
    
    freeBits(bits);
    freeBits(bits2);
    return NULL;
}

static char *test_setBitsFromBits(void)
{
    bits = makeBits(64);
    bits2 = makeBits(64);

    setBitsFromString(bits,  "0000000000000000000000000000000000000000001001001001001001001001");
    setBitsFromBits(bits, bits2);

    mu_assert(bits2->words[0] == bits->words[0], "bits and bits2 0-31 not equal after copy (%#010x, %#010x)", bits->words[0], bits2->words[0]);
    mu_assert(bits2->words[1] == bits->words[1], "bits and bits2 32-64 not equal after copy (%#010x, %#010x)", bits->words[1], bits2->words[1]);

    freeBits(bits);
    freeBits(bits2);
    return NULL;
}

static char *test_leftShiftBits(void)
{
    bits = makeBits(64);

    mu_assert(0, "Test not yet implemented. Please do so.");

    freeBits(bits);
    return NULL;
}

static char *test_rightShiftBits(void)
{
    bits = makeBits(64);

    mu_assert(0, "Test not yet implemented. Please do so.");

    freeBits(bits);
    return NULL;
}


static char *all_tests(void) {
    mu_run_test(test_makeBits);
    mu_run_test(test_setBitsFromString);
    mu_run_test(test_andBits);
    mu_run_test(test_orBits);
    mu_run_test(test_invertBits);
    mu_run_test(test_setBitsFromBits);
    mu_run_test(test_leftShiftBits);
    mu_run_test(test_rightShiftBits);

    return NULL;
}

RUN_TESTS(all_tests)
