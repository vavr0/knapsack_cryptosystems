Project Direction We Reconfirmed
- The repo is a small framework, not a single MH demo.
- Current intended architecture was reaffirmed from the code and thesis:
  - main.c tiny, calls app_run(...)
  - app.c top-level demo/user-facing flow
  - cli.c single centralized parser
  - bench.c benchmark-only path
  - scheme_*.c crypto-only implementations
  - no shared orchestration/runtime layer
- We agreed not to reopen the architecture and not to add a generic system-style layer.
- We agreed the refactor is “done enough” structurally and the next work should be additive feature work, not redesign.
Assessment We Reached
- The refactor was basically successful.
- The biggest remaining implementation gap was randomness/seed semantics, not module architecture.
- The main correctness/extensibility problems before feature work were:
  - fake/unused seed plumbing
  - libc srand/rand global RNG usage
  - seeded keygen not actually being seeded
  - scheme-layer printing (show_steps) leaking UI into crypto
- We decided to fix these before moving to new features.
What We Changed Conceptually
We focused on Phase 1/2 cleanup around reproducible randomness and seeded keygen.
RNG / Seed Work We Planned And Then Implemented
1. We chose PCG-style explicit-state PRNG over libc rand().
- No hidden global RNG state.
- One caller owns one PrngState.
- This was specifically to support:
  - reproducibility
  - deterministic benchmarks
  - future variants and attacks without random-state interference
2. We settled the seed model.
- --seed is one user-facing deterministic run seed.
- That seed gets expanded into the two PCG seed inputs:
  - initstate
  - initseq
- If --seed is absent:
  - use OS entropy to obtain seed material
  - no continuing hidden entropy use after startup
- We explicitly discussed future expert mode with --initstate/--initseq, but decided not to implement it now.
- If ever added later, it should be an override mode, not the primary interface.
3. We agreed on widening seed types to u64.
- Because the PRNG state/seeding is naturally u64.
- This avoids fake narrowing and future repainting.
Files / Modules Added Or Reshaped
rand module
- You created a minimal explicit-state PRNG wrapper:
  - PrngState
  - prng_seed(...)
  - prng_rand(...)
- Later you added:
  - prng_rand_u64(...)
- We intentionally avoided keeping global-style prng_rand() / hidden static state.
- We discussed _r naming and removed that complexity because explicit-state functions are now the only API.
seed module
- You created seed.h / seed.c.
- Responsibility split:
  - rand.* = deterministic PRNG only
  - seed.* = entropy/default seeding + seed expansion
- Seed helpers now conceptually do:
  - fill entropy from OS
  - expand one u64 user seed into two u64s
  - resolve final PRNG init pair from either explicit seed or entropy
Entropy discussion and implementation
- We discussed cross-platform entropy handling.
- You wanted OS-native entropy, not time-based fake randomness as the primary path.
- We clarified:
  - Linux: getrandom(...)
  - macOS/BSD: arc4random_buf(...)
  - Windows: BCryptGenRandom(...)
- You chose to keep it simple and proper rather than building fallback-heavy infrastructure immediately.
- We also discussed weak fallback later, but it was not the main focus for the first pass.
Seed expansion
- We used a deterministic mix_u64(...) style helper and seed_expand_u64(...).
- Purpose:
  - turn one user seed into two good-seeming u64 values
  - avoid trivial correlation like seed ^ C1, seed ^ C2 only
- We discussed why mix_u64 matters:
  - more than “make two different values”
  - it decorrelates and scrambles them for better initialization behavior
What Was Wrong Before
Before these changes:
- app/bench used time(NULL) and libc RNG
- --seed did not mean what it looked like
- keygen ignored the passed seed
- benchmark message randomness and keygen randomness were not clearly owned
What Was Actually Refactored
Parser / CLI
- We confirmed centralized parsing is still the right design.
- You fixed seed parsing to use u64.
- We discussed scheme-id validation duplication:
  - valid scheme ids were hardcoded in both cli.c and scheme.c
- We aligned on the cleaner fix:
  - cli.c validates using scheme_resolve(...)
  - not by duplicating literal string compares
- You repaired the CLI part.
Scheme keygen params
- We agreed SchemeKeygenParams.seed was no longer the right abstraction.
- Since PRNG seeding actually needs two values, we moved to:
  - initstate
  - initseq
- That is more honest than pretending one seed value is what scheme keygen directly consumes.
App / Bench seed wiring
- app.c and bench.c were updated to:
  - resolve seed pairs
  - pass initstate/initseq into SchemeKeygenParams
- bench.c got explicit PRNG-owned message generation instead of libc rand().
- We discussed why caller-owned RNG is the right design:
  - top level owns seed/reproducibility policy
  - helpers only consume already-decided RNG streams
Benchmark semantics
- We agreed that for benchmarking:
  - the message should stay fixed across reps
  - keys should eventually vary deterministically per rep
- At this stage, the groundwork is in place, though the full per-rep distinct key-sequence policy is still a future improvement.
MH implementation changes
We spent a lot of time cleaning and improving scheme_mh.c.
Naming cleanup
We discussed and adjusted confusing names:
- moved away from some old keypair/impl confusion
- current public wrapper is now SchemeKey with data
- MH internal names were cleaned up toward:
  - priv_weights
  - pub_weights
  - mod
  - mult
  - mult_inv
- We discussed alternatives like secret_weights/public_weights too.
Seeded MH keygen
The big improvement:
- mh_keygen(...) now seeds a local PrngState from params.initstate/initseq
- mh_key_build(...) now actually consumes randomness
We changed the old mostly-fixed construction into a seeded one:
- private weights are now generated as a superincreasing sequence using a running sum
- the current logic is basically:
  - choose small positive delta
  - set priv_weights[i] = sum + delta
  - update sum += priv_weights[i]
- This means every seed now changes the actual private sequence, not just metadata
Modulus / multiplier
We discussed at length how to choose:
- delta
- modulus
- multiplier
Key conclusion:
- modulus does not need to be prime for basic MH
- it just needs:
  - mod > sum(priv_weights)
  - multiplier invertible modulo mod
So the current basic approach became:
- choose mod = sum + margin
- choose random multiplier candidate
- retry until mpz_invert(...) succeeds
We also discussed parameter scales:
- delta currently small, like 1..16
- modulus margin currently based on n
- multiplier sampled from PRNG output reduced modulo mod
We also added:
- prng_rand_u64(...)
- used to improve range for some seeded choices
Decrypt correctness
We fixed the important correctness hole:
- after greedy decryption, if remainder s != 0, decryption returns crypto failure
- previously it could silently accept inconsistent recovery
That was a meaningful correctness fix.
Printing / show_steps
This was a major design discussion.
We realized:
- show_steps was the only thing forcing scheme-layer printing
- app.c cannot reconstruct MH decrypt steps itself without access to scheme internals
- building a trace API right now would be extra work with unclear value
Since you said you likely would not reimplement the step output and had more important things to build:
- we removed show_steps
- we removed scheme-layer step printing
- we simplified the decrypt API accordingly
This was a good cleanup:
- scheme layer is crypto-only again
- no dead feature pressure
- no need to invent a generic trace system
What We Tested
You ran a lot of useful runtime tests.
Determinism checks
- Same seed + same message in demo produced identical ciphertext repeatedly.
- Different seeds produced different ciphertext while decrypting correctly.
Unseeded checks
- Running without --seed produced different ciphertext across runs.
- That is consistent with entropy-backed seeding.
Length sweeps
- Demo worked for:
  - very short messages
  - moderate messages
  - long messages
- Ciphertext grew as expected.
- Decrypt matched in all observed cases.
Bench reproducibility
- Same benchmark seed produced same resolved initstate/initseq.
- Bench outputs were stable structurally across reruns.
- Timing varied slightly as expected.
Bench scaling
You tested:
- n = 8, 16, 32, 64
- then larger:
  - 128
  - 256
  - 512
  - 1024
  - 2048
  - 4096
The scaling looked sane:
- keygen dominates
- encrypt/decrypt grow with n
- no hangs in multiplier selection
- no visible crypto failures
Large demo
You ran a 1024-bit message demo successfully with a fixed seed.
- deterministic ciphertext repeated exactly
- decrypted message matched exactly
This is a strong sign that:
- seeded keygen is wired correctly
- decrypt correctness is stable at larger sizes too
What We Learned From Testing
- The refactor around seed/PRNG is working.
- MH seeded keygen is not fake anymore.
- The current implementation seems operationally stable over a meaningful range.
- The current parameter choices are at least good enough for baseline experimentation.
Small Remaining Rough Edges
Not blockers, but noted:
- some stale comments/TODOs still exist
- some minor naming polish could still happen
- Windows portability around mpz_set_ui(... prng_rand_u64(...)) was discussed but intentionally not prioritized
- benchmark per-rep distinct deterministic key sequences are still a future step if wanted
Bigger Design Conversation About Next Work
We discussed what should come next now that plumbing is in a good place.
We ruled out:
- more architecture redesign
- building an “attack API” first
We agreed the next work should likely be feature work.
We discussed two broad directions:
1. another scheme/variant
2. attacks
On attacks
- We explicitly decided not to build a generic attack API first.
- Better approach:
  - implement one concrete attack later
  - generalize only if patterns actually emerge
On variants
We discussed several possibilities:
- mh-permuted
- iterative/stronger transformed MH variants
- weaker/tighter variants/profiles for attack demos
Your current feeling:
- simple permutation alone feels historically not very interesting
- and not especially worthwhile because it is still broken in broadly the same classical family
That is a valid concern and should carry into the next session.
What We Seem To Lean Toward
Not final, but current direction is roughly:
- a more interesting historically motivated MH-family strengthening or variation is probably more valuable than just permutation
- or go directly into attack-oriented work if that helps the thesis more
- but do not start with a generic attack abstraction
Important architectural principle we kept repeating
- simpler architecture
- additive extensions
- no shared orchestration layer
- no framework-first approach
- only abstract once repeated concrete cases justify it
What To Tell Your Teacher/Class
A concise way to frame the work:
1. The major refactor was stabilized rather than reopened.
2. Centralized CLI and split app/bench/scheme architecture were preserved.
3. Randomness/seed semantics were made honest and reproducible.
4. libc/global RNG usage was replaced by explicit local PRNG state.
5. Seed handling now supports deterministic reproducible runs and entropy-backed default runs.
6. MH key generation was changed from effectively fixed behavior to true seeded randomized construction.
7. Decrypt correctness was tightened with remainder validation.
8. Scheme-layer demo printing/step tracing was removed to keep crypto code clean.
9. Benchmarks and demos were tested across multiple seeds and up to large n, showing stable deterministic behavior.
Recommended Next Session Starting Point
Open the next session with this:
- The architecture should stay as-is.
- Do not redesign.
- The seed/PRNG and seeded MH baseline are now done enough.
- show_steps has been removed.
- Large seeded demos and benchmarks have already been tested successfully.
- The next task should be deciding between:
  1. a historically meaningful next variant, likely more interesting than plain permutation
  2. moving toward one concrete attack on the current/basic MH baseline
- Avoid building an attack framework first.
- Keep changes additive and practical.
My honest recommendation for next session
Start by deciding between:
1. a more historically interesting strengthened MH-family variant than plain permutation
2. one concrete attack implementation on the current baseline
Given your current hesitation about permutation, I would probably start that next session by explicitly asking:
- what historically meaningful variant is most worth implementing next for the thesis?
- or should the next milestone be the first attack instead?
If you want, in the next session you can paste this summary and then ask for:
- a ranked shortlist of historically meaningful next variants
- or a direct comparison of “next variant vs first attack” for thesis value.
