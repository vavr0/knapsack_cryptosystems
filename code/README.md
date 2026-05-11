# C Implementation

This directory contains the C implementation used in the thesis work on historical knapsack cryptosystems.

## Structure

- `src/` - application flow, CLI, benchmarks, plaintext helpers, attacks, and scheme implementations
- `include/` - public headers
- `tests/` - small CLI checks
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

Run basic checks:

```bash
./tests/test.sh
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

Benchmark example. Bench output is CSV:

```bash
./build/debug/knapsack bench --scheme mh-classic --n 128 --reps 10 --seed 123
```

Benchmark with an explicit bitstring:

```bash
./build/debug/knapsack bench --scheme mh-classic --bits 101010 --reps 10 --seed 123
```

Brute-force attack example:

```bash
./build/debug/knapsack attack --attack brute --n 12 --seed 123
```

Meet-in-the-middle attack example:

```bash
./build/debug/knapsack attack --attack mitm --n 32 --seed 123
```

## CLI summary

Demo mode accepts:

- `--bits BITS` - raw bitstring input
- `--msg TEXT` - plaintext input converted to bits
- `--n BITS` - demo block size in bits; plaintext input defaults to 128-bit blocks
- `--scheme ID` - `mh-classic`, `mh`, `mh-permuted`, or `mh-iterated`
- `--seed SEED` - deterministic seed

Bench mode accepts:

- `--bits BITS` - explicit raw bitstring input
- `--n BITS` - random message length in bits
- `--reps N` - repetitions; default is 10
- `--scheme ID` - `mh-classic`, `mh`, `mh-permuted`, or `mh-iterated`
- `--seed SEED` - deterministic seed

Attack mode accepts:

- `--attack ID` - `brute` or `mitm`
- `--bits BITS` - explicit raw bitstring input
- `--n BITS` - random message length in bits
- `--scheme ID` - currently `mh-classic` or `mh`
- `--seed SEED` - deterministic seed

Attack output is CSV. The implemented attacks are generic subset-sum attacks against the public knapsack instance, not full Shamir key recovery.

## Notes

- This code is for historical and educational exploration.
- The implemented Merkle-Hellman variants are not secure for real-world use.
