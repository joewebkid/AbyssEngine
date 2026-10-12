# AEMath Matrix constructor/product — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `libgof2hdaa.so` constructor
`_ZN11AbyssEngine6AEMath6MatrixC2Ev` at ELF `0x6e8d8` stores an
identity basis and three trailing ones. ARM `vld1.64` loads the literal
words `1, 0, 1, 1` and `vst1.32` writes them at offsets `0x28..0x34`
(`m[10..13]`); a separate store writes one at `0x38` (`m[14]`). The
local constructor had zero at `m[10]` and `m[13]`. Those two elements
now follow the original. The symbol's 0x40-byte extent includes its
literal pool; it does not alone prove that each matrix object is 0x40
bytes. The current C++ type remains 15 floats / 0x3c bytes.

Android `_ZN11AbyssEngine6AEMathmlERKNS0_6MatrixES3_` at ELF
`0x802c8` writes its result through offset `0x38` (`m[14]`) and has no
store at offset `0x3c`. The local product wrote `result.m[15]` one
float past its 15-element result. That store is removed; the existing
15-element math remains unchanged. This is a bounds correction, not a
claim that the entire matrix ABI or product source shape is recovered.

The native host smoke links the production math units and checks the
default matrix, a nontrivial product with translation/scaling, and the
existing assignment/operators. Its temporary executable is removed.

## Verification and limits

- Native `gof2` static library: pass. ARM: 206/206 translation units,
  zero failed. Host smoke: `Matrix constructor/product host smoke: OK`.
  Verifier suite: 37 run, 36 pass, one skipped.
- Constructor focused strict fuzzy/source-shape: 50.0%/78.7% before,
  100%/100% after. It is not linked- or byte-exact; the target and base
  each have 19 disassembled items after the change. Product focused
  strict fuzzy stays 2.2%; source-shape moves 48.6% to 47.9% because
  the out-of-bounds store was removed. It remains non-exact. Reports:
  `A:/Galaxy on Fire new version/_work/matrix_ctor_before_20261012.json`,
  `A:/Galaxy on Fire new version/_work/matrix_ctor_after_20261012.json`,
  `A:/Galaxy on Fire new version/_work/matrix_product_before_20261012.json`,
  and `A:/Galaxy on Fire new version/_work/matrix_product_after_20261012.json`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  rises 92.5321% to 92.5365%; strict fuzzy rises 80.8034% to
  80.8140%. Linked-exact remains 2,376 and byte-exact 1,008, with no
  exact-symbol losses or substitutions. Full report:
  `A:/Galaxy on Fire new version/_work/matrix_ctor_product_full_20261012.json`.

The host smoke does not run original assets, OpenGL, or ARM32 gameplay.
The 0x3c/0x40 owner-layout question remains open. `PaintCanvas.cpp`
still has writes to `Matrix::m[15]` despite the declared 15-element
array; those sites require per-buffer/layout audit before changing them.
