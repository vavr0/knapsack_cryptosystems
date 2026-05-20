#pragma once
#include "cli.h"
#include "error.h"

/*
Run CSV benchmark for the
selected scheme and input size.
*/
KnapStatus bench_run(CliFlags *flags);
