# EaseInOutMatrix clamp form (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Original `Increase(float)` at `0x6ed08–0x6ed34` multiplies the frame
delta by `0.5f`, divides by duration, adds the current value at
`+0x74`, and applies `vmin.f32` against `1.25f`. `Decrease(float)` at
`0x6ed34–0x6ed60` uses `-0.5f` and `vmax.f32` against `0.75f`.
Both store the result at `+0x74` and tail-call `UpdateCurrentValue()`.

The earlier `std::min` / `std::max` expressions were mathematically
correct, but the matching Clang 7 build took a reference to their
temporary arguments, produced a stack-protected frame, and emitted
35/34 instructions against 11/11 in the original. Direct conditional
clamps preserve the same comparisons and bounds, including a NaN
intermediate, while producing 11 instructions per function.

Focused ARMv7 verification compiles 209/209 translation units.
`Increase` rises from 21.1% source-shape / 21.7% strict fuzzy to
100% / 100%; `Decrease` rises from 21.6% / 22.2% to 100% / 100%.
The two functions are still not linked-exact or byte-exact. The
`EaseInOutMatrix` unit retains five linked-exact and one byte-exact
functions. Focused report:
`_work/ease_matrix_clamp_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. These checks do
not exercise animation on a device; live matrix interpolation remains
unverified. Redmi testing remains stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
improves from 96.53% to 96.57%, and strict fuzzy from 86.40% to
86.44%. Linked-exact remains 2,763 and byte-exact remains 1,099;
no comparison pair or exact-match set lost an entry. Full report:
`_work/ease_matrix_clamp_full_20261103.json`.

Next priority: investigate the separate default constructor's
remaining 59.4% source-shape, beginning with its ARM-confirmed
initialization order and avoiding changes made only for a score.
