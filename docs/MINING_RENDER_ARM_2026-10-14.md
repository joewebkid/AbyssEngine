# MiningGame render2D service and call recovery (2026-10-14)

## Evidence and recovery

Android 2.0.16 `MiningGame::render2D()` starts at ELF `0x11f12c` in
the shared-workspace IDA export
`analysis/gof2_libgof2hdaa_full_ida.c`, lines 157161–157398. The
source now reads `LAYER_DIAMETERS`, `Globals::layout`,
`Globals::Canvas`, `Globals::status`, `Globals::font`,
`Globals::gameText` and `Globals::w` directly instead of unassigned
holders. The four ring quadrants call the eight-argument canvas image
method; core, ore icon and progress label call the five-argument
method with separate alignment and placement flags. The previous
packed `0x4411`/`0x2411` values were truncated to one byte by a
different overload.

The ore label now uses the original integer amount plus `"t"`,
the canvas font ID rather than a cast `String*`, and the original
`Status::getShip()->getFreeSpace()` route. The warning colors use
`Layout::getPulseValue`; the campaign prompt uses key 616 and a
1,000-tick pulse divisor, not the former 2,500-tick divisor. These
changes repair actual rendering parameters as well as code shape.

Focused ARM `render2D` comparison rises from **25.6% source-shape /
14.5% strict fuzzy** (59 generated versus 464 target instructions)
to **97.4% / 78.9%** (454 versus 464). It is neither linked-exact
nor byte-exact. The production-method host smoke checks even and odd
ring choices, four quadrant flags, image placement flags, progress
width, warning and ore colors, drill/marquee positions, ore text and
prompt text/positions, and the no-prompt campaign branch. The fixture
uses service and string doubles; it does not render game assets or
prove original ARM32 visual/runtime behavior.

Native `gof2`, the constructor/layer/update smokes and 37 verifier
tests pass (one skipped). A complete native mining sequence, including
the update's status-loss and achievement-medal branches and resource
ownership on teardown, remains untested. Those are the next
priorities.

The full ARM corpus has 4,718 comparisons and 4,323/4,524 original
functions covered (201 missing). Average source-shape rises from
93.06% to 93.08% and strict fuzzy from 81.48% to 81.50%. All 2,403
linked-exact and 1,008 byte-exact pairs are preserved with no gains
or losses; the 123-symbol extra set is unchanged.
