# PaintCanvas perspective depth coefficient (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers `PaintCanvas::SetProjectionMatrix3d` at `0x7b808`.

At `0x7b910–0x7b940`, the original converts `farPlane` and
`nearPlane` from float to double, computes `double(farPlane) +
double(farPlane)`, multiplies by `double(nearPlane)`, divides by the
double conversion of the float difference `nearPlane - farPlane`,
then converts back to float for matrix element 14. Matrix element 10
continues to use float arithmetic. The reconstructed source had
calculated element 14 entirely in float. It now follows the original
precision and operation order. This also matches the separate
`ResetPersMatrix` path verified in the preceding pass.

Focused ARMv7 verification compiles **211/211** translation units.
Target source-shape rises **95.3% -> 96.7%** and strict fuzzy
**52.6% -> 56.9%**; the function remains non-exact. Windows native
and ARM64 static builds pass, as does strict ARM64 shared linking.
The raw 211-object ARM32 unmatched-symbol inventory stays **205**,
with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape remains **96.82%**, strict fuzzy
**86.89%**, **2,798 linked-exact**, **1,113 byte-exact**, 201 missing,
zero skipped, zero wrong-type, and 149 extra symbols. Exact-match
membership sets are unchanged. These rounded means do not show the
improvement in this one function.

Live perspective rendering and full ARM32 shared linking remain
unverified. Next priority: restore an unresolved gameplay call from
static ARM evidence; validate
projection on a device when available.
