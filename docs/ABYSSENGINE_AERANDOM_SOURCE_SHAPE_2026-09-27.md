# AbyssEngine AERandom ARM pass (2026-09-27)

The latest full ARM report contains 1,009 `_ZN11AbyssEngine*` comparisons.
Before this pass their unweighted source-shape average was **88.52%** (74.79%
strict). The requested **99.9% for the whole namespace remains open**; a
single small class cannot establish that goal.

## Accepted source forms

Android `libgof2hdaa.so` / IDA anchors are `AERandom` default constructor
`0x719e0`, `nextInt()` `0x71aa4`, and `nextInt(int)` `0x71ad0`. The standalone
`next(int)`, `reset()` and `setSeed(long long)` were already 100% source-shape
and were not changed.

| Function | Before | After | Accepted form |
| --- | ---: | ---: | --- |
| `nextInt()` | 22.5% | 100% | In-place 48-bit LCG step and 31-bit extraction, matching 16/16 normalized ARM instructions. |
| `nextInt(int)` | 55.0% | 96.4% | In-place LCG for power-of-two bound; local seed/rejection loop and one final seed commit otherwise. |
| Default constructor `C1`/`C2` | 72.6% each | 100% each | In-place time-based seed initialization without changing the already exact `reset()`. |

The 11-symbol `AERandom` mean rose from **83.9% to 99.7% source-shape**.
All seven previously 100%-shape members stay at 100%; the final class has ten
100%-shape symbols. Reports are local generated evidence:
`_work/aerandom_baseline_20260927.json`,
`_work/aerandom_nextint_trial_20260927.json`,
`_work/aerandom_bounded_trial_20260927.json`, and
`_work/aerandom_constructor_trial_20260927.json`.

## Rejected/deferred forms

- The former `nextInt()` tail call to `next(31)` gave only 22.5% source-shape;
  it was replaced. Marking `next(int)` globally always-inline was not used:
  that would perturb many other already verified call sites.
- The former bounded implementation committed the seed through repeated
  `next(31)` calls. Android holds the seed in a local recurrence for rejection
  sampling and writes it after acceptance; the old source form was replaced.
- The remaining bounded-function diff is 50 versus 49 instructions, mainly
  register/literal scheduling. No dummy stores, volatile variables, or fake
  branches were introduced to force its final 3.6 source-shape points.

The checked iOS engine dumps do not expose this symbol family, and the local
PC installation is a resource/runtime build rather than an attributed C++
decompilation. Neither is counted as an independent ARM-shape confirmation.
Android instructions are the authoritative source for this package.

## AEMath Matrix comparison follow-up

Android `AEMath::operator==(Matrix const&, Matrix const&)` at `0x80448` and
`operator!=` at `0x8052c` compare exactly the first 12 matrix floats. The
previous `==` loop and `!=` call-through produced 34.0% and 1.5%
source-shape. Direct comparisons raised `!=` to 100% linked- and byte-exact;
the first uniform 12-term `==` expression reached 97.9% source-shape but
placed its final branch differently. Splitting the first ten comparisons from
the final two, matching Android control flow, made `==` 100% linked- and
byte-exact as well. The 67-symbol focused `AEMath` mean rose from **90.9% to
93.4% source-shape**. Reports: `_work/aemath_baseline_20260927.json`,
`_work/aemath_explicit_compare_trial_20260927.json`, and
`_work/aemath_split_tail_trial_20260927.json`.

Rejected source forms: the old loop/call-through and the neutral-to-final
uniform `==` chain were not kept. The accepted branch split is supported by
both the Android pseudocode and 66/66 ARM instructions; no synthetic spills
or unused fields were added. The recovered `Matrix` header holds 15 floats,
but these operators intentionally compare only the first 12 as the Android
body does. Other Matrix functions with 16-element accesses require a separate
layout/ABI audit, not an inferred change in this package.

## Full-corpus gate

After both accepted packages, all 204 ARM translation units compile. The
unscoped report `_work/full_arm_abyssengine_aerandom_aemath_20260927.json`
completed 4,626 comparisons with zero skipped and zero `missing_wrong_type`;
its whole-corpus source-shape is 89.61%, with 2,075 linked-exact and 945
byte-exact functions. UCRT64 `gof2` linked `libgof2.a` after each package.

For the actual 1,009-symbol `_ZN11AbyssEngine*` family, the fresh full report
measures **88.856% source-shape / 75.184% strict**, up from **88.52% /
74.79%** before these changes. The 99.9% goal remains open. The largest
remaining source-shape deficits by unit are `PaintCanvas` (1,584.5 aggregate
points), `Engine` (1,135.2), `ApplicationManager` (1,008.1), `String`
(783.4), core `AbyssEngine` (686.2), and `Transform` (469.7). The next major
packages must address those bodies and their platform/GL/runtime contracts;
another leaf-only pass cannot close the namespace-wide gate.

## Matrix assignment/identity/in-place multiplication follow-up

Android IDA anchors: `Matrix::operator=` at `0x80130`, `Matrix::operator*=`
at `0x8016a`, and `MatrixIdentity` at `0x80694`. The original `Matrix` is
0x3c bytes: 12 basis/translation floats followed by a three-float `Vector`.
The previous 16-element assignment and identity loops wrote beyond that
object. The accepted forms copy three 16-byte chunks, then assign the tail
`Vector`; zero the two four-float spans and set the native identity diagonal;
and multiply the first 12 floats in place before multiplying the tail vector.
`Vector::operator=` is kept out of line in this translation unit, as the
Android assignment and identity bodies both call it.

| Function | Previous source | Current source | Strict / status |
| --- | ---: | ---: | --- |
| `Matrix::operator=` | 11.4% | 100% | 100%, linked-exact (19/19) |
| `MatrixIdentity` | 37.3% | 100% | 100%, linked-exact (39/39) |
| `Matrix::operator*=` | 28.6% | 92.2% | 14.1%, 107 local vs 91 target instructions |

The 67-symbol `AEMath` unit rose from **93.39% to 96.60% source-shape**.
The 1,009-symbol AbyssEngine namespace rose from **88.856% / 75.184%
source/strict to 89.068% / 75.386%**. Focused reports are
`_work/aemath_matrix_assign_block_trial_20260927.json`,
`_work/aemath_matrix_identity_trial_20260927.json`, and
`_work/aemath_matrix_family_accepted_20260927.json`. The full report is
`_work/full_arm_abyssengine_matrix_package_20260927.json`: 204 ARM units
compile; 4,626 comparisons complete, zero wrong-type misses, 2,077
linked-exact and 945 byte-exact. UCRT64 `gof2` links `libgof2.a`.

Rejected: one 48-byte `__builtin_memcpy` lowered to a library call and
achieved only 74.1% for assignment, versus the target's three NEON copies.
The old 16-float loops and temporary-matrix `operator*=` were not retained.
The in-place multiply is behavior-backed by Android but not byte-matched;
its remaining ARM register scheduling and extra loads must not be called
exact. The iOS dumps and PC resources checked in the earlier pass do not
independently attest these particular bodies. The `reinterpret_cast` tail
access and other Matrix operations still need a dedicated runtime/aliasing
audit; this package establishes ARM source shape, not runtime certification.

For the 99.9% namespace target, the next substantive work is the large
`PaintCanvas`, `Engine`, and `ApplicationManager` bodies. Small math leaves
alone cannot close the remaining 10.832 average source-shape points.
