#include "merkle.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "address.h"
#include "context.h"
#include "params.h"
#include "utils.h"
#include "utilsx1.h"
#include "wots.h"
#include "wotsx1.h"

#ifdef TEST_WOTS
extern void chain_lengths_jazz(uint32_t *, const uint8_t *);
#endif

#ifdef TEST_WOTS_TREEHASH

extern void treehash_wots_jazz(const void *);

void treehashx1_wots_jasmin(unsigned char *root, const spx_ctx *ctx, uint32_t leaf_idx, uint32_t tree_addr[8],
                            void *info);

void treehashx1_wots_jasmin(unsigned char *root, const spx_ctx *ctx, uint32_t leaf_idx, uint32_t tree_addr[8],
                            void *info) {
    // We remove the idx_offset parameter because it is always zero
    // We remove the tree_height parameter because it is always SPX_TREE_HEIGHT
    // These were also removed from the ref impl

    void *args[10];

    args[0] = (void *)root;
    args[1] = (void *)ctx->pub_seed;
    args[2] = (void *)ctx->sk_seed;
    args[3] = (void *)&leaf_idx;
    args[4] = (void *)tree_addr;
    args[5] = (void *)((struct leaf_info_x1 *)info)->wots_sig;
    args[6] = (void *)&((struct leaf_info_x1 *)info)->wots_sign_leaf;
    args[7] = (void *)((struct leaf_info_x1 *)info)->wots_steps;
    args[8] = (void *)((struct leaf_info_x1 *)info)->leaf_addr;
    args[9] = (void *)((struct leaf_info_x1 *)info)->pk_addr;

    treehash_wots_jazz(args);
}
#endif

#ifdef TEST_MERKLE
extern void merkle_sign_jazz(uint8_t *, unsigned char *, const spx_ctx *, uint32_t *, uint32_t *, uint32_t);
#endif

/*
 * This generates a Merkle signature (WOTS signature followed by the Merkle
 * authentication path).  This is in this file because most of the complexity
 * is involved with the WOTS signature; the Merkle authentication path logic
 * is mostly hidden in treehashx4
 */
void merkle_sign(uint8_t *sig, unsigned char *root, const spx_ctx *ctx, uint32_t wots_addr[8], uint32_t tree_addr[8],
                 uint32_t idx_leaf) {
    unsigned char *auth_path = sig + SPX_WOTS_BYTES;
    struct leaf_info_x1 info = {0};
    unsigned steps[SPX_WOTS_LEN];

    info.wots_sig = sig;

#ifndef TEST_WOTS
    chain_lengths(steps, root);
#else
    chain_lengths_jazz(steps, root);
#endif

    info.wots_steps = steps;

    set_type(&tree_addr[0], SPX_ADDR_TYPE_HASHTREE);
    set_type(&info.pk_addr[0], SPX_ADDR_TYPE_WOTSPK);
    copy_subtree_addr(&info.leaf_addr[0], wots_addr);
    copy_subtree_addr(&info.pk_addr[0], wots_addr);

    info.wots_sign_leaf = idx_leaf;

#ifndef TEST_WOTS_TREEHASH
    treehashx1(root, auth_path, ctx, idx_leaf, 0, SPX_TREE_HEIGHT, wots_gen_leafx1, tree_addr, &info);
#else
    (void)auth_path;  // suppress unused variable warning
    treehashx1_wots_jasmin(root, ctx, idx_leaf, tree_addr, &info);
#endif
}



/* Compute root node of the top-most subtree. */
void merkle_gen_root(unsigned char *root, const spx_ctx *ctx) {
    /* We do not need the auth path in key generation, but it simplifies the
       code to have just one treehash routine that computes both root and path
       in one function. */
    unsigned char auth_path[SPX_TREE_HEIGHT * SPX_N + SPX_WOTS_BYTES];
    uint32_t top_tree_addr[8] = {0};
    uint32_t wots_addr[8] = {0};

    set_layer_addr(top_tree_addr, SPX_D - 1);
    set_layer_addr(wots_addr, SPX_D - 1);

#ifndef TEST_MERKLE
    merkle_sign(auth_path, root, ctx, wots_addr, top_tree_addr,
                (uint32_t)~0 /* ~0 means "don't bother generating an auth path */);
#else
    merkle_sign_jazz(auth_path, root, ctx, wots_addr, top_tree_addr,
                     (uint32_t)~0 /* ~0 means "don't bother generating an auth path */);
#endif
}
