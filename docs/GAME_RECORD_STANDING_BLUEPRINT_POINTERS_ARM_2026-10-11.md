# GameRecord standing and blueprint pointers — Android ARM (2026-10-11)

In Android `GameRecord::load` at `0x1559f0`, slot `0x13c` is transferred
to the live `Status::standing` pointer and slot `0x140` is traversed as
an array of `BluePrint *` objects. The Android `recordStoreRead` path
constructs those objects and assigns the same slots. The former native
C++ stored both pointers in 32-bit integers; later it copied blueprint
pointer elements through `Array<uint32_t>`, truncating them on 64-bit
builds.

ARM now declares `Standing *` and `Array<BluePrint *> *` at the
confirmed offsets. The 64-bit native class keeps the raw slots for
its existing layout but uses separate aligned pointer members.
Construction, reader assignment, blueprint traversal, and transfer
to `Status` use typed pointers throughout. The reader constructs an
`Array<BluePrint *>` instead of an untyped pointer array.

Native `gof2`, all 206 ARM translation units, and 36 verifier tests
pass (one skipped). Focused `GameRecord` retains two linked-exact
destructor aliases; `RecordHandler` adds two blueprint-array template
comparisons, one linked-exact. Existing focused function shape and
exact pairs do not change. The full corpus has 4,713 comparisons of
4,323/4,524 original functions, 92.24% average source-shape, 80.43%
strict fuzzy, 2,353 linked-exact, and 1,008 byte-exact. The two new
comparisons do not change unique original-function coverage; one adds
a linked-exact pair. No previously exact pair, skipped unit, or
wrong-type symbol was introduced.

This does not prove a native save/reload round trip. Adjacent
`GameRecord` fields from `0x144` onward still include raw 32-bit
slots used as pointers, and the full game executable is not linked.
Next audit the pending-product and agent arrays and the remaining
pointer-bearing slots before exercising runtime serialization.
