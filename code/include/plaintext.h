#pragma once

#include "buffer.h"
#include "common.h"
#include "error.h"

// Bit buffer split into fixed-size encryption blocks.
typedef struct {
    BitBuf bits;
    u64 block_size;
    u64 block_count;
    u64 used_bits;
} BitBlocks;

BitBlocks bit_blocks(void);
void bit_blocks_clear(BitBlocks *blocks);

// Pad and split a bit view into fixed-size blocks.
KnapStatus blocks_from_bits(BitBlocks *out, BitView bits, u64 block_size);

// Return one block view from a block collection.
BitView get_bit_block(const BitBlocks *blocks, u64 index);

// Convert text bytes to bits, most significant bit first.
KnapStatus bit_buf_from_text(BitBuf *out, const TextBuf *text);

// Convert a bit view back to text bytes.
KnapStatus text_buf_from_bits(TextBuf *out, BitView bits);
