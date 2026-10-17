# Weapon-position asset-backed loader check (2026-10-17)

`tools/run_weapon_positions_asset_smoke.py` extracts the current
`FileRead::loadWeaponPositions` body and runs it at host `-O0/-O2`
against the local Android 2.0.16 `weapons_hd.bin` (7,484 bytes,
SHA-256 `eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82`).
The resource remains outside Git. The check supplies a small host
`AEFile` shim and uses the repository's `Array` implementation; it
does not execute the full game or the ARM binary.

Both optimization levels pass. The absent-file branch returns null
without opening a file. With the resource present, ship 63 (first
record) and ship 1 (later record) return four outer slots with the
expected weapon/turret/engine counts and transformed coordinates.
The file is opened and closed once for each successful request, and
the temporary path string is gone before `OpenRead`. The returned
arrays are released after assertions. The incremental native `gof2`
build remains green; a focused ARM recheck compiles **208/208** units
and retains **97.7% source-shape / 53.1% strict fuzzy** for the
nonexact loader (`weapon_positions_asset_smoke_focus_20261017.json`).

No production source changed in this package. The latest completed
ARM corpus therefore remains the Level cleanup report:
**4,323/4,524** originals over **4,723** comparisons,
**93.74%/82.88%** rounded average source-shape/strict fuzzy,
**2,520 linked-exact / 1,076 byte-exact**. This host test adds
behavioral evidence, not a new exact match. A running-game asset
mount and the reachability of missing IDs on legitimate paths remain
unverified.

One non-market path is now source-backed: Android ARM
`RecordHandler::recordStoreRead` at `0xcc0b8..0xcc0d0` reads a ship
ID from the save and immediately indexes `Globals::ships` for
`Ship::makeShip`, with no intervening ID check in that sequence.
The recovered `GameRecord` subsequently installs its ship into
`Status`. Thus a changed or malformed save can bypass the ordinary
market's ID exclusions. This does not show that a legitimate game
save contains one of the five IDs missing from `weapons_hd.bin`, nor
does it establish a safe EOF policy for the original loader. Other
direct/scripted ship-change paths and a full-game run remain open.
