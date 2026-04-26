#include "error.h"

const char *knap_status_str(KnapStatus status) {
    switch (status) {
    case KNAP_OK:
        return "KNAP_OK";
    case KNAP_STATUS_HELP:
        return "KNAP_STATUS_HELP";
    case KNAP_ERR_INVALID:
        return "KNAP_ERR_INVALID";
    case KNAP_ERR_ALLOC:
        return "KNAP_ERR_ALLOC";
    case KNAP_ERR_CRYPTO:
        return "KNAP_ERR_CRYPTO";
    case KNAP_ERR_INTERNAL:
        return "KNAP_ERR_INTERNAL";
    default:
        return "KNAP_ERR_INTERNAL";
    }
}

