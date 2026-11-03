# Station main-screen rendering (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `ModStation::OnRender2D` spans `0xdb810–0xdbf28`.
Offsets, branch conditions, and call order below were checked in ARM
disassembly; the IDA export in `analysis/gof2_libgof2hdaa_full_ida.c`
helps reconstruct C++ String expressions but does not establish the
ABI on its own.

## Recovered path

After `Layout::drawFooterStation` at `0xdb978`, the original draws the
station header using its current `Station::getName` result. Stations
other than index `101` append a space and GameText key `136`.
When station byte `+0x61` is set, it draws style-2 box with the four
coordinates at `+0xa0..+0xac`, then the system information:

- The image ID at station `+0x4c` is drawn only if the system has an
  owner. Its X is layout `+0x28`; its Y is layout `+0x0c + +0x20 +
  2 * +0x2c` (`0xdbafc–0xdbb26`).
- The system name is followed by a space and GameText key `137`.
  If its measured width exceeds the remaining box width, the text
  falls back to the system name alone (`0xdbba0–0xdbbec`).
- The next row draws GameText key `133`, `": "`, and the station
  technology level (`0xdbc36–0xdbcba`). If layout byte `+0x286` is
  set and the system has an owner, a third row draws GameText key
  `406 + race` (`0xdbcd2–0xdbd4e`).
- The five buttons in the array at `+0x8c`, the launch button at
  `+0x90`, and the credits button at `+0x94` are drawn in that order.
  The news ticker at `+0x1c` is drawn unless station index is `101`
  or `108`, or system index is `25`; a null ticker also skips it
  (`0xdbd56–0xdbdae`). The layout reward message follows.

After the main-control block, the signed 64-bit timer at `+0x30`
clears station byte `+0xb0` when it exceeds `2999`. Otherwise the
screen draws GameText key `54` if that byte is set, or key `50` if
it is clear, at `layout +0x28 + station +0xa8` and layout `+0x18`
(`0xdbdbf–0xdbe12`).

Windows native and ARM64 static `gof2` builds pass; focused ARMv7
compiles 209/209 translation units. `OnRender2D` source-shape rises
from 35.9% to 89.4% (185 to 609 generated instructions against 667
original instructions). Strict fuzzy falls from 21.8% to 10.8%:
the recovered high-level behavior has substantially different
instruction order and temporary String lifetimes. There is no exact
match claim. Focused report:
`_work/modstation_main_focused_20261103.json`.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with mean source-shape 96.53% and strict fuzzy 86.40%.
There are 201 missing and zero skipped. Pair-by-pair comparison with
`_work/modstation_windows_full_20261103.json` finds no lost pairs,
no lost linked-exact matches (2,763 retained), and no lost byte-exact
matches (1,099 retained). Full report:
`_work/modstation_main_full_20261103.json`.

This path has not been exercised in live gameplay. The standalone
host still stores the ARM `+0xa0` box-X value in a provisional pointer
field, and no runtime test has confirmed the text layouts or image
assets. The next priority is to verify the station host lifecycle and
the remaining `OnRender2D` String/field shape without changing the
confirmed behavior to improve a metric. Redmi testing remains stopped.
