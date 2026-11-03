# Station window draw routing (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass compares the `ModStation::OnRender2D` body at
`0xdb810–0xdbf28` with the rebuilt ARM object. Runtime window behavior
has not been exercised.

The original branch sequence at `0xdb83e–0xdb894` reads window flags
`+0x66`, `+0x65`, `+0x64`, `+0x67`, `+0x68`, then `+0x62`. It draws
`HangarWindow` at `+0x78`, `SpaceLounge` at `+0x74`, `MissionsWindow`
at `+0x80`, `StarMap` at `+0x10`, `StatusWindow` at `+0x7c`, or
`MenuTouchWindow` at `+0x50`. Those calls now use the actual child
objects rather than unresolved helper declarations.

At `0xdb894–0xdb8ba`, medal flag `+0x6a` selects `ChoiceWindow` at
`+0x88` before the primary flag `+0x63` selects `ChoiceWindow` at
`+0x70`. Dialogue flag `+0x69` then draws `DialogueWindow` at `+0x84`.
After the 2D pass, the original calls `HangarWindow::render3D` and
`SpaceLounge::draw3DShip` from their corresponding child pointers,
subject to the layout help-window flag. These direct calls are now
restored. The station main-screen path starts with
`Layout::drawFooterStation` at `0xdb978`; the previous placeholder
called an unresolved credits helper. The rest of this main-screen path
still needs recovery.

The ARM32 header assertions now cover every child pointer used by
these routes, including the two `ChoiceWindow` instances. Windows
native and ARM64 static `gof2` builds pass. Focused ARMv7 verification
compiles 209/209 translation units; `OnRender2D` source-shape changes
from 35.8% to 35.9% and strict fuzzy from 20.3% to 21.8%.
The full corpus compares 4,323/4,524 original functions in 4,769
pairs, with mean source-shape 96.52% and strict fuzzy 86.41%.
It retains all 2,763 linked-exact and 1,099 byte-exact pairs,
with 201 missing and zero skipped. A pair-by-pair comparison against
`_work/modstation_radio_exit_full_20261103.json` found zero lost
pairs and zero lost exact matches. Reports are
`_work/modstation_windows_focused_20261103.json` and
`_work/modstation_windows_full_20261103.json`.

The main-screen header, faction badge, system name, station controls,
and credit display remain provisional or absent in `OnRender2D`.
The next priority is that path, using the same ARM evidence. Redmi
testing remains stopped.
