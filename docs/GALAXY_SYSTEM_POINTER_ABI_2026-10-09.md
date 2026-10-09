# Galaxy and Status system pointer contract (2026-10-09)

In the Android 2.0.16 ARM binary, `Galaxy::getSystem(int)` at `0x175e98`
returns null for a negative index and otherwise loads a `SolarSystem *` from
the galaxy's systems array into `r0`. `Status::getSystem()` at `0x0abd60`
loads the pointer stored at `Status + 0x1a4` into `r0`. The recovered
`Status::setStation` path assigns the galaxy result to that field. These are
pointer contracts, not integer indices.

`Galaxy::getSystem` and `Status::system` now use `SolarSystem *` in source.
`Status::getSystem` returns the field directly, and internal consumers no
longer round-trip it through `int32_t`. The direct `GameRecord` caller and
the `Status::getSystem` callers that used Windows `long` now keep the pointer
intact. The 32-bit ARM field remains four bytes at offset `0x1a4`; the ARM
build checks that layout. The existing compile-time smoke verifies the
declaration, stored-field and getter types on the native host.

Validation: native `gof2` builds, 206/206 ARM translation units compile,
35/36 verifier tests pass with one optional skip, and four scope tests pass.
Focused checks keep `Galaxy::getSystem` and `Status::getSystem` byte-exact;
`Status::setStation` remains at 86.5% source-shape. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_system_pointer_20261009.json`:
**4,323/4,524** original functions compared across **4,689** rows,
**91.87%** source-shape, **79.93%** strict fuzzy, **2,313 linked-exact** and
**997 byte-exact**, zero wrong-type misses or skipped units. Compared by
`(unit, symbol)` with the preceding station-pointer report, there are zero
lost rows or exact pairs and zero exact gains. This is an ABI/runtime repair,
not an assembly-shape improvement.

Native whole-game runtime remains untested. Widening `Status::system` on a
64-bit host changes its native struct layout; Android's 32-bit layout remains
unchanged, but other native code that assumes raw 32-bit offsets has not been
audited end to end. The next bounded priority is the index-as-pointer use in
`Status::activateNewWanted`, which needs direct ARM control-flow confirmation
before replacing it with a systems-array lookup.
