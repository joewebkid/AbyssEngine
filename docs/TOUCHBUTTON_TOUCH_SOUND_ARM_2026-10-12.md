# TouchButton touch sound and text width — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `TouchButton::OnTouchBegin` at ELF `0x1699d8` plays
sound `0x7c` through `Globals::sound` after a successful hit test;
`OnTouchEnd` at `0x169a54` plays `0x7b` on a successful release.
The local source routed both through a null static holder. Both now use
the original singleton without changing the hit-test, visibility,
transparency, or touch-state branches.

Android `TouchButton::replaceTextKeepSize` at `0x169304` measures the
new argument's width through `Globals::Canvas`. The local source read
the separate `PaintCanvas::gCanvas` holder after assigning the text;
it now uses the original canvas and argument. These edits remove the
last static null holder from `TouchButton.cpp`.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206 translation
  units. Verifier suite: 37 run, 36 passed, one skipped.
- Focused `TouchButton` comparison: 44 symbols. `OnTouchBegin` rises
  from 92.1% source-shape / 83.3% strict fuzzy to 100% / 100%; `OnTouchEnd`
  from 93.5% / 78.0% to 100% / 100%; `replaceTextKeepSize` from
  98.5% / 92.3% to 100% / 100%. All three newly become linked-exact,
  not byte-exact. The focused unit now has 32 linked-exact and 14
  byte-exact symbols. Report:
  `A:/Galaxy on Fire new version/_work/touchbutton_sound_text_focused_20261012.json`.
- Full corpus: 4,714 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. Average source-shape rises
  92.41% to 92.42%; strict fuzzy rises 80.68% to 80.69%.
  Linked-exact rises 2,370 to 2,373; byte-exact stays 1,008.
  Per-symbol comparison found exactly the three new linked pairs above
  and no prior exact-pair losses. Full report:
  `A:/Galaxy on Fire new version/_work/touchbutton_sound_text_full_20261012.json`.

No live audio/touch event or native UI runtime test was performed.
The matched call sequence does not prove that platform sound services,
game assets, or event routing are initialized in a runnable host.
