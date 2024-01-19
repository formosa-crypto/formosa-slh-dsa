#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "address.h"
#include "api.h"
#include "hash.h"
#include "merkle.h"
#include "params.h"
#include "randombytes.h"
#include "utils.h"
#include "wots.h"

// address
extern void copy_keypair_addr_jazz(uint32_t addr[8], uint32_t in_addr[8]);
extern void copy_subtree_addr_jazz(uint32_t out_addr[8], uint32_t in_addr[8]);
extern void set_keypair_addr_jazz(uint32_t addr[8], uint32_t type);
extern void set_layer_addr_jazz(uint32_t addr[8], uint32_t layer);
extern void set_tree_addr_jazz(uint32_t addr[8], uint64_t tree);
extern void set_type_jazz(uint32_t addr[8], uint32_t type);

// merkle [FIXME: Fails]
extern void merkle_sign_jazz(uint8_t *sig, uint8_t *root, const spx_ctx *ctx, uint32_t wots_addr[8],
                             uint32_t tree_addr[8], uint32_t idx_leaf);
extern void merkle_gen_root_jazz(uint8_t *root, const uint8_t *pub_seed, const uint8_t *sk_seed);

// fors
extern void fors_pk_from_sig_jazz(uint8_t *pk, const uint8_t *sig, const uint8_t *m,
                                  const uint8_t *pub_seed, const uint32_t fors_addr[8]);
extern void fors_sign_jazz(uint8_t *sig, uint8_t *pk, const uint8_t *m, const uint8_t *pub_seed,
                           const uint8_t *sk_seed, const uint32_t fors_addr[8]);

// hash
typedef struct {
    uint8_t R[SPX_N];
    uint8_t pk[SPX_PK_BYTES];
} args;

extern uint32_t hash_message_jazz(uint8_t *digest, uint64_t *tree, const uint32_t *leaf_idx,
                                  const args *_args, const uint8_t *msg, size_t msg_len);

extern void gen_message_random_jazz(uint8_t *R, const uint8_t *sk_prf, const uint8_t *optrand,
                                    const uint8_t *msg, size_t msg_len);

// Thash
extern void thash_35(
    uint8_t *out, const uint8_t *in, const uint8_t *pub_seed,
    uint32_t addr[8]);  // 35 is the value of INBLOCKS for sphincs shake 128f simple

// Wots
extern void wots_pk_from_sig_jazz(uint8_t *pk, const uint8_t *sig, const uint8_t *msg,
                                  const spx_ctx *ctx, uint32_t addr[8]);

// Wrapper
static void hash_message_jazz_(uint8_t *digest, uint64_t *tree, uint32_t *leaf_idx,
                               const uint8_t *R, const uint64_t *pk, const uint8_t *m,
                               size_t mlen) {
    args _args;
    memcpy(_args.R, R, SPX_N);
    memcpy(_args.pk, pk, SPX_PK_BYTES);
    hash_message_jazz(digest, tree, leaf_idx, &_args, m, mlen);
}

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

    /* Compute root node of the top-most subtree. */
    merkle_gen_root(sk + 3 * SPX_N, &ctx);
    // FIXME: // FIXME: // FIXME: // FIXME: Merkle gen root is wrong
    // merkle_gen_root_jazz(sk + 3*SPX_N, ctx.pub_seed, ctx.sk_seed);

    memcpy(pk + SPX_N, sk + 3 * SPX_N, SPX_N);

    return 0;
}

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

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen,
                          const uint8_t *sk) {
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

    set_type_jazz(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type_jazz(tree_addr, SPX_ADDR_TYPE_HASHTREE);

    randombytes(optrand, SPX_N);

    // gen_message_random(sig, sk_prf, optrand, m, mlen, &ctx);
    gen_message_random_jazz(sig, sk_prf, optrand, m, mlen);

    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    sig += SPX_N;

    set_tree_addr_jazz(wots_addr, tree);
    set_keypair_addr_jazz(wots_addr, idx_leaf);

    /* Sign the message hash using FORS. */
    fors_sign_jazz(sig, root, mhash, ctx.pub_seed, ctx.sk_seed, wots_addr);
    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr_jazz(tree_addr, i);
        set_tree_addr_jazz(tree_addr, tree);

        copy_subtree_addr_jazz(wots_addr, tree_addr);
        set_keypair_addr_jazz(wots_addr, idx_leaf);

        merkle_sign(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);
        // FIXME:
        // merkle_sign_jazz(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);
        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        /* Update the indices for the next layer. */
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT) - 1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    *siglen = SPX_BYTES;

    return 0;
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen,
                       const uint8_t *pk) {
    spx_ctx ctx;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];
    unsigned char root[SPX_N];
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

    set_type_jazz(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type_jazz(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type_jazz(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    // hash_message_jazz_(mhash, &tree, &idx_leaf, sig, pk, m, mlen); // FIXME: This fails

    sig += SPX_N;

    /* Layer correctly defaults to 0, so no need to set_layer_addr_jazz */
    set_tree_addr_jazz(wots_addr, tree);
    set_keypair_addr_jazz(wots_addr, idx_leaf);

    fors_pk_from_sig_jazz(root, sig, mhash, ctx.pub_seed, wots_addr);
    sig += SPX_FORS_BYTES;

    /* For each subtree.. */
    for (i = 0; i < SPX_D; i++) {
        set_layer_addr_jazz(tree_addr, i);
        set_tree_addr_jazz(tree_addr, tree);

        copy_subtree_addr_jazz(wots_addr, tree_addr);
        set_keypair_addr_jazz(wots_addr, idx_leaf);

        copy_keypair_addr_jazz(wots_pk_addr, wots_addr);

        /* The WOTS public key is only correct if the signature was correct. */
        /* Initially, root is the FORS pk, but on subsequent iterations it is
           the root of the subtree below the currently processed subtree. */
        wots_pk_from_sig(wots_pk, sig, root, &ctx, wots_addr);
        // wots_pk_from_sig_jazz(wots_pk, sig, root, &ctx, wots_addr);
        sig += SPX_WOTS_BYTES;

        /* Compute the leaf node using the WOTS public key. */
        // thash(leaf, wots_pk, SPX_WOTS_LEN, &ctx, wots_pk_addr);
        thash_35(leaf, wots_pk, ctx.pub_seed, wots_pk_addr);

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

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m,
                unsigned long long mlen, const unsigned char *sk) {
    size_t siglen;

    crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

    memmove(sm + SPX_BYTES, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm,
                     unsigned long long smlen, const unsigned char *pk) {
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
