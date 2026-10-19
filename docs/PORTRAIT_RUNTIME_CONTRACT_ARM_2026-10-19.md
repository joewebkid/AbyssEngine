# Portrait registration lifetime and first consumer (2026-10-19)

Base: `22193e4062bad5a965668b2a60c25a2c3d9438f8`, isolated checkout
`_work/sol-portrait-runtime-20261019`, branch `codex/sol-portrait-runtime-20261019`.
Production changes are confined to `src/engine/render/ResourceMaterial.cpp`.
No BuildResourceList, platform/GLES, PaintCanvas, LensFlare, shared project status
or validation document changes. The gate reads those sources without editing them.
Inspected: ResourceMaterial, ResourceTexture, ResourceMesh, BuildResourceList,
PaintCanvas, ImageFactory/ImageFactoryPortraitData, String, the relevant AbyssEngine
loader/concatenation bodies, and the existing portrait/resource/asset gate fixtures.

## Primary evidence and correction

Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`_Z13loadPortraitsPN11AbyssEngine6EngineE`, Thumb `0x112d4c`, size `0x543a`.
The binary and OBB extraction are read in place, never copied into this checkout.
The existing [flag evidence](PORTRAITS_GLOBAL_FLAGS_ARM_2026-10-19.md) identifies
the five GOT relocations and all 152 ordered base paths and resource IDs.

The original normal path allocates Resource and ResourceTexture before constructing
the base-path String (`0x112de8`, `0x112df0`, `0x112dfc`). It performs two String
concatenations and the texture constructor, fills the resource, calls AddResource
(`0x112e54`), and only then destroys the four temporary Strings (`0x112e60–0x112e6c`).
The previous named `fullName` statement destroyed three intermediate Strings
before the resource allocations and before AddResource.

The registrations now use one full expression per record: AddResource of a new
aggregate Resource whose payload is a new ResourceTexture of the concatenated
String. This restores the allocation/temporary lifetime contract, preserves the
Resource layout, and removes no failure check or resource variant. The optional
ARM evidence test now also verifies the normal-path sequence of eight named calls
for **each of the 152 records**, rather than checking only strings and IDs.
Four indirect String destructor calls follow each original AddResource. Resource
field-store scheduling is still compiler-dependent; an exact match is not claimed.

The five zero/nonzero byte tests retain this priority:

| Condition | Suffix | Original files present |
| --- | --- | --- |
| iPadLarge | `_ipad_large` | 152/152 |
| otherwise iPadHD | `_ipad_1440` | 148/152 |
| otherwise retinaDisplay OR iPad OR n9 | `_ipad` | 150/152 |
| otherwise | empty | 152/152 |

The six absent paths remain `data/textures/4_2_3_ipad.aei`,
`4_2_4_ipad.aei`, and `12_0_0_ipad_1440.aei` through
`12_3_0_ipad_1440.aei` in that directory. There is no invented fallback.
Android ARM is the authority for this patch; iOS/PC inputs were not needed to
change behavior and no cross-platform asset equivalence is claimed.

## Continuous bounded registration and consumer gate

`tools/run_portrait_runtime_gate.py` source-extracts the contiguous production
BuildResourceList segment from `Resource *resources[2488]` through the actual
`loadPortraits(engine)` call. This includes:

- the complete 2,488-slot table and production bulk SetResourceList;
- the **152 Image-resource loop** immediately after that table;
- the complete platform/post-platform registration tail;
- the complete production loadPortraits body and constructors/String operations.

The ordinary route therefore has **3,026** records at this boundary:
2,488 + 152 Image records + 234 platform/common records + 152 portrait textures.
The 152 Image loop is essential: image IDs 5000–5151 reference texture IDs
10200–10351, region zero. A table-plus-platform test omitting this loop cannot
demonstrate the portrait consumer contract. The initial TextureCreateGlobal GL
prelude and subsequent loadLowTexturesAndMaterials are outside this bounded gate.

For all 32 combinations of the five flags (including distinct high-bit nonzero
values), every portrait's ordered ID, suffix/path, resource type, unloaded state
and Image-resource mapping is checked. Production ImageFactory::loadImage then
calls production Image2DCreate, FindResource, TextureCreate and AddResource code.
The negative-ID row returns null without an upload. All 4,864 positive calls stop
by throwing at the **entry to TextureCreateFromFileIntern**. No upload body or
GL call runs, no successful GPU result or cached handle is injected, no ImagePart
constructor runs, and no Image2D is created. Texture and Image resource handles
remain -1. Host Engine/ApplicationManager/Canvas bootstrap is a fixture service,
not a claim about the native object layout or platform startup.
As in the previous portrait smoke, the fixture supplies the String_GetAEChar
adapter by calling production String::GetAEChar. The production ResourceTexture
source still calls that free-function shim, whose definition is absent from src;
the static-library build does not prove it can link into a complete executable.

The ImageFactory descriptor is checked from its actual ID table: rows 9, 11 and
12 expose column zero, so the consumer column/frame cannot universally be
inferred from the filename part/variant. The gate covers these resource routes
directly; it does not assert that createChar generates all these descriptors or
repair its choice/ID tables without further evidence.

In a **separate CPU parser check**, production ImageCreateFromFile/ImageRelease
read all four filename families from the original Android OBB extraction:
602 present files parse successfully, six missing files return -1 and null output,
and all 602 successful opens have matching closes. This establishes parser and
missing-open behavior at the bounded file service; the upload-entry stops are
never resumed and the parser check supplies no texture handles. It does not
demonstrate decompression by a GPU, visual correctness, or the real platform
file provider's missing-asset policy.
With `--obb`, the runner first compares all 602 parser input files byte-for-byte
against ZIP entries in the original `main.47947006.net.fishlabs.gof2hdallandroid2012.obb`
and verifies that the same six files are absent from the archive. No files are extracted.

## Validation

Commands run in the isolated checkout; original binary/symbols and NDK r18b are
external through `GOF2_VERIFY_WORK` and `GOF2_NDK_ROOT`. Native CMake uses
`-O0 -g0`; matching ARM uses the unchanged canonical `-Oz` flags.

```powershell
python tools/run_portrait_runtime_gate.py --cxx C:/msys64/ucrt64/bin/g++.exe --assets 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets' --obb 'A:/Galaxy on Fire new version/references/unpacked/gof2_hd_android/main.47947006.net.fishlabs.gof2hdallandroid2012.obb'
python tools/run_load_portraits_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe --assets 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets'
python tools/run_image_factory_portrait_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
python -m unittest discover -s tools/verify -p 'test_*.py'
```

Native `gof2` builds. Both portrait gates pass at `-O0/-O2`; the older one
checks 96 flag/value cases and all 152 ordered IDs/paths. The existing
ImageFactory smoke passes; 53 verifier tests run with one existing skip and no failures.

For the **loadPortraits body**, focused source-shape rises **92.5% -> 98.6%**,
strict fuzzy **66.8% -> 89.5%**. Unrelated ResourceMaterial constructors are
unchanged and are not used to pad this metric.
Target/base instruction counts are 8,771/8,770 after the change (previous base
8,304); linked-exact and byte-exact are both false. The remaining 1.4 source-shape
points are not evidence of missing behavior and were not pursued with semantic
changes or verifier normalization.

The immediate two-symbol registration/payload family also includes
`_ZN11AbyssEngine15ResourceTextureC2ERKNS_6StringEf`, unchanged at **90.2%
source-shape / 53.3% strict fuzzy**. Its unweighted family mean is therefore
**94.4% source-shape / 71.4% strict fuzzy**, below the requested 98% family target.
The constructor lies outside the permitted production edit file. Original ARM
`0x11b6e2` calls String::GetAEChar directly, and `0x11b6e8` reads String length
at +8 for the allocation/copy size; current ResourceTexture.cpp uses the free
shim and GetStringLength on the returned bytes. These are follow-up evidence,
not changes smuggled into ResourceMaterial or a claim about arbitrary Unicode
paths. All portrait paths in this gate are ASCII.

The full first-consumer family is defined as the following eight distinct symbols
(constructor aliases are not counted twice). It measures **96.80% source-shape /
78.66% strict fuzzy** after the change, also below 98%. These rows use the
unchanged verifier's disassembly/compare functions on the after-build objects;
the first row is the only changed object/function in this family.

| Symbol / method | Source-shape | Strict fuzzy |
| --- | ---: | ---: |
| loadPortraits | 98.6% | 89.5% |
| ResourceTexture(String const&, float), C2 | 90.2% | 53.3% |
| PaintCanvas::AddResource | 100.0% | 100.0% |
| PaintCanvas::FindResource | 100.0% | 100.0% |
| PaintCanvas::TextureCreate(callback overload) | 95.5% | 51.7% |
| PaintCanvas::TextureCreate(id, out, bool) | 100.0% | 100.0% |
| PaintCanvas::Image2DCreate | 96.6% | 81.9% |
| ImageFactory::loadImage | 93.5% | 52.9% |

Both full runs use the unchanged verifier with `--no-build --fail-on-wrong-type`
and independent before/after base and target directories. The after build reuses
derived objects as incremental inputs, not original game binaries. SHA-256 of all
208 object pairs confirms only `engine/render/ResourceMaterial.o` changed; the
other 207 are identical. Both ARM build gates report 208 compiled, zero failed.

Both full reports compare **4,731 (unit, symbol) pairs**, covering **4,323/4,524**
original functions. Both have **2,545 linked-exact**, **1,079 byte-exact**,
zero skipped units, zero wrong-type signatures, 201 absent originals and 124 extra
symbols. The rounded corpus means remain **93.90% source-shape / 83.17% strict
fuzzy**. Actual `(unit, symbol)` set comparison finds **zero losses and zero gains**
for both exact categories. Comparing every report row finds only loadPortraits
changed. There is no exact-match improvement hidden in the family mean.

Generated local reports are not staged:

- `_work/portrait-before-full.json`, SHA-256
  `8cc79a127547bbcdc11753b77639127cb661f6416c31111506ed68ff1233c802`;
- `_work/portrait-after-full.json`, SHA-256
  `9ba339f078f2674908d4f11e940d801ab3dece680cacbe1d997dbc6e4c3db4d1`;
- `_work/portrait-exact-pairs.json`, pair-set/row comparison summary;
- `_work/portrait-before-focused.json`, `_work/portrait-after-focused.json`,
  `_work/portrait-consumer-focused.json`, and before/after object hash manifests.

The ARM commands use the usual local NDK environment documented above:

```powershell
& C:/msys64/usr/bin/bash.exe tools/verify/build_objs.sh _work/portrait-arm
python tools/verify/verify.py --build-dir _work/portrait-arm --no-build --report _work/portrait-before-full.json --fail-on-wrong-type
& C:/msys64/usr/bin/bash.exe tools/verify/build_objs.sh _work/portrait-arm-after
python tools/verify/verify.py --build-dir _work/portrait-arm-after --no-build --report _work/portrait-after-full.json --fail-on-wrong-type
```

## Boundary and next step

The verified boundary is registration followed by the first portrait consumer's
entry to the texture uploader. Next, with broader file scope, repair and validate
the ResourceTexture String adapter/length contract, then the platform/GLES uploader and real file
provider, including a non-Retina tablet test for the reachable missing race-4
variants. A native executable/device run and visible portrait remain outstanding.
Neither this host gate nor ARM source-shape proves a playable game or GPU success.
