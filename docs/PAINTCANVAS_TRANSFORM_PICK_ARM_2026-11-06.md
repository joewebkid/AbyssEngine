# PaintCanvas transform picking (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA's IDA analysis and the original ARM instructions were checked for
both `PaintCanvas::TransformGet2DPickedTextureRegion` overloads at
`0x7b424` (recursive transform) and `0x7b590` (transform index).
REA's generated parameter names are not treated as source authority;
the ARM argument registers and stack slots establish the mapping.

The index overload copies the selected transform's 60-byte world
matrix, converts the input `(x, y, z)` from signed integers to floats,
applies `AEMath::MatrixInverseTransformVector`, then calls the
recursive overload with the three transformed coordinates and the
original `shift`. The reconstruction instead transformed `(y, z,
shift)`, passed the original `x`, and replaced `shift` with zero.
For an invalid index, original bytes at `0x7b66c` are `0xbf800000`;
both result fields are `-1.0f`, followed by a diagnostic on stderr.
The reconstruction returned `(0, 0)` and called an undefined helper.

The recursive overload converts `((x >> shift) >> shift)` and
`((z >> shift) >> shift)` to float for `MeshIntersect`, returning when
both result fields differ from `-1.0f`. If no mesh hits, it copies
each child's world matrix, inverse-transforms the unshifted `(x, y,
z)`, and recurses with the transformed coordinates and unchanged
`shift`. Original bytes at `0x7b588` are also `0xbf800000`, used for
the final no-hit result. The reconstruction had swapped coordinates,
passed zero shift to children, and returned zero on total miss.
The actual `MeshIntersect` result type is now shared between its
definition and caller without changing its machine-level behavior.

Focused ARMv7 verification compiles **211/211** translation units.
The index overload rises **85.4% -> 97.8%** source-shape and
**29.9% -> 82.5%** strict fuzzy; the recursive overload rises
**80.4% -> 92.9%** source-shape and **26.1% -> 60.9%** strict fuzzy.
Both remain non-exact. `MeshIntersect` itself remains **86.2%**
source-shape and **43.7%** strict fuzzy. Windows native and ARM64
static builds pass, as does strict ARM64 shared linking. The raw
211-object ARM32 unmatched-symbol inventory falls **205 -> 195**:
ten picking placeholders disappear, with no new unmatched name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs. Mean source-shape rises **96.82% -> 96.83%** and
strict fuzzy **86.89% -> 86.91%**. It retains **2,798 linked-exact**
and **1,113 byte-exact**, 201 missing, zero skipped, zero wrong-type,
and 149 extra symbols. The complete exact-match membership sets are
unchanged.

The 201 missing symbols in `cmake-build-match/verify/missing.txt` are
bundled ZIP/zlib/OpenSSL and C++ runtime functions, not separately
missing gameplay methods. That count must not be read as 201 absent
game functions. The ARM32 unmatched-symbol inventory is a separate
list: it includes platform imports and remaining reconstruction
placeholders.

Live touch picking and full ARM32 shared linking remain unverified.
Next priority: inspect the remaining gameplay placeholders in the
ARM32 inventory, especially camera and font call targets, and verify
touch picking on a device when one is available without disrupting
another active device test.
