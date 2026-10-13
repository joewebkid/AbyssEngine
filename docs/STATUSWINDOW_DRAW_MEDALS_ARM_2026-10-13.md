# StatusWindow medal grid and detail pane — Android ARM audit (2026-10-13)

Scope: the medal-grid and selected-medal detail block inside
`StatusWindow::draw()` at Android 2.0.16 ELF address `0x15831c`.
The IDA reconstruction at
`analysis/gof2_libgof2hdaa_full_ida.c:203961-204027` and the original
ARM instructions around `0x1597be-0x1599bc` provide the primary evidence.

The medal header on iPad uses `top + scrollOffset`, not the unscrolled
top. The original calls `TouchButton::setPosition(int,int)`, preserving
each button's existing flags; the local code instead called the
three-argument overload with `0x44`. The selected-medal text starts at
`screenH - layout[0x10] - layout[0x24] - layout[0x2c] - lineHeight *
lineCount` and passes `false` for centering to the five-argument
`Globals::drawLines`. The former four-argument call truncated `screenH`
to a byte and could place the text far outside its panel.

A production-block host smoke checks both iPad and phone medal positions,
visibility culling, the iPad header, selected-medal panels and the
`drawLines` call. Native `gof2`, all 206 ARM translation units and 37
verifier tests pass (one skipped). Focused `draw` source-shape moves from
84.4% to 84.6%; strict fuzzy moves from 21.8% to 21.4%. The latter is
a small regression in an approximate shape measure, not an exact-symbol
loss. The 22-row `StatusWindow` family remains 90.7% source-shape and
74.4% strict fuzzy at report precision, seven linked-exact and one
byte-exact. The edited `draw` remains non-exact.

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.82% average source-shape and 81.21% strict
fuzzy at report precision, 2,395 linked-exact and 1,008 byte-exact.
Complete `(unit,symbol)` exact-set comparisons against the preceding
package show no gains or losses; the extra-symbol set stays at 123.
The report is `A:/Galaxy on Fire new version/_work/status_draw_medals_full_20261013.json`.
The smoke uses fixture text and services, not original assets or a
complete UI frame. ARM32 runtime,
interactive selection, and actual rendering are unverified. The final
header/footer calls already follow the Android order but do not have a
separate runtime check. Next priority: `StatusWindow` construction and
its unassigned global-holder fields.
