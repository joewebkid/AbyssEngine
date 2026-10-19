# BuildResourceList ordinary iPad tail — Android ARM, 2026-10-19

Scope: branch `codex/sol-ipad-tail-20261019`, base
`009d6227499e94b1c17ffa10656b7be7912bfe57`. Only the ordinary iPad
continuation after nine common non-Retina textures is restored. This
is a native recovery package; no Godot change or runtime claim.

Inspected: `BuildResourceList.cpp`, its Globals/Engine/PaintCanvas
interfaces and call context, production resource constructors,
`run_full_resource_table_smoke.py`, its C++ fixture, verifier/ELF tools,
previous resource evidence and validation notes, and original Android
2.0.16 ARM ELF and OBB. No original binaries/assets are committed.

## Flag and control-flow evidence

`BuildResourceList` is ARM Thumb `0x0e8f54–0x112c88`.
At `0x10a25a`, the literal at `0x10a270` is loaded; ADD-PC at
`0x10a25c` resolves `0x21038c`. Its ELF `.rel.dyn` relocation is
`R_ARM_GLOB_DAT` (21), symbol `_ZN7Globals6iPadHDE`. The load at
`0x10a25e` stores the resolved **address of Globals::iPadHD in r8**;
`0x10a262` reads its byte. This register survives the relevant calls
as a callee-saved register. The later `0x10be22` reads that same flag,
not a new device flag inferred from filenames.

The ordinary route is:

1. `iPadHD == 0` goes via `0x10a268 → 0x10a6c4`.
2. `retinaDisplay == 0` enters `0x10ee40`, registering nine common textures.
3. `0x10effa–0x10f004` reads `iPad` via GOT `0x21032c`, relocation
   `_ZN7Globals4iPadE`. Nonzero branches to `0x10a4dc`.
4. `0x10a4dc–0x10a4e6` reads Retina via GOT `0x2103e4`, relocation
   `_ZN7Globals13retinaDisplayE`. Zero branches to `0x10be20`.
5. `0x10be22–0x10be2e` reads iPadHD through r8 and compares with zero;
   zero branches to `0x10f160`.

Thus the new eight-record continuation requires `iPadHD == 0`,
`retinaDisplay == 0`, `iPad != 0`. ARM uses zero/nonzero byte tests;
the host fixture also covers values 2 and 255. No guessed flag is added.

## Ordered ordinary iPad records

All records: kind 2, unused -1, ResourceTexture value 0.0f. Paths below
are relative to `data/textures/`. Literal resolves as `ADD address + 4
+ uint32(literal)` in this Thumb code.

| Order | ID | Filename | ADD-PC | Literal | ID instruction |
|---|---:|---|---|---|---|
| 1 | 10089 | gof2_interface3_ipad.aei | 0x10f16a | 0x10f560 | 0x10f172 |
| 2 | 10063 | gof2_interface2_ipad.aei | 0x10f19c | 0x10f564 | 0x10f1a4 |
| 3 | 10064 | gof2_items_ipad.aei | 0x10f1ce | 0x10f568 | 0x10f1d6 |
| 4 | 11632 | gof2_items_ipad_2.aei | 0x10f200 | 0x10f56c | 0x10f208 |
| 5 | 11630 | gof2_dlc_interface_ipad.aei | 0x10f232 | 0x10f570 | 0x10f23a |
| 6 | 27341 | gof2_challenge_interface_ipad.aei | 0x10f264 | 0x10f574 | 0x10f26c |
| 7 | 27342 | gof2_campaign_select_ipad.aei | 0x10f296 | 0x10f578 | 0x10f29e |
| 8 | 27343 | gof2_shop_button.aei | 0x10f2c8 | 0x10f57c | 0x10a664 |

Eight constructor calls target PLT `0x65e68`; seven registrations target
`0x65ea4` before the final shared join. `0x10f2d0 → 0x10a662` sets
kind 2 / ID 27343, then `0x10a668 → 0x10bf32` stores and registers
the eighth record at `0x10bf42`. The last shop filename has **no iPad
suffix**. This distinction follows the literal, not a naming heuristic.

## Confirmed evidence outside this implementation

For iPadHD nonzero and Retina zero, `0x10be2e` falls through instead.
The following five entries are source-backed but their integration into
all HD and subsequent shared paths is outside the ordinary-iPad task:

| ID | Filename under data/textures/ | ADD-PC | Literal |
|---:|---|---|---|
| 10089 | gof2_interface3_1440.aei | 0x10be3c | 0x10be48 |
| 10063 | gof2_interface2_ipad_1440.aei | 0x10be76 | 0x10be80 |
| 10064 | gof2_items_ipad_1440.aei | 0x10beae | 0x10beb8 |
| 11632 | gof2_items_ipad_2_1440.aei | 0x10bee6 | 0x10bef0 |
| 11630 | gof2_dlc_interface_ipad_1440.aei | 0x10bf1e | 0x10bf28 |

The initial GL setup, HD false-path additions, and post-platform records
from `0x10bf46` are not covered by this bounded execution gate. Existing
unconditional later records/placeholders remain unresolved, including
potential duplicate IDs; full BuildResourceList execution is not safe
to infer from this gate. Next priority: recover the shared post-platform
record stream and integrate HD continuations with their correct joins.

## Asset evidence and execution limits

Original ELF SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original OBB: `main.47947006.net.fishlabs.gof2hdallandroid2012.obb`,
archive prefix `47947_GOF2CONTENT_ETC/assets/`. All eight new paths and
all five HD evidence paths above are present (13/13). The optional
`--obb` gate checks every unique path in the bounded platform selection:
58/58 present. It reads archive metadata, without extracting assets.
Presence does not prove texture decoding, filtering or rendering.

`run_full_resource_table_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
--obb <original.obb>` passes at -O0 and -O2. It executes production
construction of 2,488 table entries, production bulk registration,
and the source-extracted platform branch. The new branch yields 17
records (nine common + eight iPad). Non-iPad yields 15. Existing bounded
HD/Retina cases are preserved. All 64 combinations of flags taking
values {0,1,2,255} pass counts; every ordinary/iPad ID and path is checked
in registration order. These counts deliberately stop at the platform
branch and do not describe the complete original HD tail.

`test_build_resource_ipad_tail_arm.py` verifies original ELF relocation
symbols, branch operands, PC-relative strings, IDs, eight constructor
calls, seven direct registrations and the final shared ID/join. It also
cross-checks the ordered eight production records. This is independent
binary evidence, not only a test mirroring the added source.

Native gof2 builds; selected original-asset Level skybox/canvas/mesh
regression passes at -O0/-O2. Full verifier unit suite: 49 tests,
48 pass and one pre-existing skip. No game executable or visual/runtime
station-to-flight test was run.

## ARM validation

Fresh base build at 009d6227: 208/208 translation units, zero failed.
Before full corpus: 4,731 comparisons, 4,323/4,524 unique original
functions, zero skipped units, zero wrong signatures; mean source-shape
93.87%, mean strict fuzzy 83.05%, 2,529 linked-exact and 1,076 byte-exact.
Before BuildResourceList: source-shape 75.4%, strict fuzzy 21.1%,
60,817 target / 48,611 base instructions, nonexact.

The requested >=98% source-shape is not claimed for this huge function.
This change recovers the bounded ordinary-iPad branch; it does not invent
remaining resources or alter unrelated code to improve a corpus metric.

After focused BuildResourceList: source-shape **75.6%**, strict fuzzy
**21.5%**, nonexact (before **75.4% / 21.1%**). The focused command is
`python tools/verify/verify.py --build-dir _work/ipad-arm --no-build
--unit engine/render/BuildResourceList --only '^_Z17BuildResourceList'
--report _work/ipad-after-focused.json`. Both before and after ARM
builds compile 208/208 translation units with zero failures.

After full corpus: **4,731 comparisons**, **4,323/4,524** unique original
functions, **93.87% source-shape / 83.06% strict fuzzy**, **2,529
linked-exact**, **1,076 byte-exact**, zero skipped units, zero wrong
signatures (`--fail-on-wrong-type` passes). Before means were
93.87% / 83.05%. BuildResourceList has 48,721 base instructions after
this change, against 60,817 original instructions.

Exact sets were compared as `(unit with normalized slashes, symbol)`:
**0 linked-exact losses, 0 gains; 0 byte-exact losses, 0 gains** against
the fresh base report. Comparison with the supplied integrated reference
`_work/decomp_combined_full_20261019.json` likewise finds zero losses in
both exact-pair sets. Counts alone were not used as preservation proof.

Raw before/focused/after reports, exact-pair summary and gate logs are
saved locally under ignored `cmake-build-ipad-validation-reports/`.
Native build outputs were cleaned using the build system (209 files).
Automatic approval review rejected both recursive cleanup and a narrower
file-only cleanup with only "blocked by policy" as the reason. ARM output
and scratch files remain under `_work/`, outside this commit. No asset or original
binary is added to Git. Only this note and the three production/fixture
files plus the focused binary-evidence test belong to this package.
