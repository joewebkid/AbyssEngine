# BuildResourceList HD and post-platform tail — Android ARM, 2026-10-19

Scope: `codex/sol-ipad-hd-tail-20261019`, base
`51bddeaa74b7264d80e59ee8cef78a54d4d5a558`, dedicated checkout
`A:/Galaxy on Fire new version/_work/sol-ipad-hd-tail-20261019`.
This is native reconstruction. No Godot, Level, LensFlare, PaintCanvas,
PROJECT_STATUS or VALIDATION changes belong to this package.

Inspected: AGENTS, README, CONTRIBUTING, PROJECT_STATUS, VALIDATION,
BUILD_RESOURCE_IPAD_TAIL_ARM_2026-10-19, BuildResourceList and its resource
constructors/Globals/Engine/PaintCanvas interfaces, the full-table host gate,
verifier/disassembler/ELF tools, and the original Android 2.0.16 ELF/OBB.
Existing notes were leads; the instruction stream and relocations were re-read.

## Evidence and corrected flow

Original ELF SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BuildResourceList` is Thumb `0x0e8f54–0x112c88`.
The bounded tail starts at `0x10a25a`, following table registration and
152 initial Image registrations, and ends at `0x10edbe`. Its out-of-line
ordinary texture blocks extend through `0x10f2d0`; those are included.

Four ELF R_ARM_GLOB_DAT relocations identify the tested byte flags:

| GOT | Symbol |
|---|---|
| 0x21038c | Globals::iPadHD |
| 0x21032c | Globals::iPad |
| 0x2103e4 | Globals::retinaDisplay |
| 0x2103f0 | Globals::iPadLarge |

The last is a correction to the earlier bounded gate: ADD-PC `0x10a896`
plus literal `0x10aa14` resolves GOT `0x2103f0`, whose symbol is
`_ZN7Globals9iPadLargeE`. After nine Retina textures,
`0x10a89e` branches to `0x10a4ce` when **iPadLarge**, not iPad, is nonzero.
When zero, seven more phone textures are registered before the same iPad
check. Consequently iPad+Retina+iPadLarge=0 really registers phone records
and then the eight large iPad records. Those original duplicate IDs are
preserved; there is no guessed deduplication policy.

Every path passing `0x10a4ce` tests iPad. Zero enters `0x10aa34`, registers
texture 11630 (`gof2_dlc_interface.aei`) and then the non-iPad Image stream.
This includes ordinary/Retina phone routes after their shop-button join at
`0x10a4b4–0x10a4ca`. Nonzero iPad tests Retina at `0x10a4dc`: Retina
selects eight large iPad textures; otherwise `0x10be22` tests iPadHD through
r8, selecting five HD textures or eight ordinary iPad textures. All those
iPad paths register their last texture through `0x10bf32–0x10bf42`.

## Five HD records

All are kind 2, unused -1, ResourceTexture value 0.0f. Paths are relative
to `data/textures/`. Constructor PLT is `0x65e68`, AddResource `0x65ea4`.

| ID | Filename | ADD-PC | Literal | Registration |
|---:|---|---|---|---|
| 10089 | gof2_interface3_1440.aei | 0x10be3c | 0x10be48 | 0x10be60 |
| 10063 | gof2_interface2_ipad_1440.aei | 0x10be76 | 0x10be80 | 0x10be98 |
| 10064 | gof2_items_ipad_1440.aei | 0x10beae | 0x10beb8 | 0x10bed0 |
| 11632 | gof2_items_ipad_2_1440.aei | 0x10bee6 | 0x10bef0 | 0x10bf08 |
| 11630 | gof2_dlc_interface_ipad_1440.aei | 0x10bf1e | 0x10bf28 | 0x10bf42 |

These run with iPadHD!=0, iPad!=0, Retina==0, after the original twelve
HD textures. The first path deliberately has no `_ipad` suffix.

## Post-platform registration

The previous flat tail, including placeholders and unconditional HD
textures, is replaced with the original alternatives and shared continuation:

- iPad: 136 Image records, from the allocation at `0x10bf46` through the
  final registration at `0x10d8bc`. First: ID 3001, packed payload 0x00002d6e;
  second: ID 3101, payload 0x00012d6e. These are two uint16 words, not pointers.
- Non-iPad: 107 Image records from `0x10aa6e`; first ID 3000, payload
  0x00002d6e. The last record is prepared at `0x10bd9a–0x10bdb4`, then
  `0x10bdb6` jumps to the shared store/registration at `0x10d8b4–0x10d8bc`.
- Both: 27 small planet/ring textures, beginning at `0x10d8c0`, and 84
  Image records. Last AddResource is `0x10edba`, ID 9510, payload 0x00056acf.
- The subsequent helper calls remain `loadPortraits` at `0x10edc2` and
  `loadLowTexturesAndMaterials` at `0x10edc8`; their execution is outside
  this host gate.

The source contains 419 registration statements across all alternatives.
`REGISTER_RES` reuses the existing ADD_RES construction body to preserve
Resource-before-payload allocation; the two-word Image allocation is explicit.
It does not introduce a new runtime registration helper.

## Tests and limits

`test_build_resource_hd_tail_arm.py` supplies a bounded integer instruction
oracle. It follows original branches, skips literal pools, resolves PC-relative
strings and GOT relocations, models operator new/ResourceTexture/AddResource,
and reads actual stored IDs, kinds, -1 fields and payload words. Unknown
instructions/calls, uninitialized data, unexpected allocation sizes and
nonzero texture floats fail. Caller-saved registers are invalidated after
calls. A saved canary pointer is a sentinel; execution stops before the
canary/epilogue and helper calls. This is not an ARM runtime emulator or an
ABI/device test.

The host gate source-extracts production registration and constructors and
executes them with a recording canvas at -O0/-O2. With `--arm-evidence`,
it compares **every ordered record** against the ELF oracle for all 256
combinations of {0,1,2,255} across the four flags. It checks count, ID, kind,
unused field, texture name/value and both Image words. The original
2,488-slot table/bulk handoff is still exercised. The older ordinary-iPad
binary test remains green; only its source matcher accepts REGISTER_RES.

Canonical tail record counts (iPadLarge irrelevant where shown as either):

| iPadHD | iPad | Retina | iPadLarge | Count |
|---:|---:|---:|---|---:|
| 0 | 0 | 0 | either | 234 |
| 0 | 1 | 0 | either | 264 |
| 0 | 0 | 1 | 0 / 1 | 235 / 228 |
| 0 | 1 | 1 | 0 / 1 | 271 / 264 |
| 1 | 0 | either | either | 231 |
| 1 | 1 | 0 | either | 264 |
| 1 | 1 | 1 | either | 267 |

All **91/91 unique tail texture paths** are present in the original
`main.47947006.net.fishlabs.gof2hdallandroid2012.obb`, under
`47947_GOF2CONTENT_ETC/assets/`. Only ZIP metadata is read; nothing is
extracted or added to Git. Presence is not decoding/rendering evidence.

Reproduce with a configured GOF2_VERIFY_WORK and NDK r18b:

```powershell
$env:GOF2_VERIFY_LOCAL_NDK = '1'
python tools/run_full_resource_table_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe --arm-evidence --obb <original.obb>
python -m unittest discover -s tools/verify -p 'test_*.py'
```

Native gof2 passes with MSYS2 UCRT64 (Debug, -g0 to limit disk usage).
Both ARM builds compile **208/208** translation units, zero failed.
Verifier unit suite: **52 tests, 51 passed, one pre-existing skip**.
The full-table/tail host gate passes -O0/-O2 with original ELF/OBB evidence.

This does not execute the initial GL setup, the initial 152-Image loop or
the downstream portrait/low-texture helpers in one continuous game process.
The recording canvas does not validate production texture lookup/loading,
GPU upload, native ABI execution or rendering. A successful registration
oracle does not establish complete BuildResourceList or a playable game.
The >=98% source-shape aspiration is not reached for the whole function;
record-for-record tail agreement is not renamed as a source-shape score.

Next step: join the initial GL/152-Image setup and this tail to production
resource lookup/load consumers, keeping original duplicate registration
behavior, before claiming full initialization or a native runtime result.

## ARM comparison

Fresh base 51bddeaa: 4,731 comparisons, 4,323/4,524 unique original
functions; mean source-shape 93.90%, strict fuzzy 83.16%, 2,545
linked-exact and 1,079 byte-exact. Zero skipped units and zero wrong-type
signatures. The before BuildResourceList row is 75.6% source-shape,
21.5% strict fuzzy, 60,817 target / 48,721 base instructions, nonexact.

Focused after BuildResourceList is **76.9% source-shape / 18.8% strict
fuzzy**, 60,817 target / 51,334 base instructions, nonexact. The strict
fuzzy fall from 21.5% is material and recorded explicitly. Source-shape
is an allocation-insensitive structural score, not semantic correctness;
the stronger claim for this package is ordered record agreement in the
bounded tail. No >=98% source-shape claim is made, including for a
synthetically sliced fragment. The rest of the function still requires
recovery/code-generation work.

Commands:

```powershell
python tools/verify/verify.py --build-dir _work/hd-after-arm --no-build --unit engine/render/BuildResourceList --only '^_Z17BuildResourceList' --report _work/hd-after-focused.json
python tools/verify/verify.py --build-dir _work/hd-after-arm --no-build --fail-on-wrong-type --report _work/hd-after-full.json
```

Before and after objects are kept separately; SHA-256 comparison of all
208 ARM object pairs finds only BuildResourceList.o changed, with 207
other objects byte-identical. Native outputs use -g0 to save disk, and
the ELF/NDK/OBB are referenced in their existing locations without copies.
No cleanup prohibition was bypassed and no original assets are committed.

After full corpus: **4,731 comparisons, 4,323/4,524 unique originals,
93.90% mean source-shape / 83.16% mean strict fuzzy, 2,545 linked-exact,
1,079 byte-exact**, zero skipped units and zero wrong-type signatures.
Means round to the same values as the base; the focused strict decline
above must not be hidden by corpus averages. Extra symbols fall 125 to
124: the synthetic makeRes helper is no longer emitted out of line.

Exact sets were compared as **(unit with normalized slashes, symbol)**
against the fresh 51bddeaa report: **zero linked-exact losses/gains and
zero byte-exact losses/gains**. Counts alone were not used as proof.
Focused and full BuildResourceList rows agree.

Local reports/logs remain under this checkout's `_work/`; generated
objects, reports and scratch scripts are not staged. Report SHA-256:

| Report | SHA-256 |
|---|---|
| hd-before-full.json | 614f6dd4c335a720eb952318b3d95e5112ff9a12060a9e68575e0f15dba57c95 |
| hd-after-focused.json | 71b15545d31295f7e29199c0f189fec937b768142dd22cce814af04677266b91 |
| hd-after-full.json | 945dca1429d1221c6368100e75c955c2f37bbe6a7891a36677bb8810bed1a674 |

The pairwise audit is `_work/hd-exact-pair-summary.json`. The before full
report is byte-identical to the integrated base report recorded in
VALIDATION, independently confirming the requested baseline. No native
game executable or device/visual test was run. No push, merge or PR.
