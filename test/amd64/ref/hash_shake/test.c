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
#include "hash.h"
#include "macros.h"
#include "randombytes.h"
#include "test_commons.h"

extern void prf_addr_jazz(uint8_t *out, const uint8_t *pub_seed, const uint8_t *sk_seed, const uint32_t *addr);
extern void gen_message_random_jazz(uint8_t *R, const uint8_t *sk_prf, const uint8_t *optrand, const uint8_t *msg,
                                    size_t msg_len);
extern void hash_message_jazz(uint8_t *digest, const uint8_t *R, const uint8_t *PK, const uint8_t *msg, size_t msg_len,
                              void *cenas);

void test_prf_addr(void);
void test_gen_message_random(void);
void test_hash_message(void);

void test_prf_addr(void) {
    uint8_t out_ref[SPX_N] = {0};
    uint8_t out_jazz[SPX_N] = {0};
    spx_ctx ctx;
    uint32_t addr[8] = {0};

    randombytes(out_ref, SPX_N);
    memcpy(out_jazz, out_ref, SPX_N);
    randombytes(ctx.pub_seed, SPX_N);
    randombytes(ctx.sk_seed, SPX_N);
    randombytes((uint8_t *)addr, 8 * sizeof(uint32_t));

    assert(!memcmp(out_ref, out_jazz, SPX_N));
    prf_addr(out_ref, &ctx, addr);
    prf_addr_jazz(out_jazz, ctx.pub_seed, ctx.sk_seed, addr);
    assert(!memcmp(out_ref, out_jazz, SPX_N));
}

void test_gen_message_random(void) {
#ifndef MSG_SIZE
#define MSG_SIZE 32
#endif

    uint8_t R_ref[SPX_N] = {0};
    uint8_t R_jazz[SPX_N] = {0};
    uint8_t sk_prf[SPX_N] = {0};
    uint8_t optrand[SPX_N] = {0};
    uint8_t msg[MSG_SIZE] = {0};
    spx_ctx ctx;

    randombytes(sk_prf, SPX_N);
    randombytes(optrand, SPX_N);
    randombytes(msg, MSG_SIZE);

    gen_message_random(R_ref, sk_prf, optrand, msg, SPX_N, &ctx);
    gen_message_random_jazz(R_jazz, sk_prf, optrand, msg, SPX_N);
    assert(!memcmp(R_ref, R_jazz, SPX_N));

#undef MSG_SIZE
}

void test_hash_message(void) {
    void *buffer = malloc(sizeof(uint64_t) + sizeof(uint32_t));
    if (!buffer) {
        perror("malloc failed");
        exit(1);
    }

    // TODO:

    free(buffer);
}

int main(void) {
    run_test(test_prf_addr, "PRF ADDR", TESTS);
    run_test(test_gen_message_random, "GEN MESSAGE RANDOM", TESTS);
    run_test(test_hash_message, "HASH MESSAGE (test not implemented yet)", TESTS);
    return EXIT_SUCCESS;
}
