# Hud Mission And Cargo Lifetime ARM Pass

Date: 2026-08-19

## Scope

This package aligns the mission, cargo, passenger, production and timed panel
inside Android `Hud::draw` at `0x163b90`. The primary evidence is Android
`Hud.c` lines 3444-3829 plus the delinked ARM body for
`_ZN3Hud4drawExxP9PlayerEgobjj`.

## Confirmed Dispatch

The panel has six confirmed paths:

1. mission type `12`: draw the `killCountB : killCountA` counter;
2. mission type `184`: passenger panel, except campaign mission `102` at
   station `113` and mission `139` at station `131`, which select the cargo
   presentation outside alien orbit;
3. mission type `174`: production-good amount/capacity and remaining amount;
4. ordinary cargo: current load, maximum load and `t` suffix;
5. positive timed mission argument outside campaign mission `42`: formatted
   mission timer;
6. no matching special case: ordinary cargo panel.

The volatile-cargo overlay is shared by the kill, passenger/cargo, production
and ordinary cargo paths. It queries the image width and height separately,
reads volatile force twice when it is at most `1.0f`, and draws a cropped
region at the cargo-panel origin.

## Global And Call Shape

The earlier local body cached `Status::gStatus`, `PaintCanvas *` and the panel
coordinates. Android reloads `Globals::status`, `Globals::Canvas` and the Hud
coordinate fields around calls that may alias global state. The local body now
preserves those reads.

The production branch also restores the native two-query contract:

- first call `Ship::getCargo(productionGoodIndex)` to test presence;
- repeat `getShip`, `getMission`, `getProductionGoodIndex` and `getCargo` before
  reading `Item::getAmount`.

This is deliberate source-shape recovery, not an optimization. It reproduces
the original call graph even though a cached Item pointer would be simpler.

## String Slots

The strong-protector ARM build now uses the same eight 12-byte temporary slots
as Android: `sp+124`, `136`, `148`, `160`, `172`, `184`, `196`, and `208`.
The recovered expression families are:

| Panel | Expression |
| --- | --- |
| Passenger | `passengers + " / " + maxPassengers` |
| Cargo | `currentLoad + " / " + maxLoad + "t"` |
| Production | `amount + " / " + (amount + freeSpace)` |
| Production remaining | `productionGoodAmount - statusValue` |
| Kill counter | `killCountB + " : " + killCountA` |
| Timer | default String filled by `Globals::longToTimeString` |

Temporary operands are destroyed after the final concatenation, while the
result String remains live through its draw call. The complete exception tail
is not byte-matched yet, but the slot count, offsets and normal-path cleanup
order agree with the Android body.

## Verification

- UCRT64 `libgof2.a`: green.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem *` versus
  integer failures remain.
- `Hud::draw`: `38.7%` -> `42.9%`.
- Generated body: `3027` -> `3018`; Android target: `3223`.
- Stack reservation remains exact at `224` bytes with `d8-d11` saved.
- Across 48 `_ZN3Hud` comparisons: `70.1%` average, 22 linked-exact and 16
  raw-byte-exact; no exact function regressed.

The nine-instruction reduction is accepted because the source call topology,
stack slots and fuzzy alignment all moved toward the target. Raw instruction
count alone is not a correctness metric for this large function.

## Remaining Work

The next focused package should align the progress/mining branch joins and
their final String cleanup edges. `Hud::draw` still differs by 205 generated
instructions and is not byte-exact.
