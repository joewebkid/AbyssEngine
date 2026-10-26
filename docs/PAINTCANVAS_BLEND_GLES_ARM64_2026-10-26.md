# PaintCanvas blend-mode GLES call targets (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original ARMv7 `PaintCanvas::SetBlendMode` starts at `0x7cb49`.
Disassembly shows direct calls to `glTexEnvi` (`0x7cb7e`), `glDisable`
(`0x7cba6`), `glEnable` (`0x7cbb6`), `glBlendFunc` (`0x7cc40`), and
`glDepthMask` (`0x7cc96`), with repeated calls in later blend cases.
The recovered source had six unresolved `paintcanvas_ext_sbm_gl*`
declarations in this function. They now forward inline to the matching
GLES 1 entry points, including `glAlphaFunc` for the alpha-test case.
Other blend-state helpers remain separate and unverified.

Native and AArch64 static `gof2` builds pass. The ARMv7 verifier compiles
**209/209** units and the focused blend function retains **92.9%
source-shape / 83.4% strict fuzzy**, nonexact. With the ARM64 ZIP and
crypto dependencies, the strict AArch64 whole-archive unresolved-name
set falls **522 → 516**. Exactly the six blend GLES wrapper names
disappear, with no new names. This does not produce the game `.so` or
verify live blend output.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs with zero skips. Mean source-shape/strict fuzzy
remains **94.72%/84.21%**. The verifier reports **2,603 linked-exact**
and **1,083 byte-exact** pairs; comparison with the preceding full report
shows zero gains and zero losses in either exact symbol set.
