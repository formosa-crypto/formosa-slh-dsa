#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TESTS
#define TESTS 10000
#endif

#include "address.h"
#include "macros.h"
#include "randombytes.h"
#include "test_commons.h"

extern void set_layer_addr_jazz(uint32_t addr[8], uint32_t layer);
extern void set_tree_addr_jazz(uint32_t addr[8], uint64_t tree);
extern void set_type_jazz(uint32_t addr[8], uint32_t type);
extern void copy_subtree_addr_jazz(uint32_t out_addr[8], uint32_t in_addr[8]);
extern void set_keypair_addr_jazz(uint32_t addr[8], uint32_t type);
extern void copy_keypair_addr_jazz(uint32_t addr[8], uint32_t in_addr[8]);
extern void set_chain_addr_jazz(uint32_t addr[8], uint32_t chain);
extern void set_hash_addr_jazz(uint32_t addr[8], uint32_t hash);
extern void set_tree_height_jazz(uint32_t addr[8], uint32_t tree_height);
extern void set_tree_index_jazz(uint32_t addr[8], uint32_t tree_index);

void test_set_layer_addr(void);
void test_set_tree_addr(void);
void test_set_type(void);
void test_copy_subtree_addr(void);
void test_set_keypair_addr(void);
void test_copy_keypair_addr(void);
void test_set_chain_addr(void);
void test_set_hash_addr(void);
void test_set_tree_height(void);
void test_set_tree_index(void);

#ifndef ADDR_SIZE
#define ADDR_SIZE (sizeof(uint32_t) * 8)
#endif

void test_set_layer_addr(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t layer;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&layer, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_layer_addr_jazz(addr_jazz, layer);
    set_layer_addr(addr_ref, layer);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_set_tree_addr(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint64_t tree;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&tree, sizeof(uint64_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_tree_addr_jazz(addr_jazz, tree);
    set_tree_addr(addr_ref, tree);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_set_type(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t type;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&type, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_type_jazz(addr_jazz, type);
    set_type(addr_ref, type);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_copy_subtree_addr(void) {
    uint32_t in_addr_ref[8], out_addr_ref[8];
    uint32_t in_addr_jazz[8], out_addr_jazz[8];

    randombytes((uint8_t *)in_addr_ref, ADDR_SIZE);
    randombytes((uint8_t *)out_addr_ref, ADDR_SIZE);
    memcpy(in_addr_jazz, in_addr_ref, ADDR_SIZE);
    memcpy(out_addr_jazz, out_addr_ref, ADDR_SIZE);

    assert(!memcmp(in_addr_ref, in_addr_jazz, ADDR_SIZE));
    assert(!memcmp(out_addr_ref, out_addr_jazz, ADDR_SIZE));
    copy_subtree_addr_jazz(out_addr_jazz, in_addr_jazz);
    copy_subtree_addr(out_addr_ref, in_addr_ref);
    assert(!memcmp(in_addr_ref, in_addr_jazz, ADDR_SIZE));
    assert(!memcmp(out_addr_ref, out_addr_jazz, ADDR_SIZE));
}

void test_set_keypair_addr(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t keypair;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&keypair, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_keypair_addr_jazz(addr_jazz, keypair);
    set_keypair_addr(addr_ref, keypair);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_copy_keypair_addr(void) {
    uint32_t in_addr_ref[8], out_addr_ref[8];
    uint32_t in_addr_jazz[8], out_addr_jazz[8];

    randombytes((uint8_t *)in_addr_ref, ADDR_SIZE);
    randombytes((uint8_t *)out_addr_ref, ADDR_SIZE);
    memcpy(in_addr_jazz, in_addr_ref, ADDR_SIZE);
    memcpy(out_addr_jazz, out_addr_ref, ADDR_SIZE);

    assert(!memcmp(in_addr_ref, in_addr_jazz, ADDR_SIZE));
    assert(!memcmp(out_addr_ref, out_addr_jazz, ADDR_SIZE));
    copy_keypair_addr_jazz(out_addr_jazz, in_addr_jazz);
    copy_keypair_addr(out_addr_ref, in_addr_ref);
    assert(!memcmp(in_addr_ref, in_addr_jazz, ADDR_SIZE));
    assert(!memcmp(out_addr_ref, out_addr_jazz, ADDR_SIZE));
}

void test_set_chain_addr(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t chain;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&chain, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_chain_addr_jazz(addr_jazz, chain);
    set_chain_addr(addr_ref, chain);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_set_hash_addr(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t hash;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&hash, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_hash_addr_jazz(addr_jazz, hash);
    set_hash_addr(addr_ref, hash);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_set_tree_height(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t tree_height;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&tree_height, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_tree_height_jazz(addr_jazz, tree_height);
    set_tree_height(addr_ref, tree_height);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

void test_set_tree_index(void) {
    uint32_t addr_ref[8], addr_jazz[8];
    uint32_t tree_index;

    randombytes((uint8_t *)addr_ref, ADDR_SIZE);
    memcpy(addr_jazz, addr_ref, ADDR_SIZE);

    randombytes((uint8_t *)&tree_index, sizeof(uint32_t));

    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
    set_tree_index_jazz(addr_jazz, tree_index);
    set_tree_index(addr_ref, tree_index);
    assert(!memcmp(addr_ref, addr_jazz, ADDR_SIZE));
}

int main(void) {
    run_test(test_set_layer_addr, "set_layer_addr", TESTS);
    run_test(test_set_tree_addr, "set_tree_addr", TESTS);
    run_test(test_set_type, "set_type", TESTS);
    run_test(test_copy_subtree_addr, "copy_subtree_addr", TESTS);
    run_test(test_set_keypair_addr, "set_keypair_addr", TESTS);
    run_test(test_copy_keypair_addr, "copy_keypair_addr", TESTS);
    run_test(test_set_chain_addr, "set_chain_addr", TESTS);
    run_test(test_set_hash_addr, "set_hash_addr", TESTS);
    run_test(test_set_tree_height, "set_tree_height", TESTS);
    run_test(test_set_tree_index, "set_tree_index", TESTS);
    return EXIT_SUCCESS;
}
