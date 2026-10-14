# PlayerEgo roll recovery (2026-10-14)

Android `PlayerEgo::roll(int)` at `0x9bbf8` is the primary source,
cross-checked against the IDA/Ghidra dumps. Thumb
`0x9bc14–0x9bd94` reads the ship matrix's floats at `+0x10/+0x14`,
caps the frame amount at 60, and resets the roll matrix and state
only when the second component is positive and the absolute first
component is below 0.015. The old source accepted zero for that
second component and delegated nearly all other behavior to a
declared-only `PE_roll_bankFactor` shim.

The recovered body implements the ARM rate table: ±0.00075 for
inverted banking, ±0.00025 or ±0.00035 for upright banking split at
first-component magnitude 0.3, ±0.00035 on direction reversal, and
±0.0002 while autoleveling. The original also checks the second
component for NaN before choosing the autolevel rate when the first
component is negative. Direction, autolevel and rolling state are
updated at their existing ARM offsets. The final rotation uses
`MatrixSetRotation(rollMatrix, 0, 0, rate * amount)` and assigns its
returned Matrix; the previous declared-only wrappers did not express
that contract.

The production-body host smoke covers the disabled path, exact-level
reset, zero-component boundary, frame cap, both bank directions and
thresholds, direction reversals, autolevel continuation and NaN case.
Native `gof2`, 206/206 ARM translation units and 37 verifier tests
(one skipped) pass. Focused source-shape/strict fuzzy rises from
**60.8%/38.6%** to **92.2%/67.5%**; the method is not exact.

The full ARM corpus report `_work/player_roll_full_20261014.json`
compares **4,323/4,524** original functions at **93.32%** average
source-shape and **82.23%** strict fuzzy. It retains **2,482
linked-exact** and **1,065 byte-exact** pairs, with **123 extras**
and zero skipped. Compared with the preceding turret-view report,
neither exact-pair set gained or lost a member.

The fixture substitutes Matrix math and does not prove animation
appearance or ARM32 runtime. Next priority: recover the adjacent
`handleShip` path and independently validate its camera/rotation
effects.
