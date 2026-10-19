# StarSystem first sun: original ETC1 upload gate

Base: published main `04ee4257ae63d7e325084181c237061349d942c3`.
Branch: `codex/astra-starsystem-sun-20261019`. This is native work; no Godot
result is used as evidence. The previous station branch remains intact.

## Change and evidence

The only production change is the three texture-ID tables in
`src/game/world/StarSystem.cpp`: `uint16_t` becomes `uint32_t`, preserving all
values. Original rodata at `0x202260`, `0x202334`, `0x2023a0` contains four-byte
entries (sizes `0x4c`, `0x6c`, `0x6c`). At `0x133ec6`, ARM loads the low
halfword using an index shifted by two. This repairs table storage/stride;
it does not claim the whole constructor is recovered.

Primary reference is Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Evidence tests pin that binary and check instructions, calls and table values:

| Address | Evidence |
| --- | --- |
| `0x133b68` | StarSystem constructor entry |
| `0x133ec6`, `0x133ece` | Four-byte table stride and sun TextureCreate call |
| `0x133ed6`, `0x133eda` | System/index lookup after sun returns |
| `0x79a72` | Resource payload float scale at offset 4 |
| `0x79aa6` | PaintCanvas calls the internal uploader |
| `0x79aaa–0x79ab2` | Result-1 test, handle and output publication |
| `0x6fd84–0x6fdf0` | ETC1 upload loop; `glCompressedTexImage2D` at `0x6fdc2` |

The new runner extends the existing station gate. It extracts the production
Station getter and retains the intact production constructor, AEI parser and
uploader bodies. Only generated fixture interception points change. The
existing runner's build, asset transport and complete station-catalog checks
are reused. No original assets or binaries are copied into the repository.

## Sun and runtime boundary

Resource `10033` selects
`data/assets/main/3d/textures/low/etc/suns/sun_002.aei`:
SHA-256 `c500c604d94b8861c9b37b325693419e4b69c0d3c45a2dae872aff1f98e888f3`.
Its 43,733 bytes contain AEI type `0x42`, one region, a 256×256 ETC1 image,
and nine mip levels. Compressed sizes are 32768, 8192, 2048, 512, 128, 32,
8, 8, 8 bytes (43,704 total). The original parser closes at byte 43,731;
two trailing bytes are not consumed. The gate preserves that behavior.

The real GLES call receives ETC1 token `0x8d64`. A separate fixture shader
samples every uploaded mip into an RGBA framebuffer. All 349,524 output bytes
are compared against an independent CPU decoder based on the
[Khronos ETC1 specification](https://registry.khronos.org/OpenGL/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt).
The decoder supplies no upload pixels. Sampling temporarily uses nearest
filtering and restores the original linear filters. This is an oracle pass,
not a rendered native game frame.

The gate requires successful managed index 1 publication into both the sun
resource and StarSystem texture slot 0, and a valid GL texture record. The
original uploader can return 1 with an invalid managed GL record on failure;
the gate explicitly rejects that case instead of treating the return value
alone as success.

The confirmed next stop is **before station TextureCreate(10014)**, slot 1.
This first non-current station is index 5, texture index 3. No station texture
is uploaded at that stop. StarSystem construction and Level init stage 0
remain incomplete.

## Validation

- Windows x64, O0/O2: PASS on Google SwiftShader 4.0.0.1, EGL 1.4 / GLES 2.0
  (software renderer). Installed ANGLE D3D11 does not advertise ETC1; its
  negative run correctly failed the valid-record check. No ETC2 substitution
  or CPU upload fallback was introduced to hide that limitation.
- Android ARMv7, O0/O2: PASS on M2007J20CG/surya, API 31, Adreno 618,
  EGL 1.5 / GLES 3.2 driver V@0502.0. Native 32-bit layout assertions pass.
  Original files were read through the existing read-only transport; only
  test executables were pushed. ADB reverse entries were removed afterward.
- Every successful run: 20 opens / 20 closes, 17 meshes, 15 transforms,
  44 collision volumes; the inherited 4,194,304-byte atlas readback plus all
  349,524 sun RGBA bytes match. Both managed textures exist at the boundary.
  All 34 station catalogs / 135 records and edge-case reader checks pass.
- Native `gof2` static-library build: PASS. This is not a game executable test.
- Verifier suite: 66 tests, one pre-existing skip, no failures. Three new
  tests pin the original ARM evidence described above.
- Before and after: 208/208 ARM translation units compile. Object hashes
  differ only for `game/world/StarSystem.o`; the other 207 are identical.

Local logs (not committed): `_work/sun-host-final.log`, `_work/sun-android.log`,
`_work/sun-tests.log`, `_work/sun-native.log`. Runtime log SHA-256:

| Log | SHA-256 |
| --- | --- |
| Windows | `8e9dc23d953d9c13ff459157017e2f3892a0d5c18ffeae2863ebeedb21aece6a` |
| Android | `ad28a5a702ecc2c5da46081aced9c8082a3969d605a7d8b2d1bb145d023c1d64` |

## Matching and remaining risks

The family was declared before edits in `tools/verify/sun_texture_family.json`:
StarSystem C1, primary PaintCanvas TextureCreate, ImageCreateFromFile and
TextureCreateFromFileIntern. C2 is reported as an alias, not a fifth logical
function. The family is disjoint from the parallel String/ResourceTexture work.
No easy getters were added to improve the score.

Fresh focused comparisons before/after (source-shape / strict fuzzy):

| Canonical function | Before | After |
| --- | --- | --- |
| StarSystem C1 | 76.6 / 16.2 | 76.6 / 16.2 |
| PaintCanvas TextureCreate | 95.5 / 51.7 | 95.5 / 51.7 |
| ImageCreateFromFile | 97.3 / 58.8 | 97.3 / 58.8 |
| TextureCreateFromFileIntern | 92.8 / 65.8 | 92.8 / 65.8 |
| **Family mean** | **90.55 / 48.125** | **90.55 / 48.125** |

**The 98% target is not met.** These are the achieved scores, not a proven
maximum. StarSystem alone still has 681 compiled instructions versus 995 in
the original under this verifier, and low ordered control-flow agreement.
The table repair changes instruction inventory slightly without changing
the rounded composite score. No matching normalization, signature, or
gameplay semantics were changed to raise the metric. Runtime progress and
matching progress are separate results.

Fresh full before/after reports both contain 4,731 `(unit, symbol)` pairs,
covering 4,323/4,524 original functions (201 missing), with 93.91% mean
source-shape and 83.19% strict fuzzy. There are 2,547 linked-exact and 1,079
byte-exact pairs, 124 extra symbols, zero skipped units and zero wrong-type
signatures. Normalized set comparison confirms **zero lost and zero gained
exact pairs**, zero pair-set changes and zero extra-symbol changes.

| Local report | SHA-256 |
| --- | --- |
| `_work/sun-before-full.json` | `9a506a8757b3c2e96b7339ba746ee98a12f583ba34fc6f6baf91786b2fc3db38` |
| `_work/sun-after-full.json` | `997529dbbbd663ab9047dfc6ad0694c695a8c6a30d7ac0d3d8f0a3fee3b11873` |
| `_work/sun-exact-delta.json` | `7e2c5e1a9c5d8fe04a0e2690272e2998ec714445af045d8216ad58553204b9fd` |

Bootstrap, material and skybox behavior still includes inherited fixtures.
The Pan fixture uses mission 50/station 7 and overrides its gate station from
5 to 7 to exercise both jumpgates. This is not a full native executable or
an unmodified game startup. The Android fixture removes duplicate math
operator definitions only in generated link inputs, retaining the existing
Vector/Matrix definitions; production math is unchanged.

The newly exercised scale value is a float zero from the texture resource;
managed texture indices use 32-bit elements. Later constructor code remains
outside this boundary: `moveForward(0x447a0000)` converts an integer numerically
instead of using the apparent float bit pattern, and the target-array path
uses a literal 12-byte allocation plus unsigned-int array sizing. Those need
separate instruction/ABI validation before proceeding, especially on 64-bit
hosts. They are not fixed or validated by this sun gate.

## Reproduction

Run `tools/run_starsystem_sun_gate.py --out-dir _work/gles-host --asset-root
<original-assets>` with `--angle-dir <gles_swiftshader> --egl-include
<NDK-sysroot-include>` on Windows, or `--ndk <NDK25> --adb <adb> --serial
<device>` for ARMv7 Android. Each invocation runs O0 and O2. Add `--reuse-inputs`
only after a fresh parent-fixture preparation in that directory.

Build native `gof2` in `_work/native`. For ARM comparison, use the local NDK18
environment documented in `VALIDATION.md`, separate before/after build dirs,
and `tools/verify/verify.py --fail-on-wrong-type --report <report>`.
Compare complete exact-pair sets with
`tools/verify/report_station_binary_delta.py <before> <after> --family
tools/verify/sun_texture_family.json --output <delta>`.
