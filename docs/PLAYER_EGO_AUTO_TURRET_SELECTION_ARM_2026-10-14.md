# PlayerEgo auto-turret selection (2026-10-14)

Android `PlayerEgo::handleAutoTurret(int)` at `0x9ca78` scans enemies
when its accumulated timer reaches 3001 ms. Thumb
`0x9cac8–0x9cb32` confirms that each eligible candidate's virtual
`getPosition()` result is subtracted from the
PlayerEgo position cache at `+0x148`, then `VectorLength` is converted
to an integer before comparison with the initial 60,000-unit limit.
IDA and Ghidra show the same filter and call sequence. The former
source discarded `getPosition()` and passed an uninitialized array
to declared-only `Vec_sub` and `Vec_length` wrappers.

The selection loop now uses the returned `Vector` and existing
`AEMath::operator-`/`VectorLength`. Thumb `0x9cde0–0x9cde4` also
confirms that the no-target/timeout path calls
`Player::stopShooting(2)`, not `stopShooting(0)`.

The production-body host smoke checks dead, inactive, friendly and
blocked filters, nearest-target selection, the 60,000-unit boundary,
virtual position calls and the stop-weapon argument. Native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped)
pass. Focused source-shape stays **45.7%** and strict fuzzy rises
**23.5% → 24.0%**; the whole method is not exact.

Full report `_work/player_auto_turret_full_20261014.json` compares
**4,323/4,524** original functions at **93.30%** average
source-shape and **82.21%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Its
exact-pair and extra-symbol sets are unchanged from
`_work/player_dock_vector_full_20261014.json`.

The aim/fire section after target selection still calls the
declared-only `PE_hat_aimAndFire` shim and does not reconstruct the
original transform, rotation and shooting path. The host smoke
substitutes that shim and does not validate live aiming or ARM32
runtime. Next: restore that remaining aim/fire section from Android
Thumb and verify the complete method.
