# Layout scrollbar — Android ARM audit (2026-10-13)

Scope: `Layout::drawScrollBar(int,int,int,int,int)` at Android 2.0.16
ELF address `0xd4258`. The primary source is the Android ARM binary,
cross-read with `analysis/gof2_libgof2hdaa_full_ida.c:130553-130621`.
No iOS/PC implementation was used for this package. The method is shared
by `StatusWindow`, `WantedWindow`, `HangarWindow` and
`ScrollTouchWindow`.

The local method dereferenced unassigned static holders for the canvas
and track width, and used unrelated zero-valued color holders. Android
reads `Globals::Canvas`, uses `Globals::layout + 0x48` for the track
width, sets colors `0x111417ff` and `0x193043ff`, then restores the
layout draw color. The local method also drew the top cap at `y=0`;
Android draws it at the computed thumb Y. These service, color and
placement differences are behavior gaps, not merely compiler shape.
The existing thumb-size, offset/clamp and fill-pattern calculations were
kept because they already follow the Android branches.

A production-method host smoke checks the exact color order, global
track width, rectangle, fill pattern and both cap coordinates for long
and short thumb branches. The existing TouchButton/Layout host smoke,
native `gof2`, all 206 ARM translation units and 37 verifier tests pass
(one skipped). Focused source-shape rises from 39.1% to 93.1% and
strict fuzzy from 15.9% to 35.1%; 105 local versus 106 original ARM
instructions remain, so it is neither linked- nor byte-exact.

Full ARM corpus: 4,719 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.86% average source-shape and 81.24% strict
fuzzy, 2,397 linked-exact and 1,008 byte-exact. Complete
`(unit,symbol)` comparisons against the preceding package show no
exact-symbol gains or losses; the 123 extra-symbol set is unchanged.
The report is
`A:/Galaxy on Fire new version/_work/layout_scroll_full_20261013.json`.
Original texture assets, visual rendering and ARM32 runtime remain
unverified. Next priority: `Layout::drawTip` (12.9% source-shape, 108
original ARM instructions) and its currently thin local body, subject
to source-backed behavior review.
