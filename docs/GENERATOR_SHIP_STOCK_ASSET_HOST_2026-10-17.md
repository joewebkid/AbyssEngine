# Generator ship stock in the station transition (2026-10-17)

Android 2.0.16 ARM `Generator::getShipBuyList` is at `0x95bcc` in
`libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
Its ordinary-station path uses `Globals::getRandomEnemyFighter`
(`0xe49dc`) and the 64-entry ship-race table at `0x1fb5dc`.
Station 41 increases the offer count by one and forces catalog ship
10 into the first slot. The current production source contains these
branches; this package changes only the host fixture.

The real-asset fixture now extracts production `getShipBuyList`,
`getRandomEnemyFighter`, `Ship::adjustPrice` and their required getters
and tables. A real `AERandom` is supplied before station departure.
The startup station is allowed to have no ship offer, as the original
random count can be zero. Later, production `Status::departStation`
enters catalog station 41 and creates both real item and ship lists.
The test checks that the ship list is nonempty, the first offer is a
distinct clone of catalog ship 10 with race 0, and every offer has a
valid catalog ID and positive price. It releases the owned ship clones.
Both host `-O0` and `-O2` runs pass with **68/68** balanced file
opens/closes.

Native `gof2` builds and 40 verifier tests pass (one existing skip).
No production source changed, so the last full ARM report at
`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`
still applies: **208/208** translation units, **4,323/4,524**
original functions compared in **4,730** rows, **93.78% / 82.95%**
average source-shape/strict fuzzy and **2,527 linked-exact / 1,076
byte-exact**; 201 absent, zero wrong-type or skipped units. The
focused `getShipBuyList` body is **96.7% / 67.2%** and nonexact. Test
changes outside `src/` cannot alter these exact pairs.

The exact time-dependent offer distribution and other special station
branches are not yet runtime-verified. `Generator::createAgents`
remains a test substitute; character images, names, mission offers,
`Level::init` and native device runtime are unverified. Next priority:
connect a bounded real agent-generation path with explicit image/name
boundaries, then test the station-to-flight transition.
