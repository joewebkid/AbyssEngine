# Station binary reader and StarSystem entry — 2026-10-19

Base: published `35c4e0bdcd2bbbce2946689d0b23e008b489ec8b`.
Branch: `codex/astra-station-binary-20261019`, in the existing isolated
`_work/astra-gles-atlas-20261019` checkout. The earlier GLES branch and
`c855844e9bade52a600c6e70182759a5b4123a69` remain intact. No push, PR or merge.

The continuous original-asset gate now crosses `SolarSystem::getStations`
and the real `FileRead::loadStationsBinary(SolarSystem*)`, allocates the
StarSystem texture/index arrays and selects the first sun texture. It stops
**before `PaintCanvas::TextureCreate(10033, ...)` executes**. No successful
sun upload or sun handle is supplied. Neither `StarSystem` construction nor
`Level::init` stage 0 has completed; this is not a running native game.

## Source and changes

Inspected `SolarSystem.cpp/.h`, `StarSystem.cpp/.h`, `FileRead.cpp/.h`,
the relevant `AEFile::ReadSwitched`, `Station` constructor, `Array` layout,
resource registrations and the inherited Level/LensFlare/GLES fixtures.
The existing project status and validation documents were read, not edited.
Production changes are confined to `FileRead.cpp` and the StarSystem constructor.
`SolarSystem::getStations` already matches the original exactly and is unchanged.
ResourceTexture, String, ResourceMaterial and portrait tests are unchanged.

Primary source is Android 2.0.16 ARM ELF, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The saved IDA output was navigation only; claims below were checked against
raw Thumb instructions, call targets, rodata and dynamic relocations. No iOS
or PC body is substituted for Android evidence.

* `0x1558a2`: `ldr r0,[r0,#56]; bx lr` returns the Array pointer at SolarSystem
  `+0x38`, not its data pointer. ARM `Array<Station*>` is 12 bytes, data at +4;
  Station allocation at `0x1201fa` is 52 bytes.
* `0x12017a/0x120182/0x120192`: FileExist, temporary path String destruction,
  then OpenRead. The former local path survived to function return. The new
  condition uses a temporary, preserving the original lifetime.
* `0x12021c–0x12022e`: construct Station, publish it into the output array,
  destroy the by-value name temporary, increment the output position. The
  assignment now contains the new-expression, preserving publication order.
* `0x133dfe–0x133e24`: allocate a one-byte FileRead object, construct it,
  obtain the current system again, load, destroy and delete the reader **before**
  allocating either StarSystem array. The old stack reader survived to the end
  of the constructor. This is an original lifetime restoration, not a general
  recommendation to heap-allocate stateless readers.
* Constructor literals `0x133f50 + PC 0x133b88` and `0x133f54 + PC 0x133bd0`
  resolve to GOT `0x21015c` / `0x2101f4`, relocated to `Globals::status` /
  `Globals::Canvas`. Constructor accesses now use those globals, reacquiring
  status across calls, instead of `Status::gStatus`, a saved status pointer,
  and `PaintCanvas::gCanvas`.
* `0x133e28–0x133e5a` allocates texture count = loaded stations + 1 and index
  count = catalog IDs; `0x133eb8–0x133ece` selects the system sun resource.
  The 19 system textures, 34 colors and both 27 station texture tables agree
  with original 32-bit rodata at `0x202260`, `0x2022ac`, `0x202334`, `0x2023a0`.
  Existing compact C++ tables are not claimed byte-identical to that rodata.

`tools/verify/test_station_binary_arm.py` pins these anchors and table values.

## Actual runtime and fixture boundaries

`run_station_binary_asset_gate.py` builds at `-O0/-O2`. It uses the existing
real EGL/GLES gate and extracts current production bodies without rewriting
their control flow. Only ArraySetLength calls in the station reader and
StarSystem constructor are routed through a test observer that invokes the
real template with the same arguments, then retains the array addresses.
The test stops by throwing at the next sun TextureCreate entry; it does not
return fabricated station records, arrays, textures or GPU success.

Continuous run: 2,724 resource registrations; real Level, PlayerStation,
jumpgate and LensFlare prefix; 44 collision volumes, 17 meshes, 15 transforms;
three Image2Ds; one managed original atlas. All 4,194,304 RGBA payload bytes
are read back from the real GPU and compared. Then the actual station reader
reads records 0..9, returns all fields/names for IDs 5..9, closes at byte **247**,
and StarSystem requests sun **10033** with zero-initialized arrays of **6/5**.
The resource remains unresolved (`handle == -1`), and the original resource
path is `data/assets/main/3d/textures/low/etc/suns/sun_002.aei`.
Continuous file accounting is **19 opens / 19 closes**, the last being
`data/bin/stations.bin`. The caller's Level `starSystem` and `player` are still
null and its init stage remains zero.

Independent catalog tests read the original stations file through the same
production reader for **all 34 systems / 135 records**, checking every station
ID, system, tech level, texture, UTF-16 name, default field and early-close
byte position against an independent Python decoder. System 20 has unsorted
IDs; the original reader returns file order. Additional cases preserve
duplicates, empty selections, a missing ID, and absent-file null return
(an explicitly injected negative FileExist result; original files are not removed).
Empty/missing selections scan all 3,408 bytes. The original absent-file
allocation leak is preserved and documented, not silently repaired. Corrupt
or truncated files, null catalog pointers and allocation failures are not
validated. Standalone tests run **after** the continuous run and do not supply
its parser result or cached texture.

Bootstrap remains a fixture: Status is manually initialized (station 7,
campaign mission 50, Level mode 3), and the system object is constructed from
independently decoded **Pan** catalog data. Its original jumpgate ID 5 is
overridden to 7 solely to retain the previous two-jumpgate integration path.
This is not a recovered save state or a call through `loadSystemsBinary`.
Other catalog objects similarly supply explicit input conditions for the
reader, not manufactured outputs. Existing engine/canvas/manager bootstrap,
material services and successful skybox upload fixtures remain; shader/VBO
paths are disabled. No full scene destruction, render frame or gameplay loop
is established. Record cleanup in the standalone test releases the checked
empty fixture records directly and does not validate Station destruction.

Android uses the same sources with 32-bit ABI assertions. As in the inherited
GLES fixture, six duplicate ARM math operator definitions are omitted from
the generated AEMath translation unit in favor of their existing Vector/Matrix
definitions; this is an explicit executable-link boundary, not a production
math repair or permission to ignore duplicate symbol errors.

Original files are read in place from
`A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets`:

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| `data/bin/stations.bin` | 3,408 | `761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3` |
| `data/bin/systems.bin` | 3,612 | `03c2691492c18e9ac57811fc42a532532e14169cb52bfc766d464292b38bc3bd` |

Windows runs use the existing Android SDK ANGLE DLLs in place: NVIDIA GTX
1050 Ti / D3D11 driver 32.0.15.6094, EGL 1.5, ANGLE 2.1.22762 / ES 3.0.
Android runs use the attached M2007J20CG / Adreno 618, EGL 1.5 / ES 3.2
V@0502.0. Only freshly compiled test executables are pushed. Original files
are streamed read-only into process memory over temporary ADB reverse;
no original assets are copied to the device, and reverse mappings are removed.
No installed application or device setting is changed.

## Fixed comparison family and limits

The family was declared before edits: `SolarSystem::getStations`,
`FileRead::loadStationsBinary(SolarSystem*)`, and `StarSystem::StarSystem(int)`.
`tools/verify/station_binary_family.json` lists exact unit/symbol pairs; C2
is also reported as the C1 alias, not counted as another logical function.
No getters, empty destructors or unrelated exact methods are added to improve
the average.

| Function | Source-shape before → after | Strict fuzzy before → after |
| --- | ---: | ---: |
| getStations | 100.0 → 100.0 | 100.0 → 100.0 |
| loadStationsBinary(SolarSystem*) | 93.7 → 95.7 | 56.2 → 54.4 |
| StarSystem C1 (C2 identical metrics) | 78.9 → 76.6 | 18.2 → 16.2 |
| Three logical functions, equal weight | **90.8667 → 90.7667** | **58.1333 → 56.8667** |

Counting all four verifier rows, including C2 a second time, gives source-shape
**87.8750 → 87.2250** and strict fuzzy **48.1500 → 46.7000**. The target fails
under either convention.

**98% is not achieved.** The constructor remains the limiting body; the
source-backed lifetime/global fixes do not improve its whole-function metric.
The family average slightly falls. Neither the runtime prefix nor these
scores prove the remainder of StarSystem correct. In particular later
geometry placement, station texture/ring handling, raw target-array allocation
and cleanup remain beyond this gate. No family trimming or compiler-dependent
behavior change was made to hide this ceiling.
Later source audit must also check `moveForward(0x447a0000)` against its float
parameter contract and the fixed `0x0c` target-array allocation against a 64-bit
host. This gate reaches neither operation and does not clear those risks.

## Validation and reproduction

Native `cmake --build _work/native --target gof2 -- -j4` passes. This builds
`libgof2.a`, not a complete native game executable. The verifier test suite
passes **60 tests, one existing skip**. Both full ARM build phases compile
**208/208** translation units. Comparing all object SHA-256 values shows
**206 unchanged**, only `engine/file/FileRead.o` and `game/world/StarSystem.o`
changed. Windows and Android runtime gates both pass at `-O0/-O2`, including
their ABI, parser, continuous-path and real GPU assertions.
Focused complete-unit comparison covers 80 pairs in the two changed units:
10 byte-exact and 33 linked-exact pairs, with no losses or gains.

The fresh baseline full report SHA-256 exactly reproduces the published
combined-tree report: `d02e8536269ed0924aa6ef6e8da267b76876e8f4ac47d510eb0d2f311970ff71`.
The fresh final full corpus compares **4,323/4,524 original functions** in
**4,731 pairs**, with **93.90% mean source-shape**, **83.17% mean strict fuzzy**,
**2,545 linked-exact** and **1,079 byte-exact**. Skipped units and wrong-type
signatures are zero; the same 201 original functions remain outside compared
coverage. All normalized `(unit, symbol)` pairs are retained. Independent
set comparison finds **zero lost / zero gained** linked-exact and byte-exact
pairs; the 124 extra symbols are unchanged as a set, not just a count.

| Full evidence | SHA-256 |
| --- | --- |
| `_work/station-after-full.json` | `de8b90d3e712ab09068ace15f61a51bcba92ab644d8fbd0dd474466998923cce` |
| `_work/station-exact-delta.json` | `7499e0f26b99ddf72b5685a6384ebb3a664fc06783965c622b550e3f399831be` |

These results support the bounded native prefix and retain the prior corpus
baseline; they do not establish Level stage 0 completion, a full game launch,
or runtime correctness of the rest of the constructor.

From the isolated checkout, Windows runtime reproduction:

```powershell
$env:PATH = 'C:/msys64/ucrt64/bin;C:/msys64/usr/bin;' + $env:PATH
$stationAssets = 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets'
python tools/run_station_binary_asset_gate.py --out-dir _work/gles-host --asset-root $stationAssets --angle-dir 'C:/Users/Dima/AppData/Local/Android/Sdk/emulator/lib64/gles_angle' --egl-include 'C:/Users/Dima/AppData/Local/Android/Sdk/ndk/25.2.9519653/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include'
```

The first invocation regenerates inherited fixtures and runs the earlier
bounded Level regression. `--reuse-inputs` is for subsequent runs in that
same prepared output directory; it still refreshes the task's production
reader, constructor, ABI dependencies and original-file oracle. Android:

```powershell
python tools/run_station_binary_asset_gate.py --reuse-inputs --out-dir _work/gles-host --asset-root $stationAssets --ndk 'C:/Users/Dima/AppData/Local/Android/Sdk/ndk/25.2.9519653' --adb 'C:/Users/Dima/AppData/Local/Android/Sdk/platform-tools/adb.exe' --serial 'adb-45083544-vrdMP7._adb-tls-connect._tcp'
```

ARM source comparison uses the existing r18b compiler, independently of the
r25b Android executable compiler:

```powershell
$env:GOF2_VERIFY_LOCAL_NDK = '1'
$env:GOF2_NDK_ROOT = 'A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b'
$env:NDK = $env:GOF2_NDK_ROOT
$env:GOF2_VERIFY_WORK = 'A:/Galaxy on Fire new version/gof2hd-decomp-master/_work'
$env:GOF2_VERIFY_JOBS = '4'
python -m unittest discover -s tools/verify -p 'test_*.py'
python tools/verify/verify.py --build-dir _work/arm-verify --report _work/station-after-full.json --fail-on-wrong-type
python tools/verify/verify.py --no-build --build-dir _work/arm-verify --unit engine/file/FileRead --only 'loadStationsBinaryEP11SolarSystem' --report _work/station-after-reader.json
python tools/verify/verify.py --no-build --build-dir _work/arm-verify --unit game/world/StarSystem --only 'StarSystemC[12]Ei' --report _work/station-after-star.json
python tools/verify/report_station_binary_delta.py _work/station-before-full.json _work/station-after-full.json --output _work/station-exact-delta.json
```

No original binaries/assets, generated oracle includes, build products or
full machine reports are committed. Existing build directories are reused;
no cleanup policy is bypassed. Task evidence remains under `_work`:
`station-host.log`, `station-android.log`, `station-native.log`,
`station-tests.log`, focused `station-before/after-*.json`, full corpus reports,
`station-before-objects.json`, `station-object-delta.json`, `station-exact-delta.json`.

Completed runtime logs are pinned for this run:

* `station-host.log`: `e714e589034a4153a8b5886242c41b543314a4f49a46b7098a570b5cfaa229bd`
* `station-android.log`: `eadc5e3b6a26f44381ce576254fda535b090fdd1ff8d4f3a232cf59229d6e239`
