# BuildResourceList missing-slot recovery (2026-10-19)

Primary evidence is Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The function begins at `0xe8f54`. Thumb disassembly shows the original
passing 2,488 pointers to `PaintCanvas::SetResourceList` at `0x10a1b4`–
`0x10a1c2`. The recovered source previously assigned only 2,434 of those
array positions, leaving 54 uninitialized. This is an incomplete table,
not evidence that the original intentionally registered null slots.

The following ten missing records are restored from their ARM stack-slot
stores, IDs, constructor arguments and PC-relative strings. The store
offset for the shown low slots is `sp + 788 + slot*4`; the last record uses
`r8 + 9,948` before the `SetResourceList` call.

| Slot | ARM store | ID | Payload |
| ---: | --- | ---: | --- |
| 522 | `0xf0084` | 33410 | Texture `skybox_asteroid_belt_diffuse.aei`, value 0 |
| 525 | `0xf012c` | 33240 | Texture `container_003_terran_diffuse.aei`, value 0 |
| 538 | `0xf03e0` | 33301 | Texture `explosion_emp.aei`, value 0 |
| 540 | `0xf0444` | 33340 | Texture `stations_pirates_dmg_diffuse.aei`, value 0 |
| 571 | `0xf0aa8` | 33302 | Texture `bomb_rocket_fx.aei`, value 0 |
| 577 | `0xf0bf8` | 33311 | Texture `bomb_emp_a_normal_specular.aei`, value 0 |
| 582 | `0xf0d04` | 33330 | Texture `explosion_void_station.aei`, value 0 |
| 589 | `0xf0e6a` | 33303 | Material `(33302, BlendMode_2)` |
| 592 | `0xf0f12` | 33312 | Material `(33310, 33311, BlendMode 28)` |
| 2487 | `0x10a1bc` | 14287 | Mesh `explosion_void_station_alpha_lookat.aem`, material 27302, clone flag false |

The eight recovered file paths exist in the extracted Android asset set.
The asteroid-belt texture is an AEI type-3 uncompressed image, decoded as
format 3; the earlier four skybox files in the host gate decode as format
11. The asset-backed gate now checks record 33410's path and value, loads
the original file, and checks cached reuse at `-O0/-O2`.
Its SHA-256 is `94aeb7e8e1f50f3d3c13374f4cece5f2fb50974aac5a6272cd99b46be784a036`.
The last mesh also passes the in-process resource-ID `MeshCreate` handoff
through production `MeshCreateFromFile`/`MeshReadData`; its animation
collection is observed after the file closes. Its SHA-256 is
`323ba7f1228a5df7df582d8014adccb1a16f4e345675192b5114931c06ca0ace`.
Animated-mesh clone-on-reuse is not exercised by this gate.

After the first ten-record recovery, the table still had **44 unrecovered positions**: 608, 609, 611, 629,
632, 634, 637–640, 643, 645–648, 650–653, 655–658, 660–663, 665,
667, 669, 673, 681, 896, 897, 899, 900, 977, 997, 998, 1700,
1871, 1874, 1877 and 1883. The C++ stack array is now zero-initialized,
so these holes contain null pointers instead of indeterminate host stack
bytes. This is a **host-safety workaround**, not a claim that Android ARM
zeroed the array or that the missing resources were optional. Complete
`BuildResourceList` execution and all resource consumers are **not
validated**. The next priority is the concentrated 608–681 gap cluster,
then an ARM-backed native `SetResourceList` handoff and a complete-table
host gate. Its current C++ body forwards to the unresolved
`paintcanvas_ext_set_reslist`; a successful static-library build does
not prove executable linkage.

Validation at this revision: native `gof2` and all 208 ARM translation
units build; the asset-backed host gate passes at `-O0/-O2`; 40 verifier
unit tests pass (one existing skip). The full ARM corpus compares
4,323/4,524 original functions, with 2,529 linked-exact and 1,076
byte-exact matches. Both exact-match sets are unchanged from the prior
full report. Corpus mean source-shape/strict fuzzy remains 93.87%/83.06%.
Focused `BuildResourceList` source-shape moves from 73.5% to 73.8%; its
strict fuzzy moves from 22.8% to 21.7%, so this is a semantic/table
coverage gain, not an across-the-board machine-code similarity gain.

## Follow-up: material cluster 608–629

The same Android ARM function confirms four more missing material records:

| Slot | ARM store | ID | Constructor payload |
| ---: | --- | ---: | --- |
| 608 | `0xf122c` | 34700 | texture IDs 33120, 33121; blend 28 |
| 609 | `0xf126a` | 34701 | texture ID 34500; blend 2 |
| 611 | `0xf12ca` | 34703 | texture ID 33120; blend 0 |
| 629 | `0xf1678` | 34601 | texture ID 34501; blend 2 |

Slot 610 already existed, but incorrectly used texture ID 0. The ARM
constructor at `0xf127c`–`0xf1280` passes 33120 with blend 8; source now
matches that pair. Slot 609's 34500 value is traced from the ARM
`r6 - 1` value used for resource 293, and the original slot 293 record
also has ID 34500. These are table/constructor facts, not proof that a
complete resource-list consumer can resolve all referenced textures;
IDs 33120 and 33121 do not yet have direct records in the recovered C++
table. The remaining unrecovered slot count is 40. A complete-table host
gate remains necessary.

The follow-up builds natively and compiles all 208 ARM units. The
asset-backed skybox/mesh gate passes at `-O0/-O2`; 40 verifier tests pass
(one existing skip). The full corpus still compares 4,323/4,524 original
functions and retains all 2,529 linked-exact and 1,076 byte-exact pairs
without losses. Mean source-shape/strict fuzzy is 93.87%/83.05%, versus
93.87%/83.06% before this follow-up. Focused `BuildResourceList`
source-shape is 73.9%, versus 73.8%; strict fuzzy falls from 21.7% to
13.6%. This is a recorded strict-fuzzy metric regression despite the
ARM-backed table corrections; exact matching of this giant function
remains open.
