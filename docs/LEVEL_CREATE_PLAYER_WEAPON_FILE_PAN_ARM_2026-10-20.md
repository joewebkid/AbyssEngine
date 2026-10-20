# Continuous Pan player creation through weapons_hd.bin — 2026-10-20

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `FileRead::loadWeaponPositions` from `Level::createPlayer` at Thumb
`0xaf360–0xaf36c`, then consumes the type-3 position/scaling pairs at
`0xaf372–0xaf3e8`. The loader at `0x1212e4` opens
`data/bin/weapons_hd.bin`. The original 7,484-byte file has SHA-256
`eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82`.
Pan (`ship id 0`) is record 5: one type-0 and one type-1 mount, no type-2
mount, and four type-3 position/scaling pairs.

`tools/run_level_player_weapon_positions_pan_gate.py` continues the
original-asset `Level::init` path through production `FileRead` decoding,
its file close, and the four production `AEGeometry` position/scaling
operations. `tools/run_level_player_complete_pan_gate.py` continues through
the non-weapon equipment loop, temporary-vector cleanup and the return
from the full production `Level::createPlayer` body. Both pass on Windows
SwiftShader and Android ARMv7 at `-O0/-O2`. The continuous fixture now
has **73** meshes, **82** transforms and **38/38** closed file operations.
It verifies all four Pan type-3 positions and scales, the outer slot
counts, and the original file path. It stops immediately after
`createPlayer` returns into `Level::init`; Stage 0 itself has not returned.

The Pan fixture's three synthetic equipment items have non-weapon type 99
and zero used gun slots. Weapon-only services in the returning gate fail
closed; that path is not claimed continuous. Separately,
`run_createplayer_weapon_contract.py` passes the complete extracted
`createPlayer` body over all 59 original weapon records in both equipment
orders at `-O0/-O2`: 118 cases, 1,576 released loader vectors, 472 type-3
geometry pairs, and 30 turret cases. This is a bounded CPU contract, not
a running-game equipment scenario.

No production C++ changed in this package. The full ARM report for the
identical source tree is `createplayer-rotation-full.json` (SHA-256
`9e9bbe7f8ed74bc81dc5b0889ddcf93cdebdb8469f9c4e7c880b8c786dbdac5f`):
**4,323/4,524** original functions, **4,735** comparisons,
**93.95%/83.24%** mean source-shape/strict fuzzy, **2,551 linked-exact**,
**1,079 byte-exact** and zero skipped units. The next integration boundary
is the post-player portion of `Level::init`; particle allocation/material
loading and a continuous nonempty weapon load remain unverified.

A fresh focused comparison of this unchanged source tree gives
`Level::createPlayer` **86.1%/35.2%** and
`FileRead::loadWeaponPositions` **97.5%/49.9%**
source-shape/strict fuzzy; neither is exact. The focused report is
`player-weapons-focused.json` (SHA-256
`aae260f656b83124ca9f9863e8e7e46220ad084ef1f5fc00984695cdd48641fe`).
