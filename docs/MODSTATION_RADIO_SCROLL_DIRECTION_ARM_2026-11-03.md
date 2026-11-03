# Station radio scroll direction (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The radio update branch in `ModStation::OnUpdate` reads the Y anchor
at station `+0x108` and compares it with half the screen height.
At `0xda1fa–0xda224`, it multiplies elapsed milliseconds by the
float literal at `0xda2e0`, adds that value to the anchor, and starts
a 4,000 ms hold when the result reaches or crosses the midpoint.
The literal bytes `8f c2 f5 bc` decode to approximately `-0.03f`.
When the hold timer at `+0xb8` is nonpositive, the path at
`0xda2e4–0xda306` applies the **same negative** displacement before
repositioning the scroll box.

The reconstructed code already used an upward displacement before
the midpoint, but its later scroll-box branch added `+0.03 * dt`.
That sent the radio text downward after the pause. Both expressions
now use the confirmed negative constant. The midpoint comparison,
hold timer, image-height query, and scroll-box placement remain as
in the ARM-backed reconstruction.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 translation units. `OnUpdate`
source-shape changes from 76.4% to 76.3%; strict fuzzy remains
27.1%. Focused report:
`_work/modstation_radio_scroll_focused_20261103.json`.

Live radio scrolling remains unverified. The same audit found that
campaign step 43 must start timer `+0xb4` at `-12000`, not
`1,000,001`; the original `0xd7b44–0xd7b4a` loads the literal at
`0xd7d64` and stores timer/hold words together. The corrected timer
no longer begins above the `136001` exit threshold. Focused
`OnTouchEnd(int,int,void*)` source-shape rises 83.1% to 83.2% and
strict fuzzy rises 22.8% to 23.0%; report:
`_work/modstation_radio_timer_focused_20261103.json`.
The full ARM corpus after both corrections compiles 209/209 units and
compares 4,323/4,524 original symbols in 4,769 pairs, with 201 missing
and zero skipped. Mean source-shape is 96.53%, strict fuzzy 86.40%;
2,763 pairs are linked-exact and 1,099 byte-exact. Compared with the
preceding fade checkpoint, no pair or exact-match set was lost.
Full report: `_work/modstation_radio_timing_full_20261103.json`.
The next priority is runtime validation of the radio sequence on a
suitable non-Redmi device. Redmi testing remains stopped.
