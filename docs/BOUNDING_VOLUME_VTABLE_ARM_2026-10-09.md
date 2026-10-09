# BoundingVolume virtual ABI recovery (2026-10-09)

The Android 2.0.16 `libgof2hdaa.so` ELF has 28-byte vtables for
`BoundingVolume` (`0x20dd8c`), `BoundingSphere` (`0x20de68`) and `BoundingAAB`
(`0x20d834`). Each contains two header words followed by exactly five method
slots: `getCollisionNormal`, `update`, `collide`, `outerCollide`, then
`projectCollisionOnSurface` (pure virtual in the base). `readelf -rW` resolves
those entries to the named methods and `__cxa_pure_virtual`; the original
exports D1/D2 destructors but no D0 deleting destructor for these classes.

The previous C++ declaration put a virtual destructor in the first two slots
and did not make `getCollisionNormal` virtual. Its compiled base vtable was
32 bytes, and typed virtual calls from PlayerFixedObject used the wrong
offsets. The base destructor is now nonvirtual, the five declarations have
the original order, and the two derived normal methods explicitly override.
No data fields, method signatures or collision formulas changed.

`tools/verify/test_bounding_volume_vtable.py` checks the original and compiled
ELF relocation targets and 28-byte table sizes. It passes all 15 compiled
slots against the original, and rejects a saved pre-fix object with its
32-byte base table. Example invocation (point `--objects` to the compiled
ARM `base/engine/math` directory):

```powershell
python tools/verify/test_bounding_volume_vtable.py --readelf <NDK arm-linux-androideabi-readelf.exe> --original <libgof2hdaa.so> --objects <base/engine/math>
```

The integrated native `gof2` library and all 206 ARM translation units build.
The full `--fail-on-wrong-type` corpus compares 4,323/4,524 originals across
4,689 rows, with zero wrong-type misses or skipped units. Against the prior
integrated report, source-shape rises **91.85% → 91.87%**, strict fuzzy
**79.89% → 79.92%**, linked-exact **2,309 → 2,313**, and byte-exact stays
**997**. Comparison rows and both exact-pair sets have zero losses by
`(unit,symbol)`. The linked gains are three `ArrayReleaseClasses<BoundingVolume*>`
occurrences and `PlayerFixedObject::moveForward`. Extra symbols fall from 115
to 112. Full report:
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_boundingvolume_abi_20261009.json`.

This proves the compiled ARM virtual-slot layout, not complete collision
behavior or native gameplay. Derived destructors have empty bodies; deletion
through a base pointer now calls only the base destructor, matching the
original nonvirtual ABI but remaining a C++ host-language ownership concern.
At this vtable-only checkpoint, the separate projection return-contract
mismatch and 64-bit pointer-width paths remained unrepaired. A native game
and an ARM-device runtime were not executed. The projection follow-up below
repairs that contract and its callers without losing the exact-pair sets.

## Projection return contract follow-up (2026-10-09)

Android `0x11f8b0` receives a hidden `Vector` result pointer in `r0`, the
input vector in `r1`, and the volume array in `r2`. It copies the three input
floats into the result, makes two passes, rereads the array size on each loop
test, calls virtual `outerCollide` then `projectCollisionOnSurface`, and assigns
the returned vector into the result. `PlayerStaticFar` `0x11c52e`,
`PlayerStation` `0x12287c` and `PlayerFixedObject` `0x154e16` forward exactly
those registers. The mangled function name does not encode C++ staticness or
the return type, so the ARM register contract is decisive here.

The old declaration was a non-static `void` method. Two callers passed a
12-byte `Vector` as a fake `BoundingVolume*`; the method wrote fields at
offsets `+4/+8/+12`, including one word beyond that vector. PlayerFixedObject
called a declared but undefined compatibility function instead. The method is
now static and returns a real `Vector`; all three callers use it directly.
The input remains unchanged, a null volume list returns a copy of the input,
and the two passes reread `size()` as the ARM loop does.

`tools/bounding_volume_projection_smoke.cpp` executes the production method
with a null list and a probe volume. It verifies the returned coordinates,
unchanged input, and two collision/projection calls. The focused host build
links the actual `BoundingVolume.cpp` and `Vector.cpp`; its only stubs satisfy
an unrelated, non-exercised helper:

```powershell
g++ -std=c++17 -O0 -ffunction-sections -fdata-sections -Isrc tools/bounding_volume_projection_smoke.cpp src/engine/math/BoundingVolume.cpp src/engine/math/Vector.cpp '-Wl,--gc-sections' -o <C-drive-output.exe>
```

Native `gof2`, all 206 ARM units, 36 verifier tests (35 pass, one optional skip) and
four scope-filter tests pass. The full `--fail-on-wrong-type` report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_projection_contract_20261009.json`.
It retains 4,323/4,524 compared originals across 4,689 rows, zero wrong-type
misses/skips, **2,313 linked-exact** and **997 byte-exact**. The comparison
set and both exact-pair sets have zero losses or gains from the preceding ABI
report. Corpus source-shape remains **91.87%** at two-decimal precision and
strict fuzzy rises **79.92% → 79.93%**. The method itself improves from
**71.2% → 96.4% source-shape** and **34.1% → 50.9% strict fuzzy**; it is not
linked- or byte-exact. The undefined compatibility symbol is absent from
source and from the native library's undefined-symbol list.

This confirms the return and call ABI plus a narrow host behavior case, not
complete collision runtime. ARM-device gameplay and wider ownership behavior
remain untested. The next priority is a bounded audit of pointer-width
contracts such as `Galaxy::getStation` before claiming a 64-bit native port.
