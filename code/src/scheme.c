#include "scheme.h"
#include <string.h>

const SchemeOps *scheme_resolve(const char *id) {
    if (!id || strcmp(id, "mh") == 0 || strcmp(id, "mh-classic") == 0) {
        return scheme_mh_get();
    }
    return NULL;
}
