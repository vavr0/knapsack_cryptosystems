# Knapsack Thesis Code Plan (No-Backtrack Roadmap)

This plan is designed to avoid backward refactors and keep the codebase stable
while you add new schemes, attacks, benchmarking, visualization, and better UX.

The order matters: freeze foundations first, then extend.

## 0) Non-Negotiable Rules (apply in every phase)

1. Keep public interfaces small and explicit (`include/*.h` is your contract).
2. Add tests before or with every contract change.
3. Keep reproducibility first-class (seed, build mode, metadata).
4. Separate concerns strictly:
   - CLI parses and validates.
   - `app` orchestrates mode-level flow.
   - `system` runs one full pipeline.
   - `scheme_*` implements crypto details.
   - future `attack_*` implements attack details.
5. Never introduce scheme-specific logic into `app`, `cli`, `bench`, `system`.
6. Every user-facing failure path must return a typed error code + readable message.

## 1) Immediate Known Gaps To Fix First

- Build flow is functional but can be simplified and made easier to reason about.
- `include/common.h` declares containers/strings but implementation file is missing.
- CLI numeric parsing is not fully checked (`atoi` and unchecked `strtoul` paths).
- Help text/prog-name behavior is inconsistent between top-level and mode handlers.
- Current MH key generation can fail for some `n` because multiplier/modulus may not be coprime.
- Bench reproducibility is partial (seed is printed but randomness/timing contract is not fully formalized).

## 2) Phase Plan (do in order)

## Phase 1 - Build System Simplification and Hygiene (first)

Goal: simple, predictable build/run/test commands with no stale artifacts.

Tasks:
- Keep Makefile minimal and deterministic.
- Make debug/release switching explicit and safe.
- Add standard targets: `all`, `debug`, `release`, `clean`, `run-demo`, `run-bench`, `test`.
- Add optional quality targets: `format`, `lint`, `san` (asan/ubsan), `check`.
- Keep GMP detection simple (`pkg-config` first, small fallback path).
- Ensure dependency tracking (`-MMD -MP`) stays enabled.

Acceptance criteria:
- `make`, `make debug`, `make release` always build correctly after mode switches.
- `make clean && make` works on a fresh machine with documented requirements.
- Help output clearly documents all targets and mode behavior.

## Phase 2 - Contract Freeze for Core APIs

Goal: freeze stable interfaces before more features.

Contracts to freeze now:
- `common.h` (containers/strings/slices + invariants).
- `scheme.h` (ops, keypair, keygen params).
- `system.h` (request/output/metrics + error behavior).
- `cli.h` (parse result model and validation policy).
- new `error.h` (typed status/error codes shared across modules).

Tasks:
- Define preconditions/postconditions in comments for each exported function.
- Introduce one shared error enum (not only `-1`).
- Define ownership/lifetime rules (who allocates, who frees).

Acceptance criteria:
- No public API change without explicit plan update.
- Contract doc is short and complete.

## Phase 3 - Foundation Types Implementation (`common.c`) + Tests

Goal: complete and trust your base abstractions.

Tasks:
- Implement all declarations from `include/common.h`.
- Enforce invariants:
  - `I32Array`: `len <= cap`, null policy consistent with `cap`.
  - `StrBuf`: NUL-terminated, `len < cap`, bounds-checked get/set.
  - `StrSlice`: valid pointer/length usage.
- Add unit tests for all edge cases (empty, growth, reserve, invalid indexes, overflow checks).

Acceptance criteria:
- All container/string tests pass in debug and release.
- No leaks under sanitizer/valgrind equivalent.

## Phase 4 - CLI Hardening + Message Input Model (bits and plain text)

Goal: define input behavior once and avoid future CLI refactors.

Tasks:
- Replace `atoi`/unchecked numeric parsing with checked `strtol`/`strtoul` and range checks.
- Standardize parse errors (`invalid value for --reps`, etc.).
- Fix mode help/prog name output.
- Define message input model:
  - `--msg-bits` for direct bit strings.
  - `--msg-text` for plain text.
  - explicit encoding policy (UTF-8 bytes).
  - explicit block policy (how text becomes `n`-bit blocks).
  - explicit padding policy and reversal on decrypt.

Acceptance criteria:
- Invalid numeric/text inputs never silently fall back.
- Help text matches actual behavior.
- Plain text roundtrip is deterministic and documented.

## Phase 5 - Scheme Architecture Lock-In (Extensibility)

Goal: add new knapsack variants without touching orchestration code.

Tasks:
- Add scheme registry/resolver (`--scheme`).
- Add `SchemeId` and `SchemeKeygenParams` (`seed`, flags, future security level).
- Keep `SchemeOps` stable with strict ownership/error semantics.
- Preserve MH classic as reference implementation.

Acceptance criteria:
- Adding new scheme = add new `scheme_x.c` + register once.
- `app/system/bench` remain unchanged for new scheme support.

## Phase 6 - Correctness Stabilization (MH + Pipeline)

Goal: guarantee pipeline correctness for supported parameter ranges.

Tasks:
- Fix MH keygen so modular inverse always exists.
- Define allowed parameter range and enforce it.
- Add roundtrip regression tests across many `n` values and seeds.
- Ensure `system` checks/dealloc paths are complete and leak-safe.

Acceptance criteria:
- No known `n` where roundtrip randomly fails in valid range.
- Regressions are caught by tests.

## Phase 7 - Reproducible Benchmarking (Data You Can Defend in Thesis)

Goal: trustworthy measurements and reproducible outputs.

Tasks:
- Keep CSV for raw rows.
- Add JSON sidecar metadata:
  - scheme id, keygen params, seed, timestamp, build mode, compiler flags, git commit.
- Define timing policy:
  - clock source,
  - warmup policy,
  - reps/statistics (mean + median + stddev if useful).
- Make benchmark behavior deterministic under explicit seed.

Acceptance criteria:
- Same config + same seed => reproducible outputs.
- Every figure can be traced to one results file + metadata.

## Phase 8 - Attack API (add early, keep simple)

Goal: prevent attack integration from forcing future refactors.

Tasks:
- Add `attack.h` with `AttackOps` (`id`, `name`, `run`, result struct).
- Define attack input/result schema independent of one scheme.
- Implement one baseline attack first (toy brute force or known simple baseline).
- Add attack benchmark command path.

Acceptance criteria:
- New attack plugin can be added like a scheme module.
- Attack outputs are machine-readable and benchmark-friendly.

## Phase 9 - Breakability Demo Mode

Goal: show educational/security behavior clearly.

Tasks:
- Add weak parameter presets.
- Sweep `n` and report attack success rate + runtime.
- Export data for plotting directly.

Acceptance criteria:
- Demo outputs are consistent and easy to present in thesis/seminar.

## Phase 10 - Visualization and Reporting Pipeline

Goal: one-command reproducible figures.

Tasks:
- Add plotting scripts (Python ok).
- Input only from CSV/JSON outputs, no manual edits.
- Add `make plot` target for thesis figure generation.

Acceptance criteria:
- Figures regenerate from raw data reliably.
- Axis labels/units are consistent with benchmark definitions.

## Phase 11 - CI/Quality Gates

Goal: keep quality stable while project grows.

Tasks:
- Add CI jobs for build + tests (debug/release).
- Add sanitizer job.
- Optionally add formatting/lint check.

Acceptance criteria:
- Main branch cannot drift into broken state.
- Failures are caught early.

## Phase 12 - Thesis Integration Pass (parallel, then final)

Goal: no end-stage panic writing implementation details.

Tasks:
- Continuously mirror architecture decisions into `thesis/implementation.tex`.
- Keep results mapping table: figure -> command -> output file -> git commit.
- Document attack findings in `thesis/variations.tex` and `thesis/results.tex`.

Acceptance criteria:
- Every major claim in thesis has reproducible artifact backing it.

## 3) Suggested File/Module Evolution (minimal but scalable)

Current layout can stay flat for now, but follow this logical ownership:

- `include/common.h`, `src/common.c` - core reusable types/utilities
- `include/error.h` - shared status codes
- `include/scheme.h`, `src/scheme_*.c` - scheme modules + registry
- `include/attack.h`, `src/attack_*.c` - attack modules + registry
- `include/system.h`, `src/system.c` - orchestration pipeline
- `include/cli.h`, `src/cli.c` - parsing/validation only
- `include/bench.h`, `src/bench.c` - benchmark runner/output
- `src/app.c`, `src/main.c` - top-level mode dispatch and UX

## 4) Lock-In Checkpoints (must pass before moving on)

Checkpoint A (after Phase 3):
- Build stable + core containers implemented + unit tests passing.

Checkpoint B (after Phase 5):
- Stable API contracts + scheme registry + no hardcoded scheme in orchestrators.

Checkpoint C (after Phase 7):
- Reproducible benchmark outputs with metadata + timing policy finalized.

Checkpoint D (after Phase 8/9):
- Attack API live + one baseline attack + breakability demo data working.

Checkpoint E (final):
- CI stable + plots reproducible + thesis chapters tied to artifacts.

## 5) Recommended Next 7 Work Items (strict order)

1. Simplify and finalize Makefile + add `make test` scaffold.
2. Add `error.h` and migrate key paths from raw `-1` to typed status.
3. Implement `src/common.c` exactly to `common.h` contracts.
4. Add minimal test runner and first unit tests (`common`, `utils`, roundtrip smoke).
5. Harden CLI parsing/help behavior and finalize `--msg-bits`/`--msg-text` decisions.
6. Add scheme registry + `--scheme` support + keygen params struct.
7. Fix MH invertibility and lock roundtrip regression tests across parameter ranges.

---

If you follow this sequence, future additions (new schemes, attacks, visualizations,
better UX) should mostly be additive, not refactor-heavy.
