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
- Focused ARM reports compare 26 and 15 rows respectively. With ELF-declared
  function bounds, all 41 are linked-exact; 12 are raw-byte-exact.
- The original ELF symbol table gives `std::bad_typeid::what()` a 12-byte
  body; our ARM object also gives it 12 bytes. The older delinker gave the
  target 108 bytes, extending into adjacent code. After the shared delinker
  uses the ELF-declared size, it compares 4/4 instructions and is linked-exact.
- Native UCRT64 `gof2` builds. The matching ARM build compiles 206/206
  translation units. Full verification completes with zero skipped and zero
  wrong-signature symbols.

| Full-corpus metric | Previous | With wrappers | With ELF sizes |
| --- | ---: | ---: | ---: |
| Original unique functions compared | 4,281/4,524 | 4,322/4,524 | 4,322/4,524 |
| Missing original functions | 243 | 202 | 202 |
| Comparison rows | 4,632 | 4,673 | 4,673 |
| Mean source-shape | 89.85% | 89.92% | 89.97% |
| Mean strict fuzzy | 76.84% | 77.03% | 77.07% |
| Linked-exact rows | 2,113 | 2,153 | 2,156 |
| Raw-byte-exact rows | 965 | 977 | 977 |

Comparison of the **sets** of exact unit/symbol pairs found no linked-exact
or byte-exact losses at either step. The ELF-size correction adds three
linked-exact rows without changing source code or coverage; the other two are
`JNIEnv::CallStaticBooleanMethod` and `ConfigReader::RegisterTokenReadFunction`.
Reports are local ignored evidence under
`_work/blueprint-isunlocked/`: `alias_sections_full.json`,
`ndk_libcxxabi_stdlib_exception_focused.json`,
`ndk_libcxxabi_stdlib_typeinfo_focused.json`, and
`stdlib_runtime_full.json`, `typeinfo_declared_size_focused.json`, and
`stdlib_runtime_elf_sizes_full.json`.

These figures measure symbol coverage and code generation, not the semantic
completeness of the game. The next meaningful priority is source-backed game
behavior/ABI work; further verifier changes need the same full-corpus
exact-set regression gate.
