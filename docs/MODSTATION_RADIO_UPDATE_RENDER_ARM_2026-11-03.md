# Station radio update and render path (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Evidence is original ARM disassembly and ELF relocations, checked
against the rebuilt ARM code. Live scene behavior remains unverified.

## Recovered behavior and layout

`ModStation::OnUpdate()` at `0xd9c78–0xdaabc` uses the application
manager at station `+0x08` for elapsed time, then updates sound,
layout, playing time, the credits button at `+0x94`, and achievements.
The playing-time guard is the window flag at station `+0x62`, not a
byte of the elapsed-time accumulator. The `Globals::enterSpaceLounge`
relocation at GOT `0x2103f8` replaces a null holder; its branch sets
the alarm byte at `+0xd8`, uses sound parameter `2.0`, stops event
`0x7a`, and plays event `0x6c`.

The radio branch at `0xda144–0xda374` advances the scene timer at
station `+0xb4`. On crossing `-6000`, it replaces the cut scene with
mode `2` and sets rotation speed `0.00003f` (literal `0xda2d8`).
It updates the cut scene with the frame delta and reads
`Radio::lastMessageShown()` from the object at `+0x54`. When the
last message has shown, it adjusts the Y component of the vector at
`+0x104` with displacement `-0.03` per millisecond, uses the hold timer at
`+0xb8`, and repositions the `ScrollTouchBox` at `+0x98` using the
height of image `+0x5c` and half the screen height. At timer
`136001` (literal `0xda664`), it calls
`Status::nextCampaignMission(true)`, clears
`Globals::switch_to_target_setting`, selects module `5`, and clears
the radio flag at `+0x60`. The prior source treated the `SpaceLounge*`
at `+0x74` as the timer and made this transition unconditionally.

`ModStation::OnRender2D()` at `0xdb810–0xdbf28` calls
`Radio::draw(timer,nullptr,nullptr)` in the radio branch. After the
last message it draws image `+0x5c` with placement flags `20,68`
and the centered scroll box. The screen fade uses the original
timer ranges and `FillRectangle(0,0,Globals::w,Globals::h)`; the cut
scene renders for timer values greater than `-6000`. The canvas and
application-manager fields at `+0x04` and `+0x08` are typed and
asserted on ARM32. The subsequent module-field audit confirmed that
`RegisterApplicationModule` calls `SetApplicationManager`, which
fills both fields; `OnUpdate` and `OnRender2D` now read them directly.
The later fade-denominator audit corrected the `-6000..-1` interval
to divide by the ARM literal `-6000.0f`.
The later scroll audit corrected the post-hold direction to use the
same negative ARM displacement as the approach to the midpoint.

## Verification

- Windows native and ARM64 static `gof2` builds pass; ARMv7 compiles
  209/209 translation units with zero errors.
- Focused `OnUpdate` source-shape `66.8% -> 76.4%`, strict fuzzy
  `28.3% -> 26.7%`; generated instructions `643 -> 824`.
- Focused `OnRender2D` source-shape `16.5% -> 35.8%`, strict fuzzy
  `8.7% -> 20.3%`; generated instructions `68 -> 181`.
- Full ARM corpus: 4,323/4,524 originals in 4,769 pairs, mean
  source-shape 96.52%, strict fuzzy 86.40%, 2,763 linked-exact,
  1,099 byte-exact, 201 missing, zero skipped. Compared with the
  previous full report, there are no lost pairs or exact matches.

Reports: `_work/radio_runtime_full_20261103.json`,
`_work/modstation_radio_exit_focused_20261103.json`, and
`_work/modstation_radio_exit_full_20261103.json`.

## Limits and next priority

The campaign step-43 branch initializes timer `+0xb4` to `-12000`,
as confirmed later by the ARM literal at `0xd7d64`. An earlier
reading had wrongly assigned the separate reward threshold
`1,000,001` to this field. The `136001` exit threshold is reached
only after the timer advances. Other station update and render routes
still contain provisional shims, and the full radio scene has not
been exercised. Next validate the radio sequence at runtime on a
non-Redmi device when available.
