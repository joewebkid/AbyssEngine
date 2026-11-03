# EaseInOutMatrix default construction (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

At `0x6e820–0x6e8b8`, the original default constructor builds the
first `Matrix`, builds `m_q0`, zeroes the position vector at `+0x4c`,
builds `m_q1`, zeroes the delta vector at `+0x68`, and builds the
current and maximum matrices. It then places two identical 15-float
identity matrices on the call stack for `SetRange` and sets duration
at `+0xf0` to zero. The last three matrix elements are all `1.0f`,
matching the engine's 15-float `Matrix` default value.

The reconstruction assigned the vectors in the constructor body,
after the other members had been built, through `Vector::operator=`.
It also default-constructed a local matrix, called `MatrixIdentity`
again, and copied that local twice. Moving the zero vectors to the
member initializer list restores their ARM-confirmed order. Passing
two direct matrix values removes the redundant local construction
and copy while keeping the same `SetRange` inputs. The earlier
redundant `MatrixIdentity` call had no value effect but produced an
extra call and stack frame.

Focused ARMv7 verification compiles 209/209 translation units.
Both constructor symbols (`C1` and `C2`) improve from 59.4%
source-shape / 19.4% strict fuzzy to 100% / 100% and become
linked-exact. They are not byte-exact. The `EaseInOutMatrix` unit
now has seven linked-exact and one byte-exact symbols. Focused
report: `_work/ease_matrix_ctor_member_init_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. Live easing and
camera motion on a device remain unverified; Redmi testing stays
stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
increases from 96.57% to 96.58%, and strict fuzzy from 86.44% to
86.47%. Linked-exact rises from 2,763 to 2,765, while byte-exact
remains 1,099. Neither comparison pairs nor exact-match sets lose
entries. Full report: `_work/ease_matrix_ctor_full_20261103.json`.

Next priority: audit the separate `EaseInOutMatrix(Matrix,Matrix,int)`
constructor's position/delta initialization and by-value argument
handling against ARM. Its current source-shape / strict fuzzy is
78.2% / 54.3%.
