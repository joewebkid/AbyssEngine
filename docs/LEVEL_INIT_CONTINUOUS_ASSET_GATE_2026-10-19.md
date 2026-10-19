# Level init: continuous asset-backed prefix (2026-10-19)

Status: **partial**. The complete production bodies of `Level::Level`,
`Level::init` and `Level::createSpace`, compiled against the production class
headers, now execute continuously after the complete 2,488-record table
registration, from a poisoned Level allocation through stage-0 manager
construction and the original skybox loaders. The gate stops
explicitly at the first `PlayerStation` constructor. **Stage 0 does not return;
full Level initialization and rendering are not validated.**

Inspection covered `AGENTS.md`, `README.md`, `CONTRIBUTING.md`, current
project/validation notes, `Level.cpp/.h`, `StarSystem.cpp`, the station/player
constructor chain, `PaintCanvas`, the resource tables and constructors, and
the existing Level stage-0/skybox/constructor host tests. Production edits
are limited to `Level.cpp/.h`; the other changes are narrow host tests and
this note.

## Source-backed correction

Primary source: Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original and symbol maps were read in place through `GOF2_VERIFY_WORK`;
no original binary or asset was copied into this checkout.

`Level::Level(int)` at **0xadac8** had fixed ARM byte lengths in its C++
clears. They left pointer slots and the initialization stage uninitialized on
the 64-bit host. The old constructor test used uninitialized stack storage
without checking these fields, so its pass did not establish a safe entry to
`init`. Filling the allocation with `0xa5` reproduces the failure before the
change and passes afterward at both optimization levels.

| ARM anchor | Confirmed operation | C++ correction |
| --- | --- | --- |
| 0xadb20 | Clear 16 bytes at Level+0x90 | Clear four pointer slots using their host size |
| 0xadb28 | Clear 16 bytes at Level+0x84 | Same, from `particleRenderBoolPtr` |
| 0xadb30 | Clear 16 bytes at Level+0x74 | Same, from `field_74` |
| 0xadb34, 0xadb60 | Clear 0x65 bytes starting at +0xd8 | Clear from `asteroidWaypoint` through the `friendCargoStolen` byte using member offsets |
| 0xadb7a | Halfword zero at +0x188 | Also initialize `alarmRequested` at +0x189 |
| 0xadb9c | Halfword zero at +0x288 | Also initialize `supernovaFlareActive` at +0x289 |
| 0xadbb0 | Halfword zero at +0x29c | Also initialize `field_29d` |

ARM offset assertions lock the main clear endpoints (+0xd8/+0x13c) and stage
field (+0x134). Additional assertions lock all three four-pointer spans on
both ARM32 and the host. These are layout corrections, not new gameplay
rules or heuristic initial values. The function remains nonexact.

## Continuous execution and its boundary

`Level::init` at **0xadf94** calls the ten particle-manager constructors at
0xae058, 0xae0aa, 0xae0ce, 0xae0f4, 0xae11a, 0xae14a, 0xae170, 0xae19a,
0xae1c2 and 0xae1e4; **0xae1ee** calls `Level::createSpace` (**0xae5b8**).
The fixture follows that call, not a separate manual call to a skybox snippet.

The controlled external world state is ordinary orbit, system index 4,
texture index 2, station index 7 and campaign index 50. It is fixture input,
not a claim that this combination was loaded from the original galaxy
catalog. `Status` construction and the predicates/accessors used before the
boundary are production bodies, as are `SolarSystem` construction, the
default `Station`, Matrix and RNG behavior.

The run checks ten correctly initialized particle managers and their canvas
and resource IDs, then loads these four original Android assets:

| Resource | Original asset path below the supplied asset root |
| --- | --- |
| Mesh 0x45bb | `data/assets/main/3d/meshes/skyboxes/skybox_stars.aem` |
| Texture 0x2767 | `data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_001.aei` |
| Mesh 0x458a | `data/assets/main/3d/meshes/skyboxes/skybox_002.aem` |
| Texture 0x2753 | `data/assets/main/3d/textures/low/etc/skyboxes/skybox_002.aei` |

The entire 2,488-entry construction/registration block comes verbatim from
`BuildResourceList`, including mesh slots 1653 and 1667. Its GL prelude and
post-table tail are excluded. Two additional texture records come verbatim
from `LowResourceTable`; the fixture verifies 2,490 non-null records before
calling Level. This joins the previously separate full-table and entry gates.
Production resource constructors, bulk/individual registration, lookup,
resource-ID canvas calls, `MeshCreateFromFile`/`MeshReadData`, and
`ImageCreateFromFile`/`ImageRelease` run in the same process. The test checks
nonempty mesh/image data, assigned Level handles, seeded sky rotation, and
**four opens/four closes**. It uses the real `PaintCanvas` class layout.

After this, production `createSpace` allocates its four-slot landmark array
(ARM **0xaeb76–0xaeb7a**) and evaluates `Status::inEmptyOrbit` (**0xaeb82**).
The call to **`PlayerStation::PlayerStation(Station*)` at 0xaebe4** is an
intentional exception boundary in this fixture. No station object is
reported as successfully constructed. All other unimplemented dependencies
are explicit aborting link stubs. Returning from `Level::init`, or entering
any unexpected dependency, fails the gate. At the measured boundary,
`field_134` remains 0 and `player`/`starSystem` remain null.

Host file I/O, Engine/PaintCanvas bootstrap, material creation and GPU upload
are explicit service boundaries. Material IDs are checked but material
textures/shaders are not loaded. Image bytes are decoded before issuing test
texture handles. This is a source-extracted host gate, not a linked Android
runtime, complete `BuildResourceList` execution, native ABI proof, GPU test, or lifecycle
test of the complete Level destructor.

## Validation

Native `gof2` builds; ARM builds compile **208/208** units with zero failures.
The final assertion-only header rebuild also passes 208/208; all 208 object
files are byte-identical to the after-run corpus inputs.
The verifier unit suite runs **45 tests**, with one existing skip.

The following pass at **-O0 and -O2**:

- Poisoned production Level constructor, including pointers, stage and flags.
- New continuous `run_level_init_asset_gate.py` prefix with original assets.
- Existing guarded stage-0 allocation smoke.
- Existing skybox branch smoke and asset-backed selector/cache integration.

Focused Android comparison (source-shape / strict fuzzy):

| Symbol | Before | After | Exact? |
| --- | --- | --- | --- |
| Level C1/C2 | 95.0% / 75.5% | 95.0% / 75.0% | No |
| Level::init | 84.8% / 52.7% | 84.8% / 52.7% | No |
| Level::createSpace | 86.1% / 46.2% | 86.1% / 46.2% | No |

The small constructor fuzzy decrease is recorded; the patch corrects
confirmed writes and host initialization, not code-generation appearance.
Focused reports are `_work/level_init_before_focus.json` and
`_work/level_init_after_focus.json` in this checkout.

Fresh full-corpus before/after reports:
`_work/level_init_before_full.json` and `_work/level_init_after_full.json`.
Both compare **4,323/4,524 original functions** in **4,731 rows**, with 201
missing, zero skipped, zero wrong-type and 125 extra symbols. Linked-exact
stays **2,529** and byte-exact stays **1,076**. Explicit `(unit, symbol)` set
comparison gives **zero lost and zero gained pairs** in both categories;
the compact result is `_work/level_init_exact_set_delta.json`.
Mean source-shape remains **93.87%**; rounded mean strict fuzzy moves from
**83.06% to 83.05%**. Only C1/C2 constructor rows have changed metrics.

Reproduce the new gate (with UCRT64 tools on PATH):

```powershell
python tools/run_level_init_asset_gate.py `
  --cxx C:/msys64/ucrt64/bin/g++.exe `
  --out-dir _work/init-gate `
  --asset-root 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets'
```

## Remaining work

Connect the production station constructor, collision data and geometry/
material/transform consumers at the measured boundary, then the gate and
wormhole constructors, `StarSystem`, `createPlayer`, and stage 1. The current
fixture deliberately cannot certify those paths by substituting successful
objects. In particular, the later `StarSystem` constructor still allocates a
fixed 0x0c-byte target array and uses `ArraySetLength<unsigned int>` before
writing host pointers; that is a separate unchecked host-width hazard.

There is no required edit to `BuildResourceList.cpp` or `ResourceMaterial.cpp`
for this prefix. Full construction and material consumers remain dependencies
for extending it. Common project status/validation documents are left to the
coordinator; only this technical note belongs to this recovery package.
