# StarSystem station seed width — 2026-10-20

## Evidence and correction

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `StarSystem::StarSystem(int)` at `0x1340a4–0x1340b8`, the original calls
`Status::getStation`, `Station::getIndex`, multiplies the returned 32-bit
index by 300 with `mul.w r2, r0, r1`, then sign-extends the low 32 bits with
`asrs r3, r2, #31` for `AERandom::setSeed(long long)`. The previous C++ cast
to `long long` before multiplying and its ARM object used `smull`, retaining
a full 64-bit product. The corrected expression uses unsigned 32-bit
multiplication followed by signed 32-bit conversion; the rebuilt ARM object
now emits `mul.w` and `asrs` at the call site.

This is a real input-contract correction for large or negative station
indices. The original Pan fixture has station index 7, so both expressions
produce 2100 there; that asset alone cannot distinguish them. The focused
source-extraction test evaluates the production expression at 7, -1,
7,158,279, -7,158,279, `INT_MIN` and `INT_MAX`, using the actual
`AERandom::setSeed` implementation at `-O0/-O2`. The old expression fails
the negative control in both modes. A separate test asserts the four
decisive original ARM instructions and call target.

## Continuous boundary and limits

The existing original-asset Level/GLES gate now continues past the five
station textures, record cleanup and target-array construction, executes
the real `AERandom::setSeed(2100)`, checks its low/high state, and stops
before the first `nextInt(14)` draw in the current C++. Windows SwiftShader and a real Android
ARMv7/Adreno device pass at `-O0/-O2`. This does **not** establish the
first random choice, planet geometry/placement, complete `StarSystem`
construction or lifetime, Level stage 0, or a playable native frame.
Original ARM allocates the first `AEGeometry` at `0x13410e–0x134128`
before its `nextInt(14)` call at `0x134206–0x13420c`; the current C++ draws
before that allocation. This order and the later non-sun rejection/scale
logic are separate open reconstruction work, not covered by this seed gate.

Native `gof2` builds in both the existing worktree build and a fresh local C:
build directory. The first in-worktree archive attempt briefly failed for
lack of A: space; a later retry passed without deleting repository files.
The ARM build compiles 208/208 units in
a local C: build directory. The verifier test suite runs 83 tests with one
pre-existing skip and no failures. Both constructor aliases change from
**76.6% to 76.7%** source-shape; strict fuzzy stays **18.4%**. The full
corpus retains **4,323/4,524** original functions in **4,735** pairs,
**93.92%** mean source-shape, **83.20%** strict fuzzy, **2,551 linked-exact**
and **1,079 byte-exact**. Compared by `(unit, symbol)` with the preceding
integrated report, no linked- or byte-exact pair is gained or lost. The
ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/seed-after-full.json`
(SHA-256 `5f10a3fa2e5c2d9f09cfe59adeddabda0dc0b484c1859af837981c25cc4e281c`).

Next bounded priority: inspect the first RNG draw and the following planet
creation/placement against original ARM, then extend the continuous gate
without treating the seed check as completion of stage 0.
