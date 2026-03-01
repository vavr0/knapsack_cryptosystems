#include "utils.h"

#include <string.h>

b8 bits_is_valid(const char *s) {
    if (!s || s[0] == '\0') {
        return 0;
    }

    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] != '0' && s[i] != '1') {
            return 0;
        }
    }

    return 1;
}

i32 bits_to_array(const char *s, i32 *out, size_t n) {
    if (!s || !out) {
        return -1;
    }

    if (strlen(s) != n) {
        return -1;
    }

    for (size_t i = 0; i < n; i++) {
        if (s[i] != '0' && s[i] != '1') {
            return -1;
        }
        out[i] = s[i] - '0';
    }

    return 0;
}
