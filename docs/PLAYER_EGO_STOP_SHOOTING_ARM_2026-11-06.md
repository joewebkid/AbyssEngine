# PlayerEgo stop-shooting dispatch (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers `PlayerEgo::stopShooting(int)` at `0x9adf0`.

Original ARM branches on the turret-active byte at `+0x1a0`. When
active, it calls `Player::stopShooting(int)` with slot 2. Otherwise,
it returns if the ego is dead. For parameter 1, it calls
`Player::stopShooting(int, int)` with slot 1 and the current
secondary-weapon index at `+0x10c`; all other values call the
single-argument overload with the parameter. Tail veneers
`0x19ba48` and `0x19ba58` resolve to the corresponding PLT entries
`0x62484` and `0x624c0`.

The reconstructed control flow was already equivalent, but its
ARM32 build called two undefined stand-ins. Direct typed calls now
resolve to the original `Player` overloads on both ARM32 and ARM64.
The two stand-in declarations are removed.

Focused ARMv7 verification compiles **211/211** translation units.
The target retains **100.0%** source-shape, **95.8%** strict fuzzy,
and 24 instructions versus 24 original; it remains non-exact.
Windows native and ARM64 static builds pass, as does strict ARM64
shared linking. The raw 211-object ARM32 unmatched-symbol inventory
falls **191 -> 189** with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape **96.83%**, strict fuzzy
**86.93%**, **2,798 linked-exact**, **1,113 byte-exact**, 201 missing,
zero skipped, zero wrong-type, and 152 extra symbols. Both complete
exact-match membership sets are unchanged.

Live weapon-stop behavior and full ARM32 shared linking remain
unverified. Next priority: inspect the remaining gameplay-specific
ARM32 placeholders, including the division call in `PlayerEgo` and
the UI sine call, against original ARM targets.
