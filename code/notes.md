# Meeting Notes

## Last Two Weeks - Main Progress

- Reworked the project architecture away from the old mixed `system` pipeline.
- Split responsibilities more clearly:
  - `app.c` now handles demo/user-facing flow
  - `bench.c` now handles benchmark-only execution
  - `scheme_*.c` stays crypto-only
  - `cli.c` is now the single parsing/validation entrypoint
- Kept `main.c` minimal so the program now enters through one top-level app runner.

## Contract / API Cleanup

- Stabilized the shared status/error flow around `KnapStatus`.
- Moved message handling onto the new minimal bit abstractions:
  - `BitView`
  - `BitBuf`
- Updated the scheme API to use typed message inputs/outputs instead of older ad hoc raw-array handling.
- Removed `has_seed` from scheme keygen params so the scheme layer now receives a concrete seed directly.

## CLI Refactor

- Finished the parser transition to one public function: `parse_args(...)`.
- Added checked numeric parsing for benchmark parameters such as `--n`, `--reps`, and `--seed`.
- Integrated direct parsing of `--msg` into `BitBuf`.
- Cleaned up old duplicated parser code and older demo/bench parsing leftovers.
- Added scheme id validation and lightweight scheme resolution support (`mh`, `mh-classic`).

## Demo / App Flow

- Rebuilt demo execution directly through the stable scheme API:
  - key generation
  - encryption
  - decryption
  - roundtrip verification
- Kept interactive bit input for demo mode when `--msg` is not supplied.
- Added cleaner demo output printing for plaintext / ciphertext / decrypted bits.

## Benchmarking Work

- Rebuilt the benchmark path around the same crypto core instead of the old shared orchestration layer.
- Added timing for separate phases:
  - keygen
  - encrypt
  - decrypt
  - total
- Added warmup reps and measured reps.
- Added CSV-style benchmark output that is already suitable for later plotting/comparison.
- Bench now reuses one fixed message for a whole benchmark run for fairer comparisons.

## Merkle-Hellman Implementation Work

- Updated the classic MH implementation to match the new scheme interface.
- Fixed the modular inverse/invertibility issue in the current implementation.
- Precomputed and stored the modular inverse in the MH key material so decrypt no longer recomputes it every run.
- Improved internal key initialization/error flow so key construction is safer than before.

## Why This Matters

- The code is now in a much better place for adding:
  - multiple scheme variants
  - alternative decryption strategies
  - attack modules
  - reproducible benchmark comparisons
- In other words, a lot of work went into making the codebase extensible and thesis-usable rather than just adding one more ad hoc demo.

## Current State

- The refactor is largely complete.
- Demo and benchmark paths both work through the shared crypto contract.
- The project is now ready to move from architecture cleanup into actual comparative work.

## Immediate Next Steps

- Add an alternative decrypt mode / implementation for comparison with the current greedy MH decrypt.
- Add at least one additional scheme variation through the new scheme resolution path.
- Start using the benchmark harness for comparison experiments across variants/settings.
- After that, connect the implementation work back into thesis text/results.

## Honest Gap

- Less progress was made on thesis writing itself than on code architecture.
- However, the recent work was mainly foundational: it reduces future rewrite cost and makes upcoming experimental/thesis work much more systematic.
