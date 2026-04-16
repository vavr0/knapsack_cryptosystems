# OpenCode Project Plan

## Architecture Rules

- Keep the current small-framework architecture:
  - `main.c` tiny entrypoint
  - `app.c` top-level demo/user-facing flow
  - `cli.c` centralized parsing
  - `bench.c` benchmark-only path
  - `scheme_*.c` crypto-only implementations
- Do not add a generic orchestration/runtime layer.
- Keep changes additive and practical.
- Prefer the smallest correct change.
- Do not build framework-first abstractions.
- Keep attacks scheme-specific.
- Keep variants in separate files.
- Extract only truly shared MH low-level helpers.

## Current Direction

- The randomness/seed refactor is done enough for now.
- `mh-classic` is the baseline.
- `mh-permuted` exists but should be refactored.
- `mh-iterated` is the more meaningful next variant.
- Attacks should come after the baseline/variant work is clean enough.

## Demo Plaintext Feature Plan

### Scope

- Add plaintext support to `demo` mode only.
- Leave `bench` unchanged.
- Keep scheme layer block-based and unchanged.

### CLI Semantics

- `--bits` = raw bitstring input
- `--msg` = plaintext message input
- Reject using both `--bits` and `--msg`.
- In plaintext mode:
  - require `--n`
  - require `n % 8 == 0`
- In bit mode:
  - reject `--n`

### Interactive Demo

- If neither `--bits` nor `--msg` is given:
  1. ask whether input mode is `bits` or `text`
  2. if `bits`, prompt for bitstring
  3. if `text`, prompt for plaintext and then block size `n`

### Plaintext Processing
- Convert plaintext bytes to bits.
- Split into `n`-bit blocks.
- Zero-pad the final block.
- Keygen once using block size `n`.
- Encrypt each block.
- Decrypt each block.
- Reassemble decrypted bits into bytes.
- Trim back to the original plaintext length.

### Likely Code Changes

- `include/cli.h`
  - add plaintext field while keeping `BitBuf` for raw bits
- `src/cli.c`
  - parse `--bits`
  - parse `--msg`
  - validate mode constraints
- `include/bitvec.h` / `src/bitvec.c`
  - add helpers for bytes/string <-> bits
- `src/app.c`
  - add plaintext demo path
  - add block loop encrypt/decrypt
  - print one ciphertext block per line for plaintext mode

## `mh-permuted` Refactor Plan

### Current Problems

- `src/scheme_mh_permuted.c` duplicates too much of classic MH.
- The permutation logic is harder to reason about than it should be.
- The file feels like a fork instead of a small variant.

### Refactor Goal

- Keep separate files:
  - `src/scheme_mh.c`
  - `src/scheme_mh_permuted.c`
  - later `src/scheme_mh_iterated.c`
- Extract only shared low-level MH helpers.
- Do not merge all variants into one big file.
- Do not build a generic variant framework.

### Shared Helper Module

Create one MH-private shared helper area reusable by at least two variants.

Good candidates:
- superincreasing sequence generation
- modulus selection above sequence sum
- invertible multiplier selection and modular inverse computation
- subset-sum encryption over public weights
- greedy recovery over superincreasing private weights
- seeded permutation generation

Possible shape:
- `src/scheme_mh_common.c`
- private header for MH-common helpers

### Permutation Simplification

Use one canonical mapping only.

Preferred meaning:
- `perm[i] = private index used by public/message slot i`

Then:
- public construction uses `priv_weights[perm[i]]`
- encryption uses public slot `i`
- decryption recovers bits in private order
- final output maps `message[i] = priv_bits[perm[i]]`

## `mh-iterated` Direction

- Keep `mh-iterated` in its own file.
- It is the more meaningful next variant than permutation alone.
- Prefer a historically grounded iterated Merkle-Hellman construction.
- Start with a fixed small number of layers, likely 2.
- Avoid configurable layer count at first.
- Prefer repeated modular disguise layers first.
- Do not add extra complexity unless justified by the source material.

## Attack Direction

### Principles

- Do not build a generic attack API first.
- Attacks should be concrete and scheme-specific.
- First attacks should target classic MH.

### Likely Attack Order

1. brute force
2. meet-in-the-middle
3. Shamir

### Practical Constraint

- Even while the current runtime scheme key still stores both public and private data internally, attack code should conceptually use only the public-side MH data it needs.

### Likely Future Files

- `src/attack_mh_bruteforce.c`
- `src/attack_mh_mitm.c`
- `src/attack_mh_shamir.c`

## Branch / Repo Cleanup

### Git
- Create a development branch before the next implementation pass.
- Keep `main` cleaner and use the dev branch for refactors and doc cleanup.

### Markdown Cleanup

Current markdown files:
- root `README.md`
- `code/research.md`
- `code/summary.md`
- `code/random.md`

Likely cleanup direction:
- keep `README.md` as the main entrypoint
- keep `research.md` if still useful as thesis/reference material
- fold or move `summary.md` and `random.md` into a cleaner docs/notes area if they remain useful
- remove stale planning files only after important context is preserved elsewhere

### README Improvement Plan

The root `README.md` should include:
1. project overview
2. current implemented schemes/features
3. architecture summary
4. build instructions
5. demo usage examples
6. benchmark usage examples
7. clangd / LSP setup note:
   - `compile_commands.json` is machine-generated
   - generate with `bear -- make debug`
8. short note that the repo is educational / historical, not modern secure cryptography

## Constraints To Keep

- Keep changes additive.
- Prefer the smallest correct change.
- No framework-first abstraction.
- No generic attack abstraction before concrete attacks exist.
- No major architecture redesign unless clearly justified.
