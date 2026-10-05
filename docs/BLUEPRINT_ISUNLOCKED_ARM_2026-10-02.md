# BluePrint::isUnlocked ARM ABI correction — 2026-10-02

## Evidence and change

Android HD `libgof2hdaa.so`, symbol `_ZN9BluePrint10isUnlockedEv` at
`0x17735a`, loads the unsigned byte at `BluePrint+0x08` and returns that value
directly. The Android IDA dump in `analysis/gof2_libgof2hdaa_full_ida.c`
shows the same return contract. The prior C++ declaration returned `bool` and
the implementation normalized the field with `locked != 0`; that extra
normalization was not present in the native body. Both declaration and
definition now return `int` and the raw `locked` byte. Callers in `Status` and
`HangarList` already consume the result as an integer or truth value; no
caller-side change was required. The similarly named `lock` and `unlock`
functions were not altered: the Android body of each stores `1` at `+0x08`.

## Validation

Baseline is clean commit `f60694dd`, with full-corpus report
`_work/mission-generation/final_full.json` in the Level mission worktree.
The focused `isUnlocked` comparison changed from **70.9% source-shape /
57.1% strict fuzzy, neither exact** to **100% / 100%, linked-exact and
byte-exact**. Reports for this worktree are
`_work/blueprint-isunlocked/baseline_focused.json` and
`_work/blueprint-isunlocked/after_focused.json`.

The native UCRT64 `gof2` static library built; the matching ARM build compiled
**204/204** translation units. The full ARM corpus compared **4,619 rows**,
covering **4,274/4,524** original symbols, with **0 skipped**. Linked-exact
rows rose **2,079 → 2,080** and byte-exact rows **959 → 960**. Comparison of
the complete `(unit, mangled symbol)` exact sets found **zero lost pairs** and
exactly one gained pair in each set: `BluePrint::isUnlocked`. The 250 missing
names remain 7 wrong-signature and 243 absent. Final generated report:
`_work/blueprint-isunlocked/final_full.json`.

This is a source-backed ABI/code-generation correction, not a claim of a
runtime gameplay test or of completed BluePrint recovery.
