# PaintCanvas vibration call targets (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `PaintCanvas::HasVibration` at `0x7c2f0` and
`PaintCanvas::Vibrate(unsigned short)` at `0x7c2f6`. Both load their
`Engine *` field from `this + 0x34` and tail-branch through veneers
`0x19b318` and `0x19b328`, respectively. The veneer targets are the
PLT entries for `Engine::HasVibration` at `0x5fdcc` and
`Engine::Vibrate(unsigned short)` at `0x5fdd8`. The original engine
vibration method at `0x84552` returns immediately; the engine query
at `0x8453c` computes a Boolean value.

The reconstructed ARM32 methods previously called the undefined
`paintcanvas_ext_has_vibration` and `paintcanvas_ext_vibrate` helpers.
They now call the typed engine methods directly, forwarding the
duration. `PaintCanvas::HasVibration` now returns `bool`; this return
type is inferred from its engine target and return register because
C++ return types are absent from these mangled method names.

Focused ARMv7 verification compiles **210/210** translation units.
Both methods retain **100%** source-shape, **100%** strict fuzzy and
linked-exact membership. The raw unmatched-symbol inventory over 210
ARM32 objects falls **249 -> 247**, removing exactly the two helper
names with no new unresolved name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No linked-exact or byte-exact pair is gained or
lost relative to the preceding report. Windows native and ARM64 static
builds pass, as does strict ARM64 shared linking. Device behavior and
full ARM32 shared linking remain unverified.
