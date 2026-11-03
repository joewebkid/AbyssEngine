# Cargo price source indices and global modifier (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `Status::calcCargoPrices` at `0xaca10` locates the
source solar system with `Item::getMinPriceSystem()`, the upper-price
system with `Item::getMaxPriceSystem()`, and the local system with
`Station::getSystem()`. It seeds the per-station random generator with
`Station::getIndex()`. After computing the item price, it applies
`Globals::globalPriceRaise` as a percentage when nonzero.

The reconstruction used `Item::getIndex()` for the source system and
`Station::getIndex()` for the local system. These are different fields
from the two system indices and can select the wrong entries in the
galaxy array. It also dereferenced a permanently null pointer for the
price modifier. The three sources now match the original ARM calls
and global. The random seed still uses the station index as before.

Focused ARMv7 verification compiles 209/209 translation units.
`calcCargoPrices` source-shape rises from **86.3% to 90.2%**;
strict fuzzy changes from **42.5% to 41.4%**. The generated count
increases from 198 to 220 instructions against 249 original. The
146-symbol `Status` unit keeps 103 linked-exact and 66 byte-exact
matches, with no exact-set loss. Focused report:
`_work/status_cargo_price_sources_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. Actual prices
have not been compared against a live original game or a captured
fixture. The producer of nonzero `Globals::globalPriceRaise` remains
unverified in the reconstruction. The original exported symbol is at
`0x2182e8`. REA/IDA found code references only at `0xacc54–0xacc56`
and `0x17556e–0x175570`; both load the value, in `calcCargoPrices`
and `Ship::adjustPrice`. No in-library store was observed. Indirect
or external writes remain possible. Next check the price calculation
with representative system and item data.

The full ARM corpus compares **4,323/4,524** original functions in
**4,769** pairs, with 201 missing and zero skipped. Mean source-shape
rises from **96.74% to 96.75%**; mean strict fuzzy remains **86.74%**.
Linked-exact stays **2,779** and byte-exact **1,108**. Pair and exact
sets have no losses against `_work/record_all_slots_full_20261103.json`.
Full report: `_work/status_cargo_price_sources_full_20261103.json`.
