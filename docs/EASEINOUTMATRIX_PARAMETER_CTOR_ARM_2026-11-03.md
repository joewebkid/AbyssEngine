# EaseInOutMatrix parameterized construction (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `EaseInOutMatrix(Matrix, Matrix, int)` constructor at
`0x6eae2–0x6ebb8` builds `m_min` and `m_q0`, zeroes the three floats
at `+0x4c`, builds `m_q1`, zeroes the three floats at `+0x68`, then
builds `m_current` and `m_max`. It copies the by-value matrix
arguments into the `SetRange` call and converts the integer duration
to float at `+0xf0`.

The earlier source assigned temporary zero vectors through
`Vector::operator=` in the constructor body, after both matrix
members were constructed. Initializing these vector members in the
initializer list restores the ARM-confirmed construction order and
avoids the temporary assignment calls. The `SetRange` argument
values and duration conversion are unchanged.

Focused ARMv7 verification compiles 209/209 translation units.
Both constructor symbols (`C1` and `C2`) improve from 78.2%
source-shape / 54.3% strict fuzzy to 100% / 100% and become
linked-exact. They remain non-byte-exact. The `EaseInOutMatrix` unit
has nine linked-exact and one byte-exact symbol. Focused report:
`_work/ease_matrix_parameter_ctor_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. Live easing and
camera motion on a device remain unverified; Redmi testing stays
stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
increases from 96.58% to 96.59%, and strict fuzzy from 86.47% to
86.49%. Linked-exact rises from 2,765 to 2,767, while byte-exact
remains 1,099. Neither comparison pairs nor exact-match sets lose
entries. Full report: `_work/ease_matrix_parameter_ctor_full_20261103.json`.

Next priority: inspect the separate `SetRange(Matrix,Matrix)`
interpolation setup and matrix value flow against ARM. Its current
source-shape / strict fuzzy is 83.1% / 43.9%.
