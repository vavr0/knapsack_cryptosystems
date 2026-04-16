# C Implementation

This directory contains the C implementation used in the thesis work on historical knapsack cryptosystems.

## Structure

- `src/` - application flow, CLI, benchmarks, and scheme implementations
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

Classic demo example:

```bash
./build/debug/knapsack demo --scheme mh-classic --msg 101010 --seed 123
```

Benchmark example:

```bash
./build/debug/knapsack bench --scheme mh-classic --n 128 --reps 10 --seed 123 --format csv
```

## Notes

- This code is for historical and educational exploration.
- The implemented Merkle-Hellman variants are not secure for real-world use.
