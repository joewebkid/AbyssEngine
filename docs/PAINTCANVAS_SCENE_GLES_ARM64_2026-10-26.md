# PaintCanvas 2D, 3D and background state calls (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `PaintCanvas::BeginBG` (`0x7e1e1`), `Begin3d`
(`0x7ce45`) and `Begin2d` (`0x7e2a1`) call GLES state and matrix
functions. Representative ARM calls are `glDisable`/`glDepthMask`
at `0x7e1f2`/`0x7e1f8`, `Engine::SetColor` at `0x7e218`,
`glLoadIdentity`/`glScalef`/`glLoadMatrixf` at
`0x7e24e`/`0x7e25c`/`0x7e26a`, and `glTexEnvi`/`glMatrixMode`/
`glMultMatrixf` at `0x7e322`/`0x7e32a`/`0x7e35c`.
The recovered functions now call those existing GLES/Engine targets
through local inline declarations. The minimal GLES header lacked
`glMultMatrixf`, so this translation unit declares its C ABI locally.
Two Thumb-to-ARM veneers resolve the remaining branch targets:
`0x19b3c8` reaches `Engine::SetPerspMatrix@plt` (`0x5fe80`), and
`0x19b3d8` reaches `glLoadIdentity@plt` (`0x5fe8c`). The recovered
matrix-load and final reset calls now use those targets. Other
unresolved PaintCanvas helpers remain for separate recovery.

Native and AArch64 static `gof2` build. The ARMv7 verifier compiles
**209/209** units. Focused source-shape/strict fuzzy is:

| Function | Before | After | Exact |
| --- | --- | --- | --- |
| `BeginBG` | 86.4% / 62.0% | 78.4% / 49.4% | no |
| `Begin3d` | 89.3% / 86.4% | 80.5% / 65.8% | no |
| `Begin2d` | 87.6% / 79.7% | 87.6% / 79.7% | no |

The lower scores in the first two functions reflect their shorter
newly generated ARM code; the original call targets and arguments
support the accepted behavior. The entire changed PaintCanvas unit
compares **198** symbols and retains the same **92 linked-exact** and
**23 byte-exact** symbol sets, with zero losses. Other source files and
build flags are unchanged, so the previous full-corpus snapshot
(**4,323/4,524** originals over **4,743** pairs; **2,603
linked-exact**, **1,083 byte-exact**) remains the latest full run.

With the previously built ARM64 ZIP and crypto dependencies, the
strict whole-archive unresolved-name set falls **516 → 502**, and the
JNI-reachable set falls **276 → 262**. Both lose the same 14
scene-state wrapper names and gain none. No game `.so` is produced;
live scene rendering remains unverified.
