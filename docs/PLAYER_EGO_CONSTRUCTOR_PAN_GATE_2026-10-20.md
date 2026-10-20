# PlayerEgo normal-path constructor gate — 2026-10-20

The continuous original-asset Pan gate now returns from the complete
production `PlayerEgo::PlayerEgo(Player*)` body. Its test copy of
`Level::createPlayer` throws immediately after the constructor returns;
`setShip`, the rest of `createPlayer`, and `Level::init` stage 0 still do not
complete. The selected scenario has neither a black-market system nor a
supernova system/orbit, so the guarded supernova geometry path is compiled
but not executed.

Original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `Status::inBlackMarketSystem` at `0x99ec2` and writes its result to
`PlayerEgo+0x354` at `0x99ece`. That slot is `autoTurretEnabled`. The previous
C++ initialized it unconditionally to zero. The production constructor now
uses the original predicate, and the header asserts the ARM offset. The
original tail writes steering at `0x99ed2–0x99ef2`, chooses cloak recharge
from difficulty at `0x99f56–0x99f94`, and conditionally enters the supernova
effect at `0x99f98–0x9a04a`.

The gate passes at `-O0` and `-O2` on Windows SwiftShader and an Android ARMv7
device. It checks 50 MovingStars billboards, synthetic Ship-derived values,
six sound registrations, five original-AEI HUD regions, steering history,
visibility and docking defaults, the 5,000-unit default cloak recharge,
normal-path supernova fields, and **32/32** original-file lifetimes. The
inherited level path contains **68 meshes**, **72 transforms**, and **44**
station collision volumes. Native `gof2` builds, all **208/208** ARM units
compile, and **91** verifier tests pass (one existing skip).

Focused constructor source-shape/strict fuzzy changes from
**82.9%/50.8%** to **83.6%/51.7%** for both constructor aliases; neither is
exact. The 173-function `PlayerEgo` unit retains **99 linked-exact** and
**61 byte-exact** matches. The full ARM corpus compares **4,323/4,524**
original functions in **4,735** pairs at **93.95%** mean source-shape and
**83.24%** strict fuzzy, with **2,551 linked-exact**, **1,079 byte-exact**,
**201** absent originals and zero skipped units. Both exact `(unit, symbol)`
sets are identical to the preceding report. The full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/player-ego-ctor-full.json`
(SHA-256 `ed4bcdbd97e39fcdc009b5eb042282957618e845d5218faea6e2d93662854e19`).

The Item objects and Ship values in this gate are
fixtures, not claims that catalog-populated equipment, the black-market
positive case, or the supernova effect were runtime-verified. Next: execute
`setShip` and finish `Level::createPlayer`, then cover the guarded branches
with source-backed state.
