# Hud Event Dispatch And String Lifetime ARM Pass

Date: 2026-08-23

## Scope

This package audits the behavior-bearing event family around
`Hud::hudEvent(int, PlayerEgo *, int)` and `Hud::hudEventMedal(int, int)`.
Evidence was checked against the Android IDA bodies at `0x1627a0` and
`0x1625a0`, the independent Android Ghidra output, the typed `Hud` layout and
focused Clang 7 ARM comparisons.

The complete event map `1..47`, GameText IDs, sound calls, queue priority mask
and transfer-event asymmetry were already source-backed. This pass restores
the two largest remaining String lifetime graphs and types the packed transfer
state used by events `35`, `37`, `39` and `41`.

## Cargo Loss String Graph

Events `30` and `47` format cargo loss with different suffixes:

- event `30`: `"-" + arg + "t " + GameText[1396]`;
- event `47`: `"-" + arg + "t " + GameText[1476]`.

The prior C++ body first assigned `"-" + arg + "t "` to a named `amount`
String. That ended the first full expression and destroyed four temporary
Strings before the GameText suffix was appended. Android keeps one complete
seven-object expression alive through assignment:

1. `String("-")`;
2. `String(arg)`;
3. the first `operator+` result;
4. `String("t ")`;
5. the second `operator+` result;
6. an explicit `String(..., false)` copy;
7. the final GameText `operator+` result.

Both event branches now use that source shape. The complete expression is
destroyed in reverse order before `Hud::clearQueue`, matching the recovered
Android body. This raises `Hud::hudEvent` from `66.1%` to `76.3%`.

## Packed Dock Transfer State

`Hud+0x278` is a two-byte transfer state whose low byte enables transfer
progress and whose high byte selects reverse direction. `Hud.h` now exposes
the ABI-neutral `dockTransferState` union alias at the confirmed offset.

The start events write the native halfwords directly:

| Event | Mission markers | Packed state | Tail |
| ---: | ---: | ---: | --- |
| `35` | `1` | `0x0001` | return |
| `37` | `1` | `0x0101` | return |
| `39` | `0` | `0x0001` | return |
| `41` | `0` | `0x0101` | continue to queue tail |

This replaces pairs of independent byte assignments without changing class
size or field offsets. Event `41` remains intentionally asymmetric, as shown
by Android IDA, Android Ghidra and the iOS control body.

## Medal Event Shape

The medal text clamp now lives inside the String expression:

`GameText[id + 0x5e3] + ":" + min(percent, 100) + "%"`

This preserves the Android order: construct the GameText/colon prefix, clamp
the percentage, then continue the same temporary chain. Duplicate suppression
constructs its String argument directly in the call slot; unlike the large
event switch, this direct temporary improves the whole function. The focused
score rises from `84.1%` to `84.9%`, and the normalized diff begins only after
the first 84 instructions.

## Rejected Experiments

- Constructing the large `hudEvent` duplicate probe directly in the call slot
  removed one local copy but changed the complete unwind graph and reduced the
  score from `76.3%` to `50.6%`; the named probe was restored.
- Merging jump/cloak enable and disable cases through a shared source label
  reduced `hudEvent` from `76.3%` to `75.5%`; it was reverted.
- Explicit queue-tail snapshots for screen width and both margins reduced the
  score from `76.3%` to `76.2%`; they were removed.
- Equivalent loop and medal-tail lifetime rewrites were instruction-neutral
  or locally worse and were not retained.

No synthetic padding, volatile register forcing, fake canary storage or inline
assembly is present.

## Validation

- UCRT64 `libgof2.a`: green; existing warnings outside this package remain.
- ARM translation units: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::hudEvent`: `66.1% -> 76.3%`, target/base `1088/1005` instructions.
- `Hud::hudEventMedal`: `84.1% -> 84.9%`, target/base `182/169` instructions.
- all 48 Hud functions: `90.7% -> 91.0%` average, 27 linked-exact and 19
  raw-byte-exact.

## Remaining Boundary

The large switch is source-backed but not byte-matched. Remaining differences
are concentrated in switch-table/literal-pool layout, jump/cloak shared-store
placement, queue-tail register ownership and exception cleanup entries. The
lower generated instruction count is not a completion claim.

The next large behavior package should return to `Hud::draw` at `50.0%` and
take one complete predicate-to-draw-to-color-restore cluster at a time.
