# CutScene turret geometry (2026-10-17)

Android ARM `CutScene::checkForTurret()` at `0x98a68..0x98e0a`
uses the main `AEGeometry+0x0c` transform for the detach at
`0x98a8e..0x98a96`, three optional component attachments at
`0x98c64..0x98cc8`, and three final turret/ship attachments at
`0x98d88..0x98dae`. The previous C++ used the distinct
`childTransform` field at `+0x14` for all seven calls.

The same ARM path reads the first weapon-position vector from slot
**2** of the outer array (`0x98d24..0x98d3c`) and sets both turret
geometries to it. It then translates only the *second* geometry
(`0x98d40..0x98d4e`). The literal pool at `0x98e48..0x98e74`
supplies the following `(x,y,z)` translations:

| Item index | Translation |
| --- | --- |
| 47, 49 | `(0, 60, 0)` |
| 48 | `(0, 55, 0)` |
| 180 | `(0, 84, 0)` |
| 181 | `(0, 88, -11)` |
| 182 | `(34, 88, 1.6)` |
| 198 | `(0, 80, 0)` |
| 199 | `(0, 38, 0)` |
| 200 | `(0, 75, 0)` |
| 224 | `(0, 82, 0)` |
| other | `(0, 0, 0)` |

At `0x98d52..0x98d84`, both geometries rotate by
`(0, 3.1415927f, 0)` only when the item index is **outside**
`198..200`; the prior C++ had the opposite condition and zero-valued
rotation placeholders. The reconstructed code now follows the
observed branch, literal values and object selection. The extracted
host `-O0/-O2` tests check all listed item indices, the third
position slot, rotation condition and seven transform-ID calls.

Focused ARM `checkForTurret` source-shape/strict fuzzy rises from
**89.6%/45.8%** to **93.8%/48.7%**, still nonexact. Native `gof2`
and **208/208** ARM units pass. The host tests exercise extracted
source fragments, not the entire live ARM32/GPU path; file-loading
ownership and runtime rendering remain unverified. Full-corpus
results are recorded in [PROJECT_STATUS.md](PROJECT_STATUS.md) and
[VALIDATION.md](VALIDATION.md).
