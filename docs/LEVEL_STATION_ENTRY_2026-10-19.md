# Level station entry: production constructor and real assets

Status: **partial native entry gate**. The continuous production
`Level::Level -> Level::init -> Level::createSpace` path now returns from
the actual `PlayerStation` constructor, builds both actual `PlayerJumpgate`
objects, and enters `StarSystem -> LensFlare`. It stops explicitly at
`PaintCanvas::Image2DCreate(0x508, ...)`. Neither `LensFlare` nor `StarSystem`
returns; `Level::starSystem` and `Level::player` remain null and stage stays 0.
This is not a complete initialization, rendering test, or playable native game.

Base: `009d6227499e94b1c17ffa10656b7be7912bfe57`, branch
`codex/astra-station-entry-20261019`. Original assets and the original shared
library were read in place, not copied or committed.

## Inspected sources and changes

Read the repository instructions, project/validation notes, the previous
continuous-entry note, `Level.cpp/.h`, `PlayerStation.cpp/.h`,
`PlayerStaticFar.cpp/.h`, `PlayerStatic.cpp`, `KIPlayer.cpp/.h`, `Player.cpp`,
`PlayerJumpgate.cpp`, `StarSystem.cpp`, `LensFlare.cpp`, `FileRead.cpp`,
`AEFile.cpp`, `AEGeometry.cpp/.h`, `AEGeometryPosition.cpp`, `PaintCanvas.cpp`,
`LODManager.cpp`, `Mesh.cpp`, `Transform.cpp/.h`, bounds/math/Array sources,
and existing resource/mesh/Level gate tools. Existing resource-table inputs
were inspected and reused; `BuildResourceList.cpp`, `ResourceMaterial.cpp`,
`tools/full_resource_table_smoke.cpp`, `docs/PROJECT_STATUS.md`, and
`docs/VALIDATION.md` were not changed.

Primary evidence is Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Android IDA extracts for PlayerStation and KIPlayer were a cross-check;
addresses below refer to the original ELF, not the host executable.

| Source anchor | Correction |
| --- | --- |
| PlayerStaticFar ctor 0x11c254; position writes 0x11c286; update 0x11c326/0x11c338 | Remove invented duplicate Player/position members; use inherited Player, initial position and two KIPlayer scratch vectors. ARM assertions lock the added fields at +0x130/+0x134 and size 0x140. |
| PlayerStatic ctor 0xe08cc, final argument at 0xe0912 | Pass `false` to KIPlayer, as the original does. |
| KIPlayer ctor 0xa57c8, condition 0xa5844 | Geometry wrapping depends on the final boolean argument, not a geometry handle being nonzero. |
| KIPlayer 0xa592c, 0xa593a/0xa593e, 0xa598a, 0xa5996 | Restore paired byte initialization and constants 5.0f / 0x40066666 instead of truncated byte assignments and zero placeholder globals. |
| PlayerStation ctor 0x121818, literal 0x121b9c | Maximum HP 9999999. ARM assertions lock root +0x140, index +0x148, radius +0x154, animation tail +0x170 and size 0x174. |
| 0x1218d6 | The missing-collision fallback excludes only stations 109/110, not 111. |
| 0x121bbe/0x121cfa, 0x121f1c | Restore the station-111 branch through mission 93. Preserve the distinct late threshold >94. Mission 94 does not use either special branch. |
| 0x121eb8/0x121ebc | Late special stations load static collision record 2002, not their station index. |
| Station ctor geometry calls; 0x121dfe/0x121e04 | Restore false clone arguments and the station-101 animation update; restore heap reader/ordinary child lifetimes and explicit release of collision data (0x1223e2). |
| 0x1222c6–0x1223c0 | Collision center remap is (-x, z, y); offsets are zero. Boxes use remapped extents, factor 1.0 or 0.9, then doubled dimensions. Spheres use absolute extent with factor 0.5 or 0.4. |
| 0x122400; literals 0x12256c/0x12257c/0x122580 | Restore byte +0x71 = 1 (meaning unconfirmed), LOD distance 70000, rotation pi about Y, and broad radius = mesh bound + 5000. |
| TransformAddMeshId 0x7b1c6, add 0x7b1ee–0x7b1f8 | Add the Mesh pointer itself, not its vertex stream. Use real Mesh/Transform arrays, bounds and animation members. |
| TransformAddChild 0x7b2ee; 0x7b284/0x7b348 via veneer 0x19b2a8 | Replace unresolved geometry helper shims with the confirmed bounds merge and `CollectAnimationData` calls. |
| BSphere::Merge(Transform) | Use the actual Transform members instead of a fixed +0xe0 read that is invalid when host Array pointers widen. ARM field meaning is unchanged. |

These changes are source-backed, not visual/gameplay heuristics. Names such as
the byte at +0x71 remain deliberately unnamed. Source-shape is a comparison
metric, not proof that every branch or lifecycle is recovered.

## Continuous execution and explicit boundaries

`tools/run_level_station_asset_gate.py` first runs the previous continuous
gate at both optimizations and reuses its verbatim resource-table and Level
bodies. The new executable uses production headers/vtables. PlayerStation,
PlayerStaticFar, PlayerStatic, PlayerJumpgate, AEGeometry, Mesh, Transform,
math and bounds are compiled from their source files. The necessary KIPlayer,
Player, FileRead, canvas, StarSystem and LensFlare bodies are extracted
verbatim. Extraction/host replacement markers are checked rather than silently
accepting a changed source boundary.

The fixture retains the previous controlled state: ordinary orbit, system 4,
race 0, texture 2, station 7, campaign 50, and seeded production RNG. It is
not a catalog-loaded galaxy. `Status::gStatus`/`Globals::status` and the canvas
globals are bound to the same fixture objects. The original table registers
2488 records plus the same two low-resolution skybox texture records.

Measured path:

1. Poisoned Level allocation and production constructor, then all ten stage-0
   particle managers and `createSpace`.
2. Both skyboxes through real AEM/AEI loaders.
3. The call at **0xaebe4** returns from production PlayerStation **0x121818**,
   including the PlayerStaticFar/PlayerStatic/KIPlayer/Player chain,
   `FileRead::loadStationCollision`, real station meshes and collision volumes.
4. The loop call at **0xaeeee** constructs both production PlayerJumpgate
   objects (**0xa4fa4**), their animation handles and LOD geometry. The second
   gate exercises the real animated Mesh/Transform clone path.
5. Production StarSystem **0x133b68** calls LensFlare at **0x133bd6**.
   Production LensFlare **0x11d00c** reaches its first Image2DCreate call at
   **0x11d03c**, ID **0x508**. The gate checks that the image resource exists
   and is uncached, then throws a dedicated boundary exception.

Returning from `Level::init`, missing the boundary, or invoking an unsupported
dependency fails. All remaining link services in `level_station_traps.inc`
abort; no successful station, jumpgate, StarSystem or LensFlare substitute is
returned. `createPlayer` and later stage methods remain traps.

The measured fixture creates **17 meshes**, **15 transforms**, and **44 station
collision volumes** (23 boxes, 21 spheres), with **14 opens / 14 closes**.
The gate asserts the ordered file list and material requests. Original files:

- Existing four skybox AEM/AEI files from the preceding gate.
- `data/bin/collision.bin`.
- `data/assets/main/3d/meshes/stations/`: `station_007_terran.aem`,
  `station_007_terran_emissive.aem`, `station_007_terran_lights_add.aem`.
- `data/assets/main/3d/meshes/jumpgates/`: `jumpgate_terran.aem`,
  `jumpgate_terran_emissive.aem`, `jumpgate_terran_anim_add.aem`,
  `jumpgate_terran_jump_anim_add.aem`, `jumpgate_terran_lod_1.aem`,
  `jumpgate_terran_lod_2.aem`.

Independently decoded collision-record values check the first box center
(-25951, -273, 25988), half-extents (5859, 5028, 5918), and sphere 22 center
(-2, 9865, 9357), radius 2000. Geometry, inherited Player identity, LOD distances,
visible/hidden gate state and radii, nonempty mesh data, box outer inclusion,
sphere inclusion, and station sphere/outside collision queries are asserted.
The LOD manager contains the two gates; this ordinary station has no LOD.

Host boundaries remain **file I/O, Engine/PaintCanvas bootstrap, material
creation and GPU upload**. Material IDs are checked (20010, 20011, 20004,
20003, and absent ID 3) and produce no material; station textures/shaders are
not loaded. ID 3 is absent from the table and production lookup would leave
the invalid handle unchanged. Real skybox image bytes are decoded before
issuing test texture handles. VBO support is explicitly false; the real
`MeshConvertToVBO` capability check runs on the clone path, while both actual
VBO upload functions are aborting traps.
The inherited host MeshRelease implementation is also replaced with an
aborting boundary in this gate; a cleanup path is not silently certified.

The next dependency, Image2DCreate, still uses unresolved resource/texture/
region adapters. Passing it by returning a fabricated image would not validate
StarSystem. Later StarSystem also has a fixed-width target-array allocation
that remains unsafe on the host. No claim is made for those operations.

## Validation and comparison

Validation passed:

- Native UCRT64 Release `gof2` build, including the final header changes.
- Final NDK r18b ARM build: **208/208**, zero failures.
- Verifier unit suite: **46 tests**, one existing skip.
- New continuous station gate and preceding continuous Level gate at
  **-O0 and -O2**; identical boundary and asset/count assertions in both.
- Existing guarded stage-0 allocation, skybox branch, original-asset skybox
  integration, and full resource-table HD/iPad/Retina/default-platform gates,
  each at **-O0 and -O2**. Existing mesh parser/color/truncation/lifecycle
  smokes also run through the gate preparation.

Focused PlayerStation comparison against the same original ELF:

| Scope / metric | Before | After |
| --- | --- | --- |
| Constructor C1/C2 source-shape | 78.5% | 90.6% |
| Constructor C1/C2 strict fuzzy | 24.9% | 23.4% |
| Constructor byte-exact / linked-exact | No / No | No / No |
| All 23 unit comparisons, mean source-shape | 95.53% | 98.57% |
| All 23 unit comparisons, mean strict fuzzy | 76.30% | 86.53% |
| Unit linked-exact / byte-exact pairs | 8 / 3 | 16 / 4 |

The **unit average above 98% is not the constructor's score**. The strict
constructor score decreased and is reported explicitly. No byte-match claim
is made for the constructor. The focused after report is
`_work/station-after-focus-final.json`; before rows come from the fresh full
baseline report, not an older project's status number.

Fresh full-corpus before/after reports are `_work/station-before-full.json`
and `_work/station-after-full.json`. Both contain **4731 comparisons**,
**4323/4524** original functions covered, **201 missing**, **zero skipped**,
**zero wrong-type**, and **125 extra symbols** (the extra-symbol sets also
remain identical).

| Full-corpus metric | Before | After |
| --- | --- | --- |
| Mean source-shape | 93.87% | 93.90% |
| Mean strict fuzzy | 83.05% | 83.16% |
| Linked-exact pairs | 2529 | 2545 |
| Byte-exact pairs | 1076 | 1079 |

Explicit normalized **(unit, symbol) set comparison**, not just net totals:

- Linked-exact: **0 lost, 16 gained**. Gains are PlayerJumpgate
  `timeToJump`, `animationEnded`, `addJumpAnimationHandle`; PlayerStatic
  C1/C2; PlayerStaticFar `getInitPosition`, `getProjectionVector`,
  `projectCollisionOnSurface`; PlayerStation `ArrayRelease<int>`,
  `getPosition`, `setPosition`, `getProjectionVector`,
  `projectCollisionOnSurface`, `render`, `getRoot`, `translate`.
- Byte-exact: **0 lost, 3 gained**: PlayerJumpgate
  `addJumpAnimationHandle`, PlayerStaticFar `getInitPosition`, and
  PlayerStation `getRoot`.
- One nonexact comparison row (`PlayerStation / ArrayRemoveAll<int>`) is
  replaced by `PlayerStation / ArrayRelease<int>`, matching the original call.
  The total row count is unchanged. There are 36 common rows with changed
  metrics, including effects of the inherited-layout correction in callers.

The complete pair lists, changed-row metrics and per-unit summaries are in
`_work/station-exact-set-delta.json`; per-unit before/after extracts are
`_work/station-focus-*.json`. Some nonexact scores decrease:
`TransformAddMeshId` source-shape is 91.5% -> 90.9% while strict fuzzy is
33.6% -> 50.8%; the source-backed pointer/animation correction is retained.
`BSphere::Merge(Transform)` has unchanged ARM metrics; its correction matters
for host pointer width. No old exact pair is hidden by either change.

Report SHA-256 values:

```text
before full: 655faca7b0a36d9fcb8cabcaf355288e6f98875cfa6c16fed1cb6a9f61c4ec0e
after full:  6b952e3106556988884e06125eedee4468decd645cd5aa1c5c1a34692a86e7b3
pair delta:  03dcc16ea9a0473459d0b076046a3f5d41543ed1986fa16133c042d2464c73a5
```

Reproduce the gate with MSYS2 UCRT64 tools on PATH:

```powershell
python tools/run_level_station_asset_gate.py `
  --cxx C:/msys64/ucrt64/bin/g++.exe `
  --out-dir _work/station-gate `
  --asset-root 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets'
```

## Remaining risks

- The new continuous asset fixture covers ordinary station 7/race 0 only.
  The special station branches and alien collision factors are source-backed
  changes but have not been executed with their original assets here.
- PlayerStaticFar's frame-update camera/settings placeholders remain; the
  constructor gate does not execute the frame loop.
- Some rare station-108 wrapper lifetimes and repeated canvas lookup structure
  still differ from the original. Constructor source-shape remains below 98%.
- The existing leaf `BoundingAAB::collide` delegates to its child-volume base
  behavior; this gate tests box `outerCollide`, not a complete combat collision
  model. No general collision/lifecycle correctness is claimed.
- Full Level destruction, rendering, GPU, audio, controls, wormhole creation,
  createPlayer, subsequent stages and Android native execution are untested.
  The native `gof2` target is a build gate; the host executable is a bounded
  source-extracted runtime gate. Neither replaces an ARM device/runtime test.
