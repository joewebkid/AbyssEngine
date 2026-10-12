# Globals getBoundedString — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 function at ELF address `0xe469c` first copies the input
string. At `0xe46c4`–`0xe46dc` it measures the input through
`Globals::Canvas->GetTextWidth(Globals::font, text)` and truncates only if
the measured width exceeds the requested width. At `0xe46f8` it passes
`width - 3` to `getLine`; `0xe470c`–`0xe4726` constructs the `"..."`
literal and concatenates it after the shortened line. The literal's
`.rodata` bytes are `2e 2e 2e 00` at `0x1cb165`. The temporary heap `String`
is destroyed through its virtual destructor at `0xe4736`–`0xe473e`.

The recovered source instead dereferenced three null placeholders, measured
an invalid font/text pair, prefixed an empty string, and never deleted the
temporary heap object. These were replaced with the ARM-confirmed globals,
suffix order, and deletion. No in-tree caller beyond this function's own
declaration was found, so no caller changed.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Globals` comparison: 49 functions; ten linked-exact and one
  byte-exact retained. `getBoundedString` source-shape rose 82.2% to
  100.0%, strict fuzzy 37.8% to 91.8%; it remains non-exact.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type; 92.33% average source-shape,
  80.48% strict fuzzy, 2,353 linked-exact, and 1,008 byte-exact. A
  per-symbol comparison with the preceding full report found only this
  function's metrics changed and no exact-pair losses. Report:
  `A:/Galaxy on Fire new version/_work/globals_bounded_full_20261012.json`.

No live UI or text-width behavior was exercised. The native target is a
static library, not a runnable game. Next priority: audit the `drawLines`
overloads, which still contain null canvas/line-height placeholders.
