# Asset-backed Status::resetGame startup (2026-10-17)

The Android 2.0.16 ARM function `_ZN6Status9resetGameEv` at `0x0ad304`
reads settings from `Globals::options`, resets `Level` jump state, reads
the item and ship catalogs from `Globals::items` and `Globals::ships`,
places `Mission::empty` in the second mission slot, clones catalog ship
10, and selects station 78 (`0x0ad7e8`–`0x0ad84a`). It then clears cargo
and equips six catalog items at indices 2, 2, 54, 59, 82 and 73,
followed by item 36 with amount 6 (`0x0ad854`–`0x0ad908`). The prior
source instead dereferenced a null ship, used station 0, and called
null function pointers through integer-truncated native pointers.

The same ARM binary's `BluePrint::BluePrint(int)` at `0x176f20` reads its
template from `Globals::items` (`0x176f42`). The ingredient-list,
quantity-list, ingredient-value and auto-completion-price methods do the
same at `0x177134`–`0x177332`. The prior C++ used `Item::g_items`, which is
never assigned in the recovered initialization path. All five BluePrint
bodies now use the observed catalog pointer.

Focused ARM verification for `Status::resetGame` rose from **32.9% to
88.1%** source-shape and **15.4% to 34.7%** strict fuzzy. Both variants
of the BluePrint constructor remain **99.4%** source-shape and **94.5%**
strict fuzzy; none of these functions are linked- or byte-exact. The
focused BluePrint family retains all **22 linked-exact** and **12
byte-exact** comparisons after replacing the catalog pointer. The
whole-corpus report at
`A:/Galaxy on Fire new version/_work/status_reset_blueprint_full_final_20261017.json`
compares **4,323/4,524** originals in **4,730** rows, averaging
**93.78%** source-shape and **82.94%** strict fuzzy. It has **2,526
linked-exact / 1,076 byte-exact**, 201 missing originals, zero wrong-type
and zero skipped units. Pairwise comparison with the preceding full
report finds zero exact losses and five linked-exact gains.

The native host fixture extracts the production startup, loaders,
Galaxy, Status, Mission, BluePrint, Item and Ship methods, mounts seven
local Android catalogs, and executes the new-game path at `-O0` and
`-O2`. It checks ship 10, station 78, the campaign mission and empty
mission slot, 27 agents, 25 wanted records, the complete catalog-derived
blueprint count, both settings flags, cleared jump state, all seven
equipped items and amount 6 on the final item. It also computes an
actual blueprint's ingredient value and completion price against the
catalog. **56/56** file
open/close pairs balance. Fixture boundaries remain for the Globals
constructor/destructor and hints reset, Station destructor, and text
services; this test does not exercise a platform application host.

Unverified: invalid/missing catalog paths through reset, repeated
reset with a populated save, downstream departure and flight, and
Android-device behavior. Other `Item::g_items` readers in Galaxy,
HangarList, MGame and ListItemWindow still need source-backed catalog
reconciliation before those paths are trusted. The next milestone is a
station-to-flight runtime slice after inventorying the departure and
platform boundaries.
