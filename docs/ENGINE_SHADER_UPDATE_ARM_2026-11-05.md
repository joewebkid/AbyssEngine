# Engine shader-update call targets (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
tail-branches from the shader-enabled color and light paths to the
Thumb/ARM veneer at `0x19b658`. Its PC-relative target is PLT
`0x60768`, named `Engine::ShaderUpdate()`. For example,
`LightSetRimColor` branches at `0x85dfa`, material ambient at
`0x85b44`, and the other material color paths use the same veneer.
There are no original `ShaderUpdateRimColor` or
`ShaderUpdateMaterialColor` methods.

The reconstructed ARM32 code called those two undefined helper
names. It now calls `Engine::ShaderUpdate()` directly, as the ARM64
path already did. The existing method body moved to
`EngineShaderUpdate.cpp` so calls from `Engine.cpp` retain an external
branch relocation, matching the original source shape. A direct
same-file call had compiled `LightSetRimColor` into a short local
branch: its normalized instructions stayed 100%, but it lost a
previously counted linked-exact match. The separate translation unit
restores that match without retaining the incorrect helper target.

Focused ARMv7 compilation passes **210/210** translation units.
`LightSetMaterialColorAmbient` and
`LightSetGlobalSceneColorAmbient` rise to **100%** source-shape and
linked-exact; the diffuse and specular material paths rise from
**93.2% to 98.6%** source-shape and **61.0% to 92.9%** strict fuzzy.
`LightSetRimColor` and `ShaderUpdate` retain their linked-exact
memberships. The raw unmatched-symbol inventory across 210 ARM32
objects falls **252 -> 250** with exactly the two undefined helper
names removed and no new names.

Windows native and ARM64 static builds pass, as does strict ARM64
shared linking. Device rendering behavior and full ARM32 shared
linking remain unverified. The full ARMv7 corpus compares **4,323/4,524**
original functions in **4,770** pairs: **96.80%** mean source-shape,
**86.82%** strict fuzzy, **2,788 linked-exact** and **1,108 byte-exact**.
There are 201 missing functions, zero skipped and zero wrong-type.
Compared with the prior full report, no linked-exact or byte-exact
symbol is lost. The two gained linked-exact symbols are
`LightSetMaterialColorAmbient` and `LightSetGlobalSceneColorAmbient`.
`ShaderUpdate` changes translation unit, so pair membership by
`(unit, symbol)` changes even though its symbol remains linked-exact.
