# Layout empty footer — Android ARM audit (2026-10-13)

Scope: `Layout::drawEmptyFooter(bool)`
(`_ZN6Layout15drawEmptyFooterEb`) in the Android 2.0.16 ELF at
`0xd3fbc`. Primary evidence is the original Thumb disassembly and
`analysis/gof2_libgof2hdaa_full_ida.c:130413-130454` in the shared
research directory. No iOS/PC implementation was needed.

The local function dereferenced unassigned holders for color, screen
dimensions and footer height. It queried image width using that color,
omitted one image-placement flag and called an unrelated footer
animation shim instead of drawing the back button. Android sets
`this->drawColor`, measures `field_0x340`, draws `footerImageLeft`
at `(0, Globals::h)` with flags `0x11, 0x21`, fills the middle with
`footerPatternImage` using `Globals::layout->field_0x10`, then draws
the right cap at `(Globals::w - imageWidth, Globals::h - footerHeight)`.
Only when `showBack` is true does it set the back button visible and
draw it. It re-reads `Globals::Canvas` between calls; the recovery
keeps that behavior.

Focused verification rose from 43.0% source-shape / 20.5% strict
fuzzy to 100.0% / 100.0%, adding a linked-exact match but not a
byte-exact match. The existing TouchButton/Layout smoke now extracts
the production method and checks both `showBack` branches, image
selection and placement, flags, color and geometry. Native `gof2`,
all 206 ARM units and 37 verifier tests pass (one skipped).

Full ARM corpus: 4,718 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.90% average source-shape and 81.28% strict
fuzzy, 2,399 linked-exact and 1,008 byte-exact. Complete
`(unit,symbol)` comparison against the preceding report shows only
the new `drawEmptyFooter` linked-exact symbol; no linked- or
byte-exact symbols were lost, and the 123 extra symbols are unchanged.
Reports: `A:/Galaxy on Fire new version/_work/layout_empty_footer_before_20261013.json`,
`layout_empty_footer_after_20261013.json`, and
`layout_empty_footer_full_20261013.json`. Original assets, visual
rendering and ARM32 runtime remain untested. Next priority is the
bounded `Layout::drawWindow(String,int,int,int,int,bool)` path.
