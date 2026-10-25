# StarSystem render matrix ABI (2026-10-25)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `StarSystem::render` starts at `0x135104`. Its nine
matrix calls branch to the exported `AbyssEngine::AEMath` functions:

| Original call addresses | Target |
| --- | --- |
| `0x13513e` | `MatrixGetPosition(const Matrix&)` |
| `0x1351e2`, `0x1352ba` | `MatrixGetUp(const Matrix&)` |
| `0x1351f2`, `0x1352ca` | `MatrixGetLookAt(const Vector&, const Vector&, const Vector&)` |
| `0x13526a`, `0x1352fa`, `0x135344`, `0x13540a` | `MatrixSetScaling(Matrix&, float, float, float)` |

The recovered source instead declared four unrelated global functions,
including a `void MatrixSetScaling(Matrix*, ...)` signature that
omitted the original struct return. All call sites now use the existing
`AEMath` declarations. The updated ARMv7 object has nine relocations to
the same mangled targets as the original calls, preserving the hidden
return argument where applicable.

The planet-ring branch had a separate wrong-local bug. At
`0x1353ea–0x135412`, the original copies `AEGeometry::getMatrix()` into
the same local matrix that it then scales by four and passes to
`AEGeometry::setMatrix()`. The recovered code copied into
`savedCamera` but scaled and passed `lookAt`. It now copies directly
into `lookAt`, so the ring transformation uses the planet matrix.

The focused `StarSystem::render` source-shape/strict fuzzy scores change
from **87.9%/19.6%** to **87.3%/19.4%**, still nonexact. The score change
is accepted because the corrected call destination, ABI and ring input are
directly observed in the original binary. The 36-function focused unit
has no linked-exact or byte-exact losses. Native `gof2`, the AArch64
static archive and **209/209** ARMv7 translation units build. Strict
AArch64 whole-archive unresolved symbols fall **576 → 574**, eliminating
the global `MatrixGetPosition(Vector*, void*)` and
`MatrixSetScaling(Matrix*, float, float, float)` dependencies. Other
callers still require two of the previous global matrix helpers.

The complete ARM corpus compares **4,743** symbol pairs and covers
**4,323/4,524** original functions; **201** are missing and none were
skipped. Mean source-shape is **94.71%**, strict fuzzy **84.21%**,
linked-exact **2,601** and byte-exact **1,083**. The linked-exact and
byte-exact symbol sets have no losses or gains against the preceding
Engine projection baseline.

Live sun, streak and planet-ring rendering is unverified. The game
shared library and APK are not yet linkable or runnable. Next priority
is the similarly misbound matrix and vector calls in weapon update
paths, followed by device rendering checks when Android startup works.
