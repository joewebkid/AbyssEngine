# KIPlayer and SpaceLounge matrix call recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Five recovered call sites used
`AEMath::MatrixMultiply(Matrix &, const Matrix &)`, a declaration with
no implementation. The similarly named existing two-const helper
calculates a product and discards it. The original ARM instructions
instead call the mutating `AEMath::Matrix::operator*=` at:

- `0xa5972` in `KIPlayer::KIPlayer`
- `0xa5dc0` in `KIPlayer::setShipGroup`
- `0xa638c` in `KIPlayer::setPosition(const Vector &)`
- `0x1706b2` in `SpaceLounge::updateScreenPositions`
- `0x170a42` in `SpaceLounge::update`

Each caller now applies `operator*=` to the same destination matrix and
same right-hand operand. This restores the parent ship transform and
the lounge camera transform; it removes one unique unresolved AArch64
symbol. No substitute arithmetic or metric-only branch was added.

Focused ARMv7 source-shape/strict fuzzy for the `KIPlayer` constructor
aliases rises **91.3%/82.4% → 91.4%/83.5%**. `setShipGroup` rises
**95.0%/93.5% → 100%/100%** and becomes linked-exact. The vector
`setPosition` rises **86.6%/81.0% → 100%/100%** and becomes
linked-exact. `SpaceLounge::updateScreenPositions` stays
**32.1%/13.5%**, and `SpaceLounge::update` stays **70.8%/9.5%**;
both remain nonexact because of larger unrecovered bodies.

Native `gof2`, AArch64 static `gof2`, and the focused ARMv7 units build.
The strict AArch64 whole-archive link inventory falls **545 → 544**
unique unresolved names, with no new names. It still cannot produce a
game shared library. Live ship and lounge transforms remain unverified.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs with zero skips. Mean source-shape and
strict fuzzy remain **94.72%/84.21%** after report rounding. The
linked-exact set rises **2,601 → 2,603**, comprising the two
`KIPlayer` gains above; byte-exact remains **1,083**. Symbol-level
comparison finds zero losses in either exact set.
