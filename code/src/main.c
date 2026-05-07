#include "app.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc < 1 || !argv) {
        return 1;
    }
    KnapStatus status = app_run(argc, argv);
    if (status != KNAP_OK) {
        fprintf(stderr, "error: %s\n", knap_status_str(status));

        return 1;
    }
    return 0;
}
