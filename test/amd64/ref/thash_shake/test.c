#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TESTS
#define TESTS 10000
#endif

#include "address.h"
#include "context.h"
#include "macros.h"
#include "randombytes.h"
#include "test_commons.h"
#include "thash.h"

extern void thash_inplace_jazz(uint8_t *out, const uint8_t *pub_seed, const uint32_t *addr);

void test_thash_inplace(void);

void test_thash_inplace(void) {
    uint8_t out[SPX_N] = {0};
    uint8_t in[SPX_N];
    spx_ctx ctx;
    uint32_t addr[8] = {0};

    randombytes(in, SPX_N);
    randombytes(ctx.pub_seed, SPX_N);
    randombytes((uint8_t *)addr, 8 * sizeof(uint32_t));

    thash(out, in, 1, &ctx, addr);
    thash_inplace_jazz(in, ctx.pub_seed, addr);

    assert(!memcmp(in, out, SPX_N));
}

int main(void) {
    run_test(test_thash_inplace, "thash inplace", TESTS);
    return EXIT_SUCCESS;
}
