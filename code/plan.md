# Benchmark Rewrite Plan (Thesis-Clean)

## Goal

- Make `bench` measure only the real cryptosystem pipeline via the unified API
(`knapsack_run_once`), with reproducible and meaningful timings.

## Why Rewrite

Current `compare` path is not aligned with the architecture:

- It bypasses the unified API.
- It does not use the GMP/real cryptosystem path.
- It produces near-instant timings that are not useful for thesis results.

## Scope

### 1) Redefine Benchmark Contract

- `bench` should benchmark only the actual cryptosystem pipeline.
- All benchmarked work must flow through `knapsack_run_once(...)`.

### 2) Remove Misleading Compare Mode

- Remove `--kind compare` from CLI and app dispatch.
- Delete `bench_run_compare_csv` and toy subset-sum compare code from `bench.c`.
- Keep a single production benchmark path.

### 3) Ensure Reproducibility Across macOS/Linux

- Replace dependence on libc `rand()` for benchmark data generation.
- Add deterministic internal RNG (seeded by `--seed`) so same seed => same messages on both OSes.

### 4) Improve Timing Quality

- Keep per-phase timings: keygen/encrypt/decrypt/total.
- Add warmup runs (small default).
- Add optional batching for tiny `n` to avoid timer-granularity noise.

### 5) Prepare for Future Scheme Comparison

- Add `--scheme` argument now (default `mh-classic`).
- Include `scheme` column in CSV output.
- Later, adding a second scheme should require minimal changes.

### 6) Clean Output Format

- Standardize CSV columns, for example:
  - `scheme,n,reps,seed,warmup,keygen_ms,encrypt_ms,decrypt_ms,total_ms`
- Keep seed visible in CSV and stderr for traceability.

### 7) Separate Theory Illustration from Production Bench

- If needed, keep subset-sum (general vs superincreasing) as a separate `experiments` tool.
- Do not mix it into `bench`.


## Expected File Touch List

- `code/include/bench.h`
- `code/src/bench.c`
- `code/include/cli.h`
- `code/src/cli.c`
- `code/src/app.c`
- `code/run.sh`


## Execution Strategy (Recommended)

### Commit 1: Contract Cleanup

- Remove `compare` path and all non-API benchmark logic.
- Keep one benchmark mode working end-to-end through unified API.

### Commit 2: Quality + Reproducibility

- Add deterministic RNG.
- Add warmup/batching options.
- Add `--scheme` and update CSV schema.


## Done Criteria

- `bench` only uses unified API path.
- No toy solver timing in production benchmark.
- Same `--seed` gives repeatable benchmark inputs across OSes.
- CSV output includes enough metadata to reproduce plots/tables in thesis.
- Build and run succeed on both macOS and Linux.
