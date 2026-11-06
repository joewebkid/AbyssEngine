# MGame mission-success dialogue constructor (ARM32)

Date: 2026-11-06

## Evidence and change

In the original APK, `MGame::successCheck()` allocates `0x74` bytes
at `0x18000a`–`0x18000c`, then calls the default
`DialogueWindow::DialogueWindow()` constructor at `0x180012`. The
constructor is exported as `_ZN14DialogueWindowC1Ev` at `0x166f5c`.
The previous ARM32 source allocated the same size but passed the
object to an undefined variadic `DialogueWindow_ctor(...)` stand-in.
The source now uses `new DialogueWindow()` on all targets. The ARM32
object references `_ZN14DialogueWindowC1Ev` and no longer references
`_Z19DialogueWindow_ctorz`.

## Verification

- Focused ARMv7 build: 211/211 translation units; target
  source-shape 98.5% -> 98.8%, strict fuzzy 90.0% -> 90.3%, still
  non-exact.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved symbol names: 187 -> 186. Only
  `_Z19DialogueWindow_ctorz` was removed; no new name was added.
- Full ARM corpus: 4,323/4,524 original functions, 4,772 pairs,
  96.83% mean source-shape, 86.93% strict fuzzy, 2,798 linked-exact,
  1,113 byte-exact, 201 missing, zero skipped, zero wrong-type, 152
  extra symbols. Both exact membership sets match the preceding
  corpus result.

The original-binary call target and static compilation are verified.
Live mission-success behavior and full ARM32 shared-library linking
remain unverified. Most remaining unresolved ARM32 names belong to
FMOD and other external dependencies; the next bounded work should
investigate those boundaries without assuming a silent audio stub
reproduces original behavior.
