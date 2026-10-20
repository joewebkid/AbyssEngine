# PlayerEgo Ship-defaults integration gate — 2026-10-20

The Pan original-asset gate now continues from `Level::init` through
`Level::createPlayer`, the source-derived `PlayerEgo` constructor prefix,
and its complete `MovingStars` constructor. It then runs the production
`Player::getCombinedHP`, `Ship` accessors/equipment and cargo queries,
`Globals::addSoundResourceToList`, and `Status::hardCoreMode` up to the
first HUD-image call. The extracted constructor test copy throws immediately
before `PaintCanvas::Image2DCreate(0x4c0, ...)`; neither HUD image creation
nor the rest of `PlayerEgo`/`Level::init` is covered.

Android 2.0.16 ARM (`libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `Player::getCombinedHP` at `0x99af8`, stores the result at
`PlayerEgo+0x130` at `0x99b02`, calls the boost delay/speed/time getters at
`0x99b22/0x99b30/0x99b5c`, and selects the handling-dependent sound ID
around `0x99bbc–0x99c64`. It calls `Status::hardCoreMode` at `0x99c9a`.
These are instruction anchors, not a claim that the entire constructor
matches byte-for-byte.

`tools/run_station_player_ego_stars_gate.py` passes on Windows SwiftShader
and an Android ARMv7 device at both `-O0` and `-O2`. In addition to the
previous 50-star checks, it asserts combined HP **470**, boost speed **7**,
delay **4200**, time **6300**, handling approximately **52.9**, shield
recovery **400**, free cargo space **50**, and one registered engine sound
resource **45**. It checks the absent equipment, cloak, drive and hardcore
branches for this fixture. All inherited 26/26 original-file operations
still close, and the level remains at stage 0.

The test Ship is synthetic with empty equipment. Its usual `refreshValue`
is deliberately bypassed by the inherited fixture; boost, agility, regen
and cargo-plus fields are assigned explicitly before entry. This does not
validate a catalog-populated Ship or the equipment-present branches. The
fixture also supplies procedural mesh storage and a sound-resource list;
those are not claims about recovered production services.

No production C++ changed. Native `gof2` remains green. The latest full
ARM corpus for this source revision remains **4,323/4,524** originals in
**4,735** pairs, **93.95%** mean source-shape, **83.24%** strict fuzzy,
**2,551** linked-exact and **1,079** byte-exact, with zero skipped units.
Next: exercise the original HUD image resources and equipment-present
branches, then finish `PlayerEgo` construction and `setShip`.

The later [HUD/sound gate](PLAYER_EGO_HUD_SOUNDS_ARM_2026-10-20.md) crosses
both of those checkpoints; its test boundary is still within the constructor.
