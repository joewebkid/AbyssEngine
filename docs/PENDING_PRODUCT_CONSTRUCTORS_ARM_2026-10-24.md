# PendingProduct constructors — Android ARM recovery (2026-10-24)

The Android 2.0.16 ARM library constructs an empty `String` before filling
either `PendingProduct` constructor. At `0x12d3c8`, the `BluePrint*` overload
then calls `getIndex`, obtains and assigns a temporary station name, destroys
the temporary, and calls `getStationIndex` and `getQuantity`. At `0x12d448`,
the saved-product overload stores the blueprint index, assigns the supplied
station name, and stores the station index and quantity. The previous member
initializers copied the name directly and reordered those operations.

The recovered constructor bodies now follow those ARM call and store orders.
Focused verification gives **100% source-shape / 100% strict fuzzy and
linked-exact** for both C1/C2 aliases of each constructor. The two destructor
aliases were already linked-exact, so the six-symbol family is **6/6
linked-exact**; none is byte-exact. This is code-shape evidence, not a live
gameplay or persistence test.

Windows native `gof2` builds, the NDK r18b builds **208/208** ARM units, and
the verifier suite passes **93 tests (one skip)**. The full corpus compares
**4,323/4,524** original functions over **4,736** pairs with zero skips:
mean source-shape **94.19% → 94.23%**, strict fuzzy **83.54% → 83.60%**,
100%-fuzzy **2,731 → 2,735**, linked-exact **2,564 → 2,568**, and byte-exact
**1,082 → 1,082**. Exact-set comparison found four new constructor aliases
and no lost linked- or byte-exact symbols; **201** originals remain missing.

Next: inspect `Status::addPendingProduct` and `RecordHandler` save/load paths
against the original, then exercise a production-to-reload round trip. Their
end-to-end semantics and live Android behavior remain unverified here.
