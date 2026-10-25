# Engine FBO dispatch targets (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The six short `Engine` methods at `0x6dcbe..0x6dd10` load either
`postEffectFBO` (`Engine+0x414`) or `refractFBO` (`Engine+0x418`),
check for null, then branch directly to these `FBOContainer` methods:

| Engine method | Original destination |
| --- | --- |
| `ActivateTextureFBO`, `ActivateRefractFBO` | `Activate` |
| `ActivateRender2TextureFBO`, `ActivateRender2FracFBO` | `BeginCapture` |
| `DeactivateRender2TextureFBO`, `DeactivateRender2FracFBO` | `EndCapture` |

The recovered C++ used three undefined global wrappers as call
targets. Those declarations are removed; the six methods now call
the confirmed `FBOContainer` members directly. All six were already
**100% source-shape / 100% strict fuzzy / linked-exact** before the
change and retain those scores afterward. The linked-exact metric
masks branch relocation targets, so it did not expose the wrong
destination.

Native `gof2`, the AArch64 static archive and **209/209** ARMv7
translation units build. A source-extracted host test checks both
null and non-null FBO paths at `-O0/-O2`. A strict AArch64
whole-archive link with system GL/log/Android/zlib libraries now
reports **577** unresolved symbols, down from **580**. No shared
library or live Android FBO rendering is claimed. The internal GL
behavior of `FBOContainer` remains outside this dispatch check.

The full ARM corpus compares **4,323/4,524** original functions over
**4,743** pairs with zero skips. Mean source-shape/strict fuzzy stay
**94.71%/84.21%**; **2,601 linked-exact** and **1,083 byte-exact**
symbols remain, with zero symbol-level gains or losses against the
preceding full report.

Next integration priority: resolve the remaining actual `Engine`
startup calls and verify FBO creation/capture in a live GL context.
