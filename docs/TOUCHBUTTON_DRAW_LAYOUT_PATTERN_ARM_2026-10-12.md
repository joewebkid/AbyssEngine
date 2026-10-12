# TouchButton draw, hit inset, and Layout pattern — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `TouchButton::draw` at ELF `0x1693d8` reads
`Globals::Canvas` and `Globals::layout`. Its three-slice mask is the
literal `0x001ef400` at `0x16991c`: kinds whose bit is **clear** draw the
middle pattern and right cap. The local source had a zero mask, the
opposite branch, and null layout holders.

The same ARM function supplies the medal's `iconTexId` tint, distinct
pressed overlay coordinates, a `0xffffff80` progress tint, and the
actual button `x/y` to `DrawRegion2D`. Text uses `textColor` (or a fixed
translucent alpha), split-text width comes from `splitText`, and the
number label uses `Globals::font` with its own gray tint. When the
button has a `subId` image, that image is drawn at the computed text
offset rather than reusing a frame image.

`TouchButton::touchedInside` at ELF `0x169938` reads the kind-3 hit inset
from `Globals::layout + 0x38`, not from the canvas engine pointer.
`Layout::drawBGPattern` at ELF `0xd3574`, called by `draw` and other
layout methods, reads `Globals::Canvas` for every image operation; its
previous null canvas holder has been removed. No original assets or
binary content were added.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units. Focused `TouchButton` comparison:
  44 symbols, 29 linked-exact and 14 byte-exact. `draw` rises from 85.9%
  to 93.3% source-shape and from 19.8% to 33.1% strict fuzzy;
  `touchedInside` rises from 90.2% to 90.6% source-shape and from 63.0%
  to 64.8% strict fuzzy. Neither is exact.
- Focused `Layout` comparison: 54 symbols. `drawBGPattern` rises from
  93.6% to 94.6% source-shape and from 32.0% to 45.9% strict fuzzy;
  it is not exact. Focused reports are
  `A:/Galaxy on Fire new version/_work/touchbutton_draw_hit_focused_20261012.json`
  and `A:/Galaxy on Fire new version/_work/layout_pattern_focused_20261012.json`.
- The verifier suite runs 37 tests: 36 pass and one is skipped.
- Full corpus: 4,714 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. Rounded average source-shape
  remains 92.41%; strict fuzzy rises 80.67% to 80.68%. Linked-exact
  remains 2,370 and byte-exact remains 1,008. Comparing the exact
  symbol sets against the previous full report found no losses or gains.
  Final report:
  `A:/Galaxy on Fire new version/_work/touchbutton_draw_layout_full_20261012.json`.

No live UI rendering or touch test has been run. These source-backed
fixes remove immediate null-holder and argument faults but do not prove
the full game UI works. `draw`, `touchedInside`, and `drawBGPattern`
still have non-exact instruction shapes; examine their remaining
branches and try a focused native UI smoke before claiming runtime
readiness.
