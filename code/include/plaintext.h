#pragma once

#include "buffer.h"
#include "common.h"
#include "error.h"

typedef struct {
    BitBuf bits;
    u64 block_size;
    u64 block_count;
    u64 used_bits;
} BitBlocks;

BitBlocks bit_blocks(void);
void bit_blocks_clear(BitBlocks *blocks);

KnapStatus blocks_from_bits(BitBlocks *out, BitView bits, u64 block_size);
BitView get_bit_block(const BitBlocks *blocks, u64 index);

KnapStatus bit_buf_from_text(BitBuf *out, const TextBuf *text);
KnapStatus text_buf_from_bits(TextBuf *out, BitView bits);
