# TouchButton/Layout host smoke and pattern remainder — 2026-10-12

## What was checked

`tools/run_touchbutton_touch_smoke.py` extracts the unchanged production
method bodies for `TouchButton::touchedInside`, `OnTouchBegin`, `OnTouchMove`,
`OnTouchEnd`, `draw`, `Layout::setDrawColor`, `Layout::drawBGPattern`, and
`Layout::drawFade`.
The generated source is compiled with fixture canvas, layout, and sound
services, then run in an automatically removed temporary directory.
It tests touch bounds and state transitions, sound call order, three-slice
and progress drawing, text position, overlays, partial pattern tiles, and
both fade/full-screen fill branches.

Fade assertions were added after the original pattern audit; their
ARM evidence and current metrics are recorded in
[the Layout fade audit](LAYOUT_DRAW_FADE_ARM_2026-10-12.md).

The first expanded host run exposed a real coordinate error in
`Layout::drawBGPattern`: the right partial-width strip started at
`y + fullH`, overlapping the bottom remainder, instead of at `y`.
Android 2.0.16 `Layout::drawBGPattern` at ELF `0xd3574` initializes that
strip's vertical cursor from the input `y` and increments it by the image
height. The source now follows that behavior. The smoke passed after the
one-line correction.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units. Focused `Layout` comparison:
  `drawBGPattern` source-shape 94.6% to 95.7%; strict fuzzy falls from
  45.9% to 22.7%. The source-backed behavior correction does not make
  it exact. Focused report:
  `A:/Galaxy on Fire new version/_work/layout_pattern_partial_tile_focused_20261012.json`.
- Host smoke: pass, `TouchButton/Layout host smoke: OK`.
- Verifier suite: 37 tests run, 36 pass and one is skipped.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  stays 92.42%; strict fuzzy rounds down from 80.69% to 80.68%.
  Linked-exact remains 2,373 and byte-exact remains 1,008. Per-symbol
  exact-pair sets were compared with the preceding full report: no gains
  or losses. Full report:
  `A:/Galaxy on Fire new version/_work/layout_pattern_partial_tile_full_20261012.json`.

The fixture checks these production method bodies, not original constructors,
real renderer/audio initialization, event routing, original game assets, or
ARM execution. A full UI/game runtime pass is still needed before claiming
end-to-end behavior.
