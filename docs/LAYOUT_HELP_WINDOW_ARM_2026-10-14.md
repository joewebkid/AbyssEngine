# Layout help window — Android ARM audit (2026-10-14)

Scope: `Layout::initHelpWindow(AbyssEngine::String)` and
`Layout::drawHelpWindow()` in the Android 2.0.16 ELF at `0xd4b40`
and `0xd4bc0`. The Thumb code is primary evidence, cross-checked
with the IDA export at
`analysis/gof2_libgof2hdaa_full_ida.c:130945-130972` and the
Ghidra export at
`analysis/gof2_gidra_engine_full_source.c:105281-105327`.
No iOS/PC implementation was used.

The original creates a `ChoiceWindow` only when absent, plays sound
`0x7e` through `Globals::sound`, fetches title text `0x187` through
`Globals::gameText`, calls the three-argument
`ChoiceWindow::set(title, message, false)`, then sets the open flag
and clears the pressed flag. The local code dereferenced an
unassigned sound holder and called the two-argument `set` overload,
which selects different title/button text and could not reproduce
the help window. These are corrected. Keeping the `ChoiceWindow`
pointer before the text lookup also follows the original call
sequence.

The original `drawHelpWindow()` calls `ChoiceWindow::draw()` without
a null check. The local check is removed. Normal callers first
check `choiceWindowOpen`, which `initHelpWindow()` sets only after
creating the `ChoiceWindow`; directly invoking `drawHelpWindow()`
with a null pointer remains invalid, as in the original.

The production-method host smoke checks first and repeated help
initialization, sound ID, localized title ID, `false` button flag,
message content, window reuse, state flags and draw dispatch.
Focused results: `initHelpWindow` rises from 93.9% to 100.0%
source-shape and 74.2% to 97.9% strict fuzzy, but is not exact.
Its remaining diff is the order of two independent byte stores;
the current source expresses the Android order, and forcing a
compiler-specific order would not improve behavior.
`drawHelpWindow` rises from 39.4% to 100.0% source-shape and from
28.6% to 100.0% strict fuzzy, adding a linked-exact (not
byte-exact) result. Native `gof2`, all 206 ARM translation units,
the production-method host smoke and 37 verifier tests pass
(one skipped). Original ARM32 runtime and visual rendering with
game assets remain untested.

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.97% average source-shape, 81.41% strict fuzzy, 2,403
linked-exact and 1,008 byte-exact. Compared with the preceding
full run, `drawHelpWindow` is the sole linked-exact addition; the
complete byte-exact and extra-symbol sets are unchanged, and no
previous exact match is lost. Next priority: inspect
`Layout::showMissionRewardMessage()` and its display partner,
which still reference an unassigned sound holder and have
unverified UI behavior.
