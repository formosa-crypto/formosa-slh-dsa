#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "address.h"
#include "api.h"
#include "fors.h"
#include "hash.h"
#include "merkle.h"
#include "params.h"
#include "print.h"
#include "randombytes.h"
#include "thash.h"
#include "utils.h"
#include "wots.h"

/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void) { return CRYPTO_SECRETKEYBYTES; }

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void) { return CRYPTO_PUBLICKEYBYTES; }

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void) { return CRYPTO_BYTES; }

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void) { return CRYPTO_SEEDBYTES; }

#ifndef TEST_MERKLE

/*
 * Generates an SPX key pair given a seed of length
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed) {
    spx_ctx ctx;

    /* Initialize SK_SEED, SK_PRF and PUB_SEED from seed. */
    memcpy(sk, seed, CRYPTO_SEEDBYTES);

    memcpy(pk, sk + 2 * SPX_N, SPX_N);

    memcpy(ctx.pub_seed, pk, SPX_N);
    memcpy(ctx.sk_seed, sk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(&ctx);

    /* Compute root node of the top-most subtree. */
    merkle_gen_root(sk + 3 * SPX_N, &ctx);

    memcpy(pk + SPX_N, sk + 3 * SPX_N, SPX_N);

    return 0;
}

#else

extern void merkle_gen_root_jazz(uint8_t *root, const uint8_t *pub_seed, const uint8_t *sk_seed);

int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed) {
    spx_ctx ctx;
    uint8_t root_jazz[SPX_N] = {0};
    unsigned char root_ref[SPX_N] = {0};

    /* Initialize SK_SEED, SK_PRF and PUB_SEED from seed. */
    memcpy(sk, seed, CRYPTO_SEEDBYTES);

    memcpy(pk, sk + 2 * SPX_N, SPX_N);

    memcpy(ctx.pub_seed, pk, SPX_N);
    memcpy(ctx.sk_seed, sk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(&ctx);

    /* Compute root node of the top-most subtree. */
    merkle_gen_root(sk + 3 * SPX_N, &ctx);
    merkle_gen_root_jazz(root_jazz, ctx.pub_seed, ctx.sk_seed);

    memcpy(root_ref, sk + 3 * SPX_N, SPX_N);

    if (memcmp(root_jazz, root_ref, SPX_N) != 0) {
        print_str_u8("ref", root_ref, SPX_N);
        print_str_u8("jazz", root_jazz, SPX_N);
    }

    assert(memcmp(root_jazz, root_ref, SPX_N) == 0);

    memcpy(pk + SPX_N, sk + 3 * SPX_N, SPX_N);

    return 0;
}

#endif

/*
 * Generates an SPX key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
    unsigned char seed[CRYPTO_SEEDBYTES];
    randombytes(seed, CRYPTO_SEEDBYTES);
    crypto_sign_seed_keypair(pk, sk, seed);
    return 0;
}

#ifdef TEST_FORS
/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *sk) {
    spx_ctx ctx;

    const unsigned char *sk_prf = sk + SPX_N;
    const unsigned char *pk = sk + 2 * SPX_N;

    unsigned char optrand[SPX_N];
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char root[SPX_N];
    uint32_t i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};

    uint8_t sig_jazz[SPX_BYTES - SPX_N];
    uint8_t root_jazz[SPX_N];

    memcpy(ctx.sk_seed, sk, SPX_N);
    memcpy(ctx.pub_seed, pk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);

    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);

    randombytes(optrand, SPX_N);
    gen_message_random(sig, sk_prf, optrand, m, mlen, &ctx);

    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);

    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    // copy the state
    memcpy(sig_jazz, sig, SPX_BYTES - SPX_N);
    memcpy(root_jazz, root, SPX_N);

    fors_sign(sig, root, mhash, &ctx, wots_addr);

    fors_sign_jazz(sig_jazz, root_jazz, mhash, ctx.pub_seed, ctx.sk_seed, wots_addr);

    assert(memcmp(sig, sig_jazz, SPX_BYTES - SPX_N) == 0);
    assert(memcmp(root, root_jazz, SPX_N) == 0);

    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        // merkle_sign(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);
        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT) - 1));
        tree = tree >> SPX_TREE_HEIGHT;
    }
    *siglen = SPX_BYTES;

    return 0;
}
#else

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *sk) {
    spx_ctx ctx;

    const unsigned char *sk_prf = sk + SPX_N;
    const unsigned char *pk = sk + 2 * SPX_N;

    unsigned char optrand[SPX_N];
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char root[SPX_N];
    uint32_t i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};

    memcpy(ctx.sk_seed, sk, SPX_N);
    memcpy(ctx.pub_seed, pk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);

    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);

    randombytes(optrand, SPX_N);
    gen_message_random(sig, sk_prf, optrand, m, mlen, &ctx);

    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);

    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

#ifdef DEBUG_FORS_SIGN
    extern int fors_sign_test_number;  // global variable defined in test_sign.c
    char file_path[256];
    sprintf(file_path, "fors_sign_failed_tests/test_%d.txt", fors_sign_test_number++);

    fprint_str_u8(file_path, "sig_ref", sig, SPX_BYTES - SPX_N);
    fprint_str_u8(file_path, "pk_ref", root, SPX_N);
    fprint_str_u8(file_path, "mhash", mhash, SPX_FORS_MSG_BYTES);
    fprint_str_u8(file_path, "pub_seed", ctx.pub_seed, SPX_N);
    fprint_str_u8(file_path, "sk_seed", ctx.sk_seed, SPX_N);
    fprint_str_u8(file_path, "addr", wots_addr, 8 * sizeof(uint32_t));
#endif

    fors_sign(sig, root, mhash, &ctx, wots_addr);

    sig += SPX_FORS_BYTES;
    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        // merkle_sign(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);
        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT) - 1));
        tree = tree >> SPX_TREE_HEIGHT;
    }
    *siglen = SPX_BYTES;

    return 0;
}

#endif

#ifndef TEST_FORS

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen, const uint8_t *pk) {
    spx_ctx ctx;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];
    unsigned char root[SPX_N] = {0};
    unsigned char leaf[SPX_N];
    unsigned int i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t wots_pk_addr[8] = {0};

    if (siglen != SPX_BYTES) {
        return -1;
    }

    memcpy(ctx.pub_seed, pk, SPX_N);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

#ifdef TEST_HASH_MESSAGE
    unsigned char mhash_jazz[SPX_FORS_MSG_BYTES];
    uint64_t tree_jazz;
    uint32_t idx_leaf_jazz;

    typedef struct {
        uint8_t R[SPX_N];
        uint8_t pk[SPX_PK_BYTES];
    } args;

    // Copy state
    args _args;
    memcpy(_args.R, sig, SPX_N);  // randomness
    memcpy(_args.pk, pk, SPX_PK_BYTES);

    assert(memcmp(sig, _args.R, SPX_N) == 0);
    assert(memcmp(pk, _args.pk, SPX_PK_BYTES) == 0);

    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    hash_message_jazz(mhash_jazz, &tree_jazz, &idx_leaf_jazz, &_args, m, mlen);  // jasmin impl

    assert(memcmp(mhash_jazz, mhash, SPX_FORS_MSG_BYTES * sizeof(unsigned char)) == 0);
    assert(tree_jazz == tree);
    assert(idx_leaf_jazz == idx_leaf);

#else
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
#endif

    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    fors_pk_from_sig(root, sig, mhash, &ctx, wots_addr);

    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        copy_keypair_addr(wots_pk_addr, wots_addr);
        wots_pk_from_sig(wots_pk, sig, root, &ctx, wots_addr);
        sig += SPX_WOTS_BYTES;

        thash(leaf, wots_pk, SPX_WOTS_LEN, &ctx, wots_pk_addr);

        compute_root(root, leaf, idx_leaf, 0, sig, SPX_TREE_HEIGHT, &ctx, tree_addr);
        sig += SPX_TREE_HEIGHT * SPX_N;

        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT) - 1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    if (memcmp(root, pub_root, SPX_N)) {
        return -1;
    }
    return 0;
}

#else

int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen, const uint8_t *pk) {
    spx_ctx ctx;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];

    ////
    unsigned char root[SPX_N];
    uint8_t root_jazz[SPX_N];
    ///

    unsigned char leaf[SPX_N];
    unsigned int i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t wots_pk_addr[8] = {0};

    if (siglen != SPX_BYTES) {
        return -1;
    }

    memcpy(ctx.pub_seed, pk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

    /* Derive the message digest and leaf index from R || PK || M. */
    /* The additional SPX_N is a result of the hash domain separator. */
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    sig += SPX_N;

    /* Layer correctly defaults to 0, so no need to set_layer_addr */
    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    fors_pk_from_sig(root, sig, mhash, &ctx, wots_addr);
    fors_pk_from_sig_jazz(root_jazz, sig, mhash, ctx.pub_seed, wots_addr);

    assert(memcmp(root, root_jazz, SPX_N) == 0);

    sig += SPX_FORS_BYTES;

    /* For each subtree.. */
    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        copy_keypair_addr(wots_pk_addr, wots_addr);

        /* The WOTS public key is only correct if the signature was correct. */
        /* Initially, root is the FORS pk, but on subsequent iterations it is
           the root of the subtree below the currently processed subtree. */
        wots_pk_from_sig(wots_pk, sig, root, &ctx, wots_addr);
        sig += SPX_WOTS_BYTES;

        /* Compute the leaf node using the WOTS public key. */
        thash(leaf, wots_pk, SPX_WOTS_LEN, &ctx, wots_pk_addr);

        /* Compute the root node of this subtree. */
        compute_root(root, leaf, idx_leaf, 0, sig, SPX_TREE_HEIGHT, &ctx, tree_addr);
        sig += SPX_TREE_HEIGHT * SPX_N;

        /* Update the indices for the next layer. */
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT) - 1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    /* Check if the root node equals the root node in the public key. */
    if (memcmp(root, pub_root, SPX_N)) {
        return -1;
    }

    return 0;
}

#endif

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk) {
    size_t siglen;

    crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

    memmove(sm + SPX_BYTES, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk) {
    /* The API caller does not necessarily know what size a signature should be
       but SPHINCS+ signatures are always exactly SPX_BYTES. */
    if (smlen < SPX_BYTES) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    *mlen = smlen - SPX_BYTES;

    if (crypto_sign_verify(sm, SPX_BYTES, sm + SPX_BYTES, (size_t)*mlen, pk)) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    /* If verification was successful, move the message to the right place. */
    memmove(m, sm + SPX_BYTES, *mlen);

    return 0;
}
