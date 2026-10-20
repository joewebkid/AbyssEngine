# PlayerEgo HUD atlases and equipment-gated sounds — 2026-10-20

The continuous Pan gate now reaches five `PlayerEgo` HUD images after the
50 `MovingStars` billboards and Ship-derived constructor fields. It uses
production `PaintCanvas::Image2DCreate`, `ImageCreateRegionFromFile`, and
the AEI/GLES texture upload path, with original asset files. The extracted
test copy of the constructor stops immediately before `steerLimit`; full
`PlayerEgo` construction, `Level::createPlayer` return, and `setShip` are
not established.

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `Ship::getFirstEquipmentOfSort(19)` at `0x99dda`, then registers sound
IDs **1, 3, 2** through the `Globals::addSoundResourceToList` GOT slot
`0x210244` when present. Sort **9** registers **25** at `0x99e08–0x99e14`;
sort **10** registers **23** at `0x99e22–0x99e2e`. These three branches were
missing from the recovered C++ constructor and are now restored. The same
original body calls `Image2DCreate` with resource IDs **1216, 1230, 8030,
8029, 1352** at `0x99e74–0x99eb6`, writing its five fields at offsets
`+0x23c`, `+0x240`, `+0x244`, `+0x248`, `+0x238`.

The two original atlases are `gof2_interface.aei` (1024², SHA-256
`1544b7c6652be504f27edf64fcece6a53533f38bdbf5991a35c16e3a3c173fa6`)
and `gof2_interface3.aei` (512², SHA-256
`b80781799324d14b4a3b555de56f5238f6dae852f4104c9908bc66c449461a33`).
The gate checks the five decoded atlas rectangles against their original
AEI region tables, the managed texture indices **0/0/7/7/0**, and the
second atlas's full GPU pixel readback. It also checks cached repeat calls,
all **32/32** original-file opens/closes, and the previous star/Ship values.

The gate passes on Windows SwiftShader and an Android ARMv7 device at
`-O0/-O2`. Three fixture-only `Item` objects carry sorts 19, 9 and 10
with a non-weapon type, so the production equipment lookup and sound-list
methods run without changing the three gun-slot counts. The fixture's
`Item` constructor and Ship value-refresh bypass are not native-behavior
claims; catalog-populated equipment and the other constructor branches
remain unverified.

Focused ARM constructor verification rises from **78.5% to 82.9%**
source-shape and **47.6% to 50.8%** strict fuzzy, with no exact match for
either constructor alias. Native `gof2`, **208/208** ARM translation units,
and **91** verifier tests pass (one existing skip). The complete ARM corpus
compares **4,323/4,524** originals in **4,735** pairs at **93.95%** mean
source-shape and **83.24%** strict fuzzy, with **2,551 linked-exact** and
**1,079 byte-exact** pairs, **201** absent originals and zero skipped units.
The `(unit, symbol)` exact sets are identical to the pre-change report;
only the two constructor aliases changed score. The full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/player-ego-hud-sounds-full.json`
(SHA-256 `9e586874cff03232929b651987d04e234f21e915a0f19b70ab0469823de75572`).

Next: finish the remaining `PlayerEgo` constructor fields and the guarded
supernova effect with source-backed item/Ship state, then let
`Level::createPlayer` proceed into `setShip` in the continuous gate.
