# Mesh loader: fixed three-body ARM audit, 2026-10-02

Latest validated continuation: source **98.40% unweighted / 98.41% weighted**.
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

## Continuation: verify tangent operands and copy the normal Vector

Direct original ARM caller/helper inspection confirms the final tangent sequence:
normal is copied to sp+96 (fp), accumulated tangent to sp+80 (r8), and
VectorDot at 0x6ce04 receives r0=normal and r1=tangent. Multiply at 0x6ce12
receives normal and dot, with its hidden result at sp+44. Subtraction at
0x6ce20 receives tangent minus scaled normal; Normalize at 0x6ce2a receives
that result. Cross at 0x6ce46 receives normal and normalized tangent, with
its hidden result at sp+56. The original delinked VectorCross body independently
confirms lhs cross rhs and its hidden-result ABI. Therefore current
dot(N,T), normalize(T-N*dot), cross(N,Tnormal) operands match the original.
No sign or operand reversal was introduced.

Four input-copy forms were tested on the current full-diagonal reader state:

| Input source form | Read source / strict | Compiled instructions |
|---|---|---:|
| Copy normal as Vector, retain scalar tangent copy | **97.6 / 55.7** | 867 |
| Copy accumulated tangent as Vector | 97.5 / 56.3 | 870 |
| Copy both as Vector | 97.5 / 56.1 | 868 |
| Aggregate-initialize both Vector inputs | 97.5 / 55.6 | 869 |

Only the first form is retained. It copies the same three normal components
into a local Vector and uses it in the existing dot, multiply and cross calls.
This is a compiler-shape choice supported by the original input-copy behavior;
exact original C++ syntax is not claimed recovered. Helpers, arguments, math
ordering, reads and other bodies remain unchanged. Earlier input-copy trials
used a different reader state and do not substitute for this interaction check.

UCRT64 gof2 static-library build, verbatim reader with actual selected vector
math, caller with actual Mesh.cpp enhanced parser, and separate actual Mesh.cpp
parser smoke pass. Existing three indexed triangle fixtures verify tangent and
binormal components for XY and XZ planes, including mixed UVs; bounds and
truncation checks also pass. No redundant fixture was added for this copy-only
change. File/unused Transform boundaries remain mocked. These checks do not
prove a whole native executable's ABI or game/device runtime readiness.

Fresh full ARM rebuild covers 204 translation units in `_work/normalinput-full`.
There are 4,619 comparisons, zero skipped, 4,274/4,524 originals compared,
missing 250 (7 wrong type, 243 absent), extra 130. Actual normalized
`(unit, symbol)` sets equal the clean baseline: linked-exact 2,079, byte-exact
959; lost [] and gained [] for both. The other 202 objects are byte-identical
to baseline; Core equals the focused normal-copy winner and Mesh equals
fullbounds-full. Body/signature comparison confirms source edits stay inside
the three authorized bodies.

Full strict/source averages: 75.98/88.99. Full report SHA-256: `78db07999425a2ba550dd5f26c1deb273e4a0657f72fc34b6c1bb073bb9508f7`.
Evidence: `_work/read_tangent_fullbounds_inputs.py`, four
`_work/readtangentfullbounds-*.json` reports, `_work/normalinput_full.json`,
`_work/normalinput_three.json`, `_work/normalinput_full_build.log`,
`_work/normalinput_full_verify.log`, `_work/normalinput_native.log`,
`_work/normalinput_read_smoke.log`, `_work/normalinput-read-smoke/`,
`_work/normalinput_smoke.exe`, original_mesh.asm and original delinked
AEMath target object.

Fixed scores are Create 98.4, Read 97.6, Enhanced 97.9. Original instruction
weights remain 291/873/636, total 1,800. Means **97.966667% unweighted /
97.835333% weighted** (97.97/97.84 displayed) remain below both >=99.0%
requirements. The goal stays active; this does not establish a compiler ceiling.

## Continuation: use the exact original V5 rotation literal

The earlier parser and loose smoke assertion used the rounded decompiler
spelling 6.2832f. Direct ELF PT_LOAD mapping of the original Android 2.0.16
binary proves a different float word: VA 0x6c2f8 contains bytes db 0f c9 40,
word 0x40c90fdb, value 6.2831854820251465. Neighboring literals at 0x6c2f4
and 0x6c2fc are exactly 100.0f and 360.0f. The original binary SHA-256 is
6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58.
Original instructions 0x6c25c, 0x6c296 and 0x6c2a0 divide the loaded key by
100 in float, multiply channel 6 by this float literal, and divide by 360
in float. Thus the instruction operands and precision are established from
the binary, independently of the decompiler's approximate decimal display.

The production literal becomes 6.283185307179586f, which rounds to exactly
0x40c90fdb. No operation ordering or other channel behavior changes. The
test constructs its expected float from the original uint32 word using memcpy
and checks exact numerical equality for the fixture's rotation result. The old
parser fails this assertion with exit 3; the corrected parser passes. The
previous absolute tolerance of 0.000001 had hidden the approximation error.
This corrects original-value fidelity, without adding runtime calls or effects.
The focused Mesh.o has the same 5,056-byte size as fullbounds-full and differs
at exactly one byte: file offset 0x868 changes 0xf9 to 0xdb in this literal.
All instruction and other object bytes are unchanged by the constant correction.

UCRT64 gof2 library build, actual Mesh.cpp parser smoke with all existing
truncated V5 prefixes, verbatim reader/actual vector math, and caller with
actual enhanced parser pass. Boundaries remain mocked and whole-game/device
runtime and native executable ABI remain unverified.

Fresh all-204-TU ARM rebuild is `_work/angle-full`. Full corpus has 4,619
comparisons, zero skipped, 4,274/4,524 originals compared, missing 250
(7 wrong type, 243 absent), extra 130. Actual normalized `(unit, symbol)`
linked-exact and byte-exact sets equal baseline: 2,079 and 959, lost [] and
gained [] for both. The other 202 objects are byte-identical to baseline;
Core equals normalinput-full and Mesh equals the focused angle-literal object.
Source body/signature boundaries remain unchanged outside the three bodies.

Full strict/source averages: 75.98/88.99. Full JSON SHA-256: `78db07999425a2ba550dd5f26c1deb273e4a0657f72fc34b6c1bb073bb9508f7`.
Evidence: `_work/probe_enhanced_angle.py`, `_work/angle_literal.json`,
`_work/angle_full.json`, `_work/angle_three.json`, `_work/angle_full_build.log`,
`_work/angle_full_verify.log`, `_work/angle_old_regression.log`,
`_work/angle_native.log`, `_work/angle_read_smoke.log`,
`_work/angle-read-smoke/`, `_work/angle_smoke.exe` and original_mesh.asm.

Fixed source scores remain Create 98.4, Read 97.6, Enhanced 97.9, with original
instruction weights 291/873/636. Means **97.966667% unweighted / 97.835333%
weighted** remain below both >=99.0% requirements. This semantic correction
does not complete the goal or prove an objective compiler ceiling.

## Continuation: output-copy interactions after the accepted normal copy

The previous goal turn made authoritative progress: normal-copy source match
rose to 97.6, its full-corpus validation and commit b1ea59c1 completed, and
direct ELF evidence exposed the rounded V5 rotation literal. The angle-full
verification was live when interrupted. Current-state inspection found that
process terminal/missing and a completed full JSON/log, rather than restarting
the job. The completed report and all object/set assertions above passed;
the exact-literal correction is committed as 7a248f48.

Four local copy-form trials were then measured against the accepted normal-copy
and full-diagonal reader state. Earlier output-copy trials used a different
reader, so this checks interaction with the newly changed local Vector:

| Trial | Read source / strict | Compiled instructions | Call-count match |
|---|---|---:|---:|
| memcpy normalized tangent output | 97.4 / 55.6 | 865 | 100.0 |
| memcpy binormal output | 97.4 / 55.9 | 865 | 100.0 |
| memcpy both outputs | 97.3 / 55.8 | 863 | 100.0 |
| Aggregate-initialize only the normal Vector | 97.5 / 55.6 | 869 | 100.0 |

None beats the retained reader's 97.6 source match, and no trial is retained.
The original scalar output stores at 0x6ce2e..0x6ce64 motivate checking
equivalent local copy forms, not a claim that original C++ used memcpy.
The experiment's finally block restores the accepted source, checked against
the committed state. No helpers, flags, matcher, denominator or other bodies
were changed. No new full rebuild is needed for these rejected temporary edits.
Evidence: `_work/read_normalcopy_output_sweep.py`, four
`_work/readnormalcopyoutputs-*.json` reports and their saved source snapshots.
The validated means stay 97.966667/97.835333; the goal remains active below 99.

## Continuation: enhanced local lifetime and per-group count evidence

The previous turn made progress by validating and committing the exact V5
literal and rejecting measured output-copy interactions. This turn starts from
clean tracked state at 7314f5f9 and uses the completed angle-full objects.
Direct original inspection confirms signed type loads at 0x6bc88 and the
corresponding scale/rotation dispatches, while optional type==2 is compared
with an unsigned load. Changing all section types to unsigned is therefore
not supported as a recovery claim. Prefix counts and optional count/presence
are repeatedly loaded from sp+28; V5 per-channel count is signed at r7-82.
The shared physical slot motivates count-lifetime experiments, not recovered
original C++ lexical scopes. Helpers and compiler/matcher settings stay fixed.

| Local lifetime trial | Enhanced source / strict | Compiled instructions |
|---|---|---:|
| V5 value/key outside inner key loop, inside channel | 97.4 / 64.1 | 641 |
| V5 value/key outside channel loop | 97.7 / 67.3 | 636 |
| V5 count outside channel, value/key inside channel | 97.7 / 67.3 | 636 |
| Optional-section value/key outside key loop | 97.9 / 67.1 | 639 |
| Position group shares one unsigned count between type cases | 97.9 / 53.8 | 635 |
| Scale group shares one unsigned count between type cases | 97.7 / 52.0 | 636 |
| Rotation group shares one unsigned count between type cases | 97.8 / 66.9 | 644 |
| All three separately scoped groups share their own count | 97.5 / 52.2 | 632 |
| Position vector declared before the timestamp read | 97.9 / 67.5 | 636 |
| Scale vector declared before the timestamp read | 97.9 / 67.5 | 636 |
| Rotation vector declared before the timestamp read | 97.9 / 67.5 | 636 |
| All three vectors declared before the timestamp read | 97.9 / 67.5 | 636 |

No form improves the requested source score. The last four whole Mesh.o files
are byte-identical to angle-full, a stronger finding than equal rounded scores:
these declaration-order-only forms do not change the compiled object here.
None is retained. Accepted source is restored after each sweep and its tracked
diff is empty; body/signature comparison remains valid outside the three bodies.
The initial group-count experiment's pattern assertion detected the optional
fourth read before any source write; anchoring it to exact 12-space indentation
limits the subsequent sweep to the three intended prefix groups.

Evidence: `_work/enh_optional_value_lifetime_sweep.py`,
`_work/enh_group_count_sweep.py`, `_work/enh_vector_declaration_sweep.py`,
their twelve focused JSON reports and saved source snapshots. Native/full
corpus reruns are unnecessary for these restored rejected edits; accepted
code is exactly the previously validated state. The next useful search should
inspect remaining control-flow/operand differences rather than repeat these
measured local declaration forms. Means remain 97.966667/97.835333, both
below 99.0; this evidence neither completes the goal nor proves a compiler ceiling.

## Continuation: reader product and vertex-format predicate identity

Previous turn yielded measured progress by eliminating twelve specific enhanced
local-lifetime forms and proving four object identities. This continuation starts
from clean tracked state at 5fff8b2e. It re-reads the authoritative angle-full
fixed-three rows rather than relying on the displayed means alone:

| Body | Source | Instruction inventory | Opcode inventory | Control inventory | Calls | Size |
|---|---:|---:|---:|---:|---:|---:|
| Create | 98.4 | 94.7 | 97.9 | 100.0 | 100.0 | 99.3 |
| Read | 97.6 | 93.3 | 97.3 | 98.1 | 100.0 | 99.3 |
| Enhanced | 97.9 | 96.0 | 97.5 | 96.3 | 100.0 | 99.7 |

Read's largest remaining component deficit is instruction inventory. Original
0x6cd90..0x6cd98 computes three tangent products Y/X/Z before the scalar
stores at 0x6cd9c..0x6cda4. Three equivalent C++ forms were checked against
the accepted normal-copy reader: compound assignments before the stores,
named scaled products before the stores, and compound assignments at the
stores. Operand order remains numerator times reciprocal, and float arithmetic
and Y/X/Z scheduling remain unchanged. All score 97.6 / strict 55.7 with 867
instructions, and all three entire Core.o files equal the accepted angle-full
object byte for byte. The tested product-local spelling does not alter code.

Five further forms test original-style sign predicates for vertex-format bits:
individual index/UV/normal/color conditions, plus all four together. Each uses
an unsigned shift followed by a signed-negative comparison at shifts 27/30/29/28,
corresponding to bits 4/1/2/3 on the project's 32-bit native/ARM targets. This
is an equivalent target-specific condition, not recovered original source or
a general portability claim about unsigned-to-signed casts. All five again
score 97.6 / 55.7 with 867 instructions, and all entire Core.o files equal
angle-full byte for byte. No alternative predicate is retained.

Evidence: `_work/read_tangent_product_sweep.py`,
`_work/read_stream_predicate_sweep.py`, their eight focused JSON reports,
saved source snapshots and object comparisons with angle-full. The diagnostic
`_work/inventory_gap.py` comparison also identifies frame loads, zero-offset
address spellings and reused floating-register operands among residual shapes.
Those diagnostic normalizations are unchanged and are not a completion gate
or proof that a specific original C++ form can attain the objective.

After both finally restorations, tracked source diff is empty and the baseline
outside-body/signature check passes. Accepted production code, native validation
and full actual linked/byte-exact sets therefore remain the angle-full state.
Next search should follow concrete memory loads and value lifetimes rather
than repeat these product/predicate spellings. The fixed denominator remains
three unique bodies and 1,800 original instructions. Means 97.966667/97.835333
remain below both 99.0 requirements; the goal stays active.

## Continuation: guarded child loop and verified recursive-reader paths

Previous turn yielded object-identity evidence excluding eight product/predicate
spellings; accepted state starts at 72e0c954. Original reader operands at
0x6cf42 and 0x6cf52 reject zero read results. Child count uses unsigned 16-bit
loads at 0x6cf68 and 0x6d02a; the original has an initial zero guard before
allocation and a comparison at 0x6d030..0x6d034. Format/material setup at
0x6cfea..0x6cff4 uses one captured parent pointer. Recursive read rejects -1
at 0x6d004..0x6d006, then merges bounds and appends the child. The count is
loaded again after each completed child; no fixed captured count is inferred.

Seven focused forms were measured with unchanged flags and matcher:

| Child source form | Read source / strict | Compiled instructions |
|---|---|---:|
| childPtr declared just before loop | 97.5 / 55.7 | 866 |
| childPtr declared before child-count read | 97.5 / 55.7 | 866 |
| Capture parent for format/material | 97.6 / 55.7 | 867 |
| Pre-loop childPtr plus captured parent | 97.5 / 55.7 | 866 |
| Explicit bound, reloaded after each child | 97.6 / 56.1 | 869 |
| First-count guard, then do/while | **97.9 / 56.0** | 864 |
| Same guard/do with reloaded intermediate bound | 97.9 / 56.0 | 864 |

Retain the shorter guarded do/while. For unsigned childCount, the first
`childCount != 0` condition equals the old initial `0 < childCount`; increment
and subsequent count comparison occur after the same successful child work.
The first-entry guard is necessary and preserved, unlike an unguarded do/while.
Calls, arguments, reads, merges, append ordering and failure returns are unchanged.
This is a measured compiler-shape choice consistent with original behavior,
not recovered original C++ syntax. The parent-only trial equals the accepted
Core object byte for byte. Hoisted-pointer trials change the object and regress
source score; they are not retained. The accepted Read components become
inventory 93.9, opcodes 98.0, control inventory 98.8, calls 100.0, size 98.8.

The native function-body smoke now executes 0/1/2 children using the current
reader, actual enhanced parser and verbatim BSphere::operator=/Merge bodies.
It checks complete input consumption, child order/positions, inherited format
and material, vboEligible, merged bounds, and failure at the first or second
child pivot. Existing caller, UV, tangent/binormal and truncation checks pass.
The fixture's Transform key/range construction and file/release boundaries
remain mocks; no native executable ABI or game/device runtime is proved.
Whole BSphere.cpp initially retained an unused Transform-overload dependency
on two matrix helpers under the Windows linker; selecting the exact two
exercised production bodies removes that unrelated dependency without changing
their text or substituting a mathematical mock. Native gof2 library build passes.

Existing reader failure leaves an unattached child allocated. The test captures
its failing pivot destination, derives its owner with offsetof(Mesh,pivotX),
and explicitly frees it after assertions. This is test cleanup, not a production
leak fix. The first-child failure attaches zero children; the second-child
failure preserves one completed child. Parser -1-versus-zero handling and the
original cleanup behavior remain as previously documented.

Fresh full ARM rebuild: 204 translation units, `_work/readchilddo-full`.
Full corpus: 4,619 comparisons, zero skipped, 4,274/4,524 originals compared,
missing 250 (7 wrong type, 243 absent), extra 130. Actual normalized
`(unit,symbol)` linked-exact/byte-exact sets equal clean baseline: 2,079/959,
lost [] and gained [] for each. Other 202 objects are byte-identical to baseline;
Core equals the focused guarded-do winner, Mesh equals angle-full. Baseline
outside-body/signature check confirms edits remain within the three bodies.

Full strict/source averages: 75.98/88.99. Full report SHA-256: `6105fefa5c92ed4835b9f2f1dad60a03e5b7ec2d68fd961e35237d36b079fce5`.
Evidence: `_work/read_child_lifetime_sweep.py`, `_work/read_child_bound_sweep.py`,
seven focused reports, `_work/readchilddo_full.json`, `_work/readchilddo_three.json`,
build/verify logs, `_work/readchilddo_native.log`, `_work/readchilddo_read_smoke.log`
and `_work/readchilddo-read-smoke/`.

Fixed scores: Create 98.4, Read 97.9, Enhanced 97.9. Original weights remain
291/873/636, total 1,800, exactly three unique bodies. Means **98.066667%
unweighted / 97.980833% weighted** (98.07/97.98 displayed) remain below both
99.0 requirements. The goal stays active; no compiler ceiling is established.

## Continuation: caller child-loop forms after the reader loop gain

Previous turn made authoritative progress: reader source 97.6 -> 97.9,
recursive-child native fixtures and full exact-set validation, committed at
0c8a681d. This turn starts from its clean tracked state. A perfect Read score
alone with current Create 98.4 and Enhanced 97.9 would yield only 98.766667
unweighted, so improving another body is also necessary at these fixed scores.

Original Create at 0x6d236..0x6d238 initializes the counter and jumps to the
shared comparison at 0x6d2e2..0x6d2e8 before any child allocation. The same
comparison reads subCount after each child. This justifies keeping a first
guard; it does not justify applying an unguarded do/while from the earlier
excluded trials. Ten valid caller forms were measured on the current reader
state with unchanged flags, matcher, calls and three-body denominator:

| Caller child-loop form | Create source / strict | Instructions | Call match |
|---|---|---:|---:|
| Guarded do/while | 97.3 / 75.7 | 293 | 100.0 |
| Guarded do/while with common result return | 97.3 / 76.4 | 293 | 100.0 |
| Explicit while with separate counter declaration | 98.4 / 60.7 | 289 | 100.0 |
| For with bound reloaded after each child | 98.4 / 60.7 | 289 | 100.0 |
| Guarded do, counter declared inside guard | 97.3 / 75.7 | 293 | 100.0 |
| Guarded do with reloaded intermediate bound | 97.3 / 75.7 | 293 | 100.0 |
| Guarded do with unsigned-short counter | 97.1 / 75.6 | 294 | 100.0 |
| For with unsigned-short counter | 98.4 / 60.6 | 290 | 100.0 |
| Explicit shared entry/back-edge comparison labels | 98.4 / 60.7 | 289 | 100.0 |
| Shared comparison with common result return | 98.4 / 61.0 | 289 | 100.0 |

All preserve initial count checking and repeated bound reads; none is retained.
The unsigned-short counter cannot wrap on an executed increment: the previous
comparison requires its value below a 16-bit subCount, hence at most 65,534
before increment. This is an equivalent bounded experiment, not a recovered
original counter type. The explicit shared-test labels model the visible ARM
control flow without introducing extra calls or effects.

Higher strict scores of the guarded caller forms do not satisfy the requested
source objective: the main guarded form has inventory 94.9, opcodes 97.0,
control inventory 94.7, calls 100.0 and size 99.6, versus the accepted caller's
100.0 control inventory. It adds a distinct zero guard to the compiled loop.
The source improvement found in Read therefore does not transfer to Create.
Explicit while and reloaded-bound-for whole Core objects equal readchilddo-full
byte for byte. Counter-inside-guard and guarded-bound objects equal the guarded
do object. Shared-test labels have the same rounded scores as accepted Create
but a different whole Core object; equal scores are not asserted as byte identity.

Evidence: `_work/create_guarded_child_sweep.py`,
`_work/create_guard_counter_sweep.py`, `_work/create_shared_loop_test_sweep.py`,
their ten focused reports, snapshots and object comparisons. All finally blocks
restore accepted source; tracked source diff is empty and the baseline
outside-body/signature check passes. Native and full-corpus evidence therefore
remain the unchanged readchilddo-full state. Means stay 98.066667/97.980833,
below both >=99.0 gates; the goal stays active. Further search should inspect
caller operands/value storage rather than repeat these loop/return spellings.

## Continuation: caller format widths with three separated read paths

Previous turn yielded ten caller-loop measurements and distinguished strict
improvement from the required source objective. This turn starts from clean
tracked state at 492e1cb0. The earlier width trials preceded the separated
single-mesh read paths (280 compiled Create instructions then versus 289 now),
so their outcomes do not substitute for an interaction check on current code.
Original 0x6d1bc/0x6d1f8 tests masks 0x1b/0x1a; format starts at 0x1f and
only clears bits during the seven-byte signature loop, so its value stays
within 0..31. All three narrow types represent every reachable value exactly.
This justifies testing widths without claiming recovered original C++ types.

| Format/value-lifetime form | Create source / strict | Instructions | Call match |
|---|---|---:|---:|
| unsigned char format | 96.3 / 68.3 | 289 | 100.0 |
| unsigned short format | 96.3 / 68.3 | 289 | 100.0 |
| signed char format | 97.1 / 56.2 | 289 | 100.0 |
| Preserve header-word predicate as bool | 98.4 / 60.7 | 289 | 100.0 |
| Preserve enhanced predicate as bool | 98.2 / 60.7 | 289 | 100.0 |
| Preserve both predicates as bool | 98.2 / 60.7 | 289 | 100.0 |
| Preserve both masks as unsigned values | 98.2 / 60.7 | 289 | 100.0 |

The saved predicates/masks are computed inside the successful format guard,
before the subsequent header/vertex-format reads. Format is not modified after
signature comparison; AEFile reads never receive its address. Thus predicates
keep the same values, calls, arguments and short-circuit conditions. None
improves source match, and none is retained. The header-bool whole Core object
equals readchilddo-full byte for byte. The three enhanced-mask variants equal
one another. The unsigned-byte and unsigned-short objects differ despite
identical rounded scores and instruction counts; no byte identity is inferred
from those figures. Narrow types still preserve call-count match 100.0, unlike
the earlier merged-path caller state, but this does not make them winners.

A suspected field-offset lead was checked directly before editing layout:
original 0x6c654/0x6c668 uses indexCount offset 40, and 0x6c670/0x6c67a uses
indices offset 44, matching current 32-bit Mesh fields and compiled operands.
No layout correction is supported by that lead. Mesh.h's default constructor
is user-provided; allocation spelling is not evidence that constructor fields
should be reordered or manually copied. No header/helper change was made.

Evidence: `_work/create_width_separate_calls_sweep.py`,
`_work/create_format_mask_lifetime_sweep.py`, seven focused reports, snapshots,
object comparisons and original_mesh.asm. Finally restorations leave tracked
source diff empty; baseline outside-body/signature checks pass. Accepted code
and full/native validation remain readchilddo-full, with exact pair sets
2,079 linked / 959 byte unchanged. Means remain 98.066667/97.980833, below
both 99.0 gates. The next search should follow remaining caller operand
load/store relationships rather than repeat these width/mask forms.

## Continuation: signature operand scheduling and per-byte masks

Previous turn produced width/mask interaction evidence on the separated caller
paths; this turn starts from clean tracked state at 28412010. Original
0x6d164..0x6d176 loads plain/V2/V3 signature bytes before the plain comparison
at 0x6d178. V4 is loaded at 0x6d180, V2 compared at 0x6d184, V5 loaded at
0x6d186; V3/V4/V5 comparisons follow at 0x6d190/0x6d198/0x6d1a0.
These primary operands motivate an interleaved-source trial, without claiming
the original compiler scheduling proves an exact C++ declaration order.

| Signature source form | Create source / strict | Instructions | Call match |
|---|---|---:|---:|
| Preloads interleaved as suggested by original operands | 98.4 / 60.7 | 289 | 100.0 |
| Preload first three signature bytes | 98.4 / 60.7 | 289 | 100.0 |
| Preload all five signature bytes | 98.4 / 60.7 | 289 | 100.0 |
| Reverse five comparison statements | 98.4 / 60.7 | 289 | 100.0 |
| Compare version signatures before plain signature | 98.4 / 60.7 | 289 | 100.0 |
| Accumulate mismatch bits, then clear format bits once per byte | 97.5 / 57.5 | 290 | 100.0 |
| Accumulate keep-mask, then intersect format once per byte | 97.5 / 57.0 | 291 | 100.0 |

Comparisons read immutable seven-element signature arrays and the same local
input byte; distinct bit-clears commute, so changing their order preserves
all resulting format values. The mismatch form clears the union of precisely
those mismatching bits; the keep form intersects their complement within 0x1f,
the invariant format domain. This changes actual data flow without adding
calls, artificial side effects or a new denominator. Neither improves score.

The first three whole Core objects equal accepted readchilddo-full byte for
byte. Reverse/version-first comparison objects differ from that object despite
equal scores; their equality is not asserted. All finally blocks restore the
accepted source, and tracked source diff is empty. Baseline outside-body and
signature comparison passes, so accepted full/native evidence and exact pair
sets remain the previously verified readchilddo-full state.

Evidence: `_work/create_signature_operand_sweep.py`,
`_work/create_signature_byte_mask_sweep.py`, seven focused JSON reports,
snapshots and object comparisons. These results rule out the measured signature
preload/order/byte-mask forms on this state, not every possible caller rewrite
or an objective compiler ceiling. Means remain 98.066667/97.980833, below
both >=99.0 requirements; the goal stays active.


## Continuation: caller completion operands and animated lifecycle fixture

Original 0x6d31a closes the file before CollectAnimationData at 0x6d326.
The timeBetweenFrames load at 0x6d32a follows Collect, the mesh load at
0x6d32c precedes float-to-longlong conversion at 0x6d32e, and the animation
load at 0x6d334 follows it. These operands motivate local-lifetime trials;
they do not prove the exact original C++ spelling.

| Caller source form | Source / strict | Instructions |
|---|---|---:|
| Explicit float rate after Collect | 98.4 / 60.7 | 289 |
| Mesh local before conversion | 98.4 / 60.7 | 289 |
| Explicit longlong start after Collect | 98.3 / 60.3 | 289 |
| Reuse mesh local after Collect | 98.4 / 60.7 | 289 |
| Ignored two-byte header word near file handle | 98.2 / 60.3 | 289 |
| Ignored header word inside format guard | 97.9 / 60.8 | 288 |
| Two-byte header array inside format guard | 97.9 / 60.8 | 288 |

All seven retain call match 100.0. Explicit-float, mesh-before-conversion and
reuse-mesh whole Core objects equal accepted readchilddo-full byte for byte.
The last two differ despite equal scores. Original Read at 0x6d1d0 stores
two bytes into the former signature buffer; their semantic meaning is not
established. The storage trials preserve the read size and ignored value.
No trial improves score; accepted production bodies were restored.

The native caller fixture now exercises a V4 single mesh with nonempty
animation through actual Create, Read and enhanced-parser bodies. Transform
boundary mocks record Collect and both range calls. Collect asserts Close
has already occurred, then changes timeBetweenFrames to 3.75f. The final
caller range must target the loaded mesh animation with start 3 and end
10000000. Thus the fixture checks load timing and truncation, not merely
whether the range function was called. Full file consumption and successful
Close/no-release lifecycle are also asserted.

Positive native Reader and caller smoke runs pass. A temporary negative
mutation in the extracted caller body hoists the time load before Collect;
it fails the lastRangeStart assertion with exit 3. The accepted include was
restored and regenerated, and the positive run passes again. Windows abort
reporting is suppressed in this fixture to keep intentional assertion failures
noninteractive. The Transform operations remain boundary mocks: this does not
prove their native implementation, the full executable ABI or a device run.

Evidence: _work/create_loaded_operand_sweep.py,
_work/create_version_storage_sweep.py, seven focused JSON reports and source
snapshots; tools/mesh_create_smoke.cpp; _work/create_loaded_smoke.log;
_work/check_loaded_time_fixture.py and _work/premature_loaded_time.log.
Outside-body/signature checks pass and production source diff is empty.
The authoritative full-corpus state remains readchilddo-full: 204 units,
4619 comparisons, linked-exact 2079 and byte-exact 959 pairs unchanged.
Means remain 98.066667 / 97.980833, below both 99.0 gates. Only the stronger
native fixture and this evidence are retained; the goal remains active.


## Continuation: enhanced field addresses/key loops and current caller guards

Twenty-four additional bounded trials start from a278ce9d. They preserve read
order, failure returns, allocation sizes, helper calls, all three caller reader
sites and the fixed denominator. No production source change is retained.

Seven enhanced local-lifetime forms declare Y/Z pointers or references either
before the bounds reads or at their respective read sites, or declare vector,
axis-key or V5 channel counters separately from the for header. All seven
produce the whole Mesh object byte-identical to readchilddo-full, with Enhanced
97.9 / 67.5 and 636 instructions. No pointer cast/layout change is needed.
Original 0x6bc26..0x6bc80 keeps Y/Z field addresses across bounds reads and uses
them for the Y/Z remap; this motivates the ordinary source trials without
claiming recovered C++ declarations.

| Enhanced key-loop group/form | Source / strict | Instructions | Whole object identical |
|---|---|---:|---|
| Vector keys: while | 97.9 / 67.5 | 636 | yes |
| Vector keys: guarded do | 97.1 / 69.0 | 637 | no |
| Axis keys: while | 97.9 / 67.1 | 636 | no |
| Axis keys: guarded do | 96.7 / 62.3 | 642 | no |
| Optional keys: while | 97.9 / 67.5 | 636 | yes |
| Optional keys: guarded do | 97.7 / 66.9 | 637 | no |
| V5 signed keys: while | 97.9 / 67.5 | 636 | yes |
| V5 signed keys: guarded do | 97.7 / 67.4 | 637 | no |

Each guarded do checks the initial positive count before its first iteration;
unsigned zero and signed V5 negative counts remain empty. The same count is
retested after each increment, and unknown-section handling is unchanged.
Original axis-key entry jumps to the shared count comparison at 0x6bcb8 /
0x6bd4a..0x6bd50; separate guarded-do source is a heuristic alternative, not
an exact recovery of that original control flow. These four do forms reduce
source score. An increase in strict score for vector keys does not satisfy
the source-shape objective and is not accepted.

Five caller forms revisit early input guards on the current separated-call
state: combined early return; engine then path; path then engine; combined
return with enhanced failure guard; combined return with plain failure guard.
All yield Create 98.4 / 60.7, 289 instructions, and whole Core objects identical
to readchilddo-full. The invalid-input -4 return and valid-path lifecycle remain
unchanged. This rules out these interactions on the current body, not only on
the older merged-call implementation where an early-return trial was measured.

Four reader forms use a Vector-typed accumulator with component zero stores,
a Vector numerator output with the existing Y/X/Z write order, unsigned-short
triangle indices, or both accumulator/output forms. Allocation remains
vcount*12 with operator new[], and the three Vector += calls and math order
remain unchanged. Original 0x6cd9c..0x6cda4 writes the computed Y/X/Z values
into the temporary vector storage before the first += call. All four whole
Core objects equal readchilddo-full byte for byte: Reader 97.9 / 56.0,
864 instructions. The typed spelling does not produce an ARM improvement.

All 24 trials have call-count match 100.0. Evidence:
_work/enhanced_operand_lifetime_sweep.py (final corrected declarations),
_work/enhanced_key_loop_sweep.py, _work/create_early_guard_current_sweep.py,
_work/read_typed_accumulator_sweep.py, focused reports and source snapshots,
and _work/current_operand_trial_identities.json. Finally blocks restore accepted
sources; baseline outside-body/signature checks pass. Existing full validation
remains authoritative because production source is unchanged. Exact pair sets
remain 2079 linked / 959 byte, and means remain 98.066667 / 97.980833. Both
99.0 gates are unmet. No compiler ceiling is established; the goal stays active.


## Continuation: reader bound copies, stream counts and shared scratch

Current tracked state starts clean at 7a15daf4. Sixteen ordinary source trials
use the same canonical ARM flags, helpers, verifier and three-body denominator.
All production source is restored; no trial improves the requested metric.

| Reader form | Source / strict | Instructions |
|---|---|---:|
| Copy maximum as whole Vector | 97.7 / 55.4 | 866 |
| Copy minimum as whole Vector | 97.8 / 55.1 | 865 |
| Copy both bound vectors | 97.7 / 55.7 | 867 |
| Subtract direct bound-array views | 97.6 / 56.5 | 858 |
| Pass temporary diagonal directly to Length | 97.9 / 56.0 | 864 |
| Independent per-stream uint counts | 97.9 / 56.0 | 864 |
| Independent per-stream ushort counts | 97.9 / 56.0 | 864 |
| Independent per-stream const uint counts | 97.9 / 56.0 | 864 |
| Tangent-only ushort count | 97.9 / 56.0 | 864 |
| Shared center/triangle float scratch | 97.8 / 55.8 | 869 |
| Shared center/final tangent float scratch | 97.8 / 56.7 | 869 |
| Shared center/triangle/final tangent scratch | 97.8 / 56.6 | 869 |
| Keep min/max arrays in function scope | 97.8 / 56.5 | 863 |

Whole-vector copies preserve the extrema values and full max-minus-min
diagonal. Direct views remove copies without changing the verified read-only
math helper semantics. All five retain the required assignment, subtraction
and Length calls, and all thirteen reader trials retain call match 100.0.
Only the temporary-diagonal form and the four count forms give whole Core
objects identical to readchilddo-full. Equal scores alone are not used as
identity proof.

The count variants keep each useful vertexCount load at its existing point.
The compressed-normal loop still reloads the slot count in its condition;
its unused captured count is removed in the per-stream forms. Every field is
unsigned short, so the narrower local holds the complete value, and all
allocation lengths remain unchanged. The shared scratch variants retain the
center, triangle and final tangent writes and their helper calls in order.
The actual helpers consume these vectors synchronously. Original temporary
storage at 0x6cd9c..0x6cda4 and normal copy at 0x6cde2..0x6cdf0 motivates
examining phase reuse, without establishing a shared original C++ declaration.
These variants change stack lifetime/layout but do not improve score.

| Enhanced completion/cleanup form | Source / strict | Instructions | Call match |
|---|---|---:|---:|
| Failure block before labeled successful finish | 97.9 / 67.5 | 636 | 100.0 |
| Common release, result selected before cleanup | 96.9 / 68.6 | 638 | 96.2 |
| Common release with successful fallthrough | 96.9 / 68.6 | 638 | 96.2 |

The first completion form keeps the same scores and instruction count, but
its whole Mesh object differs from readchilddo-full. Both common-release forms
preserve one destructor/delete
pair per empty or failed parse and attach only populated successful animation;
they merge static cleanup call sites and lower source match. No shared-cleanup
rewrite is retained, and no native runtime equivalence beyond the inspected
control paths is claimed for these rejected candidates.

Evidence: _work/read_bounds_copy_sweep.py,
_work/read_stream_count_lifetime_sweep.py, _work/read_scratch_lifetime_sweep.py,
_work/enh_completion_layout_sweep.py, sixteen focused JSON reports/snapshots,
and _work/bounds_count_completion_trial_identities.json. All finally blocks
restore accepted source. Outside-body/signature checks pass and tracked
production source diff is empty. The preceding readchilddo-full 204-unit
validation and native tests remain authoritative; no new implementation
requires a full rebuild. Exact pair sets remain 2079 linked / 959 byte,
with means 98.066667 / 97.980833. Both 99.0 gates remain unmet. The next search
should exclude these concrete bound-copy/count/scratch/completion forms;
these measurements do not establish an objective compiler ceiling.


## Continuation: compressed color destination field reload

Start state is clean tracked 4084c6af. Original Android 0x6ced0 reads
colors from the captured mesh at offset 12 for every converted component;
0x6ced2..0x6ceda adds the separate byte offset and stores the normalized value.
Retained source replaces the once-captured colorFloats pointer with
((float *) m->colors)[i] inside the compressed color loop. The unused pointer
declaration is removed. Allocation, file-read order, unsigned byte conversion,
division by 255.0f, loop condition and raw-buffer release are unchanged.
The original operands support field reload timing, not recovered C++ spelling.

The focused new object has ldr r3, [r0, #12] inside the component loop,
followed by address addition and vstr. This is the actual field-reload change;
the original is not a simple persistent destination pointer increment.
The count-load placement still differs. No fake effects, barriers, volatile,
assembly, flags, helpers or layout changes are introduced.

Eighteen measured variants:

| Trial | Source / strict | Instructions | Call match | Whole object equals previous |
|---|---|---:|---:|---|
| readchildguardlife-child_before_count | 97.8 / 55.7 | 868 | 100.0 | no |
| readchildguardlife-child_function_scope | 97.8 / 55.7 | 868 | 100.0 | no |
| readchildguardlife-child_inside_nonempty_guard | 97.9 / 55.6 | 863 | 100.0 | no |
| readchildguardlife-short_child_counter | 97.9 / 55.9 | 865 | 100.0 | no |
| readchildguardlife-signed_child_counter | 97.9 / 56.0 | 864 | 100.0 | yes |
| readcolorfunnel-bool_funnel | 96.9 / 54.2 | 862 | 98.2 | no |
| readcolorfunnel-local_combined_guard | 97.9 / 55.6 | 862 | 100.0 | no |
| readcolorfunnel-read_count_funnel | 97.0 / 52.1 | 869 | 98.2 | no |
| readcolorfunnel-signed_status_funnel | 96.9 / 50.9 | 856 | 98.2 | no |
| enhsectiondispatch-all_three_if | 97.7 / 67.0 | 636 | 100.0 | no |
| enhsectiondispatch-position_if | 97.8 / 53.5 | 636 | 100.0 | no |
| enhsectiondispatch-rotation_if | 97.8 / 67.3 | 636 | 100.0 | no |
| enhsectiondispatch-scale_if | 97.8 / 67.3 | 636 | 100.0 | no |
| readcolorstore-byte_offset_field | 97.9 / 56.6 | 873 | 100.0 | no |
| readcolorstore-copy_field_byte_offset | 97.8 / 56.4 | 873 | 100.0 | no |
| readcolorstore-copy_field_captured_count | 97.8 / 56.5 | 873 | 100.0 | no |
| readcolorstore-copy_field_dynamic_count | 97.9 / 56.1 | 874 | 100.0 | no |
| readcolorstore-field_each_store | 98.0 / 55.9 | 867 | 100.0 | no |

Only field_each_store improves source match, from Reader 97.9 / 56.0 /
864 instructions to 98.0 / 55.9 / 867. Its slight strict-score decrease is
reported; the objective is source-shape. The guarded child-loop variants are
retested on the accepted guarded-do state; earlier declaration experiments
used a for loop. Signed counter is byte-identical; unsigned-short count and
pointer lifetime variants are not. All preserve the initial zero-count guard.

The color funnel variants preserve read success/failure and prevent enhanced
reads after failed color input. Their common returns merge one static call
site and lower call/source match, so they are rejected. The local combined
guard also fails to improve. Section-dispatch if/else variants preserve
0/1 dispatch and unknown-type fallthrough, but regress Enhanced. Separate
byte-offset and memcpy color-store forms preserve conversion and component
ordering but do not improve source match. None of these is retained.

Fresh validation of the retained source:

- Native UCRT64 gof2 static-library build passes.
- Reader color/position/tangent/truncation smoke passes, including eight
  unsigned RGBA components for two vertices and incomplete file prefixes.
- Caller lifecycle, animated completion, children/bounds/early-failure smoke
  with actual Mesh.cpp parser, selected AEMath bodies and Vector code passes.
- Separate actual Mesh.cpp enhanced-parser smoke passes.
- Baseline comparison confirms no changes outside the three allowed bodies
  or their signatures.
- All 204 ARM translation units rebuilt; full verifier compares 4619 rows,
  4274/4524 originals, skips zero, missing 250 (7 wrong type, 243 absent),
  extra 130. Actual normalized pair sets equal the clean baseline: linked
  2079 and byte 959, lost [] and gained [] for each.
- The other 202 objects are byte-identical to baseline. Core equals the
  successful focused field-reload object; Mesh equals angle-full.

Full strict/source averages: 75.98 / 88.99. Full report
SHA-256: `250d29bf804fdea50d7012de57eb17cf6f2bddee3fc7e0004ebc6a52a413eb50`.

Evidence: _work/read_guarded_child_lifetime_sweep.py,
_work/read_color_completion_funnel_sweep.py, _work/enh_section_dispatch_sweep.py,
_work/read_color_store_sweep.py; eighteen focused reports and snapshots;
_work/colorfield_trial_identities.json; _work/colorfield_full.json and
_work/colorfield_three.json; _work/colorfield_full_build.log and
_work/colorfield_full_verify.log; _work/colorfield_native.log,
_work/colorfield_read_smoke.log, _work/colorfield_enhanced_smoke.log.

| Fixed unique body | Source-shape | Strict | Original weight |
|---|---:|---:|---:|
| MeshCreateFromFile | 98.4% | 60.7% | 291 |
| MeshReadData | 98.0% | 55.9% | 873 |
| ReadEnhancedDataFromFile | 97.9% | 67.5% | 636 |

Exactly these three bodies give **98.100000% unweighted / 98.029333% weighted**.
Both >=99.0 gates remain unmet; the goal stays active. There is no demonstrated
compiler ceiling. Native library and bounded body tests do not establish full
executable ABI correctness or an Android-device runtime result. No push, PR
or merge is performed.


## Continuation: color bound, position predicates and enhanced entry locals

Start state is clean tracked 64082f6c with validated means 98.100000 /
98.029333 and the per-component colors-field load retained. Thirteen trials
keep canonical flags, helpers, signatures and the fixed three-body denominator.
None improves source score; all changes are restored.

| Trial | Source / strict | Instructions | Whole object equals colorfield-full |
|---|---|---:|---|
| readcolorbound-captured_component_count | 97.9 / 56.9 | 873 | no |
| readcolorbound-captured_vertex_count | 97.9 / 56.9 | 873 | no |
| readcolorbound-signed_component_count | 97.9 / 56.9 | 873 | no |
| readcolorbound-slot_component_count | 97.9 / 56.9 | 873 | no |
| readpositionpredicate-direct_shift_negative | 97.7 / 55.9 | 873 | no |
| readpositionpredicate-shared_shift_negative | 97.7 / 55.9 | 873 | no |
| readpositionpredicate-signed_compressed_local | 97.7 / 55.9 | 873 | no |
| readpositionarms-integer_float_before_short | 98.0 / 55.9 | 867 | yes |
| readpositionarms-integer_float_before_short_signed | 97.7 / 55.9 | 872 | no |
| enhentrylife-both_after_allocation | 97.9 / 67.5 | 636 | yes |
| enhentrylife-format_after_allocation | 97.9 / 67.5 | 636 | yes |
| enhentrylife-self_after_allocation | 97.9 / 67.5 | 636 | yes |
| enhentrylife-signed_format_current | 97.9 / 67.5 | 636 | yes |

All thirteen have call-count match 100.0. Original 0x6cebe..0x6cec2 captures
four times the vertex count before the compressed-color loop. Four ordinary
forms use the captured vertex count or an unsigned/signed component count,
loaded through m or slot at the existing pre-loop point. Each keeps the
per-component colors-field load and component division, but gives 97.9,
below accepted 98.0. Signed counters stay within [0,262140], so no overflow
or negative loop limit is introduced. Equal scores do not imply byte identity;
the identity column is based on actual whole-object comparisons.

Original 0x6c6a8 shifts flags by 29, and 0x6c6ba branches with BMI for
short position data. Direct or shared signed tests of flags << 29 select exactly
bit 4 on the supported two-complement ARM/native compilers. The existing
unsigned shift prevents signed left-shift overflow. All three give 97.7.
Inverting the top bit-test and placing integer/float decoding before the short
else arm preserves bit-4 precedence, all reads and extrema updates. The
ordinary bit-test variant remains 98.0; using the signed shared shift gives
97.7. No branch order is claimed as recovered C++.

Enhanced entry trials place the format cast, the this alias, or both after
new Transform, preserving the allocation/constructor and first-read order.
A signed-byte format is also retested on the accepted current body; only
bits 0x18/0x10 are observed, so its sign does not change these predicates.
All four retain Enhanced 97.9 / 67.5 and 636 instructions. Allocation size,
exception cleanup and helper bodies are untouched.

Evidence: _work/read_color_captured_bound_sweep.py,
_work/read_position_predicate_sweep.py, _work/read_position_arm_order_sweep.py,
_work/enh_entry_local_lifetime_sweep.py; thirteen focused reports/snapshots;
_work/color_bound_position_entry_identities.json. Finally blocks restore
accepted code. Outside-body/signature checks pass and tracked production
source diff is empty. Full colorfield-full validation and native tests remain
authoritative because no production change is retained. Exact pair sets remain
2079 linked / 959 byte with no losses; means stay 98.100000 / 98.029333.
Both >=99.0 gates remain unmet, and no objective compiler ceiling is shown.
The goal stays active; future search should exclude these measured forms.


## Continuation: extrema comparisons, coordinate counters and normal fallback

Start state is clean tracked 9180f22c. Eleven focused reader trials use the
same canonical flags, helpers, signatures and fixed denominator. None improves
accepted source 98.0, and no production source change remains.

| Trial | Source / strict | Instructions | Whole Core object equals colorfield-full |
|---|---|---:|---|
| readextremapred-maximum_update_first | 98.0 / 54.9 | 867 | no |
| readextremapred-reverse_and_maximum_first | 98.0 / 54.9 | 867 | no |
| readextremapred-reverse_comparison_operands | 98.0 / 55.9 | 867 | yes |
| readposcountercurrent-both_remaining | 98.0 / 55.6 | 867 | no |
| readposcountercurrent-integer_remaining | 98.0 / 55.6 | 867 | no |
| readposcountercurrent-integer_signed_index | 98.0 / 55.9 | 867 | yes |
| readposcountercurrent-short_remaining | 98.0 / 55.9 | 867 | yes |
| readposcountercurrent-short_signed_index | 98.0 / 55.9 | 867 | yes |
| readnormalfallback-common_xyz_output | 98.0 / 55.9 | 867 | no |
| readnormalfallback-local_fallback_copy | 98.0 / 52.9 | 874 | no |
| readnormalfallback-static_fallback_copy | 98.0 / 52.9 | 874 | no |

All call-count matches remain 100.0. The first three forms reverse the
comparison operands, update maximum before minimum, or combine these changes
in both coordinate-conversion loops. Ordered comparisons preserve the same
NaN-false behavior; min/max arrays are disjoint and the updates commute.
Original 0x6c74c..0x6c76c compares current extrema against the converted
component. This motivates testing predicate spelling, without claiming exact
original C++. No arithmetic, limits or bounds-diagonal behavior is changed.

Original integer coordinates decrement the remaining component count at
0x6c724 and test it at 0x6c778; the short path also uses a remaining count.
The five counter trials independently test short/integer remaining loops,
both together, or signed indices in each loop. Every count is at most
3*65535 = 196605, so signed index and bound casts cannot overflow. Source
still indexes the same component and uses the same quotient-derived axis.
This revisits the earlier both-loop remaining-counter form on a materially
different accepted reader containing the guarded child loop, full diagonal,
normal copy and per-component color field load. No new divisor/helper is added.

The fallback forms copy the fixed positive-zero/one/zero triplet from a local
or static const float array, or select XYZ locals in the length branch and
write all three at one common exit. They preserve sqrtf, lower/upper clamps,
normal-field reload and component store order. Two copy forms increase the
instruction count without improving source match; the common-output form
also fails to improve. No alternative is retained.

Evidence: _work/read_extrema_predicate_sweep.py,
_work/read_position_counter_current_sweep.py, _work/read_normal_fallback_sweep.py;
eleven focused JSON reports and snapshots;
_work/extrema_position_fallback_identities.json. Finally blocks restore the
accepted source; baseline outside-body/signature checks pass and tracked
production diff is empty. The full colorfield-full rebuild, actual exact-pair
set comparisons and native tests remain authoritative. Means remain
98.100000 / 98.029333, pairs 2079 linked / 959 byte with no losses. Both
>=99.0 gates are unmet; the goal remains active. These concrete exclusions do
not establish an objective compiler ceiling.


## Continuation: caller header guards, initialization and count dispatch

Current tracked start is clean fbd4b187. Fifteen ordinary caller source forms
use canonical flags, unchanged helpers and signatures, and all three reader
call sites. No variant improves accepted Create source 98.4; production
source is restored after each sweep.

| Trial | Source / strict | Instructions | Whole Core object equals colorfield-full |
|---|---|---:|---|
| createheaderguards-direct_mesh_read_address | 98.4 / 60.7 | 289 | yes |
| createheaderguards-header_combined_failure_guard | 98.4 / 60.7 | 289 | yes |
| createheaderguards-header_first_read_failure_guard | 98.4 / 60.7 | 289 | yes |
| createheaderguards-header_separate_failure_guards | 98.4 / 60.7 | 289 | yes |
| createinitbounds-both_list_initialization | 98.4 / 60.7 | 289 | yes |
| createinitbounds-child_bound_argument_first | 98.4 / 60.7 | 289 | yes |
| createinitbounds-child_list_initialization | 98.4 / 60.7 | 289 | yes |
| createinitbounds-material_before_output_store | 98.4 / 61.0 | 289 | no |
| createinitbounds-parent_bound_argument_first | 98.4 / 60.7 | 289 | yes |
| createinitbounds-top_list_initialization | 98.4 / 60.7 | 289 | yes |
| createopenfailure-typed_delete_guarded | 98.4 / 60.7 | 289 | yes |
| createopenfailure-typed_delete_implicit_null_guard | 98.4 / 60.7 | 289 | yes |
| createcountdispatch-single_then_multi_else | 97.0 / 56.8 | 290 | no |
| createcountdispatch-single_then_multi_failure_guard | 97.1 / 57.1 | 290 | no |
| createcountdispatch-strict_greater_count | 98.4 / 60.7 | 289 | yes |

All fifteen retain call match 100.0. Header forms pass the same offset-zero
vertexFormat destination as a Mesh pointer, split the short-circuit condition
into individual failure guards, combine the failures, or separate only the
ignored two-byte read. The exact read order, mask 0x1b condition, global-time
reset and all failure release/Close behavior remain intact. No field offset,
header meaning or original C++ syntax is inferred from these spellings.

Initialization forms use list initialization for the top mesh, child, or both.
Mesh.h was inspected: its default constructor is user-provided and its data
members are scalar/union fields, so these forms call the same constructor,
without an added aggregate zero-fill. Another form places material assignment
before the output-slot store; both writes finish before OpenRead, and fresh
allocated Mesh storage is disjoint from a valid caller output slot. This form
only changes strict match and is not retained. Named child/parent bounds
addresses in either evaluation order preserve the same Merge arguments and
its position after the child reader and before ArrayAdd.

Open-failure forms use typed delete with explicit or implicit null guard.
Mesh has an implicit trivial destructor; no owning destructor/helper is
introduced. Both preserve deletion before clearing the output, and no file
Close is added on failed OpenRead. No such deallocation rewrite is retained.

Count dispatch tests subCount >1 in place of >=2, or places the <2 single
reader branch before the multi branch with else/failure-guard structure.
All preserve the initial child-loop test, the separate enhanced ==1 and plain
!=-1 success predicates, and failure cleanup. The reordered forms reduce
source match despite preserving three static reader call sites. They are
heuristic source alternatives, not claimed recovered original C++.

Evidence: _work/create_header_guard_sweep.py,
_work/create_initialization_bounds_sweep.py,
_work/create_open_failure_delete_sweep.py, _work/create_count_dispatch_sweep.py;
fifteen focused reports/snapshots and
_work/create_header_initialization_dispatch_identities.json. Finally restorations
leave tracked production diff empty, and baseline outside-body/signature
checks pass. Existing full colorfield-full ARM/native validation remains
applicable: exact pair sets 2079 linked / 959 byte unchanged, and means
98.100000 / 98.029333. Both >=99.0 gates remain unmet; no objective compiler
ceiling is demonstrated. The goal remains active.


## Continuation: zero default V5 channel value

Start state is clean tracked ffe95b59. Five focused V5 channel-value forms
retain the same masks, file reads, InsertKeyFrame call and hasAnimation store.
The zero-initialized channel form improves Enhanced from source 97.9 / strict
67.5 / 636 raw instructions to source 98.0 / strict 66.6 / 638. The strict
score decrease is explicit; the requested objective is source-shape.

Retained source sets unsigned int channel = 0 before the switch and leaves
that value in the default case. Cases 0/1/2/3/6 still assign their masks and
case 6 still applies the exact recovered float literal's angular conversion.
The loop index can only be 0..6; positions 4/5 therefore still pass channel 0,
as in the original V5 switch branches selected by a TBB jump table.
This is not an original mask-value array. This is ordinary default-value source
construction, not an added artificial effect or recovered C++ spelling.
Other cases replace the initial value with their existing constants. Read,
conversion, failure cleanup and animation attachment behavior are unchanged.

| V5 channel form | Source / strict | Instructions | Call match |
|---|---|---:|---:|
| Initial zero, default leaves it unchanged | 98.0 / 66.6 | 638 | 100.0 |
| Signed int channel | 97.9 / 67.5 | 636 | 100.0 |
| Unsigned long long channel | 97.9 / 67.5 | 636 | 100.0 |
| Signed long long channel | 97.9 / 67.5 | 636 | 100.0 |
| Rotation calculation before channel assignment | 97.9 / 67.5 | 636 | 100.0 |

The four rejected forms produce whole Mesh objects byte-identical to the
preceding colorfield-full object. The retained object's measured components
are instruction inventory 96.0, opcode inventory 97.5, control inventory 96.6,
ordered control 75.2, call count 100.0, normalized size 100.0. The unchanged
matcher normalizes dual-word operations before source scoring; raw instruction
counts alone do not determine its normalized-size component. No flags, helpers,
ABI layouts, denominator, assembly, volatile or barriers are changed.

Fresh validation of the retained source:

- Native UCRT64 gof2 static-library build passes.
- Actual Mesh.cpp enhanced-parser smoke passes: all seven V5 channel masks,
  both zero defaults, exact angular literal/value, signed counts, optional
  sections, empty/populated animation and truncated V5 prefixes are covered.
- Verbatim reader with actual selected AEMath/Vector code passes color,
  position, tangent and truncation fixtures. Caller with actual Mesh.cpp
  parser and selected bounds code passes lifecycle, animated completion,
  child bounds and early-failure fixtures.
- Baseline outside-body/signature checks pass.
- All 204 ARM translation units rebuilt. Full corpus has 4619 comparisons,
  4274/4524 original bodies compared, zero skipped, 250 missing (7 wrong type,
  243 absent), and 130 extra symbols.
- Actual normalized pair sets equal clean baseline: linked 2079 / byte 959,
  lost [] and gained [] for each. The other 202 objects are byte-identical
  to baseline; Core equals colorfield-full, Mesh equals the focused winner.

Full strict/source averages 75.98 / 88.99. Report SHA-256:
`ad5544a59714c3ffa0f3b22cfba5e2b58779176523d9351f1bd704f6aebfef19`.

Evidence: _work/enh_channel_value_sweep.py, five focused JSON reports/snapshots,
_work/channelzero_trial_identities.json; _work/channelzero_full.json,
_work/channelzero_three.json, _work/channelzero_full_build.log,
_work/channelzero_full_verify.log; _work/channelzero_native.log,
_work/channelzero_read_smoke.log, _work/channelzero_enhanced_smoke.log.

| Fixed unique body | Source | Strict | Original weight |
|---|---:|---:|---:|
| MeshCreateFromFile | 98.4% | 60.7% | 291 |
| MeshReadData | 98.0% | 55.9% | 873 |
| ReadEnhancedDataFromFile | 98.0% | 66.6% | 636 |

Exactly these three bodies give **98.133333% unweighted / 98.064667% weighted**.
The required two >=99.0 means are still unmet; the goal remains active.
No objective compiler ceiling is established. Native library and bounded
body tests do not prove full native executable ABI correctness or an Android
runtime test. No push, PR or merge is performed.


## Continuation: zero-channel lifetime, types and V5 key-loop interaction

Current tracked start is clean aee9d0c4 with the validated default channel
initializer. Twelve focused variants preserve the same masks, data reads,
angular conversion, helper calls and hasAnimation store. None improves source
98.0; all production changes are restored.

| Trial | Enhanced source / strict | Instructions | Whole Mesh object equals channelzero-full |
|---|---|---:|---|
| enhchannelzerolife-channel_before_key_read | 98.0 / 66.6 | 638 | yes |
| enhchannelzerolife-channel_outside_key_loop | 97.6 / 66.3 | 631 | no |
| enhchannelzerolife-no_explicit_default | 98.0 / 66.6 | 638 | yes |
| enhchannelzerolife-rotation_separate | 96.8 / 64.8 | 632 | no |
| enhchannelzerotypes-rotation_case_first | 98.0 / 66.6 | 638 | yes |
| enhchannelzerotypes-scalar_cases_reverse_order | 98.0 / 66.6 | 638 | yes |
| enhchannelzerotypes-signed_channel_zero | 98.0 / 66.6 | 638 | yes |
| enhchannelzerotypes-wide_signed_channel_zero | 98.0 / 66.6 | 638 | yes |
| enhchannelzerotypes-wide_unsigned_channel_zero | 98.0 / 66.6 | 638 | yes |
| enhchannelzerokeyloop-positive_guard_do | 97.8 / 66.7 | 639 | no |
| enhchannelzerokeyloop-positive_guard_for | 97.8 / 66.4 | 641 | no |
| enhchannelzerokeyloop-short_key_counter | 98.0 / 66.7 | 639 | no |

All twelve retain call match 100.0. Removing the explicit default label still
leaves initial channel 0 for positions 4/5. Separating c==6 processing from
the switch preserves rotation/mask results but lowers the source score.
Moving initialization before key reads changes no externally visible state.
Moving it outside the key loop preserves zero defaults because c is invariant
within that loop and every other case assigns its constant each iteration;
it also lowers source match. No hoisted-channel form is retained.

Signed int and signed/unsigned long long types are retested with the new
initializer, not assumed equivalent in code generation from the previous
uninitialized declaration. All masks are nonnegative and fit these types,
and the longlong callee channel arguments remain identical. Rotation-case
and scalar-case source reordering likewise preserve exclusive dispatch and
all values; none improves source score.

The V5 key-loop forms check count >0 before for/do, or use an unsigned-short
key index. Signed input count remains short; integer promotion makes the
short-index comparison signed on both supported ARM/native compilers. With
count <=32767, the loop reaches at most index 32767 before termination and
cannot wrap the unsigned-short counter. Zero/negative counts still skip all
keys, and count is still retested after each iteration. These interactions
are measured on the new zero-initializer state; the older guarded-do trial
used assignment in the default case. Both positive guards lower source
score, and the narrow counter only changes strict score/count.

Evidence: _work/enh_zero_channel_lifetime_sweep.py,
_work/enh_zero_channel_type_order_sweep.py,
_work/enh_zero_channel_key_loop_sweep.py; twelve focused reports/snapshots;
_work/channelzero_lifetime_type_loop_identities.json. Finally blocks restore
accepted source. Baseline outside-body/signature checks pass, and production
source diff is empty. Full channelzero-full ARM/native validation remains
authoritative, with actual pair sets 2079 linked / 959 byte unchanged and
means 98.133333 / 98.064667. Both >=99.0 gates remain unmet; no objective
compiler ceiling is demonstrated. The goal remains active.


## Continuation: common axis insertion and frame conversion

Start state is clean tracked 4da9f346. Nine focused Enhanced trials preserve
position/scale/rotation channel maps, the position Y sign change, file reads,
positive-minimum updates and key insertion order. No form improves accepted
source 98.0, and all production edits are restored.

| Trial | Enhanced source / strict | Instructions | Whole Mesh object equals channelzero-full |
|---|---|---:|---|
| enhaxiscommon-all_common_call | 96.7 / 64.1 | 625 | no |
| enhaxiscommon-all_zero_channel_guard | 96.7 / 64.1 | 625 | no |
| enhaxiscommon-position_common_call | 97.6 / 65.1 | 633 | no |
| enhaxiscommon-rotation_common_call | 97.6 / 66.1 | 634 | no |
| enhaxiscommon-scale_common_call | 97.6 / 66.0 | 634 | no |
| enhaxisframe-all_axis_frames | 97.8 / 64.0 | 643 | no |
| enhaxisframe-position_frame | 98.0 / 65.4 | 639 | no |
| enhaxisframe-rotation_frame | 98.0 / 66.0 | 640 | no |
| enhaxisframe-scale_frame | 98.0 / 66.0 | 640 | no |

All nine preserve compiled call-count match 100.0. Original position-axis
instructions at 0x6bcf8..0x6bd42 choose masks 1/4/2 and converge on a shared
InsertKeyFrame call; the Y path negates the key before calling. The first
four source forms select a channel in switch(axis), retain that sign change,
and call once after the switch. A default continue preserves the original
unknown-axis skip; valid loop indices remain 0..2. Three individual groups
and all three together are measured. Another all-group form initializes
channel 0 and calls only for nonzero masks, also preserving unknown skips.
Both common-all forms lower source score, despite retaining runtime insertion
behavior and compiled call counts. They are heuristic source construction,
not an exact recovery of original C++.

The remaining four forms compute an int frame from value after the key read
and before the axis switch, passing that same frame in each existing case.
Each valid loop index uses one of the original three case calls. The input
float-to-int conversion, mask arguments and key sign are unchanged for valid
data. No file or global-time operation moves. Individual groups keep the
rounded source score while changing strict score and instruction count;
all three together reduce source match. None is retained.

Evidence: _work/enh_axis_common_call_sweep.py,
_work/enh_axis_frame_value_sweep.py, nine focused JSON reports/snapshots and
_work/axis_call_frame_identities.json. Finally blocks restore accepted code;
baseline outside-body/signature checks pass and production diff is empty.
The full channelzero-full ARM/native validation remains authoritative,
including actual exact-pair equality to baseline: 2079 linked / 959 byte,
with no losses. Means remain 98.133333 / 98.064667. Both >=99.0 requirements
remain unmet; no objective compiler ceiling is established. The goal stays
active, with these common-call/frame forms excluded from future search.


## Continuation: UV and accumulator interactions

Eleven isolated reader trials were tested against the accepted channel-zero
implementation. Original compressed UV conversion reads the destination field
at 0x6c92c and selects unchanged versus inverted V at 0x6c954-0x6c956. Original
float UV conversion rereads texCoords at 0x6ca14 and retains its doubled count
from 0x6c9e2-0x6c9e4. The owner-reload form previously regressed on an older
reader; this experiment checks its interaction with the current implementation.
The triangle count is computed before the vertex-count load at 0x6cc66-0x6cc6c.
Accumulator clearing appears at 0x6cc7c-0x6cc94; its original C++ form remains
unproven. These are source-form experiments, not recovered source claims.

| Trial | Read source / strict | Instructions | Calls | Whole Core object equals accepted |
|---|---|---:|---:|---|
| compressed_if | 98.0 / 55.9 | 867 | 100.0 | yes |
| compressed_reverse_condition | 98.0 / 55.9 | 867 | 100.0 | yes |
| compressed_store_branches | 98.0 / 55.9 | 867 | 100.0 | yes |
| float_owner_reload | 97.8 / 56.5 | 870 | 98.2 | no |
| float_component_one | 97.8 / 56.3 | 870 | 98.2 | no |
| float_byte_offset | 97.8 / 56.5 | 870 | 98.2 | no |
| count_after_triangles | 98.0 / 55.9 | 867 | 100.0 | yes |
| chained_zero | 98.0 / 55.9 | 867 | 100.0 | yes |
| pointer_zero | 98.0 / 55.9 | 867 | 100.0 | yes |
| vertex_memset | 98.0 / 55.8 | 866 | 100.0 | no |
| component_loop | 96.9 / 56.5 | 854 | 98.2 | no |

No variant improves the required source score. All edits were restored. The
three compressed-V branching forms, triangle/vertex count order, chained zero
stores and per-vertex pointer stores produce byte-identical whole Core objects.
The float owner-reload and byte-offset forms also equal each other, but differ
from accepted. Equal scores alone were not used to infer object identity.

Local evidence: `_work/uvinteraction-*.json`,
`_work/accuminteraction-*.json`, `_work/uv_interaction_identities.json` and
`_work/accum_interaction_identities.json`, with saved trial source snapshots.
Only the reader body was varied; helper/header/compiler/matcher inputs were
unchanged. The tracked production sources are restored and the baseline
outside-body/signature check passes. No native or full-corpus rerun is claimed
for rejected variants. Accepted full-corpus/native evidence remains
`_work/channelzero-full` and its associated reports/logs. Fixed-three means
remain **98.13% unweighted / 98.06% instruction-weighted**, below both 99.0%
requirements. No push, PR or merge was performed.


## Continuation: triangle operands and output copy interactions

Twelve isolated reader trials were compiled with unchanged canonical inputs
against the accepted channel-zero state. Original position operand loads occur
at 0x6cd26-0x6cd58, followed by differences at 0x6cd5c-0x6cd74 and products at
0x6cd78-0x6cd98. Trial locals make those loads/differences explicit without
changing expression operand order, reciprocal, accumulation order or math
helpers. Unsigned-short index locals preserve the range of each on-disk index.
Original tangent result copies use word loads/stores at 0x6ce32-0x6ce40;
binormal copies start at 0x6ce4c. The second family tests separate four-byte
memcpy calls or one twelve-byte copy for these existing result writes, without
adding helper calls or changing headers. These are compiler source-form
hypotheses; no exact original C++ syntax is claimed.

| Trial | Read source / strict | Instructions | Calls | Whole Core object equals accepted |
|---|---|---:|---:|---|
| origin_zyx | 98.0 / 55.9 | 867 | 100.0 | yes |
| all_position_loads | 98.0 / 55.9 | 867 | 100.0 | yes |
| deltas_before_reciprocal | 98.0 / 55.9 | 867 | 100.0 | yes |
| deltas_after_reciprocal | 98.0 / 55.9 | 867 | 100.0 | yes |
| deltas_before_denominator | 98.0 / 56.0 | 867 | 100.0 | no |
| short_indices | 98.0 / 55.9 | 867 | 100.0 | yes |
| component_copy_tangent | 98.0 / 55.9 | 867 | 100.0 | yes |
| component_copy_binormal | 98.0 / 55.9 | 867 | 100.0 | yes |
| component_copy_both | 98.0 / 55.9 | 867 | 100.0 | yes |
| whole_copy_tangent | 97.8 / 55.7 | 865 | 100.0 | no |
| whole_copy_binormal | 97.8 / 56.0 | 865 | 100.0 | no |
| whole_copy_both | 97.6 / 55.9 | 863 | 100.0 | no |

None improves the requested source score. Eight produce the byte-identical
whole Core object. Computing position differences before the denominator
changes the object and improves strict score by 0.1, but source remains 98.0;
that change is not retained. Whole-vector result copies regress source. All
trial production edits were restored. Actual object bytes were compared,
not inferred from equal scores.

Fresh allocation-inventory diagnostic against `_work/channelzero-full` still
shows five missing `ldrh g0,[g1,#2]` forms versus six additional
`ldrh g0,[g0,#2]`, ten missing frame-slot loads, and paired multiply/divide
operand-role differences. These counts are diagnostic categories, not proof
of ten missing semantic loads: allocation-role normalization and address
spelling affect them. No normalizer or matcher was changed. The next search
should target counter/load/loop lowering rather than repeat equivalent
coordinate locals or output-copy spellings.

Local evidence: `_work/triangleoperands-*.json`,
`_work/outputcopyinteraction-*.json`, `_work/triangle_operand_identities.json`,
`_work/output_copy_interaction_identities.json`, saved trial snapshots and
fresh `_work/MeshReadData_normalized_{target,base}.txt`. The baseline check
confirms unchanged signatures and all content outside the three allowed
bodies; tracked production diff is empty. No native/full-corpus rerun is
claimed for these rejected trials. Authoritative accepted native/full-corpus
evidence remains the channel-zero validation. Means remain **98.13%
unweighted / 98.06% instruction-weighted**, so both 99.0% gates remain unmet.
No push, PR or merge was performed.


## Continuation: final counters and zeroing interactions

Fourteen isolated reader variants test the final normal/tangent loop's counter
and its interaction with preceding accumulator initialization. Original final
loop decrements its remaining vertex count at 0x6ce54 and tests at 0x6ce66;
triangle index offset and count advance at 0x6cdba-0x6cdc6. Zeroing is guarded
at 0x6cc7c-0x6cc80 and lowered into clearing at 0x6cc94. Guarded do/while
variants explicitly preserve the zero-count skip. Original unsigned-short
vertex count is at most 65535 and triangle count at most 21845: unsigned-short
forward indices cannot wrap before their exit checks. The trial named
`final_short_remaining` narrows only the initial bound, retaining unsigned-int
remaining/index variables; it is not a short remaining counter. All forms
preserve component addresses and math-helper/store order. They are source
hypotheses, not recovered original C++.

| Trial | Read source / strict | Instructions | Calls | Whole Core object equals accepted |
|---|---|---:|---:|---|
| final_remaining_for | 97.0 / 55.9 | 859 | 98.2 | no |
| final_remaining_do | 96.6 / 59.4 | 864 | 98.2 | no |
| final_remaining_while | 97.0 / 55.9 | 859 | 98.2 | no |
| final_short_index | 98.0 / 55.9 | 867 | 100.0 | yes |
| final_component_offset | 98.0 / 55.9 | 867 | 100.0 | yes |
| triangle_short_index | 98.0 / 54.2 | 865 | 100.0 | no |
| both_short_indices | 98.0 / 54.2 | 865 | 100.0 | no |
| final_short_remaining | 97.0 / 55.9 | 859 | 98.2 | no |
| final_do_vertex_memset | 96.6 / 59.4 | 864 | 98.2 | no |
| final_do_guarded_do | 96.6 / 53.3 | 854 | 100.0 | no |
| final_do_remaining_zero | 96.7 / 59.6 | 869 | 98.2 | no |
| final_for_vertex_memset | 97.0 / 55.9 | 859 | 98.2 | no |
| zero_guarded_do | 97.1 / 55.2 | 851 | 100.0 | no |
| zero_remaining | 96.8 / 56.4 | 857 | 98.2 | no |

No required source-score gain occurs. All edits were restored. Final short
index and independent component offset equal the accepted whole Core object.
For/while remaining forms and narrowed initial bound equal each other; both
short indices equal triangle short index. Equal scores alone were not treated
as identity. Guarded final do/while reaches strict 59.4 but source 96.6, and
combining with remaining zeroing gives strict 59.6/source 96.7; neither can
meet the requested objective through strict gains alone.

Direct relocation/call inspection, saved in
`_work/final_remaining_do_calls.json`, counts 61 call instructions in accepted
versus 60 in the final remaining do trial. The static-symbol multiset loses
one `_ZdaPv` call. Three register-indirect calls use r4 instead of r5. This is
not evidence that an executed deallocation is missing; it is a static control
layout difference. It also corrects the tentative zeroing-call explanation:
the missing static call is delete[], not a clearing/division helper. The
combined guarded zero/final loops recover call-count score 100.0 but still
regress source to 96.6. No cleanup operation was added to manipulate the score.

Evidence: `_work/readcounterinteraction-*.json`,
`_work/counterzerointeraction-*.json`,
`_work/read_counter_interaction_identities.json`,
`_work/counter_zero_interaction_identities.json` and saved trial snapshots.
Baseline signatures/outside-body check passes and tracked production diff is
empty. Rejected variants have no claimed native/full-corpus validation;
accepted channel-zero native/full-corpus evidence remains authoritative.
Fixed-three means remain **98.13% unweighted / 98.06% instruction-weighted**,
below both 99.0% gates. No push, PR or merge was performed.


## Continuation: byte axis counters in enhanced parsing

Start state is clean tracked 50aac106. Twenty focused trials examine bounds
remap/copy/read guards, axis and V5-channel counter types, and the individual
axis sections. Retain only unsigned char for the three per-axis outer counters.
Each starts at zero, iterates exactly 0/1/2 and exits at 3, so no wrap or signed
comparison difference is possible. Inner counts, signed V5 counts, masks,
negation, file read sizes/order, InsertKeyFrame timing, failures and helper
calls stay unchanged. No header/layout, extra condition, table or effect is
introduced. This is a bounded source-type hypothesis, not recovered C++ type
information or proof that original used byte counters. Original V5 loop uses
an unsigned <=6 branch at 0x6c2d0-0x6c2d4; that independent spelling is also
tested and produces the accepted pre-change whole Mesh object.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals prior accepted |
|---|---|---:|---:|---|
| z_word_copy | 98.0 / 66.6 | 638 | 100.0 | yes |
| both_word_copy | 98.0 / 66.6 | 638 | 100.0 | yes |
| negative_before_y_store | 98.0 / 66.6 | 638 | 100.0 | yes |
| z_store_first | 98.0 / 66.6 | 638 | 100.0 | no |
| shared_read_failure_guard | 98.0 / 66.6 | 638 | 100.0 | yes |
| v5_unsigned_int | 98.0 / 66.6 | 638 | 100.0 | yes |
| v5_unsigned_short | 97.9 / 66.3 | 640 | 100.0 | no |
| v5_unsigned_char | 97.9 / 66.7 | 639 | 100.0 | no |
| v5_signed_char | 98.0 / 66.6 | 638 | 100.0 | yes |
| axes_unsigned_short | 97.6 / 66.2 | 644 | 100.0 | no |
| axes_unsigned_char | 98.3 / 66.9 | 641 | 100.0 | no |
| axes_signed_char | 98.0 / 66.6 | 638 | 100.0 | yes |
| v5_unsigned_limit_six | 98.0 / 66.6 | 638 | 100.0 | yes |
| position | 98.1 / 66.7 | 639 | 100.0 | no |
| scale | 98.1 / 66.7 | 639 | 100.0 | no |
| rotation | 98.1 / 66.7 | 639 | 100.0 | no |
| position_scale | 98.2 / 66.8 | 640 | 100.0 | no |
| position_rotation | 98.2 / 66.8 | 640 | 100.0 | no |
| scale_rotation | 98.2 / 66.8 | 640 | 100.0 | no |
| all_axes_v5_byte | 98.1 / 67.0 | 642 | 100.0 | no |

Retained Enhanced rises 98.0 -> 98.3 source and 66.6 -> 66.9 strict, with 641 raw instructions versus original 636. Its unchanged matcher components:
- instruction_inventory_match: 95.8
- opcode_inventory_match: 97.4
- control_inventory_match: 98.7
- ordered_control_flow_match: 75.2
- call_count_match: 100.0
- size_match: 99.5

The gain is specifically control-inventory improvement 96.6 -> 98.7; ordered
control stays 75.2. Instruction/opcode inventory slightly regress 96.0 ->95.8
and 97.5 ->97.4, and normalized size 100.0 ->99.5. These tradeoffs are explicit;
the required unchanged aggregate source metric rises despite them. No claim
is made that every instruction category or literal C++ source improves.

Fresh validation of the retained source:

- Native UCRT64 gof2 static-library build passes.
- Actual Mesh.cpp enhanced parser passes existing empty/populated animation,
  position-axis/vector, signed V5 count, all seven V5 masks including zero
  defaults, exact angular literal/value, optional and truncated-prefix fixtures.
- New fixture covers two keys on every one of the nine position/scale/rotation
  axes. It checks insertion order, channel, value, frame, minimum time and
  attached range. Every truncated prefix fails through cleanup; empty counts
  traverse all nine axes and attach no animation. Separate intentionally wrong
  scale/rotation channel mutations both fail with exit 3 at the new assertion.
  No negative mutation touches production source.
- Verbatim reader/actual selected math and Vector smoke passes; caller with
  actual Mesh.cpp and selected BSphere implementation passes lifecycle,
  animated Collect time/range, child bounds and early-failure fixtures.
- Baseline outside-body/signature check passes; tracked source change is only
  the three axis counter declarations in ReadEnhancedDataFromFile.
- All 204 ARM translation units rebuilt. Full corpus: 4619 comparisons,
  4274/4524 originals compared, zero skipped, missing 250 (7 wrong type,
  243 absent), extra 130.
- Actual normalized pair sets equal clean baseline: linked 2079 / byte 959,
  lost [] and gained [] for each. Other 202 whole objects equal baseline.
  Core equals prior accepted channelzero-full; Mesh equals the focused winner.
  Canonical flags digest is unchanged.

Full strict/source averages 75.98 / 89.0. Full JSON SHA-256:
`daff54ce138a01065492dea9b27e4fad03271d21ab6aa51b24336572e54d7fc8`.

Evidence: `_work/enhboundscopy-*.json`, `_work/enhlooptypes-*.json`,
`_work/enhaxisbyte-*.json` and the three corresponding identity ledgers;
`_work/axisbyte_full.json`, `_work/axisbyte_three.json`,
`_work/axisbyte_full_build.log`, `_work/axisbyte_full_verify.log`,
`_work/axisbyte_native.log`, `_work/axisbyte_read_smoke.log`,
`_work/axisbyte_enhanced_build.log`, `_work/axisbyte_enhanced_smoke.log`,
`_work/axisbyte_negative_fixture_results.json` and wrong-channel logs.

| Fixed unique body | Source | Strict | Original weight |
|---|---:|---:|---:|
| MeshCreateFromFile | 98.4% | 60.7% | 291 |
| MeshReadData | 98.0% | 55.9% | 873 |
| ReadEnhancedDataFromFile | 98.3% | 66.9% | 636 |

Exactly these three unique bodies give **98.233333% unweighted / 98.170667% weighted**.
Both required >=99.0 means are still unmet; goal remains active. No compiler
ceiling is established. Native library/body tests use file/Transform mocks
and do not prove whole native executable ABI or Android device/runtime
correctness. No Godot check is used as native evidence. No push, PR or merge.


## Continuation: inner key widths and byte-axis exits

Fourteen focused enhanced-parser variants start from f8607229, preserving its
three accepted byte axis counters. Nine forms use unsigned-short inner key
indices separately for vector/axis/optional/V5 groups, all unsigned-count
groups, all groups, or one axis-key section. Unsigned count bound is at most
65535: the last iteration is index 65534, then index 65535 fails the condition
without another increment, so no wrap. V5 signed-short count remains unchanged;
integer promotion of a short unsigned index preserves negative-count skipping
and the maximum positive bound 32767. No byte inner index is used.

Five outer-axis forms test !=3, <=2, one shared unsigned-char axis variable,
and do/while with <3 or !=3. Each still visits 0/1/2 in order. Shared axis is
assigned zero at each for initializer; no uninitialized value is read. Outer
do needs no nonzero-count guard because axis section always contains three
count records even when all inner counts are zero. No read, key formula,
channel mapping, helper call or cleanup effect changes. These remain compiler
source-form hypotheses, not recovered exact types or loop syntax.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| vectors | 98.1 / 53.4 | 644 | 100.0 | no |
| axes | 98.0 / 67.1 | 645 | 100.0 | no |
| optional | 98.2 / 66.8 | 642 | 100.0 | no |
| v5 | 98.2 / 67.0 | 642 | 100.0 | no |
| all_unsigned_counts | 97.8 / 53.1 | 649 | 100.0 | no |
| all_key_indices | 97.7 / 52.9 | 650 | 100.0 | no |
| position_axis | 98.2 / 66.8 | 642 | 100.0 | no |
| scale_axis | 98.2 / 66.8 | 642 | 100.0 | no |
| rotation_axis | 98.2 / 67.2 | 643 | 100.0 | no |
| not_equal_three | 97.6 / 66.4 | 641 | 100.0 | no |
| at_most_two | 98.3 / 66.9 | 641 | 100.0 | yes |
| shared_axis | 98.3 / 66.9 | 641 | 100.0 | yes |
| do_less_three | 97.2 / 65.9 | 644 | 100.0 | no |
| do_not_equal_three | 97.2 / 65.9 | 644 | 100.0 | no |

No trial improves accepted 98.3 source. The <=2 spelling and shared outer
axis variable produce byte-identical whole Mesh objects. All remaining
objects differ from accepted; matching source scores are not treated as
identity. The two do forms also differ from each other despite equal scores.
All production edits were restored. The accepted inner widths and for loops
are retained, so no native/full corpus rerun is claimed for rejected variants.

Evidence: `_work/enhkeyshort-*.json`, `_work/enhaxisexit-*.json`, saved source
snapshots, `_work/enh_key_short_identities.json` and
`_work/enh_axis_exit_identities.json`. Baseline outside-body/signature check
passes and tracked production diff is empty. Accepted authoritative evidence
remains `_work/axisbyte_full.json` and its native/full build/check logs.
Fixed-three means remain **98.23% unweighted / 98.17% weighted**; exact pairs
remain 2079 linked / 959 byte with no losses in the accepted full validation.
Both >=99.0 requirements remain unmet; the goal is active. No push/PR/merge.


## Continuation: section type storage and typed vector keys

Eleven focused Enhanced trials start from clean c2d64ae1 and retain byte axis
counters. Seven split the section-type storage: one or all three distinct
signed-short locals, one or all lexical scopes, separate signed/unsigned
optional type, and all scoped locals plus unsigned optional type. First three
original dispatches use signed loads (position at 0x6bc88); optional ==2 can
use either signed or unsigned short without changing accepted values. Each
read still transfers two bytes and each switch/unknown fallthrough remains
unchanged. These are lifetime/source-type trials, not recovered C++ scopes.

Four further forms replace float[3] vector-key locals with existing Vector in
position, scale, rotation or all three groups. Read still transfers 12 bytes
to the local, and InsertKeyFrame receives &vec.x. Existing Vector has three
float fields and a checked size of 12; no constructor/assignment/conversion
helper call is added. Original vector-key read is at 0x6bdb6 and insertion at
0x6bdd4. No formula, frame conversion, allocation or failure cleanup changes.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| type: position_local | 97.3 / 48.9 | 641 | 100.0 | no |
| type: all_locals | 97.5 / 57.0 | 641 | 100.0 | no |
| type: position_scope | 97.9 / 50.3 | 641 | 100.0 | no |
| type: all_scopes | 97.5 / 60.9 | 641 | 100.0 | no |
| type: optional_short | 98.2 / 66.5 | 643 | 100.0 | no |
| type: optional_unsigned_short | 98.2 / 66.5 | 643 | 100.0 | no |
| type: all_scopes_optional_unsigned | 98.2 / 44.6 | 643 | 100.0 | no |
| vector: position | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector: scale | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector: rotation | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector: all_vectors | 98.0 / 55.9 | 641 | 100.0 | no |

No trial exceeds retained 98.3 source. Each individual Vector substitution
produces the accepted byte-identical whole Mesh object, while substituting all
three together produces a different object with source 98.0/strict 55.9. Thus
individual identity cannot be extrapolated to a combined source change. The
separate optional signed/unsigned type variants equal each other but differ
from accepted; all other type forms differ too. Actual object bytes, not equal
scores, establish these identities. Every production edit was restored.

Evidence: `_work/enhsectiontypes-*.json`, `_work/enhvectorlocal-*.json`,
source snapshots, `_work/enh_section_type_identities.json` and
`_work/enh_vector_local_identities.json`. Baseline outside-body/signature check
passes and tracked production diff is empty. Native/full-corpus reruns are
not claimed for rejected temporary variants; accepted axisbyte-full reports
and native tests remain authoritative. Fixed-three means stay **98.23%
unweighted / 98.17% weighted** and actual accepted pair sets stay 2079 linked /
959 byte, with no losses. Both >=99.0 means remain unmet; goal remains active.
No push, PR or merge was performed.


## Continuation: caller counter types and reader axis widths

Fourteen additional focused Core trials start from fdb95ee1. Eight caller
forms vary signed/unsigned-long format, signed/unsigned-long child index,
initial nullptr child pointer, and byte/short/unsigned signature index. Format
remains in 0..31 after its masks. Child index never exceeds the unsigned-short
subCount (maximum 65535), so signed int cannot overflow; signature index visits
exactly 0..6. No child count storage or read is widened. Pointer default is
assigned before first use on every actual child iteration. All three reader
call sites, guards, file order and failure cleanup remain present. Original
caller shared child test reloads count at 0x6d2e2-0x6d2e8; no captured replacement
bound or unguarded do/while is introduced. Source-type variants are hypotheses,
not recovered source declarations.

Six reader forms vary the local coordinate-axis remainder type for both
short/integer position paths or only one. i - 3*(i/3) is in 0..2 for every
unsigned component index, so all tested signed/unsigned byte/short/int types
preserve the index. Original quotient-derived formula and input/output/extrema
order stay unchanged. No counter is narrowed in this family.

| Group / trial | Source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| Create: signed_format | 98.4 / 60.7 | 289 | 100.0 | yes |
| Create: long_format | 98.4 / 60.7 | 289 | 100.0 | yes |
| Create: signed_child_index | 98.4 / 60.7 | 289 | 100.0 | yes |
| Create: long_child_index | 98.4 / 60.7 | 289 | 100.0 | yes |
| Create: child_pointer_default | 98.3 / 60.7 | 289 | 100.0 | no |
| Create: byte_signature_index | 98.4 / 60.7 | 289 | 100.0 | yes |
| Create: short_signature_index | 98.4 / 60.7 | 289 | 100.0 | yes |
| Create: unsigned_signature_index | 98.4 / 60.7 | 289 | 100.0 | yes |
| Read: both_unsigned_char | 98.0 / 55.3 | 867 | 100.0 | no |
| Read: both_unsigned_short | 98.0 / 55.3 | 867 | 100.0 | no |
| Read: both_int | 98.0 / 55.9 | 867 | 100.0 | yes |
| Read: both_signed_char | 98.0 / 55.9 | 867 | 100.0 | yes |
| Read: short_position_byte | 98.0 / 55.5 | 867 | 100.0 | no |
| Read: int_position_byte | 98.0 / 55.6 | 867 | 100.0 | no |

No requested source-score gain occurs. Seven caller forms and two reader
forms produce byte-identical accepted whole Core objects. Initial child
nullptr produces a different object and caller source 98.3 versus accepted
98.4; it is rejected. Both unsigned reader axis widths equal each other but
differ from accepted, with source unchanged and strict reduced. Equal source
scores are not evidence of identity. All production edits were restored.

Evidence: `_work/createcountertypes-*.json`, `_work/readaxiswidth-*.json`,
saved source snapshots, `_work/create_counter_type_identities.json` and
`_work/read_axis_width_identities.json`. Baseline outside-body/signature check
passes, tracked production diff is empty. No native/full-corpus rerun is
claimed for rejected forms. Accepted `_work/axisbyte_full.json` and its native
and full ARM logs remain authoritative: **98.23% unweighted / 98.17% weighted**,
2079 linked-exact / 959 byte-exact pairs equal baseline with no losses.
Both >=99.0 requirements remain unmet; goal stays active. No push/PR/merge.


## Continuation: indexed stream owner reload and scopes

Eight reader trials start from clean e2318429. Direct original inspection:
0x6c648 loads the Mesh for the index-format test; 0x6c654 addresses indexCount;
0x6c664 reloads *slot after its read, 0x6c668 reads indexCount from that owner,
0x6c66c allocates, and 0x6c670 stores indices through the captured owner.
0x6c672 reloads *slot for the buffer read; 0x6c688 reloads after that read,
then vertexCount is addressed at 0x6c68e. The current source captures m before
the first indexCount read. This is an unresolved operand-read-order difference;
earlier 'captured index owner' descriptions do not establish the exact original
capture point. Assembly reread alone does not prove an in-game behavioral bug
with the existing AEFile boundary, and fixtures do not model mutation of *slot
by the indexCount read. No exact source/ownership recovery is claimed.

First family reloads m before allocation, uses a separate fresh index owner,
reloads both allocation/read owners, changes allocation count to *slot directly,
or removes the temporary allocation result. Second family scopes a fresh owner
inside the indexed branch, declares common m before or after vertexCount read,
or combines scope with direct allocation assignment. They preserve byte counts,
file sequence, three reader call sites in the caller, allocation size and
failure conditions. The original reload forms remain experimental because they
regress the required source-shape score; no trial is retained.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| reload_before_allocation | 97.6 / 55.9 | 870 | 100.0 | no |
| separate_allocation_owner | 97.6 / 55.9 | 870 | 100.0 | no |
| reload_read_owner | 97.6 / 55.9 | 870 | 100.0 | no |
| direct_count_allocation | 98.0 / 55.7 | 867 | 100.0 | no |
| reload_direct_assignment | 97.6 / 55.9 | 870 | 100.0 | no |
| fresh_scoped_owner | 97.6 / 55.9 | 870 | 100.0 | no |
| vertex_owner_after_read | 97.6 / 55.9 | 870 | 100.0 | no |
| fresh_scoped_direct_assignment | 97.6 / 55.9 | 870 | 100.0 | no |

Seven fresh-capture/scope variants produce byte-identical whole Core objects
to each other, with source 97.6 versus accepted 98.0. The direct allocation
count form differs from both them and accepted and preserves source 98.0 while
strict falls 55.9 ->55.7. Object identity was checked from actual bytes. All
production trial edits were restored.

Evidence: `_work/readindexowner-*.json`, `_work/readindexscope-*.json`,
saved source snapshots, `_work/read_index_owner_identities.json` and
`_work/read_index_scope_identities.json`, plus original_mesh.asm cited above.
Baseline outside-body/signature check passes and tracked production diff is
empty. No native/full-corpus validation is claimed for these rejected forms;
accepted axisbyte-full/native evidence remains authoritative. Fixed means
remain **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked /
959 byte with no losses. Both >=99.0 requirements remain unmet; goal stays
active. No push, PR or merge.


## Continuation: position buffer lifetimes and typed arrays

Eighteen focused reader variants start from clean e32d34a4. Six position-buffer
forms use typed short/int input arrays separately or together, one raw pointer
shared across the mutually exclusive position branches, direct position stores,
or shared raw plus direct stores. Twelve further forms use typed compressed
UV/normal/color arrays separately/together, typed output position/UV/normal/
color/tangent/index arrays separately/together, or all typed inputs/outputs.
New primitive arrays have no value initializer: no zero fill or element
constructor/destructor is introduced. Existing explicit operator delete[]
cleanup calls remain in source at success and failure sites.

Bounds/count expressions still use the same owner at each allocation/read.
Short inputs use 3*vertexCount elements, int positions also 3*vertexCount,
UV 2*vertexCount shorts, color 4*vertexCount bytes. Float outputs have their
existing 2/3/4 component counts and indices use indexCount shorts. Counts are
unsigned-short-derived (maximum 65535) and multiplied sizes fit 32-bit size_t.
Original raw allocation/free and conversions motivate these representation
trials, not a claim that original declared any particular C++ array type.
Math/field read/store order and caller reader sites are unchanged in source.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| position: short_typed | 98.0 / 56.3 | 865 | 100.0 | no |
| position: int_typed | 98.0 / 56.3 | 865 | 100.0 | no |
| position: both_typed | 98.0 / 56.6 | 865 | 100.0 | no |
| position: shared_raw | 98.0 / 55.9 | 867 | 100.0 | yes |
| position: direct_position_store | 98.0 / 55.9 | 867 | 100.0 | yes |
| position: shared_raw_direct_store | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: raw_uv | 98.0 / 55.8 | 876 | 100.0 | no |
| arrays: raw_normal | 98.0 / 55.9 | 866 | 100.0 | no |
| arrays: raw_color | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: raw_all | 97.6 / 56.6 | 863 | 98.2 | no |
| arrays: output_position | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: output_uv | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: output_normal | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: output_color | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: output_tangent | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: output_index | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: output_all | 98.0 / 55.9 | 867 | 100.0 | yes |
| arrays: all_raw_and_outputs | 97.6 / 56.6 | 863 | 98.2 | no |

No variant improves required source score. Typed position inputs improve only
strict score (up to 56.6 versus accepted 55.9). Shared raw/direct destination
forms equal accepted; all seven typed-output variants also equal accepted as
whole Core objects. Typed color input equals accepted. Combining every typed
input regresses source to 97.6; typed outputs do not change that combined
object. Actual byte comparisons establish identity, not score equality.

Relocation/call inspection `_work/typed_array_all_calls.json` finds 61 static
call instructions in accepted versus 60 in all typed inputs, with exactly one
fewer `_ZdaPv` static call and no additional static symbol call. This is code
layout/call-site evidence, not proof of missing executed cleanup. No extra
cleanup or condition is added to force a count. All experiments are rejected
and all production edits restored.

Evidence: `_work/readposbuffer-*.json`, `_work/readtypedarrays-*.json`, saved
source snapshots, `_work/read_position_buffer_identities.json`,
`_work/read_typed_array_identities.json` and call inspection above. Baseline
outside-body/signature check passes; tracked production diff is empty.
No native/full-corpus validation is claimed for rejected forms. Accepted
axisbyte-full/native evidence remains authoritative: **98.23% unweighted /
98.17% weighted**, 2079 linked /959 byte pairs equal baseline with no losses.
Both >=99.0 gates remain unmet; goal active. No push, PR or merge.


## Continuation: zero-normal branches and stream flow

Nine reader trials start from clean 6e71b62a. Five vary the zero-length branch:
fallback first, zero/nonzero continue, named nonzero predicate, or one normal
field read after sqrtf but before the branch. Original sqrt/sqrtf sequence is
at 0x6caee-0x6cb04; comparison to zero at 0x6cb08, BNE to normalization at
0x6cb10, fallback at 0x6cb12-0x6cb28. Equality/inequality inversions preserve
NaN routing as well as signed zero; no >0 or finite-value assumption is added.
Continue forms preserve the for increment and stream cleanup after the loop.
The owner-before-branch trial moves only an existing field read past no call,
keeping it after sqrtf; it does not hoist across a potential library boundary.

Original normal-format/shader exits occur at 0x6ca38 and 0x6cc3a. Four further
forms put plain normal input first, use an early exit for absent normals, use
an early exit for disabled shader after normal parsing, or combine both.
The label is on the existing m=*slot continuation before colors, outside
normal/tangent local scopes. No dummy statement or effect is added. Flags,
reads, allocations, math and deallocations stay in their prior source order
on each executed path. These are source-flow hypotheses, not recovered C++.
Prior five vertex-format sign-shift experiments were reviewed and not rerun;
they had produced identical objects, so no repeated predicate sweep is claimed.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| zero branch: fallback_first | 98.0 / 55.9 | 867 | 100.0 | yes |
| zero branch: zero_continue | 98.0 / 55.9 | 867 | 100.0 | yes |
| zero branch: nonzero_continue | 98.0 / 55.9 | 867 | 100.0 | yes |
| zero branch: named_nonzero | 98.0 / 55.9 | 867 | 100.0 | yes |
| zero branch: owner_before_branch | 98.0 / 55.9 | 867 | 100.0 | no |
| stream flow: plain_stream_first | 98.0 / 55.9 | 867 | 100.0 | no |
| stream flow: outer_early_exit | 98.0 / 55.9 | 867 | 100.0 | yes |
| stream flow: shader_early_exit | 98.0 / 55.9 | 867 | 100.0 | yes |
| stream flow: both_early_exits | 98.0 / 55.9 | 867 | 100.0 | yes |

No trial improves source or strict. Seven whole Core objects equal accepted;
owner-before-branch and plain-stream-first each differ despite equal scores.
All production edits were restored. Source-score equality was not used to
infer binary identity. No native/full-corpus rerun is claimed for rejected
forms; authoritative validation remains axisbyte-full/native evidence.

Evidence: `_work/readnormalbranch-*.json`, `_work/readnormalflow-*.json`,
source snapshots, `_work/read_normal_branch_identities.json` and
`_work/read_normal_flow_identities.json`, plus original_mesh.asm. Baseline
outside-body/signature check passes and tracked production diff is empty.
Fixed-three means remain **98.23% unweighted /98.17% weighted**; accepted
exact pair sets remain 2079 linked /959 byte equal baseline without losses.
Both >=99.0 requirements remain unmet; goal active. No push, PR or merge.


## Continuation: color component traversal forms

Ten isolated reader variants start from clean 88341db1 and retain the accepted
per-component colors-field load. Original byte load/increment occurs at
0x6cec4-0x6cec6, colors reload/offset advance at 0x6ced0-0x6ced4, divide/store
at 0x6ced6-0x6ceda and unsigned back-edge test at 0x6cede-0x6cee0. Six linear
forms test signed index, independent byte offset with the existing dynamic
bound, advancing input pointer, postincrement store, named conversion before
index increment, or guarded do/while. Every count is <=4*65535=262140; signed
int index and byte offset cannot overflow. Guarded do preserves zero-count
skipping. The byte-offset form differs from earlier captured-bound trials by
retaining the current per-iteration count read.

Four further forms traverse vertices and their four color components with
nested unsigned counters, a byte inner counter, a short vertex counter, or
four explicit stores. Bounds remain ushort-derived, output values preserve
byte/255.0f (no reciprocal substitution), and each vertex's component order is
0/1/2/3. Nested loops reload vertex count per vertex instead of per component;
these are alternative source forms for valid separately allocated buffers,
not claims of exact original load order or recovered original loops. Colors
field remains the store owner read in each source assignment. File operations,
allocation sizes and failure cleanup are unchanged.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| linear: signed_index | 98.0 / 55.9 | 867 | 100.0 | yes |
| linear: dynamic_byte_offset | 98.0 / 56.0 | 867 | 100.0 | no |
| linear: input_pointer | 98.0 / 55.9 | 867 | 100.0 | yes |
| linear: postincrement_store | 98.0 / 55.9 | 867 | 100.0 | yes |
| linear: value_then_increment | 98.0 / 55.9 | 867 | 100.0 | yes |
| linear: guarded_do | 98.0 / 55.5 | 868 | 100.0 | no |
| vertex: nested_uint | 97.6 / 54.6 | 880 | 100.0 | no |
| vertex: nested_byte_component | 97.6 / 54.6 | 880 | 100.0 | no |
| vertex: nested_short_vertex | 97.6 / 54.6 | 880 | 100.0 | no |
| vertex: four_explicit_components | 96.8 / 56.0 | 901 | 100.0 | no |

No source improvement occurs. Four linear forms equal the accepted whole Core
object. Dynamic byte offset differs despite unchanged source and improves
only strict by 0.1, so is rejected. Guarded do regresses strict. All three
nested forms produce the same whole object to each other but regress source
to 97.6; explicit four-store form regresses to 96.8. Object identities were
verified from bytes. All production edits were restored.

Evidence: `_work/readcolorloop-*.json`, `_work/readcolorvertex-*.json`, saved
source snapshots, `_work/read_color_loop_identities.json` and
`_work/read_color_vertex_identities.json`. Baseline outside-body/signature
check passes and tracked production diff is empty. No native/full-corpus
validation is claimed for rejected forms. Accepted axisbyte-full/native
validation remains authoritative: **98.23% unweighted /98.17% weighted**,
2079 linked /959 byte exact pairs equal baseline without losses. Both >=99.0
requirements remain unmet; goal active. No push, PR or merge.


## Continuation: optional presence flow and V5 mask representation

Fourteen focused Enhanced variants start from 1bded230. Eight preserve the
optional/V5 nested predicates as early exits to existing continuation points,
named bool presence, or unsigned optionalCount >0. Read failure still reaches
fail; zero presence still skips all seven V5 counts. Optional type!=2 exits
to the following V5 flag/read check. Labels sit on existing statements outside
skipped local scopes; no dummy operation is introduced. All file order, signed
key counts, masks, angular conversion and attachment/cleanup remain unchanged.

Primary V5 dispatch is clarified here: 0x6c26c is TBB [pc,r0], with seven jump
offsets at 0x6c270 (1c/04/08/0c/10/10/13). Immediate-mask branches include
0x400 before TBB, 0x800 at 0x6c27a, 0x2000 at 0x6c282, 0x4000 at 0x6c28a,
zero at 0x6c290, and rotation/0x40000 at 0x6c296-0x6c2a4. This is a table of
control-flow offsets, not a memory array of channel values. The earlier
'seven-entry table' phrase is corrected above to avoid implying data recovery.

Six hypothetical equivalent representations use an actual seven-value static
mask array (unsigned int/int/long long), before or after rotation handling.
The fixed loop c=0..6 keeps lookup within bounds; cases 4/5 still give zero.
These are newly authored alternative C++ forms, explicitly not recovered mask
arrays from the original binary. No compiler/matcher input or helper changes.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| presence: v5_presence_early | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: v5_format_early | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: v5_both_early | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: v5_named_presence | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: v5_positive_presence | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: optional_type_early | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: optional_format_early | 98.3 / 66.9 | 641 | 100.0 | yes |
| presence: optional_both_early | 98.3 / 66.9 | 641 | 100.0 | yes |
| mask array: unsigned_int_lookup_first | 96.4 / 23.1 | 621 | 100.0 | no |
| mask array: unsigned_int_rotation_first | 96.3 / 23.7 | 620 | 100.0 | no |
| mask array: int_lookup_first | 96.4 / 23.1 | 621 | 100.0 | no |
| mask array: int_rotation_first | 96.3 / 23.7 | 620 | 100.0 | no |
| mask array: long_long_lookup_first | 96.4 / 23.0 | 623 | 100.0 | no |
| mask array: long_long_rotation_first | 96.3 / 23.7 | 622 | 100.0 | no |

All eight presence-flow variants equal the accepted whole Mesh object. Every
mask-array variant differs and materially regresses source (96.3-96.4 versus
98.3), consistent with replacing original branch dispatch by data lookup.
Even signed/unsigned int forms differ as whole objects despite equal scores;
identity is established only by byte comparison. All production edits were
restored. The accepted switch/guards are retained.

Evidence: `_work/enhpresenceflow-*.json`, `_work/enhmasklookup-*.json`, source
snapshots, `_work/enh_presence_flow_identities.json`,
`_work/enh_mask_lookup_identities.json` and original_mesh.asm. Baseline
outside-body/signature check passes; tracked production diff is empty.
No native/full-corpus validation is claimed for rejected variants. Accepted
axisbyte-full/native evidence remains authoritative: **98.23% unweighted /
98.17% weighted**, 2079 linked /959 byte exact pairs equal baseline without
losses. Both >=99.0 gates remain unmet; goal active. No push, PR or merge.


## Continuation: explicit zero V5 cases and combinations

Ten focused Enhanced variants start from clean 6a4e8b37. Five expose zero
channels 4/5 explicitly, assign their default zero, omit initializer when every
switch path assigns channel, place zero labels first, or use unsigned dispatch.
Original TBB routes both 4/5 offsets to 0x6c290-0x6c294, assigning both channel
words zero; cases 0/1/2/3/6 keep their masks and rotation. The no-initializer
form still assigns channel on every case/default path before insertion; it
cannot read an uninitialized channel. Fixed c=0..6 means unsigned dispatch
selects the same cases. No predicate, dummy instruction or effect is added.

Five combinations start from the explicit-zero-assignment variant: signed
32-bit, unsigned/signed 64-bit channel, angular arithmetic before its mask
assignment, or rotation before the switch. All stored masks fit their tested
integer representations. Arithmetic still uses the exact float literal and
preserves /100, *tau, /360 order. The last variant moves the existing c==6
rotation handling outside dispatch; it does not convert other channels.
These remain source-form hypotheses, not recovered original C++ labels/types.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| zero cases: explicit_zero_labels | 98.3 / 66.9 | 641 | 100.0 | yes |
| zero cases: explicit_zero_assignment | 98.3 / 67.8 | 639 | 100.0 | no |
| zero cases: zero_labels_no_initializer | 98.3 / 67.8 | 639 | 100.0 | no |
| zero cases: zero_group_first | 98.3 / 66.9 | 641 | 100.0 | yes |
| zero cases: unsigned_dispatch | 98.3 / 66.9 | 641 | 100.0 | yes |
| combination: int | 98.3 / 67.8 | 639 | 100.0 | no |
| combination: unsigned_long_long | 98.3 / 67.8 | 639 | 100.0 | no |
| combination: long_long | 98.3 / 67.8 | 639 | 100.0 | no |
| combination: arithmetic_before_mask | 98.3 / 67.8 | 639 | 100.0 | no |
| combination: rotation_before_switch | 98.0 / 65.1 | 642 | 100.0 | no |

No variant improves requested source. Explicit zero assignment (with or without
initializer) and its four channel/order combinations all yield the same whole
Mesh object to each other: strict rises 66.9 ->67.8 and raw count falls 641 ->639,
but source stays 98.3. These strict-only gains are not retained. Three remaining
zero-case forms equal accepted; rotation-before-switch differs and regresses
source to 98.0. Actual byte comparisons establish object identity; source score
equality is not taken as identity. All production edits were restored.

Evidence: `_work/enhzerocases-*.json`, `_work/enhzerocasecombo-*.json`, source
snapshots, `_work/enh_zero_case_identities.json` and
`_work/enh_zero_case_combo_identities.json`. Baseline outside-body/signature
check passes; tracked production diff is empty. No native/full-corpus run is
claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, 2079 linked /959 byte
exact pairs equal baseline without losses. Both >=99.0 requirements remain
unmet; goal active. No push, PR or merge.


## Continuation: bounds scratch ordering and extrema locals

Eleven focused reader trials start from clean af6792c4. Seven reorder min/max
array declarations or initialization, use aggregate initializers separately
or together, or put maximum aggregate first. Initial values remain +10000000
for minima and -10000000 for maxima. Original constant stores at
0x6c69c-0x6c6ac motivate local-storage tests; no original C++ declaration order
is asserted. The verified complete max-minus-min radius is unchanged. No
padding, extra values, fake condition or helper is introduced.

Four further variants use a local minimum value, sequential minimum/maximum
locals, both extrema loaded before the first comparison, or component
references. Conditional stores remain min then max with the same predicates;
no unconditional write is added. Original minimum load/comparison/conditional
store is 0x6c74c-0x6c75a and maximum follows at 0x6c75e-0x6c76c. The preloaded
variant is an equivalent alternative for distinct scratch arrays, not a
claim that it preserves the original operand-load schedule. Stream reads,
position stores, axis computation and math helpers remain unchanged.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| scratch: maximum_declared_first | 98.0 / 55.9 | 867 | 100.0 | no |
| scratch: maximum_initialized_first | 98.0 / 55.9 | 867 | 100.0 | no |
| scratch: maximum_declared_initialized_first | 98.0 / 55.9 | 867 | 100.0 | no |
| scratch: minimum_aggregate | 98.0 / 55.8 | 871 | 100.0 | no |
| scratch: maximum_aggregate | 98.0 / 56.2 | 871 | 100.0 | no |
| scratch: both_aggregates | 98.0 / 56.1 | 874 | 100.0 | no |
| scratch: maximum_aggregate_first | 98.0 / 56.1 | 874 | 100.0 | no |
| extrema: minimum_local | 98.0 / 55.9 | 867 | 100.0 | yes |
| extrema: sequential_locals | 98.0 / 55.9 | 867 | 100.0 | yes |
| extrema: both_preloaded | 98.0 / 54.9 | 867 | 100.0 | no |
| extrema: component_references | 98.0 / 55.9 | 867 | 100.0 | yes |

Every trial keeps source 98.0; none improves the requested means. Scratch
ordering/init variants all differ as whole objects despite many equal scores.
Maximum aggregate raises strict to 56.2 only and is not retained. Minimum
local, sequential locals and references equal accepted byte for byte; both
preloaded extrema differ and regress strict to 54.9. Actual bytes determine
identity. All production edits were restored.

Evidence: `_work/readboundsorder-*.json`, `_work/readextremalocals-*.json`,
source snapshots, `_work/read_bounds_order_identities.json` and
`_work/read_extrema_local_identities.json`. Baseline outside-body/signature
check passes; tracked production diff is empty. No native/full-corpus run is
claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, 2079 linked /959 byte
exact pairs equal baseline with no losses. Both >=99.0 requirements remain
unmet; goal active. No push, PR or merge.


## Continuation: section signedness and case order on byte-axis state

Ten Enhanced trials start from clean 997070f1. Six keep the original two-byte
section field but vary shared unsigned storage, signed dispatch from unsigned
storage, unsigned optional comparison, or one unsigned position/scale/rotation
dispatch. Types 0/1 select the same cases; every other raw 16-bit representation
falls through either way, while optional compares only with 2. Signed V5 key
counts are untouched. On the tested ARM/native two's-complement targets, the
signed cast form restores the original signed interpretation at the three
switches. It is not a cross-platform implementation-defined-cast guarantee.
Original signed loads (position at 0x6bc88) remain source evidence; unsigned
alternatives are equivalent-dispatch hypotheses, not recovered declarations.

Four further variants place case 0 before case 1 individually for position,
scale, rotation or all three. Each retains its braces, inner reads/loops,
channel mappings, break and unknown fallthrough. Earlier combined case-order
trial predated the retained byte axes; these trials explicitly measure the
current state and isolate individual section effects. No helper/ABI/matcher
or compiler input changes.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| signedness: unsigned_shared_type | 98.1 / 66.4 | 641 | 100.0 | no |
| signedness: unsigned_storage_signed_dispatch | 98.3 / 66.9 | 641 | 100.0 | yes |
| signedness: unsigned_optional_comparison | 98.3 / 66.9 | 641 | 100.0 | yes |
| signedness: position_unsigned_dispatch | 98.2 / 52.8 | 641 | 100.0 | no |
| signedness: scale_unsigned_dispatch | 98.2 / 66.7 | 641 | 100.0 | no |
| signedness: rotation_unsigned_dispatch | 98.2 / 66.7 | 641 | 100.0 | no |
| case order: position | 98.3 / 66.9 | 641 | 100.0 | yes |
| case order: scale | 98.3 / 66.9 | 641 | 100.0 | yes |
| case order: rotation | 98.3 / 66.9 | 641 | 100.0 | yes |
| case order: all_sections | 98.3 / 66.9 | 641 | 100.0 | yes |

No required source gain occurs. Unsigned shared type and individual unsigned
dispatches differ and regress source; signed dispatch from unsigned storage
and unsigned optional equality equal accepted. All four case orders produce
the accepted byte-identical whole Mesh object. Actual bytes establish identity;
score equality is not used instead. All production edits were restored.
The initial case-span script stopped before any production write; its brace
parser was corrected, then all four completed compile/check trials ran.

Evidence: `_work/enhtypesign-*.json`, `_work/enhcaseorder-*.json`, source
snapshots, `_work/enh_type_sign_identities.json` and
`_work/enh_case_order_identities.json`. Baseline outside-body/signature check
passes; tracked production diff is empty. No native/full-corpus validation is
claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, 2079 linked /959 byte
exact pairs equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: tangent temporary lifetimes and output pointers

Ten Read trials start from clean dbc7139d with accepted byte-axis Enhanced.
Six change the final tangent expression by inlining Dot, the multiplication,
the subtraction into Normalize, or all three; const-qualifying the two named
Vector temporaries; or copying the tangent input as a Vector instead of three
scalar floats. Dot -> multiplication -> subtraction -> Normalize -> Cross
order and output field loads after their corresponding math calls are retained.
Temporary lifetime/source expressions are hypotheses, not recovered C++.
The Vector input copy uses the existing trivial copy constructor; no new
Vector assignment call or helper change is introduced.

Four further trials reuse the tangent output pointer for binormals, qualify
both pointer variables const, address individual Vector output members, or
share a computed unsigned output offset. They preserve output load/store and
math call order; no field load is hoisted across a call, and no whole-Vector
assignment replaces the three component stores.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| temporaries: inline_dot | 98.0 / 55.9 | 867 | 100.0 | yes |
| temporaries: inline_product | 96.5 / 55.8 | 857 | 98.2 | no |
| temporaries: inline_normalize_argument | 98.0 / 55.9 | 867 | 100.0 | no |
| temporaries: inline_all | 98.0 / 55.9 | 867 | 100.0 | no |
| temporaries: const_temporaries | 98.0 / 55.9 | 867 | 100.0 | yes |
| temporaries: vector_tangent_input | 97.9 / 56.2 | 868 | 100.0 | no |
| output: reuse_pointer | 98.0 / 55.9 | 867 | 100.0 | yes |
| output: const_pointers | 98.0 / 55.9 | 867 | 100.0 | yes |
| output: vector_members | 98.0 / 55.9 | 867 | 100.0 | yes |
| output: shared_offset | 98.0 / 55.9 | 867 | 100.0 | yes |

No required source gain occurs. Inline Dot and const temporaries produce the
accepted byte-identical whole Core object. Inline Normalize argument and
inline-all have the same scores but different whole objects from accepted
and each other. Vector tangent input raises strict only and regresses source.
All four output-pointer forms are byte-identical to accepted; equality is
established from actual object bytes, not inferred from scores.

Inline product regresses source and call count. Relocation-aware inspection
finds 61 call instructions in accepted versus 60 in this trial: one fewer
static `_ZdaPv` call site, plus three indirect BLX operands changing r5 to fp.
This is static emitted call-site evidence, not proof of missing executed
cleanup or a behavior regression. The six indirect calls are outside the
matcher's direct-call comparison. Evidence: `_work/tangent_inline_product_calls.json`.

All source variants were restored. Evidence: `_work/readtangentlife-*.json`,
`_work/readtangentoutput-*.json`, source snapshots and both identity ledgers.
Outside-body/signature baseline check passes and production tracked diff is
empty. Native/full-corpus runs are not claimed for these rejected variants;
accepted axisbyte-full/native evidence remains authoritative: **98.23%
unweighted /98.17% weighted**, 2079 linked /959 byte exact pairs equal baseline
without losses. Both >=99.0 gates remain unmet; goal active. No push, PR or merge.


## Continuation: mixed axis widths and remaining-axis traversal

Ten Enhanced trials start from clean 2006e1f0. Fresh inventory inspection
against axisbyte-full shows the original axis dispatch clears bit 31 with BIC
at 0x6bcf8, 0x6be6c and 0x6bfe2. Accepted byte counters instead emit three
ANDS #127 and three UXTB operations. This is concrete instruction evidence,
not proof of original C++ counter declarations. The previously accepted
all-byte form wins aggregate source score despite those inventory differences.

Six forms change one or two axis counters back to int independently, keeping
the remaining accepted byte counters. They preserve exactly axis 0/1/2, file
read and channel order, all inner unsigned key counters and failure paths.
The prior all-int and all-byte comparison does not establish mixed results;
these six combinations explicitly measure them. No helper, ABI, compiler or
matcher input changes.

Four further forms count remaining axes 3 -> 2 -> 1 and derive axis as
3 - remaining inside the existing loop. Int and unsigned-char counters are
tested with for and unconditional do forms: exactly three axes always exist
in a type-0 section, so both visit axis 0/1/2 and exit at remaining zero with
no underflow or wrap. This is a genuine alternate traversal representation,
not a dummy operation or recovered original loop. Inner reads and stores keep
their existing sequence.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| mixed width: signed_axes_0 | 98.2 / 66.8 | 640 | 100.0 | no |
| mixed width: signed_axes_1 | 98.2 / 66.8 | 640 | 100.0 | no |
| mixed width: signed_axes_2 | 98.2 / 66.8 | 640 | 100.0 | no |
| mixed width: signed_axes_0_1 | 98.1 / 66.7 | 639 | 100.0 | no |
| mixed width: signed_axes_0_2 | 98.1 / 66.7 | 639 | 100.0 | no |
| mixed width: signed_axes_1_2 | 98.1 / 66.7 | 639 | 100.0 | no |
| remaining axes: int_for | 96.8 / 65.0 | 650 | 100.0 | no |
| remaining axes: int_do | 95.9 / 39.1 | 649 | 100.0 | no |
| remaining axes: unsigned_char_for | 96.8 / 65.0 | 650 | 100.0 | no |
| remaining axes: unsigned_char_do | 95.9 / 39.1 | 649 | 100.0 | no |

All ten regress required source score. Each mixed-width whole object differs
from accepted and from the other mixed combinations, even where scores match.
Remaining-axis int/byte for forms are byte-identical to each other; int/byte
do forms are byte-identical to each other. The two groups differ from one
another and accepted. Object identity is measured from bytes, not score equality.
No variant is retained; production files restored.

Evidence: `_work/enhmixedaxis-*.json`, `_work/enhaxisremaining-*.json`, source
snapshots and their two identity ledgers. Baseline outside-body/signature check
passes and production tracked diff is empty. Native/full-corpus validation is
not claimed for rejected variants; accepted axisbyte-full/native evidence
remains authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets
2079 linked /959 byte equal baseline without losses. Both >=99.0 gates remain
unmet; goal active. No push, PR or merge.


## Continuation: compressed stream cleanup branch placement

Six Read trials start from clean d74725dc. The five compressed streams (short
positions, integer positions, UVs, normals and colors) currently check failed
Read, delete raw temporary and return -1, then process success and delete raw
at the end. Each trial instead places the existing successful conversion and
its final delete inside Read != 0, with the existing failure delete/return in
else. Five trials isolate one stream; the sixth combines all five.

The allocation and Read remain at their existing points. A successful Read
executes the same conversion/field stores and one raw delete; a failed Read
executes only its existing raw delete and -1 return. No additional null guard,
cleanup call, status variable or allocation is added. This is branch placement
of existing effects, not a recovered original spelling. All other bodies,
helpers, ABI declarations, flags and matcher remain unchanged.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| short_position | 98.0 / 55.9 | 867 | 100.0 | yes |
| integer_position | 98.0 / 55.9 | 867 | 100.0 | yes |
| uv | 98.0 / 55.9 | 867 | 100.0 | yes |
| normal | 98.0 / 55.9 | 867 | 100.0 | yes |
| color | 98.0 / 55.9 | 867 | 100.0 | yes |
| all_streams | 98.0 / 55.9 | 867 | 100.0 | yes |

All six whole Core objects are byte-identical to accepted axisbyte-full;
actual object comparison proves this, not equal score alone. Therefore this
particular success/else rewrite offers no source-shape improvement, individually
or in combination. No variant is retained and production source is restored.

Evidence: `_work/readcleanupbranch-*.json`, source snapshots and
`_work/read_cleanup_branch_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence stays
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: interval aliases and byte-axis common calls

Nine Enhanced trials start from clean 483575e7. Four explicitly bind a const
pointer or reference to timeBetweenFrames at entry or inside the section block,
and expand each existing update into the same nested positive-value and
smaller-global-value tests through that alias. All eight update sites keep
their location and fresh value load; no global value is captured or hoisted.
The helper definition remains unchanged outside the three bodies. Entry versus
section address lifetime is a source hypothesis motivated by original reuse
of the global address (e.g. r5 at 0x6bcde..0x6bcea), not recovered C++.

Five common-call variants revisit the earlier pre-byte-axis family on the
materially different accepted counter types. Each selects the same channel
inside switch(axis), keeps position Y negation, and uses a shared subsequent
InsertKeyFrame call. The default continues the inner loop. Three individual
sections and all three together are tested, plus all sections with zero-mask
initialization/nonzero guard. All valid axes remain 0/1/2; frame conversion,
read/update order and channel values remain unchanged. The earlier exclusion
was based on int-axis state; this measured interaction excludes the family
on the currently accepted byte-axis state as well. No helper/ABI/flag/matcher
changes, new call effects or metric padding.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| interval alias: pointer_entry | 98.3 / 66.9 | 641 | 100.0 | yes |
| interval alias: pointer_sections | 98.3 / 66.9 | 641 | 100.0 | yes |
| interval alias: reference_entry | 98.3 / 66.9 | 641 | 100.0 | yes |
| interval alias: reference_sections | 98.3 / 66.9 | 641 | 100.0 | yes |
| byte-axis common call: position_common_call | 98.0 / 65.3 | 636 | 100.0 | no |
| byte-axis common call: scale_common_call | 98.1 / 66.1 | 637 | 100.0 | no |
| byte-axis common call: rotation_common_call | 98.1 / 66.3 | 637 | 100.0 | no |
| byte-axis common call: all_common_call | 97.2 / 63.9 | 628 | 100.0 | no |
| byte-axis common call: all_zero_channel_guard | 97.2 / 63.9 | 628 | 100.0 | no |

All four interval forms are byte-identical to accepted Mesh. All common-call
forms regress source. The all-common/default-continue and all-zero-guard
objects equal each other but differ from accepted. Individual common-call
objects differ from accepted and each other; equal scores are not substituted
for actual byte comparison. No variant is retained; production source restored.
The initial common-call measurement script stopped after first verification
because its hash import was missing; finally restored source. The import was
corrected and all five trials then completed successfully.

Evidence: `_work/enhintervalalias-*.json`, `_work/enhbyteaxiscommon-*.json`,
source snapshots and both identity ledgers. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: triangle coordinate views and accumulator pointers

Nine Read trials start from clean c3244d95. Four retain the current tangent
formulas, index load order and Y/X/Z numerator/product order, changing only
coordinate access through three const Vector pointers, a const Vector base
with three derived pointers, or three const Vector references; another uses
signed int locals for the three unsigned-short indices. Every 16-bit index
fits int and scaling remains <=196605. No whole-Vector copy or custom operator
assignment is added. Existing coordinate member accesses address the same
floats. These are source access/type hypotheses, not recovered declarations.

Five accumulator-pointer forms use direct cast expressions, reuse one typed
Vector pointer, declare three pointers sequentially at each call, declare all
three before the calls, or name the existing tangent input as a const reference.
Each retains the same three Vector::operator+= calls in i0 -> i1 -> i2 order.
No arithmetic reassociation or scalar accumulation replaces the existing helper.
The precomputed-pointer form retains the same accumulation buffer and indices
already established before the calls. Original calls at 0x6cdaa..0x6cdb8
support inspection of address setup, not the exact source declaration form.
Math helper definitions and all other bodies remain unchanged.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| coordinate views: vector_pointers | 98.0 / 55.9 | 867 | 100.0 | yes |
| coordinate views: vector_base | 98.0 / 55.9 | 867 | 100.0 | yes |
| coordinate views: vector_references | 98.0 / 55.9 | 867 | 100.0 | yes |
| coordinate views: signed_indices | 98.0 / 55.9 | 867 | 100.0 | yes |
| accumulator pointers: direct_expressions | 98.0 / 55.9 | 867 | 100.0 | yes |
| accumulator pointers: vector_pointer_reuse | 98.0 / 55.9 | 867 | 100.0 | yes |
| accumulator pointers: sequential | 98.0 / 55.9 | 867 | 100.0 | yes |
| accumulator pointers: before_calls | 98.0 / 55.9 | 867 | 100.0 | yes |
| accumulator pointers: named_input_reference | 98.0 / 55.9 | 867 | 100.0 | yes |

All nine whole Core objects are byte-identical to accepted axisbyte-full.
Identity is established from actual bytes, not inferred from equal scores.
These coordinate and accumulator access forms do not change ARM output under
the canonical flags, and no trial improves the required source score.
Production source was restored; none is retained.

Evidence: `_work/readtriangleviews-*.json`, `_work/readaccumpointers-*.json`,
source snapshots and both identity ledgers. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: byte-axis count lifetime and V5 zero interactions

Nine Enhanced trials start from clean a443c3d9. Seven revisit count lifetimes
on the materially changed accepted byte-axis state, rather than assume the
older int-axis measurements predict these combinations. Three share the existing
optional unsigned-short count with vector cases, axis cases or all prefix cases.
Four instead use one unsigned-short count inside a new per-group scope for
position, scale, rotation or all three separately. Each count is populated by
the same two-byte Read before use; the signed-short V5 per-channel count remains
separate. All file operations, case selection and loop bounds retain their
order and meaning. Shared physical original stack storage at sp+28 motivates
this check but does not prove recovered lexical scopes or declarations.

The position-group form keeps required source score 98.3 while changing strict
and object bytes. Two additional forms combine it with the previously measured
explicit V5 cases 4/5/default assigning channel zero, with or without an initial
zero assignment. Valid channel dispatch, rotation arithmetic, key insertion
and hasAnimation effects are unchanged. This explicitly tests an interaction
of two source-neutral forms; equal standalone scores do not establish their
combined output. No helpers, ABI, flags, matcher or denominator changes.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| byte-axis counts: vector_counts | 97.4 / 62.6 | 641 | 100.0 | no |
| byte-axis counts: axis_counts | 97.7 / 50.1 | 646 | 100.0 | no |
| byte-axis counts: all_prefix_counts | 97.9 / 65.8 | 635 | 100.0 | no |
| byte-axis counts: position_group | 98.3 / 53.3 | 640 | 100.0 | no |
| byte-axis counts: scale_group | 97.8 / 51.5 | 642 | 100.0 | no |
| byte-axis counts: rotation_group | 98.1 / 66.6 | 649 | 100.0 | no |
| byte-axis counts: all_scoped_groups | 98.1 / 51.7 | 637 | 100.0 | no |
| position count + V5 zero: explicit_zero_assignment | 98.3 / 54.0 | 638 | 100.0 | no |
| position count + V5 zero: zero_labels_no_initializer | 98.3 / 54.0 | 638 | 100.0 | no |

No required source gain occurs. Position-group count alone retains rounded
source but regresses strict; all other count forms regress source. The two
V5 combinations retain source 98.3 and improve strict from the isolated
position-group 53.3 to 54.0, still below accepted 66.9. Their whole objects
are byte-identical to one another and different from accepted. Seven count
objects differ from accepted and each other. Actual bytes establish identity,
not score equality. No trial is retained; production files restored.

Evidence: `_work/enhbyteaxiscounts-*.json`, `_work/enhpositionzero-*.json`,
source snapshots and both identity ledgers. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: caller reader-status local storage

Six Create trials start from clean 3ad835a9. Original child reader call at
0x6d30e is followed by ADDS result,#1 and BEQ failure at 0x6d312..0x6d314.
Accepted source compares the full int result with -1 directly. Four variants
store all three reader-call results in int, unsigned int, short or signed char
locals before the existing predicates. Two further forms store only the child
result in unsigned short or unsigned char, comparing against the same type's
cast of -1 (not uncast -1 after integral promotion).

Current MeshReadData returns exactly 1 or -1 on all explicit return paths;
these values remain distinct in each tested type. This bounded contract is
necessary for the narrowed forms: they are not general int-result equivalent
and are not recovered original declarations. Int and unsigned int preserve
the predicates for every int result under the target's conventional int/uint
width. Enhanced single still tests 1, plain single still excludes -1, and
child still rejects -1. Call order, failure cleanup and successful bounds/
ArrayAdd/loaded paths remain unchanged. No helper/ABI/flag/matcher changes.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| all_int | 98.4 / 60.7 | 289 | 100.0 | yes |
| all_unsigned_int | 98.4 / 60.7 | 289 | 100.0 | yes |
| all_short | 96.6 / 59.1 | 294 | 100.0 | no |
| all_signed_char | 96.8 / 59.3 | 292 | 100.0 | no |
| child_unsigned_short | 98.2 / 60.1 | 291 | 100.0 | no |
| child_unsigned_char | 98.2 / 60.2 | 290 | 100.0 | no |

Int and unsigned-int result locals yield the accepted byte-identical whole
Core object. All narrower forms regress source and differ from accepted and
each other. Child unsigned-short reaches the original raw instruction count
291 but lowers source from 98.4 to 98.2; matching size alone does not meet the
source-shape objective. Actual object bytes establish identities. All trials
are rejected and production source restored.

Evidence: `_work/createreadstatus-*.json`, source snapshots and
`_work/create_read_status_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: axis dispatch types and bounded default selection

Eleven Enhanced trials start from clean 190cc622. Six keep all accepted byte
counters and cast the three switch expressions explicitly to unsigned int,
signed char or int, or cast one unsigned group independently. Actual axes
are 0/1/2 and remain representable in all types; selection/key conversion and
InsertKeyFrame effects do not change. Original BIC #0x80000000 and accepted
ANDS #127/UXTB differences motivate expression-type inspection, not recovered
source declarations. Explicit int/uint casts are hypotheses, not matcher edits.

The signed-char dispatch whole Mesh object equals the previously validated
pre-byte-axis channelzero-full object byte for byte, SHA-256
ad36d491cbfdd267469ef7f5603777bfbfb1a020cfecac32a08c8b1ce49b5a05.
The counter declarations remain byte-sized in this trial, so the expression
cast alone reproduces that older output. A fresh inventory diagnostic shows
BIC differences disappear, while branch-shape differences remain. This does
not imply a general optimizer equivalence beyond the measured complete object.

Five further forms select the axis-0 case through default, for position,
scale, rotation or all three, plus all with signed-char dispatch. Because these
closed internal loops execute exactly 0/1/2, default selects axis 0 after the
other two cases; all file reads, Y sign, masks, calls and inner counters retain
their valid-loop behavior. This is bounded-loop equivalence only: a hypothetical
out-of-domain axis would call the axis-0 case instead of skipping. No external
axis value enters these loops, and no extra operation is added for scoring.
No helper/ABI/flag/matcher or denominator changes.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| dispatch type: all_unsigned_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| dispatch type: all_signed_char | 98.0 / 66.6 | 638 | 100.0 | no |
| dispatch type: all_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| dispatch type: unsigned_group_0 | 98.3 / 66.9 | 641 | 100.0 | yes |
| dispatch type: unsigned_group_1 | 98.3 / 66.9 | 641 | 100.0 | yes |
| dispatch type: unsigned_group_2 | 98.3 / 66.9 | 641 | 100.0 | yes |
| default axis 0: position | 97.4 / 24.3 | 639 | 100.0 | no |
| default axis 0: scale | 97.9 / 66.5 | 637 | 100.0 | no |
| default axis 0: rotation | 98.0 / 66.9 | 638 | 100.0 | no |
| default axis 0: all | 96.7 / 24.0 | 633 | 100.0 | no |
| default axis 0: all_signed_dispatch | 96.9 / 24.0 | 630 | 100.0 | no |

Five int/unsigned dispatch forms are byte-identical to accepted. Signed-char
regresses source to 98.0 and matches the old object as verified above. Every
default-axis form regresses source and differs from accepted and each other.
Actual bytes establish identity; equal scores are not treated as identity.
All production edits restored; no trial retained.

Evidence: `_work/enhaxisdispatch-*.json`, `_work/enhaxisdefaultzero-*.json`,
source snapshots, identity ledgers and
`_work/axis_dispatch_signed_char_old_identity.json`. Baseline outside-body/
signature check passes and production tracked diff is empty. No native/full-
corpus run is claimed for rejected variants. Accepted axisbyte-full/native
evidence remains authoritative: **98.23% unweighted /98.17% weighted**, exact
pair sets 2079 linked /959 byte equal baseline without losses. Both >=99.0
gates remain unmet; goal active. No push, PR or merge.


## Continuation: triangle count and index type combinations

Seven Read trials start from clean d344822f. The triangle count is the unsigned
16-bit indexCount divided by three, so its range is 0..21845. Six forms change
only this count to int, unsigned short or short, alone or together with the
loop index of the same type. A seventh changes only the loop index to int.
Unsigned division and indexCount cast, allocation timing, triangle index loads,
coordinate formulas, helper calls and accumulation order remain unchanged.
All tested count/index types represent the complete range. The last executing
index is at most 21844; its increment reaches at most 21845, within signed
short as well as unsigned short, so no narrow counter overflow occurs.
These are source type hypotheses, not recovered declarations. No helper,
ABI, flags, matcher, denominator or extra effects change.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| count_int | 98.0 / 55.9 | 867 | 100.0 | yes |
| count_and_index_int | 98.0 / 55.9 | 867 | 100.0 | yes |
| count_unsigned_short | 98.0 / 54.2 | 865 | 100.0 | no |
| count_and_index_unsigned_short | 98.0 / 54.2 | 865 | 100.0 | no |
| count_short | 98.0 / 54.2 | 865 | 100.0 | no |
| count_and_index_short | 97.5 / 52.2 | 863 | 100.0 | no |
| signed_index | 98.0 / 55.9 | 867 | 100.0 | yes |

Int count, int count/index and int index alone are byte-identical to accepted
Core. Unsigned-short count, unsigned-short count/index and signed-short count
alone are byte-identical to each other, SHA-256
7959a2360ebbb34ef2545d1cd3a08287973844c832e39cf0313f191c4cb9e414,
but differ from accepted and lower strict while retaining rounded source.
Signed-short count/index differs from both groups and regresses source.
Identity is measured from whole-object bytes, not equal scores. No required
source gain occurs; all production edits restored and no trial retained.

Evidence: `_work/readtricounttypes-*.json`, source snapshots and
`_work/read_triangle_count_type_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: recursive child count storage on current reader

Six Read trials start from clean 16b21ecb. Signed int and unsigned-short child
counter forms revisit the accepted guarded-do loop after the retained color
field-store change, rather than assume the older 97.9 reader measurements
apply. childCount spans 0..65535; the guard prevents entry for zero and final
increment reaches at most 65535, so unsigned short does not wrap. Signed int
holds the whole range. Child allocation/read/Merge/ArrayAdd order is retained.

Four forms read the same two bytes into a serialized unsigned-short local,
then copy to int or unsigned int childCount after successful Read. Unsigned
count is additionally combined with signed int or unsigned-short loop index.
Only the bound storage changes; Read size, failure branch, initial guard and
recursive work order remain unchanged. These count-capture hypotheses assume
the ordinary synchronous Read buffer contract, not a custom implementation
retaining and mutating this stack address later. AEFile.cpp:184 forwards to
the low-level virtual Read; this inspection alone is not universal proof for
all possible implementations. No broader native runtime equivalence is claimed
for these rejected forms. No helper/ABI/flag/matcher/denominator changes.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| signed_counter | 98.0 / 55.9 | 867 | 100.0 | yes |
| short_counter | 98.0 / 55.7 | 868 | 100.0 | no |
| count_int | 96.9 / 53.6 | 862 | 98.2 | no |
| count_unsigned_int | 96.9 / 53.6 | 862 | 98.2 | no |
| wide_count_signed_counter | 96.9 / 53.6 | 862 | 98.2 | no |
| wide_count_short_counter | 96.9 / 53.6 | 863 | 98.2 | no |

Signed counter equals accepted Core byte for byte. Short counter retains
rounded source but worsens strict and differs from accepted. Int/uint count
copies and uint count plus signed index produce the same whole object as one
another, while uint count plus short index differs; all regress required source
to 96.9. Actual bytes establish identity, not equal scores. No trial retained;
production source restored.

Relocation-aware count-copy call inspection finds 61 emitted call instructions
in accepted versus 60 in uint-count trial: one fewer static `_ZdaPv` site and
three indirect BLX operands changing r5 to r6. The six indirect calls do not
enter direct-call matcher comparison. This is static call-site evidence, not
proof of a missing executed release. No synthetic cleanup effect is added.
Evidence: `_work/child_wide_count_calls.json`.

Additional evidence: `_work/readchildcounttypes-*.json`, source snapshots and
`_work/read_child_count_type_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: attached animation receiver and range-start locals

Six Enhanced trials start from clean a1444a21. After the unchanged populated
key-count test, self->animation is assigned the existing anim pointer. Three
forms use that just-assigned field as receiver for animationStart, range setup,
or both. A fourth names the just-assigned pointer as attached before loading
the existing rate. Two forms name the existing float-to-long-long conversion
as rangeStart after the animationStart store, with local or field receiver.

No intervening call separates pointer attachment from the rate load/start
store; the range start retains direct float-to-long-long conversion and is
not routed through the potentially narrower animationStart int. Original
0x6c194 attaches fp, 0x6c19e loads rate, 0x6c1aa stores int animationStart,
0x6c1ae converts float to long long, and 0x6c1be calls range setup. This order
is retained. Receiver/local spelling is a source hypothesis, not recovered
original C++. Empty/failure cleanup, helpers, ABI, compiler flags, matcher,
denominator and all other bodies remain unchanged.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| field_start_receiver | 98.3 / 66.9 | 641 | 100.0 | yes |
| field_range_receiver | 98.3 / 66.9 | 641 | 100.0 | yes |
| both_field_receivers | 98.3 / 66.9 | 641 | 100.0 | yes |
| named_attached_receiver | 98.3 / 66.9 | 641 | 100.0 | yes |
| explicit_range_start | 98.3 / 66.9 | 641 | 100.0 | yes |
| field_receiver_explicit_start | 98.3 / 66.9 | 641 | 100.0 | yes |

All six whole Mesh objects are byte-identical to accepted axisbyte-full.
Actual bytes establish identity, not equal scores. These receiver reload and
conversion-local forms do not affect canonical ARM output and offer no
required source gain. No trial retained; production source restored.

Evidence: `_work/enhfinishreceiver-*.json`, source snapshots and
`_work/enh_finish_receiver_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: section flag lifetimes and explicit zero dispatch interaction

Seven Enhanced trials start from clean 0c5b4331. Five replace the byte format
alias with two explicit optional/V5 predicates as const bools or unsigned int
masks at entry or immediately before optional sections, plus bools immediately
after Transform allocation. Only flags bits 0x18 and 0x10 matter; masking the
original unsigned flags or its low byte selects the same sections for every
32-bit input. Flags are a by-value parameter, so predicate evaluation may move
without a field/file/global load moving. All bounds and section reads, Insert
calls and failure behavior remain in order. Earlier full-format alias trials
did not test these split lifetimes on the byte-axis state.

The immediately-before-optional form preserves source 98.3 with strict 67.2.
Two further trials combine it with explicit V5 cases 4/5/default assigning
channel zero, with or without its initializer. These retain masks, rotation
arithmetic, key insertion and hasAnimation effects. This is an interaction
measurement of two individually source-neutral changes; it is not inferred
from their standalone scores. No helpers, ABI, flags, matcher, denominator or
synthetic effects change. Local declarations are hypotheses, not recovered C++.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| section flag lifetime: bool_entry | 98.2 / 67.2 | 641 | 100.0 | no |
| section flag lifetime: bool_optional | 98.3 / 67.2 | 641 | 100.0 | no |
| section flag lifetime: mask_entry | 98.2 / 67.2 | 641 | 100.0 | no |
| section flag lifetime: mask_optional | 98.3 / 67.2 | 641 | 100.0 | no |
| section flag lifetime: bool_after_allocation | 98.2 / 67.2 | 641 | 100.0 | no |
| optional flags + V5 zero: explicit_zero_assignment | 98.3 / 67.9 | 639 | 100.0 | no |
| optional flags + V5 zero: zero_labels_no_initializer | 98.3 / 67.9 | 639 | 100.0 | no |

Entry bool/mask and post-allocation bool forms are byte-identical to each other
and lower required source to 98.2. Optional bool/mask forms equal each other
but differ from entry forms and accepted. The two explicit-zero combinations
equal one another and differ from both flag-lifetime groups and accepted;
they improve strict to 67.9 but retain source 98.3. Actual whole-object bytes
establish identities; score equality is not treated as identity. No required
source gain occurs, so no trial is retained; production source restored.

Evidence: `_work/enhsectionflaglife-*.json`, `_work/enhflagzero-*.json`, source
snapshots and both identity ledgers. Baseline outside-body/signature check
passes and production tracked diff is empty. No native/full-corpus run is
claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: loaded child pointer captures in reader and caller

Eight trials start from clean 56f6a86d. Four forms are tested independently in
Read and Create child loops after successful recursive MeshReadData: copy the
resulting childPtr for bounds Merge only, copy immediately before ArrayAdd
for that operation only, copy before Merge and use for both, or alias childPtr
through a reference for both. Each preserves allocation, field inheritance,
recursive read, failure branch, Merge then ArrayAdd, and loop order. No capture
precedes the reader, which can replace childPtr through its pointer argument.

Inspected Array.h:95 shows ArrayAdd takes the mesh pointer by value. No child
pointer mutation is added, and reference forms keep aliasing the same local.
The two-operation snapshot assumes the existing bounds Merge behavior rather
than an unrelated custom helper mutating the escaped childPtr stack slot.
No broader arbitrary-callback equivalence is claimed. These are source lifetime
hypotheses, not recovered original declarations. Each trial changes only its
one allowed body; other bodies, helpers, ABI, flags and matcher stay unchanged.

| Trial | Focus body source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| read_merge_copy | 98.0 / 55.9 | 867 | 100.0 | yes |
| read_add_copy | 98.0 / 55.9 | 867 | 100.0 | yes |
| read_both_copy | 96.9 / 53.1 | 863 | 98.2 | no |
| read_both_reference | 98.0 / 55.9 | 867 | 100.0 | yes |
| create_merge_copy | 98.4 / 60.7 | 289 | 100.0 | yes |
| create_add_copy | 98.4 / 60.7 | 289 | 100.0 | yes |
| create_both_copy | 98.0 / 59.3 | 289 | 100.0 | no |
| create_both_reference | 98.4 / 60.7 | 289 | 100.0 | yes |

Six isolated-use/reference forms yield the accepted byte-identical whole Core
object. Copy used for both operations regresses source in Read and Create;
the two resulting objects differ from accepted and each other. Actual bytes
establish identity, not equal scores. No required source gain occurs; all
production edits restored and no trial retained.

Read both-copy relocation-aware inspection finds 61 emitted calls in accepted
versus 60 in trial: one fewer static `_ZdaPv` site and three indirect BLX
operands changing r5 to r6. The six indirect calls are outside direct-call
matcher comparison. This is static-site evidence, not proof of missing
executed cleanup. Evidence: `_work/child_pointer_both_copy_calls.json`.

Additional evidence: `_work/childpointercapture-*.json`, source snapshots and
`_work/child_pointer_capture_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: seven-channel V5 outer loop forms

Six Enhanced trials start from clean aa05ddfc. The V5 outer channel loop runs
exactly seven times after the existing presence check. Forms use for c!=7,
explicit while c<7, unconditional do with ++c<7, ++c<=6 or ++c!=7, or a remaining
count 7->1 deriving channel c=7-remaining. Each visits channels 0..6 in order;
all channel count reads, signed-short inner key bounds, key/update operations,
mask selection, rotation arithmetic and hasAnimation effects stay unchanged.
No guard is fabricated: this existing fixed-size channel loop always has seven
iterations when entered. All counters remain within 0..7 without overflow.
These loop spelling hypotheses are not recovered original C++. They test
outer channel traversal, not the previously measured signed inner-key loops.
Helpers, ABI, flags, matcher, denominator and other bodies stay unchanged.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| for_not_equal | 98.0 / 66.7 | 641 | 100.0 | no |
| while | 98.3 / 66.9 | 641 | 100.0 | yes |
| do_less | 98.0 / 66.8 | 640 | 100.0 | no |
| do_at_most | 98.0 / 66.8 | 640 | 100.0 | no |
| do_not_equal | 98.0 / 66.8 | 640 | 100.0 | no |
| remaining | 98.2 / 66.3 | 643 | 100.0 | no |

While form yields accepted byte-identical Mesh. All other forms regress required
source. Do <7 and <=6 objects equal each other; do !=7 differs from them despite
matching rounded scores and instruction count. Remaining-count and for !=7
objects differ from accepted, all do forms and one another. Actual whole-object
bytes establish identities, not scores. No trial retained; production restored.

Evidence: `_work/enhv5outerloop-*.json`, source snapshots and
`_work/enh_v5_outer_loop_identities.json`. Baseline outside-body/signature check
passes and production tracked diff is empty. No native/full-corpus run is
claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: cached uncompressed UV odd-component traversal

Five Read trials start from clean e6ecefb5. They change only the successful
uncompressed UV shader-flip loop, preserving the accepted uvFloats pointer
and captured vertex count. Two traverse odd component indices directly from
1 with stride 2 using int or unsigned int. Another traverses vertices and
addresses component 2*i+1. Two use a pointer advancing by two floats per vertex,
with signed or unsigned vertex index, changing only p[1] by 1.0f-p[1]. Earlier
odd-component measurement combined it with per-iteration owner-field reload;
these forms isolate traversal with the accepted cached pointer.

vertexCount is at most 65535, so component bound is at most 131070, fitting
all tested int/uint indices. Zero vertices execute no store or pointer offset.
Pointer forms begin at uvFloats and advance after each store, reaching only
the allocated array's one-past end after the final iteration. No pointer+1 is
formed outside a nonempty iteration. File reads, allocation and field/count
capture points, float subtraction and affected components remain unchanged.
No helpers, ABI, flags, matcher, denominator or other bodies change. These are
source traversal hypotheses, not recovered original loop declarations.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| odd_component_unsigned_int | 98.0 / 55.7 | 867 | 100.0 | no |
| odd_component_int | 98.0 / 55.7 | 867 | 100.0 | no |
| vertex_index | 97.7 / 55.2 | 865 | 100.0 | no |
| pointer_unsigned_int | 97.7 / 55.2 | 865 | 100.0 | no |
| pointer_int | 97.7 / 55.2 | 865 | 100.0 | no |

The two odd-component index objects are byte-identical to each other, retaining
rounded source but lowering strict. Vertex-index and both advancing-pointer
objects equal one another and regress source to 97.7. The two groups differ
from accepted and each other. Actual whole-object bytes establish identities,
not equal scores. No required source gain occurs; no trial retained and
production source restored.

Evidence: `_work/readfloatuvindex-*.json`, source snapshots and
`_work/read_float_uv_index_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: compressed UV flip storage and outer branch

Seven Read trials start from clean af28b2c5. Engine.h:581 declares enableShader
unsigned char. Five change its existing compressed-UV snapshot from explicit
nonzero bool to raw unsigned char, unsigned int, int or unsigned short, or
implicit bool conversion. All represent the full 0..255 raw range and the
existing ternary uses the same nonzero truth value. The shader flag load stays
after raw Read and output allocation at its accepted point, not before calls
that could change it. Double UV scaling/subtraction and final float stores
retain their order and formulas. No layout or helper change is introduced.

Two further forms move the conditional choice outside that loop, selecting
a complete flipped or unflipped traversal, with either branch written first.
Each performs the same per-vertex U conversion followed by V conversion and
optional 1.0-v double subtraction, using the unchanged captured flag/count/
base pointers. Empty count executes no coordinate operations. No additional
load, file call or storage effect is added. The source duplicates the two
legitimate traversal branches; it is not dummy score padding or claimed
recovered C++. Other bodies, ABI, flags, matcher and denominator stay unchanged.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| flip storage: raw_unsigned_char | 98.0 / 55.9 | 867 | 100.0 | yes |
| flip storage: raw_unsigned_int | 98.0 / 55.9 | 867 | 100.0 | yes |
| flip storage: raw_int | 98.0 / 55.9 | 867 | 100.0 | yes |
| flip storage: raw_unsigned_short | 98.0 / 55.9 | 867 | 100.0 | yes |
| flip storage: implicit_bool | 98.0 / 55.9 | 867 | 100.0 | yes |
| outer flip branch: flipped_first | 96.8 / 55.1 | 892 | 100.0 | no |
| outer flip branch: plain_first | 96.8 / 55.1 | 892 | 100.0 | no |

All five storage forms yield accepted byte-identical Core. Both outer-branch
forms yield the same whole object as each other but differ from accepted,
raise raw instruction count from 867 to 892 and regress source to 96.8.
Actual bytes establish identity, not equal scores. No required source gain;
no trial retained and production source restored.

Evidence: `_work/readuvfliptypes-*.json`, `_work/readuvouterflip-*.json`, source
snapshots and both identity ledgers. Baseline outside-body/signature check
passes and production tracked diff is empty. No native/full-corpus run is
claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: normal upper predicates and destination load lifetimes

Six Read trials start from clean e569fc13. Three capture upper-bound float
comparisons in bool locals before all lower selections, after all lower
selections, or per component, and select 1.0f or the existing cast lower result.
They retain all original float quotients, double lower comparisons/selections,
float upper predicates and numeric output values, including unordered predicate
fallthrough. No scalar lower-double operation is replaced with a NEON clamp
or helper. This is predicate scheduling, not recovered original C++.

Three additional forms move the existing nonzero-normal destination field
load before the quotients, before lower selection, or after lower selection.
All remain after sqrtf and inside the existing len!=0 branch; no owner/field
load is moved across sqrtf or another call. The intervening divisions,
comparisons and local selections do not mutate the mesh. The zero-normal
branch and all output stores keep their existing order and position. Original
pointer timing remains authoritative; these are same-value source lifetime
hypotheses rather than exact recovery. Helpers, ABI, flags, matcher, denominator,
file operations and other bodies stay unchanged.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| upper predicate storage: upper_predicates_before_lower | 98.0 / 55.9 | 867 | 100.0 | yes |
| upper predicate storage: upper_predicates_after_lower | 98.0 / 55.9 | 867 | 100.0 | yes |
| upper predicate storage: component_upper_predicates | 98.0 / 55.9 | 867 | 100.0 | yes |
| normal destination lifetime: before_quotients | 98.0 / 55.9 | 867 | 100.0 | no |
| normal destination lifetime: before_lower | 98.0 / 55.3 | 869 | 100.0 | no |
| normal destination lifetime: after_lower | 98.0 / 55.3 | 869 | 100.0 | no |

All three upper-predicate forms yield accepted byte-identical Core. Moving
destination before quotients retains scores but differs from accepted; before
and after lower selections yield the same whole object as one another, differ
from accepted and the quotient form, and regress strict. None improves required
source. Actual whole-object bytes establish identities, not score equality.
All production edits restored; no trial retained.

Evidence: `_work/readclampupperpred-*.json`, `_work/readnormaldestlife-*.json`,
source snapshots and both identity ledgers. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: reader enhanced-mask local storage

Six Read trials start from clean 909f7417. subBit is flags&0x1a, used only for
nonzero entry-pivot and final enhanced/child-section tests. Four forms store
that same mask as unsigned char, unsigned short, int or const unsigned int;
one stores its nonzero value as const bool; another removes the local and
checks the unchanged by-value flags directly at both sites. The masked value
is at most 26, fitting every tested type, and bool preserves the two nonzero
predicates for all 32-bit flags. No scalar flag or mask is read from mutable
mesh/global memory, and no field/file operation moves.

Both section tests, ReadEnhanced's existing ==0 result check, child-count read,
recursive work, failures and all other stream operations retain their order.
No helper, ABI, flags, matcher, denominator, padding operation or other body
changes. This is a local-storage hypothesis, not recovered original C++.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| unsigned_char | 98.0 / 55.9 | 867 | 100.0 | yes |
| unsigned_short | 98.0 / 55.9 | 867 | 100.0 | yes |
| int | 98.0 / 55.9 | 867 | 100.0 | yes |
| const_unsigned_int | 98.0 / 55.9 | 867 | 100.0 | yes |
| bool | 98.0 / 55.9 | 867 | 100.0 | yes |
| direct_flags | 98.0 / 55.9 | 867 | 100.0 | yes |

All six whole Core objects equal accepted axisbyte-full byte for byte. Actual
bytes establish identity, not equal scores. These enhanced-mask storage and
direct-flag forms do not affect canonical ARM output and yield no source gain.
No trial retained; production source restored.

Evidence: `_work/readenhflagtypes-*.json`, source snapshots and
`_work/read_enhanced_flag_type_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: current validation binding and shared vertex-count types

Start state is clean b316a437. A fresh read-only audit binds present production
and fixtures to accepted f8607229, rather than relying on intervening negative
trial summaries. Whole source text equals that commit for both production
translation units, three native smoke fixtures and the reader/caller extraction
runner. Canonical flag digest and exact transported argument list still match.
The preserved full axisbyte report's actual normalized pair sets equal baseline:
2079 linked and 959 byte pairs, lost/gained empty. Its 204 objects contain only
the two allowed changed units; all other 202 equal baseline byte for byte.
No new full compilation or native execution is claimed by this audit; unchanged
source/fixtures and immutable report/object evidence establish the binding to
the completed accepted validation. Evidence: `_work/current_axisbyte_binding_audit.json`.

The fixed three-body source mean is 98.233333 and weighted mean 98.170667,
with original weights 873/291/636, sum 1800. Both gates are contradicted by
current evidence: total three-body source sum is 294.7, requiring 2.3 additional
summed score points for 297.0, while weighted mean needs 0.829333 points.
These are separate requirements, not a redefinition of success. Component audit:

| Body | Instruction inventory | Opcode inventory | Control inventory | Calls | Size | Source |
|---|---:|---:|---:|---:|---:|---:|
| Read | 93.9 | 98.1 | 98.8 | 100.0 | 99.2 | 98.0 |
| Create | 94.7 | 97.9 | 100.0 | 100.0 | 99.3 | 98.4 |
| Enhanced | 95.8 | 97.4 | 98.7 | 100.0 | 99.5 | 98.3 |

Unchanged metric recomputation for each row passes. Instruction inventory is
the largest component deficit; Read has greatest weight. Fresh Read inventory
inspection shows count-load/register-role, memory-spelling, arithmetic operand
alias and branch differences. Original tangent products at 0x6cd90..0x6cd98
are numerator times reciprocal, so no reverse multiplication is introduced
merely to target an inventory pattern. Zero-offset and fp/frame-role artifacts
are diagnostics, not grounds to edit the matcher or force registers/padding.
This audit does not establish a compiler ceiling or prove all residual gaps
are artifacts.

Two focused trials then change the shared reader vcount local to unsigned
short or int. Every assignment is from the unsigned-short vertexCount field,
so 0..65535 is representable. Component bounds <=196605 and allocation lengths
<=786420 remain within int/uint range; no signed overflow or count truncation
occurs. All field capture points, loop predicates, formulas and calls remain
unchanged. This differs from older per-stream scoped-count forms and directly
tests the shared declaration on current source. No other body/helper/ABI/
flag/matcher change; types are hypotheses, not recovered C++.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| unsigned_short | 98.0 / 55.9 | 867 | 100.0 | yes |
| signed_int | 98.0 / 55.9 | 867 | 100.0 | yes |

Both whole Core objects equal accepted byte for byte. No source gain occurs;
actual bytes establish identity, not equal scores. Source restored; no trial
retained. Additional evidence: `_work/readsharedvcount-*.json`, snapshots and
`_work/read_shared_vertex_count_identities.json`. Outside-body/signature check
passes and production tracked diff is empty. Accepted axisbyte-full/native
evidence remains authoritative: **98.23% unweighted /98.17% weighted**. Both
>=99.0 gates remain unmet; goal active. No push, PR or merge.


## Continuation: compressed normal source address lifetimes

Three Read trials start from clean ae9348de. They change only compressed-normal
source access: derive a const short record pointer s+i inside each iteration;
advance a const short source pointer by three shorts with the existing i+=3;
or preload the same three signed shorts into scalar locals. X/Y/Z load and
conversion order, double scaling, length calculation, clamp semantics and
output stores remain unchanged. The pointer-advance form keeps the dynamic
slot vertexCount condition and output component index; it does not replace
that condition with a captured count or alter file/field capture points.

Each pointer addresses the same three shorts per defined iteration. A zero
count performs no pointer arithmetic; advancement after the final valid record
forms only the buffer's one-past pointer. No guard, helper call, padding or
external effect is added. Pointer/scalar declaration forms are source hypotheses,
not recovered original declarations. Helpers, ABI, flags, matcher, denominator
and other bodies remain unchanged.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| record_pointer | 98.0 / 54.1 | 868 | 100.0 | no |
| advancing_record_pointer | 97.9 / 55.7 | 866 | 100.0 | no |
| scalar_source_values | 98.0 / 55.9 | 867 | 100.0 | yes |

Scalar source locals yield accepted byte-identical Core. Record-pointer form
differs and retains source 98.0, but instruction inventory remains 93.9:
opcode inventory regresses 98.1->98.0 while size improves 99.2->99.3, balancing
aggregate score. Strict regresses 55.9->54.1. Advancing pointer differs from
both accepted and record-pointer form; instruction inventory drops to 93.5,
opcode to 98.0 and size to 99.1, with source 97.9. Both keep control inventory
98.8, ordered control 91.1 and call match 100.0. Thus no concealed improvement
to the prioritized inventory component is found. Actual bytes establish
identity, not equal scores. No trial retained; production source restored.

Evidence: `_work/readnormalsourcelife-*.json`, source snapshots and
`_work/read_normal_source_lifetime_identities.json`. Baseline outside-body/
signature check passes and production tracked diff is empty. No native/full-
corpus run is claimed for rejected variants. Accepted axisbyte-full/native
evidence remains authoritative: **98.23% unweighted /98.17% weighted**, exact
pair sets 2079 linked /959 byte equal baseline without losses. Both >=99.0
gates remain unmet; goal active. No push, PR or merge.


## Continuation: parent owner loads at animation and child inheritance

Five Read trials start from clean ab788f46. Fresh original inspection confirms
0x6cf58 loads parent once, 0x6cf5c reads its animation, and 0x6cf60..0x6cf64
forms parent/animation bound addresses and calls Merge. Transform.h:75 bounds()
is an inline reference to its existing sphere, with no intervening callback.
Original 0x6cfea..0x6cff4 likewise loads parent once for inherited vertex format
and material. These observations motivate explicit source owner locals.

After recursive read, original 0x6d00c loads parent for child-bounds Merge at
0x6d018, then 0x6d01c reloads parent before animation/ArrayAdd at 0x6d026.
These are two distinct loads across Merge, not one captured parent for both
operations. The trials retain both current post-read loads; no receiver or
animation load is hoisted across that call. The earlier report's captured
parent wording at 0x6cfea..0x6cff4 refers only to field inheritance.

Four forms capture parent for initial animation/bounds setup or child field
inheritance, using a new local or reusing existing m. A fifth combines both
new-local forms. Each capture is at the original first slot-read point, before
its field reads and without an intervening call. Fresh child allocation is
already complete before inheritance capture. Bounds Merge, recursive read,
ArrayAdd and error order stay unchanged. No helper, header/layout, flags,
matcher, denominator or other body changes. Locals are source hypotheses,
not recovered original C++ declarations or native ABI proof.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals axisbyte-full |
|---|---|---:|---:|---|
| animation_local | 98.0 / 55.9 | 867 | 100.0 | yes |
| inheritance_local | 98.0 / 55.9 | 867 | 100.0 | yes |
| animation_reuse_m | 98.0 / 55.9 | 867 | 100.0 | yes |
| inheritance_reuse_m | 98.0 / 55.9 | 867 | 100.0 | yes |
| both_local | 98.0 / 55.9 | 867 | 100.0 | yes |

All five whole Core objects equal accepted axisbyte-full byte for byte.
Actual bytes establish identity, not equal scores. These explicit owner-load
forms do not alter canonical ARM output or improve required source. No trial
retained; production source restored.

Evidence: `_work/readparentowner-*.json`, source snapshots and
`_work/read_parent_owner_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: local parsed section type/count aggregates

Four Enhanced trials start from clean 30d00ee6. A local trivial aggregate groups
the signed-short section type and unsigned-short key count, with both member
orders tested. The count is shared across the first three vector/axis groups
only, or those groups plus optional count/presence. Each member is used by
actual existing reads and dispatch/loop logic; no unused member, explicit
padding, alignment attribute, packing directive or synthetic operation is
introduced. The aggregate is declared inside the allowed body and produces
no constructor/destructor/helper implementation change.

Reads remain exactly two bytes to their appropriate short field; type retains
signed interpretation, unsigned counts retain full 0..65535, and signed V5
per-channel count stays a separate unchanged local. No contiguous structure
read is substituted for separate reads. Field order changes only local layout,
not an on-disk/native ABI structure. The preceding original shared stack-slot
observations motivate this source grouping but do not prove recovered C++
aggregate declarations. All key/value conversions, channels, helper order and
failures remain unchanged. No flags, matcher, denominator or other body change.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals axisbyte-full |
|---|---|---:|---:|---|
| type_first_prefix | 97.1 / 61.5 | 643 | 100.0 | no |
| type_first_all_unsigned | 97.0 / 48.5 | 646 | 100.0 | no |
| count_first_prefix | 97.9 / 52.9 | 638 | 100.0 | no |
| count_first_all_unsigned | 97.9 / 53.2 | 638 | 100.0 | no |

All four forms regress required source and produce distinct whole Mesh objects
from accepted and each other. Count-first variants have equal rounded source
and raw instruction counts but differ in strict and bytes. Actual byte
comparison establishes nonidentity; equal size/source is not used as proof.
No trial retained; production source restored.

Evidence: `_work/enhsectionaggregate-*.json`, source snapshots and
`_work/enh_section_aggregate_identities.json`. Baseline outside-body/signature
check passes and production tracked diff is empty. No native/full-corpus run
is claimed for rejected variants. Accepted axisbyte-full/native evidence remains
authoritative: **98.23% unweighted /98.17% weighted**, exact pair sets 2079 linked
/959 byte equal baseline without losses. Both >=99.0 gates remain unmet;
goal active. No push, PR or merge.


## Continuation: individual key counters and scalar key records

Seventeen Enhanced trials start from clean da09ecd3. The first nine change
one key-loop counter at a time: signed int for each vector or axis group,
and unsigned short for each vector group. This complements earlier grouped
counter tests rather than repeating their source snapshots. Counts remain
unsigned short, the axis counter remains unsigned char, and all operations,
read order, masks, conversions and failure paths remain unchanged. An unsigned
short loop index can reach the terminal count 65535 without wrapping; signed
int can represent all unsigned-short counts. No byte index for unbounded
key counts is introduced. Six signed forms produce the entire accepted Mesh
object byte-for-byte; the three short vector counters regress source to 98.2.

Eight further trials group a scalar key's existing time and value into a local
trivial two-float record. Both field orders are tested for the three axis loops,
optional loop, V5 loop, or all five loops. Both fields are used; no artificial
member, padding/alignment directive, extra operation, helper, destructor or
ABI-visible type is added. Each read is still separately four bytes, with the
time-between-frames update between time and value reads. Position Y negation,
V5 division and rotation conversion, and InsertKeyFrame pointer/time operands
remain in their existing order. This is a source grouping hypothesis, not a
claim of a recovered original C++ record declaration. All eight change object
bytes; none increases the required source score. Equal rounded 98.3 for the
value-first optional form is not a gain, and its strict score also decreases.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| vector_position_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector_scale_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector_rotation_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector_position_unsigned_short | 98.2 / 66.8 | 642 | 100.0 | no |
| vector_scale_unsigned_short | 98.2 / 53.8 | 642 | 100.0 | no |
| vector_rotation_unsigned_short | 98.2 / 66.8 | 642 | 100.0 | no |
| axis_position_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| axis_scale_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| axis_rotation_int | 98.3 / 66.9 | 641 | 100.0 | yes |
| time_first_axes | 97.5 / 23.1 | 646 | 100.0 | no |
| time_first_optional | 98.2 / 66.8 | 642 | 100.0 | no |
| time_first_v5 | 98.2 / 45.9 | 642 | 100.0 | no |
| time_first_all | 97.4 / 44.9 | 648 | 100.0 | no |
| value_first_axes | 98.2 / 21.9 | 649 | 100.0 | no |
| value_first_optional | 98.3 / 66.8 | 642 | 100.0 | no |
| value_first_v5 | 98.1 / 47.7 | 642 | 100.0 | no |
| value_first_all | 98.1 / 22.2 | 650 | 100.0 | no |

All seventeen forms rejected; production source restored and compared as full
text against f8607229 for both source files. Baseline outside-body/signature
check passes. No flags, matcher, fixed-three weights or denominator change.
Evidence: `_work/enh_individual_key_counter_identities.json`,
`_work/enh_scalar_key_record_identities.json`, their per-trial verifier reports
and source snapshots. Whole-object identity comes from actual byte comparison.
No new native/full-corpus run is claimed for rejected forms. Accepted full/native
evidence remains bound to unchanged source: **98.23% unweighted /98.17% weighted**,
2079 linked-exact /959 byte-exact pairs equal baseline without losses. Both
>=99.0 gates remain unmet; goal active. No push, PR or merge.


## Continuation: centered compressed-normal destination pointer

Start state is clean tracked 26fe9f19. Six focused Read trials test an interior
pointer to input/output normal triples. Original compressed-normal output uses
r4 initialized to 4 at 0x6ca92, destination base+r4 at 0x6cba2, negative-four
store at 0x6cbac, zero-offset store at 0x6cbb0 and positive-four store at
0x6cbbe. Fallback similarly stores at 0x6cb20/0x6cb24 before joining the last
store. These are actual instruction observations, not recovered variable names
or original C++ pointer declarations. The source hypothesis retains only
`float *destination = normals + i + 1` inside each existing branch and uses
destination[-1], [0], [1] for the same X/Y/Z writes in the same order.

Both branches retain the existing fresh slot/normal field read at the same
source point. No owner/count capture, virtual read, helper, clamp, precision,
floating arithmetic order, allocation, failure path or destructor is changed.
Each pointer is formed inside an executing valid triple; zero-count loops form
none. No one-before-array or outside-array pointer is introduced. Reads of
input components and dynamic loop count stay unchanged. No header, ABI layout,
unrelated body, flags, matcher or denominator change.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals prior accepted |
|---|---|---:|---:|---|
| source_center_1 | 98.0 / 54.1 | 868 | 100.0 | no |
| source_center_2 | 98.0 / 54.5 | 868 | 100.0 | no |
| destination_center_1 | 98.4 / 56.8 | 877 | 100.0 | no |
| destination_center_2 | 98.3 / 55.1 | 873 | 100.0 | no |
| source_center_1_destination_center_1 | 98.4 / 56.3 | 877 | 100.0 | no |
| source_center_2_destination_center_1 | 98.4 / 56.8 | 877 | 100.0 | no |

Retain the minimal destination_center_1 variant. Combining source_center_2
produces its identical whole Core object and adds no value. Read improves
98.0 ->98.4 source and 55.9 ->56.8 strict, although raw instruction count grows
867 ->877 (original 873). Exact original raw length is not the objective;
destination_center_2 has 873 instructions but only 98.3 source. Whole-object
identity/nonidentity is checked on actual bytes, not inferred from scores.

Fresh validation of frozen retained source:

- UCRT64 native gof2 static-library build passes.
- Verbatim current reader with actual selected math/Vector passes colors,
  positions, tangent bases, bounds and existing truncation cases. A new fixture
  checks four distinct adjacent normal records: zero fallback and three records
  with all components nonzero and differing signs/magnitudes. Expected values
  use independent double normalization with 1e-6 tolerance, checking all twelve
  component outputs. Every shorter normal-data prefix fails before attaching
  normals. Shader disabled isolates this parsing path.
- The new fixture also passes against the previous accepted reader body.
  Separate private mutations replacing normalized X by Y and zero-fallback X
  by one each fail with exit 3. Production source is never mutated for these
  negative tests; they alter only extracted private include files.
- Caller/actual Mesh/selected BSphere fixtures and actual enhanced parser pass.
- Baseline outside-body/signature check passes. Production diff is only two
  interior pointer declarations and their equivalent stores in the allowed
  reader body; the targeted reader fixture is the only test-source change.
- All 204 ARM translation units rebuilt; full corpus has 4619 comparisons,
  zero skipped. Actual linked-exact 2079 / byte-exact 959 pair sets equal clean
  baseline, lost [] and gained [] for each. Other 202 whole objects equal
  baseline. Full Core equals focused winner; Mesh equals accepted axisbyte-full.
  Canonical match flag digest and actual argument list remain unchanged.

Unchanged matcher components for Read (previous -> current):

- instruction_inventory_match: 93.9 -> 95.0
- opcode_inventory_match: 98.1 -> 98.3
- control_inventory_match: 98.8 -> 98.8
- ordered_control_flow_match: 91.1 -> 91.8
- call_count_match: 100.0 -> 100.0
- size_match: 99.2 -> 99.6

All fixed-three component scores and source scores are recomputed directly
from the fresh target/base instruction lists by the unchanged matcher. Averaging
already rounded component rows can differ by 0.1 from the matcher, which averages
unrounded component ratios before rounding source; it is not used as proof.

Full strict/source averages 75.98 / 89.0. Full JSON SHA-256:
`8544da04f3fea9f6a7c3b2a60660e6ef8f8c6fad79238d4a15f5ec20154db90c`.

| Fixed unique body | Source | Strict | Original instruction weight |
|---|---:|---:|---:|
| MeshCreateFromFile | 98.4% | 60.7% | 291 |
| MeshReadData | 98.4% | 56.8% | 873 |
| ReadEnhancedDataFromFile | 98.3% | 66.9% | 636 |

Fixed three means: **98.366667% unweighted / 98.364667% weighted**.
Both >=99.0 gates remain unmet; goal active. Native body tests use mocked
file boundaries and selected exact math bodies; neither those nor the library
build proves whole-game executable ABI or Android runtime/device correctness.
No Godot evidence or compiler ceiling claim. No push, PR or merge.

Evidence: `_work/readnormalcentered-*.json`, source snapshots,
`_work/read_normal_centered_pointer_identities.json`,
`_work/normalcenter_full.json`, `_work/normalcenter_three.json`,
`_work/normalcenter_validation_audit.json`, full build/verify logs,
`_work/normalcenter_native.log`, `_work/normalcenter_read_smoke.log`,
enhanced build/run logs and `_work/normalcenter_negative_fixture_results.json`.


## Continuation: normal-center traversal and common-Z interactions

Eleven Read trials start from clean bf0243e8, the newly validated centered
normal destination source. Eight test input record pointer/traversal and logical
vertex-counter interactions while keeping that output addressing. Record source
pointer is formed inside each iteration, or advances by three from raw's base.
The advancing base pointer ends at one-past the final triple; no interior
pointer is advanced beyond one-past. Logical vertex counters use unsigned int,
int or unsigned short, compare the same freshly read vertexCount at each
condition, and compute i=vertex*3 inside the executing body. The maximum index
is 196602; terminal vertex index 65535 fits every tested type without wrap.
Zero-count loops form no per-triple pointer. Count read timing and all actual
operations, precision, failure paths and owner field reads remain unchanged.

Three more trials test sharing final Z store after the len branch and/or
spelling fallback Y before X. Original normal path stores at 0x6cbac/0x6cbb0
and joins final Z at 0x6cbbe; fallback stores Y then X at 0x6cb20/0x6cb24 before
joining. Shared form initializes destination and Z in both branches and retains
existing branch-local field loads and all output addresses. There is no helper
or observer between these ordinary distinct-component writes. No original C++
declaration or source spelling is claimed recovered. These are interactions
with the new accepted destination form, not repeats of pre-improvement bodies.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| source_record | 98.4 / 56.8 | 877 | 100.0 | yes |
| advancing_source | 98.0 / 55.7 | 873 | 100.0 | no |
| vertex_index_unsigned_int | 98.2 / 56.1 | 876 | 100.0 | no |
| vertex_advancing_unsigned_int | 98.0 / 55.7 | 876 | 100.0 | no |
| vertex_index_int | 98.2 / 56.1 | 876 | 100.0 | no |
| vertex_advancing_int | 98.0 / 55.7 | 876 | 100.0 | no |
| vertex_index_unsigned_short | 98.2 / 56.1 | 876 | 100.0 | no |
| vertex_advancing_unsigned_short | 98.0 / 55.7 | 876 | 100.0 | no |
| shared_z_and_destination | 98.4 / 56.8 | 877 | 100.0 | yes |
| fallback_y_before_x | 98.4 / 56.8 | 877 | 100.0 | yes |
| shared_z_fallback_y_before_x | 98.4 / 56.8 | 877 | 100.0 | yes |

Four forms yield the exact accepted whole Core object. The three logical index
types yield identical objects to one another for each traversal family, and
all seven nonidentical forms regress source. None retained; production source
restored. Full source and six relevant source/fixture files compare exactly to
accepted bf0243e8. Baseline outside-body/signature check passes.

Read-only binding recheck revalidates the existing normalcenter-full JSON hash,
actual 2079 linked/959 byte pair sets against baseline, 204 objects/202 unaffected
objects, winner identity, canonical flag digest/argument list, and all fixed
three matcher components directly from the target/base instruction lists.
No native or full-corpus rerun is claimed for rejected forms. Accepted native
and full ARM evidence remains bound to restored source: **98.37% unweighted
/98.36% weighted**. Both >=99.0 gates remain unmet; goal active.

Evidence: `_work/read_normal_center_interaction_identities.json`,
`_work/read_normal_common_z_center_identities.json`, per-trial verifier JSON
and source snapshots, rechecked `_work/normalcenter_validation_audit.json`.
No helper, header, ABI layout, unrelated body, flags, matcher, denominator or
weight change. No push, PR or merge.


## Continuation: normal record-end and compressed UV pair pointers

Thirteen Read source trials start from clean d3900758. Seven complete normal
pointer exploration with a record-end pointer, using offsets -3/-2/-1. Input
end is computed per executing record or advanced by three at the beginning of
the iteration, starting from the allocation base. It reaches one-past after
the final record, never beyond it. Output end is formed after the same branch
field reload and uses the same X/Y/Z stores in order. Zero-count loops perform
none of these calculations. Combined input middle/end and output-end forms
are also checked; no helper, read/count timing, arithmetic precision/order,
clamp, allocation, destructor or failure path changes.

Normal per-record input-end form yields the accepted whole Core object.
Output-end alone yields the exact pre-improvement axisbyte-full Core object,
not merely its equal rounded score/length. Advancing input-end yields exactly
the preceding advancing base-pointer trial object. These byte identities
show which address spellings the compiler canonicalizes; no source recovery
or general compiler ceiling follows from them.

Six UV trials address a pair by its second component or end, separately for
input, output, or both. Pointer computations stay inside executing iterations.
Original compressed UV output field/load and stores are at 0x6c92c-0x6c93c
and 0x6c95e: pointer refers to the first component, followed by +4 store. The
alternate UV source spellings are hypotheses, not original recovered C++.
Both input/output components, double scale, double flip subtraction, float
conversion, per-iteration field load and stores retain existing order. All
six UV forms produce the accepted entire Core object byte-for-byte.

| Family/trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| normal/input_end_per_record | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal/input_end_advancing | 98.0 / 55.7 | 873 | 100.0 | no |
| normal/output_end | 98.0 / 55.9 | 867 | 100.0 | no |
| normal/input_center_1_output_end | 98.0 / 54.1 | 868 | 100.0 | no |
| normal/input_center_2_output_end | 98.0 / 54.5 | 868 | 100.0 | no |
| normal/input_center_3_output_end | 98.0 / 54.1 | 868 | 100.0 | no |
| normal/both_end_advancing_input | 97.9 / 55.7 | 866 | 100.0 | no |
| UV/destination_1 | 98.4 / 56.8 | 877 | 100.0 | yes |
| UV/source_1 | 98.4 / 56.8 | 877 | 100.0 | yes |
| UV/both_1 | 98.4 / 56.8 | 877 | 100.0 | yes |
| UV/destination_2 | 98.4 / 56.8 | 877 | 100.0 | yes |
| UV/source_2 | 98.4 / 56.8 | 877 | 100.0 | yes |
| UV/both_2 | 98.4 / 56.8 | 877 | 100.0 | yes |

No trial increases source; none retained. Production and relevant test source
restored and compared in full to bf0243e8. Baseline outside-body/signature check
passes. The existing normalcenter-full artifact hash, actual pair sets, all
204 objects/202 unaffected objects, winner identity, canonical flag digest and
arguments, and three matcher component rows are rechecked directly. No new
native/full-corpus execution is claimed for rejected forms. Accepted source
remains **98.37% unweighted /98.36% weighted**, linked 2079/byte 959 exact pair
sets equal baseline, lost [] and gained []. Both >=99.0 gates remain unmet;
goal active. No matcher, denominator, original weights, header, ABI layout or
unrelated body change. No push, PR or merge.

Evidence: `_work/read_normal_end_pointer_identities.json`,
`_work/read_uv_pair_center_identities.json`, their per-trial reports/snapshots
and rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: tangent output centers and current counter interactions

Fifteen Read trials start from clean c374bd9c. Nine address tangent or binormal
output triples by the middle, final component, or end, separately or together.
All field loads remain after their respective Normalize/Cross helper call;
no pointer capture is moved across a helper. X/Y/Z store order and addresses
are unchanged. The end pointer can be one-past the last triple, never beyond,
and none is formed by a zero-iteration loop. These source-addressing hypotheses
do not claim original recovered C++ pointer declarations. All nine produce
the entire accepted Core object byte-for-byte.

Six final/triangle loop trials examine interaction with the recently accepted
centered normal output source. Five repeat previously negative or identity
forms on this materially changed body: remaining for/guarded do, short final
counter, explicit component offset and short triangle counter. One adds signed
int final index. Count assignments remain from unsigned-short fields; all
counter limits fit their selected types. Count captures, helper order,
precision, stores, errors and cleanup expressions are unchanged. Guarded do
has no first iteration when count is zero. No cast creates an out-of-range
component index; explicit component stride is still three per vertex.

| Family/trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| output/tangent_1 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/binormal_1 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/both_1 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/tangent_2 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/binormal_2 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/both_2 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/tangent_3 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/binormal_3 | 98.4 / 56.8 | 877 | 100.0 | yes |
| output/both_3 | 98.4 / 56.8 | 877 | 100.0 | yes |
| counter/final_remaining_for | 97.7 / 56.7 | 874 | 98.2 | no |
| counter/final_remaining_do | 97.4 / 58.2 | 879 | 98.2 | no |
| counter/final_short_index | 98.4 / 56.8 | 877 | 100.0 | yes |
| counter/final_component_offset | 98.4 / 56.8 | 877 | 100.0 | yes |
| counter/triangle_short_index | 98.4 / 55.1 | 875 | 100.0 | no |
| counter/final_signed_index | 98.4 / 56.8 | 877 | 100.0 | yes |

Remaining forms regress source and call-count agreement; short triangle form
retains rounded 98.4 source but worsens strict. Others yield the accepted object.
None retained. Static BL/BLX plus relocation inspection of both remaining forms
is saved separately; a reduced static cleanup site count is not evidence of a
missing executed cleanup. No native correctness claim is made for rejected
forms. No dummy operation or compiler/matcher workaround is introduced.

All production and relevant test source restored and compared exactly to
bf0243e8. Read-only recheck verifies existing full JSON, actual linked 2079/byte
959 pair sets equal baseline with lost [] and gained [], 204 objects/202
unaffected objects, winner identity, canonical flags and all fixed-three metric
components from original/base instruction lists. Baseline outside-body and
signature check passes. No new native/full-corpus run claimed for rejected
forms. Accepted means **98.37% unweighted /98.36% weighted**; both >=99.0 gates
remain unmet, goal active. No other body/header/ABI, matcher, denominator or
weight change. No push, PR or merge.

Evidence: `_work/read_tangent_center_identities.json`,
`_work/read_center_counter_interaction_identities.json`, per-trial reports and
source snapshots; `_work/center_remaining_for_calls.json`,
`_work/center_remaining_do_calls.json`, rechecked normalcenter validation audit.


## Continuation: position flag interaction and shared enhanced key storage

Ten trials start from clean 1a8a6079. A fresh accepted Read instruction-inventory
gap still contains the original signed position selection. Original 0x6c6a8
is MOVS flags LSL#29, with BMI at 0x6c6ba; current source checks flags&4.
Five equivalent flag forms are tested on the materially changed normal-center
body. Four signed/high-bit variants produce the same nonaccepted whole Core
object, regressing Read 98.4 ->97.7; unsigned shift-by-31 yields the accepted
object. Signed conversions rely on the selected 32-bit two's-complement target
toolchain interpretation; no general portable-C++ implementation claim is made.
The shifted value remains nonzero precisely for the same low-three-bit flags.
No extra guard/operation, read ordering or flags meaning is introduced.

Five new Enhanced forms share only actually used local storage between sections:
vector buffer across the first three groups; key/value scalar buffers across
those groups; both; scalar buffers also through optional/V5; or all storage.
Every use is preceded by the same successful existing read. No initialization,
padding, field, helper, constructor/destructor or redundant operation is added.
Read sizes/order, minimum-time updates, masks, signed V5 count, position-Y
negation, angular conversion and attachment/cleanup stay unchanged. These are
source lifetime hypotheses under the parser's synchronous Read/key-copy use,
not recovered C++ declarations or proof about hypothetical retained caller
buffers. No native claim is made for rejected storage variants.

| Family/trial | Body source / strict | Instructions | Calls | Whole unit equals accepted |
|---|---|---:|---:|---|
| Read flag/direct_signed_shift | 97.7 / 56.8 | 885 | 100.0 | no |
| Read flag/shared_signed_shift | 97.7 / 56.8 | 885 | 100.0 | no |
| Read flag/shared_unsigned_high | 97.7 / 56.8 | 885 | 100.0 | no |
| Read flag/shared_shifted_bit | 98.4 / 56.8 | 877 | 100.0 | yes |
| Read flag/signed_shift_storage | 97.7 / 56.8 | 885 | 100.0 | no |
| Enhanced storage/vectors | 97.6 / 50.9 | 641 | 100.0 | no |
| Enhanced storage/prefix_scalars | 98.0 / 59.5 | 642 | 100.0 | no |
| Enhanced storage/prefix_all | 98.0 / 58.1 | 642 | 100.0 | no |
| Enhanced storage/all_scalars | 98.1 / 52.9 | 642 | 100.0 | no |
| Enhanced storage/all_storage | 98.1 / 38.6 | 637 | 100.0 | no |

All storage variants produce distinct Mesh objects and regress required source;
equal rounded score/size of prefix scalar/all forms is not identity. None of
the ten trials retained. Production and relevant test source restored and
compared in full to bf0243e8. Baseline outside-body/signature check passes.

Read-only binding recheck verifies existing full artifact hash, actual linked
2079/byte 959 pair sets equal baseline with lost [] and gained [], all 204
objects/202 unaffected objects, winner identities, canonical flags and fixed
three metric components directly from target/base instructions. No new native
or full-corpus run claimed for rejected variants. Accepted means **98.37%
unweighted /98.36% weighted**; both >=99.0 gates remain unmet, goal active.
No matcher, original weights, denominator, header/ABI or unrelated body change.
No push, PR or merge.

Evidence: `_work/read_center_position_flag_identities.json`,
`_work/enh_shared_key_storage_identities.json`, their per-trial reports and
source snapshots, rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: explicit frame capture after successful key reads

Six Enhanced trials start from clean 878ea831. They name the already required
time conversion as const int frameTime after the last successful read of each
key record, separately for vectors, axes, optional, V5, optional+V5, or all
eight loops. InsertKeyFrame's actual declaration is (const float*, longlong
flags, int time); the existing int conversion is retained exactly, with no
different-width time type. Conversion is not moved before the final file read,
so truncation failure does not acquire a new float-to-int operation. It also
stays after the existing updateTimeBetweenFrames. No precision change or claim
about valid NaN/out-of-range conversion is introduced; the original conversion
itself has not been removed or newly guarded.

Axis grouping moves one conversion before its bounded axis dispatch; this
rechecks an older hypothesis with the currently accepted unsigned-char axes.
Optional/vector declarations are immediately before their insertion calls.
V5 captures time before key division/rotation and channel dispatch, with all
read, time-update, channel, value conversion and insertion ordering preserved.
There is no helper between capture and use. No additional read/operation,
padding, header/layout or helper change is introduced. These declarations
are source hypotheses, not recovered original C++ locals.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| vectors | 98.3 / 66.9 | 641 | 100.0 | yes |
| axes | 97.5 / 64.0 | 643 | 100.0 | no |
| optional | 98.3 / 66.9 | 641 | 100.0 | yes |
| v5 | 98.1 / 65.6 | 648 | 100.0 | no |
| optional_v5 | 98.1 / 65.6 | 648 | 100.0 | no |
| all | 97.3 / 62.5 | 650 | 100.0 | no |

Vector and optional forms yield the accepted whole Mesh object. V5 and
optional+V5 yield the same nonaccepted object. All four nonidentical forms
regress required source. None retained; production source restored and full
source/fixture comparison to bf0243e8 passes. Baseline outside-body/signatures
pass. No new native or full-corpus run is claimed for rejected forms.

Existing full evidence is rebound by read-only checks of the JSON hash,
actual linked 2079/byte 959 pair sets equal baseline without lost/gained pairs,
204 objects/202 unaffected objects, winner identities, canonical flag digest
and argument list, and all fixed-three metric components directly from target
and base instruction lists. Accepted **98.37% unweighted /98.36% weighted**;
both >=99.0 gates remain unmet, goal active. No matcher, denominator, weights,
unrelated body, ABI layout, push, PR or merge change.

Evidence: `_work/enh_frame_capture_current_identities.json`, its six focused
verifier reports/source snapshots and rechecked normalcenter validation audit.


## Continuation: local caller parse-state aggregates

Four Create trials start from clean 6dfb2d75. A new local trivial ParseState
groups the actual eight-byte magic buffer and unsigned-short child count, with
both member orders tested. Two further forms also group the existing uint
file handle, with handle-first or handle-last order. Every member is used by
the existing parser; no padding member, explicit alignment/packing directive,
unused field, zero-fill or dummy operation is introduced. Natural compiler
alignment is left untouched. The type is local to the allowed function body,
not a header, on-disk structure or native ABI layout change.

Magic initialization still uses the existing strcpy only after OpenRead succeeds.
Handle still receives its existing zero initialization before OpenRead. SubCount
still receives the existing two-byte read immediately before dispatch, preserving
its full unsigned-short range. Reads still target separate members with identical
sizes and order; no whole-record read or new capture across calls is substituted.
Signature tables, format decisions, allocations, recursive reader calls, bounds,
array insertion, Close and cleanup expressions are unchanged. No original C++
aggregate declaration/member order is claimed recovered; this is a local storage
hypothesis after instruction-gap inspection.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| magic_count | 98.1 / 62.7 | 290 | 100.0 | no |
| count_magic | 98.3 / 50.5 | 291 | 100.0 | no |
| handle_magic_count | 98.2 / 54.2 | 292 | 100.0 | no |
| magic_count_handle | 97.8 / 63.8 | 295 | 100.0 | no |

All four objects differ from accepted and each other. Two forms improve strict
over accepted 60.7 but all regress required source from 98.4; none retained.
The count-first form has the original raw 291 instructions but still lower
source 98.3, so raw-size coincidence is not success. Production source restored.

Full source/relevant fixture comparison to bf0243e8 and baseline outside-body/
signature check pass. Read-only binding recheck verifies existing full JSON,
actual 2079 linked/959 byte pair sets equal baseline without lost/gained pairs,
204 objects/202 unaffected objects, winner identity, canonical flag digest and
arguments, and fixed-three matcher rows from actual target/base instruction
lists. No new native/full-corpus run claimed for rejected variants. Accepted
**98.37% unweighted /98.36% weighted**; both >=99.0 gates remain unmet, goal
active. No matcher, denominator, original weights, unrelated body or ABI change.
No push, PR or merge.

Evidence: `_work/create_local_parse_state_identities.json`, per-trial focused
reports/source snapshots and rechecked normalcenter validation audit.


## Continuation: caller parse-state aggregates including the child slot

Eight Create trials start from clean 3c54ff97. Unlike preceding parse-state
trials, the local aggregate includes the childPtr whose address is passed to
MeshReadData. It is grouped with handle or count in both orders, with both in
two orders, or with all three other existing locals in two orders. Every field
is used by the original operations; no padding field, attribute, packing,
extra initialization, dummy effect or helper is introduced. Natural compiler
alignment is not changed. The type is function-local, not an on-disk/header or
native ABI structure, and is not claimed recovered from original C++.

Child pointer is assigned by the same new Mesh immediately before its existing
fields and reader call. Its address still denotes that live pointer slot;
post-read bounds and array insertion use that slot at the same source points.
No child value is captured before a reader/helper or retained across an extra
call. Handle's sole existing zero initialization, magic strcpy after OpenRead,
separate read sizes/order, unsigned-short count range and all dispatch/cleanup/
Close expressions remain unchanged. This checks alias/storage interaction of
the actually addressed child slot, not a reinterpretation of disk data.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| handle_child | 98.0 / 59.5 | 291 | 100.0 | no |
| child_handle | 97.8 / 60.9 | 294 | 100.0 | no |
| count_child | 98.0 / 61.9 | 291 | 100.0 | no |
| child_count | 98.2 / 56.8 | 290 | 100.0 | no |
| handle_count_child | 97.8 / 54.5 | 292 | 100.0 | no |
| child_count_handle | 97.8 / 61.1 | 295 | 100.0 | no |
| all_handle_first | 97.8 / 49.1 | 296 | 100.0 | no |
| all_child_first | 97.8 / 57.1 | 294 | 100.0 | no |

Every object differs from accepted and each other, and every required source
score regresses from 98.4. Some strict gains do not justify retention. Two
forms have original raw 291 instructions but only 98.0 source. No trial kept;
production source restored. Full source/fixture comparison to bf0243e8 and
baseline outside-body/signature checks pass.

Read-only recheck binds existing normalcenter-full evidence: full artifact,
actual 2079 linked/959 byte pair sets equal baseline without lost/gained pairs,
204 objects/202 unaffected objects, winner identity, canonical flag digest/
argument list and fixed-three matcher rows from actual instruction lists.
No new native/full-corpus run claimed for rejected forms. Accepted means
**98.37% unweighted /98.36% weighted**; both >=99.0 gates remain unmet, goal
active. No matcher, denominator, original weights, unrelated body or ABI layout
change. No push, PR or merge.

Evidence: `_work/create_child_parse_state_identities.json`, eight focused
reports/source snapshots and rechecked normalcenter validation audit.


## Continuation: color loop forms on accepted normal-center source

Nine Read trials start from clean 5e09dfc0. Six revisit previously tested color
loop forms on the materially changed accepted normal-center body: signed index,
separate byte offset, advancing input pointer, postincrement destination index,
explicit value/increment, and guarded do. Three new forms point immediately
after the input/output scalar or both, using [-1]. The per-component color
field read remains inside each iteration; no owner/count capture across a
helper is introduced. Loop bound remains the existing freshly read vertexCount
times four at the condition, component traversal and scalar division by 255
are preserved. No extra array operation, clamp, initialization or dummy effect.

Maximum component bound is 4*65535=262140, and maximum byte offset is 1048556,
so signed int index and uint byte offset remain in range. Advancing input ends
at one-past and per-element end pointers are formed only inside executing
iterations; zero count forms none. A byte/ushort flattened component counter
is not used because it could overflow the valid range. No source pointer
declaration is claimed recovered from original C++.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| signed_index | 98.4 / 56.8 | 877 | 100.0 | yes |
| dynamic_byte_offset | 98.4 / 56.9 | 877 | 100.0 | no |
| input_pointer | 98.4 / 56.8 | 877 | 100.0 | yes |
| postincrement_store | 98.4 / 56.8 | 877 | 100.0 | yes |
| value_then_increment | 98.4 / 56.8 | 877 | 100.0 | yes |
| guarded_do | 98.3 / 56.3 | 878 | 100.0 | no |
| output_end | 98.4 / 56.8 | 877 | 100.0 | yes |
| input_end | 98.4 / 56.8 | 877 | 100.0 | yes |
| both_end | 98.4 / 56.8 | 877 | 100.0 | yes |

Seven forms produce the exact accepted whole Core object. Separate byte offset
has unchanged rounded source and 0.1 strict gain but different bytes; it adds
no required score gain and is not retained. Guarded do regresses source.
All rejected; production and relevant test source restored and compared in
full to bf0243e8. Baseline outside-body/signature check passes.

Existing normalcenter-full evidence is rebound by read-only checks: full JSON,
actual linked 2079/byte 959 pair sets equal baseline without lost/gained pairs,
204 objects/202 unaffected objects, winner identity, canonical flag digest/
argument list and fixed-three matcher components from actual instruction lists.
No new native or full-corpus execution claimed for rejected forms. Accepted
**98.37% unweighted /98.36% weighted**; both >=99.0 gates remain unmet, goal
active. No other body, header/ABI, matcher, denominator, original weights, push,
PR or merge change.

Evidence: `_work/read_center_color_loop_identities.json`, focused per-trial
reports/source snapshots and rechecked normalcenter validation audit.


## Continuation: local tangent input and intermediate records

Eight Read trials start from clean a2e60390. Local input records group actual
normal Vector and accumulated tangent float[3], with both member orders.
Local math records group actual scaled/orthogonal/normalized vectors, optionally
also the dot result. Input-only, math-only and combined forms are tested. All
fields hold used existing intermediates; no padding field, explicit alignment,
packing, unused member, helper, zero initialization or dummy operation.

Vector has a custom assignment operator, so fields are initialized directly
by braced aggregate initialization rather than default declaration followed
by Vector assignment. Intermediate initializers access preceding already
initialized fields in declaration order, preserving Dot/product/subtraction/
Normalize dependency order. No field is used before initialization. Cross and
its fresh binormal field load remain after the existing tangent output writes.
Normal-first input initialization retains original scalar load order; reversed
input member order reverses the independent normal/accumulator input loads
before any helper. It does not move a file read or helper. This ordering
distinction is explicit; no original C++ aggregate declaration is claimed.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| inputs_normal_first | 97.6 / 53.1 | 878 | 98.2 | no |
| inputs_tangent_first | 97.2 / 53.4 | 892 | 100.0 | no |
| math_vectors | 98.1 / 54.0 | 879 | 100.0 | no |
| both_vectors_normal_first | 97.2 / 56.1 | 898 | 100.0 | no |
| both_vectors_tangent_first | 98.1 / 56.0 | 880 | 100.0 | no |
| math_dot_vectors | 98.2 / 57.0 | 885 | 100.0 | no |
| both_dot_vectors_normal_first | 96.8 / 56.9 | 882 | 98.2 | no |
| both_dot_vectors_tangent_first | 97.4 / 56.1 | 877 | 98.2 | no |

All eight whole Core objects differ from accepted and each other; every source
score regresses from 98.4. None retained; production source restored. Static
BL/BLX and relocation call differences are recorded independently for all eight
forms, including register-selected indirect calls. A reduced static site count
is not evidence that an executed cleanup is missing. No native execution or
ABI correctness is claimed for rejected aggregate forms.

The three 98.2-call-score forms have 60 sites versus accepted 61, with exactly
one fewer static `_ZdaPv` site. Direct mathematical/helper call inventories do
not change; indirect register differences are r5 ->sl/r6 at three sites.

Full production/relevant fixture text equals bf0243e8; baseline outside-body/
signature checks pass. Read-only recheck verifies existing full artifact,
actual 2079 linked/959 byte pair sets equal baseline without lost/gained pairs,
204 objects/202 unaffected objects, winner identity, canonical flag digest/
arguments and fixed-three matcher rows from actual target/base instructions.
No new native/full-corpus run claimed. Accepted **98.37% unweighted /98.36%
weighted**, both >=99.0 gates unmet; goal active. No header/ABI, unrelated body,
matcher, denominator or original weight change. No push, PR or merge.

Evidence: `_work/read_tangent_local_records_identities.json`, eight focused
reports/source snapshots, `_work/tangent_local_record_call_differences.json`
and rechecked normalcenter validation audit.


## Continuation: bounds records on accepted normal-center source

Eight Read trials start from clean 6b20a4b0. Two-array local records group
the existing min/max float[3] arrays in both orders, accessed through array
references or directly by members. Four further member orders include the
existing center float[3]. Every array and component is used by the original
bounds computation; no dummy member, explicit padding/alignment/packing,
constructor, helper or extra initialization is introduced. Center storage may
begin its lifetime earlier but receives no new initialization or operation
before its existing assignments. All original min then max sentinel stores,
file reads, per-component bounds updates, center calculations, Vector copy,
maximum-minus-minimum subtraction and full-diagonal Length call remain in
their existing order. No half-diagonal radius is substituted.

Two reference/direct pairs yield byte-identical whole Core objects, proving
those aliases add no code differences for each field order. Including center
is a new grouping, not an original recovered structure declaration. The other
two-array forms revisit pre-normal-improvement hypotheses on the materially
changed current function. The type remains local, not a native ABI/header change.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| min_max_refs | 97.7 / 54.8 | 880 | 100.0 | no |
| max_min_refs | 97.7 / 54.9 | 880 | 100.0 | no |
| min_max_direct | 97.7 / 54.8 | 880 | 100.0 | no |
| max_min_direct | 97.7 / 54.9 | 880 | 100.0 | no |
| min_max_center | 96.3 / 52.2 | 893 | 98.2 | no |
| center_min_max | 97.4 / 53.9 | 881 | 100.0 | no |
| max_min_center | 96.3 / 52.7 | 893 | 98.2 | no |
| max_center_min | 97.7 / 54.5 | 878 | 100.0 | no |

Every trial regresses source from 98.4; none retained. Two call-score regressions
are independently inspected as static BL/BLX+relocation inventories; no missing
executed cleanup or native correctness claim follows from static site counts.
Both have 62 static call sites versus accepted 61: exactly one additional
`_ZdaPv` site, with direct mathematical/helper call inventories unchanged.
Production source restored, full source/fixture comparison to bf0243e8 and
baseline outside-body/signature checks pass.

Read-only recheck binds existing full artifact, actual linked 2079/byte 959
pair sets equal baseline without lost/gained pairs, 204 objects/202 unaffected
objects, winner identity, canonical flag digest/argument list and fixed-three
matcher components from actual target/base instruction lists. No new native
or full-corpus run claimed for rejected forms. Accepted **98.37% unweighted
/98.36% weighted**; both >=99.0 gates remain unmet, goal active. No matcher,
denominator, original weights, unrelated body or ABI change. No push, PR or merge.

Evidence: `_work/read_current_bounds_records_identities.json`, per-trial focused
reports/source snapshots, `_work/current_bounds_record_call_differences.json`
and rechecked normalcenter validation audit.


## Continuation: normal intermediate arrays and records

Ten Read trials start from clean 2ee1f3ed. Each of four existing scalar triples
is grouped into a local array[3] or trivial three-field record: decoded floats,
normalized float quotients, lower-bounded doubles, and output floats. Each stage
is tested separately, then all four stages together for both representations.
Every field/component replaces a used existing value; no unused field, padding,
alignment directive, initialization to zero, helper, constructor/destructor or
dummy operation. Initializer clauses retain X/Y/Z evaluation order. Existing
double input multiplication, Y-square plus X-square plus Z-square order, sqrt,
division order, double lower comparison/selection, float casts and upper tests
are unchanged. Output triple remains mutable for the original upper tests.
Fresh owner loads, accepted middle-component destination and fallback stay at
their existing source points. No file read or failure behavior is altered.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| array_decoded | 98.4 / 56.8 | 877 | 100.0 | yes |
| array_quotients | 98.4 / 56.8 | 877 | 100.0 | yes |
| array_bounded | 98.4 / 56.8 | 877 | 100.0 | yes |
| array_output | 98.4 / 56.8 | 877 | 100.0 | yes |
| array_all | 98.4 / 56.8 | 877 | 100.0 | yes |
| record_decoded | 98.4 / 56.8 | 877 | 100.0 | yes |
| record_quotients | 98.4 / 56.8 | 877 | 100.0 | yes |
| record_bounded | 98.4 / 56.8 | 877 | 100.0 | yes |
| record_output | 98.4 / 56.8 | 877 | 100.0 | yes |
| record_all | 98.4 / 56.8 | 877 | 100.0 | yes |

All ten complete Core objects equal the accepted object byte-for-byte, not
merely by score or size. None retained: these representations add no required
gain. This closes the tested local grouping family on this source/compiler;
it is not proof of a general compiler ceiling or a recovered original type.
Production source restored; full source/fixture comparison to bf0243e8 and
baseline outside-body/signature check pass.

Read-only recheck binds existing full JSON, actual linked 2079/byte 959 pair
sets equal baseline without lost/gained pairs, 204 objects/202 unaffected
objects, winner identity, canonical flag digest/argument list, and fixed-three
matcher rows from target/base instructions. No new native/full-corpus run
claimed for nonretained forms. Accepted **98.37% unweighted /98.36% weighted**,
both >=99.0 gates remain unmet; goal active. No unrelated body, header/ABI,
matcher, denominator, original weights, push, PR or merge change.

Evidence: `_work/read_normal_value_records_identities.json`, ten per-trial
focused reports/source snapshots and rechecked normalcenter validation audit.


## Continuation: common failure exits and current normal branches

Seven focused Read trials start from dc74484a and the accepted middle-component
normal destination. Two common-return forms replace existing return -1 paths
with a shared label: final enhanced/child failures only, then all failures.
The latter uses an inner body scope so jumps leave existing declarations.
Existing reads, conditions, explicit cleanup and successful return remain;
no new cleanup or initialized operation is introduced.

Five normal-branch forms revisit fallback-first, zero/nonzero continue,
named nonzero condition and owner-load-before-branch on the current accepted
source. Continue reaches the same loop increment; nothing follows the branch
in the loop. The owner-before-branch trial moves the existing normals field
load before the length test and clamp calculations, with no intervening helper.
This load-order variation is a source-shape hypothesis, not a recovered original
capture point. No header, ABI, helper or compilation flag is changed.

| Family / trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| common failure / tail_label | 98.4 / 56.8 | 877 | 100.0 | yes |
| common failure / all_label | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal branch / fallback_first | 98.2 / 59.1 | 877 | 100.0 | no |
| normal branch / zero_continue | 98.2 / 59.1 | 877 | 100.0 | no |
| normal branch / nonzero_continue | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal branch / named_nonzero | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal branch / owner_before_branch | 98.1 / 57.8 | 874 | 100.0 | no |

Four objects equal the accepted complete Core object byte-for-byte. The three
other forms regress source-shape; fallback_first and zero_continue also equal
each other byte-for-byte. Their strict-score increases do not satisfy the
requested source-shape gate. None retained; production sources and fixtures
match bf0243e8, and outside-body/signature comparisons pass.

Read-only recheck binds the existing full-corpus report to restored source:
204 objects, 202 unaffected objects equal baseline, actual linked-exact 2079
and byte-exact 959 pair sets unchanged with no lost/gained pairs, canonical
flags unchanged, and fixed-three rows recomputed with the unchanged matcher.
No new native/full-corpus run is claimed for rejected forms. Accepted means
remain **98.37% unweighted /98.36% original-instruction-weighted**. Both >=99.0
requirements remain unmet; this is not evidence of a ceiling. No push, PR or merge.

Evidence: `_work/read_common_failure_identities.json`,
`_work/read_current_normal_branch_identities.json`, seven focused reports and
source snapshots, and rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: clamp scheduling with centered normal destinations

Six focused Read trials start from 837a7f30. They retest the old clamp-form
and quotient-scheduling family with the accepted middle-component output
pointer: explicit lower-double if statements, upper ternaries, complete
component clamp staging, Y/X/lower-Y/Z quotient staging, Y-first casts/upper
tests, and interleaved promotion/lower-if. Float quotients, double lower
selection, float casts and upper comparisons on the original float quotients
remain. No arithmetic is reassociated, no helper added, no file read or field
load moved. Independent component evaluation order is varied explicitly in
the scheduling forms. These are source hypotheses, not recovered declarations.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| explicit_lower_double | 97.6 / 54.1 | 862 | 100.0 | no |
| upper_ternary | 98.4 / 56.8 | 877 | 100.0 | yes |
| component_clamps | 98.4 / 56.8 | 877 | 100.0 | yes |
| staged_y_x_z | 98.4 / 56.8 | 877 | 100.0 | yes |
| staged_y_clamps | 98.4 / 56.8 | 877 | 100.0 | yes |
| interleaved_lower_if | 97.6 / 54.1 | 862 | 100.0 | no |

Four complete Core objects equal the accepted object byte-for-byte. Both
explicit lower-if forms equal each other and regress source-shape to 97.6;
none retained. This resolves the tested interaction with centered destinations
without claiming a general compiler ceiling. Production source/fixtures match
bf0243e8 and outside-body/signature comparisons pass.

Read-only recheck binds the existing full evidence to restored source and
recomputes the fixed-three matcher rows: 204 objects, 202 unaffected objects
byte-identical to baseline, actual linked-exact 2079 and byte-exact 959 pair sets
unchanged without lost/gained pairs, canonical compilation arguments unchanged.
No new native/full-corpus run is claimed for rejected/restored trials. Accepted
means remain **98.37% unweighted /98.36% original-instruction-weighted**;
both >=99.0 gates remain unmet and goal remains active. No push, PR or merge.

Evidence: `_work/read_current_clamp_identities.json`, six
`_work/readcurrentclamp-*.json` focused reports, corresponding source snapshots,
and rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: compressed normal and UV input value copies

Twelve focused Read trials start from 4dc42858. Each copies the actual normal
X/Y/Z or UV U/V input components into local const scalars, an array, or a trivial
record, using short or value-preserving signed int. Every member is used; no
padding, reinterpretation of the input allocation as a record, extra helper,
unused field, header or ABI change. Initializer clauses preserve component order.
Normal copies move the three loads before their independent conversions. UV
copies move the second input load before the first output store: the raw input
and output are separately allocated, with no intervening call. Double scaling,
float casts, flip decision and stores remain in their existing order. These
representations are source hypotheses, not recovered original declarations.

| Family / trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| normal / short_scalars | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal / short_array | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal / short_record | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal / int_scalars | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal / int_array | 98.4 / 56.8 | 877 | 100.0 | yes |
| normal / int_record | 98.4 / 56.8 | 877 | 100.0 | yes |
| UV / short_scalars | 98.3 / 55.8 | 876 | 100.0 | no |
| UV / short_array | 98.3 / 55.8 | 876 | 100.0 | no |
| UV / short_record | 98.3 / 55.8 | 876 | 100.0 | no |
| UV / int_scalars | 98.3 / 55.8 | 876 | 100.0 | no |
| UV / int_array | 98.3 / 55.8 | 876 | 100.0 | no |
| UV / int_record | 98.3 / 55.8 | 876 | 100.0 | no |

All six normal objects equal the accepted complete Core object byte-for-byte.
All six UV objects equal each other byte-for-byte and regress source-shape from
98.4 to 98.3. None retained; production source/fixtures match bf0243e8, and
outside-body/signature comparisons pass. This eliminates these tested value-copy
forms on the current source/compiler without claiming a general ceiling.

Read-only recheck binds existing full evidence to restored production source:
204 objects, 202 unaffected objects byte-identical to baseline, actual linked
2079/byte 959 exact pair sets unchanged with no lost/gained pairs, unchanged
canonical flags and fixed-three matcher rows recomputed from instruction lists.
No new native/full-corpus run claimed for rejected trials. Accepted means remain
**98.37% unweighted /98.36% original-instruction-weighted**. Both >=99.0 gates
remain unmet; goal active. No push, PR or merge.

Evidence: `_work/read_normal_input_copy_identities.json`,
`_work/read_uv_input_copy_identities.json`, twelve focused reports and snapshots,
and rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: Enhanced parser state records

Fresh unchanged-matcher inventory inspection of accepted Enhanced shows remaining
local load/store differences. Six focused trials from 0582caaa group real used
parser state into a local record: mesh/animation pointers, file/format,
file/animation, mesh/file, then all four values in two member orders. No unused
field, explicit padding/alignment, dummy operation, added helper, header or ABI
change. Record members replace existing variable uses directly. Initialization
stays before the first read; new Transform retains its order relative to calls,
and only pure argument/self/format captures change order. No file read, time
update, key insertion, cleanup or attachment changes. Member representation is
a source-shape hypothesis, not an original-source recovery.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| self_anim | 98.3 / 66.9 | 641 | 100.0 | yes |
| file_format | 98.3 / 66.9 | 641 | 100.0 | yes |
| file_anim | 98.3 / 66.9 | 641 | 100.0 | yes |
| self_file | 98.3 / 66.9 | 641 | 100.0 | yes |
| all_source_order | 98.3 / 66.9 | 641 | 100.0 | yes |
| all_file_first | 98.3 / 66.9 | 641 | 100.0 | yes |

All six complete Mesh objects equal accepted byte-for-byte; none retained.
These local record forms do not change the residual inventory or the required
score on the current compiler/source. This does not establish a general ceiling.
Production sources/fixtures match bf0243e8; outside-body/signature checks pass.

Read-only recheck binds existing full evidence to restored source: 204 objects,
202 unaffected objects byte-identical to baseline, actual linked-exact 2079 and
byte-exact 959 pair sets equal baseline without lost/gained pairs, unchanged
canonical arguments, fixed-three rows recomputed with the unchanged matcher.
No new native/full-corpus run claimed for nonretained forms. Accepted means
remain **98.37% unweighted /98.36% original-instruction-weighted**; both >=99.0
requirements remain unmet and goal remains active. No push, PR or merge.

Evidence: `_work/enh_parser_state_identities.json`, six
`_work/enhparserstate-*.json` focused reports and source snapshots, diagnostic
normalized instruction lists, and rechecked normalcenter validation audit.


## Continuation: caller argument and handle/output records

Fresh unchanged-matcher inventory inspection identifies remaining Create
pointer-load/local-save differences. Seven focused trials start from fa0a5bf9:
records hold output/material, engine/material, engine/output, all four arguments
in forward/reverse order, or handle/output in two member orders. Every member
replaces a used existing value; no unused field, explicit padding/alignment,
helper, header or ABI change. Local captures are pure argument copies. Handle
zero initialization moves to entry in the last two forms, before the existing
engine/path guard, without new calls or observable operations. OpenRead receives
the actual used handle member address; reads, child handling, ownership cleanup
and loaded completion preserve their existing order. These are source-shape
hypotheses, not recovered original state declarations.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| out_material | 98.4 / 60.7 | 289 | 100.0 | yes |
| engine_material | 98.4 / 60.7 | 289 | 100.0 | yes |
| engine_output | 98.4 / 60.7 | 289 | 100.0 | yes |
| all_arguments | 98.4 / 60.7 | 289 | 100.0 | yes |
| arguments_reverse | 98.4 / 60.7 | 289 | 100.0 | yes |
| handle_output | 96.9 / 61.7 | 296 | 100.0 | no |
| output_handle | 96.6 / 58.5 | 297 | 100.0 | no |

Five argument-only objects equal accepted complete Core byte-for-byte. Both
handle/output records differ from accepted and each other, regressing source
to 96.9/96.6. The first increases strict score only, which does not meet the
requested source-shape gate. None retained; source/fixtures match bf0243e8 and
outside-body/signature checks pass. No general compiler ceiling is inferred.

Read-only recheck binds existing full evidence to restored production source:
204 objects, 202 unaffected objects byte-identical to baseline, actual linked
2079 and byte 959 exact pair sets unchanged without lost/gained pairs, canonical
arguments unchanged, and fixed-three matcher rows recomputed directly. No new
native/full-corpus run claimed for rejected forms. Accepted means remain
**98.37% unweighted /98.36% original-instruction-weighted**; both >=99.0 gates
remain unmet; goal active. No push, PR or merge.

Evidence: `_work/create_call_state_identities.json`, seven
`_work/createcallstate-*.json` focused reports and source snapshots, current
normalized instruction inventory, rechecked normalcenter validation audit.


## Continuation: Enhanced addressed axis/count state

Eight focused Enhanced trials start from dc1dc973. They group actual used
unsigned-char axis and unsigned-short count into one trivial local record,
in both member orders, for position, scale, rotation or all three sections.
Each count read still writes exactly two bytes to the existing logical count;
axis initialization/increment retain byte type and closed 0/1/2 domain.
Count is used only after successful input, and frame/key reads, timing update,
channel selection, negation, insertion and cleanup preserve existing order.
No record zero initialization, unused member, explicit padding/alignment,
helper, header or ABI change. Member grouping is a source-shape hypothesis.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| axis_count_0 | 97.2 / 51.9 | 647 | 100.0 | no |
| axis_count_1 | 97.2 / 40.7 | 647 | 100.0 | no |
| axis_count_2 | 97.3 / 21.7 | 647 | 100.0 | no |
| axis_count_all | 94.6 / 43.9 | 658 | 100.0 | no |
| count_axis_0 | 97.3 / 55.8 | 644 | 100.0 | no |
| count_axis_1 | 97.3 / 43.6 | 644 | 100.0 | no |
| count_axis_2 | 97.5 / 23.3 | 645 | 100.0 | no |
| count_axis_all | 95.0 / 39.8 | 650 | 100.0 | no |

All eight complete Mesh objects differ from accepted and each other, with
source regression and increased instruction counts. None retained. Passing
an addressed subobject of this record changes the compiler's alias context;
this is a plausible explanation of extra local loads, not proof of recovered
original layout or a general optimizer ceiling. Production sources/fixtures
match bf0243e8; outside-body/signature checks pass.

Read-only recheck binds existing full evidence to restored source: 204 objects,
202 unaffected objects byte-identical to baseline, actual linked 2079/byte 959
exact pair sets equal baseline without lost/gained pairs, unchanged canonical
arguments and fixed-three matcher rows recomputed directly. No new native/full
corpus run claimed for rejected forms. Accepted means remain **98.37% unweighted
/98.36% original-instruction-weighted**; both >=99.0 requirements unmet; goal
active. No push, PR or merge.

Evidence: `_work/enh_axis_count_state_identities.json`, eight
`_work/enhaxiscountstate-*.json` focused reports/source snapshots, and rechecked
`_work/normalcenter_validation_audit.json`.


## Continuation: position pointer offsets and advancing traversal

Fifteen focused Read trials start from a9f7de3a. Nine derive per-iteration input,
output or both pointers one element past the accessed position, independently
for short/int branches or both. Access remains [-1] at the original component;
pointers are formed only in executing iterations and reach at most one-past
the full 3*vertexCount allocation. Existing field loads remain at their source
points. No out-of-allocation pointer is formed.

Six further trials increment input pointers, or both input/output pointers,
after each component. Index i remains for the same bounds-axis computation.
The output pointer is captured before the loop for both-advance forms: the
loop contains no calls or writes to the mesh position-pointer field, and the
output allocation is separate from the mesh/input. This capture is an explicit
source hypothesis, not a claim about original owner-load timing. Raw allocation
owner remains unchanged for deletion, including failure paths. Float conversion,
extrema operations, reads, counts and cleanup source order remain unchanged.
No dummy operation, helper, header, ABI, flag or matcher change.

| Family / trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| one-past / input_short | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / input_int | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / input_both_branches | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / output_short | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / output_int | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / output_both_branches | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / both_short | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / both_int | 98.4 / 56.8 | 877 | 100.0 | yes |
| one-past / both_both_branches | 98.4 / 56.8 | 877 | 100.0 | yes |
| advance / input_advance_short | 98.4 / 56.8 | 877 | 100.0 | yes |
| advance / input_advance_int | 98.4 / 56.8 | 877 | 100.0 | yes |
| advance / input_advance_both_branches | 98.4 / 56.8 | 877 | 100.0 | yes |
| advance / both_advance_short | 97.6 / 55.9 | 883 | 98.2 | no |
| advance / both_advance_int | 98.4 / 52.1 | 874 | 100.0 | no |
| advance / both_advance_both_branches | 98.2 / 55.9 | 875 | 100.0 | no |

All nine one-past forms and three input-only advancing forms equal accepted
complete Core byte-for-byte. Three both-advance objects differ, with no required
source gain: short and combined branches regress, int ties the rounded source
score but regresses strict. None retained. Static relocation inspection of the
short both-advance trial finds 62 calls versus accepted 61, with exactly one
extra _ZdaPv site and no lost call target. This is static layout evidence, not
proof of extra cleanup on an executed path. No operation added to match counts.

Production source/fixtures match bf0243e8; outside-body/signature checks pass.
Read-only full-evidence binding rechecks 204 objects/202 unaffected objects,
actual linked 2079/byte 959 exact pair sets equal baseline without lost/gained
pairs, unchanged canonical arguments, and directly recomputed fixed-three rows.
No new native/full-corpus run claimed for rejected forms. Accepted means remain
**98.37% unweighted /98.36% original-instruction-weighted**; both >=99.0 gates
remain unmet; goal active. No general ceiling, push, PR or merge claim.

Evidence: `_work/read_position_one_past_identities.json`,
`_work/read_position_advance_identities.json`, fifteen focused reports/snapshots,
`_work/position_advance_short_call_difference.json`, rechecked validation audit.


## Continuation: recursive reader child-state records

Twelve focused Read trials start from 41d4682f. They group actual childCount,
unsigned counter c and childPtr into local records: each two-member pair in
both orders, then all six orders of the three members. Every member replaces
an existing used value. No unused field, zero fill, explicit padding/alignment,
helper, header or ABI change. Count read stays after enhanced parsing and uses
two bytes; counter initialization stays after existing animation bounds Merge;
child assignment/new occurs inside the same iteration. Recursive call receives
the actual used child member address. Its scope extends to this child section,
with no RAII effect for primitive pointer. Dynamic count comparison, parent
reloads before Merge/ArrayAdd, failure exits and cleanup remain in source order.
These local layouts are hypotheses, not recovered declarations.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| pair_childCount_c | 97.2 / 54.5 | 875 | 98.2 | no |
| pair_c_childCount | 97.2 / 54.4 | 876 | 98.2 | no |
| pair_childCount_childPtr | 97.9 / 55.9 | 881 | 100.0 | no |
| pair_childPtr_childCount | 98.2 / 56.8 | 878 | 100.0 | no |
| pair_c_childPtr | 97.1 / 54.4 | 877 | 98.2 | no |
| pair_childPtr_c | 97.2 / 54.4 | 876 | 98.2 | no |
| all_childCount_c_childPtr | 97.1 / 54.4 | 877 | 98.2 | no |
| all_childCount_childPtr_c | 97.1 / 54.4 | 877 | 98.2 | no |
| all_c_childCount_childPtr | 97.1 / 54.4 | 877 | 98.2 | no |
| all_c_childPtr_childCount | 97.1 / 54.4 | 877 | 98.2 | no |
| all_childPtr_childCount_c | 97.1 / 54.4 | 877 | 98.2 | no |
| all_childPtr_c_childCount | 97.1 / 54.4 | 877 | 98.2 | no |

All twelve complete Core objects differ from accepted and each other, and
regress source-shape. None retained. Ten counter-containing forms have 60
static calls versus accepted 61: exactly one fewer _ZdaPv site, and the three
indirect call registers change r5 to r6. Both count/child-pointer pairs preserve
all 61 call-site targets. This is static allocation/layout evidence, not proof
of missing cleanup on an executed path. No cleanup operation added for scoring.
No general compiler ceiling is inferred.

Source/fixtures match bf0243e8; outside-body/signature checks pass. Read-only
full evidence binding verifies 204 objects/202 unaffected objects, actual linked
2079/byte 959 exact pair sets equal baseline without lost/gained pairs, canonical
arguments unchanged and fixed-three rows recomputed directly. No new native/full
corpus run claimed for rejected forms. Accepted means remain **98.37% unweighted
/98.36% original-instruction-weighted**; both >=99.0 gates unmet; goal active.
No push, PR or merge.

Evidence: `_work/read_child_state_identities.json`, twelve focused reports and
snapshots, `_work/read_child_state_call_differences.json`, rechecked validation
audit.


## Continuation: inner key-loop forms on accepted byte-axis code

Twelve focused Enhanced trials start from 0547d0d8. The older inner-key family
was measured before accepted byte-axis counters. This series retests while and
guarded-do for all vector keys, all axis keys and optional keys, then guarded-do
individually for each of three vector sections and three axis sections. Initial
count >0 guards skip zero unsigned counts; every loop still retests the same
count after increment. Counters remain unsigned int with maximum bound 65535,
so no increment overflow. All reads, timing update, channel selection, key
conversion/insertion, cleanup and outer byte-axis loops retain their order.
No helper, header, ABI, flag, matcher or denominator change. Alternative loop
syntax is a source hypothesis, not a recovered original control flow.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| vector_while | 98.3 / 66.9 | 641 | 100.0 | yes |
| vector_guarded_do | 97.3 / 68.5 | 642 | 100.0 | no |
| vector_guarded_do_0 | 97.9 / 69.3 | 640 | 100.0 | no |
| vector_guarded_do_1 | 98.0 / 53.8 | 642 | 100.0 | no |
| vector_guarded_do_2 | 98.0 / 66.7 | 642 | 100.0 | no |
| axis_key_while | 98.3 / 66.6 | 641 | 100.0 | no |
| axis_key_guarded_do | 96.9 / 61.7 | 647 | 100.0 | no |
| axis_key_guarded_do_0 | 98.1 / 52.4 | 642 | 100.0 | no |
| axis_key_guarded_do_1 | 98.0 / 66.4 | 642 | 100.0 | no |
| axis_key_guarded_do_2 | 98.0 / 66.3 | 643 | 100.0 | no |
| optional_while | 98.3 / 66.9 | 641 | 100.0 | yes |
| optional_guarded_do | 98.0 / 66.7 | 642 | 100.0 | no |

Vector/optional while forms equal the accepted complete Mesh byte-for-byte.
Axis-key while differs, tying rounded source but lowering strict score; all
nine guarded-do forms regress required source score. None retained. Higher
strict in vector guarded-do variants does not satisfy the source-shape gate.
These results exclude the tested interaction on current byte-axis code, not a
general compiler ceiling. Production sources/fixtures match bf0243e8 and
outside-body/signature checks pass.

Read-only binding rechecks existing full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline without
lost/gained pairs, unchanged canonical arguments, directly recomputed fixed-three
rows. No new native/full-corpus run claimed for rejected forms. Accepted means
remain **98.37% unweighted /98.36% original-instruction-weighted**; both >=99.0
requirements remain unmet; goal active. No push, PR or merge.

Evidence: `_work/enh_current_key_loop_identities.json`, twelve focused reports
and source snapshots, rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: remaining-record key counters

Thirteen focused Enhanced trials start from 2a5254f1. They replace unused
ascending inner indices with remaining-record counters initialized from the
parsed unsigned-short bound, testing unsigned int/int/unsigned short for vector,
axis and optional groups, three individual axis sections, and all groups with
unsigned int. The original i/j has no body use, verified before replacement.
Each key is still read/inserted in stream order; count zero executes no body,
maximum 65535 fits all types and the condition tests zero before another decrement,
so neither overflow nor underflow is introduced. Outer axis and signed V5-key
loops stay unchanged. No dummy index, helper, header, ABI, flag or matcher change.

The remaining bound is captured before the loop instead of rereading the
existing count each test. This is an explicit stability assumption about normal
file-parser operation, not recovered original load timing or arbitrary callback
equivalence. Inspected current AEFile.cpp, AENormalFile.cpp and AEPakFile.cpp
Read methods forward the supplied buffer, and FileInterfaceAndroid.cpp writes
that buffer through zip_fread/fread/JNI region copying; they do not retain the
previous count pointer for later key reads. No local count assignment occurs
in key-loop bodies. These are current implementation observations, not native
runtime or original ABI proof.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| vector_unsigned_int | 98.0 / 52.5 | 641 | 100.0 | no |
| vector_int | 98.0 / 52.5 | 641 | 100.0 | no |
| vector_unsigned_short | 97.5 / 52.4 | 638 | 100.0 | no |
| axis_unsigned_int | 97.9 / 66.1 | 641 | 100.0 | no |
| axis_int | 97.9 / 66.1 | 641 | 100.0 | no |
| axis_unsigned_short | 97.3 / 66.2 | 638 | 100.0 | no |
| optional_unsigned_int | 98.3 / 66.4 | 641 | 100.0 | no |
| optional_int | 98.3 / 66.4 | 641 | 100.0 | no |
| optional_unsigned_short | 98.1 / 66.5 | 640 | 100.0 | no |
| axis_individual_0 | 98.2 / 66.7 | 641 | 100.0 | no |
| axis_individual_1 | 98.2 / 66.7 | 641 | 100.0 | no |
| axis_individual_2 | 98.2 / 66.7 | 641 | 100.0 | no |
| all_unsigned_int | 96.7 / 51.5 | 641 | 100.0 | no |

All objects differ from accepted; int/unsigned-int objects equal each other
within each vector/axis/optional group. Required source score does not improve;
optional wide forms tie the rounded source score while lowering strict. None
retained. These results exclude the tested traversal forms, not a general ceiling.
Source/fixtures match bf0243e8, and outside-body/signature checks pass.

Read-only full-evidence binding verifies 204 objects/202 unaffected objects,
actual linked 2079/byte 959 exact pair sets equal baseline without lost/gained
pairs, unchanged canonical arguments, directly recomputed fixed-three rows.
No new native/full-corpus run claimed for rejected forms. Accepted means remain
**98.37% unweighted /98.36% original-instruction-weighted**; both >=99.0 gates
unmet; goal active. No push, PR or merge.

Evidence: `_work/enh_key_remaining_identities.json`, thirteen focused reports
and snapshots, rechecked `_work/normalcenter_validation_audit.json`.


## Continuation: combined UV/color stream guards

Nine focused Read trials start from 73a6056a. For UV/color separately or together,
three source forms flatten the outer vertexFormat guard and inner compressed/plain
branch: a combined if/else-if chain, the same chain with a local bool for the
format condition, or two independent combined guards testing compressedData
first. Independent plain guard also requires compressedData ==0, so it cannot
execute after compressed conversion and avoids a new format load on that path.
Repeated format tests in flat chains have no intervening call/write. Buffers,
reads, conversions, failure cleanup and body store order remain unchanged.
No shared state is captured across a callback for these decisions. No helper,
header, ABI, flag, matcher or denominator change. These branch structures are
source hypotheses, not recovered original guards.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| flat_uv | 97.6 / 51.3 | 879 | 98.2 | no |
| flat_color | 97.7 / 55.2 | 885 | 100.0 | no |
| flat_both | 97.2 / 49.0 | 882 | 98.2 | no |
| predicate_uv | 97.6 / 51.3 | 879 | 98.2 | no |
| predicate_color | 97.7 / 55.2 | 885 | 100.0 | no |
| predicate_both | 97.2 / 49.0 | 882 | 98.2 | no |
| independent_uv | 97.8 / 53.6 | 881 | 98.2 | no |
| independent_color | 97.6 / 55.1 | 886 | 100.0 | no |
| independent_both | 97.0 / 49.5 | 901 | 100.0 | no |

All nine complete Core objects differ from accepted and regress source-shape.
Flat/predicate variants equal each other byte-for-byte for each scope; none
retained. Five variants with call-count score 98.2 have 60 static calls versus
accepted 61, with exactly one fewer _ZdaPv site. Four others preserve all 61
call-site targets; no indirect register difference appears in this inspection.
This is static control/layout evidence, not proof of missing executed cleanup.
No operation added for scoring and no general compiler ceiling inferred.

Source/fixtures match bf0243e8; outside-body/signature checks pass. Read-only
full evidence binding verifies 204 objects/202 unaffected objects, actual linked
2079/byte 959 exact pair sets equal baseline without lost/gained pairs, canonical
arguments unchanged and directly recomputed fixed-three matcher rows. No new
native/full-corpus run claimed for rejected forms. Accepted means remain
**98.37% unweighted /98.36% original-instruction-weighted**; both >=99.0 gates
unmet; goal active. No push, PR or merge.

Evidence: `_work/read_stream_guard_identities.json`, nine focused reports and
source snapshots, `_work/read_stream_guard_call_differences.json`, rechecked
validation audit.


## Continuation: normal stream and shader section guards

Four focused Read trials start from 52bef9c0. The normal-format decision remains
at the original entry point. Split-shader stores that decision in a used bool,
keeps normal input processing behind it, then moves shader/tangent processing
to a separate guarded block. Flat/independent forms additionally flatten the
compressed/plain normal guards, preserving the same captured format decision.
Engine::enableShader is still read after normal processing, so callbacks are
not bypassed by an early shader capture. Independent plain guard requires
compressedData ==0, preventing execution after compressed input. Common-section
exit instead leaves early when normal format or later shader enable is zero;
all skipped declarations lie inside the exited primitive local scopes.

No read, allocation, cleanup, helper, tangent arithmetic or output-store order
is changed. In particular Dot/Normalize/Cross sequence and tangent stores before
Cross remain. No header, ABI, flag, matcher or denominator change. These are
branch-structure hypotheses, not recovered original flow.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| split_shader | 98.4 / 56.8 | 877 | 100.0 | yes |
| flat_shader | 97.3 / 54.5 | 893 | 100.0 | no |
| independent_shader | 97.0 / 55.5 | 888 | 98.2 | no |
| common_section_exit | 98.4 / 56.8 | 877 | 100.0 | yes |

Split-shader and common-section exit equal accepted complete Core byte-for-byte.
Both flattened forms regress source; none retained. Flat-shader preserves all
61 static call-site targets. Independent-shader has 60 vs accepted 61 calls,
exactly one fewer _ZdaPv site and three indirect calls changing r5 to r8. This
is static layout evidence, not proof of missing executed cleanup. No operation
added for scoring and no general compiler ceiling inferred.

Sources/fixtures match bf0243e8, outside-body/signature checks pass. Read-only
full evidence binding verifies 204 objects/202 unaffected objects, actual linked
2079/byte 959 exact pair sets equal baseline without lost/gained pairs, canonical
arguments unchanged and directly recomputed fixed-three rows. No new native/full
corpus run claimed for rejected forms. Accepted means remain **98.37% unweighted
/98.36% original-instruction-weighted**; both >=99.0 gates unmet; goal active.
No push, PR or merge.

Evidence: `_work/read_normal_guard_identities.json`, four focused reports and
snapshots, `_work/read_normal_guard_call_differences.json`, rechecked validation
audit.


## Continuation: paired typed raw-array allocation and deletion

Six focused Read trials start from 3ab6e32d. Earlier typed-allocation trials
kept explicit operator delete[]. This series pairs array new expressions with
matching typed delete[] expressions, individually for short/int positions,
UV, normal, color, then all five. Types are short/int/short/short/unsigned char;
element counts are 3v/3v/2v/3v/4v, preserving existing byte sizes on the target.
No value initialization or element constructor/destructor added. The temporary
remains held as void*, then converted back to its actual allocated element type
for deletion at each existing success/failure site. No direct array-delete
expression is applied to memory allocated only through operator new[] calls:
matching new[] expressions are changed as well. No extra cleanup operation,
header, ABI, helper, flag or matcher change. This is a source representation
hypothesis, not recovered original allocation syntax.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| short_position | 98.4 / 57.1 | 875 | 100.0 | no |
| integer_position | 98.4 / 57.1 | 875 | 100.0 | no |
| uv | 97.9 / 55.9 | 878 | 98.2 | no |
| normal | 98.4 / 56.8 | 876 | 100.0 | no |
| color | 98.4 / 56.8 | 877 | 100.0 | yes |
| all_streams | 98.1 / 57.1 | 876 | 98.2 | no |

Color equals the accepted complete Core byte-for-byte. Other objects differ;
position pairs improve strict only while tying required source, normal ties
source/strict, and UV/all regress source. None retained. UV and all-streams
have 60 static calls vs accepted 61, with exactly one fewer _ZdaPv site and no
other call-target difference. This is static layout evidence, not proof of
missing executed cleanup. No operation added for scoring or general ceiling
inferred.

Sources/fixtures match bf0243e8 and outside-body/signature checks pass. Read-only
full-evidence binding rechecks 204 objects/202 unaffected objects, actual linked
2079/byte 959 exact pair sets equal baseline without lost/gained pairs, canonical
arguments unchanged, fixed-three matcher rows recomputed directly. No new native
or full-corpus run claimed for rejected forms. Accepted means remain **98.37%
unweighted /98.36% original-instruction-weighted**; both >=99.0 gates unmet;
goal active. No push, PR or merge.

Evidence: `_work/read_typed_pairs_identities.json`, six focused reports and
snapshots, `_work/read_typed_pairs_call_differences.json`, rechecked validation
audit.


## Continuation: retain paired position/normal raw arrays

Four interaction trials start from 33c7ef2c, combining the three previous
nonregressing raw-array forms: short positions, int positions and compressed
normals. All four raise Read from 98.4 to 98.5. Retain all three pairs, which
also gives the highest strict score 57.4 and 874 instructions (original 873).
The production diff has exactly nine substituted statements inside Read:
three allocations and their six existing success/failure deletions. Primitive
new short[3*vertexCount]/new int[3*vertexCount] preserve 6v/12v byte sizes on
the target; no value initialization, zero fill, element constructor/destructor,
additional null guard or cleanup site. Void* raw ownership remains, and delete[]
casts back to the actual allocated primitive element type. This is a source
hypothesis, not recovered original allocation/deletion syntax. Counts, read
sizes/field loads, numeric operations, stores and failure outcomes stay unchanged.
No header, ABI, helper, compilation flag, matcher or denominator change.

| Interaction trial | Read source / strict | Instructions | Calls |
|---|---|---:|---:|
| short_position_integer_position | 98.5 / 57.3 | 875 | 100.0 |
| short_position_normal | 98.5 / 57.1 | 874 | 100.0 |
| integer_position_normal | 98.5 / 57.1 | 874 | 100.0 |
| short_position_integer_position_normal | 98.5 / 57.4 | 874 | 100.0 |

All four complete Core objects differ from each other and the previous accepted
object. Rounded single-form scores did not predict this positive interaction.
Fresh frozen-source validation:

- Native UCRT64 gof2 static-library build passes.
- Verbatim actual Read and caller/actual Mesh/selected math/BSphere fixtures
  pass position/normal/color/tangent, bounds, children and early/truncated input.
- Actual Enhanced parser fixture builds and exits zero.
- Existing fixture sources/runner and Mesh.cpp match bf0243e8; no fixture change.
  This allocation representation introduces no new numeric/parser behavior
  requiring an added fixture. Earlier normal negative controls remain evidence
  of existing fixture sensitivity, not newly run negative controls here.
- Outside-body/signature checks pass. All 204 ARM units rebuilt and full corpus
  verified, with 4619 comparisons and zero skipped. Actual linked-exact 2079
  /byte-exact 959 pair sets equal baseline, lost [] and gained [] for each.
  Other 202 whole objects equal baseline. Core equals focused triple winner;
  Mesh equals accepted axisbyte-full. Canonical digest and argument list unchanged.
- All fixed-three matcher components recomputed directly from fresh target/base
  instruction lists by the unchanged matcher. Already rounded component averages
  are not substituted for the actual source metric.

Read component changes (previous -> retained):

- instruction_inventory_match: 95.0 -> 95.8
- opcode_inventory_match: 98.3 -> 98.1
- control_inventory_match: 98.8 -> 98.8
- ordered_control_flow_match: 91.8 -> 91.8
- call_count_match: 100.0 -> 100.0
- size_match: 99.6 -> 100.0

| Fixed unique body | Source | Strict | Original weight |
|---|---:|---:|---:|
| MeshCreateFromFile | 98.4% | 60.7% | 291 |
| MeshReadData | 98.5% | 57.4% | 873 |
| ReadEnhancedDataFromFile | 98.3% | 66.9% | 636 |

Fixed three means: **98.400000% unweighted /98.413167% weighted**. Original weights remain 291+873+636=1800, exactly three unique bodies.

Full strict/source averages: 75.98 /89.0. Full JSON SHA-256: `69141f6eaafd32d61ce63ae1257ac889bc0cfcc7765c514a630391e0df481af8`.

Both >=99.0 gates remain unmet; goal active. Native body tests mock file/selected
boundaries and library build is not a whole-game executable ABI or Android
runtime/device test. No Godot proof or compiler ceiling claim. No push, PR, merge.

Evidence: `_work/read_typed_pair_interaction_identities.json`, focused reports
and source snapshots, `_work/typedpair_full.json`, `_work/typedpair_three.json`,
`_work/typedpair_validation_audit.json`, `_work/typedpair_full_build.log`,
`_work/typedpair_full_verify.log`, `_work/typedpair_native.log`,
`_work/typedpair_read_smoke.log`, enhanced build/smoke logs.


## Continuation: output arrays and accumulator on paired-raw source

Nine focused Read trials start from ecc30f25. Seven repeat the typed-output
family on the materially changed accepted paired-raw source, for positions,
UVs, normals, colors, tangents/binormals/accumulator, indices, and all outputs.
Primitive new float/unsigned-short arrays preserve existing element/byte counts,
with no value initializer, zero fill or element constructor/destructor. Existing
owned-output field stores and downstream release logic stay unchanged. Two
further forms pair new float[3*vcount] with delete[] accum, separately and with
all outputs; the deallocation operand comes from that matching new[] expression.
No new cleanup, input read, arithmetic, header, ABI, helper or compiler flag.
These representations are hypotheses, not recovered original array syntax.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| output_position | 98.5 / 57.4 | 874 | 100.0 | yes |
| output_uv | 98.5 / 57.4 | 874 | 100.0 | yes |
| output_normal | 98.5 / 57.4 | 874 | 100.0 | yes |
| output_color | 98.5 / 57.4 | 874 | 100.0 | yes |
| output_tangent | 98.5 / 57.4 | 874 | 100.0 | yes |
| output_index | 98.5 / 57.4 | 874 | 100.0 | yes |
| output_all | 98.5 / 57.4 | 874 | 100.0 | yes |
| accum_pair | 98.5 / 57.4 | 874 | 100.0 | yes |
| all_outputs_accum_pair | 98.5 / 57.4 | 874 | 100.0 | yes |

All nine complete Core objects equal accepted byte-for-byte. No variant retained:
these typed outputs/accumulator add no required gain on paired-raw source. This
resolves the tested interaction, not a general compiler ceiling. Production
sources and fixtures match ecc30f25; outside-body/signature checks pass.

Read-only binding rechecks existing typedpair full proof: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
without lost/gained pairs, canonical arguments unchanged, and all fixed-three
rows recomputed directly with the unchanged matcher. No new native/full-corpus
run claimed for rejected variants. Accepted means remain **98.40% unweighted
/98.41% original-instruction-weighted**; both >=99.0 gates unmet; goal active.
No push, PR or merge.

Evidence: `_work/read_typed_pair_output_identities.json`, nine focused reports
and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: common failure exits on paired-raw source

Two focused Read trials start from c71ec4e1. They repeat shared return -1
labels for final enhanced/child failures or all failures on the newly accepted
paired-array source. Existing explicit/typed cleanup remains before each exit;
no condition, read, cleanup operation or successful return is added. All-label
uses an inner scope so jumps leave declarations rather than skip initializers.
No helper, header, ABI, compilation flag or matcher change. These branch forms
are source hypotheses, not recovered original control flow.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| tail_label | 98.5 / 57.4 | 874 | 100.0 | yes |
| all_label | 98.5 / 57.4 | 874 | 100.0 | yes |

Both complete Core objects equal accepted byte-for-byte; none retained. This
excludes the tested interaction with paired new[]/delete[] without inferring a
compiler ceiling. Sources/fixtures match ecc30f25; outside-body checks pass.
Read-only binding rechecks existing typedpair full evidence: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
without lost/gained pairs, canonical arguments unchanged and directly recomputed
fixed-three rows. No new native/full-corpus run claimed for rejected forms.
Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet;
goal active. No push, PR or merge.

Evidence: `_work/read_typed_pair_common_failure_identities.json`, two focused
reports/source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: success/else cleanup on paired raw arrays

Four focused Read trials start from e07b38ad. They move existing successful
conversion/final deletion inside Read !=0, with the same failure deletion and
return -1 in else, for short positions, int positions, normals and all three.
The original raw allocation and read remain at their source points. Each
success/failure path retains exactly one matching typed array deletion; no
allocation, condition, null guard or cleanup operation added. Read sizes,
conversions, bounds, normal clamps and owner reloads remain unchanged. This is
a branch-form interaction with accepted paired arrays, not recovered C++.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| short_position | 98.5 / 57.4 | 874 | 100.0 | yes |
| integer_position | 98.5 / 57.4 | 874 | 100.0 | yes |
| normal | 98.5 / 57.4 | 874 | 100.0 | yes |
| all_three | 98.5 / 57.4 | 874 | 100.0 | yes |

All four complete Core objects equal accepted byte-for-byte; none retained.
Sources/fixtures match ecc30f25 and outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full proof: 204 objects/202 unaffected
objects, linked 2079/byte 959 exact pair sets equal baseline without lost/gained
pairs, canonical arguments unchanged and fixed-three matcher rows recomputed
directly. No new native/full-corpus run claimed for rejected forms. Accepted
means **98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet; goal active.
No general compiler ceiling, header/ABI, helper, flag, matcher, push, PR or merge
change.

Evidence: `_work/read_typed_pair_cleanup_identities.json`, four focused reports
and snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: normal pointer views on paired-raw source

Twelve focused Read trials start from 8fbd9246. They revisit compressed-normal
input/output pointer views on accepted paired raw arrays: output offset 0/2/3
separately, then input offsets 1/2/3 combined with output offsets 1/2/3.
Offsets are in elements; accesses remain the same X/Y/Z addresses and evaluation
order. Pointer declarations are inside executing triples, at most one-past the
allocation (offset 3 in final triple); zero count forms none. Owner/count field
loads, sqrt/clamps and fallback stores keep their existing source points/order.
No before-array pointer, additional read/call, arithmetic reassociation, header,
ABI, helper, flag or matcher change. These are address-expression hypotheses,
not recovered original local declarations.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| destination_0 | 98.1 / 55.5 | 869 | 100.0 | no |
| destination_2 | 98.4 / 55.9 | 870 | 100.0 | no |
| destination_3 | 98.0 / 56.6 | 864 | 100.0 | no |
| source_1_destination_1 | 98.5 / 56.9 | 874 | 100.0 | no |
| source_1_destination_2 | 98.4 / 55.4 | 870 | 100.0 | no |
| source_1_destination_3 | 98.0 / 54.7 | 865 | 100.0 | no |
| source_2_destination_1 | 98.5 / 57.4 | 874 | 100.0 | yes |
| source_2_destination_2 | 98.4 / 55.9 | 870 | 100.0 | no |
| source_2_destination_3 | 98.0 / 55.0 | 865 | 100.0 | no |
| source_3_destination_1 | 98.5 / 57.4 | 874 | 100.0 | yes |
| source_3_destination_2 | 98.4 / 55.9 | 870 | 100.0 | no |
| source_3_destination_3 | 98.0 / 54.7 | 865 | 100.0 | no |

Input offsets 2/3 with accepted output offset 1 equal the accepted complete
Core object byte-for-byte. Input offset 1/output 1 ties required source but
lowers strict. Every other form regresses source; none retained. Several views
canonicalize to identical objects across input offsets, verified by actual SHA
and byte comparisons rather than inferred from equal scores. Accepted output
middle-component view remains best among these measured combinations; no general
compiler ceiling inferred. Sources/fixtures match ecc30f25; outside-body checks
pass.

Read-only binding rechecks existing typedpair full proof: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
without lost/gained pairs, unchanged canonical arguments, directly recomputed
fixed-three rows. No new native/full-corpus run claimed for rejected forms.
Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet;
goal active. No push, PR or merge.

Evidence: `_work/read_typed_pair_normal_view_identities.json`, twelve focused
reports/source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: normal decoded/length aggregate values

Two focused Read trials start from 9a8caeca. One groups squared length/length
into a local two-float record; the other groups decoded X/Y/Z, squared length
and length into a five-float record. Aggregate clauses are sequenced in the
existing evaluation order; later clauses reference already initialized members.
Every member replaces a used existing value. There is no uninitialized-member
read, value-zero initialization, unused field, explicit padding, constructor,
destructor or added helper. Double input scaling/casts, Y-square plus X-square
plus Z-square, sqrtf, division/clamp/fallback and output order stay unchanged.
These are source-representation hypotheses, not recovered original records.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| length_record | 98.5 / 57.4 | 874 | 100.0 | yes |
| decoded_length_record | 98.5 / 57.4 | 874 | 100.0 | yes |

Both complete Core objects equal accepted byte-for-byte; none retained. Source
and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline without
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, helper,
header/ABI, flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_length_identities.json`, two focused reports
and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: combined signature storage in Create

Six focused Create trials start from 2c97ac88. They represent the same five
seven-byte immutable signatures as rows, rows reversed, columns, columns
reversed, local column records, or one flat row-major array. All 35 bytes,
including plain signature's final zero, are preserved; original comparisons
and five format-bit clear operations remain in their existing order. Reverse
storage maps each comparison to its corresponding actual signature; no bit
association changes. Every table entry/member is used by the seven iterations.
No new read/call, speculative data, unused field, explicit padding, constructor,
helper, header or public ABI change. Combined local const storage is a source
hypothesis, not recovered original table layout.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| rows_original | 95.2 / 51.2 | 268 | 100.0 | no |
| rows_reverse | 95.2 / 51.2 | 268 | 100.0 | no |
| columns_original | 94.8 / 49.8 | 267 | 100.0 | no |
| columns_reverse | 94.0 / 49.7 | 268 | 100.0 | no |
| column_records | 94.8 / 49.8 | 267 | 100.0 | no |
| flat_rows | 95.2 / 51.2 | 268 | 100.0 | no |

All six regress required source and differ from accepted. Row-major/flat objects
equal each other byte-for-byte; column-major/column-record objects also equal.
Reverse-row ties score but differs in bytes; reverse-column differs and further
regresses source. Fewer instructions are not a substitute for requested source
shape. None retained; sources/fixtures match ecc30f25, outside-body/signature
checks pass. No general compiler ceiling inferred.

Read-only binding rechecks existing typedpair full proof: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
without lost/gained pairs, canonical arguments unchanged and directly recomputed
fixed-three rows. No new native/full-corpus run claimed for rejected tables.
Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet;
goal active. No push, PR or merge.

Evidence: `_work/create_signature_table_identities.json`, six focused reports
and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: views of separate signature arrays

Five focused Create trials start from c374c3cc. They preserve the original five
immutable seven-byte arrays and access them through static const pointer table,
local const pointer table, local mutable-pointer table, reverse static table,
or five scalar const-char pointer aliases. Every pointer is used in the existing
comparison; reverse storage preserves signature-to-format-bit mapping. No
signature data, comparison/read order, bit clearing, header guard or file input
changes. No unused element, explicit padding, helper, public ABI, flag or matcher
change. These views are source hypotheses, not recovered original pointer tables.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| static_const_pointers | 98.4 / 60.7 | 289 | 100.0 | yes |
| local_const_pointers | 98.4 / 60.7 | 289 | 100.0 | yes |
| local_mutable_pointers | 98.4 / 60.7 | 289 | 100.0 | yes |
| static_reverse_pointers | 98.4 / 60.7 | 289 | 100.0 | yes |
| local_scalar_pointers | 98.4 / 60.7 | 289 | 100.0 | yes |

All five complete Core objects equal accepted byte-for-byte; none retained.
The measured compiler canonicalizes these views to separate-array output;
no general compiler ceiling inferred. Source/fixtures match ecc30f25 and
outside-body/signature checks pass. Read-only binding rechecks existing typedpair
full proof: 204 objects/202 unaffected objects, actual linked 2079/byte 959 exact
pair sets equal baseline without lost/gained pairs, canonical arguments unchanged,
fixed-three rows directly recomputed. No new native/full-corpus run claimed for
rejected forms. Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0
gates unmet; goal active. No push, PR or merge.

Evidence: `_work/create_signature_pointer_identities.json`, five focused reports
and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: typed bounds center on paired-raw source

Four focused Read trials start from 3fc9433d. They construct the existing
computed center as an aggregate Vector instead of float[3] followed by a
const-Vector cast, with const/nonconst local and normal/nested scope variants.
The same three sums and 0.5f multiplications execute X/Y/Z in order, followed
by the existing Vector assignment to boundsCenterX. Maximum/minimum vectors,
full diagonal and VectorLength remain unchanged. No additional constructor,
destructor, copy assignment, helper, read, zero fill or operation. Header,
public ABI, flags and matcher remain unchanged. Representation/lifetime changes
are source hypotheses, not recovered original declarations.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| vector | 98.5 / 57.4 | 874 | 100.0 | yes |
| vector_scoped | 98.5 / 57.4 | 874 | 100.0 | no |
| const_vector | 98.5 / 57.4 | 874 | 100.0 | yes |
| const_vector_scoped | 98.5 / 57.4 | 874 | 100.0 | no |

Normal-scope const/nonconst objects equal accepted complete Core byte-for-byte.
Nested-scope objects equal each other but differ from accepted, despite identical
scores/counts. No form improves requested source; none retained. No general
compiler ceiling inferred. Production sources/fixtures match ecc30f25 and
outside-body/signature checks pass. Read-only binding rechecks existing typedpair
full proof: 204 objects/202 unaffected objects, actual linked 2079/byte 959 exact
pair sets equal baseline without lost/gained pairs, canonical arguments unchanged,
fixed-three rows directly recomputed. No new native/full-corpus run claimed for
rejected forms. Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0
gates unmet; goal active. No push, PR or merge.

Evidence: `_work/read_typed_pair_center_identities.json`, four focused reports
and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: triangle/final counter widths on paired-raw source

Seven focused Read trials start from 220c2f64. Triangle t is changed to short,
unsigned short or int, separately and with unsigned-short final vertex v;
final v alone is also tested. Triangle bound is derived from unsigned-short
indexCount/3 (maximum 21845), so signed/unsigned-short counters do not overflow:
last executed t is 21844, next value 21845 fails the condition. Final v bound
is unsigned-short-derived vertexCount, maximum 65535; last executed 65534,
next 65535 fails before another increment. Accumulator-zeroing v remains uint.
No index/view address, body evaluation, helper/call, file read, field load,
math/store order or cleanup changes. Counter types are source hypotheses, not
recovered declarations. Header/ABI, flags and matcher remain unchanged.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| triangle_short | 98.0 / 53.7 | 875 | 100.0 | no |
| triangle_short_final_short | 98.0 / 53.7 | 875 | 100.0 | no |
| triangle_unsigned_short | 98.5 / 55.7 | 872 | 100.0 | no |
| triangle_unsigned_short_final_short | 98.5 / 55.7 | 872 | 100.0 | no |
| triangle_int | 98.5 / 57.4 | 874 | 100.0 | yes |
| triangle_int_final_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| final_short | 98.5 / 57.4 | 874 | 100.0 | yes |

Int triangle, its combination and final v alone equal accepted complete Core
byte-for-byte. Narrow final v has no effect within either short triangle pair;
both respective pairs equal each other. Unsigned-short triangle reduces raw
length to 872 but does not improve required source and lowers strict; signed
short regresses source. None retained; no general ceiling inferred. Sources/
fixtures match ecc30f25 and outside-body/signature checks pass.

Read-only binding rechecks existing typedpair full evidence: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
without lost/gained pairs, canonical arguments unchanged, fixed-three rows
directly recomputed. No new native/full-corpus run claimed for rejected forms.
Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet;
goal active. No push, PR or merge.

Evidence: `_work/read_typed_pair_counter_identities.json`, seven focused reports
and snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: mixed signed-char axis dispatch

Six focused Enhanced trials start from 15ef84d4. They retain all unsigned-char
outer axis counters but cast one/two switch expressions to signed char in each
combination. All actual axis values are 0/1/2, representable in both types;
reads, key conversion, channels, position Y negation and insertion order stay
unchanged. Existing all-three cast and mixed int-counter measurements do not
alone prove individual mixed-expression output; these six combinations measure
it directly. No helper, header/ABI, flag, matcher or denominator change. This
is an expression-type hypothesis, not recovered original declaration evidence.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted | Equals prior corresponding int-counter object |
|---|---|---:|---:|---|---|
| signed_axes_0 | 98.2 / 66.8 | 640 | 100.0 | no | yes |
| signed_axes_1 | 98.2 / 66.8 | 640 | 100.0 | no | yes |
| signed_axes_2 | 98.2 / 66.8 | 640 | 100.0 | no | yes |
| signed_axes_0_1 | 98.1 / 66.7 | 639 | 100.0 | no | yes |
| signed_axes_0_2 | 98.1 / 66.7 | 639 | 100.0 | no | yes |
| signed_axes_1_2 | 98.1 / 66.7 | 639 | 100.0 | no | yes |

All six complete Mesh objects differ from accepted and regress source. Each
is byte-identical to its corresponding prior mixed-int-counter object, despite
keeping byte counter declarations here. Thus switch-expression type reproduces
that prior compiler output on these measured combinations. This is actual byte
comparison, not inferred equality from score or a general optimizer ceiling.
None retained. Sources/fixtures match ecc30f25; outside-body checks pass.

Read-only binding rechecks existing typedpair full proof: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
without lost/gained pairs, unchanged canonical arguments and directly recomputed
fixed-three matcher rows. No new native/full-corpus run claimed for rejected
forms. Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates
unmet; goal active. No push, PR or merge.

Evidence: `_work/enh_mixed_dispatch_identities.json`, six focused reports and
source snapshots, corresponding `_work/enhmixedaxis-*` objects, rechecked
`_work/typedpair_validation_audit.json`.


## Continuation: V5 mask preload on byte-axis code

Four focused Enhanced trials start from 2c292de9. Older 0x400 initialization
was tested before byte-axis acceptance. These forms initialize channel to
0x400, leave case 0 intact, retain all other nonzero masks/rotation and assign
zero in default. Further forms add explicit 4/5 zero labels, put 4/5 zero
assignment first, or use unsigned long long channel with explicit zero labels.
Every dispatch path initializes channel; 4/5 still insert keys with mask zero.
All masks fit tested types, so conversion to existing 64-bit flags is unchanged.
Reads, time update, /100 then exact tau multiply then /360, hasAnimation and
insertion order remain. No extra key call, helper, header/ABI, flag or matcher
change. Initialization/label syntax is a source hypothesis, not recovered C++.

| Trial | Enhanced source / strict | Instructions | Calls | Equals accepted | Equals prior explicit-zero object |
|---|---|---:|---:|---|---|
| preload_400_default_zero | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| preload_400_explicit_zero | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| preload_400_zero_first | 98.3 / 67.3 | 639 | 100.0 | no | no |
| preload_400_u64 | 98.3 / 67.8 | 639 | 100.0 | no | yes |

All tie required source and differ from accepted. Default/explicit/u64 objects
are byte-identical to each other and the prior explicit-zero-assignment object;
zero-first differs. Strict-only improvement is not the requested gate; none
retained. No general compiler ceiling inferred. Sources/fixtures match ecc30f25
and outside-body checks pass. Read-only binding rechecks existing typedpair full
proof: 204 objects/202 unaffected objects, actual linked 2079/byte 959 exact pair
sets equal baseline without lost/gained pairs, canonical arguments unchanged,
fixed-three rows directly recomputed. No new native/full-corpus run claimed for
rejected forms. Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0
gates unmet; goal active. No push, PR or merge.

Evidence: `_work/enh_current_preload_identities.json`, four focused reports/
snapshots, prior `_work/enhzerocases-explicit_zero_assignment` object, rechecked
`_work/typedpair_validation_audit.json`.


## Continuation: tangent expression lifetimes after paired raw arrays

Six focused Read trials start from cdb691e6. They repeat the prior tangent
temporary family on the now accepted paired short/int raw allocations, testing
interaction with the changed allocation code. Dot, multiplication, subtraction,
Normalize and Cross remain in their existing mathematical order; tangent stores
remain before Cross. The Vector input variant replaces the existing used
three-float input with a trivial Vector copy. No helper, unused field, padding,
extra effect, fixed register, assembly or compiler-option change is introduced.
These temporary-lifetime forms are hypotheses, not recovered original source.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| inline_dot | 98.5 / 57.4 | 874 | 100.0 | yes |
| inline_product | 97.2 / 58.7 | 868 | 98.2 | no |
| inline_normalize_argument | 98.5 / 57.4 | 874 | 100.0 | no |
| inline_all | 98.5 / 57.4 | 874 | 100.0 | no |
| const_temporaries | 98.5 / 57.4 | 874 | 100.0 | yes |
| vector_tangent_input | 98.4 / 57.7 | 875 | 100.0 | no |

No source-shape gain; none retained. inline_dot and const_temporaries equal the
accepted whole Core object byte-for-byte. The other two tied forms produce
different objects without improving the gate. inline_product reduces static
call-count agreement and source shape; vector_tangent_input also regresses
source shape despite slightly higher strict agreement.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_tangent_lifetime_identities.json`, six focused
reports and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: tangent output views and local records after paired raw arrays

Twenty-one focused Read trials start from 4259dd21 and repeat earlier output
view/local-record families on the accepted paired raw allocation source.
Four output forms reuse a pointer, qualify pointers const, access Vector
members or share the output offset. Nine centered output forms shift tangent,
binormal or both pointers by 1/2/3 floats and compensate every subscript.
Each pointer stays within its allocated array or at one-past on the final
record; every actual store still addresses its original element. Corresponding
field loads remain after Normalize/Cross and tangent stores remain before Cross.

Eight local-record forms group used normal/tangent inputs and/or dot/scaled/
orthogonal/normalized values. Aggregate clauses initialize each referenced
member before use; there are no unused members, value-zero initialization,
extra helpers, padding or constructors. Tangent-first input forms sequence
the independent accumulator reads before the normal copy; no intervening
call/store is introduced. Math, tangent stores and Cross keep their order.
These record/view forms are representation hypotheses, not recovered source.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| output: reuse_pointer | 98.5 / 57.4 | 874 | 100.0 | yes |
| output: const_pointers | 98.5 / 57.4 | 874 | 100.0 | yes |
| output: vector_members | 98.5 / 57.4 | 874 | 100.0 | yes |
| output: shared_offset | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: tangent_1 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: binormal_1 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: both_1 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: tangent_2 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: binormal_2 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: both_2 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: tangent_3 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: binormal_3 | 98.5 / 57.4 | 874 | 100.0 | yes |
| center: both_3 | 98.5 / 57.4 | 874 | 100.0 | yes |
| record: inputs_normal_first | 97.8 / 53.7 | 875 | 98.2 | no |
| record: inputs_tangent_first | 97.4 / 54.1 | 880 | 98.2 | no |
| record: math_vectors | 98.3 / 54.5 | 876 | 100.0 | no |
| record: both_vectors_normal_first | 97.6 / 58.3 | 882 | 98.2 | no |
| record: both_vectors_tangent_first | 98.3 / 56.8 | 877 | 100.0 | no |
| record: math_dot_vectors | 98.4 / 57.5 | 882 | 100.0 | no |
| record: both_dot_vectors_normal_first | 97.0 / 57.6 | 879 | 98.2 | no |
| record: both_dot_vectors_tangent_first | 97.5 / 56.7 | 874 | 98.2 | no |

All thirteen output-view forms equal the complete accepted Core object
byte-for-byte. All eight local-record forms reduce source shape (97.0�98.4
versus 98.5); five also reduce static call-count agreement. None retained.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_tangent_output_identities.json`,
`_work/read_typed_pair_tangent_center_identities.json`,
`_work/read_typed_pair_tangent_local_records_identities.json`, twenty-one
focused reports/source snapshots and rechecked typedpair validation audit.


## Continuation: triangle products, accumulation and position views after paired raw arrays

Fifteen focused Read trials start from 946ab1b8 on accepted paired raw arrays.
Six product forms use compound assignments, named scalar products, Vector
members, a named-products Vector aggregate or a Y/X/Z product record followed
by an X/Y/Z array. All preserve numerator-times-reciprocal operand direction
and Y/X/Z multiplication order, then accumulate vertices i0/i1/i2. No unused
record field, zero fill, padding, helper or additional arithmetic is introduced.

Five accumulation forms express the same three += calls with direct addresses,
typed pointer reuse, separate sequential pointers, three precomputed pointers
or a const input reference. Precomputation uses immutable local indices and
the local accumulator base; call order and operands remain unchanged.
Four position/index views use Vector pointers, a Vector base, Vector references
or signed int copies of ushort indices. Values 0..65535 fit int, and all
index-times-three offsets fit int. Triangle input field loads, formulas and
component order remain unchanged. These are source representation hypotheses,
not recovered original source declarations.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| product: compound_before_stores | 98.5 / 57.4 | 874 | 100.0 | yes |
| product: named_products_before_stores | 98.5 / 57.4 | 874 | 100.0 | yes |
| product: compound_at_stores | 98.5 / 57.4 | 874 | 100.0 | yes |
| product: vector_product_members | 98.5 / 57.4 | 874 | 100.0 | yes |
| product: named_products_vector | 98.5 / 57.5 | 874 | 100.0 | no |
| product: products_yxz_record | 98.5 / 57.5 | 874 | 100.0 | no |
| accum: direct_expressions | 98.5 / 57.4 | 874 | 100.0 | yes |
| accum: vector_pointer_reuse | 98.5 / 57.4 | 874 | 100.0 | yes |
| accum: sequential | 98.5 / 57.4 | 874 | 100.0 | yes |
| accum: before_calls | 98.5 / 57.4 | 874 | 100.0 | yes |
| accum: named_input_reference | 98.5 / 57.4 | 874 | 100.0 | yes |
| view: vector_pointers | 98.5 / 57.4 | 874 | 100.0 | yes |
| view: vector_base | 98.5 / 57.4 | 874 | 100.0 | yes |
| view: vector_references | 98.5 / 57.4 | 874 | 100.0 | yes |
| view: signed_indices | 98.5 / 57.4 | 874 | 100.0 | yes |

Thirteen forms equal the complete accepted Core object byte-for-byte. The two
aggregate product forms equal each other and increase strict agreement by 0.1
without any source-shape increase. None retained: neither requested gate moves.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_products_identities.json`,
`_work/read_typed_pair_accum_pointer_identities.json`,
`_work/read_typed_pair_triangle_views_identities.json`, fifteen focused reports
and source snapshots, rechecked `_work/typedpair_validation_audit.json`.


## Continuation: per-vector time and coordinates records

Fourteen focused Enhanced trials start from 2bdbee77. They group the already
used float key time and three-float coordinate array in a local record, with
time-first or coordinates-first member order, for each of the seven nonempty
subsets of position/scale/rotation vector-key sections. This differs from the
previous separate typed Vector buffers and shared storage across sections.
Each record remains local to its original key iteration. Reads remain separate
4-byte time and 12-byte coordinate reads, with updateTimeBetweenFrames between
them, and insertion follows the second successful read. Every member replaces
an existing used value; no zero fill, uninitialized use, padding, helper,
constructor, destructor, merged read or changed conversion is introduced.
This record layout is a source hypothesis, not recovered original structure.

The private generator initially selected a scalar-key declaration before a
later vector section, causing a compile error. Source restoration ran in
finally. After anchoring each span at its vector declaration, the entire
fourteen-form series was rebuilt and verified. Only that corrected complete
ledger is used below; the failed generation is not a successful trial.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| time_first_0 | 97.2 / 37.2 | 644 | 100.0 | no |
| time_first_1 | 97.9 / 56.9 | 641 | 100.0 | no |
| time_first_2 | 97.9 / 56.9 | 641 | 100.0 | no |
| time_first_0_1 | 97.2 / 44.5 | 644 | 100.0 | no |
| time_first_0_2 | 97.2 / 37.0 | 644 | 100.0 | no |
| time_first_1_2 | 97.9 / 56.9 | 641 | 100.0 | no |
| time_first_0_1_2 | 97.2 / 49.5 | 644 | 100.0 | no |
| vector_first_0 | 97.9 / 57.2 | 641 | 100.0 | no |
| vector_first_1 | 97.9 / 57.2 | 641 | 100.0 | no |
| vector_first_2 | 97.9 / 57.2 | 641 | 100.0 | no |
| vector_first_0_1 | 97.9 / 57.5 | 641 | 100.0 | no |
| vector_first_0_2 | 97.9 / 57.5 | 641 | 100.0 | no |
| vector_first_1_2 | 97.9 / 57.5 | 641 | 100.0 | no |
| vector_first_0_1_2 | 97.8 / 57.8 | 641 | 100.0 | no |

All fourteen forms regress source shape (97.2�97.9 versus 98.3); none retained.
Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/enh_vector_key_record_identities.json`, corrected fourteen
focused reports and source snapshots, rechecked typedpair validation audit.


## Continuation: grouped time guards and current Transform delete expressions

Eighteen focused Enhanced trials start from 4ffb2050. Twelve replace the same
time update with named comparison results, empty false branches, or a single
guarded do/while(false) block. Each form is tested for vector keys, axis keys,
optional+V5 keys and all eight sites. Positive-value comparison remains first;
the global comparison/load remains conditional on positivity. Both strict >
comparisons and the conditional store remain unchanged, including NaN and
signed-zero behavior. No unconditional same-value store or early global load
is introduced. The helper definition outside the body is unchanged.

Six cleanup trials replace explicit Transform destructor/global delete pairs
with delete anim, independently for empty animation, failure and both. Each is
tested with an explicit null guard and delete's own null check. Allocation is
already a matching new Transform expression; Transform's destructor is
nonvirtual and its declaration has no class-specific operator delete. This
rechecks the old combined-delete family with current uchar axis loops and
adds the separate cleanup-site forms. No cleanup calls are merged and no
successful populated animation is deleted. These spellings are hypotheses,
not recovered original C++.

| Group / trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| time: named_comparisons_vectors | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: named_comparisons_axes | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: named_comparisons_optional_v5 | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: named_comparisons_all | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: false_branches_vectors | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: false_branches_axes | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: false_branches_optional_v5 | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: false_branches_all | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: guarded_block_vectors | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: guarded_block_axes | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: guarded_block_optional_v5 | 98.3 / 66.9 | 641 | 100.0 | yes |
| time: guarded_block_all | 98.3 / 66.9 | 641 | 100.0 | yes |
| delete: guarded_empty | 98.3 / 66.9 | 641 | 100.0 | yes |
| delete: guarded_fail | 98.3 / 66.9 | 641 | 100.0 | yes |
| delete: guarded_both | 98.3 / 66.9 | 641 | 100.0 | yes |
| delete: implicit_null_empty | 98.3 / 66.9 | 641 | 100.0 | yes |
| delete: implicit_null_fail | 98.3 / 66.9 | 641 | 100.0 | yes |
| delete: implicit_null_both | 98.3 / 66.9 | 641 | 100.0 | yes |

Every complete Mesh object equals the accepted object byte-for-byte. None
retained; these specific forms do not improve either requested gate.
Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/enh_time_guard_groups_identities.json`,
`_work/enh_current_delete_identities.json`, eighteen focused reports/source
snapshots and rechecked typedpair validation audit.


## Continuation: split signature candidate state

Eight focused Create trials start from e75fd5e2. Read-only inventory diagnostics
of accepted Create show residual load/store and conditional-instruction
differences; they do not prove any particular source declaration. Five trials
separate one signature's candidate state into a bool initialized true, clear it
on each corresponding byte mismatch, and clear its original format bit after
the seven iterations. The other four bits remain in the original format word.
Three trials separate all five candidates using bool, unsigned char or unsigned
int, then construct the original mask from their final states. Each candidate
remains true exactly when all seven corresponding comparisons match, so the
final mask has the same interpretation. All 35 comparisons retain their order,
and no read/update/call is placed between state construction and the fmt guard.

The separate values all replace used candidate bits; no dead value, unused
record field, metric padding, helper, volatile/assembly, fixed register or
unrelated change is introduced. Signature byte tables and all later format
predicates remain unchanged. These forms are source hypotheses, not recovered
original booleans. The original bitwise state remains the accepted source.

| Trial | Create source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| separate_sigMesh | 96.6 / 64.3 | 291 | 100.0 | no |
| separate_sigV2 | 96.6 / 63.9 | 291 | 100.0 | no |
| separate_sigV3 | 96.6 / 63.9 | 291 | 100.0 | no |
| separate_sigV4 | 96.6 / 63.9 | 291 | 100.0 | no |
| separate_sigV5 | 96.6 / 63.9 | 291 | 100.0 | no |
| all_bool | 91.7 / 51.7 | 313 | 100.0 | no |
| all_unsigned_char | 92.4 / 50.6 | 318 | 100.0 | no |
| all_unsigned_int | 92.6 / 52.6 | 317 | 100.0 | no |

All eight forms reduce source shape; none retained. The five single-candidate
forms increase strict agreement but do not move either requested gate.
Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/create_signature_state_split_identities.json`, eight focused
reports/source snapshots, Create inventory diagnostics and rechecked typedpair
validation audit.


## Continuation: bounds records and per-stream extrema locals after paired raw arrays

Twenty focused Read trials start from 2d21aa8c. Eight repeat the earlier
min/max/center record-layout family on materially changed paired typed raw
allocations. Two-member orders are tested through array references and direct
members; four three-member orders include the used center array. Records have
only existing used three-float arrays, with no initialization, added padding,
constructor or unused field. Existing extrema initialization, component
comparisons/stores, center assignment, full max-minus-min diagonal and Length
remain in order. No half-diagonal/radius or input-coordinate change.

Twelve extrema forms name the minimum, name both values sequentially, preload
both or bind component references, independently for short/int position streams
and both together. Older tests covered both streams before the paired raw
allocation gain; this measures current interaction and individual streams.
Min/max arrays are disjoint, so preloading the upper component before the
conditional lower-component store preserves its value. No call intervenes,
and strict comparisons retain their NaN and signed-zero behavior. Every local
is used; no helper or added effect. These source forms are hypotheses, not
recovered original local record layouts or declarations.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| bounds: min_max_refs | 97.9 / 55.3 | 876 | 100.0 | no |
| bounds: max_min_refs | 97.9 / 55.5 | 876 | 100.0 | no |
| bounds: min_max_direct | 97.9 / 55.3 | 876 | 100.0 | no |
| bounds: max_min_direct | 97.9 / 55.5 | 876 | 100.0 | no |
| bounds: min_max_center | 96.6 / 52.8 | 888 | 98.2 | no |
| bounds: center_min_max | 97.6 / 54.5 | 877 | 100.0 | no |
| bounds: max_min_center | 96.6 / 53.3 | 888 | 98.2 | no |
| bounds: max_center_min | 97.8 / 55.0 | 875 | 100.0 | no |
| extrema: minimum_local_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: minimum_local_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: minimum_local_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: sequential_locals_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: sequential_locals_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: sequential_locals_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: both_preloaded_short | 98.5 / 56.9 | 874 | 100.0 | no |
| extrema: both_preloaded_integer | 98.5 / 56.9 | 874 | 100.0 | no |
| extrema: both_preloaded_both | 98.5 / 56.4 | 874 | 100.0 | no |
| extrema: component_references_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: component_references_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| extrema: component_references_both | 98.5 / 57.4 | 874 | 100.0 | yes |

All eight record forms regress source. Nine extrema forms equal the complete
accepted Core object byte-for-byte; the three preloaded forms tie source but
reduce strict agreement. None retained. The reference/direct two-member forms
equal each other for each matching member order, verified by object hashes.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_bounds_records_identities.json`,
`_work/read_typed_pair_extrema_local_identities.json`, twenty focused reports
and source snapshots, rechecked typedpair validation audit.


## Continuation: cyclic position component selection

Twelve focused Read trials start from 9199ae3f. They replace quotient-derived
axis selection with a used cyclic axis local: start at zero, select the same
component for this iteration, then either select 0 after 2 or increment and
reset at 3. Both unsigned int and unsigned char are tested in short positions,
int positions and both streams. By induction axis == i modulo 3 at every loop
body; even the byte increment reaches only 3 before resetting, so it does not
overflow. Zero vertices execute no updates. The original component counter,
input conversion, position store, minimum then maximum comparisons/stores,
allocation/read/failure/cleanup order are preserved. All new local operations
maintain the used component selector; no dead effect or metric padding.

Primary ARM supports the retained quotient/remainder computation, not a cyclic
counter. These trials are compiler/source-shape hypotheses, not recovered
source declarations. Every variant regresses source, so none retained.

| Trial | Read source / strict | Instructions | Call count metric | Static calls |
|---|---|---:|---:|---:|
| unsigned_int_conditional_short | 97.3 / 54.4 | 878 | 100.0 | 61 |
| unsigned_int_conditional_integer | 97.5 / 55.9 | 879 | 98.2 | 62 |
| unsigned_int_conditional_both | 95.8 / 53.3 | 867 | 94.5 | 58 |
| unsigned_int_increment_reset_short | 97.3 / 54.3 | 879 | 100.0 | 61 |
| unsigned_int_increment_reset_integer | 97.4 / 55.9 | 881 | 98.2 | 62 |
| unsigned_int_increment_reset_both | 95.8 / 53.3 | 868 | 94.5 | 58 |
| unsigned_char_conditional_short | 97.3 / 54.4 | 878 | 100.0 | 61 |
| unsigned_char_conditional_integer | 97.5 / 55.9 | 879 | 98.2 | 62 |
| unsigned_char_conditional_both | 95.8 / 53.3 | 867 | 94.5 | 58 |
| unsigned_char_increment_reset_short | 97.5 / 55.0 | 882 | 100.0 | 61 |
| unsigned_char_increment_reset_integer | 97.4 / 49.9 | 879 | 100.0 | 61 |
| unsigned_char_increment_reset_both | 96.2 / 51.7 | 872 | 96.4 | 59 |

Relocation inspection distinguishes call identities from the unchanged
matcher's count component: each changed stream removes one __aeabi_uidiv site.
The five 61-call forms replace that one division site with one additional
_ZdaPv site. Three integer-only forms have 62 calls: one fewer division and
two more delete[] sites. Three both-stream forms have 58 calls: two fewer
division sites and one fewer delete[] site. The remaining byte increment/reset
integer form has 61 calls, and its both-stream form has 59 calls, removing
both divisions without changing delete[] site count. Thus a 100.0 call-count
component here does not mean the same callee inventory. Source inspection
confirms no cleanup operation was added/removed; static-site splitting/merging
is not an executed-cleanup count or runtime equivalence proof.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_position_cyclic_axis_identities.json`,
`_work/read_position_cyclic_axis_call_differences.json`, twelve focused reports
and source snapshots, rechecked typedpair validation audit.


## Continuation: quotient-axis widths and position branches after paired raw arrays

Seventeen focused Read trials start from e4e86ac3. Fifteen retain the original
i - 3 * (i / 3) axis formula and change only its result local type to unsigned
char, unsigned short, int, signed char or short, independently for short/int
position streams and both. Result is always 0..2, representable by every tested
type; division still uses unsigned component i. Component count is at most
3*65535, with no unsigned overflow. No coordinate conversion, stores, extrema
order, dynamic count loads or allocation/cleanup change. Six earlier width
forms were measured before paired raw arrays; this checks current interaction
and adds the other individual-stream/type combinations.

Two branch forms revisit int/float-before-short placement on current paired
raw arrays. One uses the direct inverted flags&4 condition; the other tests
(int)compressedData >=0 with compressedData==flags<<29. The latter is a
two's-complement ARM/UCRT representation hypothesis, not a new portable C++
cast guarantee: shifted bit 2 becomes sign bit. Both select the same data
branches on those targets. The integer and floating predicates, branch-local
reads/allocations/stores/extrema/failure effects remain intact. No unused value,
helper, padding, assembly, fixed register or flag change.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| width: unsigned_char_short | 98.5 / 57.0 | 874 | 100.0 | no |
| width: unsigned_char_integer | 98.5 / 57.1 | 874 | 100.0 | no |
| width: unsigned_char_both | 98.5 / 56.8 | 874 | 100.0 | no |
| width: unsigned_short_short | 98.5 / 57.0 | 874 | 100.0 | no |
| width: unsigned_short_integer | 98.5 / 57.1 | 874 | 100.0 | no |
| width: unsigned_short_both | 98.5 / 56.8 | 874 | 100.0 | no |
| width: int_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: int_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: int_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: signed_char_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: signed_char_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: signed_char_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: short_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: short_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| width: short_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| branch: integer_float_before_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| branch: integer_float_before_short_signed | 97.9 / 57.8 | 873 | 98.2 | no |

Nine signed-type width forms equal the entire accepted Core object. The six
unsigned narrow forms tie source, regress strict, and equal each other for
each matching stream selection (uchar versus ushort). Direct branch inversion
equals accepted; the signed-condition form regresses source/call count despite
slightly higher strict and original-sized instruction count. None retained.
These spellings are source hypotheses, not recovered original declarations.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_axis_width_identities.json`,
`_work/read_typed_pair_position_branch_identities.json`, seventeen focused
reports/source snapshots and rechecked typedpair validation audit.


## Continuation: UV counters and color loop views after paired raw arrays

Fifteen focused Read trials start from ad149b3f. Nine color forms repeat earlier
normalcenter-era loop views on current paired raw allocation source: signed
index, dynamic byte offset, input pointer advancement, postincrement store,
value then increment, guarded do and input/output/both one-past views. Dynamic
slot vertex-count condition remains fresh each iteration, and captured m's
colors field remains freshly loaded for each output component. Bounds are at
most 4*65535 components, fitting int; one-past pointers are legal and each
actual access compensates to its original element. Division by 255 remains
float division; no multiplication/precision change or captured output pointer.

Six UV forms test signed component indices and ushort vertex counters,
independently in compressed, floating and both loops. Signed index retains
the captured vcount<<1 bound, stride 2 and component indices. Vertex form has
0..vcount-1 vertices and a used uint component offset 2*vertex. At maximum count
65535, last vertex 65534 advances to 65535 and exits without ushort overflow;
zero count does no work. Compressed double scaling/flip, floating float flip,
input/output access timing, count capture and shader checks remain unchanged.
These are source-representation hypotheses, not recovered original syntax.
No helper, dead effect, padding, new initialization or unrelated change.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| color: signed_index | 98.5 / 57.4 | 874 | 100.0 | yes |
| color: dynamic_byte_offset | 98.5 / 57.5 | 874 | 100.0 | no |
| color: input_pointer | 98.5 / 57.4 | 874 | 100.0 | yes |
| color: postincrement_store | 98.5 / 57.4 | 874 | 100.0 | yes |
| color: value_then_increment | 98.5 / 57.4 | 874 | 100.0 | yes |
| color: guarded_do | 98.5 / 56.9 | 875 | 100.0 | no |
| color: output_end | 98.5 / 57.4 | 874 | 100.0 | yes |
| color: input_end | 98.5 / 57.4 | 874 | 100.0 | yes |
| color: both_end | 98.5 / 57.4 | 874 | 100.0 | yes |
| UV: signed_component_compressed | 98.5 / 57.4 | 874 | 100.0 | yes |
| UV: signed_component_floating | 98.5 / 57.4 | 874 | 100.0 | yes |
| UV: signed_component_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| UV: ushort_vertex_compressed | 98.2 / 58.0 | 874 | 100.0 | no |
| UV: ushort_vertex_floating | 98.2 / 57.8 | 873 | 100.0 | no |
| UV: ushort_vertex_both | 97.9 / 57.5 | 871 | 100.0 | no |

Seven color views and all three signed UV indices equal accepted whole Core
byte-for-byte. Color offset/guarded-do forms tie source without improving either
gate; three ushort UV forms regress source despite slightly higher strict.
None retained. Source and fixtures match ecc30f25; outside-body/signature checks
pass. Read-only binding rechecks existing typedpair full evidence: 204 objects/
202 unaffected objects, actual linked 2079/byte 959 exact pair sets equal
baseline with no lost/gained pairs, unchanged canonical arguments and fixed-
three rows directly recomputed with the unchanged matcher. No new native/full-
corpus run claimed for rejected forms. Accepted means **98.40% unweighted /
98.41% weighted**; both >=99.0 gates unmet; goal active. No general compiler
ceiling, header/ABI, flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_color_loop_identities.json`,
`_work/read_typed_pair_uv_counter_identities.json`, fifteen focused reports/
source snapshots and rechecked typedpair validation audit.


## Continuation: per-stream extrema comparison and update order after paired raw arrays

Nine focused Read trials start from ebc9778c. Three forms reverse the comparison
operands, update maximum before minimum, or combine these changes, independently
for short positions, int positions and both. The earlier family tested only
both streams before accepted paired raw arrays; this tests their interaction
and separate streams on current source. Strict comparisons remain strict,
including NaN/zero behavior; min/max arrays are disjoint local objects and the
two updates have no intervening call or effect on the input value/axis. Thus
maximum-first changes independent local update ordering without combining
conditions (both updates still execute when both predicates hold, as required
for the initial inverted extrema sentinels). Input conversion/position stores
remain before the extrema updates; center and full-diagonal calculations stay
after all positions. These are hypotheses, not recovered original update order.
No extra arithmetic, unconditional self-store, helper, dead local or padding.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| reverse_operands_short | 98.5 / 57.4 | 874 | 100.0 | yes |
| reverse_operands_integer | 98.5 / 57.4 | 874 | 100.0 | yes |
| reverse_operands_both | 98.5 / 57.4 | 874 | 100.0 | yes |
| maximum_first_short | 98.5 / 56.9 | 874 | 100.0 | no |
| maximum_first_integer | 98.5 / 56.9 | 874 | 100.0 | no |
| maximum_first_both | 98.5 / 56.4 | 874 | 100.0 | no |
| reverse_maximum_first_short | 98.5 / 56.9 | 874 | 100.0 | no |
| reverse_maximum_first_integer | 98.5 / 56.9 | 874 | 100.0 | no |
| reverse_maximum_first_both | 98.5 / 56.4 | 874 | 100.0 | no |

All three operand-only forms equal the complete accepted Core object byte-for-
byte. Maximum-first forms tie source and reduce strict; adding operand reversal
produces the same corresponding objects. None retained; neither gate improves.
Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_extrema_order_identities.json`, nine focused
reports/source snapshots and rechecked typedpair validation audit.


## Continuation: indexed owner and typed allocation interaction after paired raw arrays

Sixteen focused Read trials start from 13c61435. They repeat eight earlier
indexed owner/scope forms on current paired raw arrays, each with original
byte allocation and primitive new unsigned short[count]. Typed arrays have no
initializer/zero fill; native and ARM ushort are two bytes, and counts are
ushort, so sizes are unchanged. These index buffers escape into the existing
Mesh ownership path; no new local deletion or failure cleanup is introduced.

Primary ARM reloads *slot after indexCount read at 0x6c664, reads indexCount
at 0x6c668, allocates at 0x6c66c and stores indices through that owner at
0x6c670. Current accepted source captures m before the count read. Seven forms
use a fresh owner there, through reload/local scope/direct assignment, optionally
also reloading the buffer-read or vertex-read owner. One changes just the
allocation count expression to fresh *slot and keeps the captured store owner.
Original observations prove memory-load timing, not exact C++ spelling, a
runtime bug or mutation of *slot by the existing file boundary. Trials with
fresh owners should not be claimed universally equivalent for a hypothetical
slot-mutating file callback; current native fixtures do not model that callback.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted |
|---|---|---:|---:|---|
| owner_reload_before_allocation_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_reload_before_allocation_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_separate_allocation_owner_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_separate_allocation_owner_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_reload_read_owner_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_reload_read_owner_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_direct_count_allocation_byte_allocation | 98.5 / 57.2 | 874 | 100.0 | no |
| owner_direct_count_allocation_typed_allocation | 98.5 / 57.2 | 874 | 100.0 | no |
| owner_reload_direct_assignment_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| owner_reload_direct_assignment_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| scope_fresh_scoped_owner_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| scope_fresh_scoped_owner_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| scope_vertex_owner_after_read_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| scope_vertex_owner_after_read_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| scope_fresh_scoped_direct_assignment_byte_allocation | 98.5 / 57.8 | 874 | 100.0 | no |
| scope_fresh_scoped_direct_assignment_typed_allocation | 98.5 / 57.8 | 874 | 100.0 | no |

All byte/typed pairs produce identical complete objects. Fourteen fresh-owner
forms equal one another, tie source at 98.5 and improve strict 57.4 ->57.8.
The two count-only forms equal one another, tie source and regress strict to
57.2. This changes the old exclusion: fresh-owner forms formerly regressed
source on axisbyte-era code but no longer do so after paired raw arrays.
They are useful candidates for subsequent interaction trials, though neither
requested gate improves by this change alone. None retained in production.

Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline with no
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_typed_pair_index_owner_identities.json`, sixteen focused
reports/source snapshots, primary original_mesh.asm and rechecked typedpair
validation audit.


## Continuation: fresh indexed owner interactions with normals and tangent counters

Twenty-one focused Read measurements start from f10e76ca: nineteen interaction
forms and two repeated reload-only controls. Each adds m=*slot after successful
indexCount read, immediately before index allocation, as supported by primary
ARM 0x6c664. This candidate now ties source and improves strict, unlike the old
axisbyte-era exclusion, so its interactions are measured separately.

Twelve normal-view forms change destination center to 0/2/3 or combine input
center 1/2/3 with destination center 1/2/3, compensating every subscript. Each
pointer is within its own three-component record or one-past on the final
record; actual accesses remain original elements. Double decoding, normalized
float quotients, lower/upper clamp semantics, fallback outputs, dynamic slot
vertex-count condition and output-field loads stay intact.

Seven counter forms use short/ushort/int triangle counters, optionally ushort
final tangent counters, plus final ushort alone. Triangle count indexCount/3
is <=21845, fitting signed short. Final vertices are <=65535: final ushort
increment reaches 65535 then exits, without overflow. Clear loop remains uint;
Dot/multiply/subtract/Normalize/stores/Cross order and count captures remain.
No helper, dead effect, padding, assembly, fixed register or metric change.
View/counter spellings are hypotheses. Fresh-owner timing has original ARM
support, but existing native fixtures do not prove equivalent behavior for
a hypothetical AEFile callback mutating *slot; no such broad proof is claimed.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| normal: index_reload_only | 98.5 / 57.8 | 874 | 100.0 | yes |
| normal: destination_0 | 98.1 / 56.3 | 869 | 100.0 | no |
| normal: destination_2 | 98.4 / 56.7 | 870 | 100.0 | no |
| normal: destination_3 | 98.0 / 57.5 | 864 | 100.0 | no |
| normal: source_1_destination_1 | 98.5 / 57.4 | 874 | 100.0 | no |
| normal: source_1_destination_2 | 98.4 / 56.2 | 870 | 100.0 | no |
| normal: source_1_destination_3 | 98.0 / 55.1 | 865 | 100.0 | no |
| normal: source_2_destination_1 | 98.5 / 57.8 | 874 | 100.0 | yes |
| normal: source_2_destination_2 | 98.4 / 56.7 | 870 | 100.0 | no |
| normal: source_2_destination_3 | 98.0 / 55.5 | 865 | 100.0 | no |
| normal: source_3_destination_1 | 98.5 / 57.8 | 874 | 100.0 | yes |
| normal: source_3_destination_2 | 98.4 / 56.7 | 870 | 100.0 | no |
| normal: source_3_destination_3 | 98.0 / 55.1 | 865 | 100.0 | no |
| counter: index_reload_only | 98.5 / 57.8 | 874 | 100.0 | yes |
| counter: triangle_short | 98.0 / 54.2 | 877 | 100.0 | no |
| counter: triangle_short_final_short | 98.0 / 54.2 | 877 | 100.0 | no |
| counter: triangle_unsigned_short | 98.5 / 56.2 | 872 | 100.0 | no |
| counter: triangle_unsigned_short_final_short | 98.5 / 56.2 | 872 | 100.0 | no |
| counter: triangle_int | 98.5 / 57.8 | 874 | 100.0 | yes |
| counter: triangle_int_final_short | 98.5 / 57.8 | 874 | 100.0 | yes |
| counter: final_short | 98.5 / 57.8 | 874 | 100.0 | yes |

No interaction improves source beyond 98.5. Five interaction forms equal the
entire reload-only object; two controls independently reproduce that object.
The other forms tie or regress source, without a better gate result. None
retained. Reload-only remains a candidate for other interactions, not a retained
gain. Source and fixtures match ecc30f25; outside-body/signature checks pass.
Read-only binding rechecks existing typedpair full evidence: 204 objects/202
unaffected objects, actual linked 2079/byte 959 exact pair sets equal baseline
with no lost/gained pairs, unchanged canonical arguments and fixed-three rows
directly recomputed with the unchanged matcher. No new native/full-corpus run
claimed for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_index_reload_normal_views_identities.json`,
`_work/read_index_reload_counter_identities.json`, twenty-one focused reports/
source snapshots and rechecked typedpair validation audit.


## Continuation: indexed owner interaction with triangle UV and coordinate scheduling

Seventeen focused Read measurements start from b5c5881f: eight source forms
with accepted owner, the same eight with fresh indexed allocation owner, and
one repeated fresh-owner control. Fresh m=*slot follows the successful
indexCount read as supported by primary ARM 0x6c664. It remains an experimental
candidate with strict 57.8 and tied source 98.5, not a retained source gain.

Five forms delay origin U until after V differences, name the two U differences,
combine those forms, delay reciprocal until after the three numerators or name
the two denominator products. Formulas, floating widths, grouping and operand
direction remain unchanged. Delayed reciprocal changes scheduling of independent
local arithmetic, not reassociation: each numerator is still multiplied by
1/denom. Three other forms preload origin position components Z/Y/X, X/Y/Z or
Y/X/Z after p1 address computation and substitute the used values in their
existing numerator expressions. No intervening mutation/call changes those
coordinates. Numerators/products remain Y/X/Z, then original i0/i1/i2 += calls;
final Dot/multiply/subtract/Normalize/stores/Cross sequence is untouched.
No helper, unused local, padding, extra arithmetic, volatile, assembly, fixed
register or metric change. Source spellings/scheduling are hypotheses, not
recovered original declarations. Fresh owner timing has ARM support, but
native fixtures do not establish equivalence for hypothetical slot-mutating
file callbacks, and no such broader claim is made.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted | Equals reload-only |
|---|---|---:|---:|---|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| late_origin_u_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| late_origin_u_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| explicit_u_differences_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| explicit_u_differences_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| late_u_and_differences_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| late_u_and_differences_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| late_reciprocal_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| late_reciprocal_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| separate_denom_products_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| separate_denom_products_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| origin_preload_zyx_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| origin_preload_zyx_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| origin_preload_xyz_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| origin_preload_xyz_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| origin_preload_yxz_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| origin_preload_yxz_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |

All eight accepted-owner forms equal the complete accepted Core object byte-
for-byte. All eight fresh-owner forms and the control equal the complete
reload-only object. This proves these particular scheduling forms have no
object-level interaction gain; equal scores alone are not used as identity
proof. None retained; code restored. Source and fixtures match ecc30f25;
outside-body/signature checks pass. Read-only binding rechecks existing
typedpair full evidence: 204 objects/202 unaffected objects, actual linked
2079/byte 959 exact pair sets equal baseline without lost/gained pairs,
unchanged canonical arguments and fixed-three rows directly recomputed with
the unchanged matcher. No new native/full-corpus run claimed for rejected
forms. Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates
unmet; goal active. No general compiler ceiling, header/ABI, flag/matcher,
push, PR or merge change.

Evidence: `_work/read_index_reload_triangle_schedule_identities.json`, seventeen
focused reports/source snapshots and rechecked typedpair validation audit.


## Continuation: unsigned raw allocation pairs with both indexed owner forms

Fourteen focused Read measurements start from 9e63aa84. All seven nonempty
subsets of short positions/int positions/compressed normals replace the paired
primitive allocation/deletion element type with corresponding unsigned short/
unsigned int, on accepted indexed owner and fresh owner after indexCount read.
Matching delete[] casts change together with each new[] expression. Raw short/
int views used by conversion remain signed, so negative input samples retain
their existing interpretation; unsigned allocation is not an unsigned decode.
Corresponding signed/unsigned integer access is permitted aliasing, and ARM/
UCRT element sizes remain 2/4 bytes. Counts are unchanged vertexCount*3 and fit
the bounded allocation arithmetic. No value initializer, zero fill, constructor,
element destructor, extra cleanup, unused field or helper is added. These are
allocation representation hypotheses, not recovered original element types.

Fresh indexed owner keeps the previously inspected ARM load timing, but is
experimental: native fixtures do not establish equivalence for a hypothetical
slot-mutating file callback. No such broader runtime proof is claimed.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted | Equals reload-only |
|---|---|---:|---:|---|---|
| short_positions_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| short_positions_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| integer_positions_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| integer_positions_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| normals_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| normals_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| short_positions_integer_positions_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| short_positions_integer_positions_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| short_positions_normals_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| short_positions_normals_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| integer_positions_normals_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| integer_positions_normals_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| short_positions_integer_positions_normals_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| short_positions_integer_positions_normals_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |

Every complete object equals its corresponding signed-allocation reference
byte-for-byte: seven accepted-owner forms equal accepted Core and seven fresh-
owner forms equal reload-only Core. No source/object interaction gain; none
retained. Code restored. Source and fixtures match ecc30f25; outside-body/
signature checks pass. Read-only binding rechecks existing typedpair full
evidence: 204 objects/202 unaffected objects, actual linked 2079/byte 959 exact
pair sets equal baseline without lost/gained pairs, unchanged canonical
arguments and fixed-three rows directly recomputed with the unchanged matcher.
No new native/full-corpus run claimed for rejected forms. Accepted means
**98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet; goal active.
No general compiler ceiling, header/ABI, flag/matcher, push, PR or merge change.

Evidence: `_work/read_unsigned_raw_pairs_identities.json`, fourteen focused
reports/source snapshots and rechecked typedpair validation audit.


## Continuation: fresh indexed owner with tangent temporary and product forms

Thirteen focused Read measurements start from 9c7c0cd0: twelve interactions
and one reload-only control. Six final-loop forms inline Dot/product/Normalize
argument/all expressions, qualify temporaries const or use a trivial Vector
tangent-input copy. Six triangle-product forms use compound/named products,
Vector members, a named-products Vector or a Y/X/Z product record. These
families were previously measured on accepted paired arrays without the fresh
indexed owner. Every form now adds m=*slot after successful indexCount read,
before allocation, matching the inspected original load timing at 0x6c664.

Dot/multiply/subtract/Normalize remain ordered, tangent stores precede Cross,
and output field loads remain after their corresponding math calls. Triangle
numerators/products retain Y/X/Z and numerator-times-reciprocal operands;
accumulation remains i0/i1/i2. Used temporaries replace used values, with no
extra operations, zero fill, dead member, padding, helper or flags change.
These source representations are hypotheses. Fresh indexed load timing has
ARM support, but fixtures do not establish equivalence for hypothetical slot-
mutating file callbacks; no broader native equivalence claim is made.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | yes |
| final_inline_dot | 98.5 / 57.8 | 874 | 100.0 | yes |
| final_inline_product | 97.3 / 59.1 | 870 | 98.2 | no |
| final_inline_normalize_argument | 98.5 / 57.8 | 874 | 100.0 | no |
| final_inline_all | 98.5 / 57.8 | 874 | 100.0 | no |
| final_const_temporaries | 98.5 / 57.8 | 874 | 100.0 | yes |
| final_vector_tangent_input | 98.4 / 58.1 | 875 | 100.0 | no |
| product_compound_before_stores | 98.5 / 57.8 | 874 | 100.0 | yes |
| product_named_products_before_stores | 98.5 / 57.8 | 874 | 100.0 | yes |
| product_compound_at_stores | 98.5 / 57.8 | 874 | 100.0 | yes |
| product_vector_product_members | 98.5 / 57.8 | 874 | 100.0 | yes |
| product_named_products_vector | 98.5 / 57.9 | 874 | 100.0 | no |
| product_products_yxz_record | 98.5 / 57.9 | 874 | 100.0 | no |

No interaction improves source above 98.5. Six interactions and the control
equal the complete reload-only object byte-for-byte. The two aggregate product
forms equal each other and improve strict alone to 57.9. Other forms tie or
regress source; none retained. Production source restored. Source and fixtures
match ecc30f25; outside-body/signature checks pass. Read-only binding rechecks
existing typedpair full evidence: 204 objects/202 unaffected objects, actual
linked 2079/byte 959 exact pair sets equal baseline without lost/gained pairs,
unchanged canonical arguments and fixed-three rows directly recomputed with
the unchanged matcher. No new native/full-corpus run claimed for rejected forms.
Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet;
goal active. No general compiler ceiling, header/ABI, flag/matcher, push, PR
or merge change.

Evidence: `_work/read_index_reload_tangent_lifetime_identities.json`, thirteen
focused reports/source snapshots and rechecked typedpair validation audit.


## Continuation: unsigned section type with both if-dispatch orders

Fifteen focused Enhanced measurements start from 1585f816: shared unsigned-short
section type control and fourteen combinations of if-dispatch. Both type==1
before type==0 and type==0 before type==1 are tested for all seven nonempty
subsets of position/scale/rotation sections. This combines previously separate
signedness and dispatch families and adds mixed-section/order combinations.
Every type read remains two bytes at its original point. Bit patterns other
than 0/1 skip the same sections, while optional type 2 remains recognized.
Sign reinterpretation does not make any other bit pattern equal 0/1/2.
V5 short count stays signed, as required for negative-count skipping. All
branch-local reads, updates, inserts, axis masks, position Y negation and
failure paths remain in their sections. No new operations, helper, table,
dead local, padding, compiler option or ABI change. These forms are hypotheses,
not recovered original type declarations or if-dispatch syntax.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals accepted |
|---|---|---:|---:|---|
| unsigned_type_control | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_0 | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_1 | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_0_1 | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_0_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_1_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| one_first_0_1_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_0 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_1 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_0_1 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_0_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_1_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_0_1_2 | 98.1 / 66.4 | 641 | 100.0 | no |

All fifteen complete Mesh objects are byte-identical to one another and to
the earlier unsigned-shared-type object. Thus the new if-dispatch combinations
do not change that object's source regression, regardless of section/order.
None retained; code restored. Source and fixtures match ecc30f25; outside-body/
signature checks pass. Read-only binding rechecks existing typedpair full
evidence: 204 objects/202 unaffected objects, actual linked 2079/byte 959 exact
pair sets equal baseline without lost/gained pairs, unchanged canonical
arguments and fixed-three rows directly recomputed with the unchanged matcher.
No new native/full-corpus run claimed for rejected forms. Accepted means
**98.40% unweighted /98.41% weighted**; both >=99.0 gates unmet; goal active.
No general compiler ceiling, header/ABI, flag/matcher, push, PR or merge change.

Evidence: `_work/enh_unsigned_section_dispatch_identities.json`, fifteen focused
reports/source snapshots, prior unsigned-shared-type object comparison and
rechecked typedpair validation audit.


## Continuation: normal length/section guards with indexed owner interaction

Nineteen focused Read measurements start from 8a97f67e: nine forms with accepted
indexed owner, the same nine with a fresh owner after indexCount read and one
reload-only control. This tests earlier normalcenter-era length/section families
on current paired raw arrays, including their fresh-owner interactions.

Five length forms place fallback first, continue after zero/nonzero branches,
name the nonzero predicate, or load the normals output field before the length
branch. They retain len==0 versus len!=0 semantics, including NaN taking the
nonzero path, all divisions/clamp/fallback values and output component order.
The early field-load form moves only a used output address across local
arithmetic/comparisons, not across a file/allocation call. It is an experimental
load-scheduling hypothesis, not a recovered original load site.

Four section forms split shader handling from stream handling, flatten or
separate compressed/plain stream guards, or use a common section-exit label.
The captured normalEnabled value replaces the original outer vertex-format
test and is used by every corresponding stream/shader guard. Compressed and
plain selections remain mutually exclusive, and enableShader is checked after
the stream. Existing read/allocation/failure/cleanup effects and math order
remain in their original execution paths. No helper, unused field, padding,
assembly, fixed register, flag or matcher change. Fresh indexed-owner timing
has original ARM support but no native proof for hypothetical slot-mutating
file callbacks; such universal equivalence is not claimed.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted | Equals reload-only |
|---|---|---:|---:|---|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| length_fallback_first_accepted_owner | 98.3 / 59.6 | 874 | 100.0 | no | no |
| length_fallback_first_fresh_owner | 98.3 / 60.1 | 874 | 100.0 | no | no |
| length_zero_continue_accepted_owner | 98.3 / 59.6 | 874 | 100.0 | no | no |
| length_zero_continue_fresh_owner | 98.3 / 60.1 | 874 | 100.0 | no | no |
| length_nonzero_continue_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| length_nonzero_continue_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| length_named_nonzero_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| length_named_nonzero_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| length_owner_before_branch_accepted_owner | 98.2 / 58.6 | 871 | 100.0 | no | no |
| length_owner_before_branch_fresh_owner | 98.1 / 59.4 | 871 | 100.0 | no | no |
| section_split_shader_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| section_split_shader_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| section_flat_shader_accepted_owner | 97.5 / 55.0 | 891 | 100.0 | no | no |
| section_flat_shader_fresh_owner | 97.5 / 55.4 | 892 | 100.0 | no | no |
| section_independent_shader_accepted_owner | 97.3 / 56.1 | 884 | 98.2 | no | no |
| section_independent_shader_fresh_owner | 97.3 / 56.6 | 884 | 98.2 | no | no |
| section_common_section_exit_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| section_common_section_exit_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |

No form improves source beyond 98.5. Four forms per owner produce their entire
reference object byte-for-byte; reload control reproduces reload-only. Zero-
continue equals fallback-first for each owner. Other forms regress source;
strict-only gains up to 60.1 do not move either requested gate. None retained;
code restored. Source and fixtures match ecc30f25; outside-body/signature checks
pass. Read-only binding rechecks existing typedpair full evidence: 204 objects/
202 unaffected objects, actual linked 2079/byte 959 exact pair sets equal
baseline without lost/gained pairs, unchanged canonical arguments and fixed-
three rows directly recomputed with the unchanged matcher. No new native/full-
corpus run claimed for rejected forms. Accepted means **98.40% unweighted /
98.41% weighted**; both >=99.0 gates unmet; goal active. No general compiler
ceiling, header/ABI, flag/matcher, push, PR or merge change.

Evidence: `_work/read_index_reload_normal_guards_identities.json`, nineteen
focused reports/source snapshots and rechecked typedpair validation audit.


## Continuation: position predicates/output storage with fresh indexed owner

Nineteen focused Read measurements start from fe468346: nine source forms with
accepted indexed owner, the same nine with fresh owner after indexCount read,
and one reload control. Three short-position predicates test signed direct
flags<<29, signed compressedData or a signed compressedData local. Shift occurs
on unsigned flags before conversion. On the two's-complement ARM/UCRT targets,
bit 2 becomes sign bit and tests the same short-position condition; this is
not a new portable C++ conversion guarantee. Other compressedData nonzero tests
preserve the shifted value's zero/nonzero status. Predicate spellings are
hypotheses, not recovered exact source.

Four forms eliminate the intermediate position allocation pointer independently
for short/int/floating streams and all streams. Two share a void raw pointer
only across mutually exclusive short/int branches, alone or together with
direct position allocation stores. Every typed raw allocation keeps its matching
typed delete[] at the same success/failure locations. There is no double cleanup,
new zero initialization or read of raw before its branch allocation. Captured
owner stores, fresh field/count loads, conversion/extrema calculations and
file-read/cleanup order stay unchanged. No helper, dead field, padding, assembly,
fixed register or matcher/flag change. Fresh indexed-owner timing has ARM
support, but fixtures do not prove equivalence for hypothetical slot-mutating
file callbacks; no broader native runtime proof is claimed.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals accepted | Equals reload-only |
|---|---|---:|---:|---|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| predicate_direct_shift_negative_accepted_owner | 97.9 / 57.9 | 872 | 98.2 | no | no |
| predicate_direct_shift_negative_fresh_owner | 97.6 / 57.8 | 878 | 98.2 | no | no |
| predicate_shared_shift_negative_accepted_owner | 97.9 / 57.9 | 872 | 98.2 | no | no |
| predicate_shared_shift_negative_fresh_owner | 97.6 / 57.8 | 878 | 98.2 | no | no |
| predicate_signed_compressed_local_accepted_owner | 97.9 / 57.9 | 872 | 98.2 | no | no |
| predicate_signed_compressed_local_fresh_owner | 97.6 / 57.8 | 878 | 98.2 | no | no |
| direct_output_short_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| direct_output_short_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| direct_output_integer_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| direct_output_integer_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| direct_output_floating_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| direct_output_floating_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| direct_output_all_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| direct_output_all_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| shared_raw_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| shared_raw_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| shared_raw_direct_outputs_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| shared_raw_direct_outputs_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |

Six storage forms per owner equal their entire reference Core object byte-for-
byte; control reproduces reload-only. All three signed-predicate forms equal
each other for each owner but regress source to 97.9/97.6 and call-count
agreement. No form improves source beyond 98.5; none retained. Code restored.
Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline without
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_index_reload_position_forms_identities.json`, nineteen
focused reports/source snapshots and rechecked typedpair validation audit.


## Continuation: fresh indexed owner with bounds copies and lifetimes

Eleven focused Read measurements start from aa0a96a6: ten bounds copy/lifetime
interactions and one reload-only control. Each reloads m=*slot after successful
indexCount read, before allocation, matching original memory timing at 0x6c664.
Five forms copy max/min/both through Vector views, subtract direct views or
pass the temporary full diagonal directly to Length. Five scope center as a
float array or Vector, initialize minimum before maximum, initialize extrema
vectors before center assignment or before center computation. Local values
remain independent of the Mesh bounds assignment; no intervening file/allocation
call changes extrema. Trivial copies introduce no additional math call.

Every form retains max-minus-min full diagonal, center max-plus-min times 0.5,
the bounds assignment/subtraction/Length call order, and extrema initialization/
input conversion semantics. No half-diagonal, reassociation, zero fill, dead
field, added helper, padding, assembly, fixed register or flag/matcher change.
These local representation/lifetime choices are hypotheses, not recovered
original syntax. Fresh indexed load has original ARM support but existing
fixtures do not establish equivalence for hypothetical slot-mutating file
callbacks; no broader native runtime proof is claimed.

| Trial | Read source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | yes |
| copy_maximum_copy | 98.3 / 57.3 | 876 | 100.0 | no |
| copy_minimum_copy | 98.4 / 57.0 | 875 | 100.0 | no |
| copy_both_copies | 98.2 / 57.6 | 877 | 100.0 | no |
| copy_direct_views | 98.2 / 58.3 | 870 | 100.0 | no |
| copy_temporary_diagonal | 98.5 / 57.8 | 874 | 100.0 | yes |
| lifetime_scoped_float_center | 98.5 / 57.8 | 874 | 100.0 | no |
| lifetime_scoped_vector_center | 98.5 / 57.8 | 874 | 100.0 | no |
| lifetime_minimum_first | 98.5 / 57.8 | 874 | 100.0 | no |
| lifetime_extents_before_assignment | 98.5 / 57.0 | 874 | 100.0 | no |
| lifetime_extents_before_center | 97.1 / 40.8 | 885 | 98.2 | no |

No form improves source beyond 98.5. Control and temporary diagonal equal the
entire reload-only Core object byte-for-byte. Scoped float/Vector center forms
equal one another but differ from reload-only, without score improvement.
Other forms tie or regress source; none retained. Production code restored.
Source and fixtures match ecc30f25; outside-body/signature checks pass. Read-only
binding rechecks existing typedpair full evidence: 204 objects/202 unaffected
objects, actual linked 2079/byte 959 exact pair sets equal baseline without
lost/gained pairs, unchanged canonical arguments and fixed-three rows directly
recomputed with the unchanged matcher. No new native/full-corpus run claimed
for rejected forms. Accepted means **98.40% unweighted /98.41% weighted**;
both >=99.0 gates unmet; goal active. No general compiler ceiling, header/ABI,
flag/matcher, push, PR or merge change.

Evidence: `_work/read_index_reload_bounds_values_identities.json`, eleven focused
reports/source snapshots and rechecked typedpair validation audit.


## Continuation: fresh indexed owner with UV flip and pair views

Fourteen focused Read measurements start from 0d6e33ac: five flip-storage forms,
two outer flip-dispatch forms, six pair views and a reload-only control. Each
adds a fresh m=*slot after successful indexCount read, before allocation.
Flip storage uses raw uchar/uint/int/ushort enableShader or implicit bool;
Engine.h declares enableShader unsigned char, so each integer type preserves
its full value/nonzero interpretation. Snapshot timing remains unchanged.
Outer-dispatch forms branch on the existing immutable flip snapshot, with
flipped or plain loop first. Only the selected loop executes, and compressed
double u/v scaling and conditional double 1-v subtraction retain their precision.

Pair views shift source/destination/both pointers by 1 or 2 and compensate
every subscript. The final shift by 2 is one-past the corresponding input/output
array; every access still addresses its original element. Fresh m->texCoords
field loads stay at their original position inside each compressed iteration.
No type is changed in decode, no float subtraction substitutes for double,
and no allocation/read/count/cleanup change. Source forms are hypotheses, not
recovered original declarations. No helper, dead effect, padding, assembly,
fixed register or flag/matcher change. Fresh indexed-owner memory timing has
ARM support but no fixture proof for hypothetical slot-mutating file callbacks.

| Group / trial | Read source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| flip: reload_control | 98.5 / 57.8 | 874 | 100.0 | yes |
| flip: storage_raw_unsigned_char | 98.5 / 57.8 | 874 | 100.0 | yes |
| flip: storage_raw_unsigned_int | 98.5 / 57.8 | 874 | 100.0 | yes |
| flip: storage_raw_int | 98.5 / 57.8 | 874 | 100.0 | yes |
| flip: storage_raw_unsigned_short | 98.5 / 57.8 | 874 | 100.0 | yes |
| flip: storage_implicit_bool | 98.5 / 57.8 | 874 | 100.0 | yes |
| flip: outer_flipped_first | 96.3 / 56.4 | 914 | 100.0 | no |
| flip: outer_plain_first | 96.3 / 56.4 | 914 | 100.0 | no |
| pair: destination_1 | 98.5 / 57.8 | 874 | 100.0 | yes |
| pair: source_1 | 98.5 / 57.8 | 874 | 100.0 | yes |
| pair: both_1 | 98.5 / 57.8 | 874 | 100.0 | yes |
| pair: destination_2 | 98.5 / 57.8 | 874 | 100.0 | yes |
| pair: source_2 | 98.5 / 57.8 | 874 | 100.0 | yes |
| pair: both_2 | 98.5 / 57.8 | 874 | 100.0 | yes |

Five storage forms, six pair views and control equal the entire reload-only
Core object byte-for-byte. Both outer-dispatch forms equal one another and
regress source to 96.3. No gain; none retained. Code restored. Source and
fixtures match ecc30f25; outside-body/signature checks pass. Read-only binding
rechecks existing typedpair full evidence: 204 objects/202 unaffected objects,
actual linked 2079/byte 959 exact pair sets equal baseline without lost/gained
pairs, unchanged canonical arguments and fixed-three rows directly recomputed
with the unchanged matcher. No new native/full-corpus run claimed for rejected
forms. Accepted means **98.40% unweighted /98.41% weighted**; both >=99.0 gates
unmet; goal active. No general compiler ceiling, header/ABI, flag/matcher,
push, PR or merge change.

Evidence: `_work/read_index_reload_uv_flip_identities.json`,
`_work/read_index_reload_uv_pair_identities.json`, fourteen focused reports/
source snapshots and rechecked typedpair validation audit.


## Main working-checkout intake: loader bodies and compiled dependencies

User requested loading changes from the main working branch to test whether
these improve the fixed-three score. Inspected main checkout
`A:/Galaxy on Fire new version/gof2hd-decomp-master`, branch main at
e473d5454d89404051c9731ac93ad52897699020, against own branch dd925f81.
Main is an ancestor of the owned branch; there are no new committed loader
changes. Both incoming loader source files have no uncommitted diff from main
HEAD. The main working checkout is dirty in many unrelated files; it was
read only and remains untouched. Snapshots include the dirty working files,
not just committed main content.

Dependency files from the accepted ARM build identify every project header
compiled into Core and Mesh. Exactly two differ in the main working checkout:
Array.h (ArrayRemove snapshots size) and AEString.h (ARM layout static asserts).
Mesh/Engine/Vector layouts and other actual compiled dependencies match.
Main math/Transform implementation changes reside in separate translation
units; no LTO is enabled and they cannot change the caller instruction shape.
They were inspected as runtime changes, not imported into this three-body task.
The main checkout also changes verifier files: no verifier, matching flags,
compiler, denominator, helper body or ABI/header update was retained.

Copied only the three incoming body spans into own local trial sources, keeping
all surrounding code. Tested six combinations, including independent body
replacement and current bodies with both changed dependency headers. These
are exact incoming source trials, not newly recovered declarations.

| Trial | Create source | Read source | Enhanced source | Mean | Original-instruction weighted mean |
|---|---:|---:|---:|---:|---:|
| current_bodies_main_headers | 98.4 | 98.5 | 98.3 | 98.400000 | 98.413167 |
| main_bodies_current_headers | 95.8 | 91.8 | 94.5 | 94.033333 | 93.400667 |
| main_bodies_main_headers | 95.8 | 91.8 | 94.5 | 94.033333 | 93.400667 |
| main_only_MeshReadData | 98.4 | 91.8 | 98.3 | 96.166667 | 95.163667 |
| main_only_MeshCreateFromFile | 95.8 | 98.5 | 98.3 | 97.533333 | 97.992833 |
| main_only_ReadEnhancedDataFromFile | 98.4 | 98.5 | 94.5 | 97.133333 | 97.070500 |

Main bodies regress individually and together. Main bodies with current versus
main headers produce identical corresponding full objects. Current bodies
with main headers produce exact accepted Core and Mesh objects:
Core ec6b0774edb1cfec44ec92f6d54e0b8148c0003f829b4fc1a7c9a2c1a82d39c3;
Mesh 71f33c2bf51d3f356a2ad3d5df4450736badfe2da89301d93c1c121432736a73.
Thus header intake cannot improve this score. Old main bodies also restore
the previously rejected half-diagonal radius and inexact 6.2832f rotation
constant, so retaining them would discard validated behavior fixes.

All six trials compile and focused ARM verification exits zero. Each fixed-three
row was recomputed directly from its disassembly with unchanged source metrics;
original counts remain 291/873/636, total 1800. Source metrics average the
matcher-produced per-body values. No improvement; no incoming implementation
change retained. Restored all four local source/header texts to dd925f81 and
verified the main snapshots did not change during the experiment. Source
signatures and code outside the three bodies remain baseline-identical.

Read-only binding audit of existing accepted full/native evidence passes:
204 translation units, 4619 comparisons, zero skipped, 202 unaffected whole
objects unchanged, exact linked-pair set 2079 and byte-pair set 959 with no
loss or gain. Existing full report SHA256
69141f6eaafd32d61ce63ae1257ac889bc0cfcc7765c514a630391e0df481af8.
No fresh full-corpus or native-runtime run is claimed for these rejected forms.
Accepted score remains **98.40% /98.41%**; both >=99.0 gates unmet, goal active.
No push/PR/merge; main checkout untouched.

Private evidence: `_work/main_intake_snapshot.json`,
`_work/main_intake_results.json`, incoming source/header snapshots,
body diffs, six Core/Mesh focused report pairs and compiled objects.


## Continuation: indexed owner with color/UV inputs and signed animation dispatch

Thirty-one focused measurements start from fd39d9dc. Sixteen Read forms add
fresh m=*slot after successful indexCount read, before index allocation:
reload-only control, nine color loop/store forms and six compressed UV input
copies. Color forms preserve dynamic vertexCount and fresh m->colors access,
byte-to-float conversion and division by 255. Signed index is bounded by the
ushort vertex count (maximum 262140 components); byte offsets are bounded by
1048560. Guarded do retains the zero-count guard. Shifted source/destination
views advance by one then address [-1], including safe one-past final views.
No persistent output pointer replaces the existing per-component field load.
UV copies use short/int scalars, two-element arrays or u/v records before
double scaling and the existing double flip subtraction. Only independent
input-load scheduling changes, with no precision or sign change. These source
forms are hypotheses, not recovered original declarations. The fresh index
owner has primary ARM support but no fixture proof for slot-mutating callbacks.

| Read trial | Source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_signed_index | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_dynamic_byte_offset | 98.5 / 57.9 | 874 | 100.0 | no |
| color_input_pointer | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_postincrement_store | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_value_then_increment | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_guarded_do | 98.5 / 57.3 | 875 | 100.0 | no |
| color_output_end | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_input_end | 98.5 / 57.8 | 874 | 100.0 | yes |
| color_both_end | 98.5 / 57.8 | 874 | 100.0 | yes |
| uv_short_scalars | 98.4 / 57.9 | 876 | 100.0 | no |
| uv_short_array | 98.4 / 57.9 | 876 | 100.0 | no |
| uv_short_record | 98.4 / 57.9 | 876 | 100.0 | no |
| uv_int_scalars | 98.4 / 57.9 | 876 | 100.0 | no |
| uv_int_array | 98.4 / 57.9 | 876 | 100.0 | no |
| uv_int_record | 98.4 / 57.9 | 876 | 100.0 | no |

Seven color variants plus control equal the entire reload-only Core object.
Byte-offset and guarded-do forms tie source 98.5 with different objects.
All six UV copy forms equal each other and regress to 98.4. None retained.

Fifteen Enhanced measurements keep the original signed short type and replace
selected switch(type) sections with equivalent if/else type==0/type==1 tests.
All seven nonempty subsets of position/scale/rotation sections are tested with
one-first and zero-first order, plus unchanged control. Invalid types skip the
same section, per-axis switches remain unchanged, count widths/key math/read
order and V5 signed counts/zero-channel behavior remain unchanged. No speculative
mask table or helper is introduced. Source order is a hypothesis.

| Enhanced trial | Source / strict | Instructions | Calls | Accepted whole Mesh |
|---|---|---:|---:|---|
| signed_type_control | 98.3 / 66.9 | 641 | 100.0 | yes |
| one_first_0 | 98.2 / 52.8 | 641 | 100.0 | no |
| one_first_1 | 98.2 / 66.7 | 641 | 100.0 | no |
| one_first_2 | 98.2 / 66.7 | 641 | 100.0 | no |
| one_first_0_1 | 98.1 / 38.8 | 641 | 100.0 | no |
| one_first_0_2 | 98.1 / 52.6 | 641 | 100.0 | no |
| one_first_1_2 | 98.1 / 66.6 | 641 | 100.0 | no |
| one_first_0_1_2 | 98.1 / 66.4 | 641 | 100.0 | no |
| zero_first_0 | 98.2 / 52.8 | 641 | 100.0 | no |
| zero_first_1 | 98.2 / 66.7 | 641 | 100.0 | no |
| zero_first_2 | 98.2 / 66.7 | 641 | 100.0 | no |
| zero_first_0_1 | 98.1 / 38.8 | 641 | 100.0 | no |
| zero_first_0_2 | 98.1 / 52.6 | 641 | 100.0 | no |
| zero_first_1_2 | 98.1 / 66.6 | 641 | 100.0 | no |
| zero_first_0_1_2 | 98.1 / 66.4 | 641 | 100.0 | no |

Corresponding zero-first/one-first forms are byte-identical full Mesh objects.
Each single section regresses to 98.2, pairs and all-three to 98.1. Control
matches accepted Mesh. No gain; none retained. All thirty-one compile/focused
verify calls exit zero; source/call metrics independently recomputed directly
from target/base instructions with the unchanged matcher. No new native/full
run is claimed for rejected forms.

Code and fixtures restored to ecc30f25. Read-only binding audit of accepted
full/native evidence passes: 204 TUs, 4619 comparisons, zero skipped, 202
unaffected objects, actual linked 2079/byte 959 exact-pair sets unchanged with
no lost/gained pairs; canonical flags and outside-body/signature boundaries
unchanged. Accepted means **98.40% /98.41%**, both >=99.0 gates unmet, goal active.
No compiler ceiling established; no header/helper/ABI, flag/matcher, push,
PR or merge change. Main checkout remains untouched.

Evidence: `_work/read_index_reload_color_uv_input_identities.json`,
`_work/enh_signed_section_dispatch_identities.json`, thirty-one focused
reports/source snapshots/objects and rechecked typedpair validation audit.


## Continuation: fresh indexed owner with extrema and child-state lifetimes

Forty successful focused measurements start from d2683890. Every form adds
fresh m=*slot after successful indexCount read, before index allocation.
Reviewed the unchanged matcher's instruction-inventory differences to choose
loads, local lifetimes and comparisons as the next hypotheses. Inventory gaps
alone do not prove the original declarations or establish a compiler ceiling.

Twenty-two extrema forms combine reload-only control with four local/reference
forms and three comparison-order forms, each in short-position, integer-position
or both decoders. Minimum/sequential locals and component references preserve
accesses; both-preloaded form loads independent min/max components before the
first update. Minimum and maximum arrays are distinct and indices stay 0..2.
Comparison reversal preserves NaN behavior; swapping independent min/max
updates preserves assignments. No new arithmetic, allocation or stream read.

| Extrema form | Source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_minimum_local_short | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_minimum_local_integer | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_minimum_local_both | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_sequential_locals_short | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_sequential_locals_integer | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_sequential_locals_both | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_both_preloaded_short | 98.5 / 57.4 | 874 | 100.0 | no |
| locals_both_preloaded_integer | 98.5 / 57.4 | 874 | 100.0 | no |
| locals_both_preloaded_both | 98.5 / 56.9 | 874 | 100.0 | no |
| locals_component_references_short | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_component_references_integer | 98.5 / 57.8 | 874 | 100.0 | yes |
| locals_component_references_both | 98.5 / 57.8 | 874 | 100.0 | yes |
| order_reverse_operands_short | 98.5 / 57.8 | 874 | 100.0 | yes |
| order_reverse_operands_integer | 98.5 / 57.8 | 874 | 100.0 | yes |
| order_reverse_operands_both | 98.5 / 57.8 | 874 | 100.0 | yes |
| order_maximum_first_short | 98.5 / 57.4 | 874 | 100.0 | no |
| order_maximum_first_integer | 98.5 / 57.4 | 874 | 100.0 | no |
| order_maximum_first_both | 98.5 / 56.9 | 874 | 100.0 | no |
| order_reverse_maximum_first_short | 98.5 / 57.4 | 874 | 100.0 | no |
| order_reverse_maximum_first_integer | 98.5 / 57.4 | 874 | 100.0 | no |
| order_reverse_maximum_first_both | 98.5 / 56.9 | 874 | 100.0 | no |

All extrema forms tie source 98.5; thirteen equal the entire reload-only Core
object. Nine preloaded/maximum-first forms change code without improvement.
Corresponding reversed and unreversed maximum-first objects are identical.

Eighteen child forms combine control with five lifetime/counter variants and
twelve records: six ordered pairs and six permutations of childCount/c/childPtr.
Each record contains only the live original locals, not unused padding fields.
Signed/ushort child counters are bounded by ushort childCount; the last legal
iteration advances to count and exits without ushort overflow. The guarded-do
zero-count behavior, failed-unattached-child lifetime, recursive read predicate,
sphere merge and fresh parent load before ArrayAdd stay unchanged. Moving the
child pointer declaration changes stack lifetime, not allocation timing.

| Child form | Source / strict | Instructions | Calls | Whole Core equals reload-only |
|---|---|---:|---:|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | yes |
| lifetime_signed_child_counter | 98.5 / 57.8 | 874 | 100.0 | yes |
| lifetime_short_child_counter | 98.5 / 57.8 | 875 | 100.0 | no |
| lifetime_child_before_count | 98.4 / 57.8 | 874 | 100.0 | no |
| lifetime_child_inside_nonempty_guard | 98.5 / 57.5 | 873 | 100.0 | no |
| lifetime_child_function_scope | 98.4 / 57.8 | 874 | 100.0 | no |
| state_pair_childCount_c | 97.4 / 55.4 | 874 | 98.2 | no |
| state_pair_c_childCount | 97.3 / 55.4 | 875 | 98.2 | no |
| state_pair_childCount_childPtr | 98.1 / 56.8 | 880 | 100.0 | no |
| state_pair_childPtr_childCount | 98.4 / 57.8 | 875 | 100.0 | no |
| state_pair_c_childPtr | 97.3 / 55.3 | 876 | 98.2 | no |
| state_pair_childPtr_c | 97.3 / 55.4 | 875 | 98.2 | no |
| state_all_childCount_c_childPtr | 97.3 / 55.3 | 876 | 98.2 | no |
| state_all_childCount_childPtr_c | 97.3 / 55.3 | 876 | 98.2 | no |
| state_all_c_childCount_childPtr | 97.3 / 55.3 | 876 | 98.2 | no |
| state_all_c_childPtr_childCount | 97.3 / 55.3 | 876 | 98.2 | no |
| state_all_childPtr_childCount_c | 97.3 / 55.3 | 876 | 98.2 | no |
| state_all_childPtr_c_childCount | 97.3 / 55.3 | 876 | 98.2 | no |

Control and signed child counter equal reload-only Core. Other lifetime forms
tie or regress. All record forms regress; ten have call-count match 98.2.
No source-level extra call or artificial effect was inserted. None retained.
Fresh indexed-owner timing has ARM support but lacks fixture proof for a
hypothetical slot-mutating callback. Source forms remain hypotheses.

First child sweep terminated during verification of
state_all_childCount_c_childPtr with "No comparable functions found" when A:
had only 69632 bytes free. Its finally block restored the source. That failed
measurement was not accepted. Freed space by deleting only 408 untracked target
objects under owned _work/normalcenter-full/target and normalinput-full/target,
after checking each SHA256 against its retained canonical typedpair-full/target
counterpart. No source, report, base object, main checkout file or baseline/
accepted validation evidence was removed. Manifest records each path, checksum,
size and retained replacement. Then reran the entire eighteen-form sweep:
all compile/focused verify calls exited zero, complete ledger written. Current
forty rows independently recomputed from target/base instructions with the
unchanged matcher. No new native/full-corpus run claimed for rejected forms.

Code/fixtures restored to ecc30f25. Read-only accepted full/native binding audit
passes: 204 TUs/4619 comparisons/zero skipped, 202 unaffected objects, actual
linked 2079/byte 959 exact-pair sets equal baseline with no lost/gained pairs,
canonical flags and outside-body/signature boundaries unchanged. Accepted
means **98.40% /98.41%**; both >=99.0 gates unmet, goal active. No header/helper/
ABI, compiler/flag/matcher, push, PR or merge change. Main checkout untouched.

Evidence: `_work/read_index_reload_extrema_identities.json`,
`_work/read_index_reload_child_forms_identities.json`, forty focused reports/
source snapshots/objects, `_work/duplicate_target_cleanup_20261006.json` and
rechecked typedpair validation audit. Older duplicate target paths in the
cleanup manifest now resolve through their explicitly recorded retained copies.


## Continuation: accumulator initialization, indexed owner and tangent counters

Thirty-seven focused Read measurements start from 71f29f4a. Sixteen test five
accumulator count/zero forms with both accepted and fresh indexed owner, five
accumulation-pointer forms with fresh indexed owner, and reload-only control.
Fresh owner m=*slot is loaded after successful indexCount read, before index
allocation. Its timing has primary ARM support but no fixture proof for a
hypothetical slot-mutating callback; this limitation still applies.

Count scheduling swaps independent triCount and vcount loads. Zero forms use
chained assignments, a local entry pointer, per-vertex memset of exactly three
floats, or a flat component loop. Float positive zero is all-zero bits on the
fixed ARM/native targets. All zero forms visit exactly 3*vcount components,
including zero components when vcount=0. The original accumulator allocation,
triangle updates and matching cleanup remain. Pointer forms directly name
original Vector accumulation targets or the input reference without changing
the original sequential calls or triangle arithmetic.

The per-vertex memset form ties source 98.5 while reducing Read from 874 to
873 instructions. Thirteen subsequent forms pair that zero form with fresh
indexed owner and six final-loop temporary forms/six triangle product forms.
Only source temporary lifetimes/views change: dot-product, scaling, subtraction,
normalization and cross-product order/operand direction stay unchanged. All
triangle components still multiply numerator by reciprocal; no precision
change or arithmetic reassociation is introduced. Then eight forms pair the
same zero/reload form with triangle-counter widths and optional final ushort
counter, including control. Triangle count <=65535/3 bounds signed short to
21845; final ushort reaches vertexCount <=65535 and exits without overflow.
These declarations are hypotheses, not recovered original source.

Accumulator zero / pointer:

| Form | Source / strict | Instructions | Calls | Accepted Core | Reload-only Core |
|---|---|---:|---:|---|---|
| reload_control | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| zero_count_after_triangles_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| zero_count_after_triangles_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| zero_chained_zero_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| zero_chained_zero_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| zero_pointer_zero_accepted_owner | 98.5 / 57.4 | 874 | 100.0 | yes | no |
| zero_pointer_zero_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| zero_vertex_memset_accepted_owner | 98.5 / 57.4 | 873 | 100.0 | no | no |
| zero_vertex_memset_fresh_owner | 98.5 / 57.8 | 873 | 100.0 | no | no |
| zero_component_loop_accepted_owner | 97.5 / 57.8 | 866 | 98.2 | no | no |
| zero_component_loop_fresh_owner | 97.6 / 58.2 | 868 | 98.2 | no | no |
| pointer_direct_expressions_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| pointer_vector_pointer_reuse_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| pointer_sequential_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| pointer_before_calls_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |
| pointer_named_input_reference_fresh_owner | 98.5 / 57.8 | 874 | 100.0 | no | yes |

Vertex memset / tangent lifetimes:

| Form | Source / strict | Instructions | Calls | Accepted Core | Reload-only Core |
|---|---|---:|---:|---|---|
| reload_control | 98.5 / 57.8 | 873 | 100.0 | no | no |
| final_inline_dot | 98.5 / 57.8 | 873 | 100.0 | no | no |
| final_inline_product | 97.3 / 59.1 | 870 | 98.2 | no | no |
| final_inline_normalize_argument | 98.5 / 57.8 | 873 | 100.0 | no | no |
| final_inline_all | 98.5 / 57.8 | 873 | 100.0 | no | no |
| final_const_temporaries | 98.5 / 57.8 | 873 | 100.0 | no | no |
| final_vector_tangent_input | 98.5 / 58.1 | 875 | 100.0 | no | no |
| product_compound_before_stores | 98.5 / 57.8 | 873 | 100.0 | no | no |
| product_named_products_before_stores | 98.5 / 57.8 | 873 | 100.0 | no | no |
| product_compound_at_stores | 98.5 / 57.8 | 873 | 100.0 | no | no |
| product_vector_product_members | 98.5 / 57.8 | 873 | 100.0 | no | no |
| product_named_products_vector | 98.5 / 58.0 | 873 | 100.0 | no | no |
| product_products_yxz_record | 98.5 / 58.0 | 873 | 100.0 | no | no |

Vertex memset / counters:

| Form | Source / strict | Instructions | Calls | Accepted Core | Reload-only Core |
|---|---|---:|---:|---|---|
| index_reload_only | 98.5 / 57.8 | 873 | 100.0 | no | no |
| triangle_short | 98.1 / 53.9 | 875 | 100.0 | no | no |
| triangle_short_final_short | 98.1 / 53.9 | 875 | 100.0 | no | no |
| triangle_unsigned_short | 98.4 / 56.2 | 871 | 100.0 | no | no |
| triangle_unsigned_short_final_short | 98.4 / 56.2 | 871 | 100.0 | no | no |
| triangle_int | 98.5 / 57.8 | 873 | 100.0 | no | no |
| triangle_int_final_short | 98.5 / 57.8 | 873 | 100.0 | no | no |
| final_short | 98.5 / 57.8 | 873 | 100.0 | no | no |

The first series has three exact accepted-Core forms and nine exact reload-only
forms; per-vertex memset ties source, while component-loop zeroing regresses to
97.5/97.6 and call-count match 98.2. No artificial call/store was added to the
source. In the second series control, inline-dot, const temporaries and four
product forms equal the entire memset/reload control Core object. Inline
product regresses to 97.3/calls98.2. Vector tangent input improves its earlier
reload-only trial from 98.4 to 98.5, but only ties accepted source and is not
retained. Two named product-record forms are byte-identical. In the third
series signed short triangle counters regress to 98.1, ushort to 98.4; signed
int/final ushort/control tie 98.5 and equal memset/reload control. No gate gain.

All thirty-seven compile/focused verify calls completed successfully. Source
and call metrics recomputed directly from original/base instruction arrays,
object hashes checked. The sixteen/thirteen ledgers and source restoration
were revalidated after interruption before the eight-counter series. To avoid
new full-source snapshot floods, thirteen/eight series retain their deterministic
source generators, object hashes and reports; source snapshots are saved only
for a gate candidate, and there was none. Their generators reconstruct each
form from the accepted ecc30f25 source, not from later mutated input.

Before starting the sixteen-form series, removed 408 untracked duplicate target
objects under owned angle-full/target and trinumer-full/target only after each
matched SHA256 of retained typedpair-full/target replacement. No source, base
object, report, baseline/current accepted evidence or main checkout file was
removed. The manifest records all deleted paths and retained copies, with
replacement hashes rechecked. No disk failure in these thirty-seven trials.

All forms rejected/restored. Code and fixtures remain ecc30f25-equivalent;
outside-body/signature boundaries and canonical arguments unchanged. Read-only
binding audit of existing accepted full/native evidence passes: 204 TUs, 4619
comparisons, zero skipped, 202 unaffected objects, actual linked 2079/byte 959
exact-pair sets equal baseline with no lost/gained pairs. No fresh native/full
run claimed for rejected forms. Accepted means **98.40% /98.41%**; both >=99.0
gates unmet, goal active. No header/helper/ABI, matcher/compiler/flag, push,
PR or merge change. No general compiler ceiling established.

Evidence: `_work/read_index_reload_accum_forms_identities.json`,
`_work/read_index_reload_memzero_tangent_lifetime_identities.json`,
`_work/read_index_reload_memzero_counter_identities.json`, focused reports/
objects/source generators and `_work/duplicate_target_cleanup_accum_20261006.json`.


## Continuation: preloaded V5 channel with count, frame and parser state

Nineteen focused Enhanced measurements (including two identical controls)
start from 87b826c2. Control initializes V5 channel to 0x400, leaves case 0
unchanged, keeps all other nonzero masks and exact rotation math, and assigns
zero in default; 4/5 still insert keys with mask zero. This is the previously
tied 98.3/67.8/639 form, not a retained implementation or recovered declaration.
All dispatch paths initialize channel. Signed V5 counts and /100 then exact tau
multiply then /360 stay unchanged. No invented lookup table or effect is added.

Twelve measurements combine this preload with three shared-prefix-count forms,
six frame-time captures, V5 count moved outside its channel loop, and shared
prefix/V5 outer count combination. Vector/axis/all mandatory counts reuse the
existing optionalCount storage only across nonoverlapping stream sections.
Every count is read before use; sizes, signedness, bounds, failure routes and
read order remain. Frame conversion is captured only after its last successful
read, before the existing dispatch, with the same key/value expression and
integer conversion. V5 channel count remains signed short outside the loop;
its declaration changes lifetime, not read timing or zero initialization.

Seven measurements pair preload with six ParseState forms plus control. Local
records hold self/anim, file/format, file/anim, self/file or all four live locals
in original/file-first member order. No unused field, fabricated padding or
new initialization is introduced. Construction and stream calls stay in their
original order. This local grouping tests register/stack effects, not an ABI
change. The control and forms are source hypotheses, not recovered C++.

Count/frame lifetime:

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals preload-only |
|---|---|---:|---:|---|
| preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| count_vector_counts | 97.4 / 63.4 | 639 | 100.0 | no |
| count_axis_counts | 97.7 / 50.9 | 644 | 100.0 | no |
| count_all_prefix_counts | 97.8 / 66.5 | 633 | 100.0 | no |
| frame_vectors | 98.3 / 67.8 | 639 | 100.0 | yes |
| frame_axes | 97.5 / 64.7 | 641 | 100.0 | no |
| frame_optional | 98.3 / 67.8 | 639 | 100.0 | yes |
| frame_v5 | 98.1 / 66.3 | 646 | 100.0 | no |
| frame_optional_v5 | 98.1 / 66.3 | 646 | 100.0 | no |
| frame_all | 97.3 / 63.4 | 648 | 100.0 | no |
| v5_count_outer | 98.0 / 67.6 | 639 | 100.0 | no |
| count_all_and_v5_outer | 97.5 / 66.4 | 633 | 100.0 | no |

Parser local records:

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals preload-only |
|---|---|---:|---:|---|
| preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| parser_self_anim | 98.3 / 67.8 | 639 | 100.0 | yes |
| parser_file_format | 98.3 / 67.8 | 639 | 100.0 | yes |
| parser_file_anim | 98.3 / 67.8 | 639 | 100.0 | yes |
| parser_self_file | 98.3 / 67.8 | 639 | 100.0 | yes |
| parser_all_source_order | 98.3 / 67.8 | 639 | 100.0 | yes |
| parser_all_file_first | 98.3 / 67.8 | 639 | 100.0 | yes |

Three count/frame forms equal preload-only Mesh: control, vector frame capture
and optional frame capture. Count forms regress to 97.4/97.7/97.8; V5 count
outer scope gives 98.0, all shared counts plus V5 scope 97.5. Remaining frame
forms regress to 97.3-98.1. All seven parser-state/control whole Mesh objects
are byte-identical to preload-only:
0bca14f7cadf2178ab305d3fcc9a0f21d0241a68e43f35527193829f0c6a7a14.
All call-count matches stay 100.0. No gain; no form retained.

Nineteen compile/focused verify calls exit zero; per-body source/call metrics
independently recomputed directly from target/base instructions and full object
hashes checked. Each source snapshot retained. Source/fixtures restored to
accepted ecc30f25. Read-only binding recheck of existing accepted full/native
evidence passes: 204 TUs/4619 comparisons/zero skipped, 202 unaffected objects,
actual linked 2079/byte 959 pair sets equal baseline with no lost/gained pairs,
canonical flags and outside-body/signature boundaries unchanged. No new native/
full-corpus run claimed for rejected forms. Accepted means **98.40% /98.41%**,
both >=99.0 gates unmet; goal active. No helper/header/ABI, matcher/compiler/flag,
push, PR or merge change. No general compiler ceiling established.

Evidence: `_work/enh_preload_count_frame_identities.json`,
`_work/enh_preload_parser_state_identities.json`, nineteen focused reports/
source snapshots/objects and rechecked typedpair validation audit.


## Continuation: V5 preload with axis count records and key counters

Twenty-two focused Enhanced measurements start from 3e8aafc6. All combine the
previously tied V5 channel preload (0x400, case 0 break, default mask zero)
with local count/axis changes, including control. The original masks, signed
V5 count and exact /100 then tau multiply then /360 remain. Cases 4/5 still
insert with zero mask. This initializer is a source hypothesis, not recovered
C++; strict-only improvement was not accepted as the requested source gate.

Eight forms group byte axis and ushort count into a two-member local AxisState,
with axis/count or count/axis member order, in one of three mandatory sections
or all three. The record contains only live original locals, no unused fields
or manufactured padding. Axis remains 0..2; count is still read before each
axis loop. Four forms share one ushort count between mutually exclusive
vector/axis modes within a scoped mandatory section, individually or all three.
The shared local is uninitialized until its original read; no read/count/failure
routing changes. These lifetime/layout alternatives are source hypotheses.

Nine forms change one mandatory key-loop counter: vector counter int or ushort,
axis key counter int, for position/scale/rotation independently. Ushort counter
is bounded by ushort count <=65535; it reaches count then exits without
wraparound. Signed int fits all counts. V5 signed loops are unaffected. All
stream calls, key conversion, timer updates, math and insertion order remain.
No helper, header, ABI, flag or verifier change is introduced.

| Trial | Enhanced source / strict | Instructions | Calls | Whole Mesh equals preload-only |
|---|---|---:|---:|---|
| preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| axis_record_axis_count_0 | 97.2 / 52.8 | 645 | 100.0 | no |
| axis_record_axis_count_1 | 97.2 / 41.4 | 645 | 100.0 | no |
| axis_record_axis_count_2 | 97.3 / 22.3 | 645 | 100.0 | no |
| axis_record_axis_count_all | 94.6 / 44.6 | 656 | 100.0 | no |
| axis_record_count_axis_0 | 97.3 / 56.7 | 642 | 100.0 | no |
| axis_record_count_axis_1 | 97.3 / 44.3 | 642 | 100.0 | no |
| axis_record_count_axis_2 | 97.5 / 23.9 | 643 | 100.0 | no |
| axis_record_count_axis_all | 95.0 / 40.3 | 648 | 100.0 | no |
| group_count_position_group | 98.3 / 54.0 | 638 | 100.0 | no |
| group_count_scale_group | 97.8 / 52.2 | 640 | 100.0 | no |
| group_count_rotation_group | 98.0 / 67.2 | 647 | 100.0 | no |
| group_count_all_scoped_groups | 98.0 / 52.4 | 635 | 100.0 | no |
| counter_vector_position_int | 98.3 / 67.8 | 639 | 100.0 | yes |
| counter_vector_scale_int | 98.3 / 67.8 | 639 | 100.0 | yes |
| counter_vector_rotation_int | 98.3 / 67.8 | 639 | 100.0 | yes |
| counter_vector_position_unsigned_short | 98.2 / 67.6 | 640 | 100.0 | no |
| counter_vector_scale_unsigned_short | 98.2 / 54.5 | 640 | 100.0 | no |
| counter_vector_rotation_unsigned_short | 98.2 / 67.6 | 640 | 100.0 | no |
| counter_axis_position_int | 98.3 / 67.8 | 639 | 100.0 | yes |
| counter_axis_scale_int | 98.3 / 67.8 | 639 | 100.0 | yes |
| counter_axis_rotation_int | 98.3 / 67.8 | 639 | 100.0 | yes |

All eight record forms regress (94.6-97.5). Shared position count ties 98.3
with a different 638-instruction object; scale/rotation/all regress. Six signed
int key-loop forms and control equal the entire preload-only Mesh object:
0bca14f7cadf2178ab305d3fcc9a0f21d0241a68e43f35527193829f0c6a7a14.
Three ushort vector counters regress to 98.2. Calls stay 100.0 for all forms.
No gain; none retained. No general compiler ceiling established.

All twenty-two compile/focused verify calls exit zero. Source/call metrics
independently recomputed directly from target/base instructions, whole object
hashes checked, source snapshots retained. Code/fixtures restored to ecc30f25.
Read-only binding recheck of existing accepted full/native evidence passes:
204 TUs/4619 comparisons/zero skipped, 202 unaffected whole objects, actual
linked 2079/byte 959 exact-pair sets equal baseline with no lost/gained pairs,
canonical arguments and outside-body/signature boundaries unchanged. No fresh
native/full-corpus run claimed for rejected forms. Accepted means
**98.40% /98.41%**; both >=99.0 gates unmet, goal active. No push, PR or merge.

Evidence: `_work/enh_preload_axis_count_identities.json`, twenty-two focused
reports/source snapshots/objects and rechecked typedpair validation audit.


## Continuation: Create child parse records with narrow read status

Seventeen focused Create measurements start from 2d707930: accepted control
and eight local child ParseState forms crossed with unsigned short/unsigned
char representation of the child MeshReadData result. Parse records group
handle/childPtr, childPtr/handle, subCount/childPtr, childPtr/subCount,
handle/subCount/childPtr, childPtr/subCount/handle, or all four live parsing
locals with handle-first or child-first order. No unused field, fabricated
padding or new initialization is added. Only original handle zeroing occurs;
magic/subCount/childPtr remain read/assignment-owned. File read order and child
allocation, merge, append, failure cleanup and success paths stay unchanged.

The child call result is explicitly converted once and compared against the
same type's converted -1. This is equivalent for the actual accepted Read,
whose parsed function body has exactly return expressions -1 and 1. That
range was asserted before constructing trials. The enhanced-single ==1 and
plain-single !=-1 predicates are untouched. No additional reader invocation
occurs. Narrow comparisons are not claimed equivalent for an arbitrary mocked
reader returning other integers, nor recovered original declarations. This is
a scoped source/codegen hypothesis, not a new error-handling contract or ABI.

| Trial | Create source / strict | Instructions | Calls | Accepted whole Core |
|---|---|---:|---:|---|
| accepted_control | 98.4 / 60.7 | 289 | 100.0 | yes |
| handle_child_unsigned_short | 97.6 / 58.9 | 293 | 100.0 | no |
| handle_child_unsigned_char | 97.8 / 59.0 | 292 | 100.0 | no |
| child_handle_unsigned_short | 97.4 / 56.6 | 296 | 100.0 | no |
| child_handle_unsigned_char | 97.6 / 56.7 | 295 | 100.0 | no |
| count_child_unsigned_short | 97.5 / 61.3 | 293 | 100.0 | no |
| count_child_unsigned_char | 97.7 / 61.4 | 292 | 100.0 | no |
| child_count_unsigned_short | 97.9 / 52.5 | 292 | 100.0 | no |
| child_count_unsigned_char | 98.0 / 52.6 | 291 | 100.0 | no |
| handle_count_child_unsigned_short | 97.4 / 54.4 | 294 | 100.0 | no |
| handle_count_child_unsigned_char | 97.5 / 54.5 | 293 | 100.0 | no |
| child_count_handle_unsigned_short | 97.4 / 56.5 | 297 | 100.0 | no |
| child_count_handle_unsigned_char | 97.6 / 56.6 | 296 | 100.0 | no |
| all_handle_first_unsigned_short | 97.4 / 48.9 | 298 | 100.0 | no |
| all_handle_first_unsigned_char | 97.5 / 49.0 | 297 | 100.0 | no |
| all_child_first_unsigned_short | 97.3 / 56.6 | 296 | 100.0 | no |
| all_child_first_unsigned_char | 97.5 / 56.7 | 295 | 100.0 | no |

All sixteen record/narrow-status interactions regress to 97.3-98.0 with calls
100.0. Accepted control is exact accepted Core. No gain; none retained. All
seventeen compile/focused verify calls exit zero; source/call metrics recomputed
directly from target/base instructions and full object hashes checked. Source
generator retains deterministic forms; full-source snapshots are written only
for a better candidate, and none occurred. No new native/full-corpus run is
claimed for rejected forms, nor runtime proof for hypothetical callback behavior.

Source/fixtures restored to ecc30f25. Read-only binding recheck of accepted
full/native evidence passes: 204 TUs/4619 comparisons/zero skipped, 202
unaffected objects, actual linked 2079/byte 959 exact-pair sets equal baseline
with no lost/gained pairs, canonical arguments and outside-body/signature
boundaries unchanged. Accepted means **98.40% /98.41%**, both >=99.0 gates unmet,
goal active. No header/helper/ABI, flag/matcher/compiler, push, PR or merge
change; no general compiler ceiling established. Main checkout untouched.

Evidence: `_work/create_child_parse_status_identities.json`, seventeen focused
reports/objects, `_work/create_child_parse_status_sweep.py` and rechecked
typedpair validation audit.


## Continuation: Create call records with shared int result and result reuse

Twenty-two focused Create measurements start from 08ac4a8d. Fifteen are an
accepted control plus seven local CallState forms crossed with per-call or
function-scope shared int readStatus. Seven follow-up measurements reuse the
existing result instead of adding readStatus. CallState holds out/material,
engine/material, engine/out, all arguments, reversed arguments, handle/out or
out/handle. It contains only the original live values; handle is zeroed once,
all other members are copied scalar arguments. No unused field or manufactured
padding, helper/header/ABI or file/call order change. These source-state
arrangements are hypotheses, not recovered declarations.

All three MeshReadData calls occur once at their original positions. Result
storage remains int, preserving its entire range; child ==-1,
enhanced-single ==1 and plain-single !=-1 tests are unchanged. Per-call locals
live in their existing child/enhanced/plain scopes. Shared readStatus is not
pre-zeroed and each comparison immediately follows assignment. Reuse forms
remove that declaration and assign the original result instead. Invalid
engine/path paths perform no read and still return the original initial -4;
valid paths retain explicit -1 or 1 returns, so last-reader result cannot leak
into the function's return value. No reliance on narrowing or a restricted
mocked-reader result domain is needed here.

CallState/int result:

| Trial | Create source / strict | Instructions | Calls | Accepted whole Core |
|---|---|---:|---:|---|
| accepted_control | 98.4 / 60.7 | 289 | 100.0 | yes |
| out_material_per_call | 98.4 / 60.7 | 289 | 100.0 | yes |
| out_material_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| engine_material_per_call | 98.4 / 60.7 | 289 | 100.0 | yes |
| engine_material_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| engine_output_per_call | 98.4 / 60.7 | 289 | 100.0 | yes |
| engine_output_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| all_arguments_per_call | 98.4 / 60.7 | 289 | 100.0 | yes |
| all_arguments_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| arguments_reverse_per_call | 98.4 / 60.7 | 289 | 100.0 | yes |
| arguments_reverse_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| handle_output_per_call | 96.9 / 61.7 | 296 | 100.0 | no |
| handle_output_shared | 96.9 / 62.0 | 296 | 100.0 | no |
| output_handle_per_call | 96.6 / 58.5 | 297 | 100.0 | no |
| output_handle_shared | 96.6 / 58.8 | 297 | 100.0 | no |

Original result reuse:

| Trial | Create source / strict | Instructions | Calls | Accepted whole Core |
|---|---|---:|---:|---|
| out_material_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| engine_material_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| engine_output_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| all_arguments_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| arguments_reverse_shared | 98.4 / 61.0 | 289 | 100.0 | no |
| handle_output_shared | 96.9 / 62.0 | 296 | 100.0 | no |
| output_handle_shared | 96.6 / 58.8 | 297 | 100.0 | no |

Five per-call argument-only forms equal the entire accepted Core, as does
control. Corresponding five shared-result forms equal each other:
b54382dab09a73fe6de9038bf9083513dc24316a1aaaf0b6773d77bc8e134c05;
strict improves 60.7 to 61.0 but required source stays 98.4. Handle/out forms
regress to 96.9/96.6. All seven result-reuse objects equal their corresponding
shared-readStatus objects byte-for-byte. Calls stay 100.0. No gate gain; none
retained. No compiler ceiling inferred.

All twenty-two compile/focused verify calls exit zero; source/call metrics
independently recomputed directly from target/base instructions and whole
object hashes checked. Deterministic source generators retained, full-source
snapshots saved only for a better candidate, and none occurred. Source/fixtures
restored to ecc30f25. Read-only binding recheck of accepted full/native evidence
passes: 204 TUs/4619 comparisons/zero skipped, 202 unaffected objects, actual
linked 2079/byte 959 exact-pair sets equal baseline with no lost/gained pairs,
canonical arguments and outside-body/signature boundaries unchanged. No fresh
native/full-corpus run claimed for rejected forms. Accepted means
**98.40% /98.41%**; both >=99.0 gates unmet, goal active. No matcher/compiler/
flag, push, PR or merge change. Main checkout untouched.

Evidence: `_work/create_call_state_int_status_identities.json`,
`_work/create_call_state_result_reuse_identities.json`, focused reports/objects,
source generators and rechecked typedpair validation audit.


## Continuation: shared Create result with counter widths and format masks

Thirteen focused Create measurements start from d0e06b19. Controls are accepted
source and the shared-result form without CallState. Seven counter/format type
variants and four cached-mask variants combine with shared result. Each
MeshReadData result is assigned once to the existing int result and tested
with the unchanged child ==-1, enhanced-single ==1 or plain-single !=-1
predicate. Full int result range is preserved. Invalid engine/path still has
initial result -4 and performs no read; valid paths keep explicit -1/1 returns.
No new variable, call, zero initialization or side effect is added for status.

Type forms use signed int/unsigned long fmt, signed int/unsigned long child
counter, or unsigned char/unsigned short/unsigned int signature index. Format
stays in 0..31, child counter <=ushort subCount, signature index exits at 7,
so no overflow or wraparound. Only these original local declarations change;
wire widths, signature arrays/comparison order, stream reads, bounds and child
behavior remain. Omitted the old default-null child pointer form to avoid
unneeded initialization that is overwritten before use.

Mask forms capture headerWord or enhanced booleans, both booleans, or both
unsigned bit masks inside if(fmt), before the original timer reset. Masks use
exact original &0x1b/&0x1a expressions, replacing corresponding predicates.
fmt is not changed thereafter; timer store cannot change this local, and
signature/read/failure routing stays unchanged. These declarations/captures
are source hypotheses, not recovered original C++. No helper/header/ABI,
assembly, fixed register, padding, compiler option or verifier change.

| Trial | Create source / strict | Instructions | Calls | Accepted Core | Shared-result Core |
|---|---|---:|---:|---|---|
| accepted_control | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| shared_result_control | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| counter_signed_format | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| counter_long_format | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| counter_signed_child_index | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| counter_long_child_index | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| counter_byte_signature_index | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| counter_short_signature_index | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| counter_unsigned_signature_index | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| mask_header_bool | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| mask_enhanced_bool | 98.2 / 60.7 | 289 | 100.0 | no | no |
| mask_both_bools | 98.2 / 60.7 | 289 | 100.0 | no | no |
| mask_both_masks | 98.2 / 60.7 | 289 | 100.0 | no | no |

Accepted control, byte/ushort signature-index forms and header bool equal the
entire accepted Core. Shared-result control and five remaining type forms
equal the entire shared-result Core b54382dab09a73fe6de9038bf9083513dc24316a1aaaf0b6773d77bc8e134c05;
strict stays 61.0 but source remains 98.4. Enhanced bool/both bools/both masks
regress to 98.2 and equal each other byte-for-byte. All calls 100.0 and Create
289 instructions. No gate gain; no form retained. No compiler ceiling inferred.

All thirteen compile/focused verify calls exit zero. Source/call metrics
independently recomputed directly from target/base instructions, full object
hashes checked. Deterministic generator retained; full-source snapshots saved
only for a better candidate, and none occurred. Source/fixtures restored to
ecc30f25. Read-only accepted full/native binding recheck passes: 204 TUs, 4619
comparisons, zero skipped, 202 unaffected whole objects, actual linked 2079/
byte 959 exact-pair sets equal baseline with no lost/gained pairs, canonical
arguments and outside-body/signature boundaries unchanged. No new native/full
run claimed for rejected forms. Accepted means **98.40% /98.41%**; both >=99.0
gates unmet, goal active. No push, PR or merge; main checkout untouched.

Evidence: `_work/create_shared_result_counter_mask_identities.json`, thirteen
focused reports/objects, source generator and rechecked typedpair validation audit.


## Continuation: Enhanced center views with V5 channel preload

Twenty-four focused Enhanced measurements start from ba12d32c: eleven center/
bounds-read forms with accepted and preloaded V5 dispatch, plus both controls.
The existing Y=oldZ, Z=-oldY remap occurs after four successful original
bounds reads. No X or radius store, floating operation beyond the existing
negation, decode precision or timer/key change is introduced. New scalar,
two-member record and two-element array views capture Y/Z in either order,
then write Y followed by Z. Each record/array member is used; no unused field
or manufactured padding is included. Independent field loads occur without
intervening calls. These local declarations are source hypotheses.

Earlier word-copy forms memcpy the original Z value or both incoming values
at float width, retaining negation and two output stores. Negative-before-store
form negates the saved Y earlier; Z-store-first captures both values then swaps
the two output-store positions. No call intervenes between those stores. Shared
read-failure guard uses left-to-right short-circuit OR for the four original
Read(...)=0 predicates, preserving sizes/order and stopping at the same failed
read. Coordinate remap still waits for all four successes.

Preloaded forms use the already measured channel=0x400/case0-break/default-zero
V5 switch. Masks 4/5 still insert zero-channel keys; all other masks, signed
count and exact /100 then tau multiply then /360 math remain. No helper/header/
ABI, compiler/flag or verifier change. No recovered source declaration claimed.

| Trial | Enhanced source / strict | Instructions | Calls | Accepted Mesh | Preload-only Mesh |
|---|---|---:|---:|---|---|
| accepted_control | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| preload_control | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| z_word_copy_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| z_word_copy_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| both_word_copy_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| both_word_copy_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| negative_before_y_store_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| negative_before_y_store_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| z_store_first_accepted | 98.3 / 66.9 | 641 | 100.0 | no | no |
| z_store_first_preload | 98.3 / 67.8 | 639 | 100.0 | no | no |
| shared_read_failure_guard_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| shared_read_failure_guard_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| scalar_yz_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| scalar_yz_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| pair_yz_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| pair_yz_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| array_yz_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| array_yz_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| scalar_zy_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| scalar_zy_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| pair_zy_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| pair_zy_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| array_zy_accepted | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| array_zy_preload | 98.3 / 67.8 | 639 | 100.0 | no | yes |

All twenty-four source values tie 98.3 and calls stay 100.0. Eleven equal the
entire accepted Mesh and eleven the entire preload-only Mesh. Only two
Z-store-first objects differ from their corresponding controls, retaining
same metrics/instruction counts. No gate gain; none retained. No general
compiler ceiling inferred.

All twenty-four compile/focused verify calls exit zero. Source/call metrics
independently recomputed directly from target/base instruction arrays and full
object hashes checked. Every source snapshot retained. Code/fixtures restored
to ecc30f25. Read-only binding recheck of existing accepted full/native evidence
passes: 204 TUs/4619 comparisons/zero skipped, 202 unaffected objects, actual
linked 2079/byte 959 exact-pair sets equal baseline with no lost/gained pairs,
canonical arguments and outside-body/signature boundaries unchanged. No fresh
native/full-corpus run claimed for rejected forms. Accepted means
**98.40% /98.41%**; both >=99.0 gates unmet, goal active. No push, PR or merge;
main checkout untouched.

Evidence: `_work/enh_preload_bounds_views_identities.json`, twenty-four focused
reports/source snapshots/objects and rechecked typedpair validation audit.


## Continuation: V5 preload with completion receivers, count capture and cleanup layout

Fourteen focused measurements start from 56579c34: ten completion receiver/
count forms including control, then four completion-layout forms including
control. All use the previously measured V5 channel=0x400/case0-break/default-
zero dispatch. Cases 4/5 still insert zero-channel keys; all other masks,
signed counts, key math and bounds conversion remain unchanged.

Original ARM loads the key-frame size at 0x11c and compares it signed against
one. Transform.h records and asserts the same offset. Original fp here is
the animation pointer; the completion instruction sequence stores it into
the mesh and uses it as receiver. Register differences do not establish an
ABI/layout defect. These are source-form hypotheses, not recovered declarations.

Six receiver forms use the attached field for start/range access, a named
attached pointer, an explicit range-start local, or their combinations. Three
count forms capture the signed size in int, const int or bool at completion.
The first attempted count declaration was in function scope and failed to
compile because earlier goto fail bypassed its initialization. The generator
now wraps completion in a nested scope. The entire ten-form sweep was rerun
successfully, including the previously completed receiver/control forms.

The second sweep puts failure cleanup before a finish label, or shares the
empty-success and failure release paths with a result local and two equivalent
label orders. No destructor/delete is added to any executed path; positive
key counts still attach animation, empty success returns 1 and read failure
returns -1. Result initialization precedes every failure jump. No new helper,
header, ABI, matcher, flag, denominator, dummy effect or padding change.

| Family / trial | Enhanced source / strict | Instructions | Calls | Preload-only Mesh |
|---|---|---:|---:|---|
| finishstate / preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / receiver_field_start_receiver | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / receiver_field_range_receiver | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / receiver_both_field_receivers | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / receiver_named_attached_receiver | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / receiver_explicit_range_start | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / receiver_field_receiver_explicit_start | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / count_local | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / count_const | 98.3 / 67.8 | 639 | 100.0 | yes |
| finishstate / count_bool | 98.3 / 67.8 | 639 | 100.0 | yes |
| completionlayout / preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| completionlayout / failure_before_finish | 98.3 / 67.8 | 639 | 100.0 | no |
| completionlayout / common_release_result | 97.2 / 68.9 | 641 | 96.2 | no |
| completionlayout / common_release_fallthrough | 97.2 / 68.9 | 641 | 96.2 | no |

All ten receiver/count forms produce the entire preload-only object byte for
byte. Failure-before-finish changes the object but ties source 98.3. The two
shared-release forms produce the same object and regress source to 97.2;
call-count shape is 96.2 despite preserved path-level cleanup behavior. Their
strict-order increase is not either goal gate. None retained; no general
compiler ceiling inferred.

All fourteen final compile/focused verifier calls exited zero. Source/call
metrics independently recomputed from instruction arrays; object hashes,
instruction counts and source snapshot presence checked. Code/fixtures match
ecc30f25 after restoration. Read-only binding audit of existing accepted
full/native evidence passes: 204 TUs, 4619 comparisons, zero skipped, 202
unaffected objects, actual linked 2079 / byte 959 pair sets equal baseline,
no lost or gained pairs; canonical arguments and body/signature boundaries
unchanged. This is no fresh native/full-corpus run for rejected variants and
does not prove whole-native-game runtime/device behavior.

Accepted means remain **98.40% / 98.41%**. Both >=99.0 gates unmet; goal active.
Main checkout untouched; no push, PR or merge.

Evidence: `_work/enh_preload_finish_state_identities.json`,
`_work/enh_preload_completion_layout_identities.json`, fourteen focused reports,
objects/source snapshots, corrected generators and typedpair validation audit.


## Continuation: V5 preload with entry/flag lifetimes, mixed axes and presence flow

Twenty-five focused measurements start from 81c18185. First ten combine V5
preloaded dispatch with entry/section-flag lifetime forms and control. Second
fifteen combine it with mixed signed/byte axis counters, optional-section
presence/format/type flow forms and control. These interactions had not been
measured by their earlier standalone sweeps. No recovered declarations claimed.

Entry forms move self, format or both after the same Transform allocation,
or use signed char for format. Only masks 0x18/0x10 inspect format, so signed
promotion does not change either predicate. Section-flag forms capture these
two masks as bool or unsigned int at entry or inside the optional block;
another captures bools just after allocation. Flags are by value and unchanged.
No read, allocation, cleanup or insertion is moved across an executed call.

Mixed-axis forms replace one or two of the three unsigned-char 0..2 loops
with int 0..2 loops. Bounds, iteration order and channel/key math remain.
Presence-flow forms express absent optional-count, format or type guards as
early jumps to their original section exit, or name the nonzero predicate,
or use unsigned-count >0. They retain read order and do not bypass required
failure cleanup, initialization or successful animation attachment.

Every form retains preloaded channel=0x400/case0-break/default-zero, including
zero-channel insertion for cases 4/5. No helper/header/ABI change, extra effect,
padding, compiler/flag or verifier change, nor denominator change.

| Family / trial | Enhanced source / strict | Instructions | Calls | Preload-only Mesh |
|---|---|---:|---:|---|
| entrylife / preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| entrylife / entry_format_after_allocation | 98.3 / 67.8 | 639 | 100.0 | yes |
| entrylife / entry_self_after_allocation | 98.3 / 67.8 | 639 | 100.0 | yes |
| entrylife / entry_both_after_allocation | 98.3 / 67.8 | 639 | 100.0 | yes |
| entrylife / entry_signed_format_current | 98.3 / 67.8 | 639 | 100.0 | yes |
| entrylife / flags_bool_entry | 98.2 / 67.9 | 639 | 100.0 | no |
| entrylife / flags_bool_optional | 98.3 / 67.9 | 639 | 100.0 | no |
| entrylife / flags_mask_entry | 98.2 / 67.9 | 639 | 100.0 | no |
| entrylife / flags_mask_optional | 98.3 / 67.9 | 639 | 100.0 | no |
| entrylife / flags_bool_after_allocation | 98.2 / 67.9 | 639 | 100.0 | no |
| axpresence / preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / mixed_axis_signed_axes_0 | 98.2 / 67.7 | 638 | 100.0 | no |
| axpresence / mixed_axis_signed_axes_1 | 98.2 / 67.7 | 638 | 100.0 | no |
| axpresence / mixed_axis_signed_axes_2 | 98.2 / 67.7 | 638 | 100.0 | no |
| axpresence / mixed_axis_signed_axes_0_1 | 98.1 / 67.6 | 637 | 100.0 | no |
| axpresence / mixed_axis_signed_axes_0_2 | 98.1 / 67.6 | 637 | 100.0 | no |
| axpresence / mixed_axis_signed_axes_1_2 | 98.1 / 67.6 | 637 | 100.0 | no |
| axpresence / presence_flow_v5_presence_early | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_v5_format_early | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_v5_both_early | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_v5_named_presence | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_v5_positive_presence | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_optional_type_early | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_optional_format_early | 98.3 / 67.8 | 639 | 100.0 | yes |
| axpresence / presence_flow_optional_both_early | 98.3 / 67.8 | 639 | 100.0 | yes |

Fourteen entire objects equal preload-only control: five entry/control and
nine presence/control. Three entry bool/mask captures share one object and
regress source to 98.2; two delayed captures share another object and tie
98.3. One signed axis regresses to 98.2, two signed axes to 98.1. No gate
gain; none retained. No general compiler ceiling inferred.

All twenty-five compile/focused verifier calls exited zero. Source/call
metrics independently recomputed from instruction arrays, whole-object hashes
and instruction counts checked; all source snapshots present. Restored code
and fixtures match ecc30f25. Read-only audit rebinds existing accepted native/
full-corpus evidence: 204 TUs, 4619 comparisons, zero skipped, 202 unaffected
objects; actual linked 2079 / byte 959 pair sets equal baseline with no lost
or gained pairs, canonical arguments and outside-body/signature boundaries
unchanged. Rejected forms received no fresh native/full-corpus runtime proof.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. No
push, PR or merge; main checkout untouched. Next investigation must address
other residual instruction differences, since these tested lifetime/presence
interactions add no useful gate gain.

Evidence: `_work/enh_preload_entry_lifetime_identities.json`,
`_work/enh_preload_axis_presence_identities.json`, twenty-five focused reports,
objects/source snapshots, generators and rechecked typedpair validation audit.


## Continuation: Read extrema with fresh index owner and per-vertex accumulator memset

Twenty-two focused Read measurements start from a88f1d33: twelve extrema
local/reference captures and nine extrema predicate/order forms, plus control.
All combine the previously measured fresh index owner after indexCount Read
with accumulator memset of twelve bytes per vertex. Original ARM has the
per-vertex twelve-byte call/decrement sequence. This interaction had not been
measured by the earlier reload-only extrema or memset+tangent-lifetime sweeps.
No recovered local declarations claimed.

Extrema forms apply to short-position, integer-position or both decode loops.
They capture minimum alone, minimum/maximum in their original order, preload
both, or refer to their active array elements. Ordered forms reverse operands
of the same comparison, test maximum first, or combine both. Converted source
values are signed short/int finite floats; min/max arrays have disjoint
components and no intervening call. Same extrema stores and comparisons, no
extra clamp, half-radius, center math or decode/normal precision change.

Accumulator remains allocated at vcount*12; memset clears exactly three floats
per existing vertex iteration. No manufactured call padding or extra touched
bytes. Fresh index owner is reloaded after the original field read, as in the
already measured standalone form. It is not a universal aliasing guarantee for
arbitrary external callbacks that mutate slot. Native reader mocks do not
establish such callback behavior. None of these trial changes is retained.

No helper/header/ABI, compiler/flag, matcher/denominator or body-boundary change.

| Trial | Read source / strict | Instructions | Calls | Memset+reload control object |
|---|---|---:|---:|---|
| reload_control | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_minimum_local_short | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_minimum_local_integer | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_minimum_local_both | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_sequential_locals_short | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_sequential_locals_integer | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_sequential_locals_both | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_both_preloaded_short | 98.5 / 57.4 | 873 | 100.0 | no |
| locals_both_preloaded_integer | 98.5 / 57.4 | 873 | 100.0 | no |
| locals_both_preloaded_both | 98.5 / 56.9 | 873 | 100.0 | no |
| locals_component_references_short | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_component_references_integer | 98.5 / 57.8 | 873 | 100.0 | yes |
| locals_component_references_both | 98.5 / 57.8 | 873 | 100.0 | yes |
| order_reverse_operands_short | 98.5 / 57.8 | 873 | 100.0 | yes |
| order_reverse_operands_integer | 98.5 / 57.8 | 873 | 100.0 | yes |
| order_reverse_operands_both | 98.5 / 57.8 | 873 | 100.0 | yes |
| order_maximum_first_short | 98.5 / 57.4 | 873 | 100.0 | no |
| order_maximum_first_integer | 98.5 / 57.4 | 873 | 100.0 | no |
| order_maximum_first_both | 98.5 / 56.9 | 873 | 100.0 | no |
| order_reverse_maximum_first_short | 98.5 / 57.4 | 873 | 100.0 | no |
| order_reverse_maximum_first_integer | 98.5 / 57.4 | 873 | 100.0 | no |
| order_reverse_maximum_first_both | 98.5 / 56.9 | 873 | 100.0 | no |

All twenty-two tie source 98.5, with 873 instructions and 100.0 call-count
shape. Thirteen entire objects equal memset+reload control. Nine preload/max-
first forms differ but do not raise either goal gate. Source restored; none
retained. No general compiler ceiling inferred.

Completed ledger contains all expected twenty-two rows, each appended only
after successful compile and focused verify subprocesses with check=True.
Source/call metrics independently recomputed from instruction arrays; object
hashes and instruction counts checked. Deterministic source generators and
objects/reports retained; no separate source snapshot for non-improving forms.
Code/fixtures match ecc30f25 after restoration. Read-only audit rebinds prior
accepted native/full evidence: 204 TUs, 4619 comparisons, zero skipped, 202
unaffected objects; actual linked 2079 / byte 959 pair sets equal baseline
with no lost/gained pairs, canonical arguments and outside-body/signature
boundaries unchanged. No fresh native/full-corpus run for rejected forms.

Accepted means remain **98.40% / 98.41%**; both >=99.0 gates unmet, goal active.
No push, PR or merge; main checkout untouched. These extrema+memset interactions
do not justify further tests of equivalent predicate spellings.

Evidence: `_work/read_reload_memzero_extrema_identities.json`, twenty-two
focused reports/objects, `_work/prepare_reload_memzero_extrema.py`, generated
sweep and rechecked typedpair validation audit.


## Continuation: Read normal views and guards with per-vertex memset and fresh index owner

Thirty-one focused Read measurements start from cfe33832: twenty-one normal
source/destination view and counter forms including control, followed by ten
normal length/section guard forms including control. Both families combine
the previously measured fresh index owner and twelve-byte per-vertex memset.
These combinations had not been covered by reload-only normal sweeps. Source
forms are hypotheses, not recovered declarations.

Twelve pointer-view forms express equivalent normal input/output addresses
around centers zero, one, two or three. Center-three pointers are at most
one-past the allocated array; actual loads/stores use negative offsets inside
the same three-component tuple. Eight source/counter forms name an uncentered
input pointer, advance that pointer, use vertex counters of unsigned int/int/
unsigned short, or combine counter and pointer progression. No extra input
read, output store, clamp, floating reassociation or changed precision.
Counts are 16-bit; counters visit 0..count-1 without wrap for valid input.

Length forms reverse the zero/nonzero branch, use continue for either completed
case, name the same predicate, or preload the destination owner before it.
Zero length still writes (0,1,0); other values execute original normalization/
clamping. Section forms split shader processing, flatten compressed/plain
dispatch, express independent predicates or share an early section exit.
Shader processing still requires normal-enabled and shader-enabled flags;
failed reads retain original cleanup/return paths. No helper/header/ABI,
compiler/flag, matcher/denominator, dummy effect or padding change.

The already documented fresh-owner callback limitation remains: this is not
a universal claim for arbitrary external callbacks that mutate slot. No trial
source is retained, nor runtime proof asserted for these rejected forms.

| Family / trial | Read source / strict | Instructions | Calls | Memset+reload control object |
|---|---|---:|---:|---|
| views / reload_control | 98.5 / 57.8 | 873 | 100.0 | yes |
| views / view_destination_0 | 98.1 / 56.2 | 868 | 100.0 | no |
| views / view_destination_2 | 98.3 / 56.7 | 869 | 100.0 | no |
| views / view_destination_3 | 98.0 / 57.4 | 863 | 100.0 | no |
| views / view_source_1_destination_1 | 98.5 / 57.4 | 873 | 100.0 | no |
| views / view_source_1_destination_2 | 98.3 / 56.3 | 869 | 100.0 | no |
| views / view_source_1_destination_3 | 97.9 / 55.0 | 864 | 100.0 | no |
| views / view_source_2_destination_1 | 98.5 / 57.8 | 873 | 100.0 | yes |
| views / view_source_2_destination_2 | 98.3 / 56.7 | 869 | 100.0 | no |
| views / view_source_2_destination_3 | 97.9 / 55.4 | 864 | 100.0 | no |
| views / view_source_3_destination_1 | 98.5 / 57.8 | 873 | 100.0 | yes |
| views / view_source_3_destination_2 | 98.3 / 56.7 | 869 | 100.0 | no |
| views / view_source_3_destination_3 | 97.9 / 55.0 | 864 | 100.0 | no |
| views / counter_source_record | 98.5 / 57.8 | 873 | 100.0 | yes |
| views / counter_advancing_source | 97.9 / 56.8 | 869 | 100.0 | no |
| views / counter_vertex_index_unsigned_int | 98.2 / 57.1 | 872 | 100.0 | no |
| views / counter_vertex_advancing_unsigned_int | 98.1 / 56.8 | 872 | 100.0 | no |
| views / counter_vertex_index_int | 98.2 / 57.1 | 872 | 100.0 | no |
| views / counter_vertex_advancing_int | 98.1 / 56.8 | 872 | 100.0 | no |
| views / counter_vertex_index_unsigned_short | 98.2 / 57.1 | 872 | 100.0 | no |
| views / counter_vertex_advancing_unsigned_short | 98.1 / 56.8 | 872 | 100.0 | no |
| guards / reload_control | 98.5 / 57.8 | 873 | 100.0 | yes |
| guards / length_fallback_first | 98.3 / 60.1 | 873 | 100.0 | no |
| guards / length_zero_continue | 98.3 / 60.1 | 873 | 100.0 | no |
| guards / length_nonzero_continue | 98.5 / 57.8 | 873 | 100.0 | yes |
| guards / length_named_nonzero | 98.5 / 57.8 | 873 | 100.0 | yes |
| guards / length_owner_before_branch | 98.1 / 59.3 | 870 | 100.0 | no |
| guards / section_split_shader | 98.5 / 57.8 | 873 | 100.0 | yes |
| guards / section_flat_shader | 97.5 / 55.4 | 891 | 100.0 | no |
| guards / section_independent_shader | 97.3 / 56.6 | 884 | 98.2 | no |
| guards / section_common_section_exit | 98.5 / 57.8 | 873 | 100.0 | yes |

Nine entire objects equal memset+reload control. Source ranges 97.9..98.5 for
views/counters and 97.3..98.5 for guards. Independent section predicates lower
call-count shape to 98.2; others retain 100.0. No goal gate gain. Source restored;
none retained. No general compiler ceiling inferred.

Both completed ledgers contain all expected rows, each appended after compile
and focused verify with check=True; final guards sweep explicitly exited zero.
Source/call metrics independently recomputed from instruction arrays; whole-
object hashes and instruction counts checked. Deterministic source generators,
objects and reports retained; no separate source snapshots for non-improving
forms. Code/fixtures match ecc30f25 after restoration. Read-only audit rebinds
prior accepted native/full evidence: 204 TUs, 4619 comparisons, zero skipped,
202 unaffected objects; actual linked 2079 / byte 959 pair sets equal baseline
with no lost/gained pairs, canonical arguments and body/signature boundaries
unchanged. No fresh native/full-corpus run for rejected variants.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. Main
checkout untouched; no push, PR or merge. These normal forms combined with
per-vertex memset do not justify keeping or repeating any tested variant.

Evidence: `_work/read_reload_memzero_normal_views_identities.json`,
`_work/read_reload_memzero_normal_guards_identities.json`, thirty-one focused
reports/objects, deterministic preparation/sweep scripts and typedpair audit.


## Continuation: Read position predicates and bounds values with per-vertex memset

Twenty-one focused Read measurements start from 26ec624a. Ten combine fresh
index owner and twelve-byte per-vertex accumulator memset with three position
compression predicates, four direct output assignments, two shared raw-buffer
lifetime forms and control. Eleven combine them with five bounds Vector copy/
diagonal expressions, five center/extrema lifetime/order forms and control.
These interactions were absent from earlier reload-only sweeps.

Original ARM has movs of shifted flags and bmi at the position compression
choice, while accepted source checks flags&4. Sign-test forms express this
through (int)(flags<<29), shared compressedData, or signed compressedData.
This predicate tests the same bit on this ARM ABI. Later nonzero compressed-
data tests retain their predicates. Direct outputs assign allocation result
directly into m->positions; shared raw-buffer locals are reused only across
mutually exclusive branches. Typed raw allocations and paired deletes remain.

Bounds forms copy all three existing extrema components into Vector values
or use direct typed views, keep a temporary/subexpression diagonal, scope the
same center computation in float array or Vector, reorder disjoint extrema
copies, or construct extrema before existing center/assignment. They retain
the same max-minus-min diagonal and full VectorLength radius, and identical
center additions/half multiplies. No changed floating precision/reassociation,
extra output store or replaced geometry formula. Local spellings are source
hypotheses, not recovered declarations.

Fresh-owner callback limitation remains as previously documented. No helper,
header/ABI, compiler/flag, matcher/denominator, dummy effect or padding change.

| Family / trial | Read source / strict | Instructions | Calls | Memset+reload control object |
|---|---|---:|---:|---|
| position / reload_control | 98.5 / 57.8 | 873 | 100.0 | yes |
| position / predicate_direct_shift_negative | 97.7 / 57.8 | 877 | 98.2 | no |
| position / predicate_shared_shift_negative | 97.7 / 57.8 | 877 | 98.2 | no |
| position / predicate_signed_compressed_local | 97.7 / 57.8 | 877 | 98.2 | no |
| position / direct_output_short | 98.5 / 57.8 | 873 | 100.0 | yes |
| position / direct_output_integer | 98.5 / 57.8 | 873 | 100.0 | yes |
| position / direct_output_floating | 98.5 / 57.8 | 873 | 100.0 | yes |
| position / direct_output_all | 98.5 / 57.8 | 873 | 100.0 | yes |
| position / shared_raw | 98.5 / 57.8 | 873 | 100.0 | yes |
| position / shared_raw_direct_outputs | 98.5 / 57.8 | 873 | 100.0 | yes |
| boundsvalues / reload_control | 98.5 / 57.8 | 873 | 100.0 | yes |
| boundsvalues / copy_maximum_copy | 98.4 / 57.3 | 875 | 100.0 | no |
| boundsvalues / copy_minimum_copy | 98.4 / 57.0 | 874 | 100.0 | no |
| boundsvalues / copy_both_copies | 98.2 / 57.6 | 876 | 100.0 | no |
| boundsvalues / copy_direct_views | 98.2 / 58.3 | 869 | 100.0 | no |
| boundsvalues / copy_temporary_diagonal | 98.5 / 57.8 | 873 | 100.0 | yes |
| boundsvalues / lifetime_scoped_float_center | 98.5 / 57.8 | 873 | 100.0 | no |
| boundsvalues / lifetime_scoped_vector_center | 98.5 / 57.8 | 873 | 100.0 | no |
| boundsvalues / lifetime_minimum_first | 98.5 / 57.8 | 873 | 100.0 | no |
| boundsvalues / lifetime_extents_before_assignment | 98.5 / 57.0 | 873 | 100.0 | no |
| boundsvalues / lifetime_extents_before_center | 97.2 / 40.9 | 884 | 98.2 | no |

Nine entire objects equal memset+reload control. All three sign predicates
produce one object, regressing source to 97.7 and call-count shape to 98.2.
Bounds copies/views range 98.2..98.5; extents-before-center regresses to 97.2
and call shape 98.2. Remaining lifetimes tie 98.5. No gate gain; all trial
changes restored. No general compiler ceiling inferred.

Both sweeps explicitly exited zero; all twenty-one compile/focused verify calls
completed. Metrics independently recomputed from target/base instructions;
whole-object hashes and instruction counts checked. Source generators, objects
and reports retained, without separate snapshots for non-improving forms.
Code/fixtures restored to ecc30f25. Read-only binding audit of prior accepted
native/full proof passes: 204 TUs, 4619 comparisons, zero skipped, 202 unaffected
objects; actual linked 2079 / byte 959 pair sets equal baseline, no lost/gained
pairs; canonical arguments and body/signature boundaries unchanged. No fresh
native/full-corpus runtime test for rejected forms.

Before these sweeps, disk space was reclaimed by deleting exactly 408 untracked
generated original target-object duplicates in zero-full/target and uv-full/
target. Every absolute path was verified under this worktree's _work; each
SHA256 was matched against retained typedpair-full/target, before manifest
write and again immediately before individual Remove-Item -LiteralPath.
No recursive deletion, source/base/report/baseline deletion or main change.
Manifest has a separate host temporary-directory backup; canonical replacement
paths and hashes rechecked here. Logical bytes reclaimed: 3,453,736. Old report
target references in these two folders now use the manifest's canonical copy.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. No
push, PR or merge; main checkout untouched.

Evidence: `_work/read_reload_memzero_position_identities.json`,
`_work/read_reload_memzero_bounds_values_identities.json`, twenty-one focused
reports/objects, source generators, typedpair audit and
`_work/duplicate_target_cleanup_zero_uv_20261007.json`.


## Continuation: Create scoped magic/count/child locals with shared read result

Sixteen focused Create measurements start from c07cf921. Two controls and all
seven nonempty combinations of narrower local scopes are measured against both
accepted direct predicates and previously measured shared int result. This
uses current accepted bodies rather than the old create_scope_sweep baseline.
No recovered local/source layout claimed.

Scope bit 1 moves char magic[8] into successful OpenRead block; its strcpy,
seven-byte signature Read and subsequent two-byte header Read remain there in
the same order. Bit 2 moves unsigned short subCount into the enhanced-format
guard before its original Read. Bit 4 moves Mesh *childPtr into each child loop
iteration, initialized by its same new Mesh. No read, allocation, ownership,
merge, ArrayAdd, cleanup or Close moves across any executed call. Labels remain
outside exiting trivial local scopes, without skipped initialization or new
destructors. Count and child pointer types are unchanged; no storage overlay,
padding, fabricated field, helper/header/ABI or signature/denominator change.

Shared-result variant reuses existing int result for the three original
MeshReadData returns. Child failure still tests ==-1, enhanced-single success
==1, plain-single success !=-1. Result retains -4 only outside the original
engine/path guard; valid paths explicitly return 1 or -1. No narrowed status
domain assumed; arbitrary int statuses still use their original predicates.

| Trial | Create source / strict | Instructions | Calls | Accepted Core | Shared-result Core |
|---|---|---:|---:|---|---|
| accepted_control | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| shared_control | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| accepted_scope_1 | 98.2 / 52.8 | 289 | 100.0 | no | no |
| accepted_scope_2 | 97.7 / 54.8 | 289 | 100.0 | no | no |
| accepted_scope_3 | 97.7 / 57.2 | 289 | 100.0 | no | no |
| accepted_scope_4 | 98.4 / 55.5 | 289 | 100.0 | no | no |
| accepted_scope_5 | 98.2 / 56.9 | 289 | 100.0 | no | no |
| accepted_scope_6 | 97.7 / 58.6 | 289 | 100.0 | no | no |
| accepted_scope_7 | 98.0 / 57.9 | 289 | 100.0 | no | no |
| shared_scope_1 | 98.2 / 53.1 | 289 | 100.0 | no | no |
| shared_scope_2 | 97.7 / 55.2 | 289 | 100.0 | no | no |
| shared_scope_3 | 97.7 / 57.6 | 289 | 100.0 | no | no |
| shared_scope_4 | 98.4 / 55.9 | 289 | 100.0 | no | no |
| shared_scope_5 | 98.2 / 57.2 | 289 | 100.0 | no | no |
| shared_scope_6 | 97.7 / 59.0 | 289 | 100.0 | no | no |
| shared_scope_7 | 98.0 / 58.3 | 289 | 100.0 | no | no |

Both controls reproduce their entire reference object. Scoped child alone
ties 98.4 but differs from both control objects. All other scoped forms regress
to 97.7..98.2. Each accepted/shared scope pair has identical source score;
strict-order differences do not raise the requested gates. All sixteen have
289 instructions and 100.0 call-count shape. No trial retained and no general
compiler ceiling inferred.

Sweep explicitly exited zero. All sixteen compile/focused verify calls pass;
source/call metrics independently recomputed from instruction arrays and full
object hashes/counts checked. Deterministic generator and focused objects/
reports retained; no separate source snapshots for non-improving forms.
Code/fixtures match ecc30f25 after restoration. Read-only accepted full/native
binding audit passes: 204 TUs, 4619 comparisons, zero skipped, 202 unaffected
objects; actual linked 2079 / byte 959 pair sets equal baseline with no lost/
gained pairs, canonical arguments and body/signature boundaries unchanged.
No fresh native/full-corpus run claimed for rejected variants.

Before this sweep, another 204 generated original target-object duplicates
from channelzero-full/target were removed individually, after absolute-path
workspace containment, untracked status and matching SHA256 to retained
typedpair-full/target checks. Manifest and separate host-temp backup precede
removal; hashes checked again before deletion. No recursive delete and no
source/base/report/baseline/accepted-proof deletion. Reclaimed logical bytes:
1,726,868. Canonical replacement paths/hashes independently rechecked here;
old channelzero-full target references now use that manifest's retained copy.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. No
push, PR or merge; main checkout untouched.

Evidence: `_work/create_scope_shared_result_identities.json`, sixteen focused
reports/objects, `_work/create_scope_shared_result_sweep.py`, typedpair audit
and `_work/duplicate_target_cleanup_channelzero_20261007.json`.


## Continuation: Create shared result with root pointer and loaded operands

Eighteen focused Create measurements start from d757ca3a. Both previously
measured shared-int-result forms are controls: original child-pointer scope
and child pointer declared inside its iteration. Each receives four root-
pointer/initialization forms or four loaded-animation operand forms. No
recovered declaration/layout claimed; all predicates remain original.

Root variants assign new Mesh directly to output then set its material, use
the output as material receiver after original m assignment, store material
before output, or name a reference to the same new Mesh and use its address.
All perform one unchanged construction, one output store and one material
store. Only the material-before-output form reorders the two disjoint stores;
no call intervenes. Reference lifetime does not destroy the heap object at
scope exit. No allocation/constructor replacement or manufactured field.

Loaded variants retain Close then animation lookup/nonzero check then
CollectAnimationData. They capture the same rate float, optionally capture
the mesh before float-to-long-long conversion, name an explicit start value,
or reuse m after a fresh output lookup. Same rate conversion and end time,
same Collect/SetAnimationRangeInTime calls; no cached pointer moved before a
call which could change it. Shared result keeps child ==-1, enhanced-single
==1 and plain-single !=-1 predicates, valid returns 1/-1 and invalid guard -4.
No narrower arbitrary-int result assumption.

No helper/header/ABI, flags/compiler, matcher/denominator, dummy effect or
padding change. Scope/body/signature boundaries remain fixed.

| Trial | Create source / strict | Instructions | Calls | Accepted Core | Shared-result Core |
|---|---|---:|---:|---|---|
| shared_control | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| shared_root_direct_output | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| shared_root_output_material_receiver | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| shared_root_material_before_output | 98.4 / 61.4 | 289 | 100.0 | no | no |
| shared_root_root_reference | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| shared_finish_explicit_float_rate | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| shared_finish_mesh_before_conversion | 98.4 / 60.7 | 289 | 100.0 | yes | no |
| shared_finish_explicit_start | 98.3 / 60.3 | 289 | 100.0 | no | no |
| shared_finish_reuse_mesh_local | 98.4 / 61.0 | 289 | 100.0 | no | yes |
| shared_child_scope_control | 98.4 / 55.9 | 289 | 100.0 | no | no |
| shared_child_scope_root_direct_output | 98.4 / 55.9 | 289 | 100.0 | no | no |
| shared_child_scope_root_output_material_receiver | 98.4 / 55.9 | 289 | 100.0 | no | no |
| shared_child_scope_root_material_before_output | 98.4 / 56.2 | 289 | 100.0 | no | no |
| shared_child_scope_root_root_reference | 98.4 / 55.9 | 289 | 100.0 | no | no |
| shared_child_scope_finish_explicit_float_rate | 98.4 / 55.5 | 289 | 100.0 | no | no |
| shared_child_scope_finish_mesh_before_conversion | 98.4 / 55.5 | 289 | 100.0 | no | no |
| shared_child_scope_finish_explicit_start | 98.3 / 55.2 | 289 | 100.0 | no | no |
| shared_child_scope_finish_reuse_mesh_local | 98.4 / 55.9 | 289 | 100.0 | no | no |

Both controls reproduce their reference objects. Sixteen source values tie
98.4; explicit-start forms regress to 98.3. Five entire objects equal shared
control and five equal scoped-child shared control; two equal accepted Core
after explicit float capture in the unscoped-child form. Material-first
strict increases do not raise either goal gate. All eighteen have 289
instructions and 100.0 call-count shape. None retained; no general compiler
ceiling inferred.

Sweep explicitly exited zero. All eighteen compile/focused verify calls pass;
source/call metrics recomputed directly from target/base instructions, whole
object hashes and instruction counts checked. Deterministic generator and
focused objects/reports retained, without separate source snapshots for
non-improving forms. Code/fixtures match ecc30f25 after restoration. Read-only
binding audit of prior accepted native/full proof passes: 204 TUs, 4619
comparisons, zero skipped, 202 unaffected objects; actual linked 2079 / byte
959 pair sets equal baseline with no lost/gained pairs, canonical arguments
and body/signature boundaries unchanged. No fresh native/full-corpus runtime
proof claimed for rejected variants.

Before this sweep, 204 more untracked generated original target duplicates
from createstatus-full/target were removed individually after absolute-path
workspace containment and matching SHA256 to retained typedpair-full/target
checks. Manifest plus separate host-temp backup precede removal; hashes
rechecked before each deletion. No recursive delete or source/base/report/
baseline/accepted-proof removal. Reclaimed logical bytes: 1,726,868. Canonical
replacement paths/hashes independently rechecked here; old createstatus-full
target references now use that manifest's retained copy.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. Main
checkout untouched; no push, PR or merge.

Evidence: `_work/create_shared_scope_root_finish_identities.json`, eighteen
focused reports/objects, `_work/create_shared_scope_root_finish_sweep.py`,
typedpair audit and `_work/duplicate_target_cleanup_createstatus_20261007.json`.


## Continuation: Enhanced optional value lifetimes and timer guards with V5 preload

Seventeen focused Enhanced measurements start from 20abca80: four optional/V5
value-lifetime forms, twelve timer guard/group forms and preload-only control.
All use previously measured channel=0x400/case0-break/default-zero dispatch,
retaining zero-channel insertion in cases 4/5. Earlier standalone timer/value
sweeps did not measure these combinations with V5 preload.

Lifetime forms move value/key declarations outside their V5 key loop, outside
the channel loop, count outside the channel loop with per-channel values, or
optional value/key outside its loop. Variables remain assigned by the same
successful reads before every use. Signed V5 counts, optional counts, original
read order and first-read-failure exit remain; no initialization skipped by
failure labels, new allocation, insertion, precision or rotation change.

Timer variants name the same positive-time and smaller-time predicates, express
negative branches followed by an update, or use one guarded do/while(false)
block. Each applies to vector sections, axis sections, optional/V5 sections or
all eight sites. Conditions preserve >0 and current interval > input semantics,
including zero/negative/NaN behavior. No extra call/store when original update
is false, no changed global value read order, helper/header/ABI, flags/compiler,
matcher/denominator, dummy effect or padding change. These are source hypotheses,
not recovered declarations.

| Trial | Enhanced source / strict | Instructions | Calls | Preload-only Mesh |
|---|---|---:|---:|---|
| preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| lifetime_v5_channel_values | 97.5 / 64.4 | 644 | 100.0 | no |
| lifetime_v5_outer_values | 98.0 / 67.6 | 639 | 100.0 | no |
| lifetime_v5_outer_count_channel_values | 98.0 / 67.6 | 639 | 100.0 | no |
| lifetime_optional_outer_values | 98.1 / 67.4 | 642 | 100.0 | no |
| time_named_comparisons_vectors | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_named_comparisons_axes | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_named_comparisons_optional_v5 | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_named_comparisons_all | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_false_branches_vectors | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_false_branches_axes | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_false_branches_optional_v5 | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_false_branches_all | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_guarded_block_vectors | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_guarded_block_axes | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_guarded_block_optional_v5 | 98.3 / 67.8 | 639 | 100.0 | yes |
| time_guarded_block_all | 98.3 / 67.8 | 639 | 100.0 | yes |

All twelve timer forms and control produce the entire preload-only object.
Value lifetimes regress source to 97.5, 98.0, 98.0 and 98.1; outer values and
outer count/per-channel values produce the same object. Call-count shape stays
100.0 throughout. No goal gate gain; all changes restored. No general compiler
ceiling inferred.

Sweep explicitly exited zero; all seventeen compile/focused verify calls pass.
Source/call metrics independently recomputed from target/base instructions;
object hashes, instruction counts and all source snapshot paths checked.
Code/fixtures match ecc30f25 after restoration. Read-only accepted full/native
binding audit passes: 204 TUs, 4619 comparisons, zero skipped, 202 unaffected
objects; actual linked 2079 / byte 959 pair sets equal baseline with no lost/
gained pairs, canonical arguments and body/signature boundaries unchanged.
No fresh native/full-corpus runtime proof for rejected variants.

To restore room after the previous commit's automatic Git maintenance failed
with out-of-diskspace, only 1,632 untracked generated original target duplicates
were removed from eight old focused-full build target folders: attach-full,
boundsinit-full, colorfield-full, colorowner-full, component-full, dispatch-full,
floatuvbound-full and fullbounds-full. Absolute paths were checked under this
worktree's _work, SHA256 matched retained typedpair-full/target before manifest
write and again before individual Remove-Item -LiteralPath. No recursive delete,
Git archive operation or source/base/report/baseline/accepted-proof deletion.
Manifest has a separate host-temp backup. Canonical replacements rechecked
here; old target-folder references use manifest paths. Logical bytes reclaimed:
13,814,944. Previous 20abca80 report was already verified readable and equal
to its working file after maintenance failure. Subsequent report commit uses
per-command maintenance.auto=false/gc.auto=0, leaving persistent Git settings
and object archive untouched by a manual maintenance action.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. Main
checkout untouched; no push, PR or merge.

Evidence: `_work/enh_preload_optional_time_identities.json`, seventeen focused
reports/objects/source snapshots, generators, typedpair validation audit and
`_work/duplicate_target_cleanup_batch8_20261007.json`.


## Continuation: Enhanced V5 preload with vector key records, arrays and declaration order

Thirty-seven focused measurements start from f9ab2c53. First twenty-two include
fourteen vector-key record forms, seven nonempty early-vector-declaration
subsets and control. Second fifteen include fourteen equivalent four-float
array forms and control. Both families use already measured V5 preloaded mask
dispatch; cases 4/5 still insert zero-channel keys. No recovered declaration
or original buffer layout claimed.

Record forms use exactly one float time and three float coordinates, with time
first or coordinates first, for any nonempty subset of position/scale/rotation
vector sections. Every field is active. Array forms use exactly four floats,
time at index 0 with coordinates at 1..3, or time at index 3 with coordinates
at 0..2, in the same section subsets. The same four-byte time Read still
precedes timer update and the same twelve-byte coordinate Read; Insert receives
the same coordinate pointer/channel and same signed conversion of time.
No padding/unused storage, extra input, new output store, floating reassociation
or precision/rotation math change. Failed reads still go to original cleanup.

Early declarations place the existing float vec[3] before the time Read in any
nonempty subset, retaining its coordinate Read and all operations at their
original sites. No helper/header/ABI, compiler/flag, matcher/denominator,
manufactured effect or signature/body boundary change.

| Family / trial | Enhanced source / strict | Instructions | Calls | Preload-only Mesh |
|---|---|---:|---:|---|
| storage / preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / record_time_first_0 | 97.2 / 37.9 | 642 | 100.0 | no |
| storage / record_time_first_1 | 97.9 / 57.7 | 639 | 100.0 | no |
| storage / record_time_first_2 | 97.9 / 57.7 | 639 | 100.0 | no |
| storage / record_time_first_0_1 | 97.2 / 45.4 | 642 | 100.0 | no |
| storage / record_time_first_0_2 | 97.2 / 37.9 | 642 | 100.0 | no |
| storage / record_time_first_1_2 | 97.9 / 57.7 | 639 | 100.0 | no |
| storage / record_time_first_0_1_2 | 97.2 / 50.1 | 642 | 100.0 | no |
| storage / record_vector_first_0 | 97.9 / 58.0 | 639 | 100.0 | no |
| storage / record_vector_first_1 | 97.9 / 58.0 | 639 | 100.0 | no |
| storage / record_vector_first_2 | 97.9 / 58.0 | 639 | 100.0 | no |
| storage / record_vector_first_0_1 | 97.9 / 58.4 | 639 | 100.0 | no |
| storage / record_vector_first_0_2 | 97.9 / 58.4 | 639 | 100.0 | no |
| storage / record_vector_first_1_2 | 97.9 / 58.4 | 639 | 100.0 | no |
| storage / record_vector_first_0_1_2 | 97.8 / 58.7 | 639 | 100.0 | no |
| storage / early_0 | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / early_1 | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / early_2 | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / early_0_1 | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / early_0_2 | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / early_1_2 | 98.3 / 67.8 | 639 | 100.0 | yes |
| storage / early_0_1_2 | 98.3 / 67.8 | 639 | 100.0 | yes |
| array / preload_control | 98.3 / 67.8 | 639 | 100.0 | yes |
| array / time_first_0 | 97.2 / 37.9 | 642 | 100.0 | no |
| array / time_first_1 | 97.9 / 57.7 | 639 | 100.0 | no |
| array / time_first_2 | 97.9 / 57.7 | 639 | 100.0 | no |
| array / time_first_0_1 | 97.2 / 45.4 | 642 | 100.0 | no |
| array / time_first_0_2 | 97.2 / 37.9 | 642 | 100.0 | no |
| array / time_first_1_2 | 97.9 / 57.7 | 639 | 100.0 | no |
| array / time_first_0_1_2 | 97.2 / 50.1 | 642 | 100.0 | no |
| array / vector_first_0 | 97.9 / 58.0 | 639 | 100.0 | no |
| array / vector_first_1 | 97.9 / 58.0 | 639 | 100.0 | no |
| array / vector_first_2 | 97.9 / 58.0 | 639 | 100.0 | no |
| array / vector_first_0_1 | 97.9 / 58.4 | 639 | 100.0 | no |
| array / vector_first_0_2 | 97.9 / 58.4 | 639 | 100.0 | no |
| array / vector_first_1_2 | 97.9 / 58.4 | 639 | 100.0 | no |
| array / vector_first_0_1_2 | 97.8 / 58.7 | 639 | 100.0 | no |

Seven early declarations and both controls equal entire preload-only object,
source 98.3. All record/array variants regress to 97.2..97.9. Every one of the
fourteen array forms equals its corresponding entire record object, rather
than merely tying metrics; this representation substitution adds no gate
gain. All call-count shape values remain 100.0. No trial retained and no general
compiler ceiling inferred.

Both sweeps explicitly exited zero. All thirty-seven compile/focused verify
calls pass; metrics independently recomputed from instruction arrays, object
hashes/counts and source snapshot presence checked. Corresponding record/array
whole hashes and row metrics independently compared. Code/fixtures match
ecc30f25 after restoration. Read-only accepted native/full binding audit passes:
204 TUs, 4619 comparisons, zero skipped, 202 unaffected objects; actual linked
2079 / byte 959 pair sets equal baseline with no lost/gained pairs, canonical
arguments and body/signature boundaries unchanged. No fresh native/full-corpus
runtime proof for rejected variants.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. Main
checkout untouched; no push, PR or merge. Further spelling variants of these
same record/array layouts have no supporting evidence for a gain.

Evidence: `_work/enh_preload_vector_storage_identities.json`,
`_work/enh_preload_vector_array_identities.json`, thirty-seven focused reports/
objects/source snapshots, deterministic generators and typedpair audit.


## Continuation: Enhanced parser records containing the addressed type field

Eighteen focused Enhanced measurements start from cc3e1d11: eight local parser
record layouts with accepted and preloaded V5 dispatch, plus both controls.
Earlier parser records combined only non-addressed source fields and compiled
to identical objects; this family also contains the short type field passed
by address to the original Read calls. It tests memory coupling rather than
another spelling of the earlier scalar-replaced records.

Each record contains the by-value unsigned file handle and signed short type,
ordered either way, optionally with the active format byte, animation pointer
or mesh pointer. All fields are used, no artificial padding or unused member.
ReadHeader declaration is inside the same function; no exported ABI/layout,
helper or fixed register. Non-type fields receive the same initial values
before the first Read; type remains uninitialized until its original two-byte
Read. No invented initialization or field effect. Original Transform allocation
still precedes all reads, constructor/destructor/delete calls unchanged.

Read argument sizes/order and pointed-to bounds/type/count/key buffers retain
their source operations; file arguments use the copied handle field. Pointer
receivers and masks use their corresponding record members. This relies on
the actual bounded Read contract rather than a claim about arbitrary external
callbacks writing beyond a supplied field. No new runtime proof is asserted
for these rejected forms. All flags, signed counts, key insertions and floating
math remain; preloaded variants still insert zero-channel keys for masks 4/5.
No compiler/flag, matcher/denominator, body signature or boundary change.
These are source hypotheses, not recovered parser records.

| Trial | Enhanced source / strict | Instructions | Calls | Accepted Mesh | Preload-only Mesh |
|---|---|---:|---:|---|---|
| accepted_control | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| preload_control | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| file_type_accepted | 95.6 / 52.9 | 646 | 100.0 | no | no |
| file_type_preload | 95.6 / 53.6 | 644 | 100.0 | no | no |
| type_file_accepted | 95.7 / 48.7 | 642 | 100.0 | no | no |
| type_file_preload | 95.7 / 62.4 | 640 | 100.0 | no | no |
| file_type_format_accepted | 95.5 / 30.4 | 647 | 100.0 | no | no |
| file_type_format_preload | 95.5 / 43.9 | 645 | 100.0 | no | no |
| type_format_file_accepted | 95.9 / 51.3 | 634 | 100.0 | no | no |
| type_format_file_preload | 95.8 / 65.5 | 632 | 100.0 | no | no |
| anim_file_type_accepted | 94.0 / 30.4 | 649 | 100.0 | no | no |
| anim_file_type_preload | 94.0 / 31.0 | 647 | 100.0 | no | no |
| file_type_anim_accepted | 93.9 / 30.3 | 650 | 100.0 | no | no |
| file_type_anim_preload | 93.9 / 31.0 | 648 | 100.0 | no | no |
| self_file_type_accepted | 95.7 / 40.6 | 650 | 100.0 | no | no |
| self_file_type_preload | 95.6 / 41.1 | 648 | 100.0 | no | no |
| file_type_self_accepted | 95.7 / 40.4 | 651 | 100.0 | no | no |
| file_type_self_preload | 95.6 / 41.2 | 649 | 100.0 | no | no |

Both controls reproduce their whole reference objects. All sixteen addressed
record variants change the object and regress source to 93.9..95.9; call-count
shape remains 100.0. Lower instruction count in one layout does not compensate
for other changed source signals. No gate gain; all trial code restored. These
results reject the tested addressed-record coupling as a useful route, without
establishing a general compiler or source-shape ceiling.

Sweep explicitly exited zero; all eighteen compile/focused verify calls pass.
Source/call metrics independently recomputed from instruction arrays; object
hashes/counts and all source snapshot paths checked. Code/fixtures match
ecc30f25 after restoration. Read-only accepted native/full binding audit passes:
204 TUs, 4619 comparisons, zero skipped, 202 unaffected objects; actual linked
2079 / byte 959 pair sets equal baseline with no lost/gained pairs, canonical
arguments and outside-body/signature boundaries unchanged. No fresh native/
full-corpus runtime proof for rejected variants.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. Main
checkout untouched; no push, PR or merge.

Evidence: `_work/enh_addressed_parser_state_identities.json`, eighteen focused
reports/objects/source snapshots, `_work/prepare_addressed_parser_state.py`,
generated sweep and rechecked typedpair audit.


## Continuation: Enhanced axis enums with accepted and preloaded V5 dispatch

Forty-four focused Enhanced measurements start from 005f3622: three local Axis
enum representations, all seven nonempty position/scale/rotation axis-loop
subsets, each with accepted and preloaded V5 dispatch, plus both controls.
Prior numeric type/exit-loop tests did not include this enum family.

Local enum forms are unscoped with implementation-selected underlying type,
scoped unsigned-char and scoped int. All define X=0, Y=1, Z=2 and End=3, all
actively used. Each selected loop begins at X, tests !=End and increments via
unsigned-int addition and conversion back to Axis. Its switch uses the same
ordinal cases X/Y/Z, same channel/key operations and same order. The loop
visits exactly 0,1,2 and stops at 3; casts stay in that range without overflow
or an out-of-range enum value. No extra field/padding, fixed register, exported
ABI/header/helper, source signature or compiler/flag/matcher change.

Bounds/count/key reads, signed counts, failed-read exits, timer updates and
floating math stay in their original places. Vector sections and optional/V5
key loops are untouched except previously measured V5 mask preload in preload
forms. Preload retains zero-channel insertion for cases 4/5. This is a source
hypothesis, not a recovered original enum declaration.

| Trial | Enhanced source / strict | Instructions | Calls | Accepted Mesh | Preload-only Mesh |
|---|---|---:|---:|---|---|
| accepted_control | 98.3 / 66.9 | 641 | 100.0 | yes | no |
| preload_control | 98.3 / 67.8 | 639 | 100.0 | no | yes |
| unscoped_0_accepted | 98.0 / 66.6 | 640 | 100.0 | no | no |
| unscoped_0_preload | 98.0 / 67.5 | 638 | 100.0 | no | no |
| unscoped_1_accepted | 98.0 / 66.6 | 640 | 100.0 | no | no |
| unscoped_1_preload | 98.0 / 67.5 | 638 | 100.0 | no | no |
| unscoped_2_accepted | 98.0 / 66.6 | 640 | 100.0 | no | no |
| unscoped_2_preload | 98.0 / 67.5 | 638 | 100.0 | no | no |
| unscoped_0_1_accepted | 97.7 / 66.4 | 639 | 100.0 | no | no |
| unscoped_0_1_preload | 97.6 / 67.2 | 637 | 100.0 | no | no |
| unscoped_0_2_accepted | 97.7 / 66.4 | 639 | 100.0 | no | no |
| unscoped_0_2_preload | 97.6 / 67.2 | 637 | 100.0 | no | no |
| unscoped_1_2_accepted | 97.7 / 66.4 | 639 | 100.0 | no | no |
| unscoped_1_2_preload | 97.6 / 67.2 | 637 | 100.0 | no | no |
| unscoped_0_1_2_accepted | 97.3 / 66.1 | 638 | 100.0 | no | no |
| unscoped_0_1_2_preload | 97.2 / 67.0 | 636 | 100.0 | no | no |
| byte_0_accepted | 98.0 / 66.7 | 641 | 100.0 | no | no |
| byte_0_preload | 98.0 / 67.6 | 639 | 100.0 | no | no |
| byte_1_accepted | 98.0 / 66.7 | 641 | 100.0 | no | no |
| byte_1_preload | 98.0 / 67.6 | 639 | 100.0 | no | no |
| byte_2_accepted | 98.0 / 66.7 | 641 | 100.0 | no | no |
| byte_2_preload | 98.0 / 67.6 | 639 | 100.0 | no | no |
| byte_0_1_accepted | 97.8 / 66.6 | 641 | 100.0 | no | no |
| byte_0_1_preload | 97.8 / 67.5 | 639 | 100.0 | no | no |
| byte_0_2_accepted | 97.8 / 66.6 | 641 | 100.0 | no | no |
| byte_0_2_preload | 97.8 / 67.5 | 639 | 100.0 | no | no |
| byte_1_2_accepted | 97.8 / 66.6 | 641 | 100.0 | no | no |
| byte_1_2_preload | 97.8 / 67.5 | 639 | 100.0 | no | no |
| byte_0_1_2_accepted | 97.6 / 66.4 | 641 | 100.0 | no | no |
| byte_0_1_2_preload | 97.6 / 67.3 | 639 | 100.0 | no | no |
| int_0_accepted | 98.0 / 66.6 | 640 | 100.0 | no | no |
| int_0_preload | 98.0 / 67.5 | 638 | 100.0 | no | no |
| int_1_accepted | 98.0 / 66.6 | 640 | 100.0 | no | no |
| int_1_preload | 98.0 / 67.5 | 638 | 100.0 | no | no |
| int_2_accepted | 98.0 / 66.6 | 640 | 100.0 | no | no |
| int_2_preload | 98.0 / 67.5 | 638 | 100.0 | no | no |
| int_0_1_accepted | 97.7 / 66.4 | 639 | 100.0 | no | no |
| int_0_1_preload | 97.6 / 67.2 | 637 | 100.0 | no | no |
| int_0_2_accepted | 97.7 / 66.4 | 639 | 100.0 | no | no |
| int_0_2_preload | 97.6 / 67.2 | 637 | 100.0 | no | no |
| int_1_2_accepted | 97.7 / 66.4 | 639 | 100.0 | no | no |
| int_1_2_preload | 97.6 / 67.2 | 637 | 100.0 | no | no |
| int_0_1_2_accepted | 97.3 / 66.1 | 638 | 100.0 | no | no |
| int_0_1_2_preload | 97.2 / 67.0 | 636 | 100.0 | no | no |

Only both controls reproduce reference whole objects. All forty-two enum
variants regress to 97.2..98.0; calls stay 100.0. Fourteen unscoped/scoped-int
pairs produce identical entire Mesh objects and row metrics. Scoped byte also
regresses. All-axis wide enum with preload reaches original 636 instructions
but source is 97.2: instruction count alone does not meet either gate. No trial
retained; no general compiler/source-shape ceiling inferred.

Sweep explicitly exited zero; all forty-four compile/focused verify calls pass.
Source/call metrics recomputed independently from instruction arrays; whole
object hashes/counts and all source snapshot paths checked. Fourteen enum
representation pairs independently compared. Code/fixtures match ecc30f25
after restoration. Read-only accepted native/full binding audit passes: 204
TUs, 4619 comparisons, zero skipped, 202 unaffected objects; actual linked
2079 / byte 959 pair sets equal baseline with no lost/gained pairs, canonical
arguments and body/signature boundaries unchanged. No fresh native/full-corpus
runtime proof for rejected forms.

Accepted means **98.40% / 98.41%**; both >=99.0 gates unmet, goal active. Main
checkout untouched; no push, PR or merge. The tested enum substitutions are
not a useful route to retaining a gate gain.

Evidence: `_work/enh_enum_axis_identities.json`, forty-four focused reports/
objects/source snapshots, `_work/prepare_enum_axis.py`, generated sweep and
rechecked typedpair audit.
