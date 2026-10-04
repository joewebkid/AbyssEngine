# ChoiceWindow::setHeight ARM global bindings (2026-10-04)

`ChoiceWindow::setHeight(int)` was already structurally recovered, but its
file-local `Layout**` and screen-height `int*` placeholders were permanently
null. Under `-Oz`, that undefined read collapsed the ARM body to one
instruction. This also broke the native path used by Hangar dialogs and
`ChoiceWindow::setMiscButton`.

The Android 2.0.16 `libgof2hdaa.so` function at `0x146dc4` loads its two
globals via GOT entries `0x210224` and `0x210280`. The original relocation
table identifies those entries as `Globals::layout` and `Globals::h`.
The accepted change uses exactly those existing globals, retaining the
original arithmetic, button branches and second layout reload.

| Check | Before | After |
| --- | ---: | ---: |
| Focused strict fuzzy | 0.0% | 100.0% |
| Focused source-shape | 0.3% | 100.0% |
| Focused linked-exact | No | Yes |
| ARM translation units | 206/206 | 206/206 |
| Full-corpus comparison rows | 4,673 | 4,673 |
| Full-corpus unique coverage | 4,322/4,524 | 4,322/4,524 |
| Full-corpus mean source-shape | 89.97% | 89.99% |
| Full-corpus mean strict fuzzy | 77.07% | 77.09% |
| Full-corpus linked-exact | 2,156 | 2,157 |
| Full-corpus raw-byte-exact | 977 | 977 |

The full `--fail-on-wrong-type` run has zero skipped and zero wrong-signature
symbols. Comparing exact unit/symbol **sets**, not just totals, found no
linked-exact or byte-exact losses. Native UCRT64 `gof2` builds. Local ignored
reports: `_work/blueprint-isunlocked/choice_setheight_before.json`,
`choice_setheight_after.json`, and `choice_setheight_full.json`; the preceding
full baseline is `stdlib_runtime_elf_sizes_full.json`.

Only this method is claimed restored. Other `ChoiceWindow` methods still use
unbound globals and need their own original relocation/call-path evidence.
No whole-class behavior or byte-exactness claim follows from this result.
