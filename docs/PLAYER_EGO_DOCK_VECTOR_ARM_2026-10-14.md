# PlayerEgo dock-vector reset (2026-10-14)

Android `PlayerEgo::dockToAsteroid` at `0x9f4a0` and
`PlayerEgo::dockToDockingPoint` at `0x9f9f4` both zero a 12-byte stack
vector and pass it to `AEMath::Vector::operator=` with destination
`PlayerEgo+0x1c8`. Raw Thumb at `0x9f4c8–0x9f4e2` and
`0x9fc0e–0x9fc2c` confirms the address and call. IDA shows the same
zero/reset branch.

That storage was exposed only as three adjacent integer fields:
`dockApproachDist`, `dockApproachThreshold` and `field_0x1d0`.
The header now overlays a named `Vec3` over those same 12 bytes,
preserving the existing field accessors. ARM-only layout assertions
pin the vector and following fields to `+0x1c8/+0x1cc/+0x1d0`.
Both exit paths use the real vector assignment instead of a
declared-only `Vec_assign` function.

The existing production-method docking smoke checks that all three
words clear on asteroid undock and leaves the rest of the docking,
cargo and repair scenarios intact. Native `gof2`, all 206 ARM
translation units and 37 verifier tests (one skipped) pass. Focused
source-shape/strict fuzzy remains **99.4%/88.2%** for
`dockToAsteroid` and **77.7%/54.3%** for `dockToDockingPoint`;
neither function is exact. This is a linkage and layout recovery,
not a claim of a higher source-shape score.

Full report `_work/player_dock_vector_full_20261014.json` compares
**4,323/4,524** original functions at **93.30%** average
source-shape and **82.21%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units.
Linked-exact, byte-exact and extra-symbol sets are unchanged from
`_work/player_maneuver_full_20261014.json`.

The full docking-point branch, live game resources and ARM32 runtime
have not been exercised. Other `PlayerEgo` methods still use
declared-only vector wrappers. Next: recover the auto-turret position
and distance path, which currently passes an uninitialized position
to `Vec_sub`.
