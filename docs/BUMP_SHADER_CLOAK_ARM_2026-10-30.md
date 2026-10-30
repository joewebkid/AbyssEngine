# BumpShaderCloak original ARM sources and layout (2026-10-30)

Primary source: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `BumpShaderCloak::Init` at ELF `0x8ac7c` passes embedded
GLSL to `ES2LoadProgram`. Its PC-relative literals point to the vertex
source at `0x1ec8d0` (1,547 bytes, SHA-256
`a41c0ba108c28117467d4c423fdfb5566819504d7231b5d411c7c1bd55a686ac`)
and the fragment source at `0x1ecedc` (2,323 bytes, SHA-256
`0190588df6131bdae557a22ee62cef148d539db3f49a189f63dcf9af69449d46`).
Both reconstructed arrays were decoded and compared byte-for-byte to
their original ELF ranges. Five attribute and 21 uniform query names
were also checked against the ARM literal order. The former source
passed resource filenames and generic input names.

Original `Init` stores shader locations at object offsets `+0x20`
through `+0x90`, leaving unknown four-byte slots at `+0x54`, `+0x7c`,
and `+0x88`. The previous class layout omitted those slots. Its
`UpdateMeshData` consequently read several location fields from
neighboring offsets, including the color and animation uniforms.
Adding three explicit unknown slots restores the observed ARM32 layout;
the rebuilt `Init` now compares linked-exact. The contents and purpose
of those three slots beyond this layout are not established.

Original `UpdateMeshData` converts display dimensions with signed
`vcvt.f32.s32`; the source now casts the returned width and height to
signed 32-bit integers before division. Its two-light branch was
written with the original positive branch first. Focused
`UpdateMeshData` improves from **98.2% to 99.2% source-shape** and from
**84.4% to 95.7% strict fuzzy**; it remains nonexact. The remaining
instruction diff is concentrated in the compiler's scheduling and
register allocation of the two inverse display dimensions. Focused
`Init` improves from **99.7% to 100% source-shape** and from **89.8%
to 100% strict fuzzy**. Five of six unit functions are linked-exact.

Native `gof2` and ARM64 static builds pass; ARMv7 compiles **209/209**
translation units. The strict ARM64 shared library links without
undefined symbols and retains the preceding **53 JNI exports**. The
signed, zip-aligned parallel-install APK at
`D:\gof2-arm64-crypto\gof2-arm64-bumpcloak-final-poco-clone-debug-20261030.apk`
contains that library byte-for-byte and its signature verifies. The
APK has not been installed or run because Play Protect blocked earlier
clone installations on Poco X3. Runtime shader compilation, rendering,
and gameplay remain unverified.

Focused report:
`D:\gof2-arm64-crypto\bump-cloak-final-source-focused-20261030.json`.
After renaming the three unknown-slot declarations, native, ARM64, and
ARMv7 were rebuilt again. The ARMv7 shader object SHA-256 remained
`bf97ab35761879db3155f7e836d241897a27d3616ccbe84ee2eae0e39f7ff081`;
the focused report was regenerated for the final source. The full
corpus compares **4,323/4,524** original functions across **4,745**
pairs, with **94.87%** mean source-shape, **84.45%** strict fuzzy,
**2,644 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Against `bump-rimcube-new-full-20261030.json`,
complete `(unit, symbol)` sets gain only `BumpShaderCloak::Init` in
linked-exact, with zero linked-exact or byte-exact losses. The rounded
corpus-wide means remain unchanged. Full report:
`D:\gof2-arm64-crypto\bump-cloak-full-20261030.json`.
