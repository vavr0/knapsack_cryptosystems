#pragma once
#include "common.h"
#include "error.h"

typedef struct {
    const u8 *data;
    u64 length;
} BitView;

typedef struct {
    u8 *data;
    u64 length;
} BitBuf;

BitView bit_view(void);
BitBuf bit_buf(void);

void bit_buf_clear(BitBuf *buf);

// TODO
b8 bit_view_is_valid(BitView view);
b8 bit_buf_is_valid(const BitBuf *buf);
b8 bit_view_equal(BitView a, BitView b);
b8 bit_buf_equal(const BitBuf *a, const BitBuf *b);
KnapStatus bit_buf_alloc(BitBuf *buf, size_t len);
KnapStatus bit_buf_copy(BitBuf *dst, BitView src);
KnapStatus bit_buf_from_cstr(BitBuf *out, const char *s);
KnapStatus bit_view_to_cstr(BitView view, char **out_str);
KnapStatus bit_buf_to_cstr(const BitBuf *buf, char **out_str);
BitView bit_buf_view(const BitBuf *buf);
