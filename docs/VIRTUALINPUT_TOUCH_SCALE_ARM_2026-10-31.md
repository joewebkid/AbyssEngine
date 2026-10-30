# Pointer scale in simulateTouch (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `simulateTouch` (entry `0x6aaf0`), the pointer normalization block
at `0x6af7c–0x6afe6` first computes `abs(easedX)`, keeps the larger
of that value and **500.0**, converts the result to an integer, then
compares it with `abs(easedY)` to select an integer denominator. The
horizontal offset is divided by that denominator; the vertical
offset is divided by half of it. Both normalized values are clamped
to [-1, 1] before easing.

The previous source used `min(abs(easedX), 500.0)` and a float
denominator. With ordinary stick values near 0.5, it scaled pointer
offsets by roughly 0.5 instead of 500, saturating small movements.
The source now follows the original maximum, integer conversion and
comparison order. This is a behavior correction supported by the ARM
instructions, not solely a code-shape adjustment.

Focused `simulateTouch` improves **83.7% → 84.4% source-shape** and
**38.7% → 42.2% strict fuzzy**. The 20-symbol focused input unit
remains **85.5% mean source-shape** after rounding and rises **74.1%
→ 74.3% mean strict fuzzy**; it retains nine linked-exact and two
byte-exact matches. Native Windows and ARM64 static builds pass;
ARMv7 compiles 209/209 translation units. Live pointer behavior
remains unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,749**
pairs: **95.12% mean source-shape**, **84.82% mean strict fuzzy**,
**2,677 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Both rounded means and exact counts are unchanged from the
preceding revision. Complete `(unit, symbol)` exact-set comparison
finds no gains or losses. Reports are stored outside Git under
`D:\gof2-arm64-crypto`.
