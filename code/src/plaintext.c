#include "plaintext.h"
#include "buffer.h"
#include "common.h"
#include "error.h"
#include <string.h>

BitBlocks bit_blocks(void) { return (BitBlocks){0}; }

void bit_blocks_clear(BitBlocks *blocks) {
    if (!blocks) {

        return;
    }
    bit_buf_clear(&blocks->bits);
    *blocks = bit_blocks();
}

KnapStatus blocks_from_bits(BitBlocks *out, BitView bits, u64 block_size) {
    KnapStatus status;
    u64 block_count;
    u64 size;
    if (!out || block_size == 0 || !bit_view_is_valid(bits) || bits.length == 0) {
        return KNAP_ERR_INVALID;
    }

    block_count = bits.length / block_size;
    if ((bits.length % block_size) != 0) {
        block_count++;
    }

    if (block_count == 0 || block_count > (u64)SIZE_MAX / block_size) {

        return KNAP_ERR_ALLOC;
    }

    size = block_count * block_size;

    bit_blocks_clear(out);
    status = bit_buf_alloc(&out->bits, (size_t)size);
    if (status != KNAP_OK) {

        return status;
    }

    memcpy(out->bits.data, bits.data, (size_t)bits.length);
    out->block_size = block_size;
    out->block_count = block_count;
    out->used_bits = bits.length;

    return KNAP_OK;
}

BitView get_bit_block(const BitBlocks *blocks, u64 index) {
    if (!blocks || index >= blocks->block_count || blocks->block_size == 0) {
        return bit_view();
    }

    u64 i = index * blocks->block_size;

    return (BitView){.data = blocks->bits.data + i,
                     .length = blocks->block_size};
}

KnapStatus bit_buf_from_text(BitBuf *out, const TextBuf *text) {
    KnapStatus status;
    u64 bit_len;

    if (!out || !text || !text->data || text->length == 0) {
        return KNAP_ERR_INVALID;
    }
    if (text->length > (u64)SIZE_MAX / 8u) {
        return KNAP_ERR_ALLOC;
    }

    bit_len = text->length * 8u;

    status = bit_buf_alloc(out, (size_t)bit_len);
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < text->length; i++) {
        u8 byte = (u8)text->data[i];

        for (u64 bit = 0; bit < 8u; bit++) {
            out->data[i * 8u + bit] = (u8)((byte >> (7u - bit)) & 1u);
        }
    }

    return KNAP_OK;
}

KnapStatus text_buf_from_bits(TextBuf *out, BitView bits) {
    size_t byte_len;
    char *text;

    if (!out || !bit_view_is_valid(bits)) {
        return KNAP_ERR_INVALID;
    }
    if (bits.length == 0 || (bits.length % 8u) != 0) {
        return KNAP_ERR_INVALID;
    }

    byte_len = (size_t)(bits.length / 8u);

    text = malloc(byte_len + 1);
    if (!text) {
        return KNAP_ERR_ALLOC;
    }

    for (u64 i = 0; i < byte_len; i++) {
        u8 byte = 0;

        for (u64 bit = 0; bit < 8u; bit++) {
            byte = (u8)(byte << 1);
            byte = (u8)(byte | bits.data[i * 8u + bit]);
        }

        text[i] = (char)byte;
    }

    text[byte_len] = '\0';

    text_buf_clear(out);
    out->data = text;
    out->length = (u64)byte_len;

    return KNAP_OK;
}
