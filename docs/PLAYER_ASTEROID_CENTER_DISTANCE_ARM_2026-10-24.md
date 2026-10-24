# PlayerAsteroid center-distance integer storage (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `PlayerAsteroid::setAsteroidCenter` at `0xe2a3c`, ARM copies the
incoming vector to static `asteroidCenter`, calls `VectorLength` at
`0xe2a60`, converts the returned float to a signed 32-bit integer with
`vcvt.s32.f32` at `0xe2a6a`, then stores those integer bits in static
`asteroidDistance` at `0xe2a72`. The earlier C++ declared that static as
`float` and stored the unconverted length. It now declares `int` and
performs the observed conversion; the Pan gate's extracted static definition
uses the same type.

Focused ARM source-shape/strict fuzzy improves **96.2%/84.4% →
100%/100%**. All 33 instructions match after normalization, yielding a new
linked-exact but not byte-exact function. A source-extracted host smoke at
`-O0/-O2` checks both the copied center and truncation of a nonintegral
length. Native `gof2`, all **208/208** ARM translation units and **93**
verifier tests pass (one skip).

The full ARM corpus compares **4,323/4,524** original functions across
**4,736** pairs, averaging **94.19% source-shape / 83.53% strict fuzzy**,
with **2,564 linked-exact**, **1,082 byte-exact**, **2,730 100%-fuzzy**,
**201 missing**, zero wrong-type and zero skipped. Pairwise exact-set
comparison with the preceding quality-string report finds exactly this one
linked-exact gain, no linked-exact loss and no byte-exact change.

The original-asset Pan gate was attempted on Windows. ANGLE lacked ETC1;
SwiftShader reached stage one but stopped at an earlier HUD image-cache
assertion before `setAsteroidCenter`. Thus it does not validate this change
in the continuous scene. ADB currently lists no device. Original downstream
reads of the static distance and live behavior remain unverified. Next:
resolve the earlier HUD gate failure, then continue the first-friend Pan ship
insertion and test on device when available.
