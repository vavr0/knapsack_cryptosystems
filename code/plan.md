# Knapsack Thesis Code Plan

This plan is meant to keep the codebase stable while it grows from one
Merkle-Hellman implementation into a small framework for variants, attacks,
benchmarks, and thesis-ready results.

Main rule: freeze contracts first, then extend.

## Current Refactor Direction

This section is the current source of truth during the ongoing refactor. If
older parts of this file conflict with it, follow this section.

### Architecture

- Parsing is centralized in `cli`.
- `main.c` stays tiny and only calls `app_run(argc, argv)`.
- `app.c` is the top-level dispatcher and user-facing flow.
- `bench.c` is benchmark-only execution.
- `scheme_*.c` is crypto only.
- There is no intended public/shared `system` orchestration layer.

### Important Decision About `system`

The old `system` layer tried to serve both:
- demo orchestration and user-facing flow
- benchmark execution and timing

We decided this was too blurry.

Final direction:
- no public/shared `system.h/.c` architecture
- demo path is orchestrated in `app.c`
- bench path is orchestrated in `bench.c`
- both paths share the crypto core through `scheme.h`

Accepted tradeoff:
- a little duplicated orchestration between demo and bench is fine
- clarity is preferred over forcing both through one universal runner/output API

### What Is Being Removed

The following old `system` API is planned for removal, not redesign:

- `KnapsackRunRequest`
- `KnapsackRunOutput`
- `KnapsackRunMetrics`
- `knapsack_run_once(...)`
- `knapsack_run_output_clear(...)`

There is intentionally no 1:1 replacement shared runner API.

### What Replaces It

Instead of a shared runner layer:

- `app.c` directly does demo orchestration:
  - prepare/read message
  - call scheme keygen/encrypt/decrypt
  - print demo output
  - verify roundtrip correctness

- `bench.c` directly does benchmark orchestration:
  - choose defaults if needed
  - loop repetitions
  - time keygen/encrypt/decrypt
  - verify roundtrip correctness
  - print benchmark output

### CLI Direction

- `CliMode` only has:
  - `CLI_MODE_DEMO`
  - `CLI_MODE_BENCH`
- help is not a mode
- help is returned as `KNAP_STATUS_HELP`
- public parser surface is one function:
  - `parse_args(int argc, char **argv, CliFlags *out)`
- `CliFlags` is one combined CLI struct, not separate demo/bench structs

### Refactor Order

1. Finish `src/cli.c`
2. Complete `app_run(...)` as the real dispatcher
3. Remove old `parse_demo_options(...)` / `parse_bench_options(...)`
4. Rewrite demo path in `app.c`
5. Rewrite `bench.c` to `bench_run(const CliFlags *flags)`
6. Remove `system.h/.c` after no caller depends on them
7. Later continue with MH correctness, tests, and scheme registry

### Guidance For Future Agents

- Do not reintroduce `system` as a shared orchestration layer.
- Do not create a new universal run/output struct for both demo and bench.
- Shared code should live at the crypto contract level (`scheme.h`), not at the
  full app-flow level.
- If demo and bench duplicate a small amount of orchestration, that is
  expected.

### Current Code State

Good:
- `main.c` is already simplified to call `app_run(...)`.
- `app.h` matches the intended top-level architecture.
- `cli.h` is mostly aligned with the new combined `CliFlags` direction.
- `bench.h` already exposes `bench_run(const CliFlags *flags)`.
- `scheme.h` is much closer to the intended contract.

Still mid-refactor / inconsistent:
- `src/cli.c` has the new parser path, but still contains old
  `parse_demo_options(...)` / `parse_bench_options(...)`.
- `src/app.c` has the new `app_run(...)` skeleton, but still contains old
  parser-based `run_demo(...)` / `run_bench(...)` code.
- `src/app.c` still includes and uses `system.h`.
- `src/bench.c` still uses the old `system`-based flow.
- `system` has not yet been removed from implementation.
- `src/system.c` is stale relative to the new `scheme.h` direction and should
  not be treated as the target architecture.

Practical warning for future sessions:
- old `system` code still exists in the repo as refactor residue
- it is not the intended end state
- do not spend time redesigning or repairing it as a permanent shared layer

## 0) Core Principles

1. Public headers in `include/` are the contract.
2. Use one shared typed status system across modules.
3. Keep the status system small and stable:
   - `KNAP_OK`
   - `KNAP_STATUS_HELP`
   - `KNAP_ERR_INVALID`
   - `KNAP_ERR_ALLOC`
   - `KNAP_ERR_CRYPTO`
   - `KNAP_ERR_INTERNAL`
4. Make zero/empty output structs valid whenever possible.
5. Document ownership clearly: who allocates, who frees, who may reuse outputs.
6. Validate early at boundaries:
   - `cli` parses and validates input
   - `app` prints user-facing messages and builds requests
   - `system` runs one full pipeline
   - `scheme_*` implements crypto details
   - future `attack_*` implements attack details
7. No scheme-specific logic in `app`, `cli`, `system`, or `bench`.
8. No hidden defaults in lower layers.
9. Add tests whenever a public contract changes.
10. Reproducibility matters from the start (seed, params, build info, outputs).

---

## 1) Current Gaps To Fix First

These are the real blockers right now:

- Public APIs still use raw `0`, `1`, and `-1` instead of typed statuses.
- CLI numeric parsing is not fully checked (`atoi`, unchecked `strtoul`).
- Help and program-name behavior are inconsistent.
- `system` has unclear ownership/output lifecycle rules.
- `knapsack_run_once` can leak if output structs are reused.
- `system`, `app`, and `bench` still hardcode Merkle-Hellman behavior.
- The current scheme API is not future-proof for new variants.
- MH key generation does not guarantee invertibility for all valid `n`.
- There is no real test scaffold yet.
- Benchmark reproducibility is only partial.
- `plan.md` should no longer assume `common.h/common.c` container work, because
  the current `include/common.h` only contains primitive typedefs/macros.
- Bit messages are still passed mostly as raw arrays plus separate lengths;
  freeze a minimal shared bit representation before more API growth.

---

## 2) Main Work Blocks

Think about the project in these blocks:

1. Contract Freeze
2. MH Stabilization
3. Extensibility
4. Reproducibility
5. Thesis Outputs

---

## 3) Phase Plan

## Phase 1 - Freeze Shared Status and API Contracts

Goal: make all public interfaces explicit before adding new features.

Tasks:
- Add `include/error.h`.
- Add `include/bits.h`.
- Define one shared status enum:
  - `KNAP_OK`
  - `KNAP_STATUS_HELP`
  - `KNAP_ERR_INVALID`
  - `KNAP_ERR_ALLOC`
  - `KNAP_ERR_CRYPTO`
  - `KNAP_ERR_INTERNAL`
- Add status-to-string helper.
- Freeze minimal shared bit/message types:
  - `BitView` for read-only bit sequences with explicit length
  - `BitBuf` for owned or writable bit buffers with explicit length
- Add only the small helper surface needed to use these bit types safely.
- Keep this layer narrow and domain-specific; do not introduce a broad generic
  container/string foundation yet.
- Convert public APIs from raw integer returns to typed statuses.
- Document preconditions/postconditions in public headers.
- Document ownership/lifetime rules.

Acceptance criteria:
- No public function uses magic `-1` / `1` as its main contract.
- All exported functions return documented typed statuses.
- Ownership rules are short, clear, and complete.
- Public bit-sequence contracts use explicit length-carrying types where they
  improve safety and clarity.

## Phase 2 - Freeze Zero-Valid Outputs and Lifecycle Rules

Goal: make outputs safe, reusable, and easy to reason about.

Tasks:
- Make output structs valid when zero-initialized where possible.
- Define reuse/clear rules for:
  - `SchemeKeypair`
  - `KnapsackRunOutput`
  - future benchmark/attack result structs
- Ensure all clear functions are safe on zeroed structs.
- Fix `knapsack_run_once` so output reuse is safe and leak-free.

Acceptance criteria:
- Reusing and clearing outputs is safe and deterministic.
- No known output lifecycle leaks remain.

## Phase 3 - Freeze Scheme API for Future Variants

Goal: avoid another interface break when new schemes are added.

Tasks:
- Add `SchemeId`.
- Add `SchemeKeygenParams`.
- Replace `keygen(size_t n, ...)` with `keygen(params, ...)`.
- Include seed now even if MH does not use it fully yet.
- Keep params minimal for now:
  - scheme id
  - `n`
  - optional seed
  - flags/reserved field
- Keep `SchemeOps` small and stable.
- Align scheme encrypt/decrypt contracts with the frozen bit/message types.
- Remove hidden fallback behavior from lower layers.

Acceptance criteria:
- New schemes can fit the same contract without redesign.
- `system` does not silently choose a scheme.
- Seed is part of the frozen public interface.

## Phase 4 - CLI Hardening and Input Model

Goal: define input behavior once and stop revisiting it later.

Tasks:
- Replace `atoi` and unchecked `strtoul` with checked parsing.
- Standardize parse failures and option validation.
- Fix help/prog-name behavior.
- Define the message input model:
  - `--msg-bits`
  - later `--msg-text`
- Document exact bit/text behavior.
- Parse bit input into the frozen bit/message representation instead of ad hoc
  raw arrays.
- Keep parsing/validation in `cli`.
- Keep user-facing messages in `app`.

Acceptance criteria:
- Invalid input never silently falls back.
- Help text matches actual behavior.
- CLI errors are typed and consistent.

## Phase 5 - Add Test Scaffold and First Contract Tests

Goal: lock down the new foundations before extending the system.

Tasks:
- Add a minimal test runner.
- Add `make test`.
- Add first tests for:
  - CLI parse success/failure
  - checked numeric parsing
  - output clear/reuse behavior
  - roundtrip smoke tests
- Add sanitizer-friendly test execution.

Acceptance criteria:
- Basic tests run from one command.
- Core contract regressions are caught automatically.

## Phase 6 - Stabilize MH and Current Pipeline

Goal: make the current scheme reliable before adding more schemes.

Tasks:
- Fix MH key generation so modular inverse always exists.
- Define allowed parameter ranges and enforce them.
- Add regression tests across many `n` values and seeds.
- Ensure full pipeline correctness and leak-safe cleanup.

Acceptance criteria:
- No known valid `n` causes random roundtrip failure.
- Tests catch correctness regressions.

## Phase 7 - Add Scheme Registry and `--scheme`

Goal: make new scheme support additive instead of invasive.

Tasks:
- Add scheme registry/resolver.
- Add `--scheme`.
- Register classic MH as the first scheme.
- Ensure `app`, `system`, and `bench` operate only through the stable scheme API.

Acceptance criteria:
- Adding a scheme means implementing a new module and registering it once.
- Orchestration code does not need scheme-specific edits.

## Phase 8 - Reproducible Benchmarking

Goal: produce benchmark data you can defend in the thesis.

Tasks:
- Keep CSV output for raw rows.
- Add JSON metadata sidecar with:
  - scheme id
  - params
  - seed
  - timestamp
  - build mode
  - compiler flags
  - git commit
- Define timing policy:
  - clock source
  - warmup policy
  - repetitions
  - summary statistics
- Make benchmark behavior deterministic under explicit seed.

Acceptance criteria:
- Same config + same seed gives reproducible results.
- Every figure/result can be traced to one command and one output set.

## Phase 9 - Attack API

Goal: prepare attack work without forcing refactors later.

Tasks:
- Add `include/attack.h`.
- Define `AttackOps` with a stable input/result contract.
- Keep attack input schema independent of one specific scheme.
- Implement one simple baseline attack first.
- Add attack-oriented benchmark path.

Acceptance criteria:
- New attacks can be added like plugins/modules.
- Attack results are machine-readable and benchmark-friendly.

## Phase 10 - Variants and Breakability Experiments

Goal: support thesis comparison across schemes and weak parameter settings.

Tasks:
- Add selected scheme variants one by one.
- Add weak parameter presets for demo/experiments.
- Sweep parameters and record attack success/runtime.
- Export results in a plotting-friendly format.

Acceptance criteria:
- Variant comparison is systematic.
- Breakability demos are reproducible and easy to present.

## Phase 11 - Plotting and Reporting

Goal: generate thesis figures from raw outputs with minimal manual work.

Tasks:
- Add plotting scripts.
- Read only from benchmark/attack output files.
- Add one-command regeneration for figures/tables.
- Keep labels/units/metadata consistent.

Acceptance criteria:
- Figures regenerate from raw data.
- Plot outputs match benchmark definitions.

## Phase 12 - CI and Quality Gates

Goal: keep the project stable while it grows.

Tasks:
- Add CI for build + tests.
- Add debug/release jobs.
- Add sanitizer job.
- Optionally add formatting/lint checks.

Acceptance criteria:
- Main branch does not drift into a broken state.
- Basic regressions are caught early.

## Phase 13 - Thesis Integration

Goal: keep implementation and thesis aligned continuously.

Tasks:
- Mirror architecture decisions into `thesis/implementation.tex`.
- Track mapping:
  - figure/table
  - command
  - output files
  - git commit
- Keep attack/variant/results chapters aligned with actual experiments.

Acceptance criteria:
- Every major thesis claim has a reproducible artifact behind it.

---

## 4) Recommended Module Layout

Keep the flat layout for now, but use this ownership model:

- `include/common.h` - primitive shared typedefs/macros
- `include/bits.h`, `src/bits.c` - minimal bit sequence view/buffer types and helpers
- `include/error.h` - shared typed statuses
- `include/scheme.h`, `src/scheme_*.c` - scheme API + implementations
- `include/attack.h`, `src/attack_*.c` - attack API + implementations
- `include/system.h`, `src/system.c` - one full pipeline run
- `include/cli.h`, `src/cli.c` - parsing and validation
- `include/bench.h`, `src/bench.c` - benchmark runner/output
- `include/app.h`, `src/app.c` - top-level user-facing flow
- `src/main.c` - mode dispatch

---

## 5) Lock-In Checkpoints

Checkpoint A:
- typed status system live
- public ownership rules documented
- zero-valid output contracts working

Checkpoint B:
- CLI hardened
- tests in place
- MH pipeline stable across supported parameters

Checkpoint C:
- stable scheme API
- scheme registry active
- no hidden hardcoded scheme behavior in orchestration

Checkpoint D:
- reproducible benchmark outputs with metadata
- timing policy finalized

Checkpoint E:
- attack API live
- at least one baseline attack implemented
- variant/attack comparison workflow functioning

Checkpoint F:
- plots reproducible
- thesis chapters tied to real outputs
- CI stable

---

## 6) Recommended Immediate Work Order

Do these next, in strict order:

1. Add `error.h` and replace raw status returns with the shared typed status enum.
2. Document ownership/lifetime rules in `scheme.h`, `system.h`, and `cli.h`.
3. Add `SchemeKeygenParams` and freeze the scheme API now.
4. Fix `system` output lifecycle and remove hidden scheme fallback behavior.
5. Harden CLI parsing and finalize bit-message input behavior.
6. Add minimal tests and `make test`.
7. Fix MH invertibility and add roundtrip regression tests.
8. Add scheme registry + `--scheme`.
9. Make benchmark output reproducible and metadata-rich.
10. Add attack API before implementing serious attack experiments.

---

## 7) Practical Design Rules

- Lower layers return typed statuses; they do not print user-facing errors.
- `app` is the main place that turns statuses into readable messages.
- Outputs should be useful even when empty.
- Fail early when validation or resource setup cannot succeed.
- Use explicit length-carrying bit view/buffer types instead of informal raw
  bit-pointer plus size pairs when they make contracts clearer.
- Do not add variants or attacks until contracts are stable.
- Seed belongs in the public keygen interface now, even if early schemes use it minimally.

---

If this order is followed, new schemes, attacks, benchmarks, and thesis outputs
should mostly be additive instead of forcing another architecture rewrite.
