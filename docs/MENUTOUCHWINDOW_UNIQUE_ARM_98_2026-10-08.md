# MenuTouchWindow fixed-family ARM recovery (2026-10-08)

Branch: `codex/menutouchwindow-arm-98-20261008`.
Starting commit: `20bf2679a88ada953d3b393bc42a0aa54a557264`; clean worktree
confirmed before editing. Production scope is `src/game/ui/MenuTouchWindow.cpp`.
Its header, ChoiceWindow, TouchButton, shared status documents, verifier formula,
normalization, compiler flags and dependencies are unchanged.

## Fixed denominator and result

Before source edits, the Android 2.0.16 symbol map and a freshly compiled
focused report fixed **29 unique original bodies / 31 symbols** and **15,353
original executable instructions**. C1/C2 share `0x12299c`; D1/D2 share
`0x125170`. Each shared original address contributes once. Weights are the
baseline `n_target`, never generated counts or alias-expanded totals.

Tracked baseline/final ledgers in `docs/validation/menutouch_unique_*_2026-10-08.json`
include every original address, alias list, frozen weight, per-body score and
original normalized-instruction SHA-256. They also record rational scores and
means before rounding, evaluated with the unchanged five-component formula.
Every rounded body score was checked against the canonical verifier report.
The helper is local `_validation_menutouch/measure.py`, outside the commit.

| Unique-body measure | Baseline | Final |
| --- | ---: | ---: |
| Unweighted source-shape, before rounding | 84.02917865454857% | 87.68414867086149% |
| Original-instruction-weighted source-shape | 59.4359999165713% | 61.51427021819672% |
| Focused linked-exact symbols | 7 | 7 |
| Focused byte-exact symbols | 6 | 6 |

**Neither 98% gate passes.** These are assembly/source-shape measurements,
not a percentage of game completion or semantic verification.

| Changed body | Original address | Weight | Baseline / final source | Final strict |
| --- | --- | ---: | ---: | ---: |
| `update(int)` | `0x129598` | 557 | 88.8550 / 95.8168% | 85.0% |
| `startSupernovaChallenge()` | `0x12d0cc` | 281 | 1.0458 / 100.0000% | 99.3% |
| `OnTouchEnd(int,int,void*)` | `0x12aabc` | 2,866 | 88.3396 / 88.4177% | 13.5% |

Challenge is **not linked-exact**, despite 100% source-shape: the compiler
schedules two independent global stores in a different order. No volatile
stores, assembly, flag changes or semantic changes were introduced to force it.

## Evidence and recovered behavior

Primary source: Android 2.0.16 `libgof2hdaa.so`, read from the shared A: evidence
directory. Cross-checks use
`gof2_recovered_sdk_first_pass/extracted/ui_input_hud/MenuTouchWindow.c` and
direct GNU ARM disassembly. The IDA extract uses original addresses; the
Ghidra export's function index is shifted by `0x10000` and is only a locator,
not an independent instruction-level proof. No iOS/PC behavioral claim is made.

`update` now preserves the observed Android protocol:

- GameText IDs: error `100`, purchase results `92..96`, restore `76`, Telekom
  `3234`, store-init results `55/56`, screenshot results `57/58`.
- Store-ready image height comes from `heapBufA[0]` at `+0x134`, not the
  unrelated scrollbar image. Content/page-height and ready/options writes
  retain the original order and values.
- Restore errors work when the restore-pending byte alone is set. The original
  halfword clear at `+0x17a` also clears the byte at `+0x17b`.
- `dt` reaches ScrollTouchWindow, MissionsWindow and ChoiceWindow; shim calls
  that discarded it are removed from this body.
- Screenshot polling runs only in menu state 13, following a fresh state read.
- Main/options button centering snapshots the original count/top position;
  the options path no longer dereferences an integer count as a pointer.

### Correction to the earlier DLC note

The claim in `MENUTOUCH_DLC_STORE_STATE_ARM_2026-08-23.md` that the purchase
block uses exported `iap_hack` symbols is incorrect for these menu paths.
Original dynamic symbols establish:

| Original data address | Identity |
| --- | --- |
| `0x2181e8` | `Globals::options` |
| `0x21821d..0x218221` | `options+0x35..0x39`, the menu purchase block |
| `0x218380..0x218382` | the distinct exported `iap_hack_dlc1/2/3Bought` bytes |

Android `update` instructions write the options block, including `+0x3b` for
store readiness; the IDA state-15 `OnTouchEnd` body reads that same block.
Both purchase writes and `_mtw_dlc_is_owned` now use the proven options offsets.
Two private substitute ownership bytes have been removed. This is a correction
of the Android behavior, not a change of purchase policy for a score.

`startSupernovaChallenge` replaces a one-instruction unresolved shim with the
original campaign/ship/equipment/station/mission/music/module sequence. ARM
instructions explicitly confirm ship-table index 46; equipment indices
231, 230, 231, 73, 54, 59, 83, 206, 75, 76 and quantity-five index 216;
station 111; mission arguments `(183,"Client",createChar(true,0),0,0,station,1)`;
the exact hints bytes; both `setKills(500)` and `setKills(220)`; and module 2.
Apparently redundant original calls are retained.

## Remaining gap and limit of this pass

The largest bodies remain substantially incomplete:

| Body | Original / generated instructions | Source-shape |
| --- | ---: | ---: |
| `draw` | 4,672 / 614 | 20.8041% |
| Constructor | 3,316 / 1,731 | 57.5576% |
| `OnTouchEnd` | 2,866 / 2,849 | 88.4177% |

Freezing just the current draw and constructor places an upper bound of
**66.7334% weighted**, even if every other body reaches 100%. This is a bound
on the current patch, not a claim that 98% is fundamentally impossible.

The constructor still partitions large construction sequences into local
helpers and retains extra guards/approximate initialization. `draw` omits much
of the original options, text/temporary lifetime, store/screenshot and layout
work. Several decompiler `Layout::drawBox`, String expression and mission calls
omit arguments; those must be audited against ARM call sites before restoring
them. Padding operations or guessing those arguments would not constitute
recovery. This pass leaves those bodies as explicit follow-up work.

Native challenge gameplay remains untested. In particular, the existing
`Galaxy::getStation(int)` API returns a 32-bit `int` used as a station pointer;
the explicit `intptr_t` cast preserves its existing ARM32 contract but does
not fix that separate API's host-pointer-width limitation. The host smoke does
not exercise challenge resources, constructors, ownership or real services.

## Validation and reproducibility

All output is under the C: worktree's `_validation_menutouch/`; A: supplies
read-only binary, maps, decompiler evidence and NDK. No A: build was performed.

Environment:

```text
GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work
GOF2_VERIFY_LOCAL_NDK=1
GOF2_NDK_ROOT=A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b
PATH includes C:/msys64/ucrt64/bin and C:/msys64/usr/bin
```

Checks completed:

- Native `gof2` build with UCRT64, including the final production source.
- All 206 ARM source-mapped translation units compile; zero failures.
- 36 verifier tests: 35 pass and the real-object test is skipped in discovery;
  its dedicated run with `engine/core/ArrayInstantiations.o` passes both tests,
  including all seven audited alias cases. Four scope-filter tests pass.
- Native ABI/lifetime smoke and ParticleSettingsRef smoke pass.
- New MenuTouchWindow update smoke passes. It extracts the production method
  unchanged and uses constructed fixture objects/service doubles, following the
  existing particle-control test approach. It covers five purchase results,
  restore-only error, restore success, source image selection/ready transition,
  screenshot state gating, and nonzero `dt` forwarding. It is a host logic
  smoke, not an ARM32 ABI or complete menu runtime test.

```text
python tools/run_menutouch_update_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe --out _validation_menutouch/update-smoke
python tools/verify/verify.py --build-dir _validation_menutouch/arm --no-build --unit game/ui/MenuTouchWindow --only ^_ZN15MenuTouchWindow --report _validation_menutouch/final-focused.json
python tools/verify/verify.py --build-dir _validation_menutouch/baseline-arm --no-build --report _validation_menutouch/baseline-full-frozen.json --fail-on-wrong-type
python tools/verify/verify.py --build-dir _validation_menutouch/arm --no-build --report _validation_menutouch/final-full.json --fail-on-wrong-type
```

The first full baseline run overlapped a rebuild, skipped MenuTouchWindow and
is rejected. The accepted baseline uses a separate 206-object tree with the
saved pre-edit MenuTouchWindow object; target/report directories are distinct
from final. No build or focused operation runs against either full-report tree
while that report is active.

Accepted full reports both exit successfully with `--fail-on-wrong-type`:

| Full-corpus measure | Frozen baseline | Final |
| --- | ---: | ---: |
| Comparison rows | 4,689 | 4,689 |
| Unique originals compared | 4,323 / 4,524 | 4,323 / 4,524 |
| Missing originals | 201 | 201 |
| Wrong-type / skipped units | 0 / 0 | 0 / 0 |
| Mean source-shape | 91.80% | 91.82% |
| Mean strict fuzzy | 79.73% | 79.76% |
| Linked-exact pairs | 2,305 | 2,305 |
| Byte-exact pairs | 997 | 997 |

The exact-set gate compares `(unit,symbol)`, not totals. It finds **zero
lost/gained comparison rows, zero linked-exact losses/gains and zero byte-exact
losses/gains**. Every focused baseline/final row also equals its corresponding
full-corpus row on all recorded comparison metrics. Coverage is unchanged.
Both tracked family ledgers contain the gate summary and report/object hashes.
The local `_validation_menutouch/full-pair-gate.json` has the same evidence.

Full report SHA-256:

```text
baseline-full-frozen.json 4d3e5b2a7d703547d12206ff1cb36301bbd48be15b13c34651317512a8478c77
final-full.json          7e54ba78d85fcbb2f54deb43ed1c7c29f6c03f2c337c14c8ed9c0d8d0867162e
```

Inspected instructions/docs: AGENTS, README, CONTRIBUTING, VALIDATION, the
current PROJECT_STATUS summary, the three earlier MenuTouch main/state/DLC
notes, MenuTouchWindow header/implementation, and the directly referenced
UI, Status, Item, Ship, Mission, Globals and ApplicationManager contracts.
Shared PROJECT_STATUS and integration reconciliation files are intentionally
left to the integrator.
