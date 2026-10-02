# Gun ARM recovery — 2026-10-02

## Status: partial; the requested target is not achieved

Fixed Android ARMv7 corpus: 26 unique original bodies, 2 constructor/destructor aliases, 2393 original instructions. Source-shape is **99.4846% unweighted / 97.7898% instruction-weighted**. Both must reach 99.0%; the weighted requirement remains unmet. This is not a completion certificate or proof of native runtime correctness.

| Metric | Clean base | This checkpoint |
|---|---:|---:|
| Source-shape, unweighted | 73.5923% | 99.4846% |
| Source-shape, original-instruction-weighted | 26.9159% | 97.7898% |
| Strict instruction shape, unweighted | 66.8038% | 93.4962% |
| Strict instruction shape, weighted | 15.4981% | 70.8297% |
| Linked-exact unique bodies | 14 | 19 |
| Byte-exact unique bodies | 13 | 13 |

Baseline manifest: `validation/gun_fixed_bodies_2026-10-02.json`. The manifest fixes body membership and original instruction weights before source changes. `tools/gun_family_metrics.py` checks that membership and weights remain unchanged and computes both means. The common verifier and its compiler flags are unchanged.

## Isolation and evidence

- Worktree: `A:/gof2-gun-arm-99`; branch: `codex/gun-arm-99-20261002`.
- Base: `f60694dd0ca578831cec832e7026b32644642d97`, the latest completed integration found when starting. The dirty main checkout was not modified.
- Primary binary: Android 2.0.16 ARMv7 `libgof2hdaa.so`; SHA256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
- Primary readings: targeted bodies in `gof2_recovered_sdk_first_pass/extracted/combat_weapons_damage/Gun.c`, targeted passages in `analysis/gof2_libgof2hdaa_full_ida.c`, original ARM disassembly and literal/rodata reads, existing `Gun.cpp` / `Gun.h`, and declarations of directly called engine/game types. The extracted pseudocode was cross-checked against ARM for disputed contracts.
- Read-only context: project context index/map/status/queue, reverse-engineering context pack, native project validation instructions and selective-transfer findings.
- No iOS/PC implementation was promoted to Android evidence. No Godot changes or Godot validation were used.
- Source changes are restricted to `src/game/weapons/Gun.cpp` and `Gun.h`; other added files are Gun-specific validation/report artifacts. PlayerTurret, TargetFollowCamera, StarMap, shared verifier and build configuration were not edited.

Toolchain: NDK r18b clang 7.0.2, canonical `tools/verify/match_flags.sh`, `-Oz`, Thumb ARMv7, softfp NEON, API 21, strong stack protector. Local NDK fallback was selected through environment variables. No alternative flags or `GOF2_MATCH` define were added to raise scores.

## Recovered contracts and accepted source forms

| Area | Evidence / final behavior | Measured decision |
|---|---|---|
| Constructor and four inline arrays | Android C1/C2 body `0x151f20`; inline `Array<Vector>` offsets 0x08/0x14/0x20/0x2c; wobble pointers at 0xac. ARM size/offset assertions added. | Real array members plus legacy aliases raised constructor source-shape from 65.4% to 93.9%; corrected initialization raised it to 98.3%. |
| Initial velocity and parking | Literal `0x3e1d1245` at `0x152124` scales input velocity independently of projectile speed. Initial lifetime -1000000; initial x=50000; active/hitSmall and adjacent flags initialized at their actual slots. | Accepted because raw ARM proved the operands and field destinations, not because a guessed multiplier improved a score. |
| Ownership | Destructor body `0x152134`; setEnemy replacement at `0x1523c2`. Wobble Vector objects and inline arrays are released; enemy Player objects are not owned. removeAllEnemies detaches. | D1 remains linked-exact; setEnemy becomes linked-exact. |
| Animation resources and mounts | Android rodata at `0x202b04` and `0x202ea8..0x203100`; actual Item/Canvas/Transform APIs. | setIndex linked-exact; integer setOffset 99.0% source-shape. No guessed ship-coordinate table. |
| Firing | shootAt `0x152428`; actual bool return, equipment/ammo gate, mine scatter, homing, mount mirrors, slot thresholds and sentry capacity. | Correct reloads, Vector return/copy forms, scalar-first multiplication and common ammo tail raised shootAt to 98.0%; shoot wrapper becomes linked-exact. |
| Launch position versus velocity | Native launch vector receives rotated input velocity; mount offset affects position. | Removed the incorrect position addition. Validated against ARM call operands. |
| Ignite | Radial damage/EMP, falloff, shock push, nuclear cloud/status effects and achievement progress; member vectors retain native intermediate updates. | Correct Vector temporaries/reloads, bool field, unordered-aware falloff and preparation of numerator/denominator before mine selection raised ignite to 97.2%. |
| Collision | Player/KI field predicates, outerCollide, strict axis bounds, mine attraction, particle/EMP/direct damage, native manual particle arguments and scatter damage. | Branch structure, reloads, ordered float tests, flags, scatter conversion and staged axis calculations raise collision to 96.6%; still the largest weighted deficit. |
| Update | `0x153754`; proper integer Transform::Update argument, rocket grace interval, mine fade, scaled Vector temporaries, expiry/medal handling. | Correct call and temporary contracts raise update to 97.5%. |
| Level collision | Android empty body `0x153750`. | Kept the actual stub; no speculative collision behavior added. |
| Translate | Android `0x153a50`; positions pointer is reloaded each iteration after the externally defined Vector addition. | Removing the cached pointer makes translate linked-exact. |

Follow-up evidence: collision ARM `0x1531dc..0x153254` uses PL/LE rejection and combines the final predicates without short-circuiting. Negating strict ordered comparisons reproduces its rejection of NaN; `>=`/`<=` did not. Ignite `0x152cc0` uses BGT to skip falloff, so unordered input follows the falloff branch. The mine movement sequence has two multiply calls and one final addition, without an intervening assignment call. Scatter scaling merges through a float result before converting back to int, including the 3x branch. `friendGun` is assigned through a bool setter and forwarded directly from its byte slot to native bool call parameters; declaring the slot bool removes artificial normalization while preserving its ARM size/offset.

Additional ARM contracts: the shared ammo tail contains one Item::changeAmount call, not two (`Gun.c` LABEL_78 and original relocations). The no-target homing path uses the scalar-first Vector multiplication overload. `0x15297a..0x152988` samples fireIndex before writing lifetime, while `0x1529d4..0x1529e6` reloads enemies/fireIndex before the second target-position call. Collision `0x15351c..0x1535aa` stages mine Y/Z calculations after preceding successful bounds; the common box computes its negative bound only after the X upper-bound test. Direct damage reloads Level/damage after external docking calls. `level` is now a real Level pointer, preserving its proven ARM offset 0x38 and the exact setter while removing host pointer truncation. The host smoke now checks pointer preservation as well.

Rejected follow-up forms: expanded weapon classification and an unsigned rejectBox mask did not improve source-shape; both were removed. Naming the scatter distance as an int reproduces the original single conversion but emitted identical instructions. These distinctions are recorded rather than treating every source edit as a score improvement.

Typed-access follow-up: Gun methods now access the active position/direction/up/hit Array<Vector> members directly. The Vector stored at 0x90 has a typed `homingDirection` member and an ARM offset assertion; its legacy integer aliases remain for BeamGun's existing address-based access. The semantic name is inferred from use, while the Vector type and offset are proven by the native assignment calls. Weapon-range subtractions now convert to unsigned before subtraction, matching ARM wraparound without signed-overflow undefined behavior. These changes leave the measured ARM body scores unchanged and are not credited as a percentage gain.

Rejected word-flag experiments: combining radial/particle predicates with bitwise OR changed compiler output but reduced collision source-shape (95.9%, 95.6%, 96.1% versus retained 96.6%); none was retained. A copied-back source initially preserved its earlier timestamp and caused an incremental build to reuse a rejected object. That diagnostic was discarded; source timestamps were refreshed and ARM/native checks rerun before recording this checkpoint. The final object is newer than the final source/header. Host checks use the active typed array/Vector members rather than reading inactive legacy aliases.

Rejected experiment: declarations suppressing template emission did not improve the constructor under the canonical flags. They were removed. No volatile access, assembly padding, injected register moves or shared metric normalization was introduced. Source-shape still depends on compiler output and is not equivalent to byte identity.

Camera-copy follow-up: inspected `Camera.h`, `Matrix.h` and `PaintCanvas::CameraGetLocal`. The camera contains an actual Matrix at offset 0x0c; the fallback is also a Matrix object. Render and collision now copy-initialize local Matrix objects from that returned address instead of treating a byte buffer as a live Matrix. The implicit copy constructor preserves the original 60-byte copy without calling the default constructor. Both ARM body results and all baseline exact pairs remain unchanged. In shootAt, a reference to targetDir is formed before the non-homing mount loop, matching the native hoisted member address. Source-shape remains 98.0%; strict shootAt shape improves from 62.6% to 62.9%.

Rejected follow-ups: a positive Y/Z bounds block with a collision label and signed collision loop indices emitted identical results. A switch-based radial classifier reduced collision source-shape to 96.2% and was removed.

## Checks and limits

- Native CMake `gof2` static-library build: passed.
- Canonical full ARM build: **204 translation units, 0 failures**. Gun ARM layout assertions passed.
- Focused verifier: all 28 symbols compared; 19 linked-exact / 13 byte-exact. The original baseline Gun exact pairs are retained.
- `python tools/gun_contract_smoke.py`: passed construction/destruction with 0/1/3 projectile slots, array dimensions, initial values, owned wobble pointers, both offset overloads, replacement enemy-container ownership, detach behavior and full-width Level pointer preservation.
- The host smoke compiles actual selected source bodies against the real header; it is deliberately isolated from the unrelated host executable link backlog. It does not execute firing, damage, collision, renderer or an ARM native game executable.
- `git diff --check`: passed.
- Full corpus: **4625 comparisons**, 4274/4524 original functions, 250 missing (7 wrong-type, 243 absent), 130 extras, 2088 linked-exact and 959 byte-exact. Baseline linked/byte exact-pair losses across the entire corpus: **zero / zero**. Six additional comparison rows arise from template emission; original coverage and the fixed Gun denominator did not change.
- Snapshot: `validation/gun_family_2026-10-02.json` contains all 28 Gun rows, baseline rows, unique-body/alias labels, both means, full-corpus totals and exact-pair loss sets.

Clean-base and checkpoint full corpus both fail `--fail-on-wrong-type` (exit 1): the identical seven C2/D2 template aliases are classified as wrong-type (KeyCode pointers, nested ImagePart arrays, BluePrint pointers, PendingProduct destructor and bool-array destructor). Baseline: 4619 comparisons, 4274/4524 original functions, 250 missing (7 wrong-type, 243 absent), 130 extras, 2079 linked-exact, 959 byte-exact. The missing/extra export report alone initially suggested that only C1/D1 were emitted. A direct GNU ARM nm read of ArrayInstantiations.o disproved that interpretation: all seven expected C2/D2 symbols are defined weak function aliases alongside C1/D1. The shared disassembly parser only credits aliases in the exact .text section; these template bodies live in .text.<symbol> sections. Its address-only alias lookup also lacks section identity. Therefore these seven gate diagnostics do not prove missing native ABI exports.

The user authorized a separate native ABI-alias repair. Because the exports already exist, no native Array changes are justified. A further decision about the originally excluded shared parser is pending. An isolated parser prototype uses (section, address) identity and accepts .text and .text.<symbol> function sections. It recognizes all seven cases in the actual object, preserves every previously parsed body, and passes a regression fixture with two distinct sections at address zero. See validation/gun_abi_alias_audit_2026-10-02.json. Full prototype corpus validation is running separately; it is not yet the authoritative gate result. No shared verifier file or comparison metric has been changed.

C1/C2 share original address `0x151f20`; D1/D2 share `0x152134`. Only C1 and D1 count in the unique-body means. D2's separately emitted local result is kept as a diagnostic row, not deleted or counted as a new original body. Its lowered local score is disclosed even though D1 remains linked-exact.

Remaining risks: no ARM executable/runtime test; inferred semantic names for some raw fields; legacy union aliases are a compatibility compromise and need further native integration review; invalid offset indices retain native unchecked behavior; detaching enemies without another owner can leak the container, as in the original. Largest remaining source-shape deficits are collision, shootAt, ignite and update. No 99/99 completion claim, push, PR or merge is authorized by this checkpoint.
