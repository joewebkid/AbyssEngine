# Portrait platform flags: Android ARM recovery (2026-10-19)

Scope: `loadPortraits(AbyssEngine::Engine*)` in `ResourceMaterial.cpp`.
Baseline: `1702643f`, isolated branch `codex/sol-portraits-20261019`.
No `BuildResourceList`, `Level`, shared status or validation document changes.

## Confirmed source links

Android 2.0.16 `libgof2hdaa.so`, Thumb function
`_Z13loadPortraitsPN11AbyssEngine6EngineE`, start `0x112d4c`, size `0x543a`.
Reference SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Each PC-relative literal leads to a GOT slot with an `R_ARM_GLOB_DAT` (21)
relocation naming the global. These are not inferred from device names.

| Old placeholder | Literal | ADD-PC | GOT | Relocation symbol | Global byte |
| --- | --- | --- | --- | --- | --- |
| ipadLargeFlag | 0x112d90 | 0x112d60 | 0x2103f0 | `_ZN7Globals9iPadLargeE` | 0x2182dc |
| ipad1440Flag | 0x112d98 | 0x112d7c | 0x21038c | `_ZN7Globals6iPadHDE` | 0x2182d5 |
| ipadFlagA | 0x112dcc | 0x112da6 | 0x2103e4 | `_ZN7Globals13retinaDisplayE` | 0x2182d6 |
| ipadFlagB | 0x112dd0 | 0x112da8 | 0x21032c | `_ZN7Globals4iPadE` | 0x2182c0 |
| ipadFlagC | 0x112dd4 | 0x112daa | 0x2103e8 | `_ZN7Globals2n9E` | 0x2182d7 |

For these Thumb ADD instructions, GOT address = instruction address + 4 +
32-bit literal. Loads at `0x112d70` and `0x112d80` test the first two bytes.
The three LDRB instructions at `0x112db2/0x112db4/0x112db6` feed ORRS at
`0x112db8/0x112dba`; the resulting low byte selects the third suffix.

Priority and string anchors:

| Condition, in order | Suffix | Literal / ADD-PC |
| --- | --- | --- |
| `Globals::iPadLarge != 0` | `_ipad_large` | 0x112d94 / 0x112d76 |
| `Globals::iPadHD != 0` | `_ipad_1440` | 0x112d9c / 0x112d86 |
| `retinaDisplay \| iPad \| n9` nonzero | `_ipad` | 0x112dd8 / 0x112dc0 |
| otherwise | empty | 0x112ddc / 0x112dc4 |

The source now reads these five `Globals` bytes directly. Removed the five
placeholder pointers and their shared zero byte. Preserved branch order,
bitwise OR, all resource constructors and all registrations.
Android evidence suffices; iOS/PC evidence was not needed.

## Resource path and execution gate

The read-only optional `test_load_portraits_arm.py` gate resolves ELF sections,
checks the five relocations and four suffix strings, and disassembles only this
function with the existing NDK wrapper. Its PC-relative string references and
MOVW/MOV.W resource constants confirm all **152 ordered base paths** and
**IDs 10200–10351 (`0x27d8–0x286f`)** against production source.
For example, `0x112dfa` selects string `0x1e5082` (`0_0_0`);
`0x112e3e` loads ID 10200. The final base path (`12_3_0`) uses ADD-PC
`0x117934`, string `0x1e5c70`, and ID 10351 at `0x11796a`.
Race 11 has only part 3 variants **2 and 3**, not variants 0 and 1.

`run_load_portraits_smoke.py` source-extracts the complete production
`loadPortraits` body, Resource layout, String operations, String concatenation,
and ResourceTexture constructor/destructor. The fixture supplies bounded
Engine/ApplicationManager/Canvas registration services and the existing
String-to-byte adapter. Language-dependent reversal asserts if called; every
tested constructor passes false. This is executable host behavior, not a
host implementation of the ARM object layout.

At both `-O0` and `-O2`, all 32 flag combinations run with each nonzero value
1, 0x80 and 0xff (96 cases, 14,592 registrations per optimization). The gate
checks priority, every ID/path in order, kind=2, unused=-1, texture value=0,
and registration count; it frees each registered resource and texture.
The expected ordered filename families come from the ARM-checked inventory.

The supplied original Android OBB extract has the following coverage:

| Suffix | Files present | Missing files |
| --- | --- | --- |
| empty | 152/152 | none |
| `_ipad` | 150/152 | `4_2_3_ipad.aei`, `4_2_4_ipad.aei` |
| `_ipad_1440` | 148/152 | `12_0_0_ipad_1440.aei` through `12_3_0_ipad_1440.aei` |
| `_ipad_large` | 152/152 | none |

All filenames are under `data/textures/`. With `--assets`, the host gate checks
every generated path against this inventory, including the six known absences.
No fallback has been invented. The game/platform file service's handling of
these missing variants still needs integration testing.

## Reachability of missing tablet variants

The original Android `OnCreateApplication` (`0xd0738`) loads device information
and, before calling `BuildResourceList` at `0xd096c`, writes `iPadHD = 0`
(`0xd091c`, GOT `0x21038c`), `iPad = dev.isPad` (`0xd0924`, GOT
`0x21032c`), and `iPadLarge = dev.isPad ? retinaDisplay : 0` (`0xd091e`–
`0xd092e`, GOT `0x2103f0`). These GOT slots are identified by
`R_ARM_GLOB_DAT` relocations, not inferred from names. Consequently, a
non-Retina Android tablet with `dev.isPad != 0` reaches the `_ipad` portrait
suffix. The `_ipad_1440` branch is not reached through this Android
initialization path because `iPadHD` is explicitly cleared; another platform
or later write would require separate evidence.

The original `ImageFactory::createChar(bool,int)` at `0x11c6f4` calls
`AERandom::nextInt` for four portrait parts (`0x11c74a`–`0x11c75c`). The
ARM-derived choice table gives race 4, part 2 five variants, with base image
ID 5094. Thus choices 3 and 4 can request image IDs 5097 and 5098 and their
texture resources 10297 and 10298, which name the two absent `_ipad` files.
This is a reachable missing-asset risk in the supplied OBB, not proof of a
crash: no end-to-end tablet run or alternate asset-provider behavior has been
verified. The supplied Android APK contains no replacement texture atlases.

## Validation

Commands run from the isolated checkout, with shared external
`GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work`,
`GOF2_VERIFY_LOCAL_NDK=1` and `GOF2_NDK_ROOT` pointing to its NDK r18b.
The original binary and assets were read in place and not copied or committed.

```powershell
python tools/run_load_portraits_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe --assets 'A:/Galaxy on Fire new version/extraction/gof2/obb_full/files/47947_GOF2CONTENT_ETC/assets'
python tools/run_image_factory_portrait_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
python -m unittest discover -s tools/verify -p 'test_*.py'
```

Native `gof2` build passes. ARM builds: 208/208 TUs before and after.
Host portrait gate passes at `-O0/-O2`; existing ImageFactory portrait smoke
passes. Verifier tests: 46 run, one existing skip, no failures.

Focused `loadPortraits`: source-shape **92.3% -> 92.5%**, strict fuzzy
**66.3% -> 66.8%**, still nonexact (8,771 target instructions;
8,261 -> 8,304 base instructions). The two ResourceMaterial constructors'
metrics are unchanged. The three-function focused report has no linked-exact
or byte-exact functions before or after.

Both complete runs compare **4,731 symbol/unit pairs**, covering
**4,323/4,524 original functions**, with **2,529 linked-exact** and
**1,076 byte-exact** pairs. Both means remain **93.87% source-shape /
83.06% strict fuzzy**. Both have zero skipped units, zero wrong-type missing
signatures, 201 absent original functions and 125 extra symbols. Comparing
the actual `(unit, symbol)` exact-pair sets gives **zero losses and zero gains**
for both linked-exact and byte-exact. The after run includes
`--fail-on-wrong-type` and succeeds.

SHA-256 comparison of all 208 before/after objects finds only
`engine/render/ResourceMaterial.o` changed; all 207 other objects are identical.
The independent before/after build directories keep objects and verifier
target writes separate. Focused reports use separate target directories too.
Local reports (ignored, not committed):
`cmake-build-portraits-after/reports/portraits_before_full.json`,
`portraits_after_full.json`, `portraits_before_focused.json`, and
`portraits_after_focused.json` in the same directory.

## Limits

No native game executable, ARM32 runtime, GPU texture upload, image decode,
or complete `BuildResourceList`/`Level::init` execution is claimed. The host
gate verifies registration and path construction, not rendered portraits.
Source-backed bindings and file presence are independent of code-shape metrics.
