# Pan light and StarSystem constructor return — 2026-10-20

The continuous original-asset Level/GLES gate now executes the production
`StarSystem::initLight` body for Pan (system texture 2, station 7) through
all Engine light setters and `PaintCanvas::FogEnable`, then observes the
real `StarSystem` constructor returning to `Level::createSpace`. The host
stops immediately after the `new StarSystem(mode)` assignment; Level
stage 0 itself has not returned.

The gate checks material RGB/alpha and shininess, global scene ambient,
two active light slots, first-light ambient/diffuse/specular, second-light
station diffuse color, particle ambient, the five fixed-function light
color calls, and the no-fog Pan branch. It also checks the constructed
six planet/position/texture slots, five station indices, flare/streak,
18 meshes, 22 transforms, 44 collision volumes and 26/26 closed original
asset reads. Windows SwiftShader and real Android ARMv7/Adreno pass at
`-O0` and `-O2`.

The executed Engine methods and `FogEnable` are extracted from production
source. Only GLES1 compatibility endpoints, which are inert in the original
binary, remain bounded host services; shader update functions and fog GL
transitions fail closed. This gate covers the fixed-function light path and
Pan's no-fog branch, not shader-enabled propagation or fog-color variants.

No production source changed in this checkpoint. The previously completed
full ARM comparison for this same source revision remains **4,323/4,524**
original functions and **4,735 pairs**, **93.93%** mean source-shape,
**83.22%** strict fuzzy, **2,551 linked-exact** and **1,079 byte-exact**,
with zero skipped units or wrong-type signatures. Its report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-ambient-full.json`
(SHA-256 `53beb4739cda71c4412ce6f9d411a979db413be872dd444527b8e43fea3c832f`).
No exact-pair regression is introduced by this fixture-only extension.

Next: verify the shader-enabled light path, fog-system variants and the
remaining Level stage-0 work. Constructor return for other systems is not
established by this Pan fixture.
