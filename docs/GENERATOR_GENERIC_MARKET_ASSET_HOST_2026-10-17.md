# Generic Generator market with real catalogs (2026-10-17)

Android 2.0.16 ARM `Generator::getItemBuyList` at `0x95428`
(`libgof2hdaa.so` SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
returns an item array for ordinary stations after the early-campaign
special case. The body walks the catalog (`0x955ee`), checks station,
campaign, technology and item attributes, then applies random gates
and distance-dependent quantities (`0x958b8` onward). This package
adds no production source code: the host fixture now calls that
existing production body through a test-only wrapper.

The test starts from seven local Android catalogs, creates a game, and
uses the catalog-backed station 78 after advancing the campaign index
to 32. A real `AERandom` is supplied so the generic loop executes;
its production `reset()` reads the clock. The test therefore checks
stable properties instead of claiming a fixed inventory: nonempty
result, strictly increasing catalog IDs, distinct cloned objects,
positive amounts, preserved catalog prices and no mutation of the
station's original three-item startup list. It releases all returned
items. The fixture then loads catalog station 91 and calls production
`Status::departStation`, which fills that station with a nonempty real
item market while ship and agent generation remain test substitutes.
The first station retains its startup stock. Both host `-O0` and `-O2`
runs pass and each balances **68/68** file opens/closes.

Native `gof2` builds and 40 verifier tests pass (one existing skip).
Because only host tests changed, the last full ARM report at
`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`
still applies: **208/208** translation units, **4,323/4,524**
original functions compared in **4,730** rows, **93.78% / 82.95%**
average source-shape/strict fuzzy, **2,527 linked-exact / 1,076
byte-exact**, 201 absent, zero wrong-type or skipped units. The
focused `getItemBuyList` body remains **97.0% / 67.9%** and nonexact;
no exact pair can change from this test-only patch.

This does not validate the exact original stochastic distribution or
the item set for a particular clock value. Ship/agent stock, other
station types, long-running ownership, `Level::init` and native device
runtime remain unverified. The fixture's empty `Mission` sentinel is
stack-owned, so cleanup restores a heap-owned mission before the
production `Status` destructor runs. Next: validate the market with a
controlled clock/RNG boundary, then replace ship and agent test stubs
in separate source-backed steps.
