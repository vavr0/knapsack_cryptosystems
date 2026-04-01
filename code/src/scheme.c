#include "scheme.h"
// TODO
#include <string.h>

const SchemeOps *scheme_resolve(const char *id) {
    if (!id || strcmp(id, "mh") == 0 || strcmp(id, "mh-classic") == 0) {
        return scheme_mh_get();
    }
    if (strcmp(id, "mh-permuted") == 0) {
        return scheme_mh_permuted_get();
    }
    return NULL;
}
