# BloomShader first render and mode recovery (2026-10-28)

## Android ARM evidence

Reference: `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Both `BloomShader::RenderEffect` overloads (`0x89820`, `0x89cd8`) read
`AbyssEngine::firstRender`, clear it, call `InternalInit`, and clear
`fboBlack` before using the four bloom FBOs. The original global is at
virtual address `0x215110` in `.data` (file offset `0x214110`) and its
initial byte is `1`. The prior source instead used a local, zero-initialized
flag with no writes elsewhere, so neither overload could initialize its
FBOs.

Both original overloads write `0xbd4ccccd` (`-0.05f`) to engine offset
`0x3ac`, which is `projMatrix[10]`. The prior source wrote `-1.0f`.
The original mode switch reads `Engine::switchBloom`; its `.data` value
at virtual address `0x2150d0` (file offset `0x2140d0`) is `3`. In mode
3, the final bloom texture is `fboBlurV` at object offset `0x6c`.
The prior source read another zero-initialized local variable and used
`fboBlack` in mode 3. The recovered source now uses the original globals,
initial values, projection coefficient, and mode-3 texture in both
overloads.

## Verification and remaining uncertainty

ARMv7 compiles 209/209 translation units; native and ARM64 static builds
pass. The ARM64 shared library links without undefined symbols and exports
all 53 JNI entry points. The silent test APK is signed and verified under
Android signature schemes v1, v2, and v3. It is stored outside the repo at
`D:\gof2-arm64-crypto\gof2-arm64-bloom-debug-20261028.apk` (SHA-256
`2f844c2d8e3a98a772b5158bac865997c338eb563a7bfd2200eb2b9f8ba4acd3`).

Focused ARMv7 verification raises the two `RenderEffect` source-shape
scores from 91.0% and 91.4% to 95.9% and 96.9%. The eleven-function
`BloomShader` family rises from 89.77% to 90.72% unweighted, or from
94.33% to 97.37% weighted by original instruction count. The six
previously linked-exact functions in this family remain linked-exact.
Neither long overload is exact. The very short nondeleting destructors
remain 52.9% source-shape: the current compiler folds away the original
vtable store, with no confirmed behavioral difference.

The full ARMv7 corpus compares 4,323/4,524 original functions across
4,745 pairs, with 94.84% source-shape, 84.39% strict fuzzy,
2,616 linked-exact, 1,085 byte-exact, and zero skipped units. The
`(unit, symbol)` pair set and both exact-match sets are identical to the
preceding full report: zero pair or exact-match losses.

The target ARM64 phone is not visible to ADB; live bloom output remains
unverified. Modes outside 0 through 3 have not been validated at runtime.
Next, test the signed APK on the target phone with GOF2 confirmed in the
foreground, then investigate the remaining render path differences.
