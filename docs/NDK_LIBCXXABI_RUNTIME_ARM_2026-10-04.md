# NDK libc++abi runtime ARM coverage (2026-10-04)

The matching build now compiles two ARM-only wrappers for the **installed NDK
r18b libc++abi source**: `stdlib_exception.cpp` and `stdlib_typeinfo.cpp`.
These are toolchain/runtime functions, not newly recovered game logic. The NDK
source remains outside the repository; the native host build sees empty
translation units.

## Evidence and validation

- Android 2.0.16 `libgof2hdaa.so` is the target. The wrappers compile the
  corresponding NDK r18b source through the existing `src/runtime/*` match
  flags in `tools/verify/build_objs.sh`.
- Focused ARM reports compare 26 and 15 rows respectively: 40/41 are
  linked-exact, 12/41 are raw-byte-exact. The remaining
  `std::bad_typeid::what()` comparison is **not** claimed exact.
- The original ELF symbol table gives `std::bad_typeid::what()` a 12-byte
  body; our ARM object also gives it 12 bytes. The delinked target object
  gives it 108 bytes, extending beyond the symbol into adjacent code.
  Thus its reported 18.2% strict / 15.9% source-shape result is a suspected
  delinking boundary artifact, not evidence for a source-level rewrite.
- Native UCRT64 `gof2` builds. The matching ARM build compiles 206/206
  translation units. Full verification completes with zero skipped and zero
  wrong-signature symbols.

| Full-corpus metric | Previous | With wrappers |
| --- | ---: | ---: |
| Original unique functions compared | 4,281/4,524 | 4,322/4,524 |
| Missing original functions | 243 | 202 |
| Comparison rows | 4,632 | 4,673 |
| Mean source-shape | 89.85% | 89.92% |
| Mean strict fuzzy | 76.84% | 77.03% |
| Linked-exact rows | 2,113 | 2,153 |
| Raw-byte-exact rows | 965 | 977 |

Comparison of the **sets** of exact unit/symbol pairs found no linked-exact
or byte-exact losses. Reports are local ignored evidence under
`_work/blueprint-isunlocked/`: `alias_sections_full.json`,
`ndk_libcxxabi_stdlib_exception_focused.json`,
`ndk_libcxxabi_stdlib_typeinfo_focused.json`, and
`stdlib_runtime_full.json`.

These figures measure symbol coverage and code generation, not the semantic
completeness of the game. The next meaningful priority is source-backed game
behavior/ABI work, while the delinker boundary should be audited separately.
