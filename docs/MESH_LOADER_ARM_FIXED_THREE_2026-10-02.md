# Mesh loader: fixed three-body ARM audit, 2026-10-02

Latest validated continuation: source **97.93% unweighted / 97.79% weighted**.
Latest semantic correction: original full bounds diagonal; earlier half-diagonal
radius assertions are superseded by the verified operand/ABI evidence below.
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

## Continuation: enhanced loop forms and positive attachment branch

Five isolated, ordinary C++ source trials used the unchanged canonical ARM
flags and matcher. Only the positive attachment branch improved source-shape
and was retained. The other four forms were restored.

| Trial | Enhanced source / strict | Compiled instructions |
|---|---|---:|
| Unsigned axis index with <= 2 | 97.6 / 64.9 | 637 |
| Signed axis index with <= 2 | 97.6 / 64.9 | 637 |
| Signed indices for unsigned-short key counts | 97.6 / 64.9 | 637 |
| Attach when signed key count >= 1, release otherwise | **97.8 / 64.9** | 637 |
| Explicit signed count == 0 or < 0 | 97.6 / 64.9 | 637 |

The retained `Mesh.cpp` change swaps the final if/else arms and inverts the
signed predicate. It preserves attachment, range setup and cleanup behavior,
including a negative signed count. Original Android instructions at
`0x6c18a..0x6c190` load the key count, compare against 1 and branch with signed
BLT to release. This supports the semantics, not a claim that the exact
original C++ was recovered. No timing formula or section/channel behavior
changed. No helper, flags or verifier changes enter the score.

Fresh validation:

- Native UCRT64 `gof2` static library build passed.
- Production `Mesh.cpp` parser smoke passed, including populated/empty key
  paths, unknown sections, signed V5 counts and every truncated V5 byte prefix.
- All 204 ARM translation units rebuilt in `_work/attach-full`.
- Full corpus: 4,619 comparisons; 4,274/4,524 original functions compared;
  zero skipped, 250 missing (7 wrong type, 243 absent), 130 extra symbols.
- Actual normalized `(unit, symbol)` sets equal the clean baseline:
  linked-exact 2,079 and byte-exact 959; lost **[]**, gained **[]** for both.
- 202 unaffected ARM objects are byte-identical to the clean baseline.
  `AbyssEngine.o` equals the previous validated continuation; `Mesh.o` equals
  the successful focused trial. Only those two objects differ from baseline.
- Full mean strict 75.98%, source 88.99%; full report SHA-256
  `7a82d5aea1a404e354419add0fc74025db662dcf538c33fffc5b115118268e86`.
  Evidence: `_work/attach_full.json`, `_work/attach_three.json`,
  `_work/attach_full_build.log`, `_work/attach_full_verify.log`,
  `_work/attach_native.log`, `_work/attach_smoke.exe`.

| Fixed unique body | Source-shape | Strict | Original instructions |
|---|---:|---:|---:|
| MeshCreateFromFile | 96.2% | 61.6% | 291 |
| MeshReadData | 97.0% | 52.6% | 873 |
| ReadEnhancedDataFromFile | 97.8% | 64.9% | 636 |

Exactly these three bodies give **97.00% unweighted / 97.153333% weighted**
(97.15% displayed), versus the preceding 96.93% / 97.08%. The >=99.0% gate
remains unmet. No objective compiler ceiling is demonstrated. This validates
the native library and bounded parser behavior; no native game executable or
Android device runtime test was performed, and source-shape is not ABI proof.

## Continuation: compressed color read through the captured mesh owner

The original Android color path loads mesh into r4 at `0x6ce74`, reads its
vertex count at `0x6ce82`, allocates raw bytes at `0x6ce86`, then reads the count
from that same r4 at `0x6ce8e` before AEFile::Read at `0x6ce96`. It does not
reload the mesh slot between these calls. The retained change uses the already
captured `m` for both raw color allocation/read sizes, rather than rereading
`*slot`. It still rereads the vertex-count field after allocation. No count
formula, conversion, destination allocation or cleanup changed. This is an
original memory-operand observation; the exact C++ spelling is not recovered.

Nineteen isolated trials used the same canonical flags and matcher:

| Trial | Body source / strict | Compiled instructions |
|---|---|---:|
| V5 channel defaults to 0x400, case 0 leaves it intact | Enhanced 97.8 / 64.9 | 637 |
| Unsigned V5 channel loop through 6 | Enhanced 97.8 / 64.9 | 637 |
| Explicit 0x7fffffff channel mask | Enhanced 97.8 / 64.9 | 637 |
| Wide unsigned format variable | Enhanced 97.8 / 64.9 | 637 |
| Direct flags predicates | Enhanced 97.8 / 64.9 | 637 |
| C++ delete expressions for cleanup | Enhanced 97.8 / 64.9 | 637 |
| Position loop terminates on unequal count | Read 97.0 / 52.6 | 879 |
| UV loop terminates on unequal count | Read 96.8 / 52.4 | 879 |
| Independent UV byte offset | Read 97.0 / 52.2 | 879 |
| UV source pair pointer | Read 97.0 / 52.6 | 879 |
| UV flip defaults to subtraction, then tests !flip | Read 97.0 / 52.6 | 879 |
| Cached shader flag as unsigned byte | Read 97.0 / 52.6 | 879 |
| Position loop uses remaining-component counter | Read 96.9 / 52.4 | 879 |
| Captured raw owner for six-byte streams | Read 96.5 / 53.0 | 881 |
| Captured raw owner for twelve-byte stream | Read 96.8 / 52.7 | 868 |
| Captured raw owner for both four-byte streams | Read 97.1 / 51.7 | 877 |
| Captured raw owner for all streams | Read 96.9 / 52.8 | 864 |
| Captured raw owner only for UV | Read 97.0 / 52.1 | 879 |
| Captured raw owner only for colors | **Read 97.1 / 52.2** | 877 |

Only the color owner change was retained. Removing its redundant `m = *slot`
assignment produced an identical focused ARM object. Strict matching regresses
from 52.6 to 52.2 while requested source-shape improves from 97.0 to 97.1.
Its components are inventory 91.3, opcodes 96.5, control 98.1, calls 100.0,
size 99.4. Rejected source changes were restored; no verifier edits remain.

Fresh gates:

- Native UCRT64 `gof2` static library build passed; the existing enhanced parser
  smoke passed against the unchanged Mesh.cpp version.
- Added `tools/mesh_read_color_smoke.cpp` and its build runner
  `tools/run_mesh_read_color_smoke.py`. It tests two integer vertices and eight
  compressed RGBA bytes including 0/255, every shorter byte prefix, and the
  zero-byte-read failure contract. It verifies converted positions/colors;
  after failed color reads the test caller frees the existing position buffer,
  as the reader does not roll back its earlier successful allocations.
- Whole AbyssEngine.cpp smoke linkage was unsuccessful because the Windows
  linker retained unrelated dependencies. The runner therefore compiles the
  current reader definition extracted verbatim, with mocked file/math/engine
  boundaries; other enhanced/tangent/child paths abort if called. It does not
  test whole-TU linkage, allocation-induced slot mutation or native ABI. The
  production translation unit is separately covered by the library/ARM builds.
- All 204 ARM translation units rebuilt in `_work/colorowner-full`; 4,619
  comparisons, zero skips, 4,274/4,524 originals compared, missing 250 (7 wrong
  type, 243 absent), extra 130. Full strict/source means remain 75.98/88.99.
- Actual `(unit, symbol)` sets equal baseline: linked-exact 2,079, byte-exact
  959, lost **[]** and gained **[]** for each. The other 202 objects are
  byte-identical to baseline; new Core equals the focused final object and Mesh
  equals the preceding validated full object.
- Full report SHA-256:
  `89eae74efd7e00aa8bbbcfb6776cf676db9d779f204ed3251f41ae5d561ea203`.
  Evidence: `_work/colorowner_full.json`, `_work/colorowner_three.json`,
  `_work/colorowner_full_build.log`, `_work/colorowner_full_verify.log`,
  `_work/colorowner_native.log`, `_work/colorowner-read-smoke/`.

Fixed three source scores: Create **96.2**, Read **97.1**, Enhanced **97.8**.
Weights stay 291/873/636, total 1,800. Means are **97.033333% unweighted /
97.201833% weighted** (97.03/97.20 displayed). The >=99.0% goal remains unmet.
No objective compiler ceiling or native game/device runtime readiness is proven.

## Continuation: format width and explicit bounds initialization

Thirteen focused trials from the validated color-owner version used unchanged
canonical flags and matching rules. Only explicit bounds initialization was
retained; all other source forms were restored.

| Trial | Body source / strict | Compiled instructions |
|---|---|---:|
| Create unsigned byte format | Create 94.7 / 69.4 | 280 |
| Create unsigned short format | Create 94.7 / 69.4 | 280 |
| Create signed byte format | Create 95.5 / 57.1 | 280 |
| Prefetch V2/V3 signature characters | Create 96.2 / 61.6 | 280 |
| Signature format updates as conditional expressions | Create 96.2 / 61.6 | 280 |
| Postincrement signature index | Create 96.2 / 61.6 | 280 |
| Captured raw owner only for short positions | Read 96.5 / 52.6 | 879 |
| Captured raw owner only for normals | Read 97.1 / 52.3 | 876 |
| Reverse min/max declarations | Read 97.1 / 52.2 | 877 |
| Unsigned short vertex counter | Read 97.1 / 52.2 | 877 |
| Unsigned byte enhanced-mask local | Read 97.1 / 52.2 | 877 |
| Explicit chained assignments of min/max components | **Read 97.3 / 51.9** | 871 |
| Minimum copied before center calculation | Read 96.9 / 52.2 | 879 |

The accepted source replaces two three-element array initializers with two
chained scalar assignments after their declarations. Every minimum is still
10000000.0f and every maximum is still -10000000.0f before any position use.
Original Android `0x6c69c..0x6c6ac` loads and stores these constants into the
six scratch components. The C++ initialization form is an equivalent source
hypothesis, not an exact recovered spelling. No math, stream sizes, comparison
order, error paths, flags or helper/matcher rules changed.

Read source improves 97.1 -> 97.3 while strict falls 52.2 -> 51.9. Components:
inventory 92.0, opcodes 96.7, control 98.1, calls 100.0, size 99.8. Compiled
instructions 871 versus original 873. The strict regression is not hidden.

Native UCRT64 `gof2` library build passed, and both parser/body smoke tests
passed. The reader smoke now also checks center (2,1,6) and radius sqrt(24)
for its known two vertices. It still compiles a verbatim extracted reader
body with bounded mocks, not the whole AbyssEngine.cpp TU or native ABI.

Fresh full ARM validation rebuilt all 204 translation units in
`_work/boundsinit-full`. Full corpus: 4,619 comparisons, zero skipped,
4,274/4,524 originals compared, missing 250 (7 wrong type, 243 absent), extra
130. Actual `(unit, symbol)` sets equal the clean baseline: linked-exact 2,079
and byte-exact 959, lost [] and gained [] for each. The other 202 ARM objects
are byte-identical to baseline; Core equals the focused successful variant,
and Mesh equals the preceding validated full object.

Full strict/source means: 75.98/88.99. Report SHA-256: `7faa4604f268b2f2e462792c5ae69cfd163011ada54437b7a24b7ff0abf5f625`.
Evidence: `_work/boundsinit_full.json`, `_work/boundsinit_three.json`,
`_work/boundsinit_full_build.log`, `_work/boundsinit_full_verify.log`,
`_work/boundsinit_native.log`, `_work/boundsinit-read-smoke/`.

The fixed three source scores are Create 96.2, Read 97.3, Enhanced 97.8;
original weights stay 291/873/636, total 1,800. Means: **97.100000% unweighted / 97.298833% weighted** (97.10/97.30 displayed).
The >=99.0% goal remains unmet. No compiler ceiling is proven, and no native
game executable/Android device runtime test was performed.

## Continuation: bounds storage, UV scheduling and timing predicates

Fifteen isolated source trials start from `64fe4558`. No source-score gain was
found; all production source changes were restored. Canonical flags, helper
definitions and the verifier remain unchanged.

| Trial | Body source / strict | Compiled instructions |
|---|---|---:|
| Initialize maximum before minimum | Read 97.3 / 51.9 | 871 |
| Separate ascending component assignments | Read 97.3 / 51.9 | 871 |
| Paired min/max component assignments | Read 97.3 / 51.9 | 871 |
| Three-axis initialization loop | Read 96.6 / 47.8 | 862 |
| UV division by 4096 instead of reciprocal multiplication | Read 97.3 / 51.9 | 871 |
| Preload both raw UV components | Read 97.2 / 51.1 | 870 |
| Load UV destination after computing both components | Read 97.2 / 51.1 | 870 |
| One six-component array, minimum first | Read 96.5 / 48.4 | 868 |
| One six-component array, maximum first | Read 96.6 / 49.1 | 868 |
| Local bounds struct, minimum first | Read 96.6 / 48.4 | 868 |
| Local bounds struct, maximum first | Read 96.6 / 49.1 | 868 |
| Inline the positive minimum update with nested tests | Enhanced 97.8 / 64.9 | 637 |
| Inline the update with a conjunction | Enhanced 97.8 / 64.9 | 637 |
| Compare the global minimum before checking positive time | Enhanced 96.2 / 61.1 | 637 |
| Reverse operands of both timing comparisons | Enhanced 94.2 / 59.0 | 645 |

The six-component array/struct trials followed the adjacent original stack
storage at `0x6c6a0..0x6c6ac`; they do not establish that the original C++ used
such an object. Their regressions rule out retaining these particular layouts.
The current separate arrays and chained scalar initialization remain better.
UV scheduling similarly produces no source gain, and replacing the timing
helper's calls with the same nested predicate produces no improvement. Timing
comparison reorderings were rejected; no altered floating-point comparison
ordering remains in the implementation.

Tracked status was clean after the sweeps and before this report addition.
The production sources are unchanged from the validated commit, so its fresh
204-TU ARM corpus, native library and bounded reader/parser tests remain the
applicable evidence; no repeat full build was required for this report-only
change. Means remain **97.10% unweighted / 97.30% weighted**, exact sets remain
2,079 linked / 959 raw with no losses. The >=99.0% goal remains unmet. These
negative trials do not demonstrate an objective compiler ceiling.

## Continuation: tangent Vector copies and loop induction variables

Eleven isolated MeshReadData trials from `06dd8dc2` produce no improvement in
the requested recorded source score. Every source trial was restored, with
tracked status clean before this report-only addition. Flags, matcher and
fixed-body denominator are unchanged.

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| memcpy of normalized tangent output | 97.2 / 51.8 | 869 |
| memcpy of binormal output | 97.2 / 52.1 | 869 |
| memcpy of both outputs | 97.1 / 52.0 | 867 |
| memcpy of the normal/accumulator inputs | 97.2 / 52.4 | 870 |
| memcpy of inputs and both outputs | 97.0 / 53.4 | 866 |
| Remaining-vertex counter in final tangent loop | 96.6 / 53.5 | 858 |
| Remaining-vertex counter in zeroing loop | 96.7 / 52.7 | 858 |
| Remaining-vertex counters in both loops | 97.3 / 51.9 | 872 |
| Remaining-triangle counter | 96.8 / 50.5 | 869 |
| Independent triangle index offset | 97.3 / 51.9 | 871 |
| Independent final vertex component offset | 97.3 / 51.9 | 871 |

Original Android copies the tangent result with word loads/stores at
`0x6ce2e..0x6ce40`; this motivated testing small memcpy forms, not asserting
the original source called memcpy. It decrements the final vertex counter at
`0x6ce54` and tests it at `0x6ce66`, motivating the remaining-counter trials.
Matching these particular source forms does not improve recorded source-shape;
their strict-score gains alone do not meet the requested objective. The
existing component stores and vertex loops are retained.

No implementation or test changed, so the preceding 204-TU ARM corpus/native
library/parser/body validation remains applicable. Fixed scores stay Create
96.2 / Read 97.3 / Enhanced 97.8; means **97.10% unweighted / 97.30% weighted**.
Exact pair sets stay 2,079 linked / 959 raw with no losses. The >=99.0% gate
remains unmet; these trials do not demonstrate a compiler ceiling.

## Continuation: triangle tangent numerator scheduling

Five isolated trials from `5453d9a4` used unchanged canonical flags and matcher:

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| Preload the origin vertex components | 97.3 / 52.3 | 871 |
| Compute all three numerators before products | **97.4 / 55.4** | 871 |
| Preload origin and compute numerators first | 97.4 / 55.4 | 871 |
| Express odd UV index with bitwise OR | 97.3 / 51.9 | 871 |
| Explicit origin UV pointer | 97.2 / 51.2 | 869 |

Only the numerator scheduling change is retained. The origin-preload variant
produces a byte-identical focused object, so its extra locals were not retained.
It uses named Y/X/Z float
numerators, then multiplies each by the existing reciprocal. All formulas,
operand order within each expression, stores, allocations and failure paths
are preserved. Original Android `0x6cd78..0x6cd8c` computes the three
numerators before the three products at `0x6cd90..0x6cd98`, followed by the
Y/X/Z scratch stores at `0x6cd9c..0x6cda4`. This is direct instruction-order
evidence, not a claim to have recovered exact C++ local names or spelling.
Read source rises 97.3 -> 97.4 and strict 51.9 -> 55.4. Components:
inventory 92.5, opcodes 96.8, control 98.1, calls 100.0, size 99.8.

The bounded reader test now exercises three indexed triangles: a unit XY
triangle, mixed UV/vertex coordinates giving tangent (1,2,0)/sqrt(5), and an
XZ-plane triangle giving tangent (1,0,2)/sqrt(5). It checks all vertex tangent
and binormal components after compressed UV/normal parsing, accumulation,
orthogonalization, normalization and cross product. Its math mocks are replaced
with seven functions extracted verbatim from production AEMath.cpp plus the
compiled production Vector.cpp. The reader definition is still extracted
verbatim; AEFile and unexercised enhanced/child boundaries remain mocked.
This validates bounded body behavior, not whole-TU executable linkage or ABI.
Earlier color/bounds/truncated-prefix checks continue to pass.

Fresh native UCRT64 `gof2` library build, enhanced parser smoke and expanded
reader/vector-math smoke passed. All 204 ARM translation units rebuilt in
`_work/trinumer-full`. Full corpus: 4,619 comparisons, zero skipped,
4,274/4,524 originals compared, missing 250 (7 wrong type, 243 absent), extra
130. Actual `(unit, symbol)` sets equal baseline: linked-exact 2,079,
byte-exact 959, lost [] and gained [] for each. The other 202 objects remain
byte-identical to baseline; Core equals the focused successful variant and
Mesh equals the preceding validated full object.

Full strict/source means: 75.98/88.99. SHA-256: `a5e8fbb01d39a4ffce14b3017b70ad84484bac1e2414f20ab5e77ea332f4b9d7`.
Evidence: `_work/trinumer_full.json`, `_work/trinumer_three.json`,
`_work/trinumer_full_build.log`, `_work/trinumer_full_verify.log`,
`_work/trinumer_native.log`, `_work/trinumer-read-smoke/`.

Fixed source scores: Create 96.2, Read 97.4, Enhanced 97.8, unchanged weights
291/873/636, total 1,800. Means: **97.133333% unweighted / 97.347333% weighted** (97.13/97.35 displayed). The >=99.0% gate remains unmet.
No objective compiler ceiling or native game/device runtime readiness is proven.

## Continuation: UV denominator, flag branches and normal-order interaction

Fourteen isolated trials from `e8339586` produce no source-score gain. The last
five specifically recheck normal-order interactions after the accepted triangle
numerator scheduling, rather than relying on their older measurements. All
production source changes were restored; flags and verifier remain unchanged.

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| Load origin U after the V differences | 97.4 / 55.4 | 871 |
| Named U differences before denominator | 97.4 / 55.4 | 871 |
| Both preceding UV changes | 97.4 / 55.4 | 871 |
| Reciprocal after numerator calculations | 97.4 / 55.4 | 871 |
| Named denominator products before subtraction | 97.4 / 55.4 | 871 |
| Signed high-bit test of compressedData for positions | 97.1 / 55.5 | 869 |
| Unsigned >= 0x80000000 test for positions | 97.1 / 55.5 | 869 |
| Right-shift compressedData high bit for positions | 97.4 / 55.4 | 871 |
| Signed compressedData local and position predicate | 97.1 / 55.5 | 869 |
| Normal Y loads and quotients before X | 97.4 / 53.3 | 872 |
| All normal Y stages before X | 97.4 / 53.5 | 872 |
| Only normal Y quotient before X | 97.4 / 55.4 | 871 |
| Only upper Y clamp before X | 97.4 / 55.2 | 871 |
| Normal Y quotient/clamp stages before X | 97.4 / 55.2 | 871 |

The high-bit tests followed original `0x6c6a8..0x6c6ba`, which shifts flags by
29 and branches on the sign bit; unsigned variants avoid a signed-conversion
assumption. None improves the recorded source score. Existing format tests
remain. Rechecking the source-backed Y-before-X ordering under the new tangent
scheduling also gives no gain, so the current normal code remains.

Tracked status was clean after every sweep completed and before adding this
section. No code/test change remains; the previous fresh 204-TU ARM/native
library and reader/parser gates remain applicable, with exact pair sets 2,079
linked / 959 raw and no losses. Means remain **97.13% unweighted / 97.35%
weighted**. The >=99.0% goal is unmet; no objective compiler ceiling is proven.

## Continuation: Create allocation expression and boolean input guard

Eight isolated Create trials from `5256c600` provide no source-score gain.
All implementation changes were restored; canonical flags, body boundaries
and matching rules stay unchanged.

| Trial | Create source / strict | Compiled instructions |
|---|---|---:|
| Separate top-level raw allocation and placement construction | 96.2 / 61.6 | 280 |
| Separate child raw allocation and placement construction | 96.2 / 60.6 | 280 |
| Both allocation/construction expressions separated | 96.2 / 60.6 | 280 |
| Top-level void storage followed by placement construction | 96.2 / 61.6 | 280 |
| Explicit invalid-pointer boolean chain | 96.2 / 62.0 | 280 |
| Explicit valid-pointer boolean chain | 96.2 / 62.0 | 280 |
| Bitwise AND of the two boolean pointer comparisons | 96.2 / 61.6 | 280 |
| Ternary input guard | 96.2 / 61.6 | 280 |

The allocation trials keep the existing Mesh constructor and sizeof(Mesh);
they add no manual copied field initialization. Original operator-new then
initialization instructions motivate this source-form test, but do not imply
the original source used placement new. The boolean chains follow the original
conditional pointer test: the decompiler records the engine-null predicate and
updates it with the path-null predicate only when engine is present. They
slightly improve strict matching, but the requested recorded source score
does not change. Ordinary new expressions and the existing guard are retained.

Tracked source status was clean after the completed sweeps and before this
report update. No source/test change remains; the preceding fresh complete ARM
corpus/native library and reader/parser validation remains applicable. Exact
sets remain 2,079 linked / 959 raw with no losses. Fixed means stay **97.13%
unweighted / 97.35% weighted**. The >=99.0% goal remains unmet; these negative
results do not prove a compiler ceiling or establish game/ABI readiness.

## Continuation: optional enhanced count and presence storage

Four valid isolated source trials from `68d4fc10` distinguish the final optional
unsigned count/presence pair from the earlier rejected all-section count sharing.
Canonical flags and matcher remain unchanged.

| Trial | Enhanced source / strict | Compiled instructions |
|---|---|---:|
| Move optional count into enclosing block | 97.9 / 67.0 | 636 |
| Move V5 presence into function scope | 97.0 / 59.6 | 639 |
| Move signed V5 count outside channel loop | 97.7 / 66.9 | 638 |
| Share only optional unsigned count and V5 presence | **97.9 / 67.5** | 636 |

An initial scripted sharing trial accidentally redirected the signed V5 count
read as well, leaving its own loop bound unwritten. That invalid trial's
90.7/50.3/553 result is excluded from valid comparisons and was never retained.
It was restored and replaced by a bounded substitution that explicitly asserts
the signed V5 count/read remain intact. The corrected sharing trial is the
last row above and is the only accepted change.

Original Android optional-count read sets r1 = sp+28 at `0x6c0d2`, and its V5
presence read sets the same address at `0x6c1c4`. The decompiler also identifies
unsigned v66 at sp+0x1c and a separate signed V5 v64 at sp+0x16. The accepted
source reuses one unsigned short for just those two reads; it preserves the
separate signed channel count and all read-before-use/failure paths. This is
source-backed storage reuse, not recovered exact C++ variable names.
Enhanced source rises 97.8 -> 97.9, strict 64.9 -> 67.5; reported instructions
636/636. Components: inventory 96.0, opcodes 97.5, control 96.3, calls 100.0,
size 99.7. Equal reported instruction counts do not imply identical code.

Native UCRT64 `gof2` library build passed, as did both enhanced and expanded
reader/vector-math smoke tests. Enhanced smoke now checks two optional keys
with V4 and with absent V5 presence, verifying the presence read overwrites the
preceding count and skips the V5 channels. Existing signed counts and all
truncated V5 byte prefixes still pass. No native game/device runtime was run.

Fresh full ARM validation rebuilt all 204 translation units in
`_work/optional-full`. Full corpus: 4,619 comparisons, zero skipped,
4,274/4,524 originals compared, missing 250 (7 wrong type, 243 absent), extra
130. Actual `(unit, symbol)` sets equal the clean baseline: linked-exact 2,079,
byte-exact 959, lost [] and gained [] for each. Other 202 objects remain
byte-identical to baseline; Core equals the preceding validated full object,
and Mesh equals the corrected focused trial.
Function-boundary comparison against the clean Git baseline also confirms both
files and signatures are identical outside the three allowed bodies.

Full strict/source means: 75.98/88.99. SHA-256: `fb73e36e062d4eb352d2c41f3f8526daa8ece1a265468f52803c1eaab585dc97`.
Evidence: `_work/optional_full.json`, `_work/optional_three.json`,
`_work/optional_full_build.log`, `_work/optional_full_verify.log`,
`_work/optional_native.log`, `_work/optional_smoke.exe`,
`_work/optional-read-smoke/`.

Fixed source scores: Create 96.2, Read 97.4, Enhanced 97.9. The original
291/873/636 weights still total 1,800. Means: **97.166667% unweighted / 97.382667% weighted** (97.17/97.38 displayed).
The >=99.0% gate remains unmet; no objective compiler ceiling or game/ABI
readiness is proven.

## Continuation: V5 finish flow and incremental prefix count reuse

Seven valid isolated trials start from `de31dd9d`; none improves the requested
source score. Every source change was restored. Canonical flags, the matcher
and the three unique-body denominator remain unchanged.

| Trial | Enhanced source / strict | Compiled instructions |
|---|---|---:|
| Early finish when V5 presence is zero | 97.9 / 67.5 | 636 |
| Early finish when V5 format bit is absent | 97.9 / 67.5 | 636 |
| Both early-finish guards | 97.9 / 67.5 | 636 |
| Express unsigned presence != 0 as > 0 | 97.9 / 67.5 | 636 |
| Share the optional word with earlier vector-group counts | 97.3 / 63.7 | 635 |
| Share the optional word with earlier axis-group counts | 97.5 / 50.7 | 641 |
| Share the optional word with all earlier group counts | 97.5 / 52.2 | 629 |

All four finish-flow trials produce Mesh.o byte-identical to the preceding
validated full object, not just equal displayed scores. These particular
equivalent CFG spellings normalize to the same compiled result. The prefix
trials were incremental tests motivated by original v66/sp+28 count reuse;
they explicitly preserve the separate signed V5 count and do not repeat the
earlier invalid read substitution. Broadening reuse regresses the measured
shape, so only the previously accepted final count/presence pair remains.

Tracked source status was clean after the sweeps and before this report-only
addition. No implementation/test changes remain; the preceding fresh 204-TU
ARM/native library and reader/parser gates remain applicable. Exact sets stay
2,079 linked / 959 raw with no losses. Means stay **97.17% unweighted / 97.38%
weighted**. The >=99.0% objective remains unmet; these negative results do not
prove an objective compiler ceiling.

## Continuation: separate single-mesh reader success checks

Nine valid isolated trials start from `60853546`. The first four change only
the control flow after the uncompressed color read in MeshReadData; none
improves source match and all were restored.

| Reader color-completion trial | Read source / strict | Compiled instructions |
|---|---|---:|
| Return directly when enhanced data is absent | 96.9 / 54.7 | 883 |
| Combined completion guard | 97.3 / 55.1 | 873 |
| Result local and combined guard | 97.3 / 55.1 | 873 |
| Inverted read-success branch | 97.4 / 55.2 | 865 |

The remaining five trials test equivalent predicates on the two existing
single-mesh MeshReadData calls in MeshCreateFromFile. Both the current reader
definition and Android original return only 1/-1. The original uncompressed
color return selection is visible at `0x6cf0c..0x6cf16`; the remaining exits
select those same two values. Thus ==1, !=-1, >0 and >=0 are equivalent for
this verified callee, without new calls, arguments, side effects or branches.

| Caller status trial | Create source / strict | Compiled instructions | Calls |
|---|---|---:|---:|
| Enhanced single-mesh success ==1 | **98.4 / 60.7** | 289 | 100.0 |
| Plain single-mesh success ==1 | 98.4 / 60.3 | 289 | 100.0 |
| Both single-mesh successes ==1 | 95.5 / 61.3 | 280 | 95.8 |
| Plain single-mesh success >0 | 97.3 / 60.0 | 289 | 100.0 |
| Plain single-mesh success >=0 | 97.3 / 60.0 | 289 | 100.0 |

Only the first caller row is retained: a one-condition change in the enhanced
single-mesh path, with the ordinary and child paths untouched. Compared with
the previous 96.2/61.6/280 result, source rises to 98.4 and strict falls to
60.7. Inventory/opcodes/control/calls/size are 94.7/97.9/100.0/100.0/99.3;
ordered control is 91.7. The original has 291 instructions.

Original Android calls the reader at `0x6d2bc` (child), `0x6d2f8` (enhanced
single), and `0x6d30e` (plain single). Using identical !=-1 predicates allowed
the current compiler to merge the single paths; the retained equivalent check
keeps all three call sites. Both original single checks test -1 via adds #1.
Direct object disassembly confirms three MeshReadData call relocations at
Core object offsets `0x3ea2`, `0x3ee2` and `0x3f28` (object offsets, not Android
virtual addresses); `_work/createstatus_create_disassembly.txt` preserves them.
The selected ==1 C++ spelling is consequently a heuristic for compiler shape,
NOT recovered exact source or an exact instruction-level compare. Its
equivalence depends on the verified 1/-1 reader contract. No matcher, flags,
helpers, aliases or denominator changed.

At the preceding Create score of 96.2, even Read=Enhanced=100 would produce
only 98.733333 unweighted. This is a conditional arithmetic bound on that
particular state, not an objective compiler ceiling. Improving Create removes
that particular barrier; neither required mean has yet reached 99.0.

Native UCRT64 gof2 static-library build passed. Existing production Mesh.cpp
enhanced parser tests and verbatim reader/production vector-math smoke passed.
The new `tools/mesh_create_smoke.cpp`, enabled by the existing runner's
`--create-smoke`, exercises verbatim current Create and Read definitions with
production Mesh.cpp. It covers plain AEMesh success, V4 single-mesh success
for subCount 0 and 1, truncated position data, truncated child-count data,
engine/path guards and OpenRead failure. It checks output cleanup and exactly
one file close on opened-file success/failure. File access, release and
unexercised Transform/clone boundaries are mocked; the reader result itself
is not mocked. These are bounded body tests,
not a whole-Core.cpp executable linkage, native ABI proof or game/device run.
The pre-existing enhanced-error ==0 convention remains unchanged: do not infer
that all enhanced-data truncations are rejected from these selected fixtures.

Fresh full ARM validation rebuilt all 204 translation units in
`_work/createstatus-full`: 4,619 comparisons, zero skipped, 4,274/4,524 originals
compared, missing 250 (7 wrong type, 243 absent), extra 130. Actual normalized
`(unit, symbol)` sets equal the clean baseline: linked-exact 2,079, byte-exact
959; lost [] and gained [] for each. Other 202 ARM objects remain byte-identical
to baseline. Core equals the accepted focused trial; Mesh equals optional-full.
Boundary comparison against the clean baseline confirms both source files and
signatures are unchanged outside the three authorized bodies.

Full strict/source means: 75.98/88.99. Report SHA-256: `505c0cbc329601461e7e58db51515fb6ad854e881fcde5d41f0430f701b948d0`.
Evidence: `_work/createstatus_full.json`, `_work/createstatus_three.json`,
`_work/createstatus_full_build.log`, `_work/createstatus_full_verify.log`,
`_work/createstatus_native.log`, `_work/createstatus_read_smoke.log`, `_work/createstatus_smoke.exe`,
`_work/createstatus-read-smoke/`, and the nine focused trial reports.

Fixed source scores: Create **98.4**, Read **97.4**, Enhanced **97.9**.
Original weights remain 291/873/636, total 1,800. Means: **97.900000%
unweighted / 97.738333% weighted** (97.90/97.74 displayed).
The required >=99.0% pair is still unmet; the goal remains active with no
demonstrated objective compiler ceiling or proven native game/ABI readiness.

## Continuation: caller status flow and optional-section mask nesting

Eight valid isolated trials start from `f785201f`; none improves either fixed
source mean. All source edits were restored. The unchanged canonical matcher
and flags were used for each focused ARM compilation/comparison.

| Trial | Body | Source / strict | Compiled instructions |
|---|---|---|---:|
| Enhanced single success >0 | Create | 97.8 / 60.7 | 289 |
| Enhanced single success >=0 | Create | 97.8 / 60.7 | 289 |
| Enhanced !=1 failure guard, then goto loaded | Create | 98.4 / 60.7 | 289 |
| Switch on enhanced result, case 1 loads | Create | 98.4 / 60.7 | 289 |
| Sequential valid-engine/path boolean guard | Create | 98.4 / 61.0 | 289 |
| Nest V5 section under mask 0x18 | Enhanced | 97.8 / 66.5 | 639 |
| Same nesting, word scoped inside mask block | Enhanced | 97.9 / 66.8 | 640 |
| Early finish when mask 0x18 is absent | Enhanced | 97.8 / 66.5 | 639 |

The caller status predicates preserve the verified 1/-1 reader result contract
and all three reader call sites (call-count match 100.0 throughout). Failure
guard and switch variants produce the entire Core object byte-identical to
the preceding full validated object. The boolean-guard interaction differs
but only improves strict score, so it is not retained. It was revisited after
the genuine separate-call-path change, rather than assuming its earlier result
applied to this materially different caller state.

Optional nesting is semantically equivalent because format bit 0x10 implies
mask 0x18 is nonzero. The first three sections still read in order for every
format; optional word reuse and the separate signed V5 channel count remain
intact. The original checks mask 0x18 at `0x6c0b6` and later bit 0x10 at
`0x6c184`; this does not establish the original C++ block nesting. No selected
trial adds file operations or changes unknown-type fallthrough.

Also inspected current allocation-shape inventories for Create and Enhanced
against the full original targets. Create's remaining differences include
zero-offset memory spellings, predicate setup, stack loads and ==1 versus
original -1 checks. Enhanced's V5 predicate currently emits lsls #27 / bmi;
the original emits ands #16 / bne. Some inventory differences are register-role
artifacts: the normalizer treats fp-relative loads as frame slots, even where
the original fp is the animation pointer. Direct normalized instruction lists
show both original and current key-count member loads at offset 284 (0x11c),
and both animationStart stores at 232 (0xe8). This is not evidence to alter
Transform layout or to introduce padding. The diagnostic matcher was only
read/called and remains unchanged. These observations are not an objective
ceiling or proof that the remaining deficit is entirely allocator noise.

Evidence: `_work/create_status_flow_sweep.py`, its five
`_work/createstatusflow-*.json` reports, `_work/enhanced_optional_mask_sweep.py`,
its three `_work/enhoptionalmask-*.json` reports, and the current normalized
instruction lists generated by `_work/inventory_gap.py` from
`_work/createstatus-full`. Tracked source status was clean after both sweeps;
function-boundary comparison still confirms only the three permitted bodies
differ from baseline. No production/test edits remain in this continuation.
The preceding fresh full ARM/native validation therefore remains applicable:
actual exact sets are 2,079 linked / 959 raw with no losses or additions.

Fixed scores remain Create 98.4, Read 97.4, Enhanced 97.9; means **97.90%
unweighted / 97.74% weighted**. Even Read=100 alone would give only 98.766667%
unweighted with the other two unchanged; at least one other body must also
improve. This is a conditional arithmetic bound, not a compiler ceiling.
Both >=99.0% requirements remain unmet and the goal stays active.

## Continuation: normal clamp forms and quotient scheduling

Seven valid isolated reader trials start from `1dd46450`. They retain float
quotients, double lower selection and float upper comparisons as numerical
operations; none improves source match. All source changes were restored.

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| Explicit promoted doubles, then lower-bound if statements | 96.5 / 54.5 | 856 |
| Upper-bound ternaries | 97.4 / 55.4 | 871 |
| Complete lower/cast/upper clamp per component | 97.4 / 55.4 | 871 |
| Common Z output after normal/zero branches | 97.4 / 55.4 | 871 |
| Interleave Y/X quotients, lower Y, Z quotient, lower X/Z | 97.4 / 55.4 | 871 |
| Same quotient staging plus Y/X cast and upper-clamp order | 97.4 / 55.2 | 871 |
| Interleave each promoted double with its lower-bound if | 96.5 / 54.5 | 856 |

The common-Z trial is a targeted interaction recheck following the accepted
triangle-numerator scheduling and current 97.4 reader state; it does not
assume the earlier 97.0 common-Z result applies unchanged. The quotient
interleaving tests follow the original sequence at `0x6cb2a..0x6cb58`: Y/X
float divides, lower-Y double comparison, Z divide and remaining double
lower comparisons. Upper comparisons remain on original float quotients,
as confirmed at `0x6cb6a..0x6cb92`. This sequence is instruction evidence,
not a recovered exact C++ declaration order.

Whole Core objects from upper ternaries, component clamps, common-Z output
and staged Y/X/Z quotients are byte-identical to createstatus-full. The
Y-cast/upper-order variant differs but only regresses strict match. Direct
allocation-shape inventory inspection of the explicit-double/if variant
shows three NEON vmax.f32 operations and the loss of the original three
double compares, three vmovmi.f64 selections and their conversions. Thus
its shorter 856-instruction result is not the requested source-shape gain.
The equivalent source form allowed different optimizer lowering; no matcher
or normalization was altered to compensate.

Evidence: `_work/read_clamp_form_sweep.py`, its four
`_work/readclampform-*.json` reports, `_work/read_clamp_schedule_sweep.py`,
its three `_work/readclampschedule-*.json` reports, and the inventory diagnostic
from `_work/inventory_gap.py` against the explicit-double/if object. Every
trial compiled with the existing canonical ARM flags and focused unit gate.
Tracked source status is clean after the sweeps; the baseline boundary check
again passes for both source files. This continuation changes only this
report and requires no new runtime validation of unchanged production code.
The preceding 204-TU ARM/native and caller/reader/parser validation remains
applicable, with actual linked-exact 2,079 / byte-exact 959 pair sets intact.

Fixed source scores remain 98.4 / 97.4 / 97.9 and means **97.90% unweighted /
97.74% weighted**. The >=99.0% goal stays active. These negative compiler-form
results establish neither a compiler ceiling nor native game/ABI readiness.

## Continuation: captured float-UV component bound

Four valid isolated trials start from `55ab1ab6`, revisiting the uncompressed
UV loop following the accepted bounds/color/numerator scheduling changes.
Canonical flags, matcher and denominator remain unchanged.

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| Use the captured vertex count for the UV bound | **97.5 / 55.7** | 872 |
| Reread texCoords from the owner inside the loop | 97.5 / 55.0 | 872 |
| Combine captured bound and owner reread | 97.1 / 55.8 | 861 |
| Combined form with a precomputed component count | 97.1 / 55.8 | 861 |

Only the first variant is retained. It changes the loop limit from repeated
`m->vertexCount << 1` to `vcount << 1`, using the count already assigned just
before the loop. No extra state, calls or effects are introduced. Original
Android loads the unsigned-short count at `0x6c9e2`, doubles it at `0x6c9e4`
and compares against that retained component limit at `0x6ca28`. It also
rereads the UV pointer at `0x6ca14`; that separate source form was tested but
is not retained because the combined change regresses the required score.
This is a partial source-backed loop-bound correction, not a claim of exact
recovered C++ or complete recovered ownership semantics.

Read source rises 97.4 -> 97.5, strict 55.4 -> 55.7. Its inventory/opcodes/
control/calls/size scores are 92.5/96.9/98.1/100.0/99.9; ordered control 90.3.
Compiled instruction count 872 versus original 873.

Native UCRT64 gof2 static-library build passed, as did the actual Mesh.cpp
enhanced parser smoke and the verbatim reader/production vector-math smoke.
The Create/Read/actual enhanced-parser test now loads two-vertex V4 float UV
fixtures with shader disabled/enabled, checking both U values and unchanged
V versus 1-V, complete file consumption, output count and file lifecycle.
Two distinct UV pairs verify both loop iterations. Existing plain/enhanced
success, selected reader failures, guard and OpenRead-failure checks pass.
File/release/unused Transform boundaries remain mocked; this is not native
ABI proof or a native game/device runtime test.

Fresh full ARM rebuild of all 204 translation units in `_work/floatuvbound-full`
passes: 4,619 comparisons, zero skipped, 4,274/4,524 originals compared,
missing 250 (7 wrong type, 243 absent), extra 130. Actual normalized
`(unit, symbol)` sets equal baseline: linked-exact 2,079, byte-exact 959,
lost [] and gained [] for both. The other 202 objects remain byte-identical
to baseline; Core equals the accepted focused object, Mesh equals the
preceding full object. Baseline function-boundary checks confirm no changes
outside the three allowed bodies or to their signatures.

Full strict/source averages: 75.98/88.99. Full report SHA-256: `07cdf48375f9a147f2032c5b051da55cf769f2032c023e671d49ce2c5ad3d191`.
Evidence: `_work/floatuvbound_full.json`, `_work/floatuvbound_three.json`,
`_work/floatuvbound_full_build.log`, `_work/floatuvbound_full_verify.log`,
`_work/floatuvbound_native.log`, `_work/floatuvbound_read_smoke.log`,
`_work/floatuvbound_smoke.exe`, `_work/floatuvbound-read-smoke/`, and the four
`_work/readfloatuvowner-*.json` trial reports.

Fixed source scores: Create 98.4, Read **97.5**, Enhanced 97.9. Original weights
remain 291/873/636, total 1,800. Means: **97.933333% unweighted / 97.786833%
weighted** (97.93/97.79 displayed). Both required >=99.0% gates remain unmet;
the goal stays active without a demonstrated compiler ceiling.

## Continuation: raw-owner interactions and enhanced format masks

Eight valid isolated trials start from `b11b2704`. The reader trials revisit
captured raw-buffer owners after the accepted float-UV bound change, not on
the older reader state used for the earlier owner experiments. All counts
still read the vertex-count field before allocation and again before file
reading; only the mesh pointer supplying those fields is changed. No count
formula, stream conversion, read order or cleanup is changed.

| Trial | Body | Source / strict | Compiled instructions |
|---|---|---|---:|
| Captured owner for compressed UV raw buffer | Read | 97.4 / 55.2 | 870 |
| Captured owner for compressed normal raw buffer | Read | 97.5 / 55.7 | 871 |
| Captured owner for short-position raw buffer | Read | 96.9 / 56.1 | 873 |
| Captured owner for int-position raw buffer | Read | 96.9 / 56.1 | 861 |
| Captured owners for both UV and normal raw buffers | Read | 97.4 / 55.2 | 869 |
| Unsigned int enhanced format variable | Enhanced | 97.9 / 67.5 | 636 |
| Direct flags predicates instead of format local | Enhanced | 97.9 / 67.5 | 636 |
| Retain flags & 0x18 as the format local | Enhanced | 97.9 / 67.5 | 636 |

Original UV destination setup at `0x6c8ea..0x6c90a` retains its owner across
allocation before rereading slot for the conversion loop. Original compressed
normal raw reads use the same captured r4 across allocation at
`0x6ca44..0x6ca60`. These are memory-operand leads for testing lifetime/owner
forms, not recovered exact source. The normal-owner alternative ties the
current displayed score and strict score but reduces the instruction count
away from 873; it is not retained. Matching 873 instructions in the short
position alternative also fails to improve the actual source-shape metric.

Enhanced format trials revisit the earlier two equivalent predicates after
the accepted optional-count/presence storage reuse. Only mask bits 0x18 and
0x10 are used by the body, so retaining just 0x18 preserves all predicates,
including inputs with unrelated higher bits. Wide-format and direct-flags
trials produce entire Mesh objects byte-identical to floatuvbound-full. The
retained-mask object differs but has unchanged reported source/strict/count.
Neither matcher nor flags was altered, and no variant was accepted.

Evidence: `_work/read_owner_uvbound_interaction.py` and five
`_work/readowneruvbound-*.json` reports; `_work/enh_flag_optional_interaction.py`
and three `_work/enhflagoptional-*.json` reports. Tracked source status is
clean after both sweeps; the baseline body/signature boundary check passes.
Only this report changes, so the preceding fresh full ARM/native and
reader/parser/caller validation remains applicable: actual linked-exact
2,079 / byte-exact 959 pair sets remain intact without losses or additions.
Fixed scores stay Create 98.4, Read 97.5, Enhanced 97.9; means **97.93%
unweighted / 97.79% weighted**. Both >=99.0% requirements remain unmet and
the goal stays active; these trials do not establish a compiler ceiling.

## Continuation: Create local scopes after separated call paths

Six valid scope/initialization trials and one equivalent common-return trial
start from `d103167d`. They specifically revisit the caller after the retained
==1 enhanced-single predicate prevented merging its two single-mesh paths.
The earlier scope results on the merged-call caller are not treated as proof
of unchanged behavior of this different compiler state. No trial improves
source match; all source changes were restored.

| Trial | Create source / strict | Compiled instructions | Call-count match |
|---|---|---:|---:|
| Magic local to OpenRead success, array initializer | 98.2 / 52.8 | 289 | 100.0 |
| subCount local to enhanced branch | 97.7 / 54.8 | 289 | 100.0 |
| childPtr local to child loop | 98.4 / 55.5 | 289 | 100.0 |
| Local magic and subCount | 97.7 / 57.2 | 289 | 100.0 |
| Local subCount and childPtr | 97.7 / 58.6 | 289 | 100.0 |
| All three narrowed scopes | 98.0 / 57.9 | 289 | 100.0 |
| Assign result and return from one common label | 98.4 / 61.0 | 289 | 100.0 |

The common-return trial improves only strict match from 60.7 to 61.0, so it
is not retained for the requested source means. Scope trials preserve each
variable's reads and pointer ownership; array initialization writes the same
eight magic bytes as the existing strcpy, including the null terminator.
No destructor-bearing automatic object or extra file operation is introduced.

Two additional exploratory loop-entry forms were compiled, but are excluded
as source-backed replacements: child do-while scores 98.0/59.1/288, and
common-return plus do-while 98.0/59.4/288 (call-count match 100.0 each).
An initial interpretation that the original enters its first child iteration
immediately after Transform creation was wrong. Direct original disassembly
shows counter zero at `0x6d236`, a branch at `0x6d238` to the subCount comparison
at `0x6d2e2`, then bcc at `0x6d2e8` to child creation at `0x6d248`.
Thus the original checks the count after Transform creation even on first
entry. The do-while forms omit that guard; they were never accepted or claimed
runtime-validated. The current for-loop preserves the demonstrated entry test.

Evidence: `_work/create_scope_callpath_interaction.py` and six
`_work/createscopecallpath-*.json` reports, `_work/create_loop_return_interaction.py`
and three `_work/createloopreturn-*.json` reports, plus original_mesh.asm around
`0x6d236..0x6d2e8`. Canonical flags, matcher and denominator are unchanged.
Tracked source status is clean after both sweeps, and baseline body/signature
boundary checks pass. Only this report changes; the preceding fresh full
ARM/native and reader/parser/caller validation still applies to unchanged
production/test sources. Exact linked 2,079 / raw 959 pair sets remain intact.
Fixed scores stay 98.4 / 97.5 / 97.9, with means **97.93% unweighted / 97.79%
weighted**. Both >=99.0% gates remain unmet; the goal stays active.

## Continuation: verify the original full bounds diagonal

This continuation finds a semantic error in the preceding reader reconstruction.
The old center-minus-minimum expression and its sqrt(24) smoke expectation
were unsupported assumptions. They are superseded by the direct original
caller/helper ABI evidence below. The required source means do not increase;
retaining a known incorrect operand solely because it ties a shape score would
leave the native reconstruction and its validation wrong.

Original constant words mapped through the ELF PT_LOAD segments are
`0x6c9ec = +10000000.0f` and `0x6c9f0 = -10000000.0f`. Initialization at
`0x6c6a0..0x6c6ac` puts maxima at sp+108/112/116 and minima at sp+120/124/128.
At `0x6c844..0x6c876` both triples are loaded to compute the center midpoint.
Vector assignment at `0x6c88c` stores that center in the mesh. The later stores
at `0x6c896..0x6c8ac` copy maxima into sp+80/84/88 and minima into sp+68/72/76.
Before subtraction at `0x6c8b0`, r0 points to result sp+96, r1 to maximum
sp+80 and r2 to minimum sp+68. The original operator- body in the delinked
AEMath target directly subtracts [r2] from [r1] and writes the three components
to [r0]. This confirms the hidden-result ABI, not merely the symbol name.
Original VectorLength at `0x6c8b6` receives that result and computes its length
with no half multiplier. Thus the computed legacy radius is length(max-min),
not length(center-min). The earlier radius test was mirroring the mistaken
implementation; its pass did not prove fidelity to this original behavior.

Three exact-operand forms were compiled with unchanged flags and matcher:

| Full-diagonal source form | Read source / strict | Compiled instructions |
|---|---|---:|
| Copies of float maximum/minimum triples | 97.5 / 55.6 | 869 |
| Copies as Vector maximum/minimum objects | **97.5 / 55.6** | 869 |
| Temporary Vector operands | 97.5 / 55.6 | 869 |

The named Vector form is retained as the simplest typed expression of the
verified operands; helpers remain unchanged. Source match stays 97.5, strict
falls 55.7 -> 55.6, reported instruction count is 869 versus original 873.
The center calculation, file reads, conversions, cleanup and all other bodies
remain unchanged. Exact original C++ variable names/temporary syntax are not
claimed recovered.

Reader smoke now checks radius sqrt(96) for the original two-vertex fixture
and sqrt(2)/sqrt(18) for the three tangent fixtures; center and tangent/binormal
checks still pass. Running these corrected checks against the preceding
verbatim reader include fails at the full-diagonal assertion (exit 3), while
the current reader passes. Windows abort reporting is disabled in this test
to keep this deliberate negative regression check noninteractive. Actual
Mesh.cpp enhanced parser, Create/Read/actual enhanced parser with two-vertex
float UVs, all existing truncations and UCRT64 gof2 library build pass.
File/unused Transform boundaries remain mocked, and no game/device runtime
or native ABI readiness is proved. Computed legacy radii follow the full
diagonal and can roughly double the preceding values (exactly double in the
checked fixtures; float midpoint rounding can affect other inputs).
Native-game impact is unverified. Enhanced metadata follows its existing path.

Fresh full ARM rebuild covers all 204 translation units in `_work/fullbounds-full`.
Full corpus: 4,619 comparisons, zero skipped, 4,274/4,524 originals compared,
missing 250 (7 wrong type, 243 absent), extra 130. Actual normalized
`(unit, symbol)` linked-exact/byte-exact sets equal baseline: 2,079/959,
lost [] and gained [] for both. The other 202 objects are byte-identical to
baseline, Core equals the typed focused winner, Mesh equals floatuvbound-full.
Baseline body/signature comparison confirms no source edits outside the
three authorized bodies.

Full strict/source averages: 75.98/88.99. SHA-256: `38477dcc98c9413d109d3344fcde3e4ce2fac4ed6cb9532d620d1114dcb6ff33`.
Evidence: `_work/read_full_bounds_diagonal_sweep.py`, three
`_work/readfulldiag-*.json` reports, `_work/fullbounds_full.json`,
`_work/fullbounds_three.json`, `_work/fullbounds_full_build.log`,
`_work/fullbounds_full_verify.log`, `_work/fullbounds_native.log`,
`_work/fullbounds_read_smoke.log`, `_work/fullbounds_old_reader_regression.log`,
`_work/fullbounds_smoke.exe`, `_work/fullbounds-read-smoke/`, original_mesh.asm
and the original AEMath target object for operator-/VectorLength inspection.

Fixed scores stay Create 98.4, Read 97.5, Enhanced 97.9. Original instruction
weights remain 291/873/636, total 1,800. Means **97.933333% unweighted /
97.786833% weighted** (97.93/97.79 displayed) remain below both >=99.0% gates.
The goal stays active; this semantic correction does not establish a compiler ceiling.

## Continuation: full-diagonal temporary lifetimes

Six valid source trials start from `a773d118` and preserve the now-verified
full max-minus-min diagonal. They keep the center arithmetic, helper calls,
file reads and cleanup unchanged; none improves the requested source score.

| Trial | Read source / strict | Compiled instructions |
|---|---|---:|
| Float center in a nested scope | 97.5 / 55.6 | 869 |
| Vector center in a nested scope | 97.5 / 55.6 | 869 |
| Declare minimum copy before maximum copy | 97.5 / 55.6 | 869 |
| Copy both extents before center assignment | 97.5 / 54.8 | 869 |
| Copy both extents before center calculation | 97.1 / 54.7 | 877 |
| Use those extent copies for center and diagonal | 97.1 / 54.7 | 877 |

Original `0x6c844..0x6c8ac` loads both triples for the midpoint, retains their
values across Vector assignment and copies them for subtraction. The trials
test that value/lifetime structure without changing the verified operands.
The last two forms produce whole Core objects byte-identical to each other:
using copied values instead of the original arrays in that early-copy form
does not change compiler output. The first five objects all differ from the
accepted fullbounds-full object, even when displayed scores are unchanged;
equal scores alone are not reported as binary identity.

Evidence: `_work/read_full_bounds_lifetime_sweep.py` and five
`_work/readboundslife-*.json` reports; `_work/read_bounds_shared_values_trial.py`
and `_work/readboundsshared-values.json`. Every trial used the canonical flags
and unchanged focused unit verifier, then restored its own source snapshot.
Tracked source status is clean and baseline body/signature checks pass.
Only this report changes; the preceding fresh 204-TU ARM/native and corrected
bounds/reader/parser/caller validation remains applicable. Actual exact pair
sets stay 2,079 linked / 959 raw without losses or additions. Scores remain
98.4 / 97.5 / 97.9, means **97.93% unweighted / 97.79% weighted**.
Both >=99.0% gates remain unmet; no compiler ceiling is demonstrated.
