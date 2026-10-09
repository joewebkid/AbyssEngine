# Main working-tree reconciliation (2026-10-05)

The uncommitted `main` checkout was inspected without changing it. The
committed feature branches were already combined in
`codex/integrate-decomp-20261005`; this follow-up is isolated in
`codex/integrate-main-working-20261005`.

The 68 non-overlapping tracked edits from `main` were transferred, together
with 23 new research documents, validation inputs and focused tools. They
cover AbyssEngine math/render/shader source recovery, HangarWindow, Radar,
ParticleSettings, application-module ABI layout, and related checks. The
existing ELF-declared function-size bound in `delink.py` was retained while
adding the tested PC-referenced final zero-literal handling from `main`.

Overlapping `Player.cpp/.h` changes were not replayed: the later Player branch
already contains their typed return, real sound globals and matrix-wrapper
behavior. The older `StarMap.cpp` working version introduced unresolved
pointer placeholders and was not substituted for the later source-backed
StarMap branch. The section-local alias correction in `asmdiff.py` was already
present. The useful validation guidance was merged into `VALIDATION.md`, while
the old status snapshot was summarized here rather than replacing newer
figures. No tracked `_work` deletions, old StarMap-document deletion or raw
local experiment reports were imported.

Validation: UCRT64 `gof2` builds; 206/206 ARM translation units compile;
verifier tests pass (36, one optional object-dependent skip); the native
ABI/lifetime and ParticleSettings smoke tests pass. Focused Player, particle
control and Mesh smoke tests pass. The full ARM corpus compares
**4,323/4,524** originals across **4,688** rows, with 201 absent, zero
wrong-type misses and zero skipped units. Mean source-shape is **91.75%** and
strict fuzzy **79.62%**. There are **2,300 linked-exact** and **997 byte-exact**
pairs: no prior exact pair was lost, with 101 linked and eight byte gains over
the preceding integrated snapshot. Reports are local, uncommitted files at
`_work/integrated_full_gun_20261005.json` and
`_work/integrated_main_working_20261005.json`.

These are comparison and compile results, not an integrated native gameplay
runtime pass. The dirty `main` checkout and GitHub remote remain untouched.

## Latest branch-tip sweep

The later Gun (`38347263`), StarMap (`e43de497`) and Mesh documentation
(`2006e1f0`) branch tips were also merged into this integration branch. All
ten known recovery-branch tips are ancestors of the resulting snapshot.
The Gun constructor C1/C2 source-shape rows rose from 98.5% to 99.0%, and
StarMap::OnTouchMove rose from 98.4% to 98.8%. The full rerun still compares
4,323/4,524 originals across 4,688 rows, at 91.75% mean source-shape and
79.62% strict, with 2,300 linked-exact and 997 byte-exact pairs. Relative to
the immediately preceding integration report, there are zero lost or gained
exact pairs. All 206 ARM units, the UCRT64 native build, 36 verifier tests
(one optional skip), four scope-filter tests and the Gun host smoke pass.
The final local report is `_work/integrated_main_working_latest_20261005.json`.

## 2026-10-08 branch-tip integration

Gun (`c6c939b2`), Mesh loader (`7928a223`) and StarMap (`796e66e8`) advanced
again and are merged here. Native `gof2`, all 206 ARM units, 36 verifier tests
(one optional skip), Gun host smokes and the Mesh read/create smokes pass.
The full corpus still compares 4,323/4,524 originals across 4,688 rows,
with 201 absent, zero wrong-type misses and zero skipped units. Source-shape
is 91.76%, strict fuzzy 79.63%, linked-exact 2,301 and byte-exact 997.
Compared by (unit, symbol) with the previous integrated report, there are
zero linked/byte losses and one linked gain: `StarMap::render`.

The verifier build and reports were placed on `C:` because `A:` was nearly
full. Prior generated JSON reports were preserved at
`C:/Users/Dima/Documents/Galaxy on Fire new version/verification-reports-20261005/`;
the new full report is `integrated_latest_20261008.json` there. The native
static library build artifact was also preserved there. No source or original
binary was moved. The independent `main` working tree and remote remain
untouched. Native gameplay/runtime integration remains untested.

## HangarWindow, MGame and Radar merge (2026-10-08)

Merged tips `f3dfa9ff`, `fefcc3be` and `5c23e165` without source conflicts.
HangarWindow and MGame pass both fixed unique-body 98% source-shape means;
Radar remains 0.023253 percentage points below the weighted gate and is not
reported as complete. The integrated native library builds, all 206 ARM units
compile and the verifier suite passes (36 tests, one optional skip).

The full integrated report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_three_20261008.json`.
It compares 4,323/4,524 original functions across 4,689 rows, with 201 absent,
zero wrong-type misses, zero skipped units and 116 extra symbols. Corpus means:
91.76% source-shape, 79.65% strict. Exact pairs: 2,301 linked, 997 byte;
both pair sets have zero losses and zero gains relative to the previous
integrated report. The new row is another occurrence of an existing original
`ArrayRemove<Ship*>` symbol; unique original coverage is unchanged. No full
native game/runtime pass was performed. `main` and GitHub remain untouched.

## ChoiceWindow, Route and Ship merge (2026-10-08)

Merged tips `1c4288bc`, `32cbcb2b` and `d8ab3bb3` without conflicts. All
three fixed unique-body families pass both 98% source-shape means; their
individual ledgers and source evidence are in the corresponding family notes.
The integrated native `gof2` library builds, all 206 ARM translation units
compile, and the verifier tests (36, one optional skip) plus four scope-filter
tests pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_choice_route_ship_20261008.json`.
It compares 4,323/4,524 originals across 4,689 rows, with 201 absent, zero
wrong-type misses and zero skipped units. Corpus means are 91.80%
source-shape and 79.73% strict fuzzy. Exact pairs are 2,305 linked and 997
byte: the (unit, symbol) comparison set and both preceding exact sets have
zero losses; four linked-exact pairs are gained. This validates the combined
assembly result, not playable native or ARM-device runtime. `main` and the
GitHub remote remain untouched.

## MenuTouchWindow, PlayerFixedObject and Generator merge (2026-10-09)

Merged tips `bf528ff9`, `b217e3a8` and `c71c7147` without conflicts.
All three packages improve their frozen unique-body source-shape means, but
none meets both 98% targets. The combined native `gof2` library builds, all
206 ARM translation units compile, 36 verifier tests run (35 pass, one optional
object-dependent skip), the two direct alias checks and four scope-filter
tests pass, and the MenuTouchWindow update host smoke passes.

The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_menutouch_playerfixed_generator_20261009.json`.
It compares 4,323/4,524 original functions across 4,689 rows, with 201
absent, zero wrong-type misses and zero skipped units. Corpus means improve
from 91.80% to **91.85% source-shape** and from 79.73% to **79.89% strict
fuzzy**. Linked-exact increases 2,305 to **2,309**; byte-exact remains
**997**. Comparison-row, linked-exact and byte-exact sets each have zero
losses by (unit, symbol). Four linked-exact gains belong to
PlayerFixedObject. Extra symbols decrease 116 to 115. No whole-game or
ARM-device runtime was performed; the BoundingVolume virtual-slot discrepancy
and pointer-width issues remain. The independent dirty `main` checkout and
GitHub remote were not changed.

## BoundingVolume virtual ABI correction (2026-10-09)

The original ELF vtable relocations and compiled ARM objects confirm a
five-method virtual order for BoundingVolume, BoundingSphere and BoundingAAB.
The incorrect virtual destructor slots were removed and `getCollisionNormal`
was made virtual with derived overrides. The new relocation-based check
matches all 15 slots and rejects a saved pre-fix 32-byte table. Native `gof2`
and all 206 ARM units build. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_boundingvolume_abi_20261009.json`:
4,323/4,524 originals, 4,689 rows, zero wrong-type misses/skips, 91.87%
source-shape, 79.92% strict fuzzy, 2,313 linked-exact and 997 byte-exact.
Relative to the preceding integration, no comparison row or exact pair is
lost; four linked-exact pairs are gained. The native/ARM game runtime remains
untested; see [the ABI note](BOUNDING_VOLUME_VTABLE_ARM_2026-10-09.md).

## BoundingVolume projection contract follow-up (2026-10-09)

The static `Vector` return and three callers now follow the ARM hidden-result
register contract. This replaces fake `BoundingVolume*` receiver casts and an
undefined PlayerFixedObject compatibility call. Native `gof2`, 206/206 ARM
units, the projection host smoke, 36 verifier tests (one optional skip), and
four scope tests pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_projection_contract_20261009.json`.
It compares 4,323/4,524 originals across 4,689 rows with no wrong-type
misses/skipped units. Corpus source-shape remains 91.87%, strict fuzzy rises
79.92% to 79.93%, and 2,313 linked-exact / 997 byte-exact pairs remain.
Comparison and exact-pair sets have zero losses. The recovered function's
source-shape rises 71.2% to 96.4%; native game/ARM-device runtime remains
untested. The original `main` checkout and GitHub remote remain untouched.

## Galaxy station pointer contract follow-up (2026-10-09)

The Android-backed `Station *` returns from `FileRead::loadStation` and
`Galaxy::getStation` now survive 64-bit native calls; direct callers no longer
round-trip through 32-bit integers or Windows `long`. Native `gof2`, all 206
ARM units, the compile-time pointer contract smoke, 36 verifier tests (one
optional skip), and four scope tests pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_station_pointer_20261009.json`:
4,323/4,524 originals, 4,689 rows, 91.87% source-shape, 79.93% strict,
2,313 linked-exact and 997 byte-exact, with no row or exact-pair losses.
The two target source-shape scores remain unchanged. Native gameplay and
original allocation/cleanup semantics are not yet verified; see
[the ABI note](GALAXY_STATION_POINTER_ABI_2026-10-09.md).

## Galaxy and Status system pointer contract follow-up (2026-10-09)

The Android-backed `SolarSystem *` result of `Galaxy::getSystem` and the
`Status + 0x1a4` stored pointer now remain typed through native 64-bit calls.
Windows `long` round-trips at the affected callers were removed. Native
`gof2`, all 206 ARM units, the compile-time pointer smoke, 36 verifier tests
(one optional skip), and four scope tests pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_system_pointer_20261009.json`:
4,323/4,524 originals, 4,689 rows, 91.87% source-shape, 79.93% strict,
2,313 linked-exact and 997 byte-exact. Comparison-row and exact-pair sets
have zero losses or gains. Both target getters remain byte-exact. The 64-bit
whole-game runtime and raw struct-layout consumers remain unverified; see
[the system ABI note](GALAXY_SYSTEM_POINTER_ABI_2026-10-09.md).

## Wanted-system array lookup follow-up (2026-10-09)

The original Android ARM function performs two indexed loads from the systems
array before `SolarSystem::getRoutes()`. `Status::activateNewWanted` now uses
those array lookups instead of casting station system numbers to pointers.
Native `gof2`, all 206 ARM units, 36 verifier tests (one optional skip), and
four scope tests pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_activate_wanted_20261009.json`:
4,323/4,524 originals, 4,689 rows, 91.87% source-shape, 79.93% strict,
2,313 linked-exact and 997 byte-exact. Comparison-row and exact-pair sets
have zero losses or gains. The repaired function's source-shape decreases
88.7% to 88.4% and strict fuzzy 42.1% to 39.2%; the original ARM lookup
supports the change despite that local metric regression. Whole-game
runtime and other paths in the function remain unverified; see
[the lookup note](STATUS_ACTIVATE_WANTED_SYSTEM_LOOKUP_ARM_2026-10-09.md).

## Status standing pointer contract follow-up (2026-10-09)

The Android ARM `Status::getStanding` loads the pointer at `Status + 0x14`
directly into `r0`. Its former `int` C++ return truncated that pointer on
native 64-bit hosts. The getter and direct callers now use `Standing *`.
Native `gof2`, 206/206 ARM units, the compile-time pointer-contract smoke,
36 verifier tests (one optional skip), and four scope tests pass. The getter
remains byte-exact. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_standing_pointer_20261009.json`:
4,323/4,524 originals, 4,689 rows, 91.87% source-shape, 79.93% strict,
2,313 linked-exact and 997 byte-exact, with no comparison-row or exact-pair
losses. Whole-game runtime and other pointer-width risks remain unverified;
see [the ABI note](STATUS_STANDING_POINTER_ABI_2026-10-09.md).

## Status planet arrays and station loader follow-up (2026-10-09)

The original ARM getter loads prove the two planet arrays at `Status +
0x1a8/+0x1ac` are pointers. Their fields, getters, `setStation` ownership
path and Radar consumers now preserve pointer width. Original call sites
prove `Status::setStation` and `StarSystem` use
`FileRead::loadStationsBinary(SolarSystem *)`, not the unrelated no-argument
stub. Native `gof2`, 206/206 ARM units, the pointer-contract smoke, 36
verifier tests (one optional skip) and four scope tests pass. The full report
is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_planet_arrays_20261009.json`:
4,323/4,524 originals, 4,689 rows, 91.87% source-shape, 79.93% strict,
2,313 linked-exact and 997 byte-exact, with no row or exact-pair losses.
`Status::setStation` improves 86.5% to 86.9% source-shape. Loader ordering,
ownership and integrated runtime remain unverified; see
[the focused note](STATUS_PLANET_ARRAY_STATION_LOADER_ARM_2026-10-09.md).
