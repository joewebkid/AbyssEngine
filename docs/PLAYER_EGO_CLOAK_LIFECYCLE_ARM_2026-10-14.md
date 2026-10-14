# PlayerEgo active cloak lifecycle (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::update` in the 2.0.16
`libgof2hdaa.so` at `0x9cfe2–0x9d002` and `0x9d310–0x9d8a6`,
cross-checked with Android IDA lines 83546–83554 and 83671–83925.
The first block decrements the cloak recharge timer at `+0x20c`
and sends HUD event 46 when it expires. The previous C++ incorrectly
used the emergency-system timer at `+0x30c` for this event; the old
checkpoint notes have been corrected. The emergency timer still
decays in the current code, but its original *later* visual update
and teardown are not yet restored.

While cloaked, ARM adds frame time to `cloakCharge` at `+0x208` and
to the 64-bit Status counter at `+0xc0`. The first 2000 units fade
shader channel 1 and transform alpha; channel 2 advances for the
whole active interval. At strictly more than 25% cloaking progress
the child transform is hidden, and below 25% on fade-out it is shown
again.
After `cloakCharge > cloakChargeMax`, the code clears the cloak and
player flag, starts recharge from `+0x368`, plays sound 30, restores
the saved material mode from `+0x394`, and restores turret resource
materials when present. ARM `0x9d7f6–0x9d84e` confirms resource IDs
20102/24085/29035/29029/29023 for the equipment cases.

The extracted production-fragment cloak smoke checks charge gating,
fade values and colors, the strict 25% visibility crossings, the
64-bit time counter, expiration and recharge, hull/turret material
restoration, and turret resource 29029. The existing docking smoke
checks that event 46 now belongs to cloak recharge and is not emitted
for emergency-timer expiration. Native `gof2`, all 206 ARM translation
units, both smokes and 37 verifier tests (one skipped) pass.

Focused `PlayerEgo::update` source-shape rises **68.2% → 77.0%**
and strict fuzzy **20.4% → 31.1%**; generated/original instruction
counts change **1,704/2,913 → 2,043/2,913**. It remains non-exact.
Full report `_work/player_cloak_lifecycle_full_20261014.json`
compares **4,323/4,524** original functions with **201 missing**,
**123 extras** and zero skipped. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. The **2,482 linked-exact**
and **1,065 byte-exact** pair sets lose and gain zero members versus
`_work/player_original_material_mode_full_20261014.json`.

The smoke does not execute the entire `update` method or live ARM32
rendering. Explicit zero-initialization of the child-hidden byte at
`+0x325` is a native safety inference, not a proven source-identical
constructor write. The emergency visual's late timing, invulnerability
reset and geometry cleanup (IDA lines 84769–84810) remain unverified
and are the next priority.
