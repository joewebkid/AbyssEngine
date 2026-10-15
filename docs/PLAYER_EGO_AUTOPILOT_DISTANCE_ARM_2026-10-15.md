# PlayerEgo autopilot waypoint distance (2026-10-15)

The Android 2.0.16 ARM binary is the primary reference. A targeted
REA/IDA read of `PlayerEgo::update` (`0x9ce38`) identified the waypoint
branch at Thumb `0x9e344–0x9e3e8`. The virtual target-position call at
`0x9e34c` is followed by `Vector::operator=` into `PlayerEgo+0xec`
(`0x9e356`). `PlayerEgo::getPosition` fills a local Vector (`0x9e362`),
which is subtracted from the stored target (`0x9e36e`). The result is
passed to `VectorLength` (`0x9e374`), converted to a signed integer,
and compared with `0x4e1f` (19999) before storing the approach flag at
`+0x330` (`0x9e38e`). The stored Vector is then passed to
`moveToPosition` (`0x9e3e4`). Existing ARM-only layout assertions fix
the three overlapping fields at `+0xec/+0xf0/+0xf4`.

The former C++ stored the target one float too early, discarded the
result of `getPosition()`, read an uninitialized local array, and
compared squared distance with 20000. That could flag a waypoint at
the wrong distance or depend on arbitrary stack contents. The revised
block stores and reuses the Vector at `+0xec`, computes the original
Euclidean length, and preserves signed integer truncation and the
inclusive threshold. The extracted-production smoke checks target
storage, a near point that distinguishes length from squared length,
and both sides of the truncated threshold. Adjacent update-beam and
update-maneuver smokes pass; native `gof2`, 206/206 ARM translation
units, and 37 verifier tests (one skipped) pass.

Focused `update` source-shape/strict fuzzy changes **80.1%/30.8% →
80.6%/30.8%** (2,171 → 2,190 normalized source tokens against
2,913 original). The function remains non-exact. This is primarily a
semantic correction, not a claim of full `update` recovery. The full
ARM report `_work/player_autopilot_full_20261015.json` compares
**4,323/4,524** original functions with **201 missing**,
**123 extras**, zero skipped, **93.41%** average source-shape,
**82.31%** strict fuzzy, **2,484 linked-exact** and **1,065
byte-exact**. The two exact symbol sets are unchanged from
`_work/player_drive_charge_full_20261015.json`. The iOS
1.1.4 analogous path and live waypoint behavior have not been checked;
REA's pseudocode was cross-checked against the Android instructions,
not treated as independent evidence. Next: controlled iOS guest trace
for a reachable autopilot/charge path and further `update` recovery.
