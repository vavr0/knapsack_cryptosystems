#include "bitvec.h"
#include "common.h"

// TODO check all these pls

BitView bit_view(void) { return (BitView){0}; }

BitBuf bit_buf(void) { return (BitBuf){0}; }

void bit_buf_clear(BitBuf *buf) {
    if (!buf) {
        return;
    }
    free(buf->data);
    *buf = bit_buf();
}

b8 bit_view_is_valid(BitView view) {
    if (view.length == 0) {
        return view.data == NULL;
    }
    if (!view.data) {
        return 0;
    }
    for (u64 i = 0; i < view.length; i++) {
        if (view.data[i] != 0 && view.data[i] != 1) {
            return 0;
        }
    }
    return 1;
}

b8 bit_buf_is_valid(const BitBuf *buf) {
    if (!buf) {
        return 0;
    }
    return bit_view_is_valid(
        (BitView){.data = buf->data, .length = buf->length});
}

b8 bit_view_equal(BitView a, BitView b) {
    if (a.length != b.length) {
        return 0;
    }
    if (a.length == 0) {
        return 1;
    }
    if (!bit_view_is_valid(a) || !bit_view_is_valid(b)) {
        return 0;
    }
    return (b8)(memcmp(a.data, b.data, (size_t)a.length) == 0);
}

b8 bit_buf_equal(const BitBuf *a, const BitBuf *b) {
    if (!a || !b) {
        return 0;
    }
    return bit_view_equal(bit_buf_view(a), bit_buf_view(b));
}

KnapStatus bit_buf_alloc(BitBuf *buf, size_t len) {
    if (!buf) {
        return KNAP_ERR_INVALID;
    }

    bit_buf_clear(buf);
    if (len == 0) {
        return KNAP_OK;
    }

    buf->data = malloc(len * sizeof(*buf->data));
    if (!buf->data) {
        return KNAP_ERR_ALLOC;
    }
    memset(buf->data, 0, len * sizeof(*buf->data));
    buf->length = (u64)len;
    return KNAP_OK;
}

KnapStatus bit_buf_copy(BitBuf *dst, BitView src) {
    if (!dst || !bit_view_is_valid(src)) {
        return KNAP_ERR_INVALID;
    }

    KnapStatus status = bit_buf_alloc(dst, (size_t)src.length);
    if (status != KNAP_OK) {
        return status;
    }

    if (src.length > 0) {
        memcpy(dst->data, src.data, (size_t)src.length);
    }
    return KNAP_OK;
}

KnapStatus bit_buf_from_cstr(BitBuf *out, const char *s) {
    if (!out || !s) {
        return KNAP_ERR_INVALID;
    }

    size_t len = strlen(s);
    if (len == 0) {
        return KNAP_ERR_INVALID;
    }

    KnapStatus status = bit_buf_alloc(out, len);
    if (status != KNAP_OK) {
        return status;
    }

    for (u64 i = 0; i < len; i++) {
        if (s[i] == '0') {
            out->data[i] = 0;
        } else if (s[i] == '1') {
            out->data[i] = 1;
        } else {
            bit_buf_clear(out);
            return KNAP_ERR_INVALID;
        }
    }

    return KNAP_OK;
}

KnapStatus bit_view_to_cstr(BitView view, char **out_str) {
    if (!out_str || !bit_view_is_valid(view)) {
        return KNAP_ERR_INVALID;
    }

    size_t len = (size_t)view.length;
    char *s = malloc(len + 1);
    if (!s) {
        return KNAP_ERR_ALLOC;
    }

    for (u64 i = 0; i < len; i++) {
        s[i] = view.data[i] ? '1' : '0';
    }
    s[len] = '\0';

    *out_str = s;
    return KNAP_OK;
}

KnapStatus bit_buf_to_cstr(const BitBuf *buf, char **out_str) {
    if (!buf) {
        return KNAP_ERR_INVALID;
    }
    return bit_view_to_cstr(bit_buf_view(buf), out_str);
}

BitView bit_buf_view(const BitBuf *buf) {
    if (!buf) {
        return bit_view();
    }
    return (BitView){.data = buf->data, .length = buf->length};
}
