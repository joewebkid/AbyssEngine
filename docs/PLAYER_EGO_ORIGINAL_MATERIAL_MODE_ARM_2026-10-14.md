# PlayerEgo original material mode (2026-10-14)

Android Thumb `PlayerEgo::setShip` at `0x9a29a–0x9a2a6` reads the
ship mesh's material pointer at `Mesh+0x30`, loads the integer at
`Material+0x20`, and stores it at `PlayerEgo+0x394`. Android IDA
lines 80774–80780 independently show the same chain. The old C++
treated `Material+0x20` as a pointer to a material list, so the
saved value was not the original material mode required by cloak
restoration. `setShip` now saves `Material::materialMode`; the
PlayerEgo field remains at the ARM ABI offset `0x394`.

The production-fragment `setShip` smoke now loads a material whose
mode is 7 and confirms that 7 is saved. Native `gof2`, all 206 ARM
translation units, and 37 verifier tests (one skipped) pass.

Focused `setShip` source-shape/strict fuzzy changes
**98.2%/91.3% → 97.8%/79.2%**, and its instruction count changes
**195/195 → 199/195** (generated/original). This is a semantic
correction directly supported by ARM, not an exact-match gain.
The full report `_work/player_original_material_mode_full_20261014.json`
compares **4,323/4,524** original functions with **201 missing**,
**123 extras**, and zero skipped. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. The **2,482 linked-exact**
and **1,065 byte-exact** pair sets lose and gain zero members versus
`_work/player_cloak_charge_full_20261014.json`.

This check does not exercise live ARM32 rendering or the full
`setShip`/cloak lifecycle. Next priority: recover the active cloak
fade, visibility, material restoration and recharge transition in
`PlayerEgo::update` around ARM `0x9d310–0x9d8a2`.
