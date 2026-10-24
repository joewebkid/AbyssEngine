# Pan first-friend PlayerFighter constructor (2026-10-24)

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
exports `PlayerFighter::PlayerFighter` at `0xdc2f8`. The unchanged production
constructor is source-extracted into
`tools/run_level_stage1_first_friend_fighter_ctor_pan_gate.py`. The gate
loads the original 233-item Android catalog before stage one, runs the
existing original-asset Pan chain through production `Level::createShip`'s
Player/EMP prelude, and catches its explicit boundary. Immediately after
the prior assertion that the Pan RNG and first-friend call arguments match
the independently replayed stage-one sequence, it calls the real Fighter
constructor with those recorded arguments and live state.

The constructor completes its real `KIPlayer` base, random route and clone,
`Generator::getLootList(-1,-1)`, `Explosion(0)` and `addFireStreaks` calls.
The fixture checks the player back-reference, fighter type and position,
non-alien radius, looped two-to-four-waypoint route and distinct clone,
explosion type and three-to-nine streaks, plus hitpoints. The inherited Pan
fixture uses a minimal `Item` constructor, so the production `Item::init`
is run on each original catalog entry before stage one. Uncalled Fighter
virtual methods are test-only fail-closed stubs; none fired. This is not a
full `PlayerFighter` translation-unit or lifetime test.

NDK r18b built ELF32 ARM executables that passed on an Android 16 device at
`-O0` and `-O2`, with original OBB assets served read-only through ADB
reverse. Executable SHA-256: `-O0`
`3b0b850b5bb8cf4055e2352162f151d381f1c8f28a7dac08f9b4898f2789d68e`,
`-O2`
`5908a1b91295865fff10f7d406bb37accc19cea5819e7fe29b611fef4c47a0f8`.
Native `gof2` remains green. The temporary device executables were removed.

The latest full-corpus report at this unchanged production revision has
**4,323/4,524** compared original functions across **4,736** pairs,
**94.11%/83.43%** mean source-shape/strict fuzzy, **2,557 linked-exact**
and **1,082 byte-exact**, zero skipped or wrong-type symbols. The Fighter
constructor's C1/C2 aliases are individually **94.5% source-shape / 38.6%
strict fuzzy**, neither exact. The new runtime gate does not change those
scores or remeasure exact-pair identities.

The gate stops before the remainder of `Level::createShip`, fighter
`setShipGroup`/`setLevel`, enemy-array insertion, update/render and
destruction. Its allocated Fighter remains until process exit. Next,
execute the `createShip` continuation with real geometry and manager
services, then check insertion and ownership on ARM before claiming that
the first ordinary friend is fully created.

Update 2026-10-24: the class-zero continuation and first insertion now
pass on Windows SwiftShader; [see the bounded host gate](LEVEL_STAGE1_FIRST_FRIEND_SHIP_RETURN_HOST_2026-10-24.md).
The ARM device continuation and lifetime check remain open.
