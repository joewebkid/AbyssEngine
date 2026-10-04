# ChoiceWindow::setMedal Android ARM recovery (2026-10-04)

Scope: `ChoiceWindow::setMedal(int, int)` and its two 32-bit image-handle
fields. Primary reference: Android 2.0.16 `libgof2hdaa.so`, function
`0x146e8c..0x147068`. This is a local branch result, not an integrated `main`
or runtime-complete claim.

## Confirmed behavior and data

- The function loads `Globals::gameText`, `Globals::status`,
  `Globals::achievements`, and `Globals::Canvas` via GOT entries `0x2102a0`,
  `0x21015c`, `0x210214`, and `0x2101f4`. The previous file-local null text
  pointer was not the native source.
- The message is six newlines (`.rodata 0x1e5f74`), followed by the result
  of `Status::replaceHash(GameText[medal + 0x610], String(getValue(medal,
  count)))`, followed by two newlines (`0x1cb162`). The original calls
  `String(int)` at `0x146f06`; the previous manual decimal builder and missing
  newlines did not reproduce this behavior. The title is
  `GameText[0x161]`, and the medal label is `GameText[medal + 0x5e3]`.
- The original indexes 32-bit resource-ID tables at `0x1fb888` (high),
  `0x1fb898` (low), and `0x202784` (45 medal backgrounds), reading their low
  16 bits for `Image2DCreate`. All 4 + 4 + 45 values in the source were
  checked word-for-word against these addresses. They are numeric IDs, not
  copied images or asset payloads.
- The original passes `ChoiceWindow+0x34` and `+0x38` directly as output
  references to `Image2DCreate`. Both fields are now `unsigned int`, preserving
  their 32-bit ARM layout and the `-1` sentinel bit pattern. The second image
  call reloads `Globals::Canvas`, as in the original.
- `ModStation::checkMedals` supplies the two observed call sites. The second
  argument is taken from the achievement-stage slot; the native branch still
  distinguishes values below/above `0x24`. The meaning of out-of-range stage
  values is not established, so this pass does not claim them safe.

## Verification

| Check | Before | After |
| --- | ---: | ---: |
| `setMedal` strict fuzzy | 25.1% | 96.3% |
| `setMedal` source-shape | 83.9% | 99.2% |
| `setMedal` generated/original instructions | 183/176 | 177/176 |
| Full ARM comparison rows | 4,675 | 4,675 |
| Unique originals compared | 4,322/4,524 | 4,322/4,524 |
| Full mean strict fuzzy | 77.20% | 77.21% |
| Full mean source-shape | 90.07% | 90.07% |
| Linked-exact / raw-byte-exact rows | 2,162 / 977 | 2,162 / 977 |

Focused verifier reports are
`_work/blueprint-isunlocked/choice_medal_before.json` and
`choice_medal_branch_reload.json`; the complete report is
`choice_medal_full.json`. The full `--fail-on-wrong-type` run has zero skipped
and zero wrong-signature functions, with 202 absent originals. Comparing
exact `(unit, symbol)` sets finds zero linked-exact or byte-exact losses.
All 206 matching ARM translation units compile, and the UCRT64 `gof2` native
library builds. `setMedal` itself is not linked- or raw-byte-exact.

## Remaining uncertainty and next priority

This is an instruction-supported source reconstruction, not a runtime test of
the complete game. `Achievements::getValue` still uses a zero-filled 135-word
table in the source, although its original ARM body at `0x157018` indexes a
135-word table beginning at `.rodata 0x203134`. Recovering that table is the
next independent behavior package; until then, the medal value shown here can
still be wrong. `ChoiceWindow::draw` separately retains unresolved color,
credit, font, layout and button-position bindings; this pass does not change
or validate those paths.
