# Layout tip initialization — Android ARM audit (2026-10-13)

Scope: `Layout::initTip()` (`_ZN6Layout7initTipEv`) in the Android
2.0.16 ELF at `0xd495c`. Evidence is the original Thumb disassembly
and `analysis/gof2_libgof2hdaa_full_ida.c:130876-130912` in the
shared research directory. No iOS/PC implementation was needed.

The old method dereferenced four unassigned static holders for color,
text ID, canvas and layout metrics. The original instead saves the
current font, game-text service and canvas from `Globals`, calls
`Globals::rnd->nextInt(13)`, loads text ID `340 + result`, and wraps
it with width `Globals::layout->field_0x78 - 2 * field_0x4c`.
It releases the previous line strings and array, then allocates a new
array. The local `ArrayRemoveAll` between release and deletion was an
extra allocation absent from the original and is now removed.

Focused ARM source-shape rose from 79.2% to 100.0%, and strict fuzzy
from 50.5% to 97.0% (66 original and local instructions). It remains
neither linked- nor byte-exact; register/GOT scheduling differs even
though the verifier's instruction, opcode, control-flow, call-count
and size components each report 100%.

A host smoke extracts the production method and checks the random
bound, text ID, font, wrap width, output array, and release/recreation
on a second call. Native `gof2`, all 206 ARM units and 37 verifier
tests pass (one skipped). The smoke does not use original fonts, text
assets or ARM32 runtime.

Full ARM corpus: 4,718 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.88% average source-shape and 81.27% strict
fuzzy, 2,398 linked-exact and 1,008 byte-exact. Complete
`(unit,symbol)` comparison with the preceding report shows no linked-
or byte-exact loss and no change to the 123 extra symbols. The one
fewer comparison is the unused, non-exact
`ArrayRemoveAll<String*>` template instantiation removed from this
translation unit, not a loss of original-function coverage. Reports:
`A:/Galaxy on Fire new version/_work/layout_init_tip_before_20261013.json`,
`layout_init_tip_after_20261013.json`, and
`layout_init_tip_full_20261013.json`. Next priority:
`Layout::drawEmptyFooter`, currently 43.0% source-shape.
