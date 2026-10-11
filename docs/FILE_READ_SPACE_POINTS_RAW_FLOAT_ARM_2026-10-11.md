# Dock-space point float reads (2026-10-11)

Android 2.0.16 ARM `FileRead::loadSpacePoints(int,int)` at `0x121550`
opens `data/bin/docks_hd.bin`, reads the two 16-bit record-header words
and the 16-bit point type with `AEFile::Read`, then reads nine 32-bit
floats per point with the raw float overload of `AEFile::Read`.

The recovered code had already restored the path and raw 16-bit reads,
but used `ReadSwitched(int32_t&)` for all nine float words. Those calls
now use `Read(float&)`, matching the ARM call sequence and preserving
little-endian float bits before position and rotation calculations.
The mapping of coordinates, the XZY rotation order, and group filtering
were not changed.

Native `gof2`, 206/206 ARM translation units, and 36 verifier tests
(one skipped) pass. Focused ARM comparison remains 75.7% source-shape
and 26.3% strict fuzzy; this function is neither linked- nor byte-exact.
The normalized metric does not capture the corrected read overloads.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_weapon_positions_full_20261011.json`,
no exact pair was lost; there are zero skipped rows and zero wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_space_points_full_20261011.json`.

Unverified: a resource-backed runtime parse. `docks_hd.bin` was not
available in the local extracted-resource set, so record values and
the caller-visible positions could not be checked independently.
