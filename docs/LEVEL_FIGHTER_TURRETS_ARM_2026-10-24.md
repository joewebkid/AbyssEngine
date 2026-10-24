# Level fighter turrets: host health and placement (2026-10-24)

Primary source: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Level::createFighterTurrets` at `0xbc2dc`. The ARM branches on the host
`KIPlayer+0x7c` value `0x2d` or `0x33`. Each branch creates static object
`0x1a74`, makes its `Player` invulnerable, calls `getMaxHitpoints` on the
**host's** `Player` at `KIPlayer+4`, and passes that result to the turret's
`Player::setMaxHitpoints`. The prior C++ instead read the turret's own
maximum health, so its assignment had no intended host-health effect.

Both branches pass a local three-float vector to `PlayerTurret::setHost`.
Original literal words at `0xbc3d8`–`0xbc3e4` establish the offsets:

| Host kind | Local offset `(x, y, z)` | Turret group |
| --- | --- | ---: |
| `0x2d` | `(0, 172.25, -460.7)` | 8 |
| `0x33` | `(0, 470, -83)` | 0 |

The former C++ supplied a zero vector for both kinds. `setHost` stores the
offset, and `PlayerTurret::update` rotates it by the host matrix before
placing the turret geometry, so this is a real placement correction. The
host's `field_0x10` link, no-target flag and append-to-enemies sequence
remain as before. The loop still observes appended elements; the original
ARM also reloads the array size each iteration.

Focused NDK r18b ARM `-Oz` verification moves from **71.0% source-shape /
54.3% strict fuzzy** to **99.8% / 69.6%** (102 original and 102 built
instructions). The function remains nonexact: the generated code reverses
the two branch bodies and uses different registers for the array load.
Changing source branch order did not change this compiler choice, so the
source follows the decompiled branch order. Focused verification checks
both branch call shapes and constants; it is not a live game test.

Native `gof2` builds, all 208 ARM translation units build, and 93 verifier
tests pass (one skip). The full corpus compares 4,323/4,524 original
functions across 4,736 pairs: mean **94.14% source-shape / 83.47% strict
fuzzy**, **2,560 linked-exact**, **1,082 byte-exact**, 201 missing and zero
skipped. Pairwise exact-set comparison with the preceding full report finds
no gains or losses. The full report SHA-256 is
`5044d568b349a59cfd2571e7f1dbba591293bfd01b29f820eaf4901cc2e19325`.

The offsets and call sequence are source-backed; the resulting live turret
position and hitpoint state have not been exercised on an Android device
because ADB currently lists no device. The pending first-friend Pan
ship-insertion gate is another runtime priority.
