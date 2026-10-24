# Level::createGun resource and sound table recovery (2026-10-24)

The primary source is Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Level::createGun` at `0xbdea4` indexes the 233-word mesh/resource table at
`.rodata 0x1fc54c` and the adjacent 233-word sound table at `0x1fc8f0`.
The original's beam-vs-object branch tests a `-1` mesh entry; rocket,
bomb, mine and sentry branches use the same resource table. The original
also reads bomb attributes through `Globals::items`, passes a float X
offset of 80 for weapon ID 48, and cascades sound IDs 15, 16, 17, 14,
13, 12 for weapon IDs 41–46 (`0xbe3ea–0xbe444`).

The previous C++ used null table pointers, null sound pointers and an
invalid item-pointer chain, so normal creation branches could dereference
null even with valid weapon data. `Level.cpp` now uses the exact original
tables, the real item array, and the confirmed constants. Unsupported
kind behavior was not changed. The two table arrays are static local
source data, not external assets or writable runtime state.

`tools/run_level_create_gun_resources_smoke.py` checks all **233/233**
entries of each C++ table against the original ELF bytes and executes the
source-extracted production `createGun` at host `-O0/-O2`. It covers the
negative-entry BeamGun route, ordinary ObjectGun, ID-48 turret offset,
rocket resource/sound, bomb attribute/resource/sound, sound fallthrough,
and registration in the Level gun array. Constructor receivers are
recording CPU fixtures; this does not execute real rendering, FMOD or
an Android weapon shot.

Native UCRT64 `gof2` builds. NDK r18b compiles **208/208** ARM units;
the full ARM verifier suite passes **93 tests** with one existing skip.
Focused `createGun` source-shape/strict fuzzy improves **53.2%/17.3% →
84.8%/31.6%**, nonexact. The full corpus compares **4,323/4,524**
originals across **4,736** pairs, mean **94.10%/83.42%** versus
**94.09%/83.41%** before. It retains **2,557 linked-exact** and
**1,082 byte-exact** pairs with **zero pair-identity losses**, zero
skipped and zero wrong-type symbols. Only `createGun` changed score.
The after-report is `_work/create_gun_full_20261024.json` (SHA-256
`588677828926942070756c7c04eb34d17efbafca2580fae53eeb91abc85a25e5`).

Remaining work: execute this path with production weapon constructors and
asset-backed scene state, then return to the first Pan `PlayerFighter`
construction and array insertion. No live device runtime or playable
combat frame is claimed.
