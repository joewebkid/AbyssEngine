# StarMap Android ARM recovery Р В Р вЂ Р В РІР‚С™Р Р†Р вЂљРЎСљ 2026-10-02

## Status and scope

**PARTIAL: the requested 99.0% target is not reached.** The fixed 20 distinct
implementation bodies measure **95.65% unweighted / 91.29538% original-instruction
weighted source-shape**. The clean baseline measures **83.81% / 73.61763%**.
Strict ordered fuzzy matching improves from **48.29% / 28.05016%** to
**69.74% / 45.21660%**, respectively. Source-shape is an inventory/control-flow
metric, not executable equivalence or a percentage of proven behavior.

- Clean baseline: `f60694dd0ca578831cec832e7026b32644642d97`.
- Isolated branch: `codex/starmap-arm-99-20261002`.
- Isolated checkout: `A:/gof2-starmap-99-20261002`.
- Changed production files: `src/game/world/StarMap.cpp`, `StarMap.h` only.
- Added validation: `tests/check_starmap_source_shape.py`, this report and its
  compact evidence snapshots. No verifier, shared configuration, Level, Gun,
  PlayerTurret, TargetFollowCamera or Godot files were changed.
- Reviewed the 2026-09-24 report and rejected forms before changing code.
  Main checkout and other agents' worktrees were left untouched.

The denominator remains **7101 original instructions** in the same 20 bodies.
Constructor C2 and destructor D2 aliases are checked against C1/D1 but excluded
from both means. No bodies, weights or verifier settings were changed to raise
the result. No volatile/register barriers, dead-code padding or synthetic
explicit template instantiations were added.

## Evidence and version boundaries

Primary reference is **Android ARMv7 GOF2 HD 2.0.16**,
`_work/bins/android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Method bodies were checked in the local IDA dump
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`;
direct ARM disassembly and ELF data were used where the decompiler was ambiguous.

Android anchors: constructor `0xc67a4`, init `0xc6bf0`, destructor `0xc749c`,
initStarSystem `0xc7580`, initLights `0xc7d9c`, setStart `0xc7dd8`,
depart `0xc7e0c`, isInPlanetMode `0xc8040`, setJumpMapMode `0xc804c`,
renderBG `0xc8056`, render `0xc8058`, drawKey `0xc80e8`,
drawOnScreenInfo `0xc8354`, draw `0xc90f4`, missionChanged `0xc9b48`,
askForJumpIntoAlienWorld `0xc9b50`, update `0xc9b90`,
OnTouchBegin `0xca7e0`, OnTouchMove `0xcab0c`, OnTouchEnd `0xcae90`.

**iOS HD 1.1.4 cross-check only:** version verified from the local app's
Info.plist (`CFBundleVersion` and `CFBundleShortVersionString` both `1.1.4`).
IDA functions `sub_1C3E5C`, `sub_1C4398`, `sub_1C8418` confirm the inline
EaseInOut objects, geometry position access, constructor scales, perspective,
marker interpolation and station world-matrix composition. iOS Status offsets
differ (target-system access +120 versus Android +124); they were not substituted
into the Android layout.

**PC release data cross-check only:** the local July-22 comparison does not
identify an executable/version number. It is therefore labelled an unidentified
PC release dataset, not an Android implementation reference. PC stations.bin
SHA-256 `65cebd57aa5cb8229ff35dfa91a15b70c55218a98697ce7617c329bb8e157572`;
the comparison report is
`analysis/gof2_pc_android_bin_comparison_20260722/pc_android_bin_diff.json`.
All 109 common station records agree; PC has 27 systems versus Android 34,
and 23 of the 27 common systems agree, with four Android route extensions.
PC cardinalities and routes were not used to overwrite the Android map.

## Accepted source changes

- Restored interior Android object offsets rather than retaining a misleading
  correct total size with shifted members. Three 16-byte inline EaseInOut objects
  replace incorrectly declared 12-byte vectors; missing slots are represented
  explicitly, the fabricated trailing padding is removed, and 32-bit compile
  assertions check the allocation size `0x1e8` and important offsets.
- Replaced local pseudo-sret casts and unresolved wrappers with typed geometry,
  matrix/vector, Station, SolarSystem, FileRead and path-finder calls. Android
  return-by-value ABI now comes from their declarations. Canonical Globals
  accesses restore service loads and call ordering.
- Constructor restores Android map scales `.012/.018/.025`, extents,
  selected-system marker position and rotation. init restores the typed station
  loader, mission/station lookup, fallback positions, camera transform and
  route/no-route branches.
- initStarSystem restores typed scale/texture overloads, initial/root rotation,
  station selection, lighting service calls and the planet-ring branch. The
  ring mask is the ARM literal `0x00210200`; IDA's apparent `Item::makeItem`
  pointer was rejected after checking literal-pool bytes.
- draw restores route visibility/pulse, route animation, race/security labels,
  header and cargo/jump-cost footer. update restores inline and heap easing,
  station world-matrix composition using station geometry index +1, marker
  position/scale interpolation, inertial motion, signed yaw wrapping, pitch/pan
  limits and path overshoot halving.
- Touch handlers restore drag signs/threshold, reverse easing on leaving planet
  mode, transition position, accepted jump outputs, equipment/passenger/cargo
  restrictions and the localized warning branches visible in Android.
- drawOnScreenInfo restores clipping, name lifetimes, repeated service getters,
  station inverse alpha, galaxy alpha, security colors and Android icon slots.
  drawKey uses the directly confirmed six-argument drawBox overload with a
  temporary String; the extra flags argument was rejected by direct disassembly.
- initLights now calls FogEnable(0, 1); setStart clears a deleted route array;
  missionChanged reads the corrected offset. These and askForJumpIntoAlienWorld
  gain exact comparisons.

## Rejected/intermediate forms and remaining uncertainty

- Initial layout assertions exposed omitted interior gaps; that form was rejected
  before accepting the ARM build. Compile errors from legacy getStation/GetEngine
  return declarations and one remaining Station_getSystem shim were corrected
  locally, without changing their shared declarations.
- A temporary Array<bool> spelling removed the existing Array<uint8_t> constructor
  exact pair from this translation unit. It was rejected. Byte-valued 0/1 storage
  is retained and documented; original Android template spelling is not claimed
  to be reproduced. No dummy instantiation was used to conceal the loss.
- An intermediate icon layout scored slightly higher but contradicted the
  Android mission/warp-gate slots; the semantic correction was retained.
- Focused packages are preserved locally as layout-trial2, draw-update-trial,
  touch-key-trial, init-typed-trial2, station-move-trial2, info-trial,
  info-reloads-trial, legend-icons-trial and globals-trial JSON/logs under _work.
- The large bodies remain below 99%; cached restrictions, temporary String
  construction, branch ordering and emitted instruction/control inventories
  require further work, particularly OnTouchEnd, init and draw. No source-shape
  score is presented as exact recovered gameplay.
- Some Layout words are accessed by confirmed raw offsets because typed shared
  fields remain unnamed. Unknown StarMap slots keep field-offset names.
- Existing ownership assumptions around station loading/deletion and repeated
  geometry attachment are retained from the Android body and need a native
  executable test. No renderer, camera, touch or mission runtime was exercised.
  A successful static-library build does not prove native ABI/runtime behavior.

## Validation

Final focused ARM build compiles **204/204 translation units, zero failures**;
22 symbols compare, zero skipped, six unique linked-exact bodies and three
unique byte-exact bodies. Clean baseline has two of each.

Native UCRT Debug `gof2` static-library build passed. Existing compiler warnings
remain; no native executable/runtime test is claimed.

The independent gate freezes the symbol set, original weights and alias
relationships and compares full-corpus exact pairs by **(translation unit,
symbol)**. `--allow-partial` relaxes only the 99% threshold, never corpus
integrity or exact-pair preservation. The default 99% gate must remain red
for this checkpoint.

Full corpus: **4619 comparisons, zero skips** in both clean baseline and final.
Overall source-shape 88.99% в†’ 89.05%; ordered fuzzy
75.97% в†’ 76.07% (including all corpus entries).
Coverage remains 4274/4524 original functions, 250 missing (7 wrong-type / 243
absent). Final extra symbols: 129, baseline: 130. Existing coverage gaps were
not removed from reporting.

Exact pair preservation gate: **PASS**, zero lost (unit, symbol) pairs.
Linked-exact **2079 РІвЂ вЂ™ 2083**; byte-exact **959 РІвЂ вЂ™ 960**. Gains are confined to
StarMap: initLights, missionChanged, askForJumpIntoAlienWorld and setStart;
missionChanged also becomes byte-exact.

Default >=99.0% gate: **FAIL**, exit 1, `StarMap >=99.0% target NOT reached`.
Explicit partial/preservation gate: **PASS**, exit 0. Negative checks confirmed
that changed original weights, omitted aliases, skipped symbols and a full-corpus
byte-exact pair loss (even with --allow-partial) are rejected.
`git diff --check` passed.

Committed evidence: `docs/validation/starmap_2026-10-02_baseline.json`,
`starmap_2026-10-02_final.json` (focused inputs) and
`starmap_2026-10-02_gate.json` (full exact-pair difference and both means).
Full raw reports and logs remain in this worktree's _work directory.

baseline full JSON SHA-256: `ceb8ddf50e03dbadd2278413942c6e1a8e59b1f6d4060d7628de66593632a6d0`.

final full JSON SHA-256: `ced2430b6b62170949d749b9b517a62b8ceda468819adb0390e88c0da2f807d4`.


### Reproduction

Use the unchanged local NDK r18b ARM verifier with GOF2_VERIFY_LOCAL_NDK=1 and
GOF2_NDK_ROOT pointing at the installed android-ndk-r18b; retain its existing
compile flags: armv7-none-linux-androideabi21, ARMv7-A Thumb, NEON, softfp,
PIC, RTTI, stack-protector-strong, Oz and libc++. Commands run from this isolated checkout:

```text
python tools/verify/verify.py --unit game/world/StarMap --only ^_ZN7StarMap --report _work/final-focused.json
python tools/verify/verify.py --no-build --report _work/final-corpus.json
cmake --build cmake-build-ucrt --target gof2 -- -k 0
python tests/check_starmap_source_shape.py _work/baseline-focused.json _work/final-focused.json _work/baseline-corpus.json _work/final-corpus.json --allow-partial
python tests/check_starmap_source_shape.py _work/baseline-focused.json _work/final-focused.json _work/baseline-corpus.json _work/final-corpus.json
git diff --check
```

Clean baseline full comparison used separately copied clean objects at
`_work/baseline-verify/base`, with --build-dir _work/baseline-verify --no-build.
No baseline/final object directory was concurrently mutated during comparison.

## Fixed unique-body measurements

Percentages are the unchanged verifier output. C2/D2 aliases are excluded.

| Android symbol | Original/current instructions | Baseline source | Final source | Final strict | Final exact |
|---|---:|---:|---:|---:|---|
| `_ZN7StarMap16drawOnScreenInfoEib` | 1207/1135 | 78.9% | 90.9% | 22.9% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap10OnTouchEndEii` | 1037/895 | 66.6% | 86.3% | 33.0% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap6updateEi` | 971/958 | 76.6% | 93.9% | 54.1% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap4drawEv` | 916/870 | 49.7% | 89.9% | 39.6% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap4initEbP7Missionbi` | 750/673 | 72.9% | 90.0% | 57.9% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap14initStarSystemEv` | 710/639 | 80.9% | 91.2% | 48.6% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMapC1EbP7Missionbi` | 333/330 | 85.1% | 93.4% | 60.3% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap12OnTouchBeginEii` | 275/273 | 91.6% | 94.8% | 63.5% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap11OnTouchMoveEii` | 265/255 | 81.3% | 90.7% | 60.4% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap7drawKeyEv` | 246/243 | 59.5% | 97.8% | 34.8% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap6departEb` | 205/204 | 94.9% | 95.1% | 60.1% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMapD1Ev` | 69/70 | 92.0% | 99.4% | 95.0% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap6renderEv` | 47/47 | 95.7% | 99.6% | 97.9% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap10initLightsEv` | 21/21 | 87.8% | 100.0% | 100.0% | linked |
| `_ZN7StarMap24askForJumpIntoAlienWorldEv` | 21/21 | 97.1% | 100.0% | 100.0% | linked |
| `_ZN7StarMap8setStartEii` | 17/17 | 88.9% | 100.0% | 100.0% | linked |
| `_ZN7StarMap14isInPlanetModeEv` | 5/5 | 100.0% | 100.0% | 100.0% | byte |
| `_ZN7StarMap14setJumpMapModeEbb` | 3/3 | 86.7% | 100.0% | 66.7% | Р Р†Р вЂљРІР‚Сњ |
| `_ZN7StarMap14missionChangedEv` | 2/2 | 90.0% | 100.0% | 100.0% | byte |
| `_ZN7StarMap8renderBGEv` | 1/1 | 100.0% | 100.0% | 100.0% | byte |

Aliases: `_ZN7StarMapC2EbP7Missionbi` = C1; `_ZN7StarMapD2Ev` = D1.
