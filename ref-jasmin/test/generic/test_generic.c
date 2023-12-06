#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include "macros.h"
#include "notrandombytes.c"

#ifndef TESTS
#define TESTS 1000
#endif

#ifndef INLEN
#define INLEN 64
#endif

extern void cond_u32_a_below_b_and_a_below_c_jazz(uint32_t a, uint32_t b, uint32_t c, uint8_t *out);
extern void cond_u32_a_eq_b_and_c_below_d_jazz(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                                               uint8_t *out);

extern void cond_u64_a_below_b_and_a_below_c_jazz(uint64_t a, uint64_t b, uint64_t c, uint8_t *out);
extern void cond_u64_a_dif_b_and_a_dif_c_jazz(uint64_t a, uint64_t b, uint64_t c, uint8_t *out);
extern void cond_u64_a_dif_b_and_c_dif_d_jazz(uint64_t a, uint64_t b, uint64_t c, uint64_t d,
                                              uint8_t *out);

extern void ull_to_bytes_jazz(uint8_t *out, uint64_t v);
extern uint64_t bytes_to_ull_jazz(const uint8_t *a);

#define zero_array_u32_jazz NAMESPACE1(zero_array_u32_jazz, INLEN)
extern void zero_array_u32_jazz(uint32_t *);

#define mem_eq_u8_jazz NAMESPACE1(mem_eq_u8_jazz, INLEN)
extern int mem_eq_u8_jazz(const uint8_t *, const uint8_t *);

extern void get_sk_prf_from_sk_jazz(const uint8_t *sk, uint8_t *prf);

// u32
void test_cond_u32_a_below_b_and_a_below_c(void);
void test_cond_u32_a_eq_b_and_c_below_d(void);

// u64
void test_cond_u64_a_below_b_and_a_below_c(void);
void test_cond_u64_a_dif_b_and_a_dif_c(void);
void test_cond_u64_a_dif_b_and_c_dif_d(void);

void test_ull_to_bytes(void);
void test_bytes_to_ull(void);
void test_zero_array_u32_jazz(void);
void test_mem_eq_u8_jazz(void);

void test_cond_u64_a_below_b_and_a_below_c(void) {
    uint64_t a, b, c;
    uint8_t r;

    for (int i = 0; i < TESTS; i++) {
        randombytes1((uint8_t *)&a, sizeof(uint64_t));
        randombytes1((uint8_t *)&b, sizeof(uint64_t));
        randombytes1((uint8_t *)&c, sizeof(uint64_t));

        cond_u64_a_below_b_and_a_below_c_jazz(a, b, c, &r);
        assert((a < b && a < c) ? (r == 1) : (r == 0));
    }
}

void test_cond_u32_a_below_b_and_a_below_c(void) {
    uint32_t a, b, c;
    uint8_t r;

    for (int i = 0; i < TESTS; i++) {
        randombytes((uint8_t *)&a, sizeof(uint32_t));
        randombytes((uint8_t *)&b, sizeof(uint32_t));
        randombytes((uint8_t *)&c, sizeof(uint32_t));

        cond_u32_a_below_b_and_a_below_c_jazz(a, b, c, &r);
        assert((a < b && a < c) ? (r == 1) : (r == 0));
    }
}

void test_cond_u32_a_eq_b_and_c_below_d(void) {
    uint32_t a, b, c, d;
    uint8_t r;

    for (int i = 0; i < TESTS; i++) {
        // These tests will most likely have a != b
        randombytes((uint8_t *)&a, sizeof(uint32_t));
        randombytes((uint8_t *)&b, sizeof(uint32_t));
        randombytes((uint8_t *)&c, sizeof(uint32_t));
        randombytes((uint8_t *)&d, sizeof(uint32_t));

        cond_u32_a_eq_b_and_c_below_d_jazz(a, b, c, d, &r);
        assert((a == b && c < d) ? (r == 1) : (r == 0));

    }

    for (int i = 0; i < TESTS; i++) {
        // these tests have a = b
        randombytes((uint8_t *)&a, sizeof(uint32_t));
        b = a;
        randombytes((uint8_t *)&c, sizeof(uint32_t));
        randombytes((uint8_t *)&d, sizeof(uint32_t));

        cond_u32_a_eq_b_and_c_below_d_jazz(a, b, c, d, &r);
        assert((a == b && c < d) ? (r == 1) : (r == 0));
    }

    for (int i = 0; i < TESTS; i++) {
        // these tests have c < d
        randombytes((uint8_t *)&a, sizeof(uint32_t));
        randombytes((uint8_t *)&b, sizeof(uint32_t));
        randombytes((uint8_t *)&d, sizeof(uint32_t));
        c = d - 10;

        cond_u32_a_eq_b_and_c_below_d_jazz(a, b, c, d, &r);
        assert((a == b && c < d) ? (r == 1) : (r == 0));
    }

    for (int i = 0; i < TESTS; i++) {
        // these tests have a == b && c < d
        randombytes((uint8_t *)&a, sizeof(uint32_t));
        b = a;
        randombytes((uint8_t *)&d, sizeof(uint32_t));
        c = d - 10;

        cond_u32_a_eq_b_and_c_below_d_jazz(a, b, c, d, &r);
        assert((a == b && c < d) ? (r == 1) : (r == 0));
    }
}

void test_cond_u64_a_dif_b_and_a_dif_c(void) {
    uint64_t a, b, c;
    uint8_t r;

    for (int i = 0; i < TESTS; i++) {
        randombytes((uint8_t *)&a, sizeof(uint64_t));
        randombytes((uint8_t *)&b, sizeof(uint64_t));
        randombytes((uint8_t *)&c, sizeof(uint64_t));

        cond_u64_a_dif_b_and_a_dif_c_jazz(a, b, c, &r);
        assert((a != b && a != c) ? (r == 1) : (r == 0));
    }
}

void test_cond_u64_a_dif_b_and_c_dif_d(void) {
    uint64_t a, b, c, d;
    uint8_t r;

    for (int i = 0; i < TESTS; i++) {
        randombytes1((uint8_t *)&a, sizeof(uint64_t));
        randombytes1((uint8_t *)&b, sizeof(uint64_t));
        randombytes1((uint8_t *)&c, sizeof(uint64_t));
        randombytes1((uint8_t *)&d, sizeof(uint64_t));

        cond_u64_a_dif_b_and_c_dif_d_jazz(a, b, c, d, &r);
        assert((a != b && c != d) ? (r == 1) : (r == 0));
    }
}

// from C impl
static void ull_to_bytes(unsigned char *out, unsigned int outlen, unsigned long long in) {
    int i;

    /* Iterate over out in decreasing order, for big-endianness. */
    for (i = (signed int)outlen - 1; i >= 0; i--) {
        out[i] = in & 0xff;
        in = in >> 8;
    }
}

void test_ull_to_bytes(void) {
    unsigned char out0[8] = {0};  // uint8_t
    unsigned char out1[8] = {0};  // uint8_t
    unsigned long long v;         // uint64_t

    for (int i = 0; i < TESTS; i++) {
        randombytes1((uint8_t *)&v, sizeof(unsigned long long));
        ull_to_bytes(out0, 8, v);
        ull_to_bytes_jazz(out1, v);
        assert(memcmp(out0, out1, 8) == 0);
    }
}

// from C impl
static unsigned long long bytes_to_ull(const unsigned char *in, unsigned int inlen) {
    unsigned long long retval = 0;
    unsigned int i;

    for (i = 0; i < inlen; i++) {
        retval |= ((unsigned long long)in[i]) << (8 * (inlen - 1 - i));
    }
    return retval;
}

void test_bytes_to_ull(void) {
    uint64_t out0, out1;
    uint8_t in[8];

    for (int i = 0; i < TESTS; i++) {
        randombytes1(in, 8);
        out0 = bytes_to_ull(in, 8);
        out1 = bytes_to_ull_jazz(in);
        assert(out0 == out1);
    }
}

void test_zero_array_u32_jazz(void) {
    uint32_t in[INLEN];
    uint32_t zero[INLEN] = {0};

    for (int i = 0; i < TESTS; i++) {
        randombytes1((uint8_t *)in, INLEN);
        zero_array_u32_jazz(in);
        assert(memcmp(in, zero, INLEN) == 0);
    }
}

void test_mem_eq_u8_jazz(void) {
    uint8_t a[INLEN], b[INLEN];
    int r;

    for (int i = 0; i < TESTS; i++) {
        randombytes1(a, INLEN);
        randombytes1(b, INLEN);
        r = mem_eq_u8_jazz(a, b);
        assert(r == memcmp(a, b, INLEN));
    }
}

int main(void) {
    // test_cond_u32_a_below_b_and_a_below_c();
    test_cond_u32_a_eq_b_and_c_below_d();

    // test_cond_u64_a_below_b_and_a_below_c();
    // test_cond_u64_a_dif_b_and_a_dif_c();
    // test_cond_u64_a_dif_b_and_c_dif_d();

    // test_ull_to_bytes();
    // test_bytes_to_ull();

    // test_zero_array_u32_jazz();

    puts("PASS: generic");
    return 0;
}
