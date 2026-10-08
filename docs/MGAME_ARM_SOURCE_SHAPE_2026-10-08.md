# MGame: fixed-family ARM source-shape gate, 2026-10-08

Worktree: `C:/Users/Dima/Documents/Galaxy on Fire new version/gof2hd-98-mgame`.
Branch: `codex/mgame-arm-98-20261008`.
Starting commit: `473310054460d0eef759db67b1a59064a06f2fb1`.

Only implementation file changed: `src/game/menu/MGame.cpp`. No header/layout,
HangarWindow, Radar, common verifier, flags, or PROJECT_STATUS changes. Builds,
reports, source extracts and trial backups are on C: in this worktree; original
binary/NDK/source references are read from A:. No push, PR or merge was performed.

## Result and fixed denominator

The family was registered before edits in
[`validation/mgame_2026-10-08_fixed_family.json`](validation/mgame_2026-10-08_fixed_family.json).
Its raw working-file SHA-256 is `8e5e5c8240e5d18d253bfec67ad9eb43d55e95bbbd0edbbb19bbf930ad61418b`.
Git normalizes CRLF text to LF; the registry's LF-normalized SHA-256 is
`5ffb783c69cef6d1dabef5220346fd50396ce523a9d367da5acfdc054391b932`.
The 45 MGame symbols cover 43 original addresses. Constructor/destructor aliases
are counted once. Eight original callback shells (seven one-token returns and
the two-token ShowLoadingScreen) are excluded; all 35 other unique bodies are
included, including the small deleting destructor and maneuverBegin.
The original `n_target` weights remain fixed at a total of 12,933.

| Measure | Starting commit | Final |
| --- | ---: | ---: |
| Mean source-shape, 35 unique bodies | 98.77142857% | **98.87428571%** |
| Original `n_target` weighted source-shape | 97.21178381% | **98.04139024%** |
| OnTouchEnd(int,int,void*) source-shape | 94.9% | **97.8%** |
| OnUpdate source-shape | 96.8% | **97.5%** |
| OnTouchEnd strict fuzzy | 38.3% | 55.6% |
| OnUpdate strict fuzzy | 58.5% | 61.1% |

The canonical source scores are rounded to one decimal by the unchanged
verifier. A supplemental audit recomputes the same five ratios before rounding:
98.87420773% mean and 98.04722943% with the frozen weights. The gate therefore
does not rely on rounding 97.9986% up to 98%.

`n_target` is the verifier's normalized original stream length, which also
contains data directives/jump-table payloads; it is not a pure executable
instruction count. A separately disclosed audit excludes these original data
tokens from weights, using the same 35 bodies and unchanged source scores:
12,478 executable tokens, 98.04335631% canonical-score weighted, or 98.04924173%
before score rounding. This audit was added after editing and does not replace
the primary frozen registry/weights. Both weight definitions pass.

Final target/local normalized lengths: OnTouchEnd 2,769/2,746; OnUpdate
3,856/3,844. Neither large function is linked-exact or byte-exact.

## Source evidence and implementation

Primary reference: Android `libgof2hdaa.so`, ARM disassembly and
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`.
OnTouchEnd is at `0x17a144`; OnUpdate at `0x17c8d8`.
The IDA dump is reference pseudocode, checked against actual ARM instructions
where field width/offset is decisive. iOS
`analysis/gof2_ios_ida_full_dump.c` (`sub_1648AC`, `sub_167F4C`) is corroboration,
not the ARM matching target. No PC byte-shape recovery is claimed.

Inspected local MGame.cpp/.h, existing MGame reconstruction notes, the canonical
verification flags and comparison implementation, CMake/VALIDATION instructions,
and the relevant context pack. The changes preserve the existing MGame helper
structure rather than changing shared layouts or inventing new APIs.

* OnTouchEnd vitals/menu/action/orbit paths reload Status, player and menu fields
  across external calls in Android order instead of retaining whole-path cached
  receivers/action masks. Menu application data comes from Globals::appManager.
  Evidence: Android IDA lines 231511-231514, 231590-231593, 231816-231824,
  232068-232147. Initial menu/orbit null guards are retained.
* Failed freelance cleanup follows repeated mission/type/ship/cargo queries and
  the original reward/status lifetime. Android IDA lines 231399-231438 support
  direct ship/item dereferences in this branch. Previously added ship/item null
  guards here are removed; the cargo-array guard remains. These native paths
  assume valid mission/ship/item state, which remains a runtime risk.
* The three shipGroup-8 choice loops capture the enemy count once and use
  `i != count`; Status stores reload the global after calls. Android IDA lines
  231934-231990 explicitly show the captured count and termination test.
* OnUpdate uses gameOverActive (+0x60) for the early core-update branch,
  cargoConversionChoiceOpen (+202) in the paused menu gate, and jumpDriveActive
  (+220) for updateJumpScene. ARM anchors: 0x17d324, 0x17d4d0, 0x17d9d0.
* Script-time's previous value comparison is signed low-32, while the new
  script time and elapsed-time thresholds use the native 64-bit pairs. Engine
  wheel lookup occurs after camera/docking predicates; listener camera lookup
  retains this->paintCanvas. Android IDA lines 233613, 233619-233655,
  233782-233784; iOS lines 138085 and 138098 corroborate the first two.
* Paused free-camera Y clamping occurs even with inertia magnitude <=1.
  Free-look state is -98, 35, 3800 while rotateAroundTarget still receives
  -0.175, 0.49. Android IDA lines 234132-234164 and 234304-234309; iOS
  lines 138593-138594 contain the same stored float bits.
* Ambient status +0x17c is read/cleared as a byte, preserving adjacent bytes
  (ARM 0x17dab0/0x17e2ee/0x17e35a). Ambient timestamps reload the status field.
  Mission 23/24 pause/delete/clear precedes random selection, as in Android
  IDA lines 234330-234358. Dialogue fields are cleared between delete/new.
* Standing is queried only inside its hint guard. Damage hints also obey the
  mining-intro guard; damage/scanner/campaign choice flags are written after
  the display calls, with the native plain-choice receiver lifetime.
  Android IDA lines 234418-234505, 234624 and 234743 onward; ARM damage flag
  read/store at 0x17e6e2/0x17e798.

These statements describe verified paths/offsets, not a claim that all MGame
semantics have been recovered. Source-shape itself is a heuristic similarity
measure; runtime equivalence does not follow from the percentage.

## Trials

Each trial used the same original, compiler flags, frozen bodies and weights.
Values below are the resulting weighted family means, not per-trial deltas.
Machine-readable metrics are in
[`validation/mgame_2026-10-08_trials.json`](validation/mgame_2026-10-08_trials.json).

| Trial report suffix | Weighted % | Decision/reason |
| --- | ---: | --- |
| vitals_reload_trial | 97.575760 | Accept: native repeated vitals receivers |
| modal_native_reload_trial | 97.233194 | Reject broad guard removal; restore guards |
| modal_reload_trial | 97.597170 | Accept reload/global manager with guards retained |
| update_gates_canvas_trial | 97.656800 | Accept verified state gates, canvas and call order |
| touch_action_receiver_reload_trial | 97.763852 | Accept live action-mask/receiver reads |
| failed_mission_reload_trial | 97.763852 | Accept native cleanup/query lifetime |
| update_native_state_trial | 97.734037 | Accept proven semantics despite metric decline |
| update_hint_lifetime_trial | 97.912928 | Accept native hint ordering/lifetimes |
| keyboard_shared_tail_trial | 97.912928 | Reject neutral reshaping; restore prior tail |
| orbit_receiver_reload_trial | 97.998570 | Accept; still below 98% |
| paused_cargo_camera_trial | 97.968754 | Accept verified modal/clamp corrections |
| camera_magnitude_trial | 97.968754 | Reject neutral expression reshape |
| ambient_random_order_trial | 97.998570 | Accept native pause/delete/RNG order; still below 98% |
| choice_status_reload_trial | 97.998570 | Accept native global receiver lifetime |
| choice_native_loop_trial | 98.041390 | Accept verified captured-count loop |
| final_focus_20261008 | 98.041390 | Confirm after helper-signature cleanup |

Rejected trial code is not included in the final diff. No aliases, shells,
relaxed flags or verifier scoring changes are used to increase the average.

## Final gates and artifacts

[`validation/mgame_2026-10-08_gate.json`](validation/mgame_2026-10-08_gate.json)
contains hashes, full-corpus summaries, pair-set comparisons, and precision
results. Focused snapshots are `mgame_2026-10-08_baseline.json` and
`mgame_2026-10-08_final.json` in that directory.

* Native `cmake --build cmake-build-ucrt --target gof2 -j 8`: PASS, exit 0,
  host `libgof2.a`. A final incremental build after whitespace cleanup also
  passes. This is a library build, not an executable/gameplay runtime test.
* Full ARM builds at both baseline and final: 206 compiled, 0 failed, canonical
  `-Oz`/NDK r18b flags. Full verifier with `--fail-on-wrong-type`: exit 0.
* Full corpus before/after: 4,688 comparisons; 4,323/4,524 originals compared;
  201 absent originals; wrong signatures 0; skipped 0; extras 121 unchanged.
* Pair sets keyed by `(unit,symbol)`: linked-exact 2,301 -> 2,301, byte-exact
  997 -> 997, zero lost and zero gained in both sets. MGame retains 16/9.
* Every non-MGame comparison row is identical to baseline. Every final frozen
  MGame body metric agrees between focused and full reports. Focused/full
  MGame objects have the same SHA-256:
  `7ccb9b30c8a882ae103ac383be718fa2b85b62780cd35bbca4f8b9fded9f643d`.
* Existing verifier tests: 36 run, 35 passed, 1 skipped. `git diff --check` passes.

Large full reports remain local at `_work/mgame_baseline_full_20261008.json`
and `_work/mgame_final_full_20261008.json`, with corresponding logs. Original
ARM extracts and per-trial reports remain in `_work/`; they are not committed.
Environment for verification: GOF2_VERIFY_LOCAL_NDK=1,
GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work,
GOF2_NDK_ROOT=A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b,
MSYS tools on PATH. Host builds use the UCRT compiler. Build directories are on C:.

Not checked: live Android/native gameplay, device control/camera/audio behavior,
or a full native executable. Existing empty non-void key-handler warnings and
32-bit-era pointer casts remain. The large methods still have substantial strict
instruction-order differences; this result passes source-shape gates and
preserves existing exact pairs, but does not prove native ABI/runtime correctness.
