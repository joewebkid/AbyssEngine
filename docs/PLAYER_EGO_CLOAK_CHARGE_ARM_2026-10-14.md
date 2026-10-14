# PlayerEgo cloak-charge transition (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::update` from the 2.0.16
`libgof2hdaa.so` at `0x9d492–0x9d4c8`, cross-checked with the IDA
update body. When not cloaked and `chargingCloak` is set, ARM adds
the stored frame time at `+0x134` to `cloakCharge` at `+0x208`.
Only when that value is **greater than** `cloakDischargeMax` at
`+0x214` does it send HUD event 29, call `toggleCloaking`, and then
clear the charging byte at `+0x1ad`. The previous C++ had the
activation method but no per-frame path to reach it.

The existing production-method cloak smoke now also extracts this
update fragment. It checks that exactly reaching the threshold
does not activate, the following frame does, HUD event 29 precedes
the transition, the flag clears afterwards, and inactive charging
leaves state unchanged. Native `gof2`, 206/206 ARM translation units
and 37 verifier tests (one skipped) pass.

Focused `PlayerEgo::update` source-shape changes **67.5% → 68.2%**
and strict fuzzy **20.5% → 20.4%**. The generated body grows from
1,683 to 1,704 instructions against 2,913 original instructions;
it remains non-exact.
Full report `_work/player_cloak_charge_full_20261014.json` compares
**4,323/4,524** original functions with **201 missing**, **123
extras** and zero skipped. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. The **2,482 linked-exact**
and **1,065 byte-exact** pair sets lose and gain zero members
against `_work/player_update_beams_full_20261014.json`.

This package restores the state transition,
not the adjacent cloak fade, material reset, engine-color update,
or recharge timing. The smoke is not a whole-method or live ARM32
test. Next priority: recover the cloaked active/fade-out lifecycle
around ARM `0x9d310–0x9d8a2`, including visibility and material
restoration.
