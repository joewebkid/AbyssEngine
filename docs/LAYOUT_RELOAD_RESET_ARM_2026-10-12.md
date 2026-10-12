# Layout reload/resetWindowDimensions — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 functions at ELF addresses `0xd2c20` and `0xd3048`
respectively reload layout images/buttons and reset window dimensions.
The IDA dump identifies their global sources, and the ARM functions
confirm the resource IDs, branches, calls, offsets, and output writes.

`reload` now uses `Globals::Canvas` for image creation, `Globals::iPad`
for the alternative image set, `Globals::gameText` with IDs 170 and 171
for back-button labels, `Globals::h`/`w` for placement, and
`Globals::font` for text height. The three buttons use the existing typed
constructors instead of unresolved varargs shims. The reset tail now
also clears the two adjacent flags at offsets `0x2ed` and `0x401`.

`resetWindowDimensions` now uses `Globals::w` and `h`, reads the inset
from `Globals::layout + 0x28`, and writes the first coordinates of
`Globals::other_buttons_x/y`. Its prior null global holders were
unusable. The function is called by `reload` and by other UI paths;
its signature and callers remain unchanged.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Layout` comparison: 54 functions. `reload` rises from 71.8%
  to 90.4% source-shape and 47.5% to 56.2% strict fuzzy;
  `resetWindowDimensions` rises from 21.6% to 77.8% source-shape and
  4.2% to 57.3% strict fuzzy. Neither is linked- or byte-exact.
- Full corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped or wrong-type. Average source-shape
  rises 92.37% to 92.39%; strict fuzzy rises 80.57% to 80.58%.
  Linked-exact remains 2,360 and byte-exact remains 1,008. Per-symbol
  comparison found no exact-pair losses. Report:
  `A:/Galaxy on Fire new version/_work/layout_reload_reset_full_20261012.json`.

No live resource reload was run. `TouchButton` constructors and `init`,
which this path calls, still use null holders for font/layout/skin data;
the native target is only a static library. Recover those dependencies
before claiming the full UI path works at runtime. Do not alter the
ARM-confirmed device branch or text IDs merely to raise fuzzy matching.
