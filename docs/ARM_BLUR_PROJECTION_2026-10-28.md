# BlurShader projection reconstruction (2026-10-28)

## Original ARM evidence and correction

The reference is Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In the five-argument `BlurShader::RenderEffect` at `0x8a7a8`, the
original clears 16 consecutive floats in `Engine::projMatrix` at
offsets `0x384` through `0x3c0` before assigning the projection
coefficients. Four `VST1.32` instructions at `0x8a7e2`, `0x8a7ea`,
`0x8a7f2`, and `0x8a7fa` write the zeros. The prior source assigned
only six elements, leaving the other ten values from the preceding
matrix. The Android source now clears all 16 with four NEON stores.

The original stores `0xbd4ccccd` (`-0.05f`) at engine offset `0x3ac`,
which is `projMatrix[10]`; the prior source stored `-1.0f`. It also
converts display dimensions as signed integers before floating-point
division. The Android path now matches these observed value sources.
The PC path retains its prior `projMatrix[10]` value because no PC
reference has been confirmed for that coefficient.

## Validation and limits

ARMv7 compiles 209/209 translation units. Native and ARM64 static builds
pass. The ARM64 shared library links without undefined symbols, and the
silent APK is rebuilt, signed, and contains that library. Focused ARMv7
verification compares ten `BlurShader` functions. The long
`RenderEffect` rises from 94.4% to 95.3% source-shape and from 50.8%
to 53.6% strict fuzzy. The family reaches 99.13% source-shape
unweighted, or 97.31% weighted by original instructions, while all
six previous linked-exact functions remain linked-exact. The long
function is still not an exact or fully verified semantic match.

Live projection and blur output remain unverified because the target
ARM64 phone is not visible to ADB. The next source investigation should
compare the remaining matrix setup and GL state transitions in this
long function with the original ARM sequence.

The full ARMv7 corpus compares 4,323/4,524 original functions across
4,745 pairs, with 94.84% source-shape, 84.39% strict fuzzy,
2,616 linked-exact, 1,085 byte-exact, and zero skipped units. The
`(unit, symbol)` pair set and both exact-match sets are identical to
the preceding full report: there are no exact gains or losses in this
projection change.
