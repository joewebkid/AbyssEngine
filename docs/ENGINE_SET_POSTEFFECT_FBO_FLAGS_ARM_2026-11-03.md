# Engine::SetPostEffect framebuffer creation flags

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The exported `Engine::SetPostEffect(unsigned int, bool)` begins at
`0x865fc`. When enabled and the post-effect FBO is absent, the original
constructs `FBOContainer` and calls `Create` at `0x86668`. Immediately
before the call it stores `0` at `[sp]` (`0x86664`) and places `1` in
`r3` (`0x86666`). For this member call, `r3` carries the first boolean
argument and the stack slot carries the second: the original call is
`Create(width, height, true, false)`.

The reconstruction passed `false, true`. The original `Create` body
uses its first boolean to select `GL_LINEAR` or `GL_NEAREST`; its
second boolean is unused. Thus the reconstructed `SetPostEffect`
selected nearest filtering where the original selected linear. The
call now passes `true, false`. Other effect flag logic was not changed.

Focused ARM comparison after the correction compiled **209/209**
translation units. `SetPostEffect` remains **80.1% source-shape** and
improves **53.4% → 55.2% strict fuzzy**; it is not exact. Windows
native and ARM64 static builds pass. The full corpus compared
**4,323/4,524** originals in **4,762** pairs at **96.24%** mean
source-shape and **86.00%** strict fuzzy, with **2,742 linked-exact**,
**1,096 byte-exact**, 201 missing and zero skipped. Relative to the
preceding `FBOContainer::Create` report, the comparison-pair and both
exact sets are unchanged.

The adjacent FBO lifecycle audit found `BeginCapture`, `EndCapture`,
`Activate` and both destructor aliases linked-exact. `Release` is
**95.5% source-shape / 96.3% strict fuzzy**; the constructor aliases
are **100% source-shape / 96.4% strict fuzzy**. The original constructor
does not initialize the three extra renderbuffer slots that `Release`
later passes to GL deletion; the reconstruction currently follows
that shape. Their runtime values and ownership need separate evidence.

Live GLES output and the appearance of post-processing effects remain
unverified. Redmi testing remains stopped by user request. Next,
audit the remaining `SetPostEffect` control flow and FBO release
lifetime against the original, then test on a permitted host.
