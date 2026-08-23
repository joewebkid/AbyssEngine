# Hud Init Menu Frame Lifetime ARM Pass

Date: 2026-08-23

## Scope

This package continues the source-shape audit of
`Hud::initHudMenu(int, Level *)` at Android address `0x1615c8`. The behavioral
contents of all four quick-menu modes were already source-backed. This pass
therefore concentrates on the common iPad anchor calculation, the mode-2
command table and the final phone/iPad position-export loops as one stack and
temporary-lifetime unit.

Evidence was checked against the Android IDA body, the independent Ghidra body,
the typed `Hud`, `Layout`, `GameSettings` and `TouchButton` layouts, and focused
ARM disassembly after every retained source change.

## Retained Source Shape

The fire-anchor branches now stage `GameSettings+0x58` before selecting the
platform offset. This preserves the native operand order in both copies of the
iPad calculation:

- iPad HD: `fireAnchorX - 112.5f`;
- iPad Large: `fireAnchorX - 160.0f`;
- other iPad layouts: `fireAnchorX - 80.0f`;
- mode 3 continues to use `GameSettings+0x54` (`steerAnchorX`) directly.

The two final `TouchButton::getPosition()` calls are again independent full
expressions. The iPad loop also reloads the selected button from the current
array after `translate`, as the Android control flow does, instead of extending
one named pointer and two named `Vector` objects across the calls.

The mode-2 action-pair table is now explicitly 16-byte aligned. This attribute
was previously disproven in isolation, but the complete later frame shape is
different: after the fire-anchor and position-return lifetime changes it
restores the Android prologue's 160-byte local reservation and dynamic
16-byte stack alignment. The table values and action ordering remain the
confirmed native values:

`0x20000, 0x40000, 0x80000, 0x100000`

No volatile register forcing, artificial stack scratch, inline assembly or
stack-canary imitation is present.

## ARM Result

Focused similarity improved from `39.1%` to `44.3%`:

| Stage | Match | Target/base instructions |
| --- | ---: | ---: |
| prior layout recurrence | 39.1% | 1245 / 1236 |
| anchor and position-return lifetime | 40.5% | 1245 / 1194 |
| complete aligned command-frame shape | 43.9% | 1245 / 1199 |
| post-translate button reload | 44.3% | 1245 / 1199 |

The lower generated instruction count is not presented as completion. The
accepted source removes redundant named `Vector` copies while improving the
normalized control-flow match.

The complete 48-function Hud set now averages `89.6%`, with 26 linked-exact
and 19 raw-byte-exact functions. No previously exact Hud function regressed.

## Rejected Experiment

Keeping two named `Vector` objects alive in phone menu compaction reduced the
focused result from `44.3%` to `41.4%` and was reverted. Although the Android
frame exposes two return areas in that branch, the experiment proves that they
come from whole-function stack coloring across mutually exclusive String and
table lifetimes, not from two explicit live source objects.

## Remaining Boundary

Android shares the aligned action table at `sp+0x40` with the explicit String
copy from the mutually exclusive equipment-label branch. The current compiler
still places that shared area at `sp+0x70`. The prologue and alignment policy
now match, but register ownership, String unwind slots and the shared table
offset do not. Closing that gap requires a complete mode-1/mode-2 allocation
graph pass or the original compiler allocation decision; it must not be faked
with padding.

This function is source-backed and native-build checked, but not byte-matched.

## Validation

- UCRT64 `libgof2.a`: green.
- ARM translation units: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::initHudMenu`: `44.3%`, `1245/1199` target/base instructions.
- all 48 `Hud` functions: `89.6%` average, 26 linked-exact, 19 byte-exact.
