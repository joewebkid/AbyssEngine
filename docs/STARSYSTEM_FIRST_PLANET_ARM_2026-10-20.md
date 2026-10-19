# StarSystem first non-selected planet — 2026-10-20

## Primary Android ARM evidence

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
initializes 24 four-byte occupancy entries after `setSeed` at
`0x1340bc–0x1340cc`. The non-selected station path constructs a real
`PlayerStatic`, draws `nextInt(11)+7`, and repeats when the candidate is
within two slots of the sun or already occupied (`0x13425e–0x134284`).
It then draws `nextInt(40)` for a three-axis scale, with double arithmetic
and literal values `0.01`, double-converted `0.8f`, and
`0.03509521484375` (`0x134286–0x1342be`, literals at `0x134418–0x13442f`).
The accepted slot is marked occupied at `0x13436a–0x134372`.

For the selected station, ARM takes a separate path at
`0x134186–0x1341a0`: it retains the preceding orbit slot, draws
`nextInt(20000)+20000`, halves it for campaign mission zero, then selects
the ring-orbit fixed distance or texture-index-dependent ranges
(`0x1341e4–0x134202`, `0x1342f4–0x13433c`, `0x134626–0x134648`).
Only this branch writes `planetScale`. Selected planets set yaw without a
new pitch draw (`0x134374–0x1343f4`); non-selected planets draw
`nextInt(4096)-2048` for pitch (`0x134522–0x134574`). After the common
`moveForward(-20000.0f)`, ARM may rotate a further half unit of the
fixed-point sun scale (`0x13458e–0x1345ba`). The original ARM anchors are
guarded by `test_starsystem_planet_slot_arm.py`.

## Executed boundary

The continuous original-asset Level/GLES gate reaches the first non-selected
planet from the Pan fixture at `-O0/-O2` on Windows SwiftShader and a real
Android ARMv7/Adreno device. It runs the actual `PlayerStatic` constructor,
reuses the loaded `plane.aem` mesh, and checks the slot-dependent pose,
scale, position, stored position, and RNG state. With seed 2100 the first
candidate is slot 15, size jitter 31, and pitch jitter -1744; the gate
observes 18 meshes, 18 transforms, 44 collision volumes, and 26/26 closed
file opens. Native `gof2`, ABI/lifetime smoke, 208/208 ARM units, and 87
verifier tests (one pre-existing skip) pass. Focused constructor aliases
change **78.1%/13.3% → 85.2%/19.2%** source-shape/strict fuzzy and remain
nonexact. The focused report is ignored at
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/first-planet-final-unit.json`.

The complete ARM corpus compares **4,323/4,524** originals in **4,735
pairs**, at **93.92%** mean source-shape and **83.20%** strict fuzzy (up
from 83.19%). It retains **2,551 linked-exact** and **1,079 byte-exact**
pairs. Independent `(unit, symbol)` set comparisons against the preceding
sun-pose report find no lost or gained exact pairs. Skipped units and
wrong-type signatures remain zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/first-planet-full.json`
(SHA-256 `aa1c18f1d31a2896618e2ba6a6498fb97bed7bd2f6d9754b3bc37918604872be`).

## Limits

The Pan draw accepts slot 15 on its first attempt, so a rejected candidate
has not yet been observed end-to-end. The selected-station branch and its
ring/texture distance overrides are source-backed by ARM but have not been
executed by this gate. Subsequent planets, complete StarSystem lifetime,
Level stage 0, and a playable native frame remain unverified.
