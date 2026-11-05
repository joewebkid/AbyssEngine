# PaintCanvas sensor forwarding (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `PaintCanvas::GetGravValue` at `0x7e7bc` and
`PaintCanvas::GetAccelValue` at `0x7e7c2`. Both load their `Engine *`
field from `this + 0x34` and tail-branch through veneers `0x19b418`
and `0x19b428`. Their targets are the PLT entries for
`Engine::GetGravValue` at `0x5fb38` and `Engine::GetAccelValue` at
`0x5ff88`. Both engine methods return a `double *` in `r0`.

The reconstructed ARM32 methods previously called undefined
`paintcanvas_ext_get_grav` and `paintcanvas_ext_get_accel` helpers.
They now call the corresponding typed `Engine` methods and return
their pointer results. The `PaintCanvas` return types are inferred
from those targets and the preserved return register; they are not
encoded in the mangled method names.

Focused ARMv7 verification compiles **211/211** translation units.
Both methods retain **100%** source-shape, **100%** strict fuzzy and
linked-exact membership. The raw unmatched-symbol inventory over 211
ARM32 objects falls **241 -> 239**, removing exactly the two helper
names and adding none.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match pair is gained or lost relative to
the preceding report. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. Device sensor behavior and full
ARM32 shared linking remain unverified.
