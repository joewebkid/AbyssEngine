# Transform frustum call ABI on AArch64 (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Transform::InCameraVF` at `0x77478` calls the four-argument
`AbyssEngine::CameraIsSphereinViewFrustum` overload. The ARM Thumb
instructions immediately before its call at `0x7758c` set `r2=0`
for the optional matrix and `r3` to the camera pointer. The
recovered source had declared a three-argument overload with no
implementation.

The AArch64 branch now calls the existing four-argument method with
a null optional matrix. The ARMv7 call form remains unchanged. Native
and AArch64 static builds pass; ARMv7 compiles **209/209** units. A
scoped `Transform` comparison retains all **23 linked-exact** and
**5 byte-exact** functions with zero exact-set losses. Strict
JNI-initialize AArch64 unresolved names fall **10 → 9** with no new
names. The last full ARM corpus remains **4,323/4,524** originals
over **4,745** pairs, **94.79% source-shape / 84.31% strict fuzzy**,
**2,607 linked-exact / 1,083 byte-exact**, zero skips.

The source-backed ABI correction was accepted; implementing a new
three-argument method would duplicate the existing frustum logic.
Live culling remains unverified and a game `.so`/APK is not yet built.
