# Legacy OpenGL capability gate (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `Engine::GlEnable` at `0x84df0–0x84e0a`, the non-shader path shifts
the requested capability right by 20, returns when the result exceeds
`0x10`, maps `0x1000000` to GL alpha test `0xbc0`, and only then calls
`glEnable` or `glDisable`. The recovered source previously omitted the
range gate, so internal shader-state capabilities such as `0x1100020`
were passed to OpenGL when shaders were disabled. `PaintCanvas` calls
`GlEnable` with `0x1100020` and `0x1100024`, so this is a reachable
behavior difference. The range gate is now restored in the non-shader
path; the shader-state bit mapping is unchanged.

Focused ARM comparison for `GlEnable` changes from **43.1% / 14.8%**
to **43.8% / 16.3%** source-shape / strict fuzzy. The 96-symbol
`engine/render/Engine` unit stays **92.6% / 79.4%**, with **43
linked-exact / 21 byte-exact** symbols. Windows and ARM64 static builds
pass; ARMv7 compiles **209/209** translation units.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs, with **95.12% mean source-shape / 84.82% mean strict
fuzzy**, **2,677 linked-exact / 1,091 byte-exact**, zero skipped and
201 missing. Complete `(unit, symbol)` comparisons against the prior
revision show no linked-exact or byte-exact gains or losses. Reports
are stored outside Git under `D:\gof2-arm64-crypto`.

The remaining large source-shape gap in `GlEnable` is the original
switch over individual shader-state bits versus the recovered lookup
table. This packet does not change that table for metric gain. Live
legacy rendering has not been tested while device work is deferred.
