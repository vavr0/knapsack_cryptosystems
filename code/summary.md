# Last Two Weeks Summary


## Main Work Completed

- Fixed randomness and seed semantics, which was the main remaining correctness gap.
- Replaced libc `srand`/`rand` style usage with explicit local PRNG state.
- Added deterministic seeded behavior and entropy-backed default behavior.
- Changed MH key generation from effectively fixed construction to true seeded randomized construction.
- Tightened decrypt correctness by rejecting non-zero remainder after greedy recovery.
- Removed `show_steps` and scheme-layer printing so crypto code stays crypto-only.

## RNG / Seed Work

- Added explicit-state PRNG in `rand.*`:
  - `PrngState`
  - `prng_seed(...)`
  - `prng_rand(...)`
  - `prng_rand_u64(...)`
- Added `seed.*` for:
  - OS entropy collection
  - expanding one user `--seed` into `initstate` and `initseq`
  - resolving final seed pairs
- Widened seed handling to `u64`.
- `--seed` now actually means deterministic reproducible runs.

## CLI / Wiring Changes

- `cli.c` now parses seeds as `u64`.
- CLI scheme validation was cleaned up to use `scheme_resolve(...)` instead of duplicated string lists.
- `app.c` and `bench.c` now resolve seed pairs and pass them into `SchemeKeygenParams`.
- `SchemeKeygenParams` now uses:
  - `initstate`
  - `initseq`

## MH Classic Improvements

- `scheme_mh.c` now seeds a local PRNG in keygen.
- Private weights are generated as a seeded superincreasing sequence.
- Modulus is chosen as `sum + margin`.
- Multiplier is sampled until it is invertible mod the modulus.
- Decrypt now returns crypto failure if greedy recovery leaves remainder.

## Testing Done

- Same seed + same message gives identical ciphertext repeatedly.
- Different seeds give different ciphertext while still decrypting correctly.
- Running without `--seed` gives different outputs across runs.
- Benchmarks were tested across multiple `n` values up to large sizes.
- A large 1024-bit demo worked correctly with fixed seed.

## Work Added

- Created a separate feature branch for `mh-permuted` development.
- Added `scheme_mh_permuted.c` as a separate scheme implementation.
- Wired it into the existing scheme resolver.
- Fixed the initial permutation logic bug so it now works consistently.
- Verified:
  - same seed repeats ciphertext
  - different seed changes ciphertext
  - decryption matches original plaintext

## Current State

- `mh-classic` is stable enough for demos and benchmarks.
- `mh-permuted` now works as a separate additive extension.
- Architecture remains simple and unchanged.
- The next likely milestone is:
  - `mh-iterated`
  - then the first concrete attack

## Commands To Show

Build debug binary:

```bash
make debug
```

Classic deterministic demo:

```bash
./build/debug/knapsack demo --scheme mh-classic --msg 101010 --seed 123
```

Permuted deterministic demo:

```bash
./build/debug/knapsack demo --scheme mh-permuted --msg 101010 --seed 123
```

Run the same command twice to show identical ciphertext.

Different seed changes ciphertext:

```bash
./build/debug/knapsack demo --scheme mh-permuted --msg 101010 --seed 124
```

Benchmark example:

```bash
./build/debug/knapsack bench --scheme mh-classic --n 128 --reps 10 --seed 123 --format csv
```
