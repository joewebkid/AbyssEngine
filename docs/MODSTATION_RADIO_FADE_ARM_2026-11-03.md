# Station radio fade denominator (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

In `ModStation::OnRender2D`, the original branch at
`0xdbe28–0xdbe4e` handles timer values from `-6000` through `-1`.
It adds `6000` to the timer, converts to float, divides by the
literal at `0xdbf0c`, and adds `1.0`. The literal bytes
`00 80 bb c5` decode to `-6000.0f`. The recovered source had used
`+6000.0f`, giving alpha `1.5` at timer `-3000` instead of the
original `0.5`. The denominator is corrected; the surrounding
timer gates and drawing calls are unchanged.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 translation units. `OnRender2D`
source-shape changes from 89.1% to 89.2%, while strict fuzzy changes
from 20.6% to 12.9%. The latter decrease reflects a machine-code
shape difference and does not override the verified sign of the
original literal. Focused report:
`_work/modstation_radio_fade_focused_20261103.json`.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with mean source-shape 96.53% and strict fuzzy 86.40%.
There are 201 missing and zero skipped. Pair-by-pair comparison with
`_work/modstation_module_abi_full_20261103.json` finds no lost pairs,
no lost linked-exact matches (2,763 retained), and no lost byte-exact
matches (1,099 retained). Full report:
`_work/modstation_radio_fade_full_20261103.json`.

The radio scene has not been exercised in live gameplay. The next
priority is to inspect the remaining radio timing and alpha boundaries
against the ARM source, then validate the scene when a suitable
non-Redmi device is available.
