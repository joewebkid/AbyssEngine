# PlayerEgo / MovingStars constructor-prefix gate — 2026-10-20

At this checkpoint the Pan original-asset gate followed the continuous `Level::init` →
`Level::createPlayer` → `PlayerEgo::PlayerEgo` call path through the real
`MovingStars` constructor. The test boundary is immediately after
`PlayerEgo` stores the stars pointer, before `Player::getCombinedHP`.
The extracted test copy of `Level::createPlayer` ends after its constructor
call; the full `PlayerEgo` constructor, `setShip`, `Level::init` stage-0
return and a rendered starfield are not established by this gate.

The Android 2.0.16 ARM binary (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
allocates 0x1c bytes, calls `MovingStars::MovingStars` at `0x99aee`, stores
its pointer at `PlayerEgo+0xf8` at `0x99af4`, then calls
`Player::getCombinedHP`. The `MovingStars` constructor at `0x146050`
allocates four 50-entry arrays, initializes each life to -1, makes 50
random draws and calls `Globals::createBillBoard`/the canvas transform
methods before requesting texture 10001. The production billboard method
uses material 20052 and 12 vertices / six triangles.

At this checkpoint `tools/run_station_player_ego_stars_gate.py` ran the inherited original
station/system/mesh/texture Pan path and then checks the constructor prefix
on Windows SwiftShader and Android ARMv7 at `-O0/-O2`. All four runs pass.
The new assertions check 50 distinct meshes and transforms, the 50 material
requests, mesh vertices/UVs/triangle indices, one mesh attached to each
transform, their 100000-unit parked translations, 50 RNG draws, life and
animation flags, and a null dock-camera pointer. The current Pan resource
fixture has no texture 10001, so the texture handle remains zero; this
does not prove the eventual native rendering path.

Only the fixture's procedural mesh allocation and point/UV/triangle writes
are test services. The `PlayerEgo` prefix, complete `MovingStars`
constructor, `Globals::createBillBoard`, RNG, transform creation/attachment,
resource list and all inherited original-file/GLES operations are production
bodies. The gate observes 68 meshes and 72 transforms after the new segment,
with the earlier 26/26 original-file reads still closed. No production C++
changed. Native `gof2` remains green, and the last
full ARM report for the same source revision remains 4,323/4,524 original
functions in 4,735 pairs, 93.95% mean source-shape, 83.24% strict fuzzy,
2,551 linked-exact and 1,079 byte-exact, with zero skipped units. Its
SHA-256 is `15bdcdc9a36710df8d91717f08a6019783d92baa949857ff3b84b556db8de5c3`.

Next: carry the production `PlayerEgo` constructor past `getCombinedHP`
with catalog-backed Ship/equipment and its image/sound dependencies, then
return through `Level::createPlayer` into `setShip`.

The later [Ship-defaults gate](PLAYER_EGO_SHIP_DEFAULTS_GATE_2026-10-20.md)
crosses `getCombinedHP` and the empty-equipment Ship branch, but its
boundary still precedes the first HUD image.
