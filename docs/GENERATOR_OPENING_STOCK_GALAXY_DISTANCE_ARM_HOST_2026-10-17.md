# Generator opening stock and Galaxy distance (2026-10-17)

The Android 2.0.16 ARM binary (`libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has a dedicated `Generator::getItemBuyList` branch at `0x95428` for
station 78 while the campaign mission is below 7. It clones catalog
items 0, 22 and 55 with amount 1 and price 0. The recovered production
body already has this branch; this package links it into the real-asset
host fixture in place of the former empty-market test stub.

With seven local Android catalogs mounted, the production
`Status::resetGame` -> `Status::departStation` path now tests that the
startup station receives exactly those three cloned items. The fixture
checks their order, IDs, amounts and prices, and releases the owned
objects. Both host `-O0` and `-O2` runs pass with 66/66 balanced file
opens/closes. The focused pre-change `getItemBuyList` report is 97.0%
source-shape / 67.9% strict fuzzy, nonexact; no Generator source was
changed by this package.

The ARM `Galaxy::distancePercent` body at `0x175f44` loads
`Globals::globals` before calling `Globals::sqrt`. The recovered source
had instead used `Globals::gGlobals`, which is initialized to null and
has no assignment in the recovered tree. The source now uses the
ARM-backed global. The host fixture calls both `distancePercent` and
`invDistancePercent` with `gGlobals == nullptr` and checks the 3-4-5
distance and inverse result 95. Focused ARM comparison keeps both
symbols at 100% source-shape / 100% strict fuzzy and linked-exact.
The object relocation now targets `Globals::globals`; the remaining
`Globals::gGlobals` relocation belongs to the separate, unreviewed
`Galaxy::distance(SolarSystem*, SolarSystem*)` function. Linked-exact
wildcards relocation bytes and does **not** prove the target global is
correct, which is why this semantic check matters.

Native `gof2` builds, all 208 ARM translation units compile, and the
verifier's 40 tests pass (one existing skip). The full report at
`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`
compares 4,323/4,524 original functions in 4,730 rows, averaging
93.78% source-shape and 82.95% strict fuzzy. It has 2,527
linked-exact and 1,076 byte-exact pairs; 201 originals remain absent,
with zero wrong-type signatures and zero skipped units. Comparison
against `status_move_wanted_full_20261017.json` finds zero exact-pair
gains or losses.

Boundaries: ordinary market stock is not exercised (the fixture's RNG
is intentionally null); `Generator::getShipBuyList`, `createAgents`
and random station choice remain test substitutes. `Level::init`,
actual flight, repeated resets and full-game runtime remain unverified.
Next, exercise ordinary market generation with a bounded real RNG and
reference catalog assertions, then integrate ship/agent generation and
the station-to-flight entry in separate packages.
