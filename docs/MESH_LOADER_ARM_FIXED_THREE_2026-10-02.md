# Mesh loader: fixed three-body ARM audit, 2026-10-02

Latest validated continuation: source **98.23% unweighted / 98.17% weighted**.
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
