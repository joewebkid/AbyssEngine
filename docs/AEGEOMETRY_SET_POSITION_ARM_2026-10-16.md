# AEGeometry position setter (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `AEGeometry::setPosition(float,float,float)` at `0xa4640`
reads the canvas at object offset `+0x2c` and transform index at `+0x0c`,
calls `PaintCanvas::TransformGetLocal`, then passes its returned matrix
pointer and the three float arguments to
`AEMath::MatrixSetTranslation(Matrix&,float,float,float)`. It does not call
`TransformSetLocal`. The `Vector` overload delegates to this setter.

The previous C++ body called the declared-only `_ae_setPosition3` hook;
there was no implementation in `src`. The new body calls the typed canvas
and math methods directly. No matrix formula or transform ownership was
changed. It lives in `AEGeometryPosition.cpp`: compiling it in the same
translation unit as the `Vector` overload let Clang use a short local
branch, losing that overload's previous linked-exact match. The separate
unit preserves the external branch shape seen in the original and both
overloads remain linked-exact. An extracted-production-body test covers both,
matrix identity, argument order, absolute-position writes, and absence of
an extra `TransformSetLocal` call. Before the source fix, the expanded
test could not compile because `_ae_setPosition3` was undeclared in the
fixture; after the fix, it passes. The host math method is a recording
fixture, so this tests routing rather than production numerical behavior.

Focused ARM verification improves from **0.7% source-shape / 0.0% strict
fuzzy**, nonexact and one local instruction, to **100% / 100%**,
**linked-exact**, with all **30** original instructions represented.
It is not byte-exact. Native `gof2`, **207/207** ARM translation units,
and **40** verifier tests (one existing skip) pass.

The full corpus compares **4,323/4,524** original functions: **201
missing**, **124 extras**, zero skipped, **93.64%/82.72%** average
source-shape/strict fuzzy, **2,508 linked-exact / 1,074 byte-exact**.
Compared to the preceding report there is one linked gain, with zero
linked or byte losses. Reports:
`A:/Galaxy on Fire new version/_work/aerandom_bound_full_20261016.json`
and
`A:/Galaxy on Fire new version/_work/aegeometry_set_position_split_full_20261016.json`.
Live Android ARM32 rendering and a platform-independent runtime oracle
for the complete transform subsystem remain untested; linked-exact
shape alone does not establish end-to-end visual equivalence.
