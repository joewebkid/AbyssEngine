# RenderEffect virtual dispatch and BlurShader ABI (2026-10-28)

## Evidence and change

The reference is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The `BlurShader` vtable symbol is at `0x20cf3c`, with its address point at
`0x20cf44`. ELF relocations place the two-argument `RenderEffect` at
address-point offset `0x14`, the four-argument overload at `0x18`, the
three-argument overload at `0x1c`, and the five-argument overload at
`0x20`. `BlurShader::RenderEffect(FBOContainer*, Engine*, float, Vector)`
at `0x8a760` loads the function pointer from its vtable at offset `0x20`
and passes the address of a local null target. That slot resolves to the
five-argument `BlurShader::RenderEffect` at `0x8a7a8`.

The previous source declared the four- and five-argument overloads as
nonvirtual base methods returning `FBOContainer*`, while the derived
overloads returned `void`. The original base functions at `0x87d2e` and
`0x87d32` are single-instruction `BX LR` stubs and do not return the
source pointer. The base overloads are now virtual `void` functions with
empty bodies. The derived overloads explicitly override them. The
declaration order matches the original vtable. The final ARMv7 object
has relocations for those four slots at offsets `0x1c`, `0x20`, `0x24`,
and `0x28` from the vtable symbol, respectively, matching the original
ordering after accounting for the eight-byte vtable header.

No source call site consumes a return value from these overloads.
Native, ARMv7, and ARM64 builds pass. The ARM64 shared library links
without undefined symbols; the silent APK is rebuilt, signed, and
contains the linked ARM64 library. The target ARM64 phone is still not
visible to ADB, so live postprocessing behavior is unverified.

## Verification and remaining work

Focused ARMv7 verification compares ten `BlurShader` pairs and fifteen
`ShaderBaseStruct` pairs. The short `BlurShader::RenderEffect` is now
linked-exact. The `BlurShader` family rises from 96.22% to 99.04%
unweighted source-shape, and from 95.51% to 96.93% weighted by original
instructions; linked-exact rises from five to six. Two base no-op
overloads become linked-exact and byte-exact, raising the base family
from seven to nine linked-exact and three to five byte-exact pairs.
The long `BlurShader::RenderEffect` remains 94.4% source-shape and is
not yet a verified semantic match; its control flow and GL state changes
need their own investigation.

The final full ARMv7 corpus compares 4,323/4,524 original functions
across 4,745 pairs, with 94.84% source-shape, 84.39% strict fuzzy,
2,616 linked-exact, 1,085 byte-exact, and zero skipped units. Relative
to the preceding full report, the pair set is identical and no exact
matches are lost. Four linked-exact pairs are gained: the two base
no-op overloads, the short `BlurShader::RenderEffect`, and
`GlowPPShader::RenderEffect(FBOContainer*, Engine*)`. The two base
no-op overloads are also the only new byte-exact pairs.
