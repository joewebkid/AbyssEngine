# Status station departure: ARM evidence and bounded host test (2026-10-17)

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
provides `_ZN6Status9isOnStackEP7Station` at `0x0a9070` and
`_ZN6Status13departStationEP7Station` at `0x0a924c`. ARM shows that
`isOnStack` returns the matching **stored Station pointer**, not a boolean.
`departStation` retains that pointer across `addStationToStack` and uses it
for the hub copy path. The prior source discarded the identity and
called the lookup twice.

The ARM departure body also initializes the current mission to
`Mission::empty`, stops the mission scan on a matching noncampaign type
11/13, uses campaign slot zero to decide rerouting, increments the reroute
counter before testing its **previous** value against ten, uses
`Galaxy::getSystem`, clears the transition flag and calls
`Ship::refreshValue`. The filtered mission-type mask is the literal
`0x8B782B`, not zero. These are source-backed behavioral repairs, not
edits made only to increase a matching score.

The asset-backed host fixture extracts the production `Status` departure,
stack, station, mission and ship methods. At `-O0` and `-O2`, it starts a
new game from seven real local Android catalogs, departs station 78,
checks creation of empty market lists through an **explicit test-only
Generator boundary**, verifies that a different Station object with the
same ID resolves to the stored pointer, tests the filtered type 150 and
noncampaign type 11 mission exits, and checks that reroute counter
9 becomes 10 without taking the random branch. It also verifies the
post-departure ship-value refresh and balanced **56/56** file
open/close pairs. The host boundary aborts if `moveWanted` is reached;
therefore the test does not validate wanted relocation or generated
station economy.

Focused ARM: `isOnStack` improves from 95.4% source-shape / 94.1%
strict fuzzy, nonexact, to **100% / 100% linked-exact**. After all
repairs, `departStation` is **84.6% source-shape / 20.4% strict fuzzy**,
nonexact (baseline 84.0% / 16.0%). Native `gof2` and 208/208 ARM TUs
build; the verifier suite reports 40 tests with one existing skip. The
full report at
`A:/Galaxy on Fire new version/_work/status_depart_full_20261017.json`
compares **4,323/4,524** original functions in **4,730** rows, at
**93.78%** average source-shape and **82.95%** strict fuzzy. It has
**2,527 linked-exact / 1,076 byte-exact** comparisons, 201 absent
originals, zero wrong-type and zero skipped units. Pairwise comparison
with the preceding full report shows **one linked-exact gain**
(`isOnStack`) and zero linked-/byte-exact losses.

Unverified: real `Generator` inventory/agent output, hub deep-copy
ownership, wanted relocation, reroute random loop (the ARM condition
appears independent of each candidate system and can repeat forever),
`Level::init`, renderer/platform services and actual flight. No claim
of complete station-to-flight integration is made. The next bounded
step is an instrumented departure-to-`Level::init` host slice with
separate tests for Generator, wanted movement and random rerouting.
