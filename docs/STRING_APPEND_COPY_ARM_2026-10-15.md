# String append temporary-copy contract (Android ARM, 2026-10-15)

## Evidence and change

The original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `String::operator+=(String const&)` at `0x727d8–0x72848`. REA/IDA
assembly shows a nonzero stored-length gate at `0x727f0`, construction of a
temporary `String(other, false)` at `0x727fe`, and use of that temporary's
length and data for the append at `0x72808–0x72824`. The copy constructor at
`0x72564–0x7258c` calls `Set(unsigned short const*)`, which measures the
zero-terminated input. Consequently, a source whose stored length extends
past an embedded zero contributes only the prefix before that zero.

The recovered `operator+=` previously copied `other.length` code units
directly. It now creates the same temporary copy and uses its length and data.
The native ABI smoke checks embedded-zero truncation and self-append. The
public signature and object layout are unchanged. The original `operator=`
at `0x727a8–0x727ba` already matches the recovered non-null-source gate and
`Set` call; no speculative self-assignment change was made.

## Validation

- Focused `operator+=` source-shape **50.5% -> 66.7%**, strict fuzzy
  **9.4% -> 30.5%**; neither linked-exact nor byte-exact.
- Native `gof2` build and `gof2_abi_runtime_smoke` pass; all **206/206** ARM
  translation units compile. Verifier tests: **37 run**, one skipped.
- Full ARM corpus: **4,323/4,524** original functions compared, **201 missing**,
  **123 extras**, zero skipped, **93.56%** average source-shape and **82.60%**
  average strict fuzzy. The **2,499 linked-exact** and **1,074 byte-exact**
  `(unit, symbol)` sets have zero losses or gains versus the preceding
  `string_set_empty_full_20261015.json` report.
- Reports are local, outside Git:
  `_work/string_append_before_20261015.json`,
  `_work/string_append_after_focused_20261015.json`, and
  `_work/string_append_full_20261015.json` in the shared workspace parent.

## Limits and next priority

The original grows its buffer with `realloc`; its `Set` path allocates with
`operator new[]`. The recovered implementation retains its internally
consistent `new[]`/`delete[]` ownership, so pointer identity, allocation
failure behavior and exact code generation remain different. Do not switch
this function alone to `realloc` without auditing all `String` allocation
paths and host-runtime behavior. Identical-input execution of the original
Android ARM32 function has not been performed; the smoke checks the recovered
production body only. Next, inspect the adjacent `operator+=(char const&)`
and numeric append paths for real behavior or ABI gaps before changing them.
