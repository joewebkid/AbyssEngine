# PlayerEgo supernova-protection construction (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::PlayerEgo(Player*)` in
the 2.0.16 `libgof2hdaa.so`: stores at `0x99a02–0x99a0a`
zero the overlapping geometry/flag bytes, `0x99f4a` initializes
the protection sound ID at `+0xb4` to `-1`, and `0x99f98–0x9a048`
selects and constructs the supernova effect. Android IDA lines
80473–80510 independently show the conditions, IDs and child order.

The branch runs only in a supernova system or orbit with equipment
sort 38. It obtains that equipment again, chooses sound 2261 for
item index 205 and 2260 otherwise, sets the byte flag at `+0x38`,
creates an empty `AEGeometry` root at `+0x34`, then creates resource
geometries `0x4973` and `0x4972` in that order. Their transform
handles are added to the root before the temporary wrappers are
destroyed. The destructor's original `+0x34` path frees the root;
the former C++ left that pointer and sound ID uninitialized.
`render`, `update` and the engine-sound methods read this state.

The recovered constructor now initializes the root and flag to
inactive values, initializes the sound ID to `-1`, and restores the
conditional construction and two child attachments. An extracted
production-tail smoke checks ordinary space, unprotected supernova,
and both protected sound IDs; it verifies child order and temporary
destruction. Native `gof2`, all 206 ARM translation units, the new
and adjacent engine-sound/docking/setShip smokes, and 37 verifier
tests (one skipped) pass.

Focused constructor source-shape rises **45.1% → 61.0%** and strict
fuzzy **28.3% → 35.2%** for both C1/C2 aliases; generated/original
instruction counts change **258/604 → 338/604**. Neither alias is
exact. Full report `_work/player_supernova_ctor_full_20261014.json`
compares **4,323/4,524** original functions, with **201 missing**,
**123 extras** and zero skipped. Corpus averages are **93.40%**
source-shape and **82.29%** strict fuzzy. The **2,483 linked-exact**
and **1,065 byte-exact** pair sets gain and lose no members against
`_work/player_engine_sound_full_20261014.json`.

The smoke runs the extracted constructor tail, not full construction
or a live ARM32 render/audio path. The constructor's other missing
initializations and emergency-geometry ownership remain unverified;
the former is the next bounded priority. No unsupported emergency
destructor cleanup was added.
