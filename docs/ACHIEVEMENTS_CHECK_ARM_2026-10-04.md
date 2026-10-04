# Achievement detection from Android ARM (2026-10-04)

Scope: `Achievements::checkForNewMedal(PlayerEgo*)` and its prerequisite
`initCheckEquipmentAndWeapons()`. Primary evidence is Android 2.0.16
`libgof2hdaa.so`: the check at `0x156914..0x156da4`, its 44-entry Thumb
branch table at `0x15697c`, and the equipment check at
`0x156da4..0x156e48`.

The check's PC-relative table address resolves to `.rodata 0x203134`, the
same 135-word value table already used by `Achievements::getValue`. The
previous local `gCFN_req` was a separate all-zero array, so it could not
evaluate any real tier. The `Status` GOT slot used by both original bodies is
`0x21015c`, relocated to `Globals::status`. The local equipment check instead
dereferenced a private null pointer. Both references now use the confirmed
objects.

The recovered switch follows all medal IDs 1..44 in the original branch
table; ID 0 has no case. The first satisfied tier wins, giving medal values
1, 2 or 3. Conditions are grouped here by the original instruction forms:

| Medal IDs | ARM-observed condition |
| --- | --- |
| 1 | Player hull-damage rate `<=` threshold |
| 2, 3, 9 | Sum bytes of `Status` arrays at `0x94`, `0x98`, `0xac`; `>=` threshold |
| 4, 11, 17, 23, 24, 25, 39 | Kills, visited stations, jumpgates, weapon count, crates, tracked credits, `Status+0x118`; `>=` threshold |
| 5..8, 10, 18, 20, 21, 26, 27, 29, 31..34 | `Status` counters at `0x9c..0xa8`, `0xb0`, `0xb8`, `0xc8`, `0xcc`, `0xd0`, `0xd4`, `0xd8`, `0xdc`, `0xe0`, `0xe8`, `0xec`; `>` threshold |
| 12 | Sum exactly 22 bytes from the array at `Status+0xb4`; `>=` threshold |
| 13, 14 | Count unlocked blueprints, or blueprints with positive production count; `>=` threshold |
| 15, 16, 19 | Playing time `>` hours threshold, mission count `>` threshold, 64-bit `Status+0xc0` duration divided by 60000 `>=` threshold |
| 22, 28, 30, 35 | No turret-and-weapon combination, enemy standing, game won, or all medals 0..43 nonzero |
| 36, 37 | Ship maximum load `>` threshold; station-ship count unsigned `>=` threshold |
| 38, 40..44 | Nonzero low byte at `Status+0x128`, `0x120`, `0x130`, `0x138`, `0x140`, `0x148` |

The original only writes a candidate if the current medal is 1, or if the
candidate is lower than the current value or the current value is zero.
The numeric `Status` field names remain intentionally unresolved: ARM proves
their offsets and comparisons, not their game-facing labels. The local
`Status::getStanding()` signature returns a 32-bit integer representation of
a pointer; this body reads the equivalent `standing` field directly to avoid
truncating a native host pointer. That choice is semantically grounded in the
current getter body but prevents an exact ARM call sequence.

Focused ARM verification: the prior stub was 17 generated instructions
against 414 original, 6.9% source-shape and 2.3% strict fuzzy. The recovered
body is 73.7% source-shape and 20.8% strict fuzzy; it is not linked-exact.
The equipment helper is 90.4% source-shape and 56.9% strict fuzzy, also not
exact. The lower score is not treated as evidence of a behavior mismatch by
itself. Native `gof2` and all 206 ARM translation units build. The full
`--fail-on-wrong-type` corpus compares 4,322/4,524 unique originals across
4,675 rows: 202 absent, zero wrong-signature and zero skipped. Overall mean
source-shape moves from 90.07% to 90.09% and strict fuzzy from 77.21% to
77.22%. Linked-exact remains 2,162 and raw-byte-exact remains 977; both
exact `(unit, symbol)` sets have zero losses and zero gains. The local
reports are `_work/blueprint-isunlocked/achievements_check_before.json`,
`achievements_check_after.json` and `achievements_check_full.json`.

## Limits and next action

No in-game runtime or save-file fixture has exercised the 45 medal paths.
The counter-offset labels, 22-byte array bound and signed duration division
are direct ARM readings; object lifetime and state initialization outside
these two bodies remain separate risks. A full corpus exact-set check is
required before accepting the package. Next, inspect the largest remaining
code-shape divergences in this function without changing observed conditions
for a metric, then add a focused runtime check when the required game-state
fixtures can be linked.
