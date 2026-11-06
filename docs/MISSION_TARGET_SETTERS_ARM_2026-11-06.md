# Mission target-name setters (ARM32)

Date: 2026-11-06

## Original-binary evidence

In the original Android 2.0.16 `libgof2hdaa.so`,
`Mission::setTargetName(String)` at `0x15bcc2` adds offset `0x1c`
to `this` and tail-branches to `String::operator=(const String&)`.
`Mission::setTargetSystemName(const String&)` at `0x15bbac`
does the same at offset `0x4c`. Each original function has only two
instructions and returns the assignment result register directly.

## Recovery and verification

Both setters now return `String&`, the assignment method's actual
return type, and return its result directly. Existing callers ignore
the return value. The current Clang 7 `-Oz` build still emits a call
and epilogue instead of the original tail branch; no assembly or
compiler-specific workaround was added.

- Focused ARMv7 build: **211/211** translation units. Each setter's
  source-shape rises **5.7% -> 19.4%**, strict fuzzy **0% -> 28.6%**,
  and instruction count falls from seven to five against two in the
  original. Neither becomes exact.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved names remain **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**
  (from **86.97%**), **2,802 linked-exact**, **1,116 byte-exact**,
  201 missing, zero skipped, zero wrong-type, and 151 extra symbols.
  Both exact membership sets are unchanged.

The return type and assignment target are backed by the original ARM
path. The exact source expression and live mission behavior remain
unverified; the two-instruction tail-call shape is not reproduced by
the current compiler configuration.
