#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "api.h"
#include "macros.h"
#include "randombytes.h"
#include "test_commons.h"

#ifndef PARAMS
#error "PARAMS undefined"
#endif

#ifndef TESTS
#define TESTS 100
#endif

#ifndef MSG_SIZE
#define MSG_SIZE 1042
#endif

void test_wots(void);

/*
 * The Wots+ functions in the C implementation are replaced by the jasmin version
 */
void test_wots(void) {
    uint8_t msg[MSG_SIZE] = {0};
    size_t message_length = MSG_SIZE;

    uint8_t secret_key[CRYPTO_SECRETKEYBYTES] = {0};
    uint8_t public_key[CRYPTO_PUBLICKEYBYTES] = {0};

    uint8_t signature[CRYPTO_BYTES] = {0};
    size_t signature_length;

    randombytes(msg, MSG_SIZE * sizeof(uint8_t));

    crypto_sign_keypair(public_key, secret_key);
    crypto_sign_signature(signature, &signature_length, msg, message_length, secret_key);

    assert(crypto_sign_verify(signature, signature_length, msg, message_length, public_key) == 0);
}

int main(void) {
    run_test(test_wots, "WOTS", TESTS);
    return EXIT_SUCCESS;
}