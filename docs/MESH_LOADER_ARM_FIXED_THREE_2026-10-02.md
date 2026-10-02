# Mesh loader: fixed three-body ARM audit, 2026-10-02

Latest validated continuation: source **96.93% unweighted / 97.08% weighted**.
The result tables below preserve earlier measurements. Chronological continuation
sections record later per-body results, corpus evidence and rejected trials.

Status: **partial; the required two >=99.0% means are NOT achieved**.
There is no demonstrated objective compiler ceiling. This report supersedes
the unconditional completeness/ceiling claims in the 2026-09-24 loader report.

## Isolation and denominator

Base: `f60694dd0ca578831cec832e7026b32644642d97`. This was the requested clean
integrated starting point; newer visible tips were separate Gun, StarMap and
camera work, not a verified replacement integration baseline. Worktree:
`A:/gof2-mesh-loader-99-20261002`, branch `codex/mesh-loader-arm-99-20261002`.
The dirty main checkout and other worktrees were not edited.

Before source edits, the symbol table and clean objects fixed these three
unique original bodies. No clone constructors, PLT veneers, aliases, small
helpers or additional functions enter either mean.

| Original body | Android VA | Original instruction weight |
|---|---:|---:|
| `_ZN11AbyssEngine18MeshCreateFromFileEPNS_6EngineEPKcPPNS_4MeshEPNS_8MaterialE` | `0x6d074` | 291 |
| `_ZN11AbyssEngine12MeshReadDataEPNS_6EngineERKjjPPNS_4MeshEPNS_8MaterialE` | `0x6c608` | 873 |
| `_ZN11AbyssEngine4Mesh24ReadEnhancedDataFromFileEjj` | `0x6bbec` | 636 |

The weight is the verifier's fixed original `n_target`, total 1,800. Its
normalized instruction lists, including its established pool/table handling,
are used unchanged. Weighted mean = sum(score * n_target) / 1800.

## Evidence reviewed

Read the workspace AGENTS, CONTEXT_INDEX, PROJECT_MAP, CURRENT_STATUS,
TASK_QUEUE and reverse-engineering context pack; native AGENTS, README,
CONTRIBUTING, PROJECT_STATUS, VALIDATION, AEM_AEI_NATIVE_LOADER_SPEC,
MESH_LOADER_SOURCE_SHAPE_ARM_2026-09-24 and
MESH_READ_ENHANCED_SEMANTICS_ARM_2026-09-24. Inspected both target source
files, Mesh/Transform/Vector/Array/AEFile headers, Vector operation declarations,
loader callers, canonical match flags and verifier/delinker implementation.

Primary: Android ARMv7 **2.0.16** `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The three exact named bodies were disassembled directly, independently of
`analysis/gof2_libgof2hdaa_full_ida.c`. The decompiler is a control-flow lead,
not arithmetic authority: its enhanced timing output omits the conditional
comparison of current time, and its normal expressions obscure float division.

Cross-check: iOS **1.1.4**, verified from the app's Info.plist, executable hash
`74ece5bedf561eed4e71b8d0b875e201c47858a2649a59d65af57390a0554958`.
`analysis/gof2_ios_ida_full_dump.c` counterparts are `sub_31FB64` (enhanced),
`sub_3212E8` (read) and `sub_322EA8` (create). They corroborate positive-minimum
timing, type dispatch, channels and version signatures. iOS's `Transform`
allocation is `0x17c`, versus Android `0x180`; its decompiled expressions are
not used to override Android scalar precision or ABI.

PC cross-check: installed `D:/Program Files/Galaxy on Fire 2/GOF2.exe`, PE32 x86,
COFF timestamp `1345017951`, hash
`277c03f92d16a5841feea147fa239652ffbb9833afc3f8df493006ffc59b9371`.
No file/product version is advertised, so a numbered PC version is **unknown**;
the timestamp and hash identify the precise build instead. Independently
disassembled the magic-string consumer around `0x4faf40..0x4fb32e`, its reader
callee `0x4f9f90`, and enhanced callee `0x4f9090`. Their descriptive identities
are inferred from the call chain and contents, not recovered PC symbols.
Five signature addresses are `0x54e9f8/0x54ea00/0x54ea0c/0x54ea18/0x54ea24`.
PC corroborates masks 4/1/2/8/16, top-level multi-mesh handling, axis remap
and the positive-minimum time comparison (`0x4f924a..0x4f9273`). PC allocates
Mesh `0x84`, not Android `0x88`, and its reader tests enhanced mask `0x18`
at `0x4f9fcb/0x4fadcb`, versus Android `0x1a`; those differences are retained
as version differences. PC `ship_005_terran.aem` has header
`563441454d657368001f0100` and hash
`ca57352cc52f6084663587928e674319085106f47195942001f853262bbff353`.
This asset corroborates the V4 stream header, not Android behavior by itself.

## Accepted source changes

- `MeshCreateFromFile`: reload `(*out)->animation` after `CollectAnimationData`,
  matching the original final call's receiver read. No changes to ImageCreate,
  TextureCreate, constructor/header definitions or other engine bodies.
- `MeshReadData`: restore per-stream vertex-count reads around allocations and
  file calls, position-buffer reads in integer conversion, and the normal
  output pointer read after `sqrtf`. Refresh the captured tangent vertex count
  at its native point. The index allocation stores through the captured Mesh.
- Restore free Vector subtraction for the diagonal and free multiply/subtract
  for tangent orthogonalization, with direct initialization of returned Vectors.
  ARM calls `AEMath::operator-` at `0x6c8b0/0x6ce20` and `operator*` at `0x6ce12`;
  the old compounds and later assignments were different call operands/ABIs.
- Restore normal clamp ordering: lower bounds use converted double values;
  upper bounds compare the original float quotients. ARM explicitly shows
  this at `0x6cb2a..0x6cba8`. The previous consecutive clamps lowered to NEON
  vmax; the accepted source form restores the scalar conditional operations.
  Neither normals nor square-root computation is changed to double division.
- Integer position axis selection uses quotient/remainder arithmetic matching
  the original uidiv route. It is equivalent for the bounded unsigned index;
  it introduces no register binding, volatile object or invented branch.
- Enhanced loader uses ordinary signed-index switches for three-axis sections
  and a switch for V5 channels, rather than a precomputed channel array.
  Channel mapping and float rotation conversion remain unchanged. Extra-section
  counts are signed shorts; the final key-frame count check is signed as in ARM.
  The preceding minimum-positive-time and unknown-section corrections are kept.

These are source-backed operation/read-order repairs, not a claim that every
malformed file, ownership path or runtime effect has been recovered completely.

## Fixed-body results

| Body | Baseline source | Final source | Baseline strict fuzzy | Final strict fuzzy | Final compiled instructions | Linked / raw exact |
|---|---:|---:|---:|---:|---:|---|
| MeshCreateFromFile | 95.8% | 95.8% | 57.1% | 58.9% | 283 | no / no |
| MeshReadData | 91.8% | 96.2% | 38.2% | 46.3% | 859 | no / no |
| ReadEnhancedDataFromFile | 94.5% | 97.3% | 22.4% | 64.3% | 639 | no / no |
| Unweighted mean | 94.03% | **96.43%** | 39.23% | 56.50% | | |
| Original-instruction-weighted mean | 93.40% | **96.52%** | 35.67% | 54.70% | | |

Means are computed from the unchanged verifier's recorded per-function scores,
which are rounded to one decimal place; displayed hundredths are aggregate
arithmetic precision, not extra precision in the underlying measurement.

Final source-score components (inventory / opcodes / control / calls / size):
create `92.1 / 97.1 / 96.8 / 95.8 / 97.2`; read
`89.8 / 96.0 / 96.8 / 100.0 / 98.6`; enhanced
`95.1 / 96.4 / 95.3 / 100.0 / 99.8`. They show where the remaining deficit
is measured instead of attributing all of it to a compiler without proof.

## Trials and remaining differences

| Focused trial | Read source / strict | Enhanced source / strict | Decision |
|---|---|---|---|
| Clean baseline | 91.8 / 38.2 | 94.5 / 22.4 | fixed reference |
| Signed axes with modulo 3 | unchanged | 94.7 / 66.2 | rejected: three extra uidivmod calls |
| Shared buffers plus modulo switches | unchanged | 93.3 / 62.0 | rejected |
| Plain signed three-axis switches | unchanged | 95.7 / 23.5 | retained as part of final |
| V5 switch added | unchanged | 97.3 / 64.4 | retained |
| Explicit signed V5/count check | unchanged | 97.3 / 64.3 | retained for native types |
| Shared buffers with plain switches | unchanged | 96.0 / 24.3 | rejected |
| Per-stream count refresh | 94.3 / 38.8 | unchanged | retained |
| Asymmetric normal clamp/pointer refresh | 94.7 / 41.5 | unchanged | retained |
| Free Vector operations | 95.5 / 42.7 | unchanged | retained |
| Position reads/quotient and captured tangent count | 96.2 / 46.3 | unchanged | retained |
| Value-initialized Vector accumulator array | 96.2 / 44.5 | unchanged | rejected: no source gain, strict regression |

MeshCreate's two single-mesh call sites still merge into one in the compiled
source. Enhanced has remaining branch-form, frame/spill and typed-load differences.
ReadData still differs in memory operands, arithmetic operand order and branch
inventory. The source metric also distinguishes printed `[register]` from
`[register, #0]` in some instruction forms; the metric and its normalization
were **not modified** to suppress those differences. These observations do not
prove that further legitimate C++ forms cannot improve the result. No volatile,
dead branch, inline assembler, register binding or changed compiler flag was used.

## Validation and limitations

Canonical NDK r18b Clang 7.0.2, `-Oz`, Thumb ARMv7, NEON softfp, strong stack
protector, API 21, and the unchanged `tools/verify/match_flags.sh`. Initial MSYS
wrapper execution hit host memory/process overhead. Local uncommitted helper
`_work/run_local.py` reads the canonical one-argument-per-line flag output and
launches the existing NDK executables directly; it imports the repository's
unchanged verifier/asmdiff and calls the unchanged delinker with explicit nm/as
executables. This changes process transport only, not comparison rules or flags.
Final ARM build recompiles every source translation unit, not just target bodies.

Native UCRT64 `gof2` static-library build passes. `tools/mesh_loader_smoke.cpp`
compiles actual production Mesh.cpp with mocked file and Transform boundaries.
It passes axis/channel/remap, positive-minimum timing, unsupported types,
signed extra counts, range setup and every byte-prefix truncation of a V5
fixture (failure must destroy the temporary Transform). Clone-only mock
dependencies abort if unexpectedly exercised. This is an executable parser
test, not a native game executable or original ARM ABI/runtime test.

All 204 source-mapped final objects were also compared byte-for-byte with the
baseline objects: only AbyssEngine.o and Mesh.o differ; the other 202 are
identical. Both final changed objects equal the respective objects used for
the focused final measurements. Textual function-boundary assertions confirm
that replacing the three modified bodies with the baseline bodies reproduces
both entire baseline source files exactly.

Full corpus: baseline and final both compile **204/204** translation units and
compare **4,619** symbol rows, **4,274/4,524** unique/original functions, with
**zero skipped units**, **250** missing (7 wrong-type / 243 absent), **2,079**
linked-exact and **959** byte-exact pairs. Full strict average changes
75.97 -> 75.98%; full source average rounds to 88.99% in both. This is not
the fixed-three mean. The final corpus reproduces all three focused rows above.

Compared sets of `(normalized object path, exact mangled symbol)`, not only
their cardinalities: **linked lost [] / gained []; raw lost [] / gained []**.
Hashes of sorted `unit + TAB + symbol` lines joined with newline, no trailing
newline, are identical before/after:

- linked: `e042db4e31668f82a912c68c30d8093d736eefefc118bcdece4c598f0440bfaa`
- raw: `2e265c1384068f8255883ffad307bad8cf7a1209866b892557fd5c74fe78055e`

Local uncommitted evidence: `_work/baseline_full.json` (SHA-256
`ceb8ddf50e03dbadd2278413942c6e1a8e59b1f6d4060d7628de66593632a6d0`),
`_work/final_full.json` (SHA-256
`f01c639c49d90b804ff1b9823ba8a538fa615ea4257e9abc45190af119f07494`),
`_work/fixed_three_final.json`, focused trial JSONs, ARM/native build logs,
and direct Android/PC assembly excerpts. They are not included in the commit.

Reproduction on this host (PowerShell; set the MSYS UCRT64 runtime on PATH):

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
$env:GOF2_VERIFY_LOCAL_NDK = '1'
$env:GOF2_NDK_ROOT = 'A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b'
# Existing verifier; --no-build reuses the fully rebuilt final objects.
& 'C:\msys64\ucrt64\bin\python.exe' tools/verify/verify.py --no-build --build-dir _work/final-verify --report _work/recheck_full.json
# Actual production Mesh parser, executable boundary smoke.
& 'C:\msys64\ucrt64\bin\g++.exe' -std=c++14 -Isrc -Ithird_party/gl -ffunction-sections -fdata-sections tools/mesh_loader_smoke.cpp src/engine/render/Mesh.cpp '-Wl,--gc-sections' -o _work/mesh_loader_smoke_recheck.exe
& .\_work\mesh_loader_smoke_recheck.exe
```

To rebuild rather than reuse objects, use the canonical build_objs.sh with
the same environment, or the local direct-launch helper described above.
Keep baseline and final build directories separate, and run only one verifier
operation per translation unit/target directory at a time.

Remaining risks: parser reads accept any nonzero return, rather than independently
checking requested byte counts; enhanced returns -1 on failure but MeshReadData
tests zero, following the original nonzero-success branch; failed child reads
and existing release/ownership behavior are not comprehensively runtime-tested.
Float-to-integer conversion of malformed nonfinite/out-of-range times remains
a native-versus-portable-C++ boundary. No Android device/ARM executable was run.
The 99.0% criterion remains unmet; **partial is not readiness**.

## Continuation: component accumulator initialization

Three additional isolated source trials followed the first commit:

| Trial | Body | Source / strict | Decision |
|---|---|---|---|
| Unsigned axis and explicit original `0x7fffffff` mask | Enhanced | 97.3 / 64.3 | rejected: no gain |
| One shared unsigned count object across enhanced sections | Enhanced | 97.1 / 65.2 | rejected: source regression |
| Explicit per-vertex three-component zero initialization | ReadData | 96.4 / 49.9 | accepted: full corpus gate passed |

The accepted loop replaces the accumulator's single explicit memset. The
original decompiler's remainder-shaped memset size is evidence of a loop
lowering; this is a source-form hypothesis, not recovered original C++.
For the unsigned-short-derived vertex count the loop clears exactly the same
components. Canonical flags and matching rules remain unchanged. The trial's
ReadData components are inventory 90.6, opcodes 96.5, control 97.3, calls 98.2,
size 99.4; instruction count 868 versus original 873. Call-count matching
regresses from 100.0 to 98.2, while the requested source score improves.

Native static-library build and the existing executable parser smoke pass.
A fresh 204/204 ARM rebuild passes; 202 unaffected object files remain
byte-identical to the clean baseline. Full exact-set comparison passes: both
sets remain identical (linked 2,079; raw 959; lost/gained empty).
The earlier tables remain the record of the first commit, not measurements
of this continuation. The >=99.0% objective remains unmet.

Current fixed-three source means are **96.50% unweighted / 96.62% weighted**.
Full corpus: 4,619 comparisons, 4,274/4,524 unique/original functions, zero
skips; full strict 75.98%, source 88.99%. Set hashes remain the hashes recorded
above. Local evidence: _work/zero_full.json SHA-256
`8607d75aab7788048e1c2a85937c5edadabd268a1df0153efbf5239b5dd0c52c`;
_work/zero_three.json, zero_full_build.log, zero_full_verify.log,
zero_native.log and zero_smoke.exe. No ARM game runtime was executed.

## Continuation: normal component loop and enhanced type read

The original normal loop increments its source component index by three and
compares it against three times the vertex count. Restore that indexing and
write the destination as three float components. The length expression follows
the original ARM accumulation order, y-square then x-square then z-square;
canonical Clang produced the same focused object for both tested source orders.
The component-loop change improves ReadData from source 96.4 / strict 49.9
(868 instructions) to source 96.5 / strict 49.8 (869 instructions).

Enhanced's type local was initialized to -1 even though the original reads it
without a preceding store. Removing that added store raises source 97.3 ->
97.4 and strict 64.3 -> 64.4, with 637 compiled versus 636 original instructions.
An early-return rewrite of MeshCreateFromFile generated unchanged scores and
was reverted. No matcher, flags or denominator changes were made.

Native static-library build and the existing parser smoke pass. The smoke
covers enhanced reads and truncation cleanup; it does not execute MeshReadData's
normal conversion. Removing the type initializer retains the original risk for
any file backend that reports a nonzero short read without writing the entire
requested field. The production API's nonzero-success behavior is unchanged.
Full fresh ARM corpus validation passes in this continuation: 204/204 TUs,
4,619 comparisons, zero skips. Both exact sets equal the original baseline:
linked 2,079 / raw 959; lost [] and gained [] for each. All other 202 objects
are byte-identical to the baseline, and both focused target objects equal the
corresponding fully rebuilt objects.

Current source results: Create **95.8%**, Read **96.5%**, Enhanced **97.4%**.
Current fixed-three means: **96.57% unweighted / 96.70% weighted**.
Full strict/source means remain 75.98% / 88.99%. Local full evidence:
`_work/component_full.json` SHA-256
`9fe384f4fc0c963410fc0b6c18fceda10bdfb1cbbd10e905dbefc1e89182dfc2`;
`component_three.json`, component_full_build.log, component_full_verify.log,
component_native.log and component_smoke.exe. The >=99.0% criterion is unmet.

## Continuation: compressed UV destination read

Primary ARM `0x6c92c` loads the captured Mesh's texCoords inside the compressed
UV conversion loop. Restore this field read instead of retaining the destination
pointer before the loop. Source shape rises ReadData 96.5 -> 97.0; strict
49.8 -> 52.6; focused compiled instruction count is 879 (original 873).
Remove the now-unused cached pointer declaration. This is an original operand
read-order restoration, not a changed formula or a new call.

Additional rejected trials: flat MeshCreate failure flow (95.8 / 59.2; no source
gain), explicit preloads of all signature bytes (95.8 / 58.9; unchanged), earlier
bounds-minimum capture (Read 96.3 / 49.8), color destination reread (96.9 / 52.4),
color count capture (96.7 / 52.7), both color changes (96.6 / 51.5), and float-UV
flip pointer/count restoration (96.9 / 52.5). Read trials other than bounds
capture include the accepted compressed UV change. Rejected edits were restored.
No Image/Texture blocks, headers, helper bodies, matcher or flags changed.

Native static-library build passes. The existing parser smoke's Mesh.cpp source
is unchanged since the preceding successful run. It still does not execute
MeshReadData's UV loop. Fresh full ARM validation passes: 204/204 TUs, 4,619 comparisons, zero skips.
Both exact sets equal the clean baseline: linked 2,079 / raw 959; lost [] and
gained [] for each. The other 202 objects remain byte-identical to baseline.
The focused and fully rebuilt AbyssEngine objects are byte-identical.

Next-search constraint: holding Create at 95.8, even two perfect remaining
bodies yield only 98.60 unweighted. Create must improve (at least 97.0 if
both other bodies were perfect) for the requested two-mean gate. This is an
arithmetic bound on the current scores, not a compiler-ceiling claim.

Current fixed-three scores: Create 95.8 / Read 97.0 / Enhanced 97.4 source.
Means are **96.73% unweighted / 96.95% weighted**. Full strict/source remain
75.98% / 88.99%. Evidence: `_work/uv_full.json` SHA-256
`e782a10c04758934c206d532ae5ca6b4010de4430fe25d29e4d3bb9628dc8f60`;
uv_three.json, uv_full_build.log, uv_full_verify.log and uv_native.log.
The >=99.0% gate is not achieved. No push, PR or merge was performed.

## Continuation: MeshCreate local storage lifetimes

Move magic, subCount and childPtr declarations to the function's main scope.
Initialize the magic bytes with strcpy at the same successful-open point as
before. Assign each childPtr immediately after allocation, before use. The
original ARM/decompiler local stack layout motivates examining this scope;
the exact original C++ declaration order is not recovered. Complete magic
initialization, error exits, branch predicates, calls and ownership are unchanged.

| Scope trial | Create source / strict | Instructions |
|---|---|---:|
| Only magic plus strcpy | 95.6 / 59.6 | 283 |
| Only subCount | 96.0 / 57.8 | 280 |
| Only childPtr | 95.6 / 58.2 | 283 |
| Magic and subCount | 96.2 / 56.4 | 280 |
| Magic and childPtr | 95.6 / 55.4 | 283 |
| subCount and childPtr | 96.0 / 53.6 | 280 |
| All three, magic/count/child/handle declaration order | 96.2 / 61.6 | 280 |

Seven distinct all-three declaration orders were measured; all score either
96.0 or 96.2 source, with strict 57.4..61.6. Retain the 96.2 / 61.6 ordinary
C++ form. No register bindings, volatile objects, invented conditions, extra
calls or changed compiler settings were introduced. One single-mesh call is
still merged, so Create call-count match remains 95.8. Create component scores
are inventory 94.1, opcodes 97.3, control 97.9, calls 95.8, size 96.1. This is
measured source progress, not proof of an objective compiler ceiling.

Native static-library build passes. Mesh.cpp and the parser smoke are unchanged
since their preceding passing run. Full fresh ARM gate passes: 204/204 TUs, 4,619 comparisons, zero skips.
Both exact sets equal baseline: linked 2,079 / raw 959; lost [] and gained []
for each. Other 202 objects remain byte-identical to baseline. The focused
and fully rebuilt AbyssEngine objects are identical.

Current fixed-three source scores: **Create 96.2 / Read 97.0 / Enhanced 97.4**.
Means: **96.87% unweighted / 97.01% weighted**. Full strict/source averages
remain 75.98% / 88.99%. Evidence: `_work/scope_full.json` SHA-256
`12b4cdd9f5ae28554c9bae4272fc86c5266508915a5a606be13d125f9f744656`;
scope_three.json, scope_full_build.log, scope_full_verify.log, scope_native.log.
The >=99.0% objective remains unmet. No push, PR or merge was performed.

## Continuation: enhanced section type dispatch

Express the three position/scale/rotation type choices as ordinary switches
with scoped cases 1 and 0. Unknown types continue to the next section, as in
the previous source and primary ARM. Keep each case's count/key/vector objects
local; a shared key or vector buffer regresses measured shape. The two case
orders generate the same focused result. No extra condition or call is added.

Enhanced source 97.4 -> **97.6**, strict 64.4 -> **64.9**, compiled instructions
remain 637 versus original 636. Components: inventory 95.8, opcodes 97.1,
control 95.3, calls 100.0, size 99.8. This source representation is consistent
with the original type dispatch, not recovered original C++ syntax.

Rejected trials: scoped-locals plus flat Create failure flow (96.2 / 62.0;
no source improvement), removal of enhanced anim null guards (unchanged),
signed-char format (unchanged), common key lifetime (97.0 / 57.8), common
three-vector lifetime (97.1 / 36.3). All rejected changes were restored.

Native static-library build and the production parser smoke pass, including
unknown-type fallthrough and every byte-prefix truncation of the V5 fixture.
Full fresh ARM corpus validation passes: 204/204 TUs, 4,619 comparisons,
zero skips. Both exact sets equal the clean baseline: linked 2,079 / raw 959;
lost [] and gained [] for each. Other 202 objects remain byte-identical to
baseline. Mesh.o equals the focused object; AbyssEngine.o equals the preceding
validated revision.

Current scores: Create 96.2 / Read 97.0 / Enhanced 97.6 source. Means are
**96.93% unweighted / 97.08% weighted**. Full strict/source averages remain
75.98% / 88.99%. Evidence: `_work/dispatch_full.json` SHA-256
`be8b095d1fbb215d6d8e359790225580f932766e4d5eda457e4a84971a7365cc`;
dispatch_three.json, dispatch_full_build.log, dispatch_full_verify.log,
dispatch_native.log and dispatch_smoke.exe. The >=99.0% gate remains unmet.
No push, PR or merge was performed.

## Continuation: Create branch/return audit without accepted edits

Rechecked the current tracked state (8d60ca51) and measured eight additional
ordinary source forms with unchanged canonical ARM flags and matcher:

| Trial | Create source / strict | Instructions | Decision |
|---|---|---:|---|
| Inverted enhanced/non-enhanced order | 96.2 / 61.6 | 280 | unchanged |
| Single-part branch before multi-part branch | 95.0 / 57.8 | 280 | rejected |
| Both branch orders inverted | 95.0 / 57.8 | 280 | rejected |
| Cases 0/1/default for subCount | 95.0 / 57.8 | 280 | rejected |
| Count switch plus inverted outer branch | 95.0 / 57.8 | 280 | rejected |
| Separate new Transform from output assignment | 96.2 / 61.6 | 280 | unchanged |
| Common result variable/return label | 96.2 / 62.0 | 280 | no source gain |
| Separate engine/path guards | 96.2 / 61.6 | 280 | unchanged |

Every trial was restored to the validated source; git tracked status was clean
before this report addition. No accepted source change means the preceding full
ARM/native/parser evidence still applies; no unnecessary full rebuild was run.
Create still has 24 target versus 23 compiled calls, with its two original
single-mesh reader call sites folded into one compiled site. The new Transform
assignment already reads *out after construction, so no field-read repair is
needed there. Remaining allocation-normalized inventory differences include
parameter spills/reloads, zero-offset memory encoding and the input guard's
conditional form. These trials reject specific source hypotheses; they do not
prove that all legitimate C++ forms are exhausted or establish a compiler ceiling.

Current validated source means remain **96.93% unweighted / 97.08% weighted**;
Create 96.2 / Read 97.0 / Enhanced 97.6. No exact pair was lost by these restored
trials. The >=99.0% goal remains unmet and active.

## Continuation: signature storage and normal operation order

Six further Create signature forms are invariant under the canonical compiler:
unsigned loop index, `i != 7`, explicit unsigned byte, signed fmt, complete
string arrays, and string pointers. All produce source 96.2 / strict 61.6,
280 compiled instructions. Seven bytes remain the comparison denominator;
full string declarations do not add an eighth-byte comparison.

ReadData trials from the validated source:

| Trial | Source / strict | Compiled instructions |
|---|---|---:|
| Only float-UV flip destination reread | 97.0 / 52.3 | 880 |
| Normal raw index plus independent end-component pointer | 96.9 / 51.2 | 880 |
| Entire normal pipeline reordered Y/X/Z | 96.9 / 50.8 | 880 |
| Only normal load declarations Y before X | 96.9 / 50.5 | 880 |
| Only quotient declarations Y before X | 97.0 / 52.6 | 879 |
| Only lower clamp declarations Y before X | 97.0 / 52.6 | 879 |
| Only conversion declarations Y before X | 97.0 / 52.6 | 879 |
| Only upper clamps Y before X | 97.0 / 52.4 | 879 |
| Quotients and all clamps Y before X | 97.0 / 52.4 | 879 |
| Common Z write after the normal/zero branches | 97.0 / 52.6 | 879 |

The primary ARM independently confirms raw Y at `(raw+2)[component_index]`,
X at `(end_component_pointer-4)` and Z at the end pointer, with component index
increment 3 and pointer increment 6 bytes (`0x6ca8a..0x6caa6`, `0x6cab2..0x6caba`,
`0x6cbb6..0x6cbba`). The square sum is Y² + X² + Z², already used by the current
source. Quotient/lower/upper stages visit Y before X before Z, while output
stores remain X/Y/Z (`0x6cb2a..0x6cbac`). The final Z store is shared after the
branches. These observations do not justify accepting a worse measured form.

All sixteen trials were restored. Git tracked status was clean before this
report addition; no source/body or verifier change remains. The preceding full
native/ARM/parser validation and exact sets still apply; no repeat full build
was necessary. Current means remain **96.93% unweighted / 97.08% weighted**.
The >=99.0% gate is unmet. No objective compiler ceiling is demonstrated.

## Continuation: ReadData storage, Vector temporaries and format predicates

Eleven isolated source trials from the validated source produce no source-score
gain. Canonical flags, the matcher, the three bodies and their weights remain
unchanged. Every trial was restored before this report update.

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| One raw pointer reused across sequential streams | 97.0 / 52.6 | 879 |
| Destination allocation assigned directly to fields | 97.0 / 52.6 | 879 |
| Shared raw pointer and direct destination assignments | 97.0 / 52.6 | 879 |
| Final tangent inputs copied as Vector objects | 97.0 / 52.8 | 877 |
| Triangle tangent assembled as a Vector | 97.0 / 52.6 | 879 |
| Both tangent Vector forms | 97.0 / 52.8 | 877 |
| Bounds center/minimum scratch as Vector objects | 97.0 / 52.6 | 879 |
| Bounds Vector form plus final tangent Vector inputs | 97.0 / 52.8 | 877 |
| Compressed predicate expressed as flags & 7 | 97.0 / 52.6 | 879 |
| Integer-position predicate expressed as flags & 3 | 97.0 / 52.6 | 879 |
| Both masked predicates | 97.0 / 52.6 | 879 |

The bit predicates are equivalent for the unsigned flags argument; both its
left-shift tests and these masks depend on the same low bits. Allocation,
read/free order and failure exits were unchanged. Typed Vector inputs slightly
improve strict/size evidence but not the requested recorded source score;
therefore no implementation change was accepted solely for cleaner syntax.

The tracked source status was clean before adding this section. No source
change remains, so the prior full ARM/native/parser gate and its exact sets
remain authoritative; no repeat full build was necessary. Current means remain
**96.93% unweighted / 97.08% weighted**. The >=99.0% gate is unmet, and these
specific negative results do not establish an objective compiler ceiling.
