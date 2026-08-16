# Hud Event Routing Recovery

Date: 2026-08-16

## Evidence

This package restores the complete Android HD `Hud::hudEvent` switch at
`0x1627a0`, including the common event-queue tail. The primary body was read
from `analysis/gof2_libgof2hdaa_full_ida.c` and
`analysis/gof2_gidra_engine_full_source.c`; the corresponding iOS body in
`analysis/gof2_ios_ida_full_dump.c` was used as an independent control-flow
check.

The iOS build uses a different GameText table: many corresponding IDs are the
Android ID minus 11. The C++ recovery deliberately keeps the Android IDs
because the repository's ARM verification target is the Android HD binary.

## Event Map

`GT(n)` below means `GameText::getText(n)`. `queue` means that processing
continues through duplicate suppression, `ListItem` construction and event
scroll setup. `return` means that the event only mutates HUD state.

| Event | Android behavior |
| ---: | --- |
| 1 | If autofire UI exists: `GT(37) + " " + GT(38)`; queue. |
| 2 | If autofire UI exists: `GT(37) + " " + GT(39)`; queue. |
| 3 | If boost UI exists and the supplied `PlayerEgo` is ready: `GT(314)`; queue. |
| 4 | If boost UI exists: `GT(315)`; queue. |
| 5 | `GT(571) + " " + GT(38)`; play sound `0x1c`; queue. |
| 6 | `GT(571) + " " + GT(39)`; play sound `0x1d`; queue. |
| 7..9 | `GT(553)`, `GT(539)`, `GT(540)` respectively; queue. |
| 10 | `GT(546) + ": " + station name`; append `" " + GT(136)` unless station index is `101`; play `0x1c`; queue. |
| 11..15 | `GT(546) + ": " + GT(550/547/548/549/545)`; play `0x1c`; queue. |
| 16..18 | `GT(307/308/309)` respectively; queue. |
| 19 | Copy the existing native Hud string at Android `Hud+0x100`; queue. |
| 20..24 | `GT(541/525/542/543/544)` respectively; queue. |
| 25 | Reset the shared charge fade timer, enable jump progress; return. |
| 26 | Disable jump progress; return. |
| 27 | `GT(322)`; queue as an important event. |
| 28 | Reset the shared charge fade timer, enable cloak progress; return. |
| 29 | Disable cloak progress; return. |
| 30 | `"-" + arg + "t " + GT(1396)`; clear the old queue, then queue as important. |
| 31 | `GT(324)`; queue as important. |
| 32..33 | `GT(218) + " " + GT(38/39)`; queue. |
| 34 | `GT(3199)`; queue. |
| 35 | Start forward transfer with mission marker visible; return. |
| 36 | `GT(3200)`, stop transfer; queue. |
| 37 | Start reverse transfer with mission marker visible; return. |
| 38 | `GT(3200)`, stop transfer; queue. |
| 39 | Start forward transfer without mission marker; return. |
| 40 | `GT(3200)`, stop transfer; queue. |
| 41 | Start reverse transfer without mission marker, then continue to the common queue tail. |
| 42 | `GT(3200)`, stop transfer; queue. |
| 43..46 | `GT(3203/3201/3202/316)` respectively; queue. |
| 47 | `"-" + arg + "t " + GT(1476)`; clear the old queue, then queue as important. |

Event `41` is intentionally asymmetric. Android IDA, Android Ghidra and the
iOS body all reach the common queue path rather than returning like events
`35`, `37` and `39`. The recovery preserves this source behavior instead of
normalizing the pair.

## Queue Tail

- `sameHudEventAsBefore` is a boolean duplicate check over the newest queue
  entries. ARM comparison rejects the tempting decompiler interpretation that
  it returns a queue index.
- Important/red `ListItem` styling uses the Android constant `0x100019`, with
  the bit index calculated from `eventId - 27`. It selects events `27`, `30`,
  `31` and `47`.
- Important events use `ListItem(String *, int)` with kind `1`; ordinary
  events use the distinct `ListItem(String *)` constructor and retain its
  default kind `-1`. The selected string is copied into the item, appended to
  the fixed event queue, measured with the active HUD font, and starts with
  scroll tick zero and scrolling enabled.
- The letterbox flag compares text width with half the screen width after the
  two native layout margins are removed.

## Corrections To The Previous Body

- Event `3` now calls `readyToBoost()` on the actual `PlayerEgo *` argument;
  the old body incorrectly treated integer `arg` as a pointer.
- Event `19` now copies the native Hud string source instead of being empty.
- The important-event mask is no longer an uninitialized placeholder and is
  indexed from event `27`, not event `1`.
- Transfer stop events now publish `GT(3200)` before disabling progress.
- All event strings and confirmed sound calls are present; the previous body
  only implemented state toggles and two UI guards.

## Boundary

The event table and queue behavior are source-backed. This is not an ARM
byte-match claim. The local host class has a compact layout, so its named
members do not occupy the original Android offsets (`Hud+0x1d8..0x1ec` and
`Hud+0x4e8..0x4f0`). Compiler-generated `String` temporary lifetimes and the
original stack-protector frame also differ. No artificial stack scratch or
canary was added to imitate those differences.

## Validation

- UCRT64 native build: `libgof2.a` links successfully.
- ARM `_ZN3Hud8hudEventEiP9PlayerEgoi`: `9.7%` fuzzy match, original `1088`
  instructions, local `885`; not linked- or raw-byte-equal.
- ARM `_ZN3Hud20sameHudEventAsBeforeEN11AbyssEngine6StringE`: `77.6%` fuzzy
  match, original `25` instructions, local `24`; not linked- or raw-byte-equal.
- ARM corpus rebuild: `201` translation units compiled; the same `3` unrelated
  units still fail on the known `SolarSystem *` versus numeric system-index
  mismatch.
