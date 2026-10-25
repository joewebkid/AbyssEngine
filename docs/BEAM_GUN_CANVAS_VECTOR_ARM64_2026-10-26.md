# BeamGun canvas and vector call recovery (2026-10-26)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BeamGun::BeamGun` starts at `0x177754`, and `BeamGun::update` at
`0x1778a4`.

The original constructor and update read `Globals::Canvas` through
the GOT relocation at `0x2101f4`. The recovered source instead
dereferenced a local `BeamGun_canvas` pointer initialized to null.
Both paths now use `Globals::Canvas`. Calls through the undefined
`BeamGun_canvasTransform` wrapper now use the original
`PaintCanvas::TransformGetTransform` target, visible at `0x1778e4`,
`0x1778f8`, `0x17791a`, and the secondary-effect calls later in
`update`.

The original `update` calls `AEMath::MatrixRotateVector` at
`0x1779c8` and `0x177a34`, then uses `Vector::operator+=` at
`0x1779d0` and `0x177a3c` to advance the respective positions.
It calls `AEMath::MatrixGetDir` at `0x177a6e` for the secondary
effect's direction. The code now uses those targets directly. The
previous secondary path copied a 12-byte `Vector` through a 60-byte
`Matrix` cast before adding the rotated offset; it now adds the
offset to the actual player-position vector. The camera matrix comes
from the `Player` transform at byte offset 4, matching the ARM
`add r1, r0, #4` before both math calls.

Focused `update` source-shape/strict fuzzy changes **87.5%/50.2% →
94.3%/50.2%**. Both constructor aliases change **89.5%/71.1% →
90.9%/60.8%**. None is exact; the constructor fuzzy decrease is
accepted because the original canvas symbol is identified by its
relocation and the old source dereferenced null. The 11-function
focused unit retains **8 linked-exact** and **2 byte-exact** symbols.
Native, AArch64 static, and **209/209** ARMv7 translation units
build. A strict AArch64 whole-archive link probe falls from **548**
to **545** unique unresolved names, with no new names. It still
fails; no game `.so` or APK was produced.

The full ARMv7 corpus compares **4,323/4,524** original functions
over **4,743** symbol/unit pairs with zero skips. Mean source-shape
is **94.72%**, up from **94.71%**; strict fuzzy remains **84.21%**.
The **2,601 linked-exact** and **1,083 byte-exact** symbol sets have
zero gains and zero losses versus the preceding full report.

The constructor's `BeamGun_secondaryMeshes` array remains a
zero-filled placeholder. The original reads a resource-ID table from
`.rodata` at `0x2068f4`, including many `-1` entries and nonzero
mesh IDs. Therefore secondary-effect selection is not yet faithful.
That table needs a separate source/asset provenance decision and
validation before inclusion. Live beam rendering and animation have
not been checked on Android. The next priority is the remaining
no-op matrix-multiplication callers and the Android runtime host.
