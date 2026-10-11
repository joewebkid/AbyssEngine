# Station blueprint and pirate-base tables (2026-10-11)

Android 2.0.16 ARM `Station` methods index two rodata tables:

| Table | Original ELF address | Values |
| --- | --- | --- |
| Pirate stations | `0x1fb7e8` | `1, 33, 47, 86` |
| Hidden-blueprint stations | `0x1fbf6c` | `132, 133, 134, 129, 123` |

The recovered `Station.cpp` had zeros in both tables. That made
`getPirateStationIndex`, `getHiddenBlueprintIndex`,
`stationHasPirateBase`, and `stationHasHiddenBlueprint` test the wrong
station IDs. The values now match an assert-based read of the original
ELF and the generated ARM `Station.o` rodata. The same IDs were already
used independently by `SolarSystem::hasPirateBase` and
`SolarSystem::hasHiddenBlueprint`.

Native `gof2`, 206/206 ARM translation units, and 36 verifier tests
(one skipped) pass. Focused comparison of the four `Station` methods
remains 96.3% average source-shape and 97.3% strict fuzzy; the two
index methods remain linked-exact, and none is byte-exact. As with the
agent price table, instruction-only exact matching did not validate
the referenced rodata.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_agent_mod_price_full_20261011.json`,
no exact pair was lost; there are zero skipped rows and zero wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_station_tables_full_20261011.json`.

Unverified: an end-to-end campaign visit/blueprint unlock at each ID,
and whether all persisted discovery flags align with this table order.
