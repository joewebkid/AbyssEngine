# ResourceTexture String contract (2026-10-19)

Base: published main `35c4e0bdcd2bbbce2946689d0b23e008b489ec8b`.
Branch: `codex/sol-resource-texture-string-20261019`, in the existing isolated
`_work/sol-portrait-runtime-20261019` checkout. The preceding portrait branch
and its commit are retained. No shared status, validation, atlas/portrait gate,
SolarSystem/StarSystem/FileRead or platform files are edited.

## Family fixed before implementation

Three distinct symbols define conversion and buffer ownership; constructor
aliases are not counted twice and unrelated methods are not added for the mean.

| Symbol | Before source-shape | Before strict fuzzy |
| --- | ---: | ---: |
| `_ZN11AbyssEngine15ResourceTextureC2ERKNS_6StringEf` | 90.2% | 53.3% |
| `_ZNK11AbyssEngine6String9GetAECharEv` | 73.3% | 66.7% |
| `_ZN11AbyssEngine15ResourceTextureD2Ev` | 100.0% | 100.0% |
| Unweighted three-symbol mean | **87.83%** | **73.33%** |

The `const char*` constructor is a separate unchanged regression control:
100% source-shape/strict fuzzy, linked-exact, not raw-byte-exact.
The existing eight-symbol portrait consumer family is not redefined as this
three-symbol family; the latter is specific to this newly assigned task.

Primary source: original Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Constructor `0x11b6d2–0x11b70c`, conversion `0x72752–0x72782`, destructor
`0x77ebc–0x77ece`. Original data and binaries stay in their existing locations.

## Confirmed contract and minimal changes

Inspected ResourceTexture.cpp/h, AEString.h, the relevant String.cpp methods,
all direct GetAEChar call sites, the existing String/portrait test fixtures and
raw original ARM. No iOS/PC inference is needed for this Android contract.

At `0x11b6e2` the constructor calls the const member String::GetAEChar directly.
At `0x11b6e8` it loads saved String length from +8, adds one, allocates that many
bytes, copies exactly that size, stores its float, and deletes the conversion
buffer at `0x11b702`. The original does not recompute strlen after conversion.
The destructor deletes the separately owned name array and then clears its pointer.
The target constructor's name/value offsets remain 0/4; there are no header,
signature or object-layout changes.
These offsets are the 32-bit ARM ABI. The host executable uses its host layout;
the ARM offset evidence comes from the original instructions and matching ARM
objects, not from assuming that host pointer widths match the game.

ResourceTexture.cpp now directly calls `name.GetAEChar()` and copies
`uint32_t(name.length) + 1` bytes. This removes its real undefined free-function
String_GetAEChar reference without defining a new shim. The unused historical
declaration in String.h is left alone; a declaration is not an executable dependency.
The misleading local variable name `utf8` is changed to `bytes`: the original
conversion is not UTF-8 encoding.

The narrowly necessary String.cpp change is justified by primary instructions,
not just matching score. For nonzero length, ARM `0x72758–0x72776` allocates
`length + 1` bytes and copies the **low byte of every 16-bit unit through
data[length]**, using LDRB at an index scaled by two. It does not append a
separately synthesized zero. For zero length, `0x72778–0x72782` allocates one byte
and writes zero without reading data. The previous helper copied only length
units and appended zero. Only GetAEChar's body is changed; String.cpp/h are not
otherwise refactored and String.h/AEString.h are unchanged.

This also explains embedded zeros: U+0100 maps to byte 0, but subsequent units
are still converted and copied according to saved length. Surrogate units are
handled independently; there is no Unicode decoding, replacement or UTF-8 policy.
Canonical strings still have a zero terminal unit. A separate raw stored-state
probe with a nonzero data[length] verifies the original low-byte rule without
claiming that such a state is a canonical text string or a valid asset path.

## Focused result with the original fixed family

| Symbol | After source-shape | After strict fuzzy | Linked-exact | Byte-exact |
| --- | ---: | ---: | --- | --- |
| ResourceTexture(String const&, float), C2 | 100% | 100% | yes | no |
| String::GetAEChar() const | 100% | 100% | yes | no |
| ResourceTexture destructor, D2 | 100% | 100% | yes | no |
| Three-symbol mean | **100%** | **100%** | **3/3** | **0/3** |

Before: 87.83% source-shape, 73.33% strict fuzzy, 1/3 linked-exact, 0/3
byte-exact. The separate const-char constructor remains 100%/100%, linked-exact,
not byte-exact. There are no compiler-flag, verifier-normalization or symbol-set
changes. These are linked-exact comparisons, not raw binary identity or a claim
that the rest of String or the game is complete.
The additional whole-String-TU comparison changes only GetAEChar's row, gains
that linked-exact pair, and loses no prior linked-exact or byte-exact pairs.

## Executable and ARM evidence gates

`run_resource_texture_string_smoke.py` compiles the **actual String.cpp and
ResourceTexture.cpp translation units** with the new profile fixture at
`-O0/-O2 -g0`; it does not source-extract replacements or supply String_GetAEChar.
Its six cases cover empty/null data, ASCII, a real embedded NUL produced by
String::operator+=(char), non-ASCII units with early zero low bytes, a noncanonical
stored terminal unit, and zero saved length with non-null data. It checks:

- two distinct length+1 allocations, temporary conversion freed before return;
- exact copied bytes beyond the first zero, unchanged source pointer/length;
- an independently owned texture name, freed exactly once by its destructor;
- exact float bits, including negative zero and a quiet NaN payload;
- the unchanged const-char constructor as a control.

The fixture's array allocator observes allocation sizes and frees; allocation
policy is a test boundary. Other unexecuted String methods have explicit abort
links for GameText::getLanguage, operator+, computeFloatString and String_printImpl
because the Windows linker retains their unresolved references. These traps are
not exercised by the contract tests and no GPU service is supplied. The tested
conversion and resource constructor/destructor execute their production bodies.

Negative controls use the published baseline source: the same component test
fails to link on String_GetAEChar; using baseline String.cpp with the fixed
constructor separately fails the stored terminal low-byte assertion as expected.
The repaired native ResourceTexture object and entire libgof2.a have no undefined
String_GetAEChar reference, checked with nm. This is a closed live component
link, not a full-game link.

`test_resource_texture_string_arm.py` independently checks original named call
order, +8 length loads, length+1 size, low-byte loop, empty branch, owned-buffer
delete and pointer clear. All 59 verifier tests run with one existing skip and
no failures. The existing String reverse fixture passes at `-O0/-O2` too.

The unchanged portrait runtime gate passes at `-O0/-O2`: 32 flag routes, 4,864
positive consumer stops at upload entry, 602 CPU image parses and six missing
variants. All 602 inputs are byte-equal to their original OBB entries. Its old
fixture adapter remains present but the production constructor no longer calls
it. Neither this regression gate nor the new component test creates a visible
portrait or demonstrates full Level initialization.

## Builds, corpus and limits

Native gof2 builds in the existing `_work/portrait-native` directory with
`-O0 -g0`. Both existing ARM build directories report 208/208 units, zero failed,
with the unchanged canonical NDK r18b `-Oz` flags. A SHA-256 comparison of all
208 object pairs finds only ResourceTexture.o and String.o changed; the other
206 are identical. Original binaries/assets are read in place, not copied, and
no build directories or assets are deleted.

Both complete reports compare **4,731 (unit, symbol) pairs**, covering
**4,323/4,524** original functions, with zero skipped units, zero wrong-type
signatures, 201 absent originals and 124 extra symbols. Corpus means rise from
**93.90%/83.17% to 93.91%/83.19%** source-shape/strict fuzzy.

Actual `(unit, symbol)` set comparison finds **zero linked-exact or byte-exact
losses**. Linked-exact grows **2,545 -> 2,547**, gaining exactly the String
ResourceTexture constructor and GetAEChar. Byte-exact remains **1,079**, with no
gains. Comparing every report row finds only those two functions changed. The
fresh before report is byte-identical to the published main report, including
its SHA-256. This independently confirms the requested starting state.

Local generated reports and scratch controls are not staged:

- `_work/texture-string-before-full.json`, SHA-256
  `d02e8536269ed0924aa6ef6e8da267b76876e8f4ac47d510eb0d2f311970ff71`;
- `_work/texture-string-after-full.json`, SHA-256
  `8c49a3f18c6cdbcfe7ea31b2192e9e8b31ef25acea8196f012e1477482613f05`;
- `_work/texture-string-exact-pairs.json`, complete pair-set/row comparison;
- `_work/texture-string-family.json`, fixed-family before/after rows and means;
- `_work/texture-string-object-hashes.json`, all 208 before/after object hashes;
- `_work/texture-string-before/after-resource.json` and
  `_work/texture-string-before/after-conversion.json` focused reports (each
  before/after spelling denotes two separate files);
- `_work/texture-string-before-all-string.json` and its after counterpart;
- `_work/texture-string-host.log`, `texture-string-tests.log`,
  `texture-string-negative.log` and `texture-string-portrait.log`.

Full ARM runs, using the existing directories and canonical environment:

```powershell
& C:/msys64/usr/bin/bash.exe tools/verify/build_objs.sh _work/portrait-arm
python tools/verify/verify.py --build-dir _work/portrait-arm --no-build --report _work/texture-string-before-full.json --fail-on-wrong-type
& C:/msys64/usr/bin/bash.exe tools/verify/build_objs.sh _work/portrait-arm-after
python tools/verify/verify.py --build-dir _work/portrait-arm-after --no-build --report _work/texture-string-after-full.json --fail-on-wrong-type
```

Reproduce the component and read-only evidence gates from this checkout:

```powershell
python tools/run_resource_texture_string_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
python -m unittest discover -s tools/verify -p 'test_*.py'
python tools/run_portrait_runtime_gate.py --cxx C:/msys64/ucrt64/bin/g++.exe --assets 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets' --obb 'A:/Galaxy on Fire new version/references/unpacked/gof2_hd_android/main.47947006.net.fishlabs.gof2hdallandroid2012.obb'
```

ARM tools use the existing external GOF2_VERIFY_WORK/original SO and
GOF2_NDK_ROOT/r18b installation. Full-game linkage, a visible portrait, real
non-ASCII file-provider behavior and complete level startup are not established
by this patch. The next portrait integration step is the real file/GLES path
using the repaired component, within a separately authorized integration scope.
