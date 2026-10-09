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
The separate `BV_staticProjectCollisionOnSurface` return-contract mismatch
and 64-bit pointer-width paths are not repaired here. A native game and an
ARM-device runtime were not executed. The next ABI priority is to reconcile
that projection contract and its callers without losing the exact-pair sets.
