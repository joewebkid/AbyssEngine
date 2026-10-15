# PaintCanvas StopDraw2FBO call contract — Android ARM, 2026-10-15

## Original evidence and recovered route

Android 2.0.16 `libgof2hdaa.so` SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `PaintCanvas::StopDraw2FBO` at `0x7d08e` executes:

| ARM site | Receiver/argument | Target |
| --- | --- | --- |
| `0x7d096` | `r0 = this->engine` (`this+0x34`) | `Engine::DeactivateRender2TextureFBO()` |
| `0x7d09e` | `r0 = this`, `r1 = 0` | `PaintCanvas::SetBlendMode(BlendMode)` |
| `0x7d0a4` | `r0 = this->engine` | `Engine::DoPostEffect()` |
| `0x7d0ae` | `r0 = this->engine`, after register restore | tail branch to `Engine::ActivateViewBuffer()` |

The first three named targets are visible in GNU ARM objdump's PLT
annotations. The final branch goes through an ARM/Thumb veneer whose
objdump label is not the callee name; REA/IDA resolves it to
`Engine::ActivateViewBuffer`. REA Evidence:
`ev_5457ec6d5f578acc688d4f6b350f032c8329e87392a90dc2aec069310c441478`.
IDA's pseudocode reports an `int` return, but the actual source contract
is `void`; the claim above is based on instructions, not that signature.
REA/IDA cannot provide complete body ranges or typed call-reference
coverage in this provider profile.

The recovered source previously called four undefined
`paintcanvas_ext_fbo_{a,b,c,d}` functions with the same register-level
arguments. It now calls the existing concrete methods. The rebuilt ARM
object emits matching non-relocation bytes, direct CALL relocations for
the first three targets, and `R_ARM_THM_JUMP24` for the final tail call
to `_ZN11AbyssEngine6Engine18ActivateViewBufferEv`.

## Validation and limits

Focused before/after verification: **100% source-shape, 100% strict
fuzzy, linked-exact, not byte-exact**. Native `gof2` and **206/206**
ARM units pass. Full corpus before/after:

| Measure | Before | After |
| --- | ---: | ---: |
| Unique compared / original | 4,323 / 4,524 | 4,323 / 4,524 |
| Average source-shape / strict fuzzy | 93.59% / 82.63% | 93.59% / 82.63% |
| Linked-exact / byte-exact | 2,503 / 1,074 | 2,503 / 1,074 |
| Matching direct-call targets / candidates | 2,348 / 77 | 2,351 / 74 |

There are zero lost or gained `(unit,symbol)` pairs in either exact set;
missing stays 201, extras 123, skipped zero. The direct-call audit does
not count the tail jump, which was checked separately. Reports are on A:
at `A:/Galaxy on Fire new version/_work/paintcanvas_stop_fbo_before_20261015.json`,
`paintcanvas_stop_fbo_after_20261015.json`,
`paintcanvas_stop_fbo_full_20261015.json`, and
`arm_call_targets_after_stop_fbo_20261015.json`.

This establishes the intended call/ABI route, not observed runtime
rendering. A native executable GL/FBO scenario and Android ARM32 runtime
check remain open. Next bounded work: inspect one PaintCanvas shim family
where the original target and behavior can both be tested without
rewriting unrelated rendering code.
