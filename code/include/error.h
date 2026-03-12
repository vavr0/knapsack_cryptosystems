#pragma once

typedef enum {
    KNAP_OK = 0,
    KNAP_STATUS_HELP,
    KNAP_ERR_INVALID,
    KNAP_ERR_ALLOC,
    KNAP_ERR_CRYPTO,
    KNAP_ERR_INTERNAL,
} KnapStatus;

const char *knap_status_str(KnapStatus status);
