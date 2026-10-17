# Level StarSystem and stream-out entry (2026-10-17)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Level::createSpace` at `0xae5b8` creates `StarSystem` after its
mode-dependent landmark branch for **all** modes (`0xaea1c`–`0xaea24`).
The previous source returned early for modes 4/23 and left mode 3 without
a `StarSystem`. Allocation now uses `sizeof(StarSystem)`, retaining the
ARM class size while avoiding a fixed 0x60-byte allocation on 64-bit hosts.
An ARM-only compile-time assertion checks `sizeof(StarSystem) == 0x60`.
The common tail now also copies the solar-system RGB values
or the `(10, 136, 10)` fallback, and converts each divided-by-three
minimum to integer as shown by `VCVT.S32.F32` at `0xaeaca`–`0xaead2`.
Those three static minimum fields are typed as 32-bit `int` in source.

`Level::init` at `0xadf94` uses the rebuilt landmarks for stream-out entry
(`0xae20e`–`0xae272`). If the station stack has a previous station in
slot 1 and the current system lacks a warp gate, it finds that station
in the system list. It places the player at four times the corresponding
`StarSystem` planet position (matching list position `i + 1`), or at
`(0, 0, 100000)` when no matching station exists, and faces the player
toward the origin. With a warp gate, absent prior station, or absent
system, it uses landmark 2's position. The ordinary `(10, 10, 10000)`
and mission-1 override remain unchanged. A stack-length guard is an
intentional host safety check; the original ARM code directly reads slot 1.

Source-extracted `-O0/-O2` host smokes cover both common-tail color paths,
ordinary entry, gate entry, station-planet entry and the missing-station
fallback. Existing station, jumpgate and wormhole-tail smokes pass. Native
`gof2`, 208/208 ARM translation units and 40 verifier tests pass (one skip).
The smokes use test doubles; they do not run production `Level::init` with
game resources. Focused `createSpace` source-shape/strict fuzzy rises
67.2%/32.3% → 71.5%/39.0%; focused `init` rises
71.7%/47.2% → 84.8%/52.7%. Neither function is exact.
Reports: `A:/Galaxy on Fire new version/_work/level_entry_starsystem_before_20261017.json`
and `level_entry_starsystem_after_20261017.json` in the same directory.

The full ARM report
`A:/Galaxy on Fire new version/_work/level_entry_starsystem_final_full_20261017.json`
compares 4,323/4,524 original functions in 4,730 rows, with 201 missing
and zero skipped/wrong-type. Mean source-shape remains 93.86% after
rounding; strict fuzzy rises 83.03% → 83.04%. Linked-exact and
byte-exact remain 2,528 and 1,076. Pairwise comparison against
`level_space_tail_final_full_20261017.json` found no exact-pair losses.

Remaining: the earlier skybox/resource setup in `createSpace` is still
incomplete, and the full native level-entry path lacks an asset-backed
runtime gate. That is the next priority; source-shape is not a substitute.
