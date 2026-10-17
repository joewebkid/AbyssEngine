# Core catalog resources through the native file stack (2026-10-17)

Android 2.0.16 ARM identifies `FileRead::loadItemsBinary` at `0x120afc`
and `FileRead::loadShipsBinary` at `0x120ca8`. Both probe and open their
`data/bin/*.bin` path, read fixed record counts (233 items, 64 ships),
create an object for every record, and close the handle. The local
evidence files remain outside Git:

| Resource | Bytes | SHA-256 |
| --- | ---: | --- |
| `items.bin` | 27,628 | `55b953629d994d553604418988d43d2b1935f93061da7040a464acad57677e95` |
| `ships.bin` | 2,304 | `4a04c753410318af223e2f6bef852d336bff175eeec767c8b82940a75859f182` |

The host file-stack test mounts those two exact logical paths alongside
`weapons_hd.bin`. It compiles the unchanged production
`AEFile` stack, the extracted unchanged `FileRead` loader bodies, and
the production `Item` and `Ship` constructors and initialization code.
The unrelated player-status branch is held null at this test boundary;
this does not execute game startup or station-to-flight play.

At host `-O0` and `-O2`, absent resources return null without opening a
file. With the resources present, all 233 item indices and all 64 ship
indices are parsed in order. The test checks first/last item attributes
and the source-backed Valkyrie/Supernova template IDs 5 and 30, their
slots, values and handling. All created objects and arrays are released;
the three-resource test observed four opens and four closes. The test
was subsequently extended to `stations.bin` and direct station lookup;
the current four-resource run observes seven of each. See
[the station ownership check](FILE_READ_LOAD_STATION_OWNERSHIP_ARM_2026-10-17.md).

The native `gof2` target and **208/208** ARM translation units pass, as
do 40 verifier tests (one existing skip). Focused ARM scores remain
**95.9% source-shape / 59.9% strict fuzzy** for `loadItemsBinary` and
**78.4% / 61.0%** for `loadShipsBinary`; neither is exact. No production
source changed, so the preceding full corpus remains authoritative:
**4,323/4,524** originals, **4,723** comparisons, **93.75%/82.89%**
source-shape/strict fuzzy and **2,520 linked-exact / 1,076 byte-exact**.

Unverified: asset packaging in a runnable host; actual gameplay use of
each constructed catalog entry; malformed or truncated catalog files;
and the full player-status-dependent constructor paths. The next
integration priority is the game-wide asset mount and an executable
station-to-flight test, not a synthetic exact-match claim for these
loaders.
