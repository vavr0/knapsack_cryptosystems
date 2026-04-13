#include "app.h"
#include <stdint.h>

int main(int argc, char **argv) {
    if (argc < 1 || !argv) {
        return 1;
    }
    KnapStatus status = app_run(argc, argv);
    if (status != KNAP_OK) {
        return 1;
    }
    return 0;
}
