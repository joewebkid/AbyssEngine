# Layout framed window — Android ARM audit (2026-10-13)

Scope: `Layout::drawWindow(String,int,int,int,int,bool)`
(`_ZN6Layout10drawWindowEN11AbyssEngine6StringEiiiib`) in the
Android 2.0.16 ELF at `0xd3780`. Primary evidence is the original
Thumb disassembly and `analysis/gof2_libgof2hdaa_full_ida.c:129997-130037`
in the shared research directory. No iOS/PC implementation was used.

The local function dereferenced unassigned holders for canvas,
layout metrics and font, supplied a pointer value as a color, and
placed the header image at screen `y=0`. Android saves the current
canvas color, optionally paints the background below layout inset
`0x8`, outlines the same rectangle in `0x255d8dff`, restores the
layout draw color, draws the header image at the caller's `(x,y)`,
then restores the saved canvas color. It reads the live
`Globals::Canvas`, `Globals::layout` and `Globals::font` services.

The original title branch requires nonzero `String::length` and
`Compare("") == 0`. The ELF byte at the referenced literal
`0x1ca472` is zero; the original `String::Compare(const char*)`
also returns nonzero for an ordinary nonempty title compared with
`""`. The branch therefore appears unreachable for ordinary
titles, but this recovery preserves the original condition rather
than inventing title behavior. Any effect of malformed strings with
embedded NUL or another platform build remains unverified.

Focused verification rises from 24.0% source-shape / 17.5% strict
fuzzy to 100.0% / 100.0%. It is neither linked- nor byte-exact.
The production-method TouchButton/Layout host smoke tests both
background branches, frame geometry and color, header placement,
ordinary title behavior and final color restoration. Native `gof2`,
all 206 ARM units and 37 verifier tests pass (one skipped).

Full ARM corpus: 4,718 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.92% average source-shape and 81.32% strict
fuzzy, 2,399 linked-exact and 1,008 byte-exact. Complete
`(unit,symbol)` comparison with the preceding report shows no
linked- or byte-exact gains or losses, and the 123 extra symbols
are unchanged. Reports:
`A:/Galaxy on Fire new version/_work/layout_draw_window_before_20261013.json`,
`layout_draw_window_after_20261013.json`, and
`layout_draw_window_full_20261013.json`. Real assets, visual output
and ARM32 runtime remain untested. Next priority:
`Layout::drawHeader(String,bool)`.
