# Galaxy station pointer return contract (2026-10-09)

The Android 2.0.16 ARM binary returns a `Station *` from both
`FileRead::loadStation(int)` (`0x11f948`) and `Galaxy::getStation(int)`
(`0x175ea8`). The former loads the first station pointer from the returned
array into `r4` and returns it in `r0`. The latter preserves that result on
the nonnegative path; on the negative path it loads
`Status::gStatus->playerStation` at offset `0x78` and returns the pointer.
The existing `Globals.cpp` call-site declaration independently used
`Station *` for `FileRead::loadStation`.

The recovered declarations and definitions previously returned `int` or
`int32_t`, narrowing the pointer on 64-bit native builds. Both now return
`Station *`. Direct callers in menu, mission, UI and world code no longer
convert the result through `long` or `intptr_t`. The symbol names and ARM
register return convention are unchanged. A compile-time smoke verifies both
public result types on the native host:

```sh
g++ -std=c++17 -I src -I third_party/jni -I third_party/gl -fsyntax-only tools/station_pointer_contract_smoke.cpp
```

Validation: native `gof2` builds; all 206 ARM translation units compile;
36 verifier tests run (35 pass, one optional skip), four scope tests pass.
The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_station_pointer_20261009.json`.
It compares **4,323/4,524** original functions across **4,689** rows with
zero wrong-type misses or skipped units: **91.87%** source-shape,
**79.93%** strict fuzzy, **2,313 linked-exact** and **997 byte-exact**.
Compared by `(unit, symbol)` with the preceding integrated report, there are
zero lost comparison rows or exact pairs and zero exact gains. The two target
functions keep their prior focused source-shape scores (FileRead 57.5%,
Galaxy 72.0%);
this is an ABI/runtime correction, not a claimed shape improvement.

The original positive `Galaxy::getStation` path allocates `FileRead` on the
heap while the current source uses a stack object. The original
`FileRead::loadStation` allocation/cleanup path also differs. Those lifetime
details, native gameplay and ARM-device runtime remain unverified. Next,
audit the separate `Galaxy::getSystem` pointer-width return contract before
changing its callers.
