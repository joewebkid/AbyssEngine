# PlayerEgo setShip emergency and cloak tail (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::setShip(int,int)` at
`0x9a274–0x9a4cc` in the 2.0.16 `libgof2hdaa.so`, cross-checked
against IDA lines 80743–80875. The focused tail spans ARM
`0x9a3f6–0x9a498`.

When emergency-system sort 27 is equipped, the original obtains
that same sort-27 item again and reads attribute **41** into
`PlayerEgo+0x310`. The former C++ instead dereferenced sort 41
and queried attribute 0, which could fail even with a valid
emergency-system item. The original tail tests the cloak item at
`+0x1b0` and calls `MeshCloneMaterial` with the ship mesh id,
writing the returned handle to `+0x388`. The former C++ copied a
mesh local-offset vector over that material handle and its
neighbors. The recovered call now preserves the material-handle
layout consumed by `toggleCloaking`.

An extracted production-fragment smoke checks the absent/present
emergency item and cloak branches, attribute 41, the copied
bounding center/radius calculation, and the cloned material's
mesh id and output slot. Native `gof2`, 206/206 ARM translation
units, and 37 verifier tests (one skipped) pass.

Focused `setShip` source-shape/strict fuzzy changes
**97.4%/90.6% → 98.2%/91.3%**. Generated and target instruction
counts are both **195**; the function is not linked- or byte-exact.
Full report `_work/player_set_ship_full_20261014.json` compares
**4,323/4,524** original functions, with **201 missing**, zero
skipped and **123 extras**. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. The **2,482 linked-exact**
and **1,065 byte-exact** pair sets lose and gain zero members
against `_work/player_cloak_full_20261014.json`.

The smoke extracts only the changed tail, not the entire ship
replacement lifecycle. Live ARM32 equipment/material behavior and
ownership of the emergency geometry remain unverified. Next
priority: inspect the repair-beam and emergency-geometry lifecycle
across `setShip`, `render` and destruction, with whole-method
testing where feasible.
