#pragma once
#include "common.h"
#include "error.h"

// Non-owning view of bits 
// stored as 0/1 bytes.
typedef struct {
    const u8 *data;
    u64 length;
} BitView;

// Owning bit buffer 
// storing bits as 0/1 
// bytes.
typedef struct {
    u8 *data;
    u64 length;
} BitBuf;

// Owning UTF-8/plain 
// byte text buffer.
typedef struct {
    char *data;
    u64 length;
} TextBuf;

BitView bit_view(void);
BitBuf bit_buf(void);
TextBuf text_buf(void);

void bit_buf_clear(BitBuf *buf);

b8 bit_view_is_valid(BitView view);
b8 bit_buf_is_valid(const BitBuf *buf);
b8 bit_view_equal(BitView a, BitView b);
b8 bit_buf_equal(const BitBuf *a, const BitBuf *b);
KnapStatus bit_buf_alloc(BitBuf *buf, size_t len);
KnapStatus bit_buf_copy(BitBuf *dst, BitView src);
// Parse a string containing only '0' and '1'.
KnapStatus bit_buf_from_cstr(BitBuf *out, const char *s);

// Format bits as a newly allocated '0'/'1' C string.
KnapStatus bit_view_to_cstr(BitView view, char **out_str);
KnapStatus bit_buf_to_cstr(const BitBuf *buf, char **out_str);
BitView bit_buf_view(const BitBuf *buf);

void text_buf_clear(TextBuf *buf);

// Copy a non-empty C string into a text buffer.
KnapStatus text_buf_from_cstr(TextBuf *out, const char *s);
