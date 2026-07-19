export const meta = {
  name: 'gof2-fix-wave',
  description: 'One race-free wave: fix in-scope verify gaps (absent/shim/template/extra/wrong_type) — one agent per file-disjoint component, each Ghidra-verified and build-gated',
  phases: [{ title: 'fix', detail: 'one agent per component subbatch, all file-disjoint' }],
}

// args = { jobs: [ { label, kind, files:[...], entries:[ {symbol,demangled,ghidra_addr,kind,
//          files,ours?,paired_extra?,paired_absent?} ] } ] }
const REPO = '/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp'
const DEEPOPEN = '/Users/fionera/Downloads/GalaxyOnFire2/DeepOpen'

const RESULT_SCHEMA = {
  type: 'object',
  additionalProperties: false,
  properties: {
    fixed: { type: 'array', items: { type: 'string' },
             description: 'mangled symbols now implemented/corrected under the exact original name' },
    deferred: { type: 'array', items: {
        type: 'object', additionalProperties: false,
        properties: { symbol: { type: 'string' }, reason: { type: 'string' } },
        required: ['symbol', 'reason'] } },
    caller_rewrites: { type: 'array', items: {
        type: 'object', additionalProperties: false,
        properties: { file: { type: 'string' }, find: { type: 'string' }, replace: { type: 'string' },
                      why: { type: 'string' } },
        required: ['file', 'find', 'replace'] },
        description: 'edits needed in files OUTSIDE this component (callers); master applies serially' },
    missing: { type: 'array', items: { type: 'string' },
        description: 'MISSING FIELD/DECL lines for classes whose header is not in this component' },
    notes: { type: 'string' },
  },
  required: ['fixed', 'deferred', 'caller_rewrites', 'missing', 'notes'],
}

const RULES = `
GOAL: clean, idiomatic, human-looking C++. The work is EXACTLY three operations: (1) ADD an absent
function (implement the missing original as a real method/free-function/ctor/dtor on the right
class); (2) REMOVE an extra (rename a misnamed function to the original, retype a wrong overload, or
— if the original inlined it — inline its body into the callers and delete it; never delete a called
function); (3) CORRECT a wrong_type (retype params so the signature matches the original). We do NOT
care about byte-for-byte matching or exact symbol/alias matching. NEVER write a byte-match hack.

ABSOLUTELY FORBIDDEN (these are the mistakes to undo, never introduce):
- asm link-name overrides: NO  asm("_ZN...")  labels on a function/variable to force a mangled name.
- ctor/dtor written as a free function taking an explicit \`self\`/\`this\` pointer (e.g.
  \`void Foo_base_ctor(Foo* self, ...) asm("..C2..")\`, \`Foo_ctor\`, \`Foo_dtor\`). Write a REAL C++
  constructor \`Foo::Foo(args){...}\` / destructor \`Foo::~Foo(){...}\` instead — it naturally mangles
  correctly. It is FINE that a real ctor/dtor also emits the C1/D1 alias variants; do not try to
  suppress them.
- __attribute__((alias(...))), [[gnu::used]] added only to keep a hack alive, .set/.global asm.
- *** FORWARD-DECLARATIONS and LOCAL STUB CLASSES to dodge a header — THE #1 DEFECT. *** To use a
  type, \`#include\` its OWNING HEADER and reference the real \`Class\` / \`Class::member\`. NEVER write
  \`class X;\` / \`namespace AbyssEngine { class Trail; }\`, NEVER a local \`struct Layout {...}\` /
  \`class LensFlare {...}\` mirror, NEVER an \`extern T x asm("_ZN...")\`. A stub/forward-decl gives the
  type the WRONG identity (your \`AbyssEngine::Trail\` is NOT the real global \`::Trail\`), and breaks
  other TUs with incomplete-type / redefinition / using-conflict errors. If the owning header
  genuinely conflicts with another you need (duplicate-String / two-PaintCanvas / unguarded
  BlendMode), FIX the conflict (add the header guard / relocate the namespace) so the real header is
  includable; if that fix is outside your component's files, DEFER with the precise reason. Stubbing
  is NEVER acceptable.

GROUND TRUTH is the original binary, for UNDERSTANDING BEHAVIOR ONLY. Use Ghidra MCP when available
(program "android_2.0.16_libgof2hdaa.so", image base 0x10000; ghidra_addr is already V+0x10000 —
mcp__ghidra__decompile_function / mcp__ghidra__disassemble_function) plus the provided orig_asm to
work out what the function DOES (its logic, params, return, field accesses), then express that as
clean C++. Do NOT reverse-engineer instruction sequences to reproduce them. The mangled symbol tells
you the correct SIGNATURE — write a normal C++ declaration with that signature (real constructor,
real method on the class, real overload); confirm it mangles right with orbnm, but never force it
with asm. If a function is too complex to recover confidently, DEFER it.

NAMING ORACLE (inspiration only, NOT authoritative): ${DEEPOPEN} (Java decompile of an older version). grep it for the class/method to recover intent, method names and field names. The Android binary always wins on signatures/layout/names when they disagree.

READABILITY / CORRECTNESS RULES (a violation is a SHORTCUT — do not commit shortcuts):
- Real C++ classes. Model polymorphism as REAL inheritance: if the original has a vtable, the base declares virtual methods and the derived uses 'override'; emit virtual dtors (D0/D1/D2) as the binary does. Never fake a virtual as a nullary static.
- Use REAL types and REAL #includes of the owning header — never void* or a forward-decl where the concrete type is known and available.
- If Ghidra shows a member call with NO receiver, read the asm to recover the real 'this' (often a member var) — never invent a nullary static to paper over it.
- C++14. NO C-style casts (use static_cast/reinterpret_cast). No 'Enum{int}'. Semantic field/param names (consult fieldmap/rename via DeepOpen + existing headers), never field_0xNN if a name is known.
- Match the surrounding file's style, comment density and idiom.

KIND-SPECIFIC:
- absent: implement the function (declare in header, define in cpp) from the decompiled body.
- shim: a free-function emulation shim (e.g. Globals_getLine / Player_ctor_cs) that should be the original's real method/namespaced function. Convert it so it mangles to the original (methodize onto the class, or move into the original namespace). Update call sites that are IN YOUR component's files; call sites in OTHER files -> report as caller_rewrites. If a paired_absent/paired_extra is given, they are the same function — fix once under the correct name.
- template: NO ACTION. The generic Array<T> container — both its member functions (resize/clear/push_back/...) and the free helpers (ArrayAdd/Remove/Release/ReleaseClasses/SetLength/RemoveAll/Set/AddCached<T>) — is defined once, always-visible, in src/engine/core/Array.h. The compiler instantiates whatever a type needs wherever it's used (or inlines it at -Oz). NEVER hand-write per-type "template void ArrayX<T>(...)" instantiation lines or per-TU copies — that is byte-match clutter, explicitly excluded from the goal. If you are ever handed a template entry, treat it as already handled (defer/no-op).
- extra: our build defines this but the original has NO such symbol. Decompile to determine which holds, then fix: (a) MISNAMED -> rename ours to the original's exact name (update callers; out-of-component callers -> caller_rewrites); (b) WRONG SIGNATURE -> retype; (c) INLINED-ONLY (original inlined it everywhere, no standalone symbol) -> inline our body into its callers and delete the standalone (out-of-component callers -> caller_rewrites). Never delete a called function without inlining.
  HARD STOP for 'extra' (and any kind): if the correct fix requires changing the TYPE SYSTEM across files you don't own — renaming/adding/removing a base-class virtual, flipping a destructor's virtualness, changing a signature that derived classes 'override', or editing any .h outside your component — DO NOT attempt it and DO NOT emit it as a caller_rewrite (those are refused and would break every derived TU if applied piecemeal). Instead DEFER the entry with a precise reason describing the coordinated cross-hierarchy change needed (which base + which derived classes + which call sites). caller_rewrites are ONLY for self-contained call-site argument changes in .cpp files (e.g. f(x) -> f(x, 0)), never header or virtual/override edits.
- wrong_type: we implement it under a different signature. Retype our params/qualifiers (and, for C1/C2/D0/D1/D2 'same demangled, different mangled' cases, recover the polymorphic vtable / emit the missing ctor/dtor variant) so it mangles to the original.

BUILD-GATE (MANDATORY) — compile EVERY .cpp you edited, in BASH (the login shell is zsh which will NOT word-split the flags; you MUST use 'bash -c'. Write the .o UNDER THE REPO, never /tmp — /tmp is inside the OrbStack VM):
  cd ${REPO} && bash -c '. tools/verify/match_flags.sh; tools/verify/orbcc $GOF2_MATCH_CXXFLAGS -c src/<file>.cpp -o ./_chk_<n>.o; echo EXIT $?'
Every compile MUST print EXIT 0. Then confirm each symbol exists under its correct mangled name from
NORMAL C++ (not an asm override): tools/verify/orbnm ./_chk_<n>.o | grep <mangled-or-substring>.
Remove the _chk_*.o when done. (Do NOT chase asm/byte equality — compiling cleanly + the right
symbol from idiomatic C++ is the bar.)

FILE DISCIPLINE: edit ONLY the files listed for your component. Anything needed in another component's file -> caller_rewrites (callers) or missing (fields/decls). Do NOT touch other files directly.

If you genuinely cannot finish an entry this pass WITHOUT a shortcut, leave it unimplemented and put it in 'deferred' with a precise reason. Never fake it.`

function promptFor(job) {
  const lines = job.entries.map((e, i) => {
    let s = `  ${i + 1}. ${e.kind}  symbol=${e.symbol}\n     demangled: ${e.demangled}`
    if (e.ghidra_addr) s += `\n     ghidra_addr: ${e.ghidra_addr}`
    if (e.ours && e.ours.length) s += `\n     our current overload(s): ${e.ours.join(' | ')}`
    if (e.paired_extra) s += `\n     paired extra (same fn, our shim name): ${e.paired_extra}`
    if (e.paired_absent) s += `\n     paired absent (the original this shim should become): ${e.paired_absent}`
    if (e.orig_asm) s += `\n     original disassembly (for understanding behavior ONLY — do not reproduce instruction-by-instruction):\n${e.orig_asm.split('\n').map((l) => '       ' + l).join('\n')}`
    return s
  }).join('\n')
  const where = job.files && job.files.length
    ? `Component files you may edit (and ONLY these): ${job.files.join(', ')}`
    : (job.target_file
        ? `These have no existing home. Put ALL of them in this exact file (create it if missing, APPEND if it exists): ${job.target_file}  — plus its header if you add a class. Do NOT invent a different filename.`
        : `These have no existing home — create one well-named .cpp (+ .h) under src/ and put them together.`)
  return `Fix these ${job.entries.length} function(s) in the Galaxy on Fire 2 decompilation so each recompiles to the ORIGINAL Android binary under its EXACT original mangled symbol.

Repo: ${REPO}
${where}

ENTRIES:
${lines}
${RULES}

Return the structured result (fixed / deferred / caller_rewrites / missing / notes).`
}

const EMBEDDED_JOBS = null // __WAVE_JOBS__ (wave_prepare.py replaces this whole line)

phase('fix')
const jobs = EMBEDDED_JOBS || (typeof args !== 'undefined' && args && args.jobs) || []
log(`wave: ${jobs.length} file-disjoint components`)
const results = await parallel(jobs.map((job) => () =>
  agent(promptFor(job), { schema: RESULT_SCHEMA, label: job.label, phase: 'fix' })
    .then((r) => ({ label: job.label, kind: job.kind, ...r }))
))
const ok = results.filter(Boolean)
const fixed = ok.flatMap((r) => r.fixed || [])
const deferred = ok.flatMap((r) => (r.deferred || []).map((d) => ({ ...d, label: r.label })))
const callers = ok.flatMap((r) => (r.caller_rewrites || []).map((c) => ({ ...c, label: r.label })))
const missing = ok.flatMap((r) => r.missing || [])
log(`wave done: fixed ${fixed.length}, deferred ${deferred.length}, caller_rewrites ${callers.length}, missing ${missing.length}`)
return { fixed, deferred, caller_rewrites: callers, missing, per_component: ok }
