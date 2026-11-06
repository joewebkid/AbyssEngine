# PlayerEgo tractor-beam construction (2026-11-06)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
allocates **28 bytes** in `PlayerEgo::setShip` at `0x9a2fc`, calls
`TractorBeam::TractorBeam(AEGeometry *, int)` at `0x9a30a`, and stores
the resulting pointer at `this + 0x1b4`. The constructor target is
PLT `0x6237c`.

The reconstructed ARM32 code called an undefined `TractorBeam_new`
helper. It now constructs the typed object directly. The generated
ARM32 object contains the same 28-byte allocation, constructor call,
and pointer store sequence. This restores the construction path used
when a ship has tractor-beam equipment.

Focused ARMv7 verification compiles **211/211** translation units.
`PlayerEgo::setShip` rises from **97.8% to 98.4%** source-shape and
from **79.2% to 83.4%** strict fuzzy; it is still not exact. The raw
unmatched-symbol inventory over 211 ARM32 objects falls **238 -> 237**,
removing only `_Z15TractorBeam_newPvi` with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match pair is gained or lost relative to
the preceding report. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. Ship equipment behavior on a device
and full ARM32 shared linking remain unverified.
