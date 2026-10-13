# StatusWindow draw globals — Android ARM audit (2026-10-13)

Scope: the shared-service bindings and text IDs in `StatusWindow::draw()`
at Android 2.0.16 ELF address `0x15831c`. The original ELF and IDA
reconstruction identify `Globals::Canvas`, `layout`, `w`, `h`, `iPad`,
`font`, `imageFactory`, `gameText`, `globals`, and `status` reads. The
previous local C++ instead dereferenced nine unassigned holders. Under
the matching ARM compiler, almost the whole method collapsed: 38 local
instructions against 2,324 in the original. This was a behavior failure,
not merely a low similarity score.

The holders are removed, and the method reads the corresponding shared
globals. Android text IDs restore the visible stats/medal/header labels
used by the existing local structure: 1597, 321, the six displayed stat
labels (568, 176, 559, 557, 564, 558), 168 and 169. Ship name text uses
`shipIndex + 913`. The playing-time formatter now writes the string that
is measured and drawn, rather than an unused temporary. The Android
method draws more fields than this local six-row loop; these IDs do not
make that loop a complete reconstruction.

A host smoke extracts the production draw setup through box-width
calculation and checks canvas, layout, scroll-bar and iPad/non-iPad paths
with fixtures. The existing standing-bar smoke was updated to the shared
status and still passes. Native `gof2`, all 206 ARM translation units,
and 37 verifier tests pass (one skipped). `draw` rises from 2.6% to
57.4% source-shape, and 1.4% to 13.8% strict fuzzy; local instructions
rise from 38 to 1,047 against 2,324 original. It is not exact. The
22-row `StatusWindow` family rises from 87.01% to 89.50% source-shape
and 73.50% to 74.06% strict fuzzy, retaining seven linked-exact and
one byte-exact symbols.

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.82% average source-shape and 81.21% strict
fuzzy, 2,395 linked-exact and 1,008 byte-exact. Complete `(unit,symbol)`
exact-set comparisons against the prior package find no gains or losses.

The smoke only exercises the draw prefix, not actual game assets or the
whole UI. The remaining stats rows, level/time spacing and positions,
ship/firepower formatting, medal detail layout, and ARM32 runtime remain
unverified; `draw` is still about half the original source shape. Next
priority: recover the stats/ship text and row geometry from the ARM body
in a separate bounded package.
