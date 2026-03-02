#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef i32 b32;

typedef struct {
    i32 *data;
    size_t len;
    size_t cap;
} I32Array;

typedef struct {
    const i32 *data;
    size_t len;
} I32View;

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} StrBuf;

typedef struct {
    const char *data;
    size_t len;
} StrSlice;

void i32_array_init(I32Array *a);
void i32_array_free(I32Array *a);
b8 i32_array_is_valid(const I32Array *a);
i32 i32_array_reserve(I32Array *a, size_t new_cap);
i32 i32_array_resize(I32Array *a, size_t new_len);
i32 i32_array_get(const I32Array *a, size_t idx, i32 *out_value);
i32 i32_array_set(I32Array *a, size_t idx, i32 value);
i32 i32_array_push(I32Array *a, i32 value);
I32View i32_array_view(const I32Array *a);

void str_buf_init(StrBuf *s);
void str_buf_free(StrBuf *s);
b8 str_buf_is_valid(const StrBuf *s);
i32 str_buf_reserve(StrBuf *s, size_t new_cap);
i32 str_buf_assign_cstr(StrBuf *s, const char *src);
i32 str_buf_append_cstr(StrBuf *s, const char *suffix);
i32 str_buf_get(const StrBuf *s, size_t idx, char *out_ch);
i32 str_buf_set(StrBuf *s, size_t idx, char ch);
const char *str_buf_cstr(const StrBuf *s);
StrSlice str_buf_slice(const StrBuf *s);

b8 str_slice_is_valid(StrSlice s);
i32 str_slice_from_cstr(const char *src, StrSlice *out_slice);
i32 str_slice_subslice(StrSlice s, size_t start, size_t len,
                       StrSlice *out_slice);
b8 str_slice_eq_cstr(StrSlice s, const char *rhs);

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
