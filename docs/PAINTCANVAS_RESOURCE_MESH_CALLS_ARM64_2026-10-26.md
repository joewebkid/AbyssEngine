# PaintCanvas resource and mesh call recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump identifies direct `FindResource` and
`MeshChangeMaterialIntern` calls in `MeshChangeResourceMaterial`
(`0x7a510`) and `MeshChangeMaterial` (`0x7a540`).
`TransformRemoveMesh` (`0x7b178`) calls `FindResource` and then
`TransformRemoveMeshId`; `TransformRemoveChild` (`0x7b34c`) removes
the child from its parent's `Array<Transform *>` and calls the
parent's `CollectAnimationData`.

The recovered source now uses those existing methods and the
`ArrayRemove<Transform *>` specialization. Resource lookup in
`MaterialResourceChangeTexture` also uses the typed `FindResource`
method with a 16-bit resource ID. The child-removal change restores
animation-data refresh after unlinking. A direct typed material call
on ARMv7 lost two pre-existing linked-exact matches, so the existing
ARMv7 call form is retained while AArch64 uses the typed method.
Without an `extern template`, the compiler emitted a duplicate
`ArrayRemove<Transform *>` body; the declaration now selects the
specialization already defined in `Array.cpp`.

Focused ARM verification compares **198** `PaintCanvas` pairs before
and after: **96.2% mean source-shape / 83.1% strict fuzzy**,
**94 linked-exact / 23 byte-exact**. There are no exact-set losses.
For `MaterialResourceChangeTexture`, source-shape rises from **96.4%
to 98.2%**, while strict fuzzy falls from **86.4% to 84.4%**.
The full ARM corpus compares **4,323/4,524** original functions over
**4,745** pairs: **94.79% source-shape / 84.29% strict fuzzy**,
**2,607 linked-exact / 1,083 byte-exact**, zero skips. Neither exact
set loses or gains a function against the preceding full report.
Native `gof2` and AArch64 `gof2` static-library builds pass.

The strict JNI-initialize AArch64 link probe first drops from **96 to
91** unresolved names. It removes six old wrapper names and exposes
the previously hidden `paintcanvas_ext_remove_meshid` inside the now
reachable typed `TransformRemoveMeshId`. The broader JNI-onload
diagnostic first drops **313 to 307** with no new names.

In the next focused change, `TransformRemoveMeshId` itself uses the
existing `ArrayRemove<Mesh *>` specialization on the selected
transform's mesh array. The original ARM function at `0x7b1a4`
calls precisely that specialization; its implementation at `0x7b288`
removes matching pointers and compacts the array, matching the
recovered template. The focused ARMv7 check stays **100% source-shape,
100% strict fuzzy and linked-exact** (not byte-exact). Native and
AArch64 static builds pass. The second full ARM corpus remains at
**4,323/4,524** originals over **4,745** pairs, **94.79% source-shape /
84.29% strict fuzzy**, **2,607 linked-exact / 1,083 byte-exact**, zero
skips, and no exact-set gains or losses. The strict AArch64 unresolved
inventory then falls **91 to 90**; the broader diagnostic falls
**307 to 306**, both with no new names.

A game shared library and APK have not been built. Live resource,
mesh-removal, material and animation-refresh behavior on Android
remains unverified.
