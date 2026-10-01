# LevelScript ARM recovery — 2026-10-01

## Status

The focused acceptance threshold is reached in packet 34 and the final corpus
regression audit passes. The requested threshold is **99% average ARM
source-shape across two unique bodies**: `LevelScript::process(int)` and the
constructor. Constructor C1/C2 aliases share address `0x135adc` and count once.
Packet 34 measures **99.05%**: process 98.5%, constructor 99.6%.
No lower ceiling is used as an acceptance substitute.

Neutral baseline: `e473d5454d89404051c9731ac93ad52897699020` (`origin/main`).
Work is isolated on `codex/levelscript-arm-recovery-20261001` in
`../_work/levelscript-arm-recovery-20261001`. The original dirty checkout was
not used as the source baseline and its source files were not changed.

## Evidence and interpretation

- Android ARMv7 2.0.16 `libgof2hdaa.so` is the primary ABI/control-flow source.
  Constructor `0x135adc..0x137ae8`; process `0x137d18..0x145bf0`.
  The workspace full IDA dump and a separate export of actual IDA instruction
  heads were examined together. Rounded decompiler floats were checked against
  literal words, and indirect calls against the original relocation table.
- iOS `sub_130A1C` independently confirms constructor branch IDs, constants,
  object relationships and the final joins. Its member offsets generally
  differ by four bytes from Android and were not copied into Android layout.
- The full iOS IDA dump does not contain a successful `sub_134098` process
  decompilation; the Ghidra dump explicitly records failure/time-out. Raw
  Thumb disassembly of the original Mach-O independently confirms the process
  prefix, message-2 transition, enemy positions and virtual position slots.
  The linear raw export stops at embedded data and is **not** a complete iOS
  process control-flow export.
- DeepOpen `references/DeepOpen/src/GOF2/LevelScript.java` was read as a
  semantic reference only. Its intro coordinates, constructor signature and
  fixed-point formulas differ from HD; they are not Android recovery evidence.

Accepted forms are ordinary member operations and typed calls supported by
the Android instructions. Rejected forms include rounded constants, a second
Matrix construction, a zero programmed-station shim, treating PlayerEgo as its
owned Player, and collapsing three script vectors into one field.

## Baseline definitions

The matching build contains **204 translation units / 204 object files**.
The full baseline build compiled all 204; the corpus skipped zero objects.
Counts below deliberately distinguish comparison rows, symbol names and
original instruction-body addresses.

| Baseline measure | Value |
|---|---:|
| Comparison rows `(unit, mangled symbol)` | 4614 |
| Unique original symbol names in `.text` | 4524 |
| Unique original bodies / addresses | 4162 |
| Compared unique symbol names | 4274 |
| Compared unique original bodies | 3929 |
| Missing exact-name symbols | 250 |
| Classified wrong-signature aliases | 7 |
| Classified genuinely absent symbols | 243 |
| Extra symbol names emitted by the build | 130 |
| Linked-exact comparison rows | 2039 |
| Linked-exact unique symbols / bodies | 1808 / 1720 |
| Byte-exact comparison rows | 932 |
| Byte-exact unique symbols / bodies | 915 / 897 |
| Full-corpus strict / source-shape mean | 75.55% / 88.73% |

The seven existing wrong-signature classifications are Array C2/D2 aliases
whose reported alternative has the same demangled argument list. This is a
verifier classification caveat, not seven newly introduced parameter changes.
`--fail-on-wrong-type` exits 1 on the unchanged baseline for those aliases;
that exit is reported separately from successful native and ARM compilation.
The verifier has not been modified to suppress it.

## Changes and measured packets

- LevelScript layout: byte sequence flags at `+0x20/+0x21`, independent return
  flag at `+0x11`, Matrix at `+0x4c`, previously missing spacer at `+0x88`,
  timers A/B/C at `+0x90/+0x98/+0xa0`, saved auto-turret/turret bytes
  at `+0xaa/+0xab`, HUD at `+0xd0`, particle handles at `+0xe0/+0xe4`.
  The constructor clears `+0x9a..+0xa9` without inventing initialization of
  `+0x98/+0x99`. ARM size stays `0xe8` with compile-time offset checks.
- Real `Level::programmedStation` routing replaces the zero local shim.
- Constructor branches, spawn overrides, camera setup, geometry resources,
  particle systems and station animation states are reconstructed from Android
  and checked against the independent iOS constructor.
- KIPlayer virtual declarations follow all 24 original slots proven by
  `_ZTV8KIPlayer` dynamic relocations. In particular awake is `+0x0c`, render
  is `+0x24`, getPosition is `+0x28`, vector setPosition is `+0x44`, and scalar
  setPosition is `+0x48`. Engine-sound and cargo helpers are nonvirtual.
- The directly needed PlayerEgo speed getter now returns the stored float;
  route storage/getter use `Route*`. Three dependent RadioMessage/Hud casts
  were removed. Mangled method names remain unchanged.
- Process recovery includes intro/title branches 0/1, special cinematic
  branches 78/81/89/106, shared 7001-ms departure, and campaign branches
  14/16/21/24/29/40/41/42/56/64/67/69/70/73/80/91/92/94/102/105/114/125/
  131/135/139/142/144/145/154/157/158. Every known dispatched mission now has
  an implementation. Source-form differences remain measurable below.
- The repeated awake calls in missions 40 and 102 are original calls.
  Mission 158 uses distinct 64-bit timers. Mission 157 preserves weak-device
  indices and the third 64-bit countdown at +0xa0.
- Level particle manager +0x74 is used in missions 41/80. The existing
  particleSystemMgr member at +0x7c is a different pointer. AEGeometry
  getMatrix already returns a reference, and its original pointer receiver
  is preserved without an extra Matrix copy.
- Packet 31 restores the separate Android blocks for mission 154 states
  2/4/6. Packet 33 restores separate mission-94 spawning states 2/3 and
  corrects mission 105 state 10 to the normalized positive light direction
  (0x142cbe..0x142cd4); the previous negative direction was incorrect.
- Packet 32's separate ship calls in mission 92 were rejected after a closer
  source review: Android selects the float speed then calls moveForward once.
  Packet 34 restores that form and float multiplication, and separates the
  original mission-92 state-3/state-4 camera blocks (which have different
  timer/target behavior). Mission 105's separate double-product calls are
  retained because its original has those three individual call sites.
  No artificial calls, volatile barriers, or dead code were added.

| Focused packet | Constructor source / strict | Process source / strict | Mean of two unique source bodies |
|---|---:|---:|---:|
| Baseline | 4.3% / 2.6% | 0.3% / 0.2% | 2.3% |
| 5 | 66.8% / 38.8% | 2.1% / 1.7% | 34.45% |
| 6 | 93.2% / 63.2% | 2.1% / 1.7% | 47.65% |
| 8 | 99.6% / 88.4% | 10.7% / 6.2% | 55.15% |
| 9 | 99.6% / 88.4% | 17.3% / 6.7% | 58.45% |
| 10 | 99.6% / 88.4% | 21.2% / 9.5% | 60.4% |
| 12 | 99.6% / 88.4% | 30.1% / 11.2% | 64.85% |
| 14 | 99.6% / 88.4% | 39.0% / 9.3% | 69.3% |
| 19 | 99.6% / 88.4% | 52.1% / 15.0% | 75.85% |
| 22 | 99.6% / 88.4% | 60.1% / 17.1% | 79.85% |
| 24 | 99.6% / 88.4% | 68.3% / 28.0% | 83.95% |
| 27 | 99.6% / 88.4% | 79.3% / 27.5% | 89.45% |
| 28 | 99.6% / 88.4% | 86.0% / 27.2% | 92.8% |
| 29 | 99.6% / 88.4% | 89.8% / 27.3% | 94.7% |
| 30 | 99.6% / 88.4% | 96.8% / 27.7% | 98.2% |
| 31 | 99.6% / 88.4% | 98.1% / 17.0% | 98.85% |
| 32 | 99.6% / 88.4% | 98.0% / 19.3% | 98.8% |
| 33 | 99.6% / 88.4% | 98.1% / 19.3% | 98.85% |
| 34 | 99.6% / 88.4% | 98.5% / 17.4% | **99.05%** |

Packet 8: 204/204 ARM objects compiled; native UCRT64 `gof2` static-library
target compiled. The LevelScript family retains all prior exact rows, with
seven linked-exact and four byte-exact rows. Alias rows are listed by the
verifier but are not double-counted in the requested two-body mean.

An intermediate **full-corpus** run after the KIPlayer vtable correction
(`vtable_full.json`, before the packet-8 process) gives 4614 comparisons,
2056 linked-exact and 945 byte-exact rows. Automated set subtraction over
`(normalized unit, mangled symbol)` found **zero lost linked-exact and zero
lost byte-exact rows** relative to the detached clean baseline; gains are
17 and 13 rows. Unique exact symbols/bodies are 1825/1736 linked and 928/910
byte.

Packet 14 full corpus has 4615 comparisons, 2057 linked-exact and 945
byte-exact rows. Automated baseline set subtraction again finds zero lost
rows, with 18 linked-exact and 13 byte-exact gains. The additional compared
row is `Array<int>::~Array()` in the LevelScript translation unit. Packet 19
compiled all 204 ARM objects and the native static-library target; its focused
family retains seven linked-exact and four byte-exact rows.

Packet 24 full corpus: 4615 comparisons, linked-exact 2057 and byte-exact
945. Automated exact-set subtraction against baseline reports zero lost
linked-exact and zero lost byte-exact rows, with 18/13 gains. Packet 32 ARM
compiled 204 objects without failures and the native static-library target
compiled. Packet 34 also compiles all 204 ARM objects and the native target.

Final full corpus, using the frozen packet-34 source and objects:

| Final measure | Value |
|---|---:|
| ARM objects compiled / failed; corpus objects skipped | 204 / 0; 0 |
| Comparison rows | 4617 |
| Compared unique original symbols / bodies | 4274 / 3929 |
| Full-corpus strict / source-shape mean | 75.71% / 88.85% |
| Fuzzy-perfect comparison rows | 2178 |
| Linked-exact rows / unique symbols / original bodies | 2058 / 1825 / 1736 |
| Byte-exact rows / unique symbols / original bodies | 945 / 928 / 910 |
| Lost baseline linked-exact / byte-exact rows | **0 / 0** |
| Gained linked-exact / byte-exact rows | 19 / 13 |
| Missing exact-name / wrong-signature / genuinely absent symbols | 250 / 7 / 243 |
| Extra build symbol names | 130 |

The final automated audit compares exact row sets against the clean baseline,
not aggregate totals alone, and also asserts that the complete wrong-signature
classification list is unchanged. All original 4524 symbols / 4162 body
addresses remain the reference corpus. The seven existing classifications
retain the baseline `--fail-on-wrong-type` caveat described above.
Final artifacts are `final_full.json` and `final_exact_regression.json` under
the local evidence directory. Native UCRT64 static-library compilation and
`git diff --check` pass. No native runtime claim follows from these gates.

The task commit includes only LevelScript.cpp/.h, KIPlayer.h,
PlayerEgo.cpp/.h, the three dependent route-cast removals in RadioMessage.cpp
and Hud.cpp, and this report. Source review covered those files and their
directly called Level, geometry, Player and Status interfaces. It excludes
local evidence, binaries, generated objects and unrelated dirty-checkout files.
The commit remains local; no push is performed.

## Meaning and limits of the acceptance metric

The two-body mean uses the existing verifier, unchanged. Source-shape is an
allocation-insensitive combination of instruction inventory, opcode inventory,
control-flow shape, direct call-count agreement and instruction-count agreement.
It is not proof of ordered instruction equality, byte equality or gameplay.
Packet 34 process strict similarity is 17.4%; constructor strict is 88.4%.
The LevelScript family still has seven linked-exact and four byte-exact rows.

Constructor target and current both have 3015 reported instructions. Two target
entries normalized as `.word #x` correspond to actual original call instructions
in the IDA-head export; the current object counts them as calls. This is a
reported disassembly/normalization caveat. It was not hidden by changing the
verifier or by adding source artifacts.

## Reproduction

Use an untouched detached baseline worktree for the full baseline; do not
rebuild or overwrite its objects while its report is running. Matching and
native builds are separate. In the Windows/MSYS environment used here:

```powershell
$env:GOF2_VERIFY_LOCAL_NDK='1'
$env:GOF2_NDK_ROOT='A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b'
$env:NDK=$env:GOF2_NDK_ROOT
$env:GOF2_VERIFY_WORK='A:/Galaxy on Fire new version/gof2hd-decomp-master/_work'
$env:Path='C:/msys64/ucrt64/bin;C:/msys64/usr/bin;'+$env:Path
& C:/msys64/usr/bin/bash.exe tools/verify/build_objs.sh cmake-build-match/verify
python tools/verify/verify.py --build-dir cmake-build-match/verify --no-build --unit game/world/LevelScript --only '^_ZN11LevelScript' --report focused.json
python tools/verify/verify.py --build-dir cmake-build-match/verify --no-build --report full.json
& C:/msys64/ucrt64/bin/cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
& 'C:/Program Files/Git/cmd/git.exe' diff --check
```

Native configuration uses Ninja, Debug, and `C:/msys64/ucrt64/bin/g++.exe`.
The build script can exit zero when some objects fail: inspect its explicit
`compiled 204, failed 0` line before accepting a report.
For exact preservation compare sets, not only aggregate counts:

```python
import json
baseline = json.load(open('baseline_full.json'))
candidate = json.load(open('full.json'))
def exact(report, key):
    return {(r['unit'].replace('\\', '/'), r['symbol'])
            for r in report['functions'] if r[key]}
for key in ('linked_equal', 'bytes_equal'):
    lost = exact(baseline, key) - exact(candidate, key)
    assert not lost, (key, sorted(lost))
```

Local packet evidence is under `_work/levelscript-recovery/`; the clean full
baseline is in `../levelscript-baseline-20261001/_work/levelscript-recovery/`.
These logs, copied IDA databases, original binaries and dump exports are not
task commit contents.

## Remaining limitations

The known campaign dispatcher is present. Remaining source-form differences
remain visible in the strict comparison. Packet 32
is superseded by packet 34, which reaches the focused threshold.
The final full-corpus audit passes without losing baseline exact rows.
`Level::createMission` and `Level::createCampaignMission` have not been expanded.

ARM offset accesses to derived fighter/station payloads are explicit because
the existing C++ derived classes duplicate/misplace payload fields. They do
not establish correct 64-bit native layouts. Several untouched getters still
represent pointers as integers (notably Status campaign mission and Galaxy
station). Existing render/skip helper shims remain unresolved and are not
reported as recovered behavior.

The native static-library compile gate and ARM object/corpus checks do not
prove a working native executable, actual gameplay, or 64-bit ABI correctness.
No Godot test is used as evidence for native recovery.
