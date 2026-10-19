# StarSystem first geometry before RNG draw — 2026-10-20

## Primary evidence and correction

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `AERandom::setSeed` at `0x1340b8`, constructs the first `AEGeometry`
at `0x13410e–0x134122`, stores it in `planetsArray[0]` at `0x134128`, and
only then calls `AERandom::nextInt(14)` at `0x134206–0x13420c`. The previous
C++ drew the sun slot before entering the geometry loop. The source now draws
inside the first iteration, after constructing and publishing its geometry.
The existing override for eligible station textures remains after the draw.

## Executed boundary

The continuous original-asset Level/GLES gate passes on Windows SwiftShader
and a real Android ARMv7/Adreno device at `-O0/-O2`. After all five station
textures, station-record release and `setSeed(2100)`, it executes the real
`AEGeometry` constructor. Resource 6768 names
`data/assets/main/3d/meshes/misc/plane.aem` with material ID 65535; the
original file is 127 bytes (SHA-256
`069a619eefd6a7e4501bc2739f5e231a99f8724643b3fd5bae07a6fb67132a30`).
The gate checks the published geometry, leaves the other five planet entries
empty, and checks that the RNG retains its post-seed low/high state before the
draw. It observes 18 meshes, 16 transforms, 44 collision volumes and 26/26
closed file opens. A regression to the previous draw-before-construction
order would fail the RNG-state assertion at this boundary.

The native `gof2` build, ABI/lifetime smoke and 208/208 ARM compilation pass;
83 verifier tests pass with one pre-existing skip. Focused constructor aliases
change **76.7%/18.4% → 76.2%/13.0%** source-shape/strict fuzzy. This local
matching decline is retained because the original call order and the
original-asset execution independently support the behavior correction.

The full corpus retains **4,323/4,524** originals in **4,735** pairs and
**93.92%** mean source-shape. Strict fuzzy changes **83.20% → 83.19%**;
**2,551 linked-exact** and **1,079 byte-exact** remain. Comparing exact
`(unit, symbol)` sets to the preceding seed report shows no gains or losses;
there are zero skipped units and zero wrong-type signatures. The ignored full
report is `C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/geometry-full.json`
(SHA-256 `5a6a2fcbce57b891a4e24280dc89af7e4c0c436b40922189215f8cf546a762ae`).

## Limit

The gate stops **before** `nextInt(14)`. It does not validate that draw's
value, the texture-index override, sun scaling, subsequent non-sun placement,
complete `StarSystem` lifetime, Level stage 0, or a playable native frame.
The original also checks a post-effect condition between first-geometry
publication and the draw (`0x13412c–0x134166`); that path is not yet
reconstructed in current C++. The non-sun ARM path includes a slot-rejection
loop at `0x13425e–0x134286`, unlike the current single draw. These are next
priorities, not claims established by this bounded gate.
