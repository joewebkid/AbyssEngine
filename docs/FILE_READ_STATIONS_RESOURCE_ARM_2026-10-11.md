# Station resource path (2026-10-11)

Android 2.0.16 ARM uses `data/bin/stations.bin` for all three
`FileRead::loadStationsBinary` overloads: the no-argument color-table pass
at `0x1206b0`, selected-ID loader at `0x11f970`, and `SolarSystem` loader
at `0x12014c`. Each probes that path and opens the same path. The recovered
source instead used `stations_binary.bin` at all six call sites. Those
six literals now match the Android calls. The available extracted resource
is 3,408 bytes; it is evidence only and is not copied into the repository.

The native `gof2` target and 206/206 ARM translation units build. The
focused ARM comparison of these three overloads reports 93.3% average
source-shape and 54.5% strict fuzzy, with no linked- or byte-exact match.
These percentages are unchanged from the previous corpus snapshot: a
resource filename correction repairs runtime lookup, not instruction shape.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_ticker_loader_20261011.json`, no exact pair
was lost; there are zero skipped rows and zero wrong-type symbols. The report
is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_stations_path_full_20261011.json`.

Unverified: a running game with the original asset mounted at `data/bin`,
and the no-argument loader's color-table writes, which remain incomplete
in the recovered source.
