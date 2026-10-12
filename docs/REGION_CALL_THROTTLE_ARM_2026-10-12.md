# Mining bar and PlayerEgo throttle region calls — Android ARM audit (2026-10-12)

## Evidence and corrections

Android 2.0.16 `MiningGame::render2D` at ELF `0x11f12c` calls the
ten-argument `PaintCanvas::DrawRegion2D` with a zero rotation and the
progress bar's X/Y as its last two arguments. The local source passed
the computed width as rotation, moved X into Y, and omitted Y. Those
arguments now follow the original. This does not repair the method's
other unrecovered null service holders.

Android `PlayerEgo::drawThrottle` at ELF `0xa1f04` reads the live
`Globals::Canvas`, `Globals::font`, and `PlayerEgo::crosshairPos`. It
constructs the percentage `String` from the thrust value, draws the
cropped gauge at the crosshair offset with zero rotation, draws that
string at its measured width, and restores white canvas color. The
local method instead dereferenced never-set anchor/text holders and
shifted the region/text arguments into the wrong parameters. The
method now follows the Android call contract and removes those two
null holders.

The host smoke extracts the unchanged production `drawThrottle` body
and checks the inactive branch, color, gauge crop/position, zero angle,
percentage text, font, and restoration through fixture services.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206
  translation units. Host smoke: `PlayerEgo throttle host smoke: OK`.
  Verifier suite: 37 run, 36 pass, one skipped.
- `PlayerEgo::drawThrottle` rises from 79.2% to 94.0% source-shape and
  from 31.2% to 57.1% strict fuzzy. `MiningGame::render2D` remains
  25.6% source-shape / 14.5% strict fuzzy; this call correction is too
  small against that much larger, still-incomplete function to change
  its rounded score. Neither function is exact. Focused reports:
  `A:/Galaxy on Fire new version/_work/region_call_throttle_before_20261012.json`,
  `A:/Galaxy on Fire new version/_work/region_call_throttle_final_20261012.json`,
  `A:/Galaxy on Fire new version/_work/mining_region_call_before_20261012.json`,
  and `A:/Galaxy on Fire new version/_work/mining_region_call_after_20261012.json`.

- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  rises from 92.5307% to 92.5338%; strict fuzzy rises from 80.7978%
  to 80.8033%. Both still round to 92.53% and 80.80% respectively.
  Linked-exact remains 2,376 and byte-exact remains 1,008. The exact
  per-symbol sets are unchanged. Full report:
  `A:/Galaxy on Fire new version/_work/region_call_throttle_full_20261012.json`.

The host smoke is not a real canvas, original-asset, or ARM runtime test.
`MiningGame::render2D` still has null service holders, and
`StatusWindow::draw` still misplaces both standing-bar region calls;
the latter's row coordinates require a separate source audit. The
shared `PaintCanvas::DrawRegion2D` callee itself still ignores its
rotation argument. Audit the remaining caller before changing that
shared renderer, then test the renderer's transform path.
