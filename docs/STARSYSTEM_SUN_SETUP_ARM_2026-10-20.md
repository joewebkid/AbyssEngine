# StarSystem first sun draw and scale — 2026-10-20

## Primary ARM evidence

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `AERandom::nextInt(14)` at `0x134206–0x13420c`, obtains the current
station texture index at `0x134216–0x13421a`, tests the `0x3a31` bitmask,
and either adds 5 to the random result or loads a value from the 14-element
table at `0x202720–0x202757`. That table matches the existing C++ overrides
`{6 × 12, 4, 16}`. The Pan fixture's station texture index is zero, so it
uses the random-plus-five path.

The previous C++ used sun and streak scales of 1000/250/15/1000 in a normal
system. Original ARM instead loads `0x3e6a6000` for all three sun axes at
`0x1344ce–0x1344de` and `0x3eea6000/0x3d3b8000/0x3e6a6000` for the
streak at `0x1344fc–0x13450e`. These are respectively
`0.2288818359375f` and `{0.457763671875f, 0.0457763671875f,
0.2288818359375f}`. In a supernova system, ARM compares the campaign mission
with 106 at `0x134458`, selecting `0x3f7de800` below 106 or `0x3fafc800`
otherwise for all sun axes. Its streak is
`{0x3ffde800, 0x3e4b2000, 0x3f7de800}`. Production C++ now uses those
literal float values and mission condition.

## Executed and focused checks

The continuous original-asset Level/GLES gate passes on Windows SwiftShader
and a real Android ARMv7/Adreno device at `-O0/-O2`. It executes the real
first `nextInt(14)` after constructing the first plane geometry. From seed
2100 the ARM-derived RNG result is zero, selecting sun slot 5 for texture
index zero; the gate checks the resulting RNG state and the actual sun and
streak scales. It observes 18 meshes, 17 transforms, 44 collision volumes
and 26/26 closed file opens. The existing bounded RNG oracle has separately
checked 2,560 `nextInt(int)` transitions against an ARM-derived model; the
new gate checks this call in its real Level/StarSystem context.

Two focused tests verify the ARM branch/literal words and compile the
production scale expressions at `-O0/-O2` for normal, mission 105 and mission
106 supernova cases. Native `gof2` builds in a local C: directory, the
existing ABI/lifetime smoke passes, 208/208 ARM units compile, and 85
verifier tests pass with one pre-existing skip. The two
constructor aliases change **76.2%/13.0% → 76.3%/14.3%**
source-shape/strict fuzzy.

The full corpus retains **4,323/4,524** original functions in **4,735**
pairs, **93.92%** mean source-shape, **83.19%** strict fuzzy,
**2,551 linked-exact** and **1,079 byte-exact**. Exact `(unit, symbol)` sets
have no losses or gains against the preceding first-geometry report; skipped
units and wrong-type signatures remain zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/sun-scale-full.json`
(SHA-256 `3a61800f236b6848db5135772dc5fb1faebd0aae3cb7df7a90075f13ff959c80`).

## Limits

The original-asset gate stops before the first sun rotation and placement.
It does not execute the supernova branch or a station-texture override,
although their source expressions and ARM constants are checked separately.
The original uses a further `nextInt(4096)` before `setRotation` at
`0x134526–0x134574`; current C++ does not yet reproduce that draw or its
rotation formula. Non-sun slot rejection, full `StarSystem` lifetime, Level
stage 0 and a playable native frame remain unverified.
