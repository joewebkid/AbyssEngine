# Globals composite ship groups 13/14 — Android ARM and Pan host (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` SHA256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`
has `Globals::getShipGroup` at `0xe4e14`. Its special type-13 branch
(`0xe4e42–0xe4ed0`) constructs base mesh **17013**, attaches **18713**
and **17038**, installs LOD **17139/17140** at 35,000/45,000, then
LOD-child meshes **17040/17041**. The type-14 branch
(`0xe505a–0xe50f4`) constructs base **14311**, attaches **14316/14315**,
installs LOD **14312/14313** at 35,000/60,000, uses **14316/14316**
as LOD children and scales all three axes by **2.0**. Both branches
call `TransformAddMesh` with `false`. The paired LOD and child values
are also present as little-endian literals at `0xe51e4–0xe51e8` and
`0xe5200–0xe5204`.

The previous source passed mesh ID zero to both child transforms,
supplied zero LOD IDs, and passed a one-element child array to a
two-LOD reader. In the original-asset Pan host gate, type 14 crashed
while trying to load mesh zero. Restoring the ARM IDs removes that
failure and fixes the out-of-bounds child-array read. The source-
extracted non-alternate large-outpost path now passes Windows
SwiftShader `-O0/-O2` through `createShip(0, 1, 14)`, 11 bounds,
wreck mesh **18304**, moving=false and final relocation. The gate also
constructs type 13 separately from original assets and checks both
LOD and child arrays. It does **not** run the later seven turret slots,
the alternate large-outpost construction, the small outpost or live
Android.

Focused ARM `getShipGroup` source-shape improves **83.0% → 83.3%**;
strict fuzzy improves **7.7% → 11.3%**. This 694-instruction original
function remains nonexact. Native `gof2`, **208/208** ARM translation
units and **93** verifier tests (one skip) pass. The full-corpus result
is **4,323/4,524** originals across **4,736** pairs, with mean
**94.19% source-shape / 83.54% strict fuzzy**, **2,731 100%-fuzzy**,
**2,564 linked-exact** and **1,082 byte-exact**. Compared symbol identities
are unchanged and pairwise exact-set losses are zero; **201** originals
remain absent, with zero wrong-type or skipped. The Pan fixture still forces one
non-alternate large outpost after preserving the selection RNG draws
and uses a test-only model of the renderer's opaque
`TransformRemoveChild` helpers.
