# ChoiceWindow modal set global bindings (2026-10-04)

The Android 2.0.16 ARM bodies for the four short `ChoiceWindow::set` overloads
at `0x1468c8`, `0x1468f4`, `0x146954`, and `0x146d5c` load GOT entry
`0x2102a0` (`Globals::gameText`). The primary eight-argument modal `set` at
`0x1469b0` also loads that entry, plus the confirmed entries below. The
recovered C++ previously dereferenced separate file-local null pointers, so
several paths compiled away and were unsafe at runtime.

| Original GOT entry | Confirmed object | Use in primary `set` |
| --- | --- | --- |
| `0x2102a0` | `Globals::gameText` | Default button text comparisons |
| `0x210224` | `Globals::layout` | Default width and modal geometry |
| `0x210278` | `Globals::w` | Horizontal centering |
| `0x210168` | `Globals::sound` | Opening sound |
| `0x210218` | `Globals::globals` | Line wrapping |
| `0x21029c` | `Globals::font` | Text measurement handle |
| `0x210280` | `Globals::h` | Vertical centering |

Only these bindings were changed. `Globals::globals` remains declared as
`void*` in the shared header, so the call uses the same local `Globals*` cast
as its established callers; changing that global type across the project is
outside this package.

| ARM metric | Before | After |
| --- | ---: | ---: |
| Five-overload mean strict fuzzy | 50.3% | 81.8% |
| Five-overload mean source-shape | 65.8% | 97.8% |
| Primary modal `set` source-shape | 30.7% | 96.1% |
| Full comparison rows | 4,673 | 4,675 |
| Unique originals compared | 4,322/4,524 | 4,322/4,524 |
| Full mean strict fuzzy | 77.15% | 77.20% |
| Full mean source-shape | 90.03% | 90.07% |
| Full linked-exact rows | 2,160 | 2,162 |
| Full raw-byte-exact rows | 977 | 977 |

The two added linked-exact rows are the `Array<String*>` release and
destructor emitted in this translation unit; they are duplicate comparison
rows, **not** two newly covered unique original functions. A full
`--fail-on-wrong-type` run completed with zero skipped and zero wrong-signature
symbols. Comparing exact unit/symbol sets found no losses. Native UCRT64
`gof2` and 206/206 matching ARM translation units build. Local ignored
reports: `_work/blueprint-isunlocked/choice_set_family_before.json`,
`choice_set_family_bindings.json`, `choice_layout_full.json` (baseline), and
`choice_set_family_full.json`.

None of the five `set` overloads is linked- or byte-exact. The primary body
still differs in instruction order and stack frame; its runtime String,
scroll-window, and button ownership paths need independent confirmation.
Next, inspect `setMedal` and `draw` only after mapping their tables and global
targets to original Android addresses.
