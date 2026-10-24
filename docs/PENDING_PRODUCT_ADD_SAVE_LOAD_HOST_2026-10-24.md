# Pending-product add/save/load host gate (2026-10-24)

Android 2.0.16 ARM `Status::addPendingProduct` at `0xa8ef4` matches an
existing entry by blueprint and station index, adds its quantity, or creates
one `PendingProduct`. In `RecordHandler::recordStoreWrite`, the block at
`0xceea0–0xcef0e` counts non-null entries, writes `-1` for no entries, and
otherwise writes the count followed by blueprint index, quantity, station
index and a wide station name. `recordStoreRead` at `0xcc5b0–0xcc63c` reads
those fields in that order and constructs the saved-product overload.

The new host gate extracts those production source blocks without rewriting
their bodies. It also uses the production `PendingProduct` constructors,
`BluePrint` getters, `String`, and `AEFile` wide-string read/write methods.
Only the initial `BluePrint` fixture, minimal `Status`/`GameRecord` holders,
and raw file byte transport are host boundaries. Both Windows `-O0` and
`-O2` pass:

- null and all-null lists serialize as `-1` and reload as absent;
- a sparse list writes only its two non-null products, preserving the integer
  field order and a Cyrillic wide-string name across reload;
- two additions for one blueprint/station merge quantities, while the same
  blueprint at a second station stays separate after save and reload.

Native `gof2` builds with no source changes, **208/208** ARM units compile,
and **93** verifier tests pass (one skip). Fresh focused ARM verification of
the surrounding family reports `Status::addPendingProduct` at **93.7% /
69.5%**, `recordStoreWrite` at **91.0% / 26.8%**, and `recordStoreRead` at
**91.5% / 17.4%** source-shape / strict fuzzy; the four constructor aliases
remain **100% / 100% linked-exact**. No production C++ changed, so the last
full corpus remains **4,323/4,524** originals over **4,736** pairs,
**94.23% / 83.60%** mean source-shape / strict fuzzy, **2,568 linked-exact**
and **1,082 byte-exact**, with **201** missing and no known exact-set loss.

This is a bounded block-level round trip, not an entire `RecordHandler`
save-file test. Real file headers, other `GameRecord` fields, failed/truncated
reads, an original save fixture and live Android behavior remain unverified.
Next priority: integrate the complete save/load file path with a controlled
file boundary, then test a full record rather than only this block.
