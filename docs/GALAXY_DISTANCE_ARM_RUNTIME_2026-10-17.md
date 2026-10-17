# Galaxy system distance ARM and runtime correction (2026-10-17)

The original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
contains `Galaxy::distance(SolarSystem*, SolarSystem*)` at `0x175f80`.
At `0x17604a`–`0x17605e`, its ARM instructions load
`Globals::globals` and call `Globals::sqrt`; at `0x176062`–`0x17606a`
they load the VFP literal **18.85** and multiply the distance. The
recovered source instead dereferenced the never-assigned
`Globals::gGlobals` and multiplied by a static zero. Both defects
made nontrivial system distance unusable. The source now uses the
ARM-backed pointer and scale.

The asset-backed host fixture executes the unchanged production
`Galaxy::distance` body for identical and different catalog systems
while `gGlobals` remains null. It checks zero for the identical system
and the 3D Euclidean distance, including integer `z / 10`, times
18.85 for different systems. Both `-O0` and `-O2` pass with the primary
Android catalog and with the optional Java-reference names overlay.
This verifies the tested calculation, not every gameplay use of it.

Focused ARM comparison for `_ZN6Galaxy8distanceEP11SolarSystemS1_`
changes from **96.8% to 97.2% source-shape** and from **70.9% to
71.7% strict fuzzy**; it remains nonexact. Full ARM comparison at
`A:/Galaxy on Fire new version/_work/galaxy_distance_full_20261017.json`
builds **208/208** translation units and compares **4,323/4,524**
original functions in **4,730** rows with zero skipped and zero
wrong-type signatures. Corpus averages remain **93.78% source-shape**
and **82.95% strict fuzzy** after rounding. Linked-exact remains
**2,527** and byte-exact **1,076**; comparison of exact-pair sets with
`generator_market_full_20261017.json` shows **zero losses and zero
gains**. Native `gof2` builds and 40 verifier tests pass (one existing
skip).

`Generator::createMission` consumes this distance but its full
agent/mission path is not yet runtime-verified. Next connect real
`Generator::createAgents` and mission creation to a bounded station
transition, checking generated-agent ownership and mission targets.
