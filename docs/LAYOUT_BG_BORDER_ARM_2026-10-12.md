# Layout border tiling — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `Layout::drawBGBorder(unsigned,unsigned,int,int,int,int,int,int)`
at ELF `0xd395c` reads image dimensions and draws every segment through
`Globals::Canvas`. The local body instead dereferenced a never-set null
canvas holder. It also sent zero rotation and zero vertical coordinates to
`DrawRegion2D`, and displaced the left edge by the corner inset. The body
now uses the live canvas and the original corner, horizontal-remainder,
vertical-edge, and vertical-remainder coordinates and angles (`3.1416f`,
`-1.5708f`, `1.5708f`). The six-argument overload at `0xd393c` is unchanged.

The original vertical loop advances its position by the *whole covered
vertical span* after each pair of edge calls, not by one edge width. This
counterintuitive step is present in both the IDA decompilation and the ARM
instructions, so the source preserves it; the host test checks two rows.

The host UI smoke now extracts and executes the production border body
instead of replacing it with a recorder. Fixture calls check the four
corners, full and partial horizontal/vertical segments, rotations, inset
and pad placement, and `drawBox` styles 5, 7, and 8 that invoke the border.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206 translation
  units. Host smoke: `TouchButton/Layout host smoke: OK`. Verifier suite:
  37 run, 36 pass, one skipped.
- Focused border comparison: the eight-argument body rises from 88.6% to
  94.2% source-shape. Strict fuzzy falls from 13.4% to 11.1%; it is not
  exact. The six-argument overload stays 100% source-shape, 100% fuzzy,
  and linked-exact. Reports:
  `A:/Galaxy on Fire new version/_work/layout_bg_border_before_20261012.json`
  and `A:/Galaxy on Fire new version/_work/layout_bg_border_focused_20261012.json`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  changes from 92.5295% to 92.5307%; strict fuzzy changes from 80.7983%
  to 80.7978%. Both still round to 92.53% and 80.80% respectively.
  Linked-exact remains 2,376 and byte-exact remains 1,008; comparing
  per-symbol sets found no gained or lost exact pairs. Full report:
  `A:/Galaxy on Fire new version/_work/layout_bg_border_full_20261012.json`.

The smoke verifies call parameters against fixture dimensions, not original
assets or screen pixels. The current `PaintCanvas::DrawRegion2D` body at
`src/engine/render/PaintCanvas.cpp` still marks its float angle parameter
unused, whereas the original ARM callee at ELF `0x78538` passes that angle
to sine and cosine. Thus real rotated-edge rendering and full-game/ARM
runtime behavior remain unverified. Recover and test that shared canvas
callee next before claiming border rendering works end to end.
