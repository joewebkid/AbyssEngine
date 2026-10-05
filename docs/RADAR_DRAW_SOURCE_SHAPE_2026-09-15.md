# Radar Whole-Body Recovery And ARM Verification

## 2026-09-21 Recheck

A fresh focused verification of the unchanged 19-symbol family reports 99.01%
unweighted source-shape, 94.66% strict fuzzy, 13 linked-exact and eight
byte-exact symbols. The instruction-weighted source-shape mean is 93.95%.
`draw(Player*, Hud*, int)` alone is 90.9% source-shape and 47.9% strict fuzzy
(3486 target versus 3546 local instructions). Its inventory score is 90.3%
and control-flow score is 78.8%. These are different measures; the family
mean must not be presented as `draw` completion or byte matching.

The current Android body was checked again against the enemy loop, lock-label
string lifetime and ambient audio tail. Two source-shape trials were rejected:
branch-local lock-label strings changed `drawCurrentLock` from 96.6/93.6 to
96.4/93.8 source/strict; widening the draw prelude flags changed `draw` from
90.9/47.9 to 90.0/47.8 (and the mission flag alone to 91.0/43.2). The
source was restored after each trial. No synthetic stack locals or compiler
barriers were introduced. Focused snapshots are in `_work/radar_*_20260921.json`.

The native UCRT64 `gof2` library builds successfully. The remaining dense
work is the real `draw` CFG/local-lifetime rewrite, especially the enemy
candidate loop and the audio join; neither is justified by a percentage alone.
The full cached-object ARM corpus also completed: 4681 comparisons, 87.4%
average source-shape, 75.4% average strict fuzzy, 2029 linked-exact and 930
byte-exact. Its report is `_work/full_corpus_radar_recheck_20260921.json`.

Date: 2026-09-15. Scope: `src/game/weapons/Radar.cpp` and `Radar.h`.
This is an Android HD ARM32 recovery package, not a Godot runtime migration.

## Measurement

The unchanged 19-symbol Radar set reaches an unweighted **99.0% source-shape
mean**, from **96.0526316%** at the beginning of this package. The goal is met
for that measure only: it is not 99% semantic completion or byte equivalence.
The machine-readable snapshot is
[radar_family_2026-09-15.json](validation/radar_family_2026-09-15.json).

| Function | Initial source | Final source | Final strict fuzzy | Target/base instructions |
| --- | ---: | ---: | ---: | ---: |
| `draw(Player*, Hud*, int)` | 73.5% | 90.8% | 41.7% | 3486 / 3511 |
| Constructor C1 | 84.3% | 98.1% | 92.0% | 797 / 803 |
| Constructor C2 | 84.3% | 98.1% | 92.0% | 797 / 803 |
| `drawCurrentLock(Hud*)` | 91.5% | 96.6% | 93.6% | 622 / 635 |
| `calcDistance(float, float, float, float, float, float)` | 93.1% | 99.1% | 91.3% | 224 / 223 |
| `elipsoidIntersect(int, int, Vector)` | 98.3% | 98.3% | 81.7% | 60 / 60 |
| Other 13 symbols | 100% | 100% | 100% | Unchanged |

The 13 linked-exact and eight raw-byte-exact symbols remain intact. The six
table entries above are neither linked-exact nor raw-byte-exact. No symbols
were added to inflate the mean, and no compiler flags, comparison weights,
normalization rules, or acceptance thresholds changed during the Radar pass.

The mean counts constructor C1/C2 and destructor D1/D2 aliases separately.
Short accessors have the same weight as the large draw body. Always inspect
the per-function results; `draw` is **90.8%**, not 99%.

| Aggregate | Source-shape |
| --- | ---: |
| Original 19-symbol mean, aliases included | 99.0000000% |
| 17-function mean, removing C2/D2 aliases | 98.9941176% |
| 19-symbol mean weighted by target instruction count | 93.8965024% |
| 17-function instruction-weighted mean | 93.2539067% |

The 19-symbol strict-fuzzy mean is **94.3315789%**. Instruction weights use
the native normalized instruction counts, not execution frequency or semantic
coverage; all of these aggregates are local Radar evidence only.

Final draw components are instruction inventory **89.6%**, opcode inventory
**95.5%**, ordered control-flow **78.9%**, and instruction-count agreement
**99.3%**. Source-shape tolerates some allocation differences; it does not
prove branch destinations, call arguments, or equivalent execution by itself.

## Evidence

Primary Android evidence is the original instructions in
`_work/bins/android_2.0.16_libgof2hdaa.so`, its symbol table, and
`../analysis/gof2_libgof2hdaa_full_ida.c`. Cross-checks use
`../analysis/gof2_ios_ida_full_dump.c`,
`../analysis/gof2_gidra_ios_engine_full_source.c`, and the extracted
`../gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Radar.c` body.
Java is not used to establish Android offsets, thresholds, or ARM shape.

| Android address | Evidence |
| --- | --- |
| `0x12d484` | Constructor family: initialization, labels, guns, Image2D output slots, equipment |
| `0x12de3c` | Destructor |
| `0x12de60`, `0x12e01c` | Vector and KIPlayer update overloads |
| `0x12df48` | Ellipse intersection |
| `0x12e050` | Whole draw body: landmarks, planets, enemies, gas, asteroids, audio |
| `0x130704` | Distance String construction and integer/floating-point recurrence |
| `0x130998` | Current-lock label, station fallback, race/quality icons and color |

Original binaries, instruction dumps, scratch trials and source dumps remain
external or ignored; the snapshot contains comparison metadata only.

## Recovered Changes

- Constructor: native zero/store order, direct Image2D member output slots,
  width/height query timing, full-expression station/DLC labels, Rocket/Bomb/
  Mine wiring, equipment re-queries, Sprite allocation and frame setup.
- Target fields now initialize their pointer union members, not only an int
  half of a pointer. This preserves full-width null initialization on the host;
  it costs 0.2 source points on each constructor versus the intermediate 98.3%
  checkpoint. The repair is retained rather than optimizing away correctness.
- Draw prelude: original passed-player context, fresh Status/Canvas queries,
  camera inverse and position scratch, mission filter and planet-approach byte.
- Station/planet bodies: native candidate exclusions, repeated mission queries,
  crosshair rounding, correct offscreen image slots, tech/distance text rows,
  fresh lock cleanup and common Sprite progress tails. Both phases now live
  inside draw with their native exit joins instead of transfer helpers.
- Enemy body: original near/far and dead-cargo split, threat classification,
  fresh player/target reads, HP/EMP images and geometry, candidate selection,
  lock timing and acquisition, cargo preview and event routing.
- Enemy lock finish: candidate reload after `isDead`, fresh fast-lock flags,
  selected threshold/lock-slot addresses and common dereferences. ARM anchors
  `0x12fe6c`, `0x12fec6..0x12fed4`, `0x12ffca..0x12ffd8` independently prove
  these details. This raises draw 89.4% to 90.5% before the asteroid follow-up.
- Gas: plasma-spark sight/range checks, fresh equipment attributes, original
  candidate blocking and delayed-lock Sprite call order. Its progress body is
  distinct from the asteroid path; the former shared helper lost native timing.
- Asteroids: four populated nearest-target slots scanned through five entries,
  virtual position/Vector recurrence, center-range quality and dead-cargo
  overlays, fresh candidate liveness and native HUD events 9/20.
- The asteroid finish no longer adds `candidate == nullptr` to the native
  active/docking gate. The native null candidate can still reach event/progress
  handling; silently returning changed behavior. Its `isDying` receiver is
  reloaded after `isDead`, as in the enemy path.
- Directional overlay: original conditional flag reads and common horizontal/
  bottom tails, with image/Canvas capture before the coordinate branches.
- Audio: direct calm/combat body, keep-event masks, threat bands, alien/station/
  campaign/DLC/race routing and reset tail. Event 143 keeps the native early
  return without counter reset.
- Current-lock: confirmed station fallback at Radar `+0x24`, source-selected
  labels, one conditional line lifetime/common cleanup, race/quality placement,
  fresh Canvas/font/layout reads, red-name path and damage suffix.
- Distance: signed 64-bit half-coordinate squares, native `1/4096` multiplier,
  metre/kilometre threshold, fractional substring and String temporary order.
- Superseded transfer scaffolding and unused Canvas caches were removed. No
  artificial stack-canary buffers, register bindings or volatile barriers added.

## Return ABI

`draw` and `drawCurrentLock` now return `void`. Android epilogues subtract the
stack guard solely for the protector check (`0x13032c..0x130344` and
`0x130f18..0x130f2a`); that subtraction is not a game result. Android MGame
render callers overwrite R0 after the calls. iOS counterparts and callers also
consume no result; their epilogues include `_Unwind_SjLj_Unregister` instead.
The Itanium names and argument lists remain unchanged. This is a supported
render contract, not a claim that original C++ headers have been recovered.

## Preserved Native Quirks

ARM remains authoritative where pseudocode is surprising. Cargo scanning steps
through pairs but passes the first pair to `catchCargo` even if a later pair
triggered the positive-count check. Its range predicate uses ORs rather than
an axis-wise bounded AND. The native X subtraction here is ego minus target;
the cached ARM trace resolves a misleading pseudocode interpretation.
These are preserved, not repaired by guessed gameplay intent.

## Rejected And Remaining Work

Bounded leaf, constructor and current-lock trials rejected regressions or kept
neutral forms only where they clarified verified source behavior. Four audio
forms did not reproduce the second physical ambient-play island: Clang merges
both into one, while all supported track paths still execute one play. Call-site
consolidation alone does not prove missing runtime behavior.

Whole draw still differs in CFG/block scheduling, hidden-return placement and
local lifetime. The observed native frame is 296 bytes with d8-d13; the current
source frame and allocation do not match it. Do not synthesize spills or unused
arrays to force those values. Large draw/current-lock execution equivalence,
remaining semantic field names and complete ARM byte matching remain open.
The packed Android offsets and size are compile-asserted; a successful 64-bit
host library build is not proof of a runnable 64-bit native game.

## Validation

Canonical local NDK r18b Clang 7.0.2, `-Oz`, Thumb ARMv7 softfp/NEON and
`-fstack-protector-strong` were unchanged throughout. Native UCRT64 `gof2`
build succeeds and links `libgof2.a`. ARM source-object coverage remains
201/204: the known unrelated `Player`, `NewsTicker` and `StarMap` SolarSystem
pointer/int conflicts remain. The build script's 206 objects includes orphan
objects; it is not 206 successfully built current translation units.

An isolated compile of the actual directional-helper body against a simple
field fixture passes all 16 flag combinations against the native delta truth
table. This is a formula smoke test, not execution of the original ARM binary.
Existing warnings elsewhere in the host build and nine ARM override warnings
in included headers remain; this package does not claim a warning-free corpus.
Godot files and resources were not changed and Android device QA was not run.

The report runner's existing `--unit` option now limits family reports as well
as `--show`. It selects the one object before traversal/delink/disassembly,
records `scoped_unit`, and excludes full-corpus coverage claims. A scoped report
cannot use `--fail-on-wrong-type`. The six new scope tests and six existing ASM
normalization tests pass. The final scoped report has zero skipped units and
finishes in about 1.4 seconds locally, rather than comparing unrelated bodies.
The optional unfiltered whole-corpus signature report was stopped because it
continued comparing unrelated large bodies. It is not recorded as a passed
gate; the complete scoped Radar report and full ARM compile inventory are the
validation for this package. The previously saved global report is unchanged.

Reproduce the family snapshot after rebuilding the matching objects:

```bash
export GOF2_VERIFY_LOCAL_NDK=1
bash tools/verify/build_objs.sh cmake-build-match/verify
python3 tools/verify/verify.py --no-build --unit game/weapons/Radar --only '^_ZN5Radar' \
  --report docs/validation/radar_family_2026-09-15.json
```

For cheap subsequent single-function inspection, avoid scanning the corpus:

```bash
python3 tools/verify/verify.py --no-build --unit game/weapons/Radar \
  --show _ZN5Radar4drawEP6PlayerP3Hudi
```
