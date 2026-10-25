# Sprite-system mesh call ABI (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `AbyssEngine::SpriteSystemCreate` begins at `0x86adc`.
At `0x86b04` it passes mesh format `0x1f` in `r3`, stores the address
of the sprite system's mesh slot on the stack at `0x86b0e`, and calls
`AbyssEngine::MeshCreate(Engine*, unsigned short, unsigned short,
signed char, Mesh**)` at `0x86b18`.

The recovered `SpriteSystemCreate` had declared a different overload
with an `unsigned int` format and `void**` output. That overload has
no definition and makes the AArch64 shared-link diagnostic fail. The
call now uses the existing, ARM-confirmed `signed char`/`Mesh**`
signature and passes the typed mesh slot. No allocation or error-path
behavior was changed.

Native `gof2`, AArch64 static `gof2`, and the focused ARMv7 unit build.
Focused source-shape/strict fuzzy for `SpriteSystemCreate` changes
**95.0%/67.1% → 94.7%/61.4%**. It remains nonexact. The metric
decrease is accepted because the original relocation target and
argument registers confirm this call; the previous target was
undefined. The strict AArch64 whole-archive unresolved-name inventory
falls **544 → 543** without new names. A game `.so` or APK is still
not available, and live sprite creation is not yet verified.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs with zero skips. Mean source-shape/strict
fuzzy remains **94.72%/84.21%** after report rounding. The
**2,603 linked-exact** and **1,083 byte-exact** symbol sets have zero
gains and zero losses versus the preceding full report.
