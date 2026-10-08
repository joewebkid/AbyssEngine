# Route unique-body ARM source-shape gate (2026-10-08)

The prepared `codex/route-arm-98-20261008` checkout started clean at
`7314772c68cb2ac0f0fbdd0e11127ba9d4b15fec`. The retained package passes the
requested fixed-family source-shape gate: **99.3409090909091% unweighted /
98.40915141430949% original-instruction-weighted**. These are source/assembly
comparison figures, not a native runtime or whole-game completion claim.

## Fixed scope

Before editing source,
[`validation/route_98_fixed_bodies_2026-10-08.json`](validation/route_98_fixed_bodies_2026-10-08.json)
fixed the original Android ELF addresses/extents, instruction weights, aliases
and baseline scores. The 25 `_ZN5Route` symbols represent **22 unique bodies /
601 original instructions**. Each C1/C2 constructor pair and the D1/D2
destructor pair contribute once. No body or weight changes in the final mean.

| Measurement | Baseline | Retained final |
| --- | ---: | ---: |
| Unique-body unweighted mean | 98.68181818181819% | 99.3409090909091% |
| Unique-body instruction-weighted mean | 96.49267886855242% | 98.40915141430949% |
| `getWaypoint(int)` source-shape / strict fuzzy | 95.4% / 65.6% | 100.0% / 100.0% |
| `update(float,float,float)` source-shape / strict fuzzy | 90.1% / 66.3% | 100.0% / 100.0% |
| `clone` source-shape / strict fuzzy | 94.9% / 54.1% | unchanged |
| Focused linked-exact / byte-exact symbols | 10 / 8 | 11 / 8 |

The final mean uses the canonical verifier's stored `source_match` values,
without rounding the average before testing both `>= 98` inequalities.
NDK r18b Clang 7.0.2, the canonical `-Oz`/Thumb ARMv7/softfp/NEON/strong
stack-protection flags, and all verifier formulas remain unchanged.

## Accepted source forms

Only `src/game/world/Route.cpp` and `Route.h` change executable source.

- `getWaypoint(int)`, Android 2.0.16 address `0x11be34`: replace the raw
  vtable-function cast, unaligned character-buffer storage and float
  type-punning with the already declared virtual `KIPlayer::getPosition()`
  returning a real `Vector`. Android calls slot `+0x28` three times, supplies
  the result buffer in `r0` and receiver in `r1`, and stores converted X/Y/Z
  at Waypoint `+0x124/+0x128/+0x12c` (calls at `0x11be70`, `0x11be8c`,
  `0x11bea8`). The typed calls preserve all three calls, receiver reloads,
  null/range branches and integer conversions. The method is now
  **linked-exact**, at 61/61 normalized instructions, but not raw-byte-exact.
- `update(float,float,float)`, address `0x11bf70`: correct its declaration
  and definition from `float` to `void`, remove the reconstructed `return x`
  / `return 0.0f`, and spell the loop-end comparison in the original operand
  order. Android early exits restore the frame directly at `0x11bf8e`,
  without a scalar result computation; the completion path restores the
  frame and tail-branches to void `Waypoint::setActive` at `0x11c066`.
  The Vector overload at `0x11bf62` forwards the three components without
  consuming a return value. No in-tree caller consumes the scalar result.
  Thresholds remain strict `-2000 < delta < 2000` separately for X/Y/Z,
  including the existing NaN rejection. Reached/reset/activation order and
  current-index transitions remain. Both source-shape and strict fuzzy
  become 100.0%, at 88/88 instructions; the **canonical verifier still
  reports `linked_equal=false`**.
  Return types do not change these Itanium symbol names.

An independent ELF-byte check of `update` finds 260/260 bytes and **zero
differences outside the four real `R_ARM_THM_CALL`/`R_ARM_THM_JUMP24`
relocation ranges**. The eight literal bytes are identical:
`0000fa440000fac4` (2000.0f, -2000.0f). The canonical differ concatenates
objdump's textual raw fields: the target's literal data are shown as Thumb
halfwords, while the base uses full `.word` values. This reverses their
presentation order and leaves offsets `252..259` mismatching in that check.
The independent raw comparison is recorded in
[`validation/route_98_update_raw_bytes_2026-10-08.json`](validation/route_98_update_raw_bytes_2026-10-08.json).
It does not replace or change the canonical exact-pair counts or verifier.

Direct evidence is the authorized Android `.so`, raw ARM disassembly and the
bounded SDK `flight_physics_camera_collision/Route.c` excerpt. The excerpt's
inferred pointer return for `update` is an inconsistent register artifact,
not a recovered C++ return contract. The available iOS IDA dump contains an
unrelated Objective-C `Route`; it supplies no independently attributed GOF2
Route body. No independently mapped PC Route body was available. No new
iOS/PC behavioral or instruction-scheduling confirmation is asserted.

## Rejected forms and retained limits

The previous raw `GetPosFn(void*)` form omits the receiver from the C++
function type and reads floats through a character buffer. It is rejected
in favor of typed virtual dispatch, without introducing ABI casts or UB.
The previous float-return form preserves `x` across calls using VFP saved
registers and emits a separate zero literal after activation; neither is
present in Android. It is rejected as an unsupported reconstruction.

`clone`, inspected against Android `0x11c0b6`, remains at 94.9%. Its coordinate
copy, full docking-target scan, conditional time/target copies, constructor
selection and loop flag transfer are preserved. No speculative short-circuit
scan, coordinate-array lifetime change or compiler-shaped pointer arithmetic
is introduced merely to increase its score. Destructor 94.1% and the docking
constructor 98.0% also remain untouched. Their outstanding matching work is
separate from the passing family average.

The focused before/after files and trial ledger are under
`cmake-build-route-research/` on C:. Original input and NDK are read from the
authorized A: paths. Original
binary data and generated objects are not committed. Shared verifier tools,
match flags, PROJECT_STATUS, ChoiceWindow and Ship are not edited.

## Validation

Final full-corpus coverage and exact-pair retention are recorded in
[`validation/route_98_gate_2026-10-08.json`](validation/route_98_gate_2026-10-08.json).
Native UCRT64 `gof2` builds `libgof2.a`. All **206/206 ARM translation units**
build without errors. The verifier suite passes 36 tests with one existing
platform skip. The full corpus keeps **4689 pairs / 4323 of 4524 originals**,
zero skipped units and zero wrong-type misses. Comparing canonical exact
sets by `(unit, symbol)` finds **zero losses**: linked-exact increases
**2301 -> 2302** (`getWaypoint(int)`), while byte-exact stays **997**.
All 25 focused rows reproduce in the full report. Only the two changed
methods alter any full-corpus comparison metric. Mean source-shape remains
91.76% at the report's two-decimal precision; strict fuzzy rises 79.65% ->
79.67%.

The baseline full report is the supplied integration report, corroborated by
freshly compiling all 206 baseline objects and proving each byte-identical
to its integration counterpart. A redundant baseline full rerun was stopped
because its bare output filename would fail JSON saving; no partial report
is substituted. The final full-corpus run completes with the unchanged
verifier and `--fail-on-wrong-type`. The final object audit finds only
`game/world/Route.o` changed; the other 205 objects are byte-identical.
Full reports and generated builds remain on C:, outside the commit.

The recovered native executable/ARM game was not run. A native library build
and assembly comparison do not validate a playable runtime; Godot/device
visual and control checks are outside this native-only change.
