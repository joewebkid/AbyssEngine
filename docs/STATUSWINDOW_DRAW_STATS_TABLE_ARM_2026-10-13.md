# StatusWindow standing and statistics table — Android ARM audit (2026-10-13)

Scope: the standing section and two-column statistics table inside
`StatusWindow::draw()` at Android 2.0.16 ELF address `0x15831c`.
The primary source is the ARM binary, cross-read with the IDA reconstruction
at `analysis/gof2_libgof2hdaa_full_ida.c:203412-203953`. Direct ARM
instructions around `0x158a2c-0x158ad0` and `0x158e54-0x158ef2` confirm
the section box coordinates, widths, and heights omitted by the IDA call
rendering. No iOS/PC implementation was used for this package.

The former code drew neither section header or box, omitted four standing
labels, and flattened statistics into six rows at the same `y` and an
unrelated value alignment. Android draws standing labels 406–409 around
the existing faction images, then header 577 and two boxed columns.
The left column has six label/value rows (568, 176, 552, 559, 557, 3235);
the right has five (564, 558, 560, 561, 567). Labels append the localized
separator; values align to their column's right inset. Direct raw `Status`
fields at ARM offsets `0xd8`, `0x118`, `0xa0`, `0xa4`, and `0xd4` supply
five rows not represented by the existing getters. Their gameplay names
are not inferred from the UI alone.

The new host smoke extracts both production table blocks and checks six
box calls, four standing labels, and all 11 statistic rows with coordinates.
The existing standing-bar and info-card smokes continue to pass, as do 37
verifier tests (one skipped). The smoke uses fixture text and services, not
original assets or a complete UI frame.

ARM focused verification: native `gof2` and all 206 translation units build.
`draw` rises from 60.1% to 84.4% source-shape and from 15.9% to 21.8%
strict fuzzy. The 22-row `StatusWindow` family averages 90.7% source-shape
and 74.4% strict fuzzy, with seven linked-exact and one byte-exact rows.
The edited `draw` has 1,728 local versus 2,324 original ARM instructions and
is not exact. Full ARM corpus: 4,720 comparisons, 4,323/4,524 original
functions covered, 201 missing, 92.82% average source-shape and 81.21%
strict fuzzy at report precision, 2,395 linked-exact and 1,008 byte-exact.
Complete `(unit,symbol)` exact-set comparisons against the preceding package
show no gains or losses; the extra-symbol set also remains unchanged at 123.
The report is `A:/Galaxy on Fire new version/_work/status_draw_stats_full_20261013.json`.

Unverified: real game text/assets, interactive display, ARM32 runtime, and
the remainder of the large `draw` function outside this table. Next audit:
the medal-grid/detail pane and final header/footer layout.
