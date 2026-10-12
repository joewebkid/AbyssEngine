# TouchButton image-handle measurements — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `TouchButton::init` at ELF `0x168598` measures actual handles
after creating images. Its medal branch uses `iconImage`; the framed
special cases use `imgFrameL` for the button body and the `subId` image
for icon centering; the normal frame branch uses `imgFrameTL`, `imgFrameT`,
and `imgFrameTR` for height/left, middle, and right dimensions. The
wide-text branch also measures `imgFrameL`. The local source instead
passed zero in all these calls. For kinds `0x10` and `0x13`, the original
also writes `subId` into `imgFrameL`, not the medal argument.

Android `TouchButton::setText` at ELF `0x1691b0` similarly measures
`subId` when an image is present and uses `Globals::Canvas`. The local
source passed zero and read through the separate `PaintCanvas::gCanvas`
holder. The call contracts and existing control flow are otherwise kept.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206 translation
  units. Verifier suite: 37 run, 36 passed, one skipped.
- Focused `TouchButton` comparison: 44 symbols, 29 linked-exact and 14
  byte-exact. `init` rises from 79.8% to 80.7% source-shape and from
  28.2% to 32.3% strict fuzzy. `setText` rises from 93.9% to 95.6%
  source-shape and from 75.0% to 80.2% strict fuzzy. Neither is exact.
- Focused report:
  `A:/Galaxy on Fire new version/_work/touchbutton_handles_focused_20261012.json`.
- Full corpus: 4,714 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. The rounded averages remain
  92.41% source-shape and 80.67% strict fuzzy; linked-exact stays 2,370
  and byte-exact stays 1,008. Comparing symbol sets against the prior
  full report found no exact-pair gains or losses. Full report:
  `A:/Galaxy on Fire new version/_work/touchbutton_handles_full_20261012.json`.

This does not prove live UI behavior. `draw` still dereferences null
layout holders and uses an unresolved frame mask, and no standalone
native UI smoke was run. Recover that path from the original before
claiming a button can be rendered. The larger control-flow and
instruction-shape gap in `init` also remains for a later source-backed
pass.
