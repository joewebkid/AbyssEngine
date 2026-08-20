# Hud Init Menu Source-Shape ARM Pass

Date: 2026-08-21

## Scope

This package audits `Hud::initHudMenu(int, Level *)` at Android address
`0x1615c8`. The recovered IDA and Ghidra bodies were checked against the ARM
object after every retained source change. The four menu modes and their
runtime behavior were already source-backed; this pass concentrates on
allocation order, button action timing, local table shape and final coordinate
cache control flow.

## Recovered Shape

- The replacement `menuButtons` array is allocated before the previous
  equipment array is released. `quickMenuType`, the old equipment pointer and
  the new menu slot now follow the native store order.
- The iPad anchor branch reads the stored `quickMenuType`. Orbit mode uses
  `steerAnchorX`; the other modes use `fireAnchorX` minus the confirmed
  112.5/160/80 platform offset.
- Cloak and jump-drive buttons receive progress/disabled state before their
  64-bit action fields are written and before `ArrayAdd`.
- Equipment actions retain the native masked index decision tree for
  `0x2000`, `0x4000`, `0x8000` and `0x10000`.
- Command mode uses the native two-pair 64-bit local table:
  `0x0004000000020000` and `0x0010000000080000`. Separate assignments recover
  the original stack-copy shape, while each selected signed 32-bit action is
  sign-extended into the two leading `TouchButton` fields.
- Docking-target actions use the same signed 64-bit store shape for
  `0x04000000 << targetIndex`.
- The final phone/iPad coordinate loops reload the menu array count at the
  same points as Android. This matters because `TouchButton::translate` and
  the two hidden-return `getPosition()` calls occur before the next loop-size
  test.
- A single `menuSlot` is threaded through the inlined builders, matching the
  recovered `v5 = &this->menuButtons` ownership path.

## ARM Progress

Focused similarity improved from `27.3%` to `35.0%`. Generated size moved
from `1152/1245` to `1188/1245` base/target instructions. This is still not a
byte-match: the remaining differences are concentrated in iPad float-local
placement, repeated `Layout+0x30` loads, the equipment String unwind slots and
register ownership across the orbit branch.

The accepted A/B steps were:

- delayed cloak/jump action insertion: `27.3% -> 28.3%`;
- native allocation/iPad source order: `28.3% -> 28.6%`;
- final menu-count reloads: `28.6% -> 30.0%`;
- command action-pair and signed store shape: `30.0% -> 34.1%`;
- equipment mask and docking signed action: `34.1% -> 35.0%`.

## Rejected Experiments

- Splitting the two final `Vector` return values into explicit nested scopes
  reduced the result from `28.6%` to `28.3%`; the original shared stack slot
  is produced by wider control-flow allocation, not lexical scope forcing.
- Adding explicit `alignas(16)` to the command table reduced `34.1%` to
  `32.7%`. Although the target prologue dynamically aligns the stack, forcing
  the attribute distorts the rest of the frame and is not retained.

No volatile/register forcing, synthetic scratch objects or inline assembly is
present in the retained source.

## Verification

- UCRT64 `libgof2.a`: green.
- ARM object coverage: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::initHudMenu`: `35.0%`, `1245/1188` target/base instructions.
- All 48 Hud functions: `88.5%` average, 26 linked-exact and 19 byte-exact.

## Next Package

The next large package should return to `Hud::draw` as a coherent multi-branch
control-flow and register-lifetime pass. Isolated Canvas or stack edits have
already been shown to regress it; the next attempt should cover one complete
cluster from predicate through final color restore.
