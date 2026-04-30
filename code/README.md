# C Implementation

This directory contains the C implementation used in the thesis work on historical knapsack cryptosystems.

## Structure

- `src/` - application flow, CLI, benchmarks, plaintext helpers, and scheme implementations
- `include/` - public headers
- `Makefile` - build configuration

## Build

Build a debug binary:

```bash
make debug
```

Build a release binary:

```bash
make release
```

Build with sanitizers:

```bash
make san
```

## Run

Interactive plaintext demo:

```bash
./build/debug/knapsack demo --scheme mh-classic --seed 123
```

Raw bitstring demo:

```bash
./build/debug/knapsack demo --scheme mh-classic --bits 101010 --seed 123
```

Plaintext demo:

```bash
./build/debug/knapsack demo --scheme mh-classic --msg "hello" --seed 123
```

Plaintext demo with explicit block size:

```bash
./build/debug/knapsack demo --scheme mh-classic --msg "hello" --n 64 --seed 123
```

Benchmark example:

```bash
./build/debug/knapsack bench --scheme mh-classic --n 128 --reps 10 --seed 123 --format csv
```

Benchmark with an explicit bitstring:

```bash
./build/debug/knapsack bench --scheme mh-classic --bits 101010 --reps 10 --seed 123 --format csv
```

## CLI summary

Demo mode accepts:

- `--bits BITS` - raw bitstring input
- `--msg TEXT` - plaintext input converted to bits
- `--n BITS` - demo block size in bits; plaintext input defaults to 128-bit blocks
- `--scheme ID` - `mh-classic`, `mh`, or `mh-permuted`
- `--seed SEED` - deterministic seed

Bench mode accepts:

- `--bits BITS` - explicit raw bitstring input
- `--n BITS` - random message length in bits
- `--reps N` - repetitions; default is 10
- `--format csv` - CSV output
- `--scheme ID` - `mh-classic`, `mh`, or `mh-permuted`
- `--seed SEED` - deterministic seed

## Current limitations

- Demo output is still bit-oriented.
- For multi-block demo messages, the printed ciphertext currently represents the last processed block only.
- Block-aware and text-aware demo output is planned for a later refactor.

## Notes

- This code is for historical and educational exploration.
- The implemented Merkle-Hellman variants are not secure for real-world use.
