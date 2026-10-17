# Agent stock and missions in the station transition (2026-10-17)

The Android 2.0.16 ARM symbol map identifies
`Generator::createAgents` at `0x96884`, `createAgent` at `0x96e8c`,
and `createMission` at `0x971b8` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The existing recovered production bodies are unchanged in this package.

In the asset-backed host fixture, production `Status::departStation`
now calls the extracted production `createAgents` body for catalog
station 41 after real item and ship stock. The fixture seeds the real
`AERandom` to 1 **at the agent boundary**: `getItemBuyList` calls
`AERandom::reset()` from wall-clock time, so a seed before the whole
station transition is not reproducible. It does not replace the
agent/mission algorithms. With either the primary catalog or an
optional overlay of 13 Java-reference name files, `-O0` and `-O2`
each produce three agents and three mission offers (first mission
type 3). The test checks station, mission target range, client image
ownership linkage, positive reward/cost fields and nonempty target
station/system names.

A direct call of the same production `createAgents` body at station
108 retains all six existing catalog agents by pointer, including
the special item and ship sellers, whose sale data and acceptance
flags are refreshed. A 31-seed run of ordinary station stock checks
3–5 offers, mission creation for mission offers, non-mission offers,
and at most one wingman offer. Test cleanup keeps retained story
agents owned by `Status` and releases the generated agents and their
missions. This is a structural ownership check, not a heap-leak audit
or proof of every exceptional path. File lifetimes balance at
**179/179** without names and **551/551** with the supplementary
names in both optimization modes.

Native `gof2` builds and the verifier unit suite runs 40 tests with
one existing skip. Only the host fixture changed, so the last full
ARM report at
`A:/Galaxy on Fire new version/_work/galaxy_distance_full_20261017.json`
still applies: **208/208** translation units, **4,323/4,524**
original functions in **4,730** rows, **93.78%** source-shape,
**82.95%** strict fuzzy, **2,527** linked-exact and **1,076**
byte-exact, with zero skipped/wrong-type units and no exact-pair
losses against the preceding report. Focused source-shape/strict
fuzzy in that report: `createAgents` **99.1%/90.0%**,
`createAgent` **97.9%/93.8%**, `createMission` **96.4%/57.5%**;
none is exact.

Not yet verified: the uninterrupted wall-clock RNG sequence across
items, ships and agents; every mission type, enemy replacement,
supernova/campaign special branches, device runtime and `Level::init`.
The Java-reference names are not proven byte-identical to the Android
2.0.16 resources. Next enter a controlled station-to-flight path and
identify the first missing runtime/platform service, while expanding
the agent scenario matrix only where new evidence changes behavior.
