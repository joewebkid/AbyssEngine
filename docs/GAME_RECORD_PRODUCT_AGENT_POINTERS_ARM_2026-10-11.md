# GameRecord pending-product and agent pointers — Android ARM (2026-10-11)

Android `GameRecord::load` at `0x1559f0` transfers slot `0x144`
(`this + 81` words) into `Status::pendingProducts`, then copies
pointer elements from the array in slot `0x148` (`this + 82` words)
into `Status::agents`. `RecordHandler::recordStoreRead` constructs
`Array<PendingProduct *>` and `Array<Agent *>` for those slots.
The former native C++ stored their addresses in 32-bit integer fields
and copied agent elements through `Array<uint32_t>`, truncating
64-bit pointers.

Both ARM slots are now typed array pointers at the confirmed offsets.
The 64-bit native class retains the raw slots for its existing layout
and uses separate aligned pointer members. Constructor, reader, and
load all use the same typed accessors, and the agent transfer copies
full-width `Agent *` elements.

Native `gof2`, all 206 ARM translation units, and 36 verifier tests
pass (one skipped). Focused `GameRecord` and `RecordHandler` comparison
counts, source-shape, and exact matches are unchanged from the prior
package. The full corpus has 4,713 comparisons of 4,323/4,524
original functions, 92.24% average source-shape, 80.43% strict fuzzy,
2,353 linked-exact, and 1,008 byte-exact. There are zero skipped units,
zero wrong-type symbols, and no loss of any previously exact pair.

Runtime save/reload remains untested. Later pointer-bearing slots,
starting with `0x14c` (wingmen), still need a native ABI audit;
the final game executable has not been linked.
