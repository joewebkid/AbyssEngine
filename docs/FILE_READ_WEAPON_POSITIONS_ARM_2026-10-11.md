# Weapon-position resource loader (2026-10-11)

Android 2.0.16 ARM `FileRead::loadWeaponPositions(int)` at `0x1212e4`
probes and opens `data/bin/weapons_hd.bin`. It reads the ship ID and
entry count as raw 16-bit values, then each entry's type and three
coordinates as raw 16-bit values. Type 3 additionally reads three raw
32-bit floats. The recovered loader used `weapon_positions.bin` and
byte-swapped the entry fields, including the floats. Those reads now
use the existing raw `AEFile::Read` overloads, matching the ARM calls.

A read-only walk of the local 7,484-byte reference resource consumes
exactly the file: 59 ship records, 552 entries and 236 type-3 extra
vectors, with valid type values 0..3 and no truncated field. For
example, the first header is little-endian ship 63 with 9 entries;
byte-swapping it would produce implausible values. The asset remains
outside the repository.

Native `gof2`, 206/206 ARM translation units, and 36 verifier tests
(one skipped) pass. Focused ARM comparison remains 83.1% source-shape
and 36.0% strict fuzzy; this loader is not linked- or byte-exact. The
runtime correction does not materially change the normalized opcode
shape measured by the verifier.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_core_loaders_full_20261011.json`, no
exact pair was lost; there are zero skipped rows and zero wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_weapon_positions_full_20261011.json`.

Unverified: a running-game read with the asset mounted at `data/bin`
and the behavior if a caller requests a ship absent from this file
(IDs 13, 14, 15, 50, 53). The original and recovered loops both stop
only when the requested ID is found, so those calls need a separate
call-path audit before changing EOF behavior.
