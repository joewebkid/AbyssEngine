# KIPlayer crate capture (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`KIPlayer::captureCrate(Hud*)` at `0xa5e60–0xa60f6`. The sole source
caller found is `TractorBeam::update`. The ARM disassembly and IDA
pseudocode agree that state 3/4 clears `hasCargo` and, when `+0x101` is
set, calls `KIPlayer::setActive(false)` without moving the object to sleep
state. Other states randomize the available amount with
`AERandom::nextInt(originalAmount)`; the amount is then bounded against
ship free space, floored at one, and passed to `Item::makeItem(int)`.
The selected amount is subtracted from the crate cargo even when item
creation fails.

The old source dereferenced null placeholder globals for the random
generator and item table, called unbounded `nextInt()` and no-argument
`makeItem()`, and passed the wrong HUD flags. The recovered code uses
`Globals::rnd`, the existing `Array<Item*>` at `Globals::items`, and
`Globals::status`. For an accepted item, `Hud::catchCargo` receives
`single=false` and `missionDelivery=special`; for a full ship it receives
`single=true` and `missionDelivery=false`. Both branches pass false for
the remaining flags. ARM confirms the two special item IDs 116/117 and
the 132–153 discovery range; those were already represented in source.

A production-method smoke compiles at `-O0/-O2` and covers no cargo,
finished state, bounded random amount and free-space cap, mission cargo
merged into equipment, full-ship rejection, ordinary cargo, discovery,
and failed item creation. Native UCRT64 `gof2` and all 208 NDK r18b ARM
translation units build; 93 verifier tests pass (one existing skip).
Focused `captureCrate` source-shape/strict fuzzy improves **32.0%/25.4% →
93.0%/66.5%** with target/base instruction counts **248/245** after the
patch. It remains nonexact.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,736** pairs. Mean source-shape/strict fuzzy changes from
**94.07%/83.39% → 94.08%/83.40%**. It retains **2,557 linked-exact**
and **1,082 byte-exact** pairs, with zero exact-pair losses, skipped or
wrong-type symbols. Only `captureCrate` changed score among existing
pairs. Full report: `capture-crate-after-full.json` (SHA-256
`cb9f1ca9a1311a6e778f7f63b41047672469bc1197c33e85251ff9c5b55966af`).

This smoke does not run a complete TractorBeam/game frame or real inventory
assets; ownership after equipment merging and the HUD presentation are not
proven end-to-end. The next candidate is the low-scoring
`PlayerEgo::calcCollision`, subject to a separate ARM/caller audit.
