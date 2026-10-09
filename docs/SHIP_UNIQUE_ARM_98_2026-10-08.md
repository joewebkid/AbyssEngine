# Ship ARM source recovery and unique-body 98% gate Р Р†Р вЂљРІР‚Сњ 2026-10-08

Scope: `src/game/ship/Ship.cpp/.h`, starting integration `7314772c`, branch
`codex/ship-arm-98-20261008`. Work/build/report files are on C: in this worktree;
A: supplies the read-only reference binary and NDK. No ChoiceWindow, Route,
PROJECT_STATUS, verifier implementation or compiler flags are changed.

## Frozen measurement

Before source edits, the canonical full ARM build and an independent focused
baseline object were captured. The family is **93 original symbols / 91 unique
Android addresses**, original instruction weight **1751**. C1/C2 and D1/D2 count
once. The frozen ledger is `validation/ship_unique_baseline_2026-10-08.json`;
every original address, alias, weight and baseline score is retained.

The unweighted mean is sum(score)/91; the weighted mean is
sum(original_weight * score)/1751. Weights are original `asmdiff.n_target`,
never candidate lengths. Existing source-shape arithmetic/flags are unchanged.
An additional observation executes the same canonical source-shape function
with presentation rounding suppressed in a local copy of its Python globals;
it changes no verifier file or arithmetic and is reported separately. Neither
mean is rounded for the threshold decision.

| Measure | Before | After |
| --- | ---: | ---: |
| Unweighted, reported per-body scores | 97.54065934065935 | 99.29890109890111 |
| Original-instruction weighted | 93.51730439748714 | 98.58829240434038 |
| Unweighted, before per-body display rounding | 97.54206595848582 | 99.30121976767589 |
| Weighted, before per-body display rounding | 93.5117090655395 | 98.58954171529481 |

Both requested means exceed 98.0%, also before per-body display rounding.

## Accepted source-backed forms

Primary evidence is Android 2.0.16 `libgof2hdaa.so`, direct disassembly plus
matching named IDA bodies. No cross-platform table or speculative formula is
substituted for the ARM evidence. These three procedures retain their existing
void declarations: inspected callers do not consume a result, and incidental
r0 contents alone do not establish the pseudo-decompiler return types.

- `adjustPrice` **0x1754f8**: station gate and signed positive-price gate;
  catalog `Array<Ship*>` via `Globals::ships`; catalog Ship index at +0x00,
  signed integer price at +0x14; race table of 64 words at **0x2067c4**;
  same-race adjustment **-0.01f**, optional signed `Globals::globalPriceRaise`
  percentage **+0.01f**, zero defaults, original single-precision evaluation
  order. The catalog/ship/price are reloaded after the system/race calls.
- `priceDecline` **0x1754c8**: signed catalog price converted to float, divided
  by **1.25f**, then truncated to signed integer. This removes the null-root
  undefined behavior that had compiled to one instruction.
- `refreshValue` **0x174608**: +0x48 is float firePower storage, confirmed by
  `vldr/vmla/vstr` (no float-to-int conversions there); both weapon passes use
  **1000.0f**, second-pass weapon mask **0x0200010f**. Sort 20 accumulates
  passengers at **+0x44**, not firePower. Factor sentinel is exact float
  **-979797952.0f / 0xce699a1f** from literal **0x17499c**, not zero. Equipment
  pointers/slots are live reloads across Item calls; damage-factor capture
  follows attribute 9 and precedes attribute 11. Mod loop compares equality
  with captured native size. `Globals::status` is the original global.
- Public `getFirePower()` keeps its existing int/raw-bits calling contract:
  `memcpy` returns float storage bits without aliasing UB. It remains raw-byte
  exact, including the Android softfp register return. Ship ARM assertions
  lock size **0x80** and price/firePower/equipment offsets **0x14/0x48/0x6c**.
- `getFirstEquipmentOfSort` **0x174ce4**: integrated cloak for catalog indices
  44/49 reads `Globals::items` Array element **95** (data +380). The fake null
  shop root is removed as part of the same proven global-binding repair.

| Function | Original instructions | Before source | After source | After strict |
| --- | ---: | ---: | ---: | ---: |
| adjustPrice | 64 | 33.7 | 100.0 | 100.0 |
| refreshValue | 299 | 88.0 | 99.0 | 82.7 |
| priceDecline | 15 | 21.4 | 100.0 | 100.0 |
| getFirstEquipmentOfSort | 41 | 94.9 | 99.0 | 63.4 |

`priceDecline` gains linked-exact; adjustPrice is strict-fuzzy 100%, **not**
linked-exact or byte-exact. Source-shape is not binary identicality.

## Rejected forms

These are semantic rejections against ARM, not invented performance trials:

- Permanent null file-local catalog/race/difficulty/shop pointers and padded
  pseudo-records: no original binding, UB, dead code in the baseline object.
- Baseline +30% same-race adjustment, 70% difficulty multiplier and 0.1 defaults:
  contradicted by original -1% / +1% / zero literals and globalPriceRaise reads.
- int accumulation/truncation of firePower, x1 scaling, mask 0x4000, passengers
  added to firePower, or a zero-factor fallback: contradicted by float stores,
  literal 1000, native mask, +0x44 store and sentinel comparison.
- Retaining cached Item/cargo/catalog values across external calls: loses the
  observed native reloads. Host smoke explicitly switches the catalog during
  the race getter and confirms the replacement catalog price is used.
- Retyping public getters or widening Status ABI solely to improve a score:
  unnecessary for this package and would spread changes outside its evidence.
  No asm padding/barriers, artificial operations, UB tricks, flags or formula
  changes were used. No score-only trial was needed after the recovered form
  passed both family gates.

## Validation

- Native Windows/UCRT CMake `gof2` static library: passes.
- Canonical final NDK r18b/Oz build: **206/206 translation units**, zero failures.
- Focused family: **93 comparisons, 54 linked-exact, 47 raw-byte-exact**.
  Previous focused exact sets survive; one linked gain is priceDecline.
- Actual Ship.cpp and Item.cpp host smoke: passes. Checks price gates,
  race discount, positive/negative percentage, signed depreciation/truncation,
  catalog replacement across a call, fractional weapon DPS, mask membership,
  passengers/boost, two cargo mods/compression/value, raw-bits getter,
  native zero factor with no selected weapons, and integrated cloak.
  Status/SolarSystem/environment getters are test doubles; this is not a
  whole-game executable or an ARM runtime/ABI pass.
- Verifier unit tests: 36 passed with one real-object fixture skipped;
  the real ArrayInstantiations alias fixture also passes (two tests, no skip).
- Full before/after coverage: **4323/4524**, 201 absent originals, **4689
  comparison pairs**, zero wrong-type, zero skipped units; all original-name
  coverage and (unit,symbol) comparison sets unchanged.
- Full exact set comparison: **2301 -> 2302 linked-exact**, **997 -> 997 byte-exact**;
  both lost-pair sets are empty, one linked gain is Ship::priceDecline. Only the
  four methods listed above change comparison metrics. The other 205 compiled
  ARM objects are byte-identical. Scoped/full family observations agree.
- Final ledger: `validation/ship_unique_final_2026-10-08.json`, including source/
  report hashes, exact set fingerprints and empty loss sets. The precise
  reported-score sums are **9036.2/91** and **172628.1/1751**; no rounded boundary
  result is used.

Full reports: `_work/baseline-full.json`, `_work/final-full.json`.
Focused reports: `_work/baseline-ship.json`, `_work/final-ship.json`.
Local measurement helper and smoke source: `_work/ship_measure.py`,
`_work/ship_smoke.cpp`; generated outputs are not committed.
Reproduce with `GOF2_VERIFY_LOCAL_NDK=1`,
`GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work`,
`GOF2_NDK_ROOT=A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b`.

## Remaining limits

No device/ARM executable game run was performed. At this checkpoint,
`Status::getStanding` still had an int pointer contract; that cross-caller
ABI risk was subsequently repaired in
[the standing follow-up](STATUS_STANDING_POINTER_ABI_2026-10-09.md).
Existing bit-punning in other getters and unrelated incomplete
subsystems are outside this focused package. The unusual native sentinel and
invalid catalog/item or zero-divisor inputs are preserved as native behavior;
no guessed runtime guards were inserted for matching. 99% source-shape does
not establish all such cases as safe or make the game playable.
