# Record reader definition tables — Android ARM (2026-10-11)

Original Android `RecordHandler::recordStoreRead` at `0xcbd34` indexes
the data arrays of `Globals::ships` and `Globals::items` when
rebuilding the player's ship, equipment, cargo, station inventories,
wingman, and station ships. The recovered source instead dereferenced
two translation-unit-local null pointers. Those ten lookup sites now
use the live `Array<Ship *>` and `Array<Item *>` objects. Ship
prototypes are no longer converted through a 32-bit `int` before
`makeShip(-1)`, which would truncate pointers in the native build.

Native `gof2` and all 206 ARM translation units build; 36 verifier
tests pass (one skipped). Focused `recordStoreRead` source-shape
improves from 26.4% to 91.6%, strict fuzzy from 14.1% to 16.9%.
Full ARM corpus: 4,323/4,524 original functions compared, 92.18%
average source-shape, 80.37% strict fuzzy, 2,350 linked-exact,
1,008 byte-exact, zero skipped units and zero wrong-type symbols.
No previously exact pair regressed. Seven newly emitted `ArraySetLength`
template symbols explain the linked-exact increase; the 14 new
comparisons also account for the slight drop in overall strict fuzzy.

The native target here is a static library, so its successful build
does not resolve every external reference. This reader still calls
four declared but unimplemented helpers (`Galaxy_getStationByIndex`,
`Standing_new`, `Standing_setStandingsArr`, `RH_str_make`). A final
native link and a real save/reload test remain required; resolving
those helpers is the next bounded priority.
