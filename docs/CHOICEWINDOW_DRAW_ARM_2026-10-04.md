# ChoiceWindow::draw Android ARM recovery (2026-10-04)

Scope: `ChoiceWindow::draw()` in the local `codex/blueprint-isunlocked-arm-20261002`
branch. The primary reference is Android 2.0.16 `libgof2hdaa.so`,
`0x1471bc..0x1474cc`. This is not an integrated game-runtime result.

## Confirmed source and data

- The original loads `Globals::layout`, `Canvas`, `status`, `achievements`,
  and `font` through GOT entries `0x210224`, `0x2101f4`, `0x21015c`,
  `0x210214`, and `0x21029c`. The old local layout and font pointers were
  permanently null. The old canvas, status and achievement bindings did not
  follow these original global routes.
- At `0x147252`, the ARM body compares `ChoiceWindow+0x30` (`medal`) with
  `0x24`, then indexes the selected color table with `ChoiceWindow+0x2c`
  (`count`). The source had compared `count` instead. The four-word color
  tables now match original `.rodata` at `0x1fb8b8` (low medals) and
  `0x1fb8a8` (high medals). The four credit values match `.rodata 0x1fb848`:
  `0, 5000, 2500, 1000`.
- The original constructs `String("+ ")` from `.rodata 0x1cb06f` and adds
  `Layout::formatCredits(value)` before measuring and drawing the reward.
  The previous source omitted this prefix and used a zero-filled credit table.
- Both image calls target the five-argument
  `PaintCanvas::DrawImage2D(unsigned, int, int, unsigned char, unsigned char)`
  overload with `0x11, 0x44`; the old source called the four-argument
  overload. The box call targets the six-argument `Layout::drawBox` overload,
  not the seven-argument one.
- Original GOT entries `0x210404`, `0x210408` and `0x20fd3c` bind
  `Globals::other_buttons_x`, `other_buttons_y` and
  `is_choice_window_visible`. Right-button coordinates go to index 2 and
  left-button coordinates to index 3; a lone left button also fills index 2.
  The old casts wrote through null placeholders. Reading `getPosition().x/y`
  directly also removes unnecessary `Vector` assignment and index-call
  sequences absent from the ARM body.

The rebuilt ARM object references the same six-argument `drawBox` and
five-argument `DrawImage2D` mangled overloads. Its three 16-byte tables
contain the expected original words. No image asset bytes are copied into
this source change.

## Verification

| Check | Before | After |
| --- | ---: | ---: |
| `draw` source-shape | 79.3% | 95.8% |
| `draw` strict fuzzy | 32.4% | 74.4% |
| `draw` generated/original instructions | 237/275 | 265/275 |
| Full compared originals | 4,322/4,524 | 4,322/4,524 |
| Full comparison rows | 4,675 | 4,675 |
| Full mean source-shape | 90.09% | 90.09% |
| Full mean strict fuzzy | 77.22% | 77.23% |
| Linked-exact / byte-exact rows | 2,164 / 977 | 2,164 / 977 |

The focused ARM run compiled 206/206 translation units and the UCRT64 native
`gof2` library built. The complete `--fail-on-wrong-type` run reports 202
absent originals, zero wrong-signature, and zero skipped. Comparing exact
`(unit, symbol)` sets with the previous complete report shows zero losses
and zero gains of either exact kind. `draw` itself is not exact. Reports are
`_work/blueprint-isunlocked/choice_draw_{before,after,full}.json`; the prior
full baseline is `status_getagents_full.json` in that ignored directory.

## Limits and next priority

No end-to-end medal-window runtime test was performed. The original tables
are indexed by `count` without an observed range guard; this pass preserves
that behavior and does not claim malformed counts safe. The next related
behavior gap is `ModStation::checkMedals` / `showMedals`, where local reward
pointers are still null and credit application needs a separate ARM-backed
recovery. Other `ChoiceWindow` paths remain outside this focused pass.
