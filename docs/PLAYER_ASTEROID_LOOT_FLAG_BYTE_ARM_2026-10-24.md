# PlayerAsteroid loot flag byte ABI (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `PlayerAsteroid` constructor at `0xe2648` initializes the
loot flag with `strb` at `0xe28f2` (`this + 0x4c`). `update(int)` at
`0xe2d20` reads it with `ldrb` at `0xe2d8e` and clears it with `strb`
at `0xe2e04`. These are byte operations, not accesses to the full
four-byte `KIPlayer::hasCargo` union storage.

The previous `PlayerAsteroid::dropsLoot()` returned `int&` to that
storage. It could test nonzero upper bytes as a loot flag and cleared
all four bytes; neither follows the observed ARM accesses. It now
returns `uint8_t&` to the existing `crateCaptured` byte at offset
`0x4c`. This single shared accessor reaches the constructor and both
clear paths in `update` without changing other `KIPlayer` users of the
union. The meaning of the three upper bytes is not established here.

Focused NDK r18b ARM `-Oz` comparison: `update` source-shape/strict
fuzzy **89.7%/52.4% → 90.1%/52.8%**; both constructor aliases move
**95.0% → 95.1%** source-shape, with strict fuzzy unchanged at
**59.1%**. None of these functions becomes exact. The rebuilt
`update` object uses byte load/store for the flag. Native `gof2` and
all **208/208** ARM translation units build; **93** verifier tests pass
(one skip). No separate live loot scenario was run.

The full ARM corpus compares **4,323/4,524** original functions over
**4,736** pairs, with mean **94.17% source-shape / 83.50% strict
fuzzy**, **2,562 linked-exact**, **1,082 byte-exact**, **2,728
100%-fuzzy**, **201 missing**, zero wrong-type and zero skipped.
Pairwise comparison with the preceding far-static baseline finds no
linked- or byte-exact gains or losses. Live Android asteroid spawning,
death, and loot remain unverified. The next source candidate is
`PlayerAsteroid::push`; the first-friend Pan ship-insertion gate remains
the device priority when ADB is available.
