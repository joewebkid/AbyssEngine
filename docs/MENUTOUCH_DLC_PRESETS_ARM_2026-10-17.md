# DLC start presets: Valkyrie and Supernova (2026-10-17)

Source: Android 2.0.16 `libgof2hdaa.so`, locally identified by SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`, with ARM function entry points
`MenuTouchWindow::startValkyrie` at `0x12ca80` and
`MenuTouchWindow::startSupernova` at `0x12cce4`. The item indices below
are pointer-array byte offsets divided by four; they are not fields in a
`ShipDefTable` row. No original binary or asset is committed.

| Start | Campaign steps | Ship / race | Equipment `(item, amount, slot)` | Cargo | Station |
| --- | ---: | --- | --- | --- | ---: |
| Valkyrie | 45 | 5 / 0 | (2,1,0), (5,1,1), (36,10,0), (81,1,0), (51,1,1), (86,10,2), (85,1,3) | — | 91 |
| Supernova | 84 | 30 / 3 | (176,1,0), (20,1,1), (36,20,0), (44,20,1), (81,1,0), (51,1,1), (68,1,2), (86,1,3), (85,1,4), (56,1,5) | (122,8) | 70 |

The previous C++ treated the `Globals::ships` array size as a pointer to
a synthetic row, discarded the returned ship and item pointers, derived
the race from a ship pointer, and called an invalid transition thunk.
The replacement uses the typed `Status`, `Ship`, `Item`, `Galaxy`, sound,
achievement, record and application APIs. It also restores the mission
argument, zero-credit writes, actual station pointer and original call
order. Repeated `getShip`, `setCredits` and equipment slots are deliberate
observed behavior, not deduplicated conveniences.

Focused ARM results, before → after:

| Function | Source-shape | Strict fuzzy | Exact |
| --- | ---: | ---: | --- |
| `startValkyrie` | 89.1% → 97.5% | 53.7% → 70.8% | neither |
| `startSupernova` | 91.4% → 96.4% | 45.8% → 64.7% | neither |

`python tools/test_menutouch_dlc_presets.py` asserts the source-backed
indices, amounts and slots in production call order. This is a source
regression test, not a simulated game launch. The two functions are
still nonexact; option-record layout and the full station-to-flight
runtime path require independent behavioral verification with assets.

The native `gof2` static-library build, 208/208 ARM translation units,
both preset tests and 40 verifier tests (one pre-existing skip) pass.
The full ARM corpus compares 4,323/4,524 original functions in 4,723
comparisons, with 201 missing and 123 extra symbols. Aggregate
source-shape/strict fuzzy moves 93.74%/82.88% → 93.75%/82.89%; linked-
exact and byte-exact counts stay 2,520 and 1,076. Comparing exact
`(unit, symbol)` sets against the previous full report finds no losses.
Local machine reports are in the sibling `_work/` directory as
`mtw_dlc_preset_{before,after,full}_20261017.json`.
