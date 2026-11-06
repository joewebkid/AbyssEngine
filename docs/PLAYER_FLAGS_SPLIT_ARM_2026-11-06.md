# PlayerEgo adjacent state bytes (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original keeps three separate state bytes at `PlayerEgo+0x354`,
`+0x355` and `+0x356`. The constructor stores the result of
`Status::inBlackMarketSystem` at `+0x354` (`0x99ece`), initializes
`+0x355` to one (`0x99a76`, after `movs r5,#1` at `0x9994a`), and
clears `+0x356` (`0x99a8a`). `setAutoTurret` and
`autoTurretIsEnabled` use `+0x355` (`0x9b2a4`, `0x9b2b4`), whereas
`isDockedToDockingPoint` and `isInDockingProcedure` read `+0x356`
(`0x9a684`, `0xa115c`).

The reconstructed layout had collapsed the first two states and
placed the docking flag one byte early. A separate `field_0x354`
now holds the market-system value; `autoTurretEnabled` and
`dockedFlag` have their confirmed ARM32 offsets. Constructor stores
are separated accordingly. In `draw`, the original gate checks
`dockedToStream` at `+0x1ee` and the high byte of `docked` at
`+0x1ed`; the source previously checked `dockingToPlanet` at
`+0x1ef` instead. The source now uses the observed byte conditions.

ARMv7 verification compiles **211/211** translation units. The
`PlayerEgo` unit compares 173 functions; seven gain linked-exact:
`setAutoTurret`, `autoTurretIsEnabled`, `isInDockingProcedure`,
`isLandingOrTakingOff`, `isDockedToMiningPlant`,
`isDockedToDockingPoint` and `isDockingToDockingPoint`. Five of these
also gain byte-exact (all except `setAutoTurret` and
`isDockedToMiningPlant`). `draw` rises from **88.3% to 88.5%**
source-shape and **59.7% to 61.2%** strict fuzzy; it remains non-exact.
Windows native and ARM64 static builds pass, as does strict ARM64
shared linking. The 211-object ARM32 unmatched-symbol inventory
remains **229** with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,799 linked-exact**, **1,113 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No previous exact-match pair is lost. The
original `PlayerEgo::update` has an additional `+0x354` market/time
gate at `0x9e11e`–`0x9e134` that remains to be reconstructed. Live
docking/autoturret behavior and full ARM32 shared linking remain
unverified.
