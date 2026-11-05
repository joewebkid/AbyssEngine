# Globals initialization constructors and FileRead lifetime (2026-11-05)

Evidence is from the original Android ARMv7 `libgof2hdaa.so`
(SHA-256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Globals::init` at `0xe3618` allocates and calls the typed constructors
of `Mission`, `Galaxy`, `Achievements`, `FileRead`, `AERandom`,
`Generator` and `FModSound` in that order. The observed allocation
sizes are `0x78`, `8`, `0x28`, `1`, `8`, `1` and `0x243c` bytes. It
calls `FileRead::~FileRead` and `operator delete` at `0xe3722..0xe3726`,
and `FModSound::tryToStopMusicForBGMusic` on the sound pointer at
`0xe37fe`. `Globals::getWreckCollision` and
`getRandomPlanetName` likewise construct and destroy a one-byte
`FileRead` object (`0xe4bd0..0xe4bea` and `0xe4a98..0xe4ad4`).

The ARM32 source previously called undefined constructor, destructor
and sound wrappers, even though the typed methods exist. The paths now
use `new`, `delete`, and the sound method directly on both pointer
widths. ARM32 compile-time assertions confirm all seven type sizes
match the original allocation sizes, guarding the replacement against
layout drift.

Focused ARMv7 comparison of `Globals.cpp` covers **50** symbol pairs.
`Globals::init` source-shape rises **86.5% -> 95.6%**, strict fuzzy
**75.9% -> 83.5%**. `getRandomPlanetName` stays **100%** source-shape
and becomes linked-exact; `getWreckCollision` source-shape changes
**97.3% -> 97.1%** while strict fuzzy rises **56.7% -> 61.3%**.
The raw unmatched-symbol inventory across 209 ARM32 objects falls
**262 -> 253**, removing exactly nine undefined wrapper names with no
new names. This inventory includes external libraries and is not a
full ARM32 link result.

Windows native, ARM64 static, ARMv7 **209/209**, and strict ARM64
shared linking pass. Full ARMv7 corpus: **4,323/4,524** original
functions compared in **4,770** pairs, mean source-shape **96.80%**,
strict fuzzy **86.79%**, **2,786 linked-exact** and **1,108
byte-exact**; 201 missing, zero skipped and zero wrong-type functions.
Comparison with the preceding full report found no lost exact-match
members, and gained `Globals::getRandomPlanetName` as linked-exact.
Runtime behavior on a device remains unverified; full ARM32 shared
linking is also unverified.
