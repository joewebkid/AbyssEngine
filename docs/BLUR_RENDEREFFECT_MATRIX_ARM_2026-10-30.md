# BlurShader RenderEffect matrix reconstruction (2026-10-30)

Primary source: original Android ARM libgof2hdaa.so, SHA-256
6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58.
The long BlurShader::RenderEffect begins at ELF 0x8a7a8. The
previous platform packet and remaining runtime limitations are in
BLUR_PLATFORM_AND_DIMENSIONS_ARM_2026-10-30.md.

At 0x8a822 the original loads four float constants from ELF
0x8aa98: [1.0, 0.0, 1.0, 1.0]. It writes these to local matrix
indices 10 through 13. Three separate stores put 1.0 at indices
0, 5, and 14; NEON stores clear indices 1-4 and 6-9. The observed
15-float Matrix is therefore:

    [1,0,0,0, 0,1,0,0, 0,0,1,0, 1,1,1]

The previous source created 16 floats and set indices 0, 5, 10,
and 15. It put a one outside Matrix's 15-float layout and left
indices 12-14 zero. The source now creates a typed Matrix with the
15 observed values and passes it directly to SetWorldViewMatrix.
This removes the reinterpret cast. In the current SetWorldViewMatrix
implementation, the shader path reads indices 0-11, so a visible
effect from the last three corrected values is not established.

Native gof2 and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. Focused BlurShader verification retains **6/10
linked-exact**. The long RenderEffect rises from **95.3% to 95.9%
source-shape** and from **53.6% to 57.8% strict fuzzy**. The family
averages **99.3% source-shape / 93.9% strict fuzzy**. The long
function remains nonexact and has not been verified at runtime.
Focused report:
D:\gof2-arm64-crypto\blur-matrix-after-20261030.json.

The rest of the long function's GL-state behavior and blur output
on Poco remain unverified.

The full ARM corpus compares **4,323/4,524** originals across
**4,745** pairs: **94.88%** mean source-shape, **84.49%** strict
fuzzy, **2,656 linked-exact**, **1,087 byte-exact**, zero skipped,
and **201 missing** (all absent). Both exact-symbol sets are
unchanged from
D:\gof2-arm64-crypto\blur-platform-full-20261030.json.
The local improvement is below full-corpus rounding precision.
Full report:
D:\gof2-arm64-crypto\blur-matrix-full-20261030.json.

Next, inspect the remaining GL-state and matrix flow in the long
RenderEffect against an independent source or a permitted runtime
capture; source-shape alone does not establish full semantics.
