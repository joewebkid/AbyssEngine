# LOD camera and settings source recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x952c4`, `LODManager::forceUpdate` loads the canvas through the
`Globals::Canvas` relocation (`0x2101f4`) and a float at offset `0x28`
from `Globals::options` (`0x21016c`). ARM calls
`PaintCanvas::CameraGetCurrent` at `0x952f4` and
`PaintCanvas::CameraGetLocal` at `0x952fc`. The recovered code instead
read two file-local pointers initialized to null and used undefined
free-function wrappers for the camera methods. It now reads the
confirmed globals and calls the methods directly.

The original `Level::initParticleSystems` calls those camera methods
at `0xbd96c`/`0xbd974` and `0xbd9f6`/`0xbd9fe`. The recovered source
now calls the typed methods once at each pair, removing two surplus
`CameraGetCurrent` calls and the undefined `CameraGetLocal` wrapper.

Native `gof2`, AArch64 static `gof2`, and **209/209** ARMv7 translation
units build. Focused source-shape/strict fuzzy changes for
`LODManager::forceUpdate` are **92.5%/63.1% → 97.1%/87.6%**.
`Level::initParticleSystems` changes **86.1%/34.3% → 85.0%/34.4%**;
the ARM call sites support the corrected call sequence despite the
source-shape decrease. The LODManager unit retains all **10
linked-exact** symbols, and the Level unit retains all **73
linked-exact** and **34 byte-exact** symbols. Neither changed function
is exact. The latest full corpus remains **4,323/4,524** originals
over **4,743** pairs, **94.72%/84.23%** mean source-shape/strict fuzzy,
**2,604 linked-exact**, **1,083 byte-exact**, zero skips; a new full
corpus was not run for this two-file edit.

The strict ARM64 unresolved-name inventory falls **461 → 459** for the
whole archive and **221 → 219** for JNI-reachable sections. Exactly
the two obsolete camera wrapper names disappear, with no new names.
The meaning of the options field at `0x28` outside LOD updating and
live LOD behavior are unverified. `PaintCanvas::End3d` remains a large
unrecovered scene pass and is the next render priority.
