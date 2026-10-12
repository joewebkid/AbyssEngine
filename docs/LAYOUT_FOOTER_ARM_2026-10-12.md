# Layout footer routing and global bindings — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `Layout::drawFooter(bool,bool)` at ELF `0xd3c3c` is the
single footer renderer. The original `drawFooter()` (`0xd3fb4`),
`drawFooterNoBackButton()` (`0xd4090`), and `drawFooterStation()`
(`0xd4098`) forward `(false,true)`, `(false,false)`, and `(true,false)`
to it. The local source had three full copies of the renderer, all
using never-set null canvas/metric/font holders. They now forward to
the common method like ARM.

The common method reads `Globals::Canvas`, `Globals::layout + 0x10`
for footer height, `Globals::layout + 0x74` for the station credits
inset, `Globals::status` for ship load and credits, and `Globals::font`
for both labels. Its cargo warning tint is `0xff2a00ff`; the load
string joins current and maximum load with `" / "` and suffix `"t"`.
The left image uses the ARM `0x11,0x21` alignment flags. In
no-back-button mode neither button draws; in station mode the
secondary button draws. The previous copied code incorrectly drew
that secondary button in both modes.

## Verification and limits

- Native `gof2` static-library build and 206/206 ARM translation units:
  pass. Verifier suite: 37 run, 36 pass, one skipped.
- Focused four-symbol comparison: wrappers rise from 4.0% source-shape
  / 0% strict fuzzy to 100% / 100% each; the shared renderer rises from
  11.3% / 6.8% to 93.4% / 34.0%. None is linked- or byte-exact.
  Reports:
  `A:/Galaxy on Fire new version/_work/layout_footer_before_20261012.json`
  and `A:/Galaxy on Fire new version/_work/layout_footer_after_focused_20261012.json`.
- The native UI host smoke executes the unchanged production footer
  bodies with fixture renderer, buttons, status, ship, and strings. It
  checks all three routing modes, load/credits labels, coordinates,
  warning tint, and image flags: `TouchButton/Layout host smoke: OK`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped or wrong-type. Average
  source-shape rises 92.42% to 92.50% and strict fuzzy 80.68% to
  80.75%. Linked-exact remains 2,373 and byte-exact remains 1,008;
  per-symbol exact-set comparison found no gains or losses. Report:
  `A:/Galaxy on Fire new version/_work/layout_footer_full_20261012.json`.

The fixture does not execute original `String` internals, currency
formatting, real canvas resources, or a full game UI. Actual screen
rendering, frame timing, and platform behavior remain untested.
`Layout::getFooterTransitionWidth` still uses a null canvas holder and
is the next related branch to audit.
