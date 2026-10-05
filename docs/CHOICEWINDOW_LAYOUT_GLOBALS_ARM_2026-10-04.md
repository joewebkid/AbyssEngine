# ChoiceWindow layout globals (2026-10-04)

The Android 2.0.16 `libgof2hdaa.so` relocation table identifies GOT entries
`0x210224`, `0x210278`, and `0x210280` as `Globals::layout`, `Globals::w`,
and `Globals::h`. The original ARM bodies for the `ChoiceWindow` constructor
at `0x1467ec`, `setWidth(int)` at `0x146da4`, and `setMiscButton(String const&)`
at `0x147068` read those entries. The recovered source instead read separate
file-local pointers permanently initialized to null.

This patch binds the three methods to the existing native globals. In
`setMiscButton`, the original reloads the layout row height independently
before translating each button and reloads the layout inset for the final
scroll position. The source now preserves those reads instead of caching one
shared delta. No button formula or branch was changed for matching alone.

| ARM check | Before | After |
| --- | ---: | ---: |
| Four-row mean strict fuzzy | 23.6% | 94.7% |
| Four-row mean source-shape | 54.9% | 99.4% |
| Linked-exact rows | 0 | 3 |
| `setMiscButton` source-shape | 17.7% | 97.6% |
| Full comparison rows | 4,673 | 4,673 |
| Unique original functions compared | 4,322/4,524 | 4,322/4,524 |
| Full mean strict fuzzy | 77.09% | 77.15% |
| Full mean source-shape | 89.99% | 90.03% |
| Full linked-exact | 2,157 | 2,160 |
| Full raw-byte-exact | 977 | 977 |

The native UCRT64 `gof2` build and 206/206 matching ARM translation units
pass. The full `--fail-on-wrong-type` corpus reports zero skipped and zero
wrong-signature symbols. Exact unit/symbol **sets** show no linked-exact or
byte-exact losses. Local ignored reports are
`_work/blueprint-isunlocked/choice_layout_before.json`,
`choice_layout_reload_after.json`, `choice_setheight_full.json` (baseline),
and `choice_layout_full.json`.

`setMiscButton` is **not** linked- or byte-exact: its remaining strict diff
includes register allocation and stack-frame shape. Other `ChoiceWindow`
methods still contain unbound globals; their behavior and the whole-class
runtime path require separate evidence and validation. The next priority is
the primary `set(...)` modal construction path, not cosmetic register tuning.
