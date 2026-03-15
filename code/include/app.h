#pragma once

#include "cli.h"
#include "error.h"

KnapStatus app_run(int argc, char **argv);
KnapStatus demo_run(CliFlags *flags);
void print_usage(const char *prog);

