# Globals drawLines family — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 functions at ELF addresses `0xe458c` (centered flag),
`0xe4608` (right-edge argument), and `0xe4688` (short wrapper) use
`Globals::Canvas` for text measurement and drawing. After every row, the
two drawing overloads load `Globals::layout` and add its field at offset
`+4` to the y coordinate (`0xe45e6`–`0xe45f0` and `0xe4668`–`0xe4672`).
The short wrapper passes `false` to the first overload.

The recovered drawing overloads instead dereferenced null canvas and
line-height placeholders. Both now use the original globals. The obsolete
local `LineMetrics` type and its assertion were removed; `Layout` already
exposes the same `+4` field. The original horizontal alignment branches
were retained. In-tree callers include `StatusWindow`, `Radio`, `MGame`,
and `Layout`.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Globals` comparison: 49 functions. The centered overload rose
  from 60.7% to 100% source-shape and from 13.5% to 100% strict fuzzy;
  the right-edge overload rose from 67.1% to 100% source-shape and from
  13.2% to 100% strict fuzzy. Both became linked-exact, bringing the
  unit from ten to twelve linked-exact matches. Neither is byte-exact.
  The short wrapper remained linked-exact.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. Average source-shape rose
  92.33% to 92.35%, strict fuzzy 80.48% to 80.52%, and linked-exact
  2,353 to 2,355; byte-exact stayed 1,008. Per-symbol comparison found
  only the two intended functions changed and zero exact-pair losses.
  Report: `A:/Galaxy on Fire new version/_work/globals_drawlines_full_20261012.json`.

No live rendering run was performed; the native target is a static library.
Next priority: `getRandomEnemyFighter`, whose current recovered source has
null RNG/table placeholders and many gameplay callers.
