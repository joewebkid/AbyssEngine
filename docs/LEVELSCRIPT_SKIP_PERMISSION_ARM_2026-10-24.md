# LevelScript skip permission: Android ARM call-shape recovery (2026-10-24)

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `LevelScript::canSkipCutsceneNow()` at `0x145e44`. It calls
`Status::getCurrentCampaignMission()` separately at `0x145e52`,
`0x145e66`, and `0x145e7a` as control reaches the three comparisons.
The returned permission is true for mission `0x9a` only in states
`1..9`, for `0x9d` only in states `2..4`, for `0x9e` when state is at
most `1`, and for other mission IDs. `MGame` uses the result to show
or hide the pause menu's skip button.

The prior C++ cached the mission before all branches. This change
restores the original three call sites while retaining the same branch
thresholds and return values. The getter currently only reads a field,
so no different gameplay result is claimed for a stable mission value;
the verified improvement is the original call/control shape.

Focused NDK r18b ARM comparison (`-Oz`) moves from **63.4%
source-shape / 47.8% strict fuzzy**, nonexact, to **100% / 100% and
linked-exact** (36 instructions). All 208 ARM translation units build,
native `gof2` builds, and 93 verifier tests pass (one skip). A live
pause-menu interaction remains untested. The pending first-friend
ship-insertion test still needs the Android device.

The full corpus compares 4,323/4,524 original functions across 4,736
pairs. Mean source-shape/strict fuzzy is 94.13%/83.46%; linked-exact
rises from 2,558 to 2,559, byte-exact remains 1,082, and there are no
pairwise exact losses. Missing originals remain 201, with zero skipped
units. The full report SHA-256 is
`ba8a13763a01fbcf64681fcf19775ecc9cb059ecae275db2abf20ed037a8046f`.
