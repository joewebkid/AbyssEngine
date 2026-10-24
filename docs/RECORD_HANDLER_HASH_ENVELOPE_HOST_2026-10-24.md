# RecordHandler hash envelope host gate (2026-10-24)

The recovered `RecordHandler::addHash`, `addHashToOptions` and `checkHash`
compute and verify `SHA256(payload || 25-byte salt || 16-byte key)`, with the
32-byte digest appended to the file. Android ARM `addHash` at `0xcdea0` and
`checkHash` at `0xcd7b0` show this input order. This is the game's format,
not a claim that the construction is a secure modern MAC.

`tools/run_pending_product_roundtrip_smoke.py` extracts the production
read/write/hash bodies and the salt into a Windows host harness. Its controlled
file boundary and synthetic 16-byte key exercise both the primary record and
options file at `-O0/-O2`. Seven payload bytes survive the write; each hash
append yields 39 bytes and the digest equals an independently calculated
SHA-256 vector (`dfe8599573786081829414da50fb09e5f16cfaf4c515e87c7e003d88677fdd55`).
`checkHash` accepts intact records and rejects a changed payload byte, changed
digest byte, changed key, and a truncated 33-byte record. The earlier gate
continues to cover the backup write. No production C++ changed in this step.

Native `gof2` builds (no-op), all **208/208** ARM translation units compile,
and **93** verifier tests pass (one existing skip). Focused ARM comparison of
the three hash functions is **82.9% source-shape / 50.0% strict fuzzy**;
none is linked- or byte-exact. The full corpus and exact-set counts are
unchanged by this test-only edit. A fresh full comparison covers
**4,323/4,524** original functions over **4,736** pairs with zero skips,
**94.23% source-shape / 83.60% strict fuzzy**, **2,568 linked-exact** and
**1,082 byte-exact**. Comparison with the preceding full report finds zero
lost or gained exact symbol pairs.

The controlled file boundary does not prove real filesystem failures,
complete record layout, a genuine game-key fixture, compatibility with a live
Android save, or a live device run. Next priority: validate a complete
record-file envelope with original data, then its read/fallback behavior.
