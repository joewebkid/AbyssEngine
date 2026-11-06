# Particle camera toggle and String::PrintOut call targets (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`ParticleSystemManager::cameraToggle` at `0x183c28` returns when the
camera set is unchanged. Otherwise it writes the new set, calls
`releaseSprites`, and tail-branches through a veneer at `0x19c948` to
the PLT entry for `ParticleSystemManager::initSprites` at `0x68994`.
The reconstruction instead called an undefined
`_psm_constructAfterCamera` symbol. It now calls the existing
`initSprites` member directly.

`String::PrintOut` at `0x73644` calls `String::GetAEChar` and then
tail-branches through a veneer at `0x19b108` to `operator delete[]`
at PLT `0x5ebfc`. The reconstruction previously called an undefined
`String_printImpl`, leaving the allocated buffer unfreed. It now
deletes that buffer. The original ARM function has no output call;
the method name alone is not evidence of printing on this build.

Focused ARMv7 checks compile **211/211** translation units. Both
functions retain **100% source-shape** and **100% strict fuzzy**.
`String::PrintOut` retains linked-exact; `cameraToggle` loses its
previous linked-exact flag. This is a correction of a masked-call
false positive: `linked_equal` ignores relocation-covered call bytes
and does not validate their destinations. The old call had the wrong
target, whereas the new call matches the original PLT destination.
The separate call-target audit confirms the `GetAEChar` call in
`String::PrintOut`; the tail-call destinations above were checked in
the original disassembly and PLT.

Windows native, ARM64 static, and strict ARM64 shared builds pass.
The 211-object ARM32 unmatched-symbol inventory falls **229 -> 227**:
only `_psm_constructAfterCamera` and `String_printImpl` disappear;
there are no new unresolved names.

Full ARMv7 corpus: **4,323/4,524** original functions in **4,772**
pairs, **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
skipped, zero wrong-type, and 149 extra symbols. Exact membership
comparison finds only the corrected `cameraToggle` linked-exact flag
lost; no byte-exact pair is lost. The overall counts should not be
interpreted as a regression in behavior in this case.

Live particle rendering and `PrintOut` behavior on a device, and full
ARM32 shared linking, remain unverified. Next priority: inspect
remaining ARM32 helper call targets and eliminate confirmed placeholder
symbols without relying on masked-byte equality alone.
