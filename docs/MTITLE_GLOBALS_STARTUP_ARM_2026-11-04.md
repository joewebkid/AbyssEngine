# MTitle startup globals and release (2026-11-04)

The original Android ARMv7 `MTitle` methods were inspected in REA:
`OnInitialize` (`0x979e2`, evidence
`ev_57b1bef91eae9957d8da2953ffe51247959cb8958c359a7d60f0f34b0b5a07bb`),
`OnRelease` (`0x97a38`, evidence
`ev_aa6857b5f04723c0d02dfdf87a3656407f8fbd3d205225908b70eafcc2d2e60d`),
`OnRender2D` (evidence
`ev_b355d6b6fc7227a8b28f991f36652260d27e405049a7b1cebbdac92a67a74d70`),
and `OnRender3D` (evidence
`ev_396f8e18b2fb0f12c6c3788d3be82550b683503835c6d70715c1e122b4dc1b24`).

The ARM32 source used null static pointers for the current layout,
image factory, and sound; the ARM64 branch used `Globals` instead.
The original ARM32 methods load `Globals::Canvas`, `Globals::sound`,
`Globals::layout`, `Globals::imageFactory`, and `Globals::appManager`.
The recovered source now uses those roots on both architectures.
The release sequence also restores `Layout::initTip()` after layout
and image-factory reload. Rendering uses the module's canvas for the
begin/end operations and `Globals::Canvas` for the observed global
operations. The elapsed-time path follows the original pair of reads
when the first sample is at most 50 ms.

Focused ARMv7 verification compiled 209/209 translation units. For
the four modified methods, mean source-shape improved from **81.8%**
to **96.4%**; strict fuzzy improved from **68.6%** to **86.5%**.
`OnRelease` and `OnRender3D` became linked-exact; `OnInitialize`
reached 96.9% source-shape and `OnRender2D` reached 88.7%.
Windows native and ARM64 static builds pass. Strict ARM64 shared
linking has zero unresolved symbols and retains 53 JNI exports.
Full ARMv7 verification compares **4,323/4,524** original functions
over **4,770** pairs: mean source-shape **96.78%** (previously 96.77%),
strict fuzzy **86.77%** (previously 86.76%), **2,782 linked-exact**
(previously 2,780), and **1,108 byte-exact**. There are 201 missing
functions, zero skipped, and zero wrong-type matches. No comparison
pair or exact-match member was lost.

The signed clone APK at
`D:/gof2-arm64-crypto/gof2-arm64-main-mtitle-debug-20261104.apk`
has SHA-256
`5ea01a0e2251e44b8a7a42e0e5d9ef4cd766f38de1df61f47c8b40a27dc3068b`.
Its v1/v2/v3 signatures and embedded ARM64 library hash were checked.
It has **not** been installed or run; startup and title rendering
remain unverified on a device.
