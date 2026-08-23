# Hud Touch And Key Lifecycle ARM Pass

Date: 2026-08-23

## Scope

This package audits the complete live touch-key lifecycle around Android
`Hud::touchBegin`, `Hud::touchMove`, `Hud::touchEnd` and
`Hud::closeHudMenu`. The recovered bodies were compared with the Android IDA
functions at `0x1669fc`, `0x166ab8`, `0x166b84` and `0x1613aa`, the independent
Ghidra output, the typed `Hud` layout and the matching Clang 7 ARM object.

The package includes one behavior correction, three source-shape corrections
and one ownership correction. It does not alter hit-box coordinates or invent
new input behavior.

## Recovered Touch Lifecycle

`touchBegin` now follows the native mask recurrence for an already occupied
key slot:

1. read the slot's previous element bit;
2. clear that old bit only when the newly touched element differs;
3. unconditionally OR the new element into `Hud+0x284`;
4. replace the slot entry and return the current mask.

This removes the non-native temporary `flags` select and restores the Android
conditional bit-clear shape. Focused similarity rises from `59.4%` to `87.0%`.

`touchMove` now preserves the incoming Y coordinate before scanning the 25 key
slots. If no slot owns analog element `0x20`, it calls
`touchBegin(x, y, key)` with the original coordinates. The previous source
passed `-1` as Y; that behavior is contradicted by both Android disassembly and
the IDA body and could prevent a moved touch from being acquired correctly.
The short Y snapshot is also used by the unclamped analog branch, matching the
native halfword store. Similarity rises from `80.3%` to `97.1%`.

`touchEnd` retains the key-array data pointer for the matched iteration while
clearing the key, element slot and aggregate mask. This removes one non-native
array-data reload and raises the function from `92.3%` to `97.8%`; target and
base are both 45 instructions.

## Menu Ownership

`closeHudMenu` no longer calls `ArrayRemoveAll` after the out-of-line
`ArrayReleaseClasses<TouchButton *>` specialization. That specialization
already destroys the buttons and releases the backing allocation; Android then
destroys the `Array` object directly, clears `Hud+0x18`, and clears the menu-open
byte at `Hud+0x282`.

The resulting function is `100%` linked-exact at 15 instructions. It is not
reported as raw-byte-exact because relocation bytes remain different.

## Rejected Init-Menu Experiment

As a negative control, the station, programmed-station, docking-target and
phone-compaction helpers were embedded directly into `Hud::initHudMenu`.
Although the runtime call order stayed source-backed, Clang changed the shared
String unwind graph and similarity fell from `44.3%` to `36.9%`. The experiment
was fully reverted. An instruction-neutral implicit String-conversion variant
was also removed.

This confirms that the remaining `initHudMenu` mismatch is a whole-function
allocation and exception-cleanup problem. No stack padding, volatile register
forcing, fake canary scratch or inline assembly was retained.

## Validation

- UCRT64 `libgof2.a`: green.
- ARM translation units: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::touchBegin`: `59.4% -> 87.0%`, target/base `65/66` instructions.
- `Hud::touchMove`: `80.3% -> 97.1%`, target/base `69/67` instructions.
- `Hud::touchEnd`: `92.3% -> 97.8%`, target/base `45/45` instructions.
- `Hud::closeHudMenu`: `93.8% -> 100%` linked-exact, `15/15` instructions.
- all 48 Hud functions: `89.6% -> 90.7%` average, 27 linked-exact and 19
  raw-byte-exact.

## Remaining Boundary

`touchBegin` still differs in the compiler's placement of the shared
touch-mask load and return reload. `touchMove` differs only in trailing
literal-pool/disassembly data after the executable epilogue, while `touchEnd`
differs only in the order of two independent zero-initialization instructions.
These are not reasons to change behavior.

The next behavior-bearing Hud package should address `hudEvent` dispatch or a
complete `Hud::draw` predicate-to-color-restore cluster. `initHudMenu` should
remain on the accepted `44.3%` frame until a complete allocation graph can be
reproduced naturally.
