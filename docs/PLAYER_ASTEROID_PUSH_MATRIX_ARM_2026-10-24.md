# PlayerAsteroid push matrix construction (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PlayerAsteroid::push(int)` at `0xe2bd8` initializes one 15-float
matrix directly at `0xe2c04–0xe2c44`. The literal words at
`0xe2d08–0xe2d14` confirm ones at indices 10, 12 and 13; the ARM
stores set indices 0, 5 and 14 to one and zero the others. At
`0xe2c7c–0xe2c90`, `AEGeometry::getMatrix()` returns a reference passed
directly as the left operand of matrix multiplication; the rotated
local matrix is the right operand.

The prior C++ constructed an unused `identity`, assigned a second
newly constructed matrix to it, separately default-constructed
`rotation`, and copied the geometry matrix before multiplication. The
recovered method initializes only the ARM-observed matrix and passes
the geometry reference directly. This preserves the original
geometry-times-rotation order and removes extra construction and
copying. The timer, last-frame-delta condition and translation formula
remain unchanged. A trial that removed the local geometry pointer
produced a worse ARM shape and no demonstrated behavior gain; it was
rejected.

Focused NDK r18b ARM `-Oz` source-shape/strict fuzzy improves
**83.6%/44.9% → 97.8%/77.8%**; `push` remains nonexact. A
source-extracted host smoke at `-O0/-O2` checks the 15 matrix values,
operand order, timer update, positive/zero/negative frame-delta paths
and resulting movement. It is not a live Android physics test.

Native `gof2` and all **208/208** ARM translation units build; **93**
verifier tests pass (one skip). The full corpus compares **4,323/4,524**
original functions over **4,736** pairs, with mean **94.17%
source-shape / 83.51% strict fuzzy**, **2,562 linked-exact**,
**1,082 byte-exact**, **2,728 100%-fuzzy**, **201 missing**, zero
wrong-type and zero skipped. Pairwise comparison with the preceding
loot-flag baseline finds no linked- or byte-exact gains or losses.
ADB lists no device; live asteroid motion remains unverified.

The next separate source candidate is `PlayerAsteroid::getProjectionVector`:
original ARM at `0xe30c8` subtracts into the class-static `pos`, while
the current C++ subtracts into a local vector and leaves `pos`
unchanged after assigning the geometry position. This needs its own
runtime and corpus gates; first-friend Pan ship insertion remains the
device priority when ADB is available.
