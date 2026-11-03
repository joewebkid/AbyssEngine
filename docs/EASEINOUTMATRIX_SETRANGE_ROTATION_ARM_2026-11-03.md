# EaseInOutMatrix SetRange rotation path (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

In original `EaseInOutMatrix::SetRange(Matrix,Matrix)` at
`0x6e9d0–0x6ea46`, the method sets `m_q0` from the minimum matrix,
constructs a local identity matrix, calls `m_q0.Convert` on it,
constructs a second quaternion, and sets that quaternion from the
converted matrix. After recording the minimum position, it builds
the maximum quaternion and subtracts the reconstructed base
quaternion from it. The original then stores this difference in
`m_q1`. The rest of the method records the position delta, resets
the interpolation parameter to `0.75f`, and updates the current
matrix.

The reconstruction previously subtracted `m_q0` directly from the
maximum quaternion. That skipped the ARM-confirmed matrix round
trip and could produce a different rotation delta for inputs whose
quaternion representation changes during the round trip. The
source now follows the original call and data order. The local
matrix's 15 values match the engine's identity matrix, including
the three final `1.0f` elements.

Focused ARMv7 verification compiles 209/209 translation units.
`SetRange` source-shape improves from 83.1% to 96.0%, while strict
fuzzy changes from 43.9% to 40.9%. Both original and reconstructed
functions have 154 instructions. `SetRange` remains nonexact; the
`EaseInOutMatrix` unit retains nine linked-exact and one byte-exact
symbol. Focused report:
`_work/ease_matrix_setrange_roundtrip_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. Live rotation
interpolation on a device remains unverified; Redmi testing stays
stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
increases from 96.59% to 96.60%; mean strict fuzzy remains 86.49%.
Linked-exact remains 2,767 and byte-exact remains 1,099. Neither
comparison pairs nor exact-match sets lose entries. Full report:
`_work/ease_matrix_setrange_full_20261103.json`.

Next priority: inspect the separate `UpdateCurrentValue` blend and
translation path against ARM. Its current source-shape / strict fuzzy
is 92.5% / 46.5%; live interpolation remains untested.
