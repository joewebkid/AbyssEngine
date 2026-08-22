# Hud Init Menu Layout Recurrence ARM Pass

Date: 2026-08-22

## Scope

This package continues the `Hud::initHudMenu(int, Level *)` audit at Android
address `0x1615c8`. It treats the four mode bodies, the equipment String graph,
the command action table and final phone/iPad packing as one compiler-lifetime
unit. The retained changes are based on both recovered Android bodies and
focused ARM comparison.

## Recovered Layout Recurrence

Android keeps two layout values under deliberately different lifetimes:

- `Layout+0x1dc`, the menu button height, is read once before the iPad branch
  and remains live through all four mode bodies;
- `Layout+0x30`, the row gap, is reloaded whenever a row is advanced.

The prior C++ collapsed both values into a single function-wide `rowStep`.
That produced the correct coordinates but retained a non-native combined
value across almost the entire switch. The code now uses the native recurrence
at every confirmed row transition:

`nextY = currentY + Layout+0x30 + cached(Layout+0x1dc)`

The initial iPad row also reloads `Layout+0x30` for the signed half-gap
formula. The final iPad packing is the narrower exception visible in Android:
it loads the row gap once inside that branch and reuses it for both
`(4 - count) * (buttonHeight + rowGap)` and the mode-3 subtraction. Phone
packing performs its own final load before selecting zero or `-rowGap`.

## Equipment Lifetime Boundary

The unique mode-1 equipment builder has been moved back into the switch body.
The retained expression still produces the confirmed seven-object cleanup
graph:

1. `" ("`;
2. decimal item amount;
3. their first sum;
4. `")"`;
5. the completed suffix;
6. the explicit `String(..., false)` copy;
7. the final localized item label.

All seven objects are destroyed after the `TouchButton` constructor and before
the masked action decision tree. The item is intentionally reloaded for the
index and amount calls, matching the Android loop. Removing the local
`always_inline` helper is instruction-neutral, but it records the actual
single-body ownership and prevents later edits from moving the cleanup graph
away from mode 1.

## ARM Result

The source-backed layout recurrence improved focused similarity from `35.0%`
to `39.1%`. Generated size moved from `1188/1245` to `1236/1245` base/target
instructions, leaving only nine instructions of count difference even though
register allocation and stack slots are not yet identical.

The full 48-function Hud set now averages `89.1%`, with 26 linked-exact and 19
byte-exact functions. No previously exact Hud function regressed.

## Rejected Experiments

- Replacing the nested equipment expression with seven named locals reduced
  `initHudMenu` from `35.0%` to `34.0%`. It extended several lifetimes to the
  lexical scope boundary and changed unwind ordering, so it was reverted.
- Reapplying `alignas(16)` to the command action table after the accepted row
  recurrence reduced `39.1%` to `37.8%`. The target stack realignment is a
  consequence of the complete frame allocation, not an attribute that can be
  safely forced on this one table.
- Directly embedding the equipment block was score-neutral and retained for
  source ownership; no synthetic scratch, volatile register forcing, inline
  assembly or stack-canary imitation was added.

## Remaining Difference

The target dynamically aligns its 160-byte local frame. Its 16-byte command
table occupies `sp+0x40` and shares that slot with the explicit String copy in
the mutually exclusive equipment branch. The current compiler places the same
shared lifetime at `sp+0x78`. The surrounding five suffix temporaries therefore
also use different slots even though their construction and destruction order
matches.

This is not a behavior gap. A later byte-match pass must move the complete
shared action/String allocation graph together or reproduce the original
compiler/toolchain decision. Isolated alignment changes are now disproven.

## Validation

- UCRT64 `libgof2.a`: green.
- ARM translation units: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::initHudMenu`: `39.1%`, `1245/1236` target/base instructions.
- all 48 `Hud` functions: `89.1%` average, 26 linked-exact, 19 byte-exact.
