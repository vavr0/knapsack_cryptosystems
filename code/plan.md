# Execution Sequence (Recommended)

## Phase 0 — Freeze interfaces + success criteria (half day)

  - Define what “done” means for v1 thesis engine: multi-scheme ready,
    reproducible bench, at least one attack, demo mode, tests passing.
  - Write short API contracts for common.h, scheme.h, attack.h
    (pre/postconditions, invariants, error behavior).
  - Decide one output layout for experiments (results/bench/...).

## Phase 1 — Complete missing foundation types (1 day)

  - Implement src/common.c for all declarations in include/common.h (I32Array, StrBuf, StrSlice).
  - Enforce invariants (len/cap, null policy, NUL-termination for strings).
  - Keep API minimal and stable; avoid overengineering.

  ### invariants 

- Valid StrBuf invariant:
  - data != NULL
  - cap >= 1
  - len < cap
  - data[len] == '\0'
- Valid I32Array invariant:
  - len <= cap
  - if cap == 0 then data == NULL
  - if cap > 0 then data != NULL
- Operational rules:
    - str_buf_set(s, idx, ch) allowed only for idx < len and ch != '\0'.
    - str_buf_get(s, idx, out) allowed only for idx < len.
    - str_buf_reserve(s, new_cap) means reserve bytes including terminator.
    - str_buf_assign_cstr(s, src) ensures cap >= strlen(src) + 1.
    - str_buf_append_cstr(s, suffix) ensures cap >= len + strlen(suffix) + 1.
    - str_buf_cstr(s) always returns a valid NUL-terminated string for valid s.

## Phase 2 — Add testing infrastructure (1 day)

  - Add a simple C test runner (minimal custom runner is enough for thesis).
  - Add make test target.
  - First test sets:
    - container tests (I32Array, StrBuf, StrSlice)
    - utils tests (bits_is_valid, bits_to_array)
    - roundtrip smoke (keygen->encrypt->decrypt for several n, fixed seeds).

## Phase 3 — CLI/input hardening (0.5–1 day)

  - Replace atoi paths with checked parsing (strtol/strtoul + range checks).
  - Standardize error messages and help text.
  - Unify mode options model so demo/bench parse similarly.

## Phase 4 — Scheme architecture (1–2 days)

  - Add SchemeId + resolver/registry (--scheme support).
  - Refactor SchemeOps.keygen to use SchemeKeygenParams (seed, flags, security level placeholder).
  - Keep MH classic as reference scheme; preserve backward behavior.

## Phase 5 — Reproducible benchmarking (1–2 days)

  - Keep CSV row output for metrics.
  - Add JSON metadata sidecar: scheme id, keygen params, seed, git commit hash, build mode/flags, timestamp.
  - Ensure deterministic run paths with explicit seed handling.

## Phase 6 — Attack interface early (1–2 days)

  - Add AttackOps abstraction (name/id/run/result schema).
  - Implement one baseline attack first (bruteforce on toy n).
  - Add attack benchmark command path (attack-bench or bench --attack ...).

## Phase 7 — Breakability demo mode (1 day)

  - Add weak parameter presets.
  - Sweep over n and report attack success rate + runtime.
  - Emit machine-readable outputs for plotting.

## Phase 8 — Visualization/data pipeline (0.5–1 day)

  - Add one reproducible plotting script (Python is fine).
  - Generate figures directly from CSV/JSON.
  - Keep script deterministic and thesis-friendly.

## Phase 9 — Thesis integration (ongoing, then final pass)

  - implementation.tex: architecture, safety abstractions, API decisions, test strategy.
  - results.tex: benchmark/attack figures, reproducibility note, interpretation.
  - variations.tex: tie attack outcomes to theory chapter claims.

Cross-cutting refactors (do gradually)
- Remove non-portable includes like <bits/time.h> and use portable timing (time.h/clock_gettime wrappers).
- Introduce typed error codes (not only -1) for diagnosability.
- Keep logs structured in bench/attack paths.
Practical milestone order

1) Foundation + tests first  
2) Scheme resolver + keygen params  
3) Reproducible bench outputs  
4) Attack API + first attack  
5) Breakability mode + figures + thesis writeup
If you want, next I’ll turn this into a task-by-task checklist with exact file-level changes (what to edit in include/ and src/ in order) so implementation is mechanical.
