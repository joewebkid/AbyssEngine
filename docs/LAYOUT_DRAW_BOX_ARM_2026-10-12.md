# Layout drawBox styles and text geometry — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `Layout::drawBox(int,int,int,int,int,String,unsigned char)`
at ELF `0xd4384` renders styles 0–10 through `Globals::Canvas`. Its
text paths use `Globals::font`; the default text inset comes from
`Globals::layout + 0x44`, while style 0/1 baselines use layout fields
`+0x1c/+0x5c`, and style 7 uses `+0x8/+0x28`. The source previously
read separate never-set null canvas, metric, and font holders. The
confirmed singleton and field accesses now replace those holders.

For styles 5 and 8, ARM forwards the caller's `h` to
`drawBGBorder`. The local code instead measured the corner image and
passed that image height, changing the box geometry. Both styles now
forward `h`. The other style/image selections and color restoration
remain in place. The label flags preserve their ARM roles: bit 1 for
right alignment, bit 2 for centering.

The host UI smoke extracts the production `drawBox` bodies and tests
all 11 styles with fixture resources. It checks frame selection,
left/right/center text coordinates, style-specific baselines, styles
5/8 forwarding the requested height, style 7's border geometry, and
saved-color restoration. Its `drawBGBorder` dependency is a fixture
recorder, not the production implementation.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206
  translation units. Verifier suite: 37 run, 36 pass, one skipped.
- Focused two-symbol comparison: the main renderer rises from 60.1%
  to 95.4% source-shape and 21.4% to 37.9% strict fuzzy. The
  six-argument overload stays 79.2% / 50.0%. Neither is exact.
  Reports:
  `A:/Galaxy on Fire new version/_work/layout_draw_box_before_20261012.json`
  and
  `A:/Galaxy on Fire new version/_work/layout_draw_box_after_focused_20261012.json`.
- Host smoke: `TouchButton/Layout host smoke: OK`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped or wrong-type. Average
  source-shape rises 92.52% to 92.53% and strict fuzzy 80.79% to
  80.80%. Linked-exact remains 2,376 and byte-exact remains 1,008.
  The per-symbol exact sets are unchanged; no exact pair was lost.
  Report:
  `A:/Galaxy on Fire new version/_work/layout_draw_box_full_20261012.json`.

Original assets, real font metrics, and full-game rendering remain
untested. At this snapshot, styles 5, 7, and 8 called a
`Layout::drawBGBorder` body with a null canvas holder, so the fixture
could not establish end-to-end border behavior.

Subsequent work replaced that border fixture with the production body;
see [the border audit](LAYOUT_BG_BORDER_ARM_2026-10-12.md). The shared
canvas region renderer remains a separate runtime limitation.
