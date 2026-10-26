# ParticleSystemMesh math and mesh calls (2026-10-26)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The Ghidra and IDA listings identify `setParticle` at `0x195d00`,
`setQuadEdge` at `0x19608c`, `init` at `0x196268`, and
`updateSingleColor` at `0x1863a0`. Their calls resolve to
`AEMath` vector/matrix operations and `PaintCanvas::MeshSetPoint`,
`MeshSetUv`, both `MeshSetColor` overloads, `MeshSetTriangle`, and
`MeshTranslatePoint`. The 17 undefined `_psm_` bridges in the
reconstruction now execute those typed methods. The trail-completion
bridge calls the existing `ParticleSystemMesh` member function.

The Android ARMv7 build keeps these adapters out of line to retain
the original call-site instruction shapes in `setQuadEdge` and `init`.
Native and AArch64 builds may inline them in the caller. An initial attempt
to inline on ARMv7 lost two existing linked-exact matches; it was
discarded. The accepted focused verifier compares 25 symbols, zero
skips, and retains the same **14 linked-exact / 6 byte-exact** symbol
sets with **93.39% source-shape / 83.43% strict fuzzy**.

Native and AArch64 static archives build. Strict AArch64 unresolved
names fall **419 to 402** for the whole archive and **179 to 162** on
the JNI-reachable route, with no new names. The game shared library
still fails linking. Particle rendering and trail motion have not run
on the device; this is source-backed call recovery and compile/link
validation, not a visual runtime test.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,745** pairs, **94.76% source-shape / 84.26% strict fuzzy**,
**2,606 linked-exact / 1,083 byte-exact**, zero skips. Both exact
symbol sets are unchanged from the preceding report. Defined symbols
absent from the original increase **127 to 144**; all 17 additions are
the explicit `_psm_` adapters required by the existing ARMv7 call
shapes. They are compatibility code, not newly reconstructed original
functions. The local full report is
`D:\gof2-arm64-crypto\psm_bridge_full_20261026.json` outside Git.

Next priority: remaining `PaintCanvas` render helpers and compatible
ARM64 audio implementation.
