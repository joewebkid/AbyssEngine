# Original atlas upload across real EGL/GLES

Base: `22193e4062bad5a965668b2a60c25a2c3d9438f8`.
Isolated branch: `codex/astra-gles-atlas-20261019`.

The bounded continuous Level path now crosses the former first-`glGetError`
boundary, uploads the original interface atlas, publishes its managed texture
record, creates all three flare Image2Ds, and completes LensFlare. Its next
explicit stop is **SolarSystem::getStations**, inside StarSystem construction.
Level stage 0 has not returned; this is not a playable native game.

## Evidence and production changes

Primary artifact: Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Read its original instructions, symbols and Android IDA upload body. No iOS,
PC, guessed GPU response or cached CPU branch was substituted.

Inspected the repository instructions/status/validation and preceding
LensFlare and Level gates; AbyssEngine, Engine, ApplicationManager,
PaintCanvas, AELoadedTexture, Image/Image2D, Array, String, GLES1Compat,
ResourceTexture/ResourceMesh, StarSystem and the inherited file/test services.
Read BuildResourceList and ResourceMaterial only for the existing test
extraction; neither was edited. PROJECT_STATUS.md, VALIDATION.md, loadPortraits,
the root repository and other worktrees were not changed.

Only production file changed: `src/engine/core/AbyssEngine.cpp`.

| ARM anchor | Confirmed correction |
| --- | --- |
| `0x6ff8a–0x6ffa4` | Engine +0x30 is **ApplicationManager**, whose +0 points to PaintCanvas. Append to canvas +0x10, then reacquire manager/canvas and return count − 1. The old direct cast of Engine::paintCanvas to PaintCanvas addressed the manager as a canvas and could corrupt memory/crash. |
| `0x6fec0`, `0x6fec8`, `0x6fece` | Destroy the temporary error-path String **before** deleting the GL texture and releasing Image. The old scope deferred String destruction until return. |
| Image reads throughout `0x6f7f4–0x6ffda` | Keep the parser-owned Image pointer live across external calls; reload format at dispatch, DXT mip format in each iteration, cubemap data for each face, and format/dataLen during managed publication. Remove the longer-lived Image/format/pixel aliases that did not follow the original loads. |

The wrapper still returns 1 regardless of its internal callee result, as in
the original. No return policy, filter setting, compression formula or
resource registration was invented to obtain a higher score.

ARM assertions in the executable lock Engine::appManager +0x30,
ApplicationManager::paintCanvas +0, PaintCanvas::cubeTextures +0x10,
Image size 0x14, and AELoadedTexture size 0x1c. Existing record assertions
also check its name/scale/isCube/valid/byteSize fields. The host's native
pointer sizes remain distinct from these ARM offsets.

## Real contexts and continuous execution

Windows: existing Android emulator **ANGLE** DLLs, EGL 1.5,
NVIDIA GeForce GTX 1050 Ti, Direct3D11 driver `32.0.15.6094`,
ANGLE `2.1.22762` (`533fa5cedc61`), reported OpenGL ES 3.0.0.

Android: attached **M2007J20CG / surya**, API 31, supports armeabi-v7a;
native EGL 1.5, Qualcomm **Adreno 618**, reported OpenGL ES 3.2
`V@0502.0`, driver date 2021-11-04. The test is a newly compiled ARMv7
executable using NDK 25.2.9519653, system libEGL/libGLESv2 and static libc++.
The separate comparison build still uses the matching NDK r18b toolchain.
Existing AVDs were listed but not launched; a real device was available.

Both environments request an ES2 pbuffer context. Every GLES2 call reaches
the real driver. glTexEnvi and AELabelObject reproduce the original empty
compatibility bodies (both begin with `bx lr`, at `0x6dad2` and `0x6daf0`);
they do not manufacture an upload result. Unavailable/failed context creation
fails the gate.

**Windows -O0/-O2 and Android ARMv7 -O0/-O2 all pass.** Each run performs
one continuous Level upload plus three separate fresh-atlas readbacks.

The same bounded production chain runs on Windows and Android:

1. Level ctor/init/createSpace, station 7, both jumpgates and StarSystem entry.
2. LensFlare calls Image2DCreate for IDs 1288–1290. Existing resource
   registration supplies texture 10062 and atlas regions 187–189.
3. The production canvas and upload functions parse
   `data/textures/gof2_interface.aei` without a cached texture handle.
4. Real GLES creates and uploads 1024×1024 RGBA. An FBO attaches the resulting
   texture, reports complete, and glReadPixels returns **all 4,194,304 bytes
   identical** to the independently located original AEI payload.
5. The manager's actual canvas receives one AELoadedTexture. Its resource
   handle and all Image2D texture IDs are managed index **0**, distinct from
   the nonzero GL name; the name, scale, byte count, valid/isCube bytes,
   Engine byte counter and ImageCount are checked.
6. Three uncached region parses complete, with original offsets
   `(71,934)`, `(160,902)`, `(285,601)` and sizes 64×64. Repeated Image2D
   requests reuse their indices without another upload.
7. SolarSystem::getStations throws the explicit next-boundary exception,
   proving LensFlare returned. No world-loading success is fabricated.

Continuous checks retain **2,724 resources, 17 meshes, 15 transforms,
44 collision volumes, 18 successful AEFile opens and 18 closes**. Four of
those opens read the interface atlas (one pixel load, three region loads).
File-existence checks are separate from that AEFile open counter.

The earlier skybox-upload/material services, Engine/ApplicationManager/
PaintCanvas bootstrap, supplied Status/world state and file interface remain
**explicit fixture boundaries**. The atlas boundary alone has been replaced
with real GLES. This is neither a complete platform host nor a test of the
original Android executable or its JNI/activity lifecycle.

Android reads original host files through a loopback-only, path-bounded,
read-only server and temporary ADB reverse mapping. Bytes go to process
memory; **no original asset/binary is copied to the device or checkout**.
Only the new test executable is pushed. Reverse mappings are removed after
each run. No installed game, activity, emulator or device setting is changed.

The inherited host fixtures had two x64-only offset assertions and a local
ifstream FileExist implementation; the new generator adapts these test
services to real 32-bit layouts and the same read-only transport. ARM linking
also exposes six pre-existing duplicate Vector/Matrix operator definitions
in AEMath.cpp and Vector.cpp/Matrix.cpp. The test explicitly selects the
existing Vector/Matrix definitions and source-extracts the remaining AEMath
code. Production math was not edited; no allow-multiple-definition linker
flag hides the issue. A general native executable still needs that link
ownership resolved separately.

## Upload/error/ownership checks

Separate fresh-upload cases use the same real context and production upload
body, with no cached GPU precondition. All three original atlases are read
back in full: 4,194,304 / 16,777,216 / 16,777,216 RGBA bytes match.
These cases are **not counted as further continuous Level progress**.

On the ordinary atlas, additionally check:

- Existing managed-record restore updates glId/byteSize in place, retains
  name/scale and does not append a second record.
- A callback sets width above GL_MAX_TEXTURE_SIZE; the real driver returns
  GL_INVALID_VALUE. The restore path deletes the failed name, sets record
  glId to −1 and byteSize to zero, without incrementing counters.
- A fresh managed failure still returns 1 and publishes the original
  sentinel record/index, with valid=1. That byte alone does **not** mean
  successful GPU residency.
- Unmanaged success returns a GL name without appending a managed record.
  Unmanaged failure returns −4, records the error/path and deletes the name.
- glIsTexture, glGetError, counters and file-close balance verify these
  branches. Test cleanup deletes surviving GL textures and records.

Full Level/canvas destruction, malformed/compressed inputs, cube faces,
context-loss lifecycle and actual sprite draws remain untested. The native
allocation-family conventions are preserved, not certified for arbitrary
replacement allocators. No rendered-frame or pixel-perfect game claim follows
from an FBO texture readback.

Original atlas SHA-256 (files are referenced in place):

```text
gof2_interface.aei             1544b7c6652be504f27edf64fcece6a53533f38bdbf5991a35c16e3a3c173fa6
gof2_interface_iphone4.aei     5313dc5ec16719efdceb1a87b0cf09742a136fc5265c1064b9f9a005bd0981d6
gof2_interface_ipad_1440.aei   6ceb260cfa93a3921cc01592c362377c4314203c7d0ca98a1c1242f851da7013
```

## ARM source-shape and call destinations

The selected narrow family is exactly ten `(unit,symbol)` pairs: seven in
AbyssEngine (upload, wrapper, pixel parser, region parser, image release,
managed ArrayAdd specialization, label function), plus both canvas
TextureCreate overloads and Image2DCreate. No constructor/destructor aliases
or unrelated tiny functions are used to increase this mean.

| Function | Before source / strict fuzzy | After source / strict fuzzy |
| --- | ---: | ---: |
| TextureCreateFromFileIntern | 92.2 / 50.3 | **92.8 / 65.8** |
| ImageCreateFromFile | 97.3 / 58.8 | 97.3 / 58.8 |
| ImageCreateRegionFromFile | 99.0 / 83.5 | 99.0 / 83.5 |
| PaintCanvas::TextureCreate callback overload | 95.5 / 51.7 | 95.5 / 51.7 |
| PaintCanvas::Image2DCreate | 96.6 / 81.9 | 96.6 / 81.9 |
| TextureCreateFromFile wrapper | 100 / 100 | 100 / 100 |
| PaintCanvas::TextureCreate output-reference wrapper | 100 / 100 | 100 / 100 |
| ImageRelease | 100 / 100 | 100 / 100 |
| ArrayAdd<AELoadedTexture*> in AbyssEngine | 100 / 100 | 100 / 100 |
| AELabelObject | 100 / 100 | 100 / 100 |

Unweighted mean: **98.06 → 98.12% source-shape**, **82.62 → 84.17%
strict fuzzy**. Original-instruction-weighted mean (1,389 instructions):
**94.931 → 95.239% source-shape**, **60.660 → 68.616% strict fuzzy**.
Five linked-exact pairs and one byte-exact pair; upload is neither.
The >=98% family target does not imply >=98% source-shape for each large
body, or an exact binary match.

Direct upload call-target audit confirms all emitted nontrivial callees,
including ImageCreateFromFile, GL imports, String lifecycle/assignment,
new/delete, ArrayAdd and ImageRelease, against the original ELF. It is not
an instruction-offset equality claim: compiled code emits one fewer static
glTexParameteri call, and the compiler eliminates three calls to the known
empty AELabelObject body. Indirect image callback dispatch remains a supplied
callback, not a statically resolved address. The callback path is exercised
by the real driver-error test. GLES entry points are linked to the actual
system/ANGLE libraries in the runtime tests.

## Validation and reproduction

Native UCRT64 `gof2` builds at `-O2 -g0`. Final matching NDK r18b build:
**208/208 units, zero failures**. Verifier unittest suite:
**55 tests, one existing skip**. New ARM evidence tests pin original manager
indirection, reacquisition, allocation size, cleanup order and call targets.
The preceding first-GL-boundary/cached-atlas regression gate also passes at
both host optimizations; its cached cases remain separate evidence.

SHA-256 comparison of all ARM objects finds exactly one changed unit,
`engine/core/AbyssEngine.o`; the other **207 are byte-identical** to baseline.
All 66 compared function pairs in that unit remain present. Only the upload
row changes: 674 → 673 reconstructed instructions against 713 original
instructions; all other rows retain their metrics and exact classifications.

Fresh before/after full corpus runs both compare **4,323/4,524 original
functions in 4,731 pairs**, with zero skipped units and zero wrong-type
signatures. Whole-corpus rounded means remain **93.90% source-shape / 83.17%
strict fuzzy**. The 201 absent symbols remain the existing support/runtime
inventory; this is not a claim of complete gameplay implementation.

Independent normalized `(unit,symbol)` **set comparison** finds:

| Exact classification | Before | After | Lost | Gained |
| --- | ---: | ---: | ---: | ---: |
| Linked-exact | 2,545 | 2,545 | **0** | **0** |
| Byte-exact | 1,079 | 1,079 | **0** | **0** |

No comparison pair is added or removed. Only the upload row changes.
The fresh baseline hash exactly reproduces the recorded integrated base.
Local reports and SHA-256:

```text
_work/gles-before-full.json
8cc79a127547bbcdc11753b77639127cb661f6416c31111506ed68ff1233c802
_work/gles-after-full.json
57164295bcd5f762baf93399ffcdb6fd1140fdc38bac71f9534b36855c6cd878
_work/gles-exact-delta.json
c459b384b86e829019a19eee2f865e42cd3aa592b5e4e38ac37e30dec6cd5a2a
_work/gles-family-final.json
ca41d764d1341cfdfd6c0edec624453c7a9b3f173b5d62981695380280c44cee
_work/gles-upload-callees.json
04598c77ad8a01b13a88c76fb8bff5b88148b6adf418e91ea089bcea9d6ac669
_work/gles-host-final.log
1daa4b278a7350f6cbc7c262541c14df89cbb9125f76a10aa87fc1c4d1baa2f3
_work/gles-android-final.log
63df6133423ceeea3af6ca8d7e8a9cc220276e55e6d9bbdc4bc225bfdf6f4289
```

Windows (existing tool paths may be supplied explicitly):

```powershell
python tools/run_gles_atlas_asset_gate.py --cxx C:/msys64/ucrt64/bin/g++.exe `
  --out-dir _work/gles-host --asset-root '<original OBB assets directory>' `
  --angle-dir '<Android SDK>/emulator/lib64/gles_angle' `
  --egl-include '<NDK>/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include'
```

Android, after preparing those fixtures (requires an already authorized ADB
device; installs no app):

```powershell
python tools/run_gles_atlas_android_gate.py --out-dir _work/gles-host `
  --asset-root '<same original assets directory>' --ndk '<NDK 25.2.9519653>' `
  --adb '<Android SDK>/platform-tools/adb.exe' --serial '<adb-device>'
```

Both runners execute `-O0` and `-O2`, with assertions enabled and `-g0`.
Use `--reuse-inputs` on the Windows runner only after initial fixture
preparation; it still refreshes the changing upload body from production.
Reports, logs, generated fixtures and executables remain local under `_work`
and are not committed. Original files are neither copied nor committed.

Next: recover/validate `SolarSystem::getStations → FileRead::loadStationsBinary`
and the following StarSystem world-texture path against Android evidence.
Keep the new real atlas context and managed-record contract while advancing
that next boundary; do not treat the remaining bootstrap/material fixtures
as a complete native runtime.
