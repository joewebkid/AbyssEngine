# Radar unique-body 98% gate (2026-10-08)

The prepared `codex/radar-arm-98-20261008` checkout started clean at
`473310054460d0eef759db67b1a59064a06f2fb1`. The requested gate is **not met**:
the retained form measures **99.629412% unweighted / 97.976747% weighted**
over the fixed unique-body set. The weighted shortfall is **0.023253 points**.
Do not round this result to a passing 98.0% or substitute the alias-inclusive
weighted mean.

## Fixed scope before source edits

[`validation/radar_98_fixed_bodies_2026-10-08.json`](validation/radar_98_fixed_bodies_2026-10-08.json)
records the composition, original ELF addresses/byte extents, immutable
instruction weights, baseline scores and aliases before the first source trial.
The 19 `_ZN5Radar` symbols represent **17 unique bodies / 5337 original
instructions**. `C1/C2` share one constructor extent; `D1/D2` share one
destructor extent. Each extent contributes once. No short function was tuned
and no body was added to or removed from the denominator.

The canonical verifier and match flags are unchanged: NDK r18b Clang 7.0.2,
`-Oz`, Thumb ARMv7, softfp/NEON and strong stack protection. Weights are the
fresh baseline report's `n_target`, including the verifier's existing
executable/literal-pool normalization. Means use the stored per-body
`source_match` values and fixed original weights, without rounding the mean
before checking both `>= 98.0` conditions.

| Measurement | Baseline | Retained final |
| --- | ---: | ---: |
| Unique-body unweighted mean | 99.600000% | 99.629412% |
| Unique-body instruction-weighted mean | 97.650159% | 97.976747% |
| `draw` source-shape | 97.0% | 97.5% |
| `draw` strict fuzzy | 48.7% | 50.6% |
| `draw` original/local instructions | 3486 / 3544 | 3486 / 3515 |
| Focused linked-exact / byte-exact symbols | 13 / 8 | 13 / 8 |

All other 18 symbol rows retain their baseline scores and instruction counts.
The aliased constructor stays 98.9%; `drawCurrentLock` stays 98.2%.
The alias-inclusive weighted mean rises from **97.817179% to 98.100732%**;
this is diagnostic only and cannot close the unique-body gate. The fixed-set final family is in
[`validation/radar_98_final_family_2026-10-08.json`](validation/radar_98_final_family_2026-10-08.json).

## Retained source forms and evidence

Only `src/game/weapons/Radar.h` and `Radar.cpp` change executable source.

- Replace the six integer placeholders at `Radar+0x16c..0x180` and their
  type-punning casts with two real 12-byte `Vector` scratch members. Android
  `draw` at `0x12e050` assigns position-return vectors to those regions via
  `Vector::operator=`: the scratch addresses are materialized at
  `0x12e7a8/0x12e7ae` and `0x12ecba/0x12ecc0`, with assignment calls including
  `0x12e8e6/0x12e900`, `0x12ee14/0x12ee2a` and `0x12f632/0x12f648`.
  ARM offset assertions now guard `0x16c/0x178`; the existing `sizeof(Radar)
  assertion still guards `0x248`. Float-zero aggregate initialization preserves
  the original zero bytes and constructor score. This removes writes through
  incompatible integer-placeholder objects. `positionScratchA/B` are descriptive
  names, not recovered original identifiers; the roles differ between phases.
  This package alone raised `draw` to 97.4%.
- Select HP background/fill by pointers to the existing image slots, then load
  the selected handles at their original points. Android chooses the
  `0xe0/0xe4/0xe8` backgrounds and `0xd0/0xd4/0xd8` fills by slot address;
  original address construction includes `0x12ec6c..0x12ec7e` and the
  conditional selections at `0x12f238/0x12f240`. The same enemy/friend/other
  choices, draw arguments, handle load before the background call and subsequent
  damage/EMP calculations remain. This raised `draw` to 97.5%.

Direct evidence is the Android 2.0.16 `.so`, its ARM instructions and the
bounded `Radar::draw` Hex-Rays excerpt. The available iOS dumps expose no
independently attributed matching `draw` body, and the PC executable has no
independently mapped Radar body. Existing render-return/caller corroboration
remains in the older Radar notes; no new iOS/PC instruction-scheduling or
pixel-equivalence claim is made here.

## Trials removed

The focused per-trial ledger is
[`validation/radar_98_trials_2026-10-08.json`](validation/radar_98_trials_2026-10-08.json).
It records 27 measured candidate/restoration runs, including the two retained
packages. Every measured family kept 13 linked-exact / eight byte-exact rows.
Rejected source forms include crosshair copies/pre-length conversions, replacing
scratch references with direct fields, raw turret/center-range/pass flags,
far-marker pointers and broader marker-pointer selection. These reduced the
source metric. Neutral forms include direct Y/Z scratch access, original-like
HP pointer defaults, off-screen slot pointers, station-label offset spelling,
boolean accumulation/masks, direct spark comparisons and single axis temporaries.
They do not justify a larger retained diff. The neutral locked-overlay pointer
was removed from the HP package.

Campaign/freelance station reload timing was also compared with the Android
loads at `0x12e474/0x12e478`, `0x12e4be/0x12e4c2` and `0x12e62c..0x12e6b2`.
Those source-order trials gave 97.5% / 97.2% and were removed. They are
instruction-scheduling leads, not evidence of a demonstrated gameplay bug.
No redundant gameplay calls, fabricated stack objects, compiler flags or
verifier formula changes were introduced to increase the score.

## Validation and remaining boundary

Baseline/final full-corpus and exact-pair retention details are recorded in
[`validation/radar_98_gate_2026-10-08.json`](validation/radar_98_gate_2026-10-08.json).
The fresh baseline matches the supplied integrated report across all 4688
`(unit, symbol)` pairs and all comparison metrics. Baseline and final generated
reports/builds live on C: under this checkout's `_work/radar98/`; original input
and NDK are read from the authorized A: paths through `GOF2_VERIFY_WORK` and
`GOF2_NDK_ROOT`. Generated binaries and original data are not committed.

Native UCRT64 `gof2` builds `libgof2.a`. The verifier regression suite runs 36
tests successfully with one pre-existing platform skip. All **206/206 ARM translation units** build. Both full reports contain **4688
comparisons / 4323 of 4524 original functions**, zero skipped units and zero
wrong-type misses. Exact sets retain **2301 linked-exact / 997 byte-exact**
`(unit, symbol)` pairs, with **zero losses and zero gains**. Pair inventories,
coverage and signatures are unchanged. Only `Radar::draw` changes any comparison
metric across the full corpus, with no source-score loss in any pair. The full
report reproduces every focused Radar row. Report hashes and the pair-level
audit are recorded in the gate JSON.

These source-shape scores are not ARM byte identity or native runtime proof.
The original ARM game and the recovered render loop were not executed. No
Godot/Android-device visual/control tests were applicable to this native-only
package. HangarWindow, MGame, shared verification tools and PROJECT_STATUS
were not edited. The unique-body 98% weighted gate remains open.
