# `getAppVersion` / `Engine::InitGL` ARM symbol alias (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
exports two function symbols at Thumb address `0x6db21`:

| Symbol | ELF size |
| --- | ---: |
| `AbyssEngine::getAppVersion()` | 0 bytes |
| `AbyssEngine::Engine::InitGL(bool,int,int)` | 408 bytes |

Disassembly at `0x6db20` uses `r0` as the engine object and receives
the other three arguments in `r1`–`r3`. The zero-size name is an alias
of the initialization function, not an independent nullary function.
The previous `AbyssEngine.cpp` reconstruction emitted a second body
under `getAppVersion()` and called three undefined
`AE_getInitGLThis/Width/Height` placeholders to manufacture those
arguments. The duplicate body and placeholders are removed. For
ARM32, `Engine.cpp` now emits a zero-size ELF function alias at the
same address as its `Engine::InitGL` body. The rebuilt `Engine.o`
symbol table confirms both names share address `0x17bd`, with alias
size 0. No repository caller of the nullary name was found.

The focused ARM comparison now places both names in `Engine.o` and
compares two symbol pairs. `getAppVersion()` improves from **94.7%**
to **98.1%** source-shape and from **71.1%** to **76.0%** strict fuzzy;
`Engine::InitGL` remains **98.1%** and **76.0%**. Neither is exact.
The raw unmatched-symbol inventory across 209 ARM32 objects falls
**265 -> 262**, removing exactly the three placeholders with no new
name. This inventory includes external libraries and does not prove
full ARM32 shared linking.

Windows native, ARM64 static, ARMv7 **209/209**, and strict ARM64
shared linking pass. Runtime startup on a device remains unverified.
Full ARMv7 corpus: **4,323/4,524** original functions compared in
**4,770** pairs, **96.79%** mean source-shape, **86.79%** mean strict
fuzzy, **2,785 linked-exact**, **1,108 byte-exact**, 201 missing, and
zero skipped or wrong-type functions. The linked-exact and byte-exact
member sets have no losses against the preceding full report.
