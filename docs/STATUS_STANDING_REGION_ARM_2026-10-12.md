# StatusWindow standing bars — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `StatusWindow::draw` at ELF `0x15831c` places the
standing row after a second stats header and paired boxes. Its bar
baseline is derived from that row, the character-image height, and
half the bar height. The local source drew both emblems, bars, and
frames at the earlier first-row `y`, passed each standing rate as a
rotation angle, shifted X into Y, and omitted the destination Y.

The source now computes the standing row and bar/frame baselines from
the ARM field offsets. Both bars pass a zero angle, zero pivots, and
their actual X/Y. Emblems use the original `0x11/0x14` flags and frames
use `0x11/0x44`. The image and bar IDs, crop width, and two standing
rate queries are unchanged. This corrects only the standing-bar block,
not the much larger `StatusWindow::draw` body.

The host smoke extracts that production block and checks both positive
and negative standing rates, an odd character-image height, both
positions, image flags, crop geometry, and zero-angle region calls.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206 translation
  units. Host smoke: `StatusWindow standing host smoke: OK`.
  Verifier suite: 37 run, 36 pass, one skipped.
- Focused `StatusWindow::draw` stays at 2.6% source-shape / 1.4% strict
  fuzzy before and after; the restored block is a small part of a
  much larger non-exact function. Reports:
  `A:/Galaxy on Fire new version/_work/status_standing_before_20261012.json`
  and `A:/Galaxy on Fire new version/_work/status_standing_after_20261012.json`.

- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  remains 92.5338% and strict fuzzy 80.8033% (rounded 92.53% /
  80.80%). Linked-exact remains 2,376 and byte-exact remains 1,008;
  comparing the per-symbol exact sets found no gain or loss. Full report:
  `A:/Galaxy on Fire new version/_work/status_standing_full_20261012.json`.

The fixture does not run the whole window or original assets. The
larger method still has never-set service holders, omitted intermediate
UI drawing, and other unverified text/layout paths. This block does
not establish end-to-end StatusWindow behavior. With the known
nonzero-angle caller mistakes in MiningGame, PlayerEgo, and this window
corrected, the shared `PaintCanvas::DrawRegion2D` transform path is the
next bounded audit; full-game and ARM runtime remain untested.
