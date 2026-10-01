# Native Level mission generation: 2026-10-01 checkpoint

The four requested bodies have been reconstructed from native evidence. The
measured mean ARM source-shape is **98.8%**. The requested 99% threshold has
**not** been reached, and this report does **not** claim a proven lower ceiling.
This is a validated restoration checkpoint, not a declaration of task completion
or native runtime correctness.

## Baseline and evidence

- Candidate branch: `codex/level-mission-generation-20261001` in the isolated
  `_work/level-mission-generation-20261001` worktree.
- Baseline: `cee16c7b2ea91540c8c7a786427d06511ebe5f36`, with a fresh full ARM
  build and comparison in `_work/level-mission-baseline-20261001`.
- Primary artifact: Android 2.0.16 `libgof2hdaa.so`, SHA-256
  `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
- Primary evidence inspected: Android symbol map, IDA method bodies, ARM
  instructions, and literal data read from ELF load segments. Relevant method
  entries are `0xaffd8`, `0xb4bb8`, `0xbe6c0`, and `0xc0de4`.
- Independent evidence: iOS `sub_10A7A8` (ordinary missions), `sub_111780`
  (campaign), `sub_12283C` (static objects), and `sub_1265F0` (radio). iOS
  supports control and constructor interpretations; its differing constants are
  not substituted for Android data.
- The workspace's `analysis/gof2_libgof2hdaa_full_ida.c` and
  `analysis/gof2_ios_ida_full_dump.c` were inspected. Verifier source and the
  affected Level, Route, KIPlayer, PlayerFixedObject, PlayerJunk, math, mission,
  and collision declarations were checked. DeepOpen was not used as binary
  evidence.

## Changes

`src/game/world/Level.cpp` restores campaign dispatch, non-empty ordinary
missions, ambient populations, special missions, routes, docking targets,
objectives, formations, radio sequences, and the source-known static object
branches. Coordinates and placement tables come from Android literal data;
they are not gameplay tuning estimates. RNG bounds, repeated calls, call order,
and array lengths were checked against ARM instructions. Original side effects,
including the initial geometry wrapper and the unused Player allocation for
type 19050, are retained.

`src/game/world/Level.h` exposes the radio array return, preserves the static
object address through `intptr_t`, and aliases the existing `+0x18c` position
storage without enlarging Level. Return type changes preserve the ARM symbol
names.

Necessary creation dependencies were repaired:

- `Route.h`: restore the real `+0x08` gap and array pointers at
  `+0x0c/+0x10/+0x14`; assert ARM size `0x18`.
- `KIPlayer.h` and `PlayerFixedObject.h`: reuse existing core storage instead
  of duplicating inherited fields; restore PlayerFixedObject's ARM size `0x1bc`
  and verified extension offsets. Assertions apply to 32-bit builds.
- `PlayerFixedObject.cpp`: replace null loot-table stand-ins with Android data,
  restore bounded RNG and loot arguments, and obtain position from the geometry.
- `PlayerJunk.h/.cpp`: restore the elapsed field at `+0x124`, allocation size
  `0x128`, and constructor exclusion flag.
- `createShip()`: use actual Globals references and the options difficulty
  value; pass the original activity flag; allocate typed objects; restore 24
  freighter collision boxes and wreck IDs.
- `getBoundingVolume()`: use the typed collision array without truncating its
  pointer and restore release/delete ownership. ARM confirms the coordinate
  remap and coefficients: AABB half-dimensions scale by 1.2 and are doubled;
  sphere radius scales by 0.6 and becomes non-negative.

Two adjacent fighter flags are written as bytes, preserving the original
two-byte effect without an unaligned C++ `uint16_t*` dereference. Unknown
extension fields are described by offsets rather than invented meanings.

## Measurements

These are the existing verifier's metrics, not estimates of source-code
equivalence. Four unique bodies are counted once each.

| Body | Baseline source-shape | Checkpoint source-shape | Checkpoint fuzzy |
| --- | ---: | ---: | ---: |
| `createCampaignMission()` | 1.5% | 98.6% | 71.6% |
| `createMission()` | 3.6% | 98.0% | 37.6% |
| `createRadioMessages(int)` | 22.8% | 100.0% | 99.9% |
| `createStaticObject(Waypoint*, int, bool)` | 1.8% | 98.6% | 40.9% |
| Mean | 7.4% | 98.8% | 62.5% |

None of the four bodies is claimed linked-exact or byte-exact. The source-shape
score compares instruction and opcode inventories, control flow, direct calls,
and body size. It tolerates allocation differences and does not prove gameplay,
ownership, or ABI correctness. Compiler flags, verifier weights, and comparison
rules were not changed. No metric padding, register barriers, or dead calls were
introduced.

LevelScript remains above the preserved source-shape baseline: process
`98.5% -> 98.7%`, constructor `99.6% -> 99.6%`; the mean of the two unique
bodies is `99.05% -> 99.15%`. Constructor C1/C2 aliases are not counted twice.
Process fuzzy similarity changes from `17.4%` to `16.5%` after the genuine
PlayerFixedObject allocation/layout repair; constructor fuzzy remains `88.4%`.

Latest-source full-corpus comparison: 4,619 rows, no skipped comparisons.
Sets of `(unit, symbol)` were compared against the fresh baseline after
normalizing path separators. **No baseline linked-exact or byte-exact pair was
lost**. Linked-exact increased `2,058 -> 2,079` (+21); byte-exact increased
`945 -> 959` (+14). Gains are in PlayerFixedObject (10 linked/8 byte), Route
(8 linked/6 byte), PlayerJunk (1 linked), and Level's Array helpers (2 linked).
Coverage is unchanged: 4,274/4,524 original functions compared, 250 missing
(7 existing signature mismatches and 243 absent), 130 extra symbols.

Local audit artifacts are `checkpoint_focused.json`, `checkpoint_levelscript.json`,
and `checkpoint_full.json` under `_work/mission-generation/`, alongside
`checkpoint_arm.log` and `checkpoint_native.log`. The baseline report is
`../level-mission-baseline-20261001/_work/mission-generation/baseline_full.json`.
These generated artifacts are not included in the source commit.

## Validation and remaining limits

- UCRT64 native `gof2` build: passed; it produces `libgof2.a`.
- ARM matching build: 204 translation units compiled, 0 failed.
- Focused Level and LevelScript comparisons: completed after the last source
  edit. Full-corpus exact sets were compared with zero losses. The staged
  source/report whitespace check passed before committing.
- The 99% objective is still open. A local code-form plateau is not a ceiling
  proof. Remaining call, inventory, and control differences need further
  evidence-based analysis.
- No native executable/gameplay run was performed. Godot was not used to
  validate the native engine, and no Godot files were changed.
- 64-bit native ABI remains unproven. Other legacy pointer-return declarations,
  fighter layout/extension accesses, and runtime shims outside these creation
  paths still require work. PlayerFixedObject's packing is verified for ARM,
  not certified for a future 64-bit port.
- The original special mission 184 references static ID 18830, which has no
  source-known branch in this Android static factory. Unsupported IDs still
  follow the original null-object/unconditional `setLevel()` behavior. This is
  an unresolved runtime risk, not an invented replacement object.
- Existing Matrix defaults outside the explicit mission payloads remain a
  separate issue: the task's matrices explicitly install the Android values.
- Raw binaries, dumps, temporary sources, builds, and JSON comparison reports
  stay local and are excluded from the task commit. No push or delegation was
  performed, and the dirty primary checkouts were not altered.
