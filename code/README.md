# C Implementation

This directory contains the C implementation used in the thesis work on historical knapsack cryptosystems.

## Structure

- `src/` - application flow, CLI, benchmarks, plaintext helpers, attacks, and scheme implementations
- `include/` - public headers
- `tests/` - small CLI checks
- `experiments/` - Python experiment runners, CSV outputs, and plots
- `Makefile` - build configuration

## Dependencies

- C compiler with C11 support
- GMP
- Python 3 with `pandas` and `matplotlib` for experiment plots

On macOS with Homebrew:

```bash
brew install gmp
```

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

Bench CSV includes timing columns and key-structure columns:

```text
keygen_ms,encrypt_ms,decrypt_ms,total_ms,delta_max,margin_bound,margin,sum_bits,density
```

Attack mode accepts:

- `--attack ID` - `brute` or `mitm`
- `--bits BITS` - explicit raw bitstring input
- `--n BITS` - random message length in bits
- `--scheme ID` - currently `mh-classic` or `mh`
- `--seed SEED` - deterministic seed

Attack output is CSV. The implemented attacks are generic subset-sum attacks against the public knapsack instance, not full Shamir key recovery.

## Experiments

Set up the Python environment:

```bash
python3 -m venv experiments/venv
./experiments/venv/bin/python3 -m pip install -r experiments/requirements.txt
```

Generate standard benchmark and attack data:

```bash
make release
./experiments/venv/bin/python3 experiments/run_experiments.py
```

Generate key-generation parameter sweeps:

```bash
./experiments/venv/bin/python3 experiments/run_param_sweep.py
```

Generate iterated-layer sweep data:

```bash
./experiments/venv/bin/python3 experiments/run_iterated_sweep.py
```

Generate plots from available CSV files:

```bash
./experiments/venv/bin/python3 experiments/plot_results.py
```

Experiment scripts write CSV files to `experiments/results/` and plots to `experiments/plots/`. Sweep scripts temporarily change compile-time constants, rebuild the binary, validate the reported parameters, and restore the source afterward.

## Implementation notes

- Default key-generation constants are `MH_DEFAULT_DELTA_MAX = 64` and `MH_DEFAULT_MARGIN_FACTOR = 64`.
- These are compact educational parameters, not secure deployment parameters.
- The multiplier is sampled over the full modulus range and must be invertible modulo the modulus.
- This code is for historical and educational exploration.
- The implemented Merkle-Hellman variants are not secure for real-world use.
