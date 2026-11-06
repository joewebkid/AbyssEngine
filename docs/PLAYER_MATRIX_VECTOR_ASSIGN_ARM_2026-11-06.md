# PlayerEgo matrix and vector assignment calls (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PlayerEgo::draw` calls `AEMath::Matrix::operator=` at `0xa1dca`
after `AEGeometry::getMatrix` when autopilot and turret mode are not
both active. Its stack matrix occupies 60 bytes from `sp+8` through
`sp+67`, with initial unit values at offsets `0x00`, `0x14` and
`0x38`. `PlayerEgo::setShip` calls `AEMath::Vector::operator=` at
`0x9a422` to copy the transform bounding center into `emergencyVec`.

The ARM32 reconstruction previously called undefined free
`Mat_assign` and `Vec_assign` placeholders. Worse, the draw local was
only 48 bytes and put its third unit value at offset `0x28`, although
the original matrix assignment writes 60 bytes. It now uses a real
60-byte `Matrix` initialized with the confirmed unit-value offsets
and a typed assignment. The emergency vector uses the typed vector
assignment on all architectures. The same source path is used for
Windows and ARM64.

Focused ARMv7 verification compiles **211/211** translation units.
`draw` rises from **86.4% to 88.3%** source-shape and from **53.9% to
59.7%** strict fuzzy. `setShip` stays at **98.4%** source-shape and
**83.4%** strict fuzzy. Neither function becomes exact. The raw
unmatched-symbol inventory over 211 ARM32 objects falls **231 ->
229**, removing exactly the two placeholder names. Windows native
and ARM64 static builds pass, as does strict ARM64 shared linking.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.85%** strict fuzzy,
**2,792 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. Exact-match membership is unchanged. Live draw
and emergency-system behavior, plus full ARM32 shared linking, remain
unverified.
