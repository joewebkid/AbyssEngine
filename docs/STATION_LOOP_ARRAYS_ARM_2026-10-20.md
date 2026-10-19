# Station texture loop, record cleanup and target arrays

Base: `95a89df553481a689bcfa27c3b064d74f6e1181b` (remote main verified
before the change).

The continuous original-asset Level/GLES fixture now passes the previous
`TextureCreate(10021)` boundary, finishes all five Pan station textures,
releases the five parsed Station records, and allocates the target, planet
and position arrays. It stops **before `AERandom::setSeed(2100)`**, before
planet placement. This does not complete StarSystem construction or Level
stage 0, and does not validate a complete native game frame or lifetime.

## Production changes and primary evidence

Primary source: Android 2.0.16 ARM ELF, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
No iOS/PC evidence substitutes for this ARM path.

Inspected `StarSystem.cpp/.h`, `Station.cpp/.h`, `Array.h`,
`LowResourceTable.cpp`, `ResourceTexture.cpp`, the existing station/sun/atlas
fixture chain, and the original ARM constructor/resource/cleanup bodies.
`FileRead.cpp`, Level player creation, Gun/PlayerEgo, central project status
and validation documents were not changed.

- `StarSystem.cpp`: retain the reader's actual `Array<Station *> *` type,
  call the existing `ArrayReleaseClasses`, then delete its Array. ARM
  `0x134038`, `0x13403e`, `0x134042` calls precisely those three operations.
  The release helper at `0xa8e28` iterates capacity, calls Station D1 and
  delete (`0xa8e42/0xa8e46`), clears each pointer, deletes the backing storage
  (`0xa8e68`) and nulls it (`0xa8e6e`). The following Array destructor sees
  that null backing pointer. Removed only the three obsolete declarations
  used by this constructor section.
- Allocate `new Array<KIPlayer *>()` and resize that typed array. Original
  `0x134046` requests 12 bytes for ARM, then `0x13404e/0x13405c` calls the
  pointer-array constructor and resize; field publication is at `+0x18`.
  A literal 12-byte object and `Array<unsigned int>` storage were wrong on
  a 64-bit host. The public field remains unchanged. The original following
  geometry/vector arrays have count + 1 (`0x134078/0x134092`).
- `LowResourceTable.cpp`: change only resource **10056 / 0x2748** from
  `-1.0f` to `0.0f`. At `0x11a328` ARM places zero in r2 before
  `ResourceTexture(const char *, float)` at `0x11a32a`; the next resource ID
  write at `0x11a334` is 10056. The PC-relative literal at `0x11a330` resolves
  to `planet_014_big.aei`. The constructor preserves this argument, so this
  is a real resource mismatch, not a fixture adjustment. Other low-table
  entries were not audited or changed.

`tools/verify/test_station_loop_arm.py` checks these original instructions,
call targets, literal path and the corrected resource value against the
pinned ELF. The fixed four-function matching family was declared before
edits in `station_loop_family.json`; the directly reached low-table
dependency is reported separately, not added to improve the family mean.

## Continuous runtime evidence

The runner `run_station_loop_asset_gate.py` extends the earlier gate. It
extracts the production Station destructor and continues the production
StarSystem constructor after each real upload. Observation checks precede
the actual destructor body; all five records have null stock/agent arrays.
Calls in those unexecuted destructor branches are explicit aborting traps,
not successful replacements. Actual String member destruction remains in
the compiled destructor.

The bounded resource bootstrap adds exactly the production block for
10056, inserted inside the extracted registration function. It does not
execute or claim validation of the whole low-resource table. Existing
Pan/station 7/mission 50 setup and the inherited jumpgate ID override remain
fixture assumptions. Android still selects one implementation of duplicate
math operators in generated link inputs; production math is unchanged.

| Station | Texture ID | Original AEI | Base dimensions / bytes |
| --- | --- | --- | --- |
| 5 | 10014 | `planet_003_small.aei` | 64x64 / 16,384 |
| 6 | 10021 | `planet_010_small.aei` | 64x64 / 16,384 |
| 7 | 10056 | `planet_014_big.aei` | 1024x1024 / 4,194,304 |
| 8 | 10024 | `planet_013_small.aei` | 64x64 / 16,384 |
| 9 | 10028 | `planet_017_small.aei` | 64x64 / 16,384 |

All five are type-3 raw RGBA AEIs, one region, a 23-byte prefix before pixels
and two trailing bytes left unread by the production parser. Their 4,259,840
base pixel bytes are compared exactly to actual GLES framebuffer readback.
Managed indices 2..6 and station indices 5..9 must be published; original
ring queries return false for these stations. No ring branch is claimed.
All generated mip levels are sampled (six per small texture, ten for the
large one) against an independent CPU box average with the existing
`ceil(level/2)` byte-rounding bound. This is a tested-backend oracle, **not**
recovered original mip pixels or a GLES-mandated filter. [ES 2.0 section
3.8.8](https://registry.khronos.org/OpenGL/specs/es/2.0/es_cm_spec_2.0.pdf)
specifies no particular mip reduction filter.

The inherited atlas and nine original ETC1 sun mips are still checked.
The final scene prefix contains 17 meshes, 15 transforms and 44 collision
volumes, with 25/25 file opens/closes. Five production Station destructors
execute before the target/planet/position arrays reach sizes 5/6/6. Empty
target pointers and zero vectors are checked. First/last target pointer
writes are checked and restored, without dereferencing observation pointers.

The separate `run_starsystem_target_array_smoke.py` extracts the actual
target-allocation statements and checks counts 0, 1 and 5, object allocation,
element width, resize storage, zero initialization and pointer round trips
at Windows O0/O2. Host object/element sizes are 24/8 bytes; ARM sizes are
12/4 bytes. It destroys only Array storage, not a StarSystem instance.

Original asset SHA-256 (all under `data/assets/main/3d/textures/low/etc/planets/`):

| File | SHA-256 |
| --- | --- |
| `planet_003_small.aei` | `150b79909b92a15e30dc1ec761b320666258a030fe0dd7cf090bb8894502213e` |
| `planet_010_small.aei` | `8d5c8f8335cc483397c3f9a272118bf0af68b712ca137bc49f3e2b403c18f6b1` |
| `planet_014_big.aei` | `3c2e0703a640a0d481f97e12eb240dc3cbe2b1a9de4942fbe0a8d9c9c77a2cdb` |
| `planet_013_small.aei` | `4ddefbad6238d1dd58659e2be124516f34d76c240e76d7f9bcbf1ee85bf2116c` |
| `planet_017_small.aei` | `b6b29a43304f34d02815194d2e2f5453875e2e0c183debd166b1ebe287fbe97f` |

## Validation and fixed family

- Windows SwiftShader EGL 1.4 / ES 2: continuous gate PASS at O0 and O2.
- Real Android ARMv7, M2007J20CG/surya, API 31, Adreno 618, EGL 1.5:
  continuous gate PASS at O0 and O2. Device-reported driver is ES 3.2
  V@0502.0; the fixture uses the existing ES2 context request and GLES path.
  The same five textures, record cleanup and 5/6/6 arrays pass with 12-byte
  Array objects and 4-byte target pointers. No oracle tolerance changed.
- Native `gof2` static-library build: PASS (not a full executable link).
- Verifier suite: 77 tests, one pre-existing skip, no failures. The skipped
  real-array-alias test requires a standalone `.o` command-line argument;
  all three new station-loop ARM evidence tests execute and pass.
- After ARM compile: 208/208. Compared with the pre-edit 208-object baseline,
  only `StarSystem.o` and `LowResourceTable.o` differ; 206 are byte-identical.

| Function | Before source / strict | After source / strict |
| --- | --- | --- |
| StarSystem C1 (C2 alias same) | 76.5 / 16.4 | 76.6 / 18.4 |
| Main PaintCanvas TextureCreate | 95.5 / 51.7 | 95.5 / 51.7 |
| ImageCreateFromFile | 97.3 / 58.8 | 97.3 / 58.8 |
| TextureCreateFromFileIntern | 92.8 / 65.8 | 92.8 / 65.8 |
| **Four-function mean** | **90.525 / 48.175** | **90.55 / 48.675** |
| Low table (separate dependency) | 90.1 / 14.3 | 90.1 / 14.3 |

**98% source-shape target not met.** No family padding, metric changes or
semantic shortcuts were used to raise it. Constructor-wide matching still
includes unrepaired code beyond the bounded runtime prefix.

The fresh full after run has **4,735 pairs**, source-shape **93.92%** and
strict fuzzy **83.20%** (before: 93.91% / 83.19%). It has **zero skipped
units and zero wrong-type signatures**. The unchanged exact-pair reporter
confirms no lost pairs, no lost exact matches and no extra-symbol changes.
Linked-exact increases **2,548 -> 2,551**; byte-exact stays **1,079**.
Four new StarSystem pairs are measured; three are linked-exact:
`ArraySetLength<KIPlayer *>`, `ArrayReleaseClasses<Station *>`, and
`Array<Station *>::D2`. The fourth is `Array<KIPlayer *>::C2`.
Unique coverage stays 4,323/4,524, with 201 absent and 124 extra symbols.

The fresh full baseline has 4,731 pairs, 4,323/4,524 original functions,
201 absent, 124 extra, 2,548 linked-exact and 1,079 byte-exact pairs, with
zero skipped units and zero wrong-type signatures. Its SHA-256
`f5a8968437ed5c84c4b5ae65065cf68b494cc737c88365f12d0fac9607e966db`
is identical to the published main baseline.

Local runtime evidence (not committed):

| File under the local output directory | SHA-256 |
| --- | --- |
| `host.log` | `0eacd13a99f05b2f912c2e193124eca272b34d9c53349cc65e7f4244b3e7ae36` |
| `android.log` | `cc9903afacad263911f3b3ab001b5604606c0010507caaf2590b638fd68aa466` |
| `object-delta.json` | `466161070c6f027e5d47e8680d78673699a33fa56b27b2134140beb4a115f0b9` |
| `tests.log` | `c37f9aebd6169a3cc3a399fa41f8f777815d713fa13bef186c88b9a6b14fbc5b` |
| `after-full.json` | `9b528144b5de616043a6de2c73330da3f6a03442016acb4a8542296c1419457c` |
| `exact-delta.json` | `5c5239c5cb8439fec1de3123d00a9105e5feedb700f08e391f89cac8c547ff47` |

## Exact boundary, limitations and reproduction

Next work: validate seed arithmetic and original planet placement/geometry
creation beginning at ARM `0x1340a4..0x1340b8`. The actual fixture seed is
7 * 300 = 2100. The gate stops before this call; ARM's 32-bit multiply then
sign extension is not a general proof of the current wider multiplication.
Later float bit-pattern conversions and placement are not reached.

Full `StarSystem::~StarSystem` still references unresolved
`ArrayRelease_KIPlayer` / `Array_KIPlayer_dtor`; this task does not validate
full StarSystem lifetime or claim those helpers are repaired. Station
records with non-null ships/items/agents are also outside this gate.

Run `tools/run_station_loop_asset_gate.py --asset-root <original-assets>
--out-dir <local-output>` with either `--angle-dir <gles_swiftshader>
--egl-include <NDK-sysroot-include>` or `--ndk <NDK25> --adb <adb>
--serial <device>`; both execute O0/O2. Use `--reuse-inputs` only after fresh
parent fixture generation for the source revision. Original assets remain
in place; Android receives requested file bytes through the existing
loopback service, with no asset filesystem copies on the device.

Run `tools/run_starsystem_target_array_smoke.py --out-dir <local-output>`
and `python -m unittest discover -s tools/verify` for component/evidence
checks. Full ARM runs use the unchanged verifier with `--fail-on-wrong-type`;
compare reports with `report_station_binary_delta.py --family
tools/verify/station_loop_family.json`. Existing ARM/native output folders
are reused; all new logs/reports/component and Android outputs are local
under `C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/astra`.
