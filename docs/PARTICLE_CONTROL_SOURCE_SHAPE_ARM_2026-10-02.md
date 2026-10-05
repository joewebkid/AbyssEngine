# Particle control helpers: ARM 99% source-shape gate

Date: 2026-10-02. Status: **99.0% source-shape and all validation gates passed**.

The fixed 11-body cluster reaches **99.090909% unweighted source-shape** and
**99.379545% weighted by executable instructions**. The initially fixed verifier
`n_target` weights give **99.369845%**; this convention also counts literal-pool
records. Baseline was 75.345455% unweighted, 80.181136% executable-instruction
weighted, and 79.961419% verifier-record weighted. Strict fuzzy reaches
95.836364% unweighted / 84.042727% executable-instruction weighted / 84.184035%
verifier-record weighted. Three bodies remain linked-exact and byte-exact; normalized 100%
for a manager helper is explicitly not a linked/byte-exact claim.

## Isolation and evidence

- Worktree: `A:/gof2-particle-control-99`; branch:
  `codex/particle-control-arm-99-20261002`.
- Base: `f60694dd0ca578831cec832e7026b32644642d97`, clean checkout. Newer
  visible commits belonged to independent task branches, not a newer verified
  common integration. The dirty main checkout and other worktrees were not edited.
- Primary binary: `_work/bins/android_2.0.16_libgof2hdaa.so`; SHA-256
  `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
  Version identification follows the repository's input designation; addresses
  and instructions were checked against this exact binary, rather than transferred
  from older reports' different address ranges.
- Symbol map: `_work/symbols/android_2.0.16.symbols.tsv`; ARM Thumb map and
  the NDK r18b Clang 7.0.2 toolchain, canonical unchanged `-Oz` flags.
- Secondary evidence: Android region of the shared
  `analysis/gof2_libgof2hdaa_full_ida.c`, functions at `0x181d96..0x181dbe`,
  `0x182790`, `0x183ab8`, `0x183b24..0x183bb4`, `0x183dc4`, `0x183e00`.
  Early same-named functions in the combined dump were not treated as this binary.
  No iOS/PC behavior was transferred or used for the gate.

Inspected: applicable AGENTS, root CONTEXT_INDEX / PROJECT_MAP / CURRENT_STATUS /
TASK_QUEUE, reverse_engineering_context, native README / CONTRIBUTING /
PROJECT_STATUS / VALIDATION, particle ABI, render-dispatch and resource-builder
audit notes, both target headers/implementations, Array, Vector/AEMath, AERandom,
ParticleSettings/Ref, Sprite/Mesh setter signatures, and concrete calls in
PlayerJunk, PlayerTurret and RocketGun. Shared verification tools were read,
not changed. Player, Gun, Mesh, StarMap, shared configs and verifier remain untouched.

## Fixed set and argument contracts

`_work/particle-fixed-bodies.json` and fresh focused baseline were saved before
production edits. The set contains 11 different original addresses and **zero
aliases**, confirmed by grouping all symbol-map entries by address. The weights
are baseline `n_target`: **451 normalized records total**. Neither the set nor weights
changed after the initial measurement. Larger class-family means are not the gate.

- Manager selector: `(int handle, unsigned char setIndex)`.
- Manager render/update/emit switches: `(int handle, bool enabled)`.
- Manager reset: `(int handle)`; virtual base reset is slot `+8`.
- Manager manual overloads: `(int, Vector const&, int, float)` and
  `(int, Vector const&, int, Vector const&, float)`.
- Base switches: `(bool)`; base manual: `(Vector by value, int,
  Vector const*, float)`. Position is copied, velocity is optional, and the int
  selects the system's particle-set array rather than the global set ID directly.

Itanium mangling does not encode the return type. Manager manual emit now uses
`void` instead of its previous fabricated uint64 result. Existing local callers
discard the result and compile unchanged. The original pop of scratch argument
registers does not establish a meaningful result; the available decompiler output
also confuses overwritten outgoing-argument stack space with a return value.
**The exact original C++ return declaration remains an inference, not recovered
debug/type information.** External clients relying on the old local return API
would require an explicit compatibility decision.

## Per-body measurements

| Address | Exact mangled symbol | Weight | Source before → after | Strict fuzzy before → after | Linked / byte after |
| --- | --- | ---: | ---: | ---: | --- |
| `00181d96` | `_ZN15IParticleSystem10enableEmitEb` | 7 | 100.0 → 100.0 | 100.0 → 100.0 | True / True |
| `00181da4` | `_ZN15IParticleSystem12enableRenderEb` | 13 | 100.0 → 100.0 | 100.0 → 100.0 | True / True |
| `00181dbe` | `_ZN15IParticleSystem12enableUpdateEb` | 2 | 100.0 → 100.0 | 100.0 → 100.0 | True / True |
| `00182790` | `_ZN15IParticleSystem10emitManualEN11AbyssEngine6AEMath6VectorEiPKS2_f` | 306 | 84.7 → 99.6 | 32.8 → 78.0 | False / False |
| `00183ab8` | `_ZN21ParticleSystemManager18enableSystemUpdateEib` | 15 | 61.1 → 100.0 | 28.6 → 100.0 | False / False |
| `00183b24` | `_ZN21ParticleSystemManager21setParticleSetByIndexEih` | 15 | 61.1 → 100.0 | 28.6 → 100.0 | False / False |
| `00183b6c` | `_ZN21ParticleSystemManager16enableSystemEmitEib` | 15 | 58.3 → 97.3 | 21.4 → 93.3 | False / False |
| `00183b90` | `_ZN21ParticleSystemManager18enableSystemRenderEib` | 15 | 58.3 → 97.3 | 21.4 → 93.3 | False / False |
| `00183bb4` | `_ZN21ParticleSystemManager11resetSystemEi` | 16 | 61.2 → 97.5 | 21.4 → 93.8 | False / False |
| `00183dc4` | `_ZN21ParticleSystemManager10emitManualEiRKN11AbyssEngine6AEMath6VectorEiS4_f` | 23 | 75.2 → 100.0 | 20.0 → 100.0 | False / False |
| `00183e00` | `_ZN21ParticleSystemManager10emitManualEiRKN11AbyssEngine6AEMath6VectorEif` | 24 | 68.9 → 98.3 | 11.1 → 95.8 | False / False |

Full component metrics and before/after rows are in
[the machine-readable report](validation/particle_control_arm_2026-10-02.json).
Weighted mean is `sum(source_match * baseline_n_target) / 451`; the unweighted
mean is the sum over exactly these 11 rows divided by 11. Strict fuzzy uses the
same fixed weights. A supplementary audit of these same original bodies, excluding
the normalized `.word` records, gives **440 executable instructions** (302 for
base manual, and one fewer record in each of the seven manager helpers). It does
not replace or improve the initially scored 451-record gate: **both weighting
conventions pass 99.0%**. Both are recorded explicitly in the JSON report.
Scores are the verifier's recorded one-decimal per-body
values; displayed aggregate precision does not increase measurement precision.

## Accepted behavior and source changes

1. Every scoped manager helper returns only for handle `-1`, selects mesh via
   bit `0x4000`, masks a mesh index with **`0x3fffbfff`**, and dispatches through
   the concrete selected `IParticleSystem*`. The old `0x3fffffff` retained the
   backend bit and indexed the wrong slot. There is no post-selection null guard
   in these ARM bodies. Update and reset use the actual member/virtual calls,
   replacing their scoped generic shims.
2. Manual emission copies the input position and passes either null or the
   supplied velocity pointer. Artificial manager return arithmetic was removed.
3. Base manual emission rotates UVs for **`flags & 0x02000000`**, proven by byte
   `+0x37` bit 1. The old bit-7 check was wrong. Native setter argument order is
   U0, V0, U1, V1; the rotation scratch order is U0, U1, V0, V1.
4. Removed five extraneous Vector assignments by initializing operator results
   or passing temporaries to the real Vector operators. Preserved the explicit
   scalar argument snapshots before these calls and repeated field reads around
   RNG and the virtual submission. The slot after a virtual callback is read
   again, rather than retaining a stale `Vector&`.
5. Retained native RNG order: velocity X/Y/Z, position X/Z/Y, lifetime/start-size/
   end-size. Later random bounds and Y subtraction reload their definition fields
   where the ARM does. Return/conversion order was aligned to the observed source
   dataflow without forcing registers or adding dead code.
6. Lifetime fallback uses `!(lifetime >= 0)`: original VFP `LT` also takes the
   unordered NaN case. Ring wrap compares `current >= maxParticles` in that order.
7. Local velocity/position scratch is typed Vector storage and UV scratch uses
   float arrays. The definition reads use the already ABI-checked SetDefinition
   fields. This replaces misaligned/raw char reinterpretation in this scoped body.
   Three already byte-exact base switches were left unchanged.

The resource-builder and render-dispatch method bodies were not edited. The
asymmetric renderMeshes selector, resource IDs, texture outputs, manager layout,
and base vtable ordering remain as on the base commit. Other unscoped manager
helpers still have their earlier reconstruction limitations; they were not added
to this gate or silently repaired.

## Trials accepted and rejected

| Trial | Outcome |
| --- | --- |
| Correct mask/guard and common pointer-table selection | Accepted behavior; initial base manual 86.4%; short helpers still 66–71% because Clang merged the table loads |
| Remove five Vector assignments | Accepted; manual 92.3%, call-count agreement 100% |
| Duplicate calls in mesh/sprite branches | Rejected final form; short helpers improved, but manual overloads fell to 83.5% / 61.2% source with duplicated call paths |
| Select the actual system pointer, then one call | Accepted; manager update/index/manual-with-velocity 100% source; remaining short helpers 97.3–98.3% |
| Reload current slot and RNG-bound fields | Accepted; manual 93.5%; callback-slot smoke verifies the changed behavior |
| Capture setter scalars and native RNG operand timing | Accepted; raw-scratch manual 95.6% |
| ARM unordered lifetime fallback, compare operand order, delayed RNG conversions | Accepted semantics; raw-scratch manual 96.5%, fuzzy 57.3% |
| Typed Vector/UV/definition storage | Accepted at the partial checkpoint; manual 96.1%, fuzzy 56.6%; raw-cast 0.4-point advantage rejected to avoid retaining unsafe scratch representation |
| Signed-shift backend test instead of bit mask | No improvement after pointer selection; plain bit test retained |
| Reuse typed scratch alone | Rejected alone: unchanged 96.1%, fuzzy 56.3% |
| Native zero-fill and X conversion before third velocity RNG; position component snapshots | Accepted with scratch reuse: 98.3%, fuzzy 75.7% |
| Explicit nonzero emission branch with memcpy from separate returned Vector | Rejected final form: control-flow 100%, but eight extra instructions; 98.6%, fuzzy 79.2% |
| Direct construction of returned Vector in scratch storage | Accepted: 99.6%, fuzzy 78.0%, 307 vs 306 records; both fixed-set mean gates pass |

The new source uses one real typed scratch object after copying its velocity
into the particle slot. Its zero case is a native all-zero 12-byte Vector clear.
The X integer is converted before the third RNG call, as the ARM does. Emission
uses conditional direct construction into local storage, avoiding the separate
Vector assignment call and an intermediate-copy load/store sequence. Vector has
three float fields, size 12, an implicit copy constructor and trivial destructor;
placement construction allocates no heap memory. These are source/dataflow forms,
not a claim of recovered original C++ spelling. No registers are named in source.

No meaningless volatile, dead branches, forced register assignments, alternate
flags, verifier modifications, aliases, or reduced body set were used.

## Validation and exact-set gate

Native `gof2` Debug static-library build passed before and after changes. The
focused host executable passes via:

```powershell
python tools/run_particle_control_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

The runner compiles unchanged production control-function text in isolation;
the Windows PE linker could not discard unrelated unresolved GPU/resource
references from the complete translation units. The only untested vtable boundary
stub is automatic base `emit`, which aborts if reached. Vector operations,
AERandom, and ParticleSettings storage are real implementations. The probe's
virtual reset/submission stand in for the Sprite/Mesh GPU endpoints; the resource
destructor is intentionally not invoked. This is a focused control test, not a
full-game or GPU/runtime claim.

Coverage includes Sprite/Mesh and high-bit mask handles; `-1` no-op; all flag
transitions; exactly one reset on render true→false; direct reset; both manual
overloads; particle-set `-1`; lifetime fallback/override/NaN; native UV routing
and flag distinction; seeded RNG lifetime/size and all spread branches;
velocity subtraction and final emitter drag; ring wrap and callback slot changes.

Fresh baseline initially compiled 203/204 TU because PlayerStation Clang crashed
with an out-of-memory/MSYS process error. The unchanged missing TU was separately
rebuilt successfully, then the complete baseline was rerun without changing its
204 baseline objects. Final comparison uses a separate object/target directory.
The three focused byte-exact switches and the complete focused family's 16 linked /
10 byte pairs are preserved; actual set differences are empty, not just equal counts.

The renewed full ARM gate completed on **204/204 translation units**, zero
compile failures and zero skipped units. Baseline and final each compare
4,619 symbol/unit pairs covering 4,274/4,524
unique originals; missing 250, wrong-signature 7.
Corpus averages are 75.97% → 76.09% strict fuzzy and
88.99% → 89.05% source-shape. These corpus means
are separate from the fixed cluster acceptance means.

Actual `(unit, mangled_symbol)` sets were compared after normalizing path
separators: **linked-exact: 2079 → 2079, lost = 0, gained = 0; byte-exact: 959 → 959, lost = 0, gained = 0**. The JSON stores sorted-pair SHA-256
digests and complete lost/gained lists. Every fixed-body focused row agrees with
the independently built full-corpus row for both scores, both exact flags and
instruction lengths. Source-file hashes were checked again before saving this
report. No equality-of-counts substitution was used.

## Remaining risks and readiness

- The requested **both-means >=99.0%** condition passes the new focused report.
  The renewed native, ARM-TU, full-corpus and exact-set gates also pass.
- The 302-instruction (306-record) base manual body dominates the weighted result. Final
  source components are instruction inventory 98.9%, opcode inventory 99.5%,
  control flow 100%, call count 100%, and size 99.7%; 307 generated versus 306
  target records. Strict fuzzy is 78.0%; source-shape 99.6% does not make the
  manual body linked-exact.
- The unchanged decoder carries `<und>` from literal-pool IT halfwords into some
  later short functions. The allocation normalizer also treats original `fp`
  object-base accesses as frame accesses: original `mov fp,r0` holds `this`, not
  a local stack frame. These are concrete metric limitations, not a corrected or
  substituted gate result. Fixing shared verification is outside this ownership.
- The native bodies require valid non-null systems and in-range handles/set
  indices. Removing local guards restores that precondition; malformed callers
  can crash. Current host probe and inspected callers do not prove every caller
  is valid at runtime.
- Existing AERandom equivalence and exact native return-type declarations are
  separate remaining questions. No complete Android ARM32 executable was run,
  and no original GPU/visual equivalence is claimed.

Raw build logs, per-trial JSON and full corpus reports stay in this isolated
worktree's `_work`; original binaries, objects and dumps are not committed.
No push, PR, or merge.
