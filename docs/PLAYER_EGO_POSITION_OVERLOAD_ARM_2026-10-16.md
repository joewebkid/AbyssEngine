# PlayerEgo position overload (2026-10-16)

Android ARM `_ZN9PlayerEgo11setPositionEN11AbyssEngine6AEMath6VectorE`
at `0x9b948` is a single tail branch through a veneer to
`PlayerEgo::setPosition(float, float, float)@plt`. The vector arrives
in the three float-argument registers. The previous recovered body
instead called `AEGeometry::setPosition(Vector const&)` directly,
bypassing the native `PlayerEgo` overload and generating 19 ARM
instructions instead of one. The vector overload now forwards
`v.x, v.y, v.z` to the float overload. Its callers, including the
stream-docking and MGame placement paths, keep the same API.

Focused ARM verification changes from **1.1% source-shape / 0.0%
strict fuzzy** (`19/1` rebuilt/original instructions) to **100% /
100%** (`1/1`). It is not linked-exact or byte-exact, so no such claim
is made. The native `gof2` build, **208/208** ARM translation units,
and 40 verifier tests (one existing skip) pass. Full corpus report
`A:/Galaxy on Fire new version/_work/player_ego_setposition_full_20261016.json`
compares **4,323/4,524** original functions, with **201 missing**
and **123 extras**. Average source-shape/strict fuzzy is **93.70% /
82.80%** (previously **93.68% / 82.78%**); **2,515 linked-exact**
and **1,075 byte-exact** pairs are unchanged, with zero losses.

This proves the Android call route and ARM instruction shape. It does
not independently exercise an ARM32 gameplay runtime, and the veneer
relocation is not claimed to reproduce the original linked bytes.

Next priority: audit the separate `CutScene::update()` call route.
The same full report scores it at 1.7% source-shape with 12 rebuilt
instructions versus one original instruction; its tail-call target and
runtime timing still need independent confirmation before any edit.
