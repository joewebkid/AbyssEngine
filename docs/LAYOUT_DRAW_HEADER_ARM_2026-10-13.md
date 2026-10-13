# Layout header — Android ARM audit (2026-10-13)

Scope: `Layout::drawHeader(String,bool)`
(`_ZN6Layout10drawHeaderEN11AbyssEngine6StringEb`) in the Android
2.0.16 ELF at `0xd4100`. Primary evidence is the original Thumb
disassembly and `analysis/gof2_libgof2hdaa_full_ida.c:130478-130532`
in the shared research directory. No iOS/PC implementation was used.

The local method dereferenced unassigned holders for cap-image width,
layout metrics and font, used the separate canvas holder, and called
an unrelated footer-animation shim for the help button. Android uses
`Globals::Canvas` for color and image operations, measures
`headerCapImage`, tiles `headerPatternImage` between the two caps,
and draws the right cap with flags `0x11, 0x12, 0x01`. For a nonempty
title it draws `headerIconImage` and text with `Globals::font`, using
the two offsets at `Globals::layout + 0x28` and `+ 0x44`. It stores
the transition flag in `helpButtonEnabled` and calls
`helpButton->draw()` only when transition is requested and no choice
window is open. The recovery restores that sequence.

Focused source-shape rises from 74.2% to 99.8%; strict fuzzy rises
from 32.1% to 97.7%. It remains neither linked- nor byte-exact.
Both sides have 86 ARM instructions. The remaining local instruction
difference is the order of equivalent additions for text X; a simple
source-level rearrangement did not change Clang's output, so the
readable expression was retained.

The expanded production-method TouchButton/Layout smoke checks cap
and pattern IDs, dimensions, flags, title text coordinates, and the
help-button branches for no transition, transition, and an open
choice window. Native `gof2`, all 206 ARM units and 37 verifier tests
pass (one skipped).

Full ARM corpus: 4,718 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.93% average source-shape and 81.33% strict
fuzzy, 2,399 linked-exact and 1,008 byte-exact. Complete
`(unit,symbol)` comparison with the preceding report shows no
linked- or byte-exact gains or losses, and the 123 extra symbols
are unchanged. Reports:
`A:/Galaxy on Fire new version/_work/layout_draw_header_before_20261013.json`,
`layout_draw_header_after_20261013.json`, and
`layout_draw_header_full_20261013.json`. Real assets, visual output
and ARM32 runtime remain untested. Next priority:
`Layout::drawWindow(String,bool)`.
