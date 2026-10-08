# ChoiceWindow unique Android ARM body gate (2026-10-08)

Branch: `codex/choicewindow-arm-98-20261008`, integration base `7314772c`.
Code scope: `src/game/ui/ChoiceWindow.cpp`; no header or dependency edits.
Route, Ship, PROJECT_STATUS and verifier implementation/flags are unchanged.

## Frozen family and measurement

Before editing, the original Android symbol map and canonical focused report
fixed **21 unique bodies / 23 symbols**, with **1,267 original executable
instructions**. C1/C2 and D1/D2 share original even ARM addresses and contribute
once each. Weights are the original `n_target`, never generated instruction counts.
The tracked baseline ledger is `validation/choice_unique_baseline_2026-10-08.json`;
the final ledger is `validation/choice_unique_final_2026-10-08.json`.

| Frozen-body source-shape mean | Baseline | Final |
| --- | ---: | ---: |
| Unweighted, before rounding | 99.11099428183964% | 99.63751298557348% |
| Original-instruction weighted, before rounding | 97.55696583779205% | 98.63400320807008% |

Both final gates pass 98.0%. The baseline weighted gate fails. The ledgers store
exact rational scores/means as well as decimal displays. They apply the unchanged
canonical `source_shape_metrics` normalization and five equal component weights
to disassembled original/generated instructions, evaluating the same inventories,
ordered/inventory control flow, call count and size using rational arithmetic.
The decision uses fractions before the verifier's one-decimal per-body rounding.
All rounded per-body values were checked against the canonical focused report.
This is source/assembly shape evidence, not a semantic or playable-game percentage.

| Body | Original address | Baseline / final source-shape | Final strict fuzzy |
| --- | --- | ---: | ---: |
| Extended `set` | `0x1469b0` | 96.1 / 97.7% | 69.6% |
| `draw` | `0x1471bc` | 95.8 / 97.9% | 86.4% |
| `setMiscButton` | `0x147068` | 97.6 / 97.6% | 79.1% |
| `set(String const&, String const&)` | `0x146954` | 96.3 / 100.0% | 100.0% |
| `set(String const&, String const&, bool)` | `0x1468f4` | 96.4 / 100.0% | 100.0% |

The two short overloads become linked-exact. Focused symbol counts are
15 -> 17 linked-exact and 5 -> 5 raw-byte-exact; the large bodies remain non-exact.

## Accepted source forms and primary evidence

Primary: Android 2.0.16 `libgof2hdaa.so`, supplied through `GOF2_VERIFY_WORK`.
Addresses below refer to that binary, not inferred iOS names or offsets.

- Both short multi-text overloads reload `Globals::gameText` before each text
  query (`0x146914/91a/922`, `0x146972/978/980`). The extended overload likewise
  reloads it for the second default-label comparison (`0x1469de`, `0x1469f8`).
  Direct `Compare(const String&)` preserves the evidenced call without a const cast.
- Extended `set` reloads the layout after wrapping (`0x146ad4`), before creating
  paired buttons (`0x146bde`), and for the right/single button (`0x146c5a`,
  `0x146ca4`). Preserve the captured layout through scroll-window allocation;
  later global owners are re-read at the native boundaries.
- The two scroll-text arguments use `String(const String&, false)` construction
  (`0x146b70`, `0x146b7a`) with reverse-order destruction after `setText`.
  Direct by-value temporaries replace the shim copy's inlined `Set` path.
- Preserve the scaled float through button creation and convert at both call
  arguments. Android has the two float-to-int conversions at `0x146c0e/1a`;
  the 0.85 and 1.5 formulas and all button flags are unchanged.
- `setMiscButton` captures padding after both width queries on the paired path
  (`0x14708c`), and at entry to the missing-button path (`0x1470a4`), rather than
  before the queries. This semantic timing repair is retained despite unchanged
  rounded source-shape.
- `draw` reloads layout after `drawMask` (`0x1471f4`) and constructs the box-title
  argument with the explicit bool-false String constructor (`0x1471f8`).
- Each measured text retains the original canvas/font and x/width snapshots
  across `GetTextWidth` (`0x1472ec..306`, `0x147346..360`); y and vertical padding
  are read after measurement, as in the original.
- Medal title color is **0xff8000ff**, the exact four-byte word at `0x147488`,
  loaded by `0x14733a`. The former white value was wrong. Independent iOS check:
  `analysis/gof2_ios_ida_full_dump.c`, `sub_12008`, sets `-8388353` before medal
  title measurement/drawing. iOS is corroboration only; its layout is different.
- Right-button pointer is checked again after `draw()` (`0x1473a4/3a6`) before
  publishing coordinates. The separate check follows Android's live owner read.

## Rejected forms and limits

- Cached GameText/layout owners across the identified native reload boundaries
  were rejected: they omit observed reads and can retain stale owners after calls.
- Implicit shim copies for the scroll-text and box-title arguments were rejected:
  their `Set` call/initialization path differs from the original bool constructor.
- Reloading canvas and x/width after measuring text was rejected: Android retains
  those snapshots. Conversely, caching y/padding before measurement is not proven.
- A single right-button check around drawing and coordinate publication was
  rejected because it misses the original post-draw check. White medal-title color
  was rejected against the Android literal and independent iOS evidence.
- No padding, artificial branches, asm barriers, UB tricks, score-formula edits,
  compiler flag changes or unrelated fixes were tried. Further register/scheduling
  experiments were unnecessary once the source-backed form passed both gates.

The reviewed surfaces include ChoiceWindow header/implementation, its caller
inventory, ScrollTouchWindow and TouchButton contracts, String constructors,
Layout/Canvas interfaces, the existing ChoiceWindow recovery notes and VALIDATION.
Native `gof2` is a static library; no ARM device execution or modal/medal runtime
test was performed. Existing medal-table count assumptions and broader ownership
contracts remain unvalidated. No heuristic coordinate/formula recovery is claimed.

## Validation and reproduction

Full reports pass `--fail-on-wrong-type`. Coverage remains **4,323/4,524**
originals, **4,689 (unit, symbol) pairs**, 201 missing, zero wrong-type and zero
skipped units. Compared original-name and comparison-pair sets are identical.
All **2,301 linked-exact and 997 raw-byte-exact baseline pairs** survive: zero
losses of either kind; the two short `set` overloads add linked-exact pairs for
a final **2,303 / 997**. Exact pair/name/report hashes are in the final ledger.
Focused and full baseline/final ChoiceWindow rows agree on every metric;
only the five methods listed above change in the full corpus. Full mean
source-shape stays 91.76%; strict fuzzy rises 79.65 -> 79.67% (report precision).

- Native Windows/MSYS2 `gof2` static-library build: pass, including final overloads.
- Canonical NDK r18b/Oz ARM build: 206/206 TUs, zero failures.
- Verifier tests: 36 run, zero failures, one real-object fixture skipped; then
  the two real `ArrayInstantiations.o` alias tests pass with zero skipped.

Environment:

```
GOF2_VERIFY_LOCAL_NDK=1
GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work
GOF2_NDK_ROOT=A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b
```

All generated objects, builds and reports stay on C: inside this worktree. A:
supplies original binary/toolchain/reference evidence only. Frozen full baseline
uses `_work/baseline-verify` with the saved pre-edit ChoiceWindow object; final
uses `_work/final-verify`. These are separate object/target trees to prevent
concurrent rebuild/report writes. The first full run overlapped a rebuild and
was discarded as baseline evidence.

```
python tools/verify/verify.py --build-dir _work/baseline-verify --no-build --report _work/baseline-full-frozen.json --fail-on-wrong-type
python tools/verify/verify.py --build-dir _work/final-verify --no-build --report _work/final-full.json --fail-on-wrong-type
python tools/verify/verify.py --build-dir _work/focused --no-build --unit game/ui/ChoiceWindow --only '^_ZN12ChoiceWindow' --report _work/final-choice.json
```

Focused reports: `_work/baseline-choice.json`, `_work/final-choice.json`.
Local, uncommitted measurement helper: `_work/choice_measure.py`; it does not modify or
replace the shared verifier. Tracked ledgers include all body weights/scores and
hashes of focused reports and objects; full-corpus pair gates are added finally.
