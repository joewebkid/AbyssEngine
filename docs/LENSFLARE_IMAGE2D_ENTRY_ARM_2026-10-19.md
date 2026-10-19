# LensFlare / Image2D entry: ARM contracts and bounded asset gate

Status: **partial native initialization**, not a native game or GPU test.
Base: `51bddeaa74b7264d80e59ee8cef78a54d4d5a558`, isolated branch
`codex/astra-lensflare-entry-20261019`.

The continuous production Level path now crosses the old
`Image2DCreate(0x508)` boundary, resolves the actual platform atlas, and enters
the production texture upload body. It stops at its **first `glGetError`**.
No LensFlare texture handle is generated, no Image2D is appended, LensFlare
does not finish, StarSystem does not finish, and Level stage 0 does not return.

A separate CPU test starts with an explicitly supplied cached texture handle
and runs the actual three-region loader and LensFlare constructor/update/
destructor on three original Android atlases. That cached precondition is not
a verified upload and is never used to resume the continuous Level gate.

## Sources inspected and changes

Read `AGENTS.md`, `README.md`, `CONTRIBUTING.md`, project status, validation,
station/continuous-entry and AEI texture/parser notes; LensFlare, Image2D,
PaintCanvas, Engine, AbyssEngine, Array, Mesh, ResourceTexture, StarSystem,
the existing station/Level/image/resource gate tools and their callers.
Read the resource table only to reuse its existing registration bodies.

Primary source: original Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The Android IDA bodies in the external `analysis/gof2_libgof2hdaa_full_ida.c`
were a cross-check. No iOS/PC behavior or visual heuristic was substituted.

| Android anchor | Confirmed contract / correction |
| --- | --- |
| Image2DCreate `0x79c78`; lookups `0x79c90`, `0x79ca4` | Replace unresolved resource adapters with actual `FindResource`. A missing image or backing texture leaves output untouched; the texture lookup precedes image-cache reuse. |
| `0x79cb4–0x79cc2` | Local output starts at zero; call `TextureCreate(textureID, output, false)`. The old shim omitted the output reference and passed true. |
| `0x79ccc–0x79ce8` | Allocate `0x18` on ARM and clear bytes 0 through `0x14`; do not invent initialization of tail padding. Common Image2D now includes restore byte +0x14 and uses host-sized allocation. |
| `0x79cf6`, `0x79cfe–0x79d1c` | Call actual `ImageCreateRegionFromFile`, propagate cached texture handle only if it is not -1, append via the native `ArrayAdd<Image2D*>` specialization, cache the image index, then write output. |
| Image2D record +4 | Keep the texture/legacy overlay word 32 bits on the host. The old private pointer union widened the word and shifted all atlas fields relative to the common record. Assertions lock shared/private size, width and restore offsets. Legacy pointer interpretation in other draw routines remains unverified. |
| LensFlare ctor `0x11d00c`; loop call `0x11d03c` | Existing three IDs `0x508–0x50a`, global-canvas image creation and supplied-canvas width/height are retained. ARM assertions lock size `0x14` and offsets +4/+8/+0xc/+0x10. |
| LensFlare dtor `0x11d06c`, call `0x11d076` | Original uses scalar `operator delete`, then zeroes the buffer pointer. Replace the `delete[]` call with that explicit native deallocator. Canvas owns the images. |
| GetWidth `0x78fb4`, GetHeight `0x78fba` | Replace unresolved getter shims with real `Engine::GetDisplayWidth/Height`. Thumb/ARM veneers `0x19b1c8/0x19b1d8` resolve to PLT `0x5e8cc/0x5e8d8`; the Engine bodies `0x84530/0x84536` read +0x368/+0x36c. |
| TextureCreate `0x79a34`, wrapper `0x79b00`, upload `0x6f7f4` | Existing production bodies are retained and executed to the first GL call. Resource cache invalidation, callback-free arguments, scale and managed flag are checked. |

`BuildResourceList.cpp`, resource records, `docs/PROJECT_STATUS.md` and
`docs/VALIDATION.md` were not changed. No resource/GPU success stub was added.
The native allocation/deallocation mismatch is intentionally explicit; this
is an Android contract recovery, not a claim that arbitrary replacement C++
allocators can interchange scalar and array allocation families.

## Continuous path and separate cached branch

Run:

```powershell
$env:PATH = 'C:/msys64/ucrt64/bin;C:/msys64/usr/bin;' + $env:PATH
python tools/run_lensflare_asset_gate.py `
  --cxx C:/msys64/ucrt64/bin/g++.exe `
  --out-dir _work/lensflare-host `
  --asset-root 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets'
```

The runner retains the preceding Level/mesh smoke checks. It reuses the
station runner's verbatim extraction and source files, then adds verbatim
Image2DCreate, getters, MeshCreate, ImageCreateRegionFromFile, Engine getters,
LensFlare destructor/update, and the entire texture-upload body. The latter
is namespaced only to separate it from the inherited skybox fixture service;
its body is unchanged. Every other GL entry point aborts if reached.

The first platform records are source-extracted after the 2,488-entry table
and two low skybox textures. On the original isolated branch, ordinary flags
add 15 records, for **2,505 resources**. The integrated gate now extracts
the entire recovered platform/post-platform tail: 234 ordinary records and
**2,724 resources** total. This matters: texture 10062 is not in the main
table; testing the previous table alone would silently return for all flare
IDs. The larger registration gate still does not perform a GPU upload.

Continuous measured chain:

1. Level constructor, ten particle managers and createSpace.
2. Production PlayerStation, both PlayerJumpgate objects, StarSystem entry.
3. LensFlare calls Image2DCreate(1288); image payload is texture 10062, region 187.
4. Both actual lookups succeed; production TextureCreate invalidates its two
   bound texture slots and enters TextureCreateFromFileIntern with the ordinary
   interface path, null callback/user/record, scale 0 and managed=true.
5. First `glGetError` (ARM call **0x6f820**) throws a dedicated boundary exception. Image and texture
   resource handles remain -1, canvas image count remains zero, and no atlas
   file is opened on this continuous path yet.

The station counts stay **17 meshes, 15 transforms, 44 collision volumes,
14 opens / 14 closes**, with the preceding gate's ordered paths and material
requests checked. The earlier **skybox upload, materials, file service and
Engine/PaintCanvas bootstrap remain test boundaries**. This change does not
turn them into a validated GPU or platform implementation.

For the separate cached branch, each atlas first passes the real
ImageCreateFromFile parser (RGBA, no mipmaps). Handle 73 is then supplied as
the explicit cache precondition. Production LensFlare creates its three
Image2D records through real region parsing and real quad MeshCreate.

| Original file under `data/textures/` | Dimensions | Regions 187 / 188 / 189: x,y,w,h |
| --- | --- | --- |
| `gof2_interface.aei` | 1024x1024 | 71,934,64,64 / 160,902,64,64 / 285,601,64,64 |
| `gof2_interface_iphone4.aei` | 2048x2048 | 561,1265,64,64 / 1811,102,64,64 / 1632,855,64,64 |
| `gof2_interface_ipad_1440.aei` | 2048x2048 | 366,1684,64,64 / 436,1767,64,64 / 1978,191,64,64 |

The fixture checks all positions, normalized UVs and six indices, dimensions,
texture handle and restore byte, image/resource indices, no extra opens on
cache reuse, untouched output for missing resources, and the missing-texture
check even when the image is cached. Getters, display dimensions, update reset
and destructor execution are checked. It does not read the constructor's
uninitialized intensity. There are **12 additional matched opens/closes**
across the three separate CPU cases, covering nine regions and three full
pixel payloads; these are not twelve additional unique files.

## Validation and exact-pair comparison

Native UCRT64 Release `gof2` builds after the final layout changes. The final
NDK r18b build compiles **208/208 TUs, zero failures**. The verifier suite
passes **49 tests, one existing skip**. The final continuous and separate CPU
asset gates pass at **-O0 and -O2**, as do the inherited Level and mesh checks.

Focused before/after uses the same 17 `(unit, symbol)` pairs: all six
LensFlare entries (including C1/C2 and D1/D2 aliases), Image2DCreate, both
TextureCreate overloads, both canvas dimension getters, both image dimension
getters, ImageCreateFromFile, ImageCreateRegionFromFile, TextureCreateFromFile
and TextureCreateFromFileIntern.

| Focused metric | Base 51bddeaa | After |
| --- | ---: | ---: |
| Unweighted source-shape | 97.85% | 98.55% |
| Unweighted strict fuzzy | 84.07% | 86.76% |
| Original-instruction-weighted source-shape (1,956 instructions) | 94.68% | 95.12% |
| Original-instruction-weighted strict fuzzy | 57.74% | 59.40% |
| Linked-exact / byte-exact pairs | 11 / 3 | 11 / 3 |
| Image2DCreate source-shape / strict fuzzy | 84.6% / 36.2% | 96.6% / 81.9% |

The **17-pair mean exceeds 98%; the Image2DCreate body does not**. It is not
byte- or linked-exact. The final instruction counts are 71 original / 73
reconstructed; the compiler keeps the first texture-ID load across the lookup
and chooses different registers. No artificial volatile/register/stack work
was introduced to force its score.

Other dependency scores remain unchanged: LensFlare render2D **94.8% / 48.7%**,
region parser **99.0% / 83.5%**, pixel parser **97.3% / 58.8%**, main canvas
TextureCreate **95.5% / 51.7%**, upload intern **92.2% / 50.3%**. The remaining
11 focused pairs are 100%/100%; three are byte-exact. LensFlare alone has a
99.13% unweighted source mean, but 95.64% instruction-weighted source mean;
constructor/destructor aliases must not conceal the render body's limits.

The masked-byte `linked_equal` metric does not prove call destinations. The
scalar-delete correction preserves the old exact counts while repairing the
callee. A separate LensFlare direct-call audit reports five resolved matching
calls, zero mismatches and five unresolved **alias-header** duplicates (C2/D2
share the original C1/D1 addresses). Original disassembly independently
confirms the destructor call to `_ZdlPv` at 0x11d076.
The Image2DCreate relocation list independently confirms both FindResource
calls, the output-reference TextureCreate overload, operator new, the actual
region parser and the Array specialization. Getter relocations target the
actual Engine getters; no removed adapter remains on these paths.

Focused reports: `_work/lensflare-before-focus.json` (rows from the fresh full
baseline), `_work/lensflare-after-focus.json`; call audit:
`_work/lensflare-call-targets.json`. Reports/builds/assets are not committed.

Fresh full reports are `_work/lensflare-before-full.json` and
`_work/lensflare-after-full.json`. The baseline report hash exactly reproduces
the recorded combined 51bddeaa baseline. Both runs compare **4,323/4,524
original functions in 4,731 unit/symbol pairs**, with 201 missing, zero skipped
units, zero wrong-type signatures, and 125 extra symbols.

| Full-corpus metric | Before | After |
| --- | ---: | ---: |
| Mean source-shape | 93.90% | 93.90% |
| Mean strict fuzzy | 83.16% | 83.17% |
| Linked-exact pairs | 2545 | 2545 |
| Byte-exact pairs | 1079 | 1079 |

Normalized **(unit, symbol) set comparison**, recorded in
`_work/lensflare-exact-delta.json`, finds **0 lost / 0 gained linked-exact**
and **0 lost / 0 gained byte-exact** pairs. No comparison row is added or
removed. Two nonexact rows change: Image2DCreate improves as above;
`PaintCanvas::DrawImage2D(unsigned,int,int,unsigned char,unsigned char)`
keeps 82.0% source-shape but strict fuzzy decreases **38.0% -> 36.8%**.
That decrease is retained and reported; the draw routine has not been
runtime-certified by this entry gate.

Full-report / pair-delta SHA-256:

```text
before: 614f6dd4c335a720eb952318b3d95e5112ff9a12060a9e68575e0f15dba57c95
after:  a24ae41f8072c6dca9e2bc7e44e305448cb95d57bf7b17eaab282fb395bcedd4
delta:  d69b11d5a6dacb74de9d45d1f62f71f7de6d7c750a0957e55f8ed54c1f2a00c6
```

## Limits and next priority

- There is no GLES context, texture creation/upload, draw, framebuffer or ARM
  device test here. The first GL query is the continuous boundary, before the
  interface image parser in the upload function. The separate CPU parser test
  does not advance that boundary.
- `LensFlare::render2D` is unchanged. Its seven sprite draws, fullscreen color
  overlay, alpha conversion and distance math have not been runtime-validated
  by this entry gate. Native width-for-height calls are retained.
- Native ctor does not initialize intensity; update must run before using it.
  Original region-loader and Image2DCreate failure cleanup/leaks are retained;
  this gate does not certify malformed-region/file-error lifecycle recovery.
- StarSystem's later fixed-width target-array allocation, world textures,
  player creation, later Level stages and complete destruction remain outside
  this workstream. Full BuildResourceList execution is not claimed.
- Next priority: a real GLES/platform host gate (preferably ARM) through
  TextureCreateFromFileIntern's atlas upload and managed record publication.
  Then remove the cached precondition and continue production LensFlare and
  StarSystem, fixing their next evidenced boundary. Do not substitute a
  fabricated successful texture to claim stage-0 completion.
