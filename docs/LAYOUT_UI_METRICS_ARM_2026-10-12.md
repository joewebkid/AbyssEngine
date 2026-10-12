# Layout UI metrics, background, and mask — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `Layout::getFooterTransitionWidth` at ELF `0xd3c08`
measures the right and left footer images through `Globals::Canvas`.
`getHelpButtonOffset` at `0xd4bd0` subtracts `Globals::layout + 0x38`
from the help button width. Both local methods previously dereferenced
never-set null holders.

`Layout::drawBG` at `0xd3544` passes the live `Globals::w/h` to
`drawBGPattern`. The no-argument `drawMask` at `0xd3140` passes those
same dimensions to its rectangle overload. Their local source also
used null dimension holders. The rectangle overload at `0xd3168`
uses `Globals::Canvas`, saves/restores its color, and fills with the
fixed `0x80` tint; the local source used `PaintCanvas::gCanvas` and
the mutable layout draw color. All five paths now follow ARM.

The existing host UI smoke executes the unchanged production bodies
with fixture canvas, button, and layout globals. It checks width-call
order and help offset, partial background geometry, both mask routes,
the `0x80` tint, and color restoration. It does not load original
textures or display real frames.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206
  translation units. Verifier suite: 37 run, 36 pass, one skipped.
- Focused five-symbol family rises from 75.4% to 97.7% average
  source-shape and 46.8% to 86.7% strict fuzzy. `getHelpButtonOffset`,
  `drawBG`, and no-argument `drawMask` become linked-exact, not
  byte-exact. `getFooterTransitionWidth` reaches 94.6% source-shape
  and explicit `drawMask` 93.7%; neither is exact. Reports:
  `A:/Galaxy on Fire new version/_work/layout_ui_metrics_before_20261012.json`
  and
  `A:/Galaxy on Fire new version/_work/layout_ui_metrics_after_focused_20261012.json`.
- Host smoke: `TouchButton/Layout host smoke: OK`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  rises 92.50% to 92.52%; strict fuzzy rises 80.75% to 80.79%.
  Linked-exact rises 2,373 to 2,376 and byte-exact remains 1,008.
  Per-symbol comparison confirms exactly the three new linked pairs
  named above and no prior exact-pair loss. Report:
  `A:/Galaxy on Fire new version/_work/layout_ui_metrics_full_20261012.json`.

Real game UI rendering, assets, ARM runtime execution, and event
routing are not verified by this fixture.
`Layout::drawBox` still has separate null canvas, font, and metric
holders; it is the next UI branch to audit.
