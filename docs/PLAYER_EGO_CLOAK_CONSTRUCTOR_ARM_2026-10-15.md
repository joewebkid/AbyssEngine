# PlayerEgo cloak-state construction (2026-10-15)

Primary evidence is Android Thumb `PlayerEgo::PlayerEgo(Player*)` in
the 2.0.16 `libgof2hdaa.so`. The halfword store at `0x99a12`
clears the cloak/charging bytes at `+0x1ac`; the equipment and
attribute branch at `0x99cdc–0x99d4e` selects the item at `+0x1b0`,
sets charge to `-1` at `+0x208`, reads attributes 35 and 36 into
`+0x210/+0x214`, and registers sound 30 when the item exists.
Android IDA lines 80344–80389 independently show those branches.
For an integrated cloak, ARM reads entry 95 of the global item
table; otherwise it requests ship equipment sort 21. The original
`Ship::getFirstEquipmentOfSort(21)` has an integrated fallback,
but only when its equipment array exists, so the constructor's
direct item-table path is material to the contract.

The recharge maximum at `+0x368` comes from the float at original
address `0x218214`, exactly `Globals::options + 0x2c` given the ELF
symbol address `0x2181e8`. ARM `0x99f56–0x99f94` selects 5000,
7000, 9000 or 12000 at difficulty thresholds 0.0, 0.6 and 1.1.
The recovered C++ reads those four option bytes with `memcpy`,
as in the established `Status::hardCoreMode` path.

Previously the cloak pointer, timers and flags were not initialized
by the C++ constructor even though `setShip`, `toggleCloaking`, the
update loop and HUD consume them. The recovered code initializes
inactive state, selects equipped or integrated cloak, records its
durations and sound resource, and sets the difficulty-dependent
recharge maximum. An extracted-production-fragment smoke checks
absent, equipped and integrated items, attribute IDs, sound
registration and all four difficulty bands. Native `gof2`, all 206
ARM translation units, the new and adjacent cloak/setShip/engine-
sound smokes, and 37 verifier tests (one skipped) pass.

Focused constructor source-shape rises **61.0% → 68.2%** and strict
fuzzy **35.2% → 41.3%** for both C1/C2 aliases; generated/original
instruction counts change **338/604 → 408/604**. Neither alias is
exact. Full report `_work/player_cloak_ctor_full_20261014.json`
compares **4,323/4,524** original functions with **201 missing**,
**123 extras** and zero skipped. Corpus averages remain **93.40%**
source-shape and **82.29%** strict fuzzy at report precision. The
**2,483 linked-exact** and **1,065 byte-exact** pair sets gain and
lose no members versus `_work/player_supernova_ctor_full_20261014.json`.

The smoke extracts constructor fragments rather than running full
construction or live ARM32 cloaking. Behavior with malformed or NaN
difficulty data, material/runtime integration, and other missing
constructor equipment state remain unverified. The next bounded
priority is the adjacent jump-drive and boost initialization; no
behavior was invented for absent items.
