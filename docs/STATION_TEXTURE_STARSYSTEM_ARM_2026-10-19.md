# First station-orbit texture: original RGBA upload

Base: published main `f7c4798d8b3fd4e3e5786d3354a4423440d15b57`.
Branch: `codex/astra-station-texture-20261019`; prior sun branch preserved.
No Godot result is used as native evidence.

## Production change and primary evidence

Inspected `StarSystem.cpp/.h`, `Status.cpp`, `PaintCanvas.cpp`,
`BuildResourceList.cpp`, `AbyssEngine.cpp`, the existing station/sun fixture,
and original ARM instructions. Only `StarSystem.cpp` changes in production:
the current-station branch calls `getCurrentCampaignMission()` before the
mode test, and the special mission-0/mode-3 branch skips `getTextureIndex()`.
Ordinary current/remote branches fetch the texture index where the original
does. This restores the call sequence shown by ARM.

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

| ARM address | Evidence |
| --- | --- |
| `0x133f1c–0x133f28` | Mission call precedes mode/mission comparisons; special branch goes to `0x133ff8` |
| `0x133f34`, `0x133f8e` | Separate current/remote texture-index calls |
| `0x133f9a` | Four-byte station texture table stride, low-halfword load |
| `0x133fa4` | Remote station texture upload |
| `0x133fb2`, `0x133fba` | Station index then ring query, after texture return |
| `0xabcd0–0xabcf0` | Original ring query: rotated range check, then bitmask; index 5 returns zero |
| `0x6fc02`, `0x6fc06` | GL_RGBA and actual `glTexImage2D` |
| `0x6fc0c–0x6fc18` | Image mip flag and actual `glGenerateMipmap` |

The loop smoke extracts the production loop and checks eight combinations
of current/remote station, mode 0/3, and mission 0/50 at O0/O2 on the 64-bit
host. Calls, uploaded resource ID, selected slot and array writes are checked.
Ring-present geometry is outside that smoke. Two added original-ELF tests
pin the branch/call and RGBA/mipmap evidence.

## Asset and honest pixel oracle

Resource **10014** is
`data/assets/main/3d/textures/low/etc/planets/planet_003_small.aei`, not a
station exterior diffuse map. SHA-256:
`150b79909b92a15e30dc1ec761b320666258a030fe0dd7cf090bb8894502213e`.
Despite the `etc` directory, AEI type 3 is raw 64×64 RGBA with a mip-generation
flag. The file has 16,409 bytes, a 15-byte header, one 8-byte region, and
16,384 pixel bytes. Original parsing stops at 16,407; two trailing bytes are
not consumed. No compressed-format substitution is involved.

The bounded continuous gate delegates to the intact production AEI parser
and uploader. It checks the real GLES texture, base-byte count, managed index
2, resource handle and StarSystem slot 1 publication. Three managed textures
exist: atlas, sun and this orbit texture. The texture-byte counter is
4,194,304 + 43,704 + 16,384; it follows original base-data accounting, not a
claim about total GPU memory used by generated levels.

Base level: actual framebuffer readback and shader sampling must match all
16,384 original RGBA bytes exactly. Generated levels 1–6 have no original
file payload. They are separately sampled and compared with an independent
box average over the corresponding base pixels. The predeclared per-channel
bound is `ceil(level/2)` byte values, allowing accumulated nearest-byte
rounding of a 2×2 pyramid. This is a box-filter/quantization check on the
tested backends, **not a GLES requirement for one exact filtering kernel**
and not recovery of original generated pixels ([ES 2.0, manual mipmap generation](https://registry.khronos.org/OpenGL/specs/es/2.0/es_cm_spec_2.0.pdf), section 3.8.8).
Measured errors are reported
in the runtime logs. The CPU oracle never supplies upload pixels.

The existing sun decoder is unchanged; its framebuffer sampling setup is
shared with this check. All nine original ETC1 sun mips remain byte-exact in
the extended gate. Texture filters are restored after sampling. The sampling
passes are fixture rendering, not a native game frame.

## Confirmed boundary and validation

The next boundary is **before `TextureCreate(10021)`**, the second station's
request (station 6, texture index 10). The first upload returns and the intact
production `Status::orbitHasPlanetRing(5)` returns zero. The first loop
iteration completes: texture slots 0/1 contain managed indices 1/2 and
station-index slot 0 contains 5. Remaining slots are zero; resource 10021 is
still unloaded. No second station upload, ring geometry, target array or
placement code is claimed as executed. The unchanged ring method is not
added to the matching family to inflate its score.

- Windows x64 O0/O2: PASS on SwiftShader 4.0.0.1, EGL 1.4 / GLES 2.0
  (software). The installed ANGLE backend still lacks the inherited sun's
  ETC1 support; no token or CPU-upload substitution was made.
- Android ARMv7 O0/O2: PASS on M2007J20CG/surya, API 31, Adreno 618,
  EGL 1.5 / GLES 3.2 V@0502.0. Native 32-bit layout assertions pass.
- All four runs: 21 opens / 21 closes, 17 meshes, 15 transforms, 44 collision
  volumes; exact atlas, sun and orbit base pixels. Generated orbit mip
  maximum errors for levels 1–6 are `[1,1,1,1,1,0]` on both backends, within
  bounds `[1,1,2,2,3,3]`. All 34 catalog / 135 record checks also pass.
- Assets were served read-only in memory to Android; only fixture executables
  were pushed. ADB reverse entries were removed after completion.
- Native `gof2` static-library build: PASS. Verifier suite: 71 tests, one
  pre-existing skip, no failures. The isolated eight-case loop smoke passes
  Windows O0/O2. These are separate compile/runtime/evidence checks.
- Before/after ARM compilation: 208/208 units pass. Object hashes differ only
  for `game/world/StarSystem.o`; 207 other objects are identical.

Local logs are not committed. Runtime log SHA-256:

| Log | SHA-256 |
| --- | --- |
| `_work/stationtex-host-final.log` | `64470ab45b03e8c8eeb32c3d35c867e46648521e45108029c8b7fc9f2a086ab5` |
| `_work/stationtex-android-final.log` | `5240b99cfd9226e08062cdb6538417b5ea2f8b62fd5dc87748d0865b2d370070` |

## Fixed matching family

The four canonical functions below were declared in chat before edits and
recorded in `tools/verify/station_texture_family.json`, with C2 listed
separately as a constructor alias.
The family excludes String/FileRead and the parallel Sol work.

| Function | Before source / strict | After source / strict |
| --- | --- | --- |
| StarSystem C1 | 76.6 / 16.2 | 76.5 / 16.4 |
| Main PaintCanvas TextureCreate | 95.5 / 51.7 | 95.5 / 51.7 |
| ImageCreateFromFile | 97.3 / 58.8 | 97.3 / 58.8 |
| TextureCreateFromFileIntern | 92.8 / 65.8 | 92.8 / 65.8 |
| **Mean** | **90.55 / 48.125** | **90.525 / 48.175** |

**The 98% target is not met.** Source-shape decreases by 0.025 percentage
points while strict fuzzy increases by 0.05. The ARM-confirmed call-order
repair is retained; semantics and family membership were not adjusted to
raise the metric. These are measured results, not a proven matching ceiling.

Complete corpus before/after: 4,731 pairs, 4,323/4,524 original functions,
201 absent, 124 extra symbols; 93.91% source-shape, 83.19% strict fuzzy.
All **2,548 linked-exact and 1,079 byte-exact pairs are preserved**, with zero
gained/lost exact pairs, pair-set changes or extra-symbol changes. The final
after run has zero skipped units and zero wrong-type signatures.

The first before run skipped BumpShaderCloak/Refract during comparison
(208 units had compiled); its summary does not expose the underlying tool
error. Both unchanged units were rerun successfully: 12 comparisons total.
The completed baseline combines those measured rows with the other 206
units. All 208 base objects were disassembled again for symbol coverage, and
the unchanged full-corpus reporter recomputed aggregates/coverage. No score
or matching algorithm changed. The resulting JSON is **byte-identical to
the published main report**. The partial report and retry provenance remain
available; the initial skip is not presented as a clean first run.

| Local report | SHA-256 |
| --- | --- |
| `_work/stationtex-before-complete.json` | `ec8ea688f4a0dbd3603d001baf415a54aabeac0bdd744e9a78a5ff1a0f4b8b33` |
| `_work/stationtex-after-full.json` | `cfe0cdea7c5e280e56c19a0b2f1ac0fec6ad289dba93304519f0c448713f4d80` |
| `_work/stationtex-exact-delta.json` | `b2f47a541c89b50b8833f8dab85a0474771ef6b425973a35381b6e1c44c25f5c` |
| `_work/stationtex-baseline-completion.json` | `f85483f5a61cef0ce3dbfae3f069316c60401f10fdc4ff8750e5acc12c172a38` |

## Limits and reproduction

Level stage 0 and StarSystem construction remain incomplete. Bootstrap,
material and skybox fixtures are inherited from the sun gate, including
Pan mission 50/station 7 and its explicit jumpgate-ID override. The Android
fixture still selects one implementation of duplicate math operators in
generated link inputs; production math is unchanged. Later float bit-pattern
and literal 12-byte target-array risks are not reached and are not repaired
or validated here. No full game executable/frame is claimed.

Run `tools/run_station_texture_gate.py --out-dir _work/gles-host --asset-root
<original-assets>` with `--angle-dir <gles_swiftshader> --egl-include
<NDK-sysroot-include>`, or `--ndk <NDK25> --adb <adb> --serial <device>`.
Both run O0/O2. Use `--reuse-inputs` only after a fresh preparation for this
source revision. The first Windows run here regenerated the parent fixtures.
Run `tools/run_starsystem_texture_loop_smoke.py` for the isolated call checks.
Run the verifier suite with `python -m unittest discover -s tools/verify`.

Full ARM runs reuse `_work/arm-final` and `_work/arm-verify` with the local
NDK18 configuration; use `--fail-on-wrong-type`. Compare exact pairs via
`report_station_binary_delta.py` with `--family
tools/verify/station_texture_family.json`. Native build reuses `_work/native`.
Original binaries/assets stay in place. No new worktree, resource copies or
bulk output directories were created; no unrelated data was removed.
