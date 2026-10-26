# PaintCanvas End3d recovery (2026-10-26)

`PaintCanvas::End3d` was a one-call placeholder to
`paintcanvas_ext_end3d`. The original Android 2.0.16 ARM function starts
at `0x7d53c` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
IDA and Ghidra views of that binary were used to recover the render-pass
order and conditions. The body now handles fog, lighting, material-mode
passes, shader toggles, glow material capture and restoration, refraction,
and final glow rendering. It calls the existing `MaterialDraw` implementation
and Engine/GLES methods directly.

The focused ARMv7 verifier compiled **209/209** translation units with
zero errors and compared all **198** PaintCanvas functions without skips.
`End3d` moved from **0.3% source-shape / 0.2% strict fuzzy** to
**90.8% / 21.5%** (1,085 recovered versus 1,182 original instructions).
It is neither linked-exact nor byte-exact. The PaintCanvas unit retains
all **93 linked-exact** and **23 byte-exact** symbols from the preceding
focused run, with zero losses. Report:
`_work/end3d_full_inline_unit_20261026.json`.

The native `gof2` static archive and the Android `arm64-v8a` static
archive build. Strict AArch64 link probes with the existing arm64 libzip
and crypto dependencies reduce unresolved names **459 to 458** for the
whole archive and **219 to 218** for JNI-reachable sections. The removed
name in both is `paintcanvas_ext_end3d(AbyssEngine::PaintCanvas*)`; no
name was added. Diagnostic logs are under
`D:/gof2-arm64-crypto/link-after-end3d-final.log` and
`D:/gof2-arm64-crypto/link-jni-gc-after-end3d-final.log`.

The latest full ARM corpus remains the preceding run: **4,323/4,524**
original functions and **4,743** pairs, **94.72% source-shape**,
**84.23% strict fuzzy**, **2,604 linked-exact**, **1,083 byte-exact**,
zero skips. It was not rerun for this change. The focused result proves
only that the PaintCanvas exact sets did not regress.

No live scene has exercised the restored function. In particular, glow
array ownership, shader state, and the 64-bit layout of inherited ARMv7
structures need an on-device rendering gate. The current ARM64 `.so`
still fails strict linking on 218 JNI-reachable names, including FMOD,
UI/bridge helpers, and other engine functions. Next priority: close
source-backed JNI-reachable engine/bridge paths while selecting a real
ARM64 audio implementation and creating an Android host target.
